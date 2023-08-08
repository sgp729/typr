// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "test_controller.h"

#include <thread>
#include <chrono>
#include <random>

namespace typr {

void test_controller::main_loop() {

        init();

while (true) {

        if (model.test_text.size() && 
            model.test_text.size() <= model.typed.size()) [[unlikely]] {
                
                model.stats.stop_timer();

                double accuracy = model.stats.get_accuracy(model.typed);

                double words_per_minute =
                        model.stats.get_avg_words_per_minute(
                        model.options.get_test_size() );

                double letter_per_sec =
                        model.stats.get_avg_typing_speed(
                        model.typed.size() );

                double duration_sec =
                        model.stats.get_duration().count() / 1000.l;

                model.stats.save(model.options.get_test_size(),
                        words_per_minute, letter_per_sec, accuracy);

                try {
                        view.display_stats(duration_sec,
                          words_per_minute, letter_per_sec,
                          accuracy);

                } catch (std::exception& exc) {
                        view.error(exc.what(), model.options);
                }
                
                new_test();
        }

        char input = view.wait_for_input();

        switch (input) {

        case ' ': [[fallthrough]];
        case '!' ... '9': [[fallthrough]];
        case ';' ... '~': [[likely]]; // most of the chars

                if (!model.options.get_test_size()
                    || !model.test_text.size()) {
                        continue;
                }
                
                if (!model.stats.is_waiting()) {
                        model.stats.start_timer();
                }

                if (model.options.get_mode() == typing_mode::forgiving
                   || input == model.test_text.at(model.typed.size()) ) {
                        model.typed.push_back(true);
                }
                else {
                        if (model.options.get_mode()
                            == typing_mode::strict) {
                                new_test();
                        }
                        else {
                                model.typed.push_back(false);
                        }
                }
                view.refresh(model.test_text,
                        model.typed, model.options);
                break;

        case 7: // backspace
                if (model.typed.size()) {
                        model.typed.pop_back();
                }
                view.refresh(model.test_text,
                        model.typed, model.options);
                break;

        case ':':
                model.stats.stop_timer();
                view.refresh(model.test_text,
                        model.typed, model.options);
                try {

                        std::string command = view.wait_for_command();

                        if (command == "r" || command == "reset") {
                                new_test();
                                break;
                        }

                        model.options.invoke_command(command);
                }
                catch (int) { // catch exit code
                        return;
                }
                catch (std::exception& exc) {
                        view.error(exc.what(), model.options);
                }
                
                break;

        default: // ignore other characters
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(24));
}

} // main_loop()

void test_controller::new_test() {

        model.stats.reset();

        model.typed.clear();
        model.test_text.clear();

        if (!model.options.get_words().size()) {
                
                return;
        }

        std::mt19937 rng{std::random_device{}()};

        unsigned previous_index = model.options.get_words().size();

        for (int word_count = 0, end = model.options.get_test_size();
                        word_count < end; ++word_count) {

                unsigned word_index = rng() % model.options.get_words().size();
                
                // if we picked the same word, change word, cap to size
                if (word_index == previous_index) {
                        ++word_index %= model.options.get_words().size();
                }

                model.test_text += model.options.get_words().at(word_index);
                model.test_text += ' ';
        }

        if (!model.test_text.empty()) {
                model.test_text.pop_back();
        }

        view.refresh(model.test_text, model.typed, model.options);
}

void test_controller::init() {

        // dummy refresh, set up colors/size etc...
        view.refresh(model.test_text, model.typed, model.options);

        std::vector<std::string> config_entries;
        
        // try load config file
        try {
                config_entries = model.options.load();
        } catch (std::exception& exc) {
                view.error(exc.what(), model.options);
        }

        // run config commands
        for (std::string& entry : config_entries) {
                try {
                        model.options.invoke_command(entry);
                } catch (std::exception& exc) {
                        view.error(exc.what(), model.options);
                }
        }

        new_test();

        view.refresh(model.test_text, model.typed, model.options);
}


}
