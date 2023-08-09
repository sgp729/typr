// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "statistics.h"
#include "utility.h"

#include <exception>
#include <fstream>
#include <string>
#include <algorithm>

namespace typr {

void statistics::reset() {

        test_start_point = time(nullptr);
        test_duration = std::chrono::milliseconds{0};
}

void statistics::start_timer() {

        timer_start_point = std::chrono::steady_clock::now();
        timer_status = true;
}

void statistics::stop_timer() {

        test_duration +=
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - timer_start_point);
        timer_status = false;
}

bool statistics::is_waiting() const {

        return timer_status;
}

std::chrono::milliseconds statistics::get_duration() const {

        return test_duration;
}

double statistics::get_avg_typing_speed(unsigned typed_size) const {

        return (double)typed_size / test_duration.count() * 1000;
}

double statistics::get_avg_words_per_minute(unsigned test_size) const {

        return (double)test_size / test_duration.count() * 60'000;
}

time_t statistics::get_test_start_point() const {

        return test_start_point;
}

double statistics::get_accuracy(std::vector<bool>& typed) const {

        return std::count(typed.begin(), typed.end(), true)
                / (double)typed.size() * 100;
}

void
statistics::save(unsigned test_size, double wpm, double duration, double accuracy) const {

        std::string config_dir;
        try {
                config_dir = find_config_dir();
        } catch (...) { throw; }

        std::ofstream stat_stream{config_dir + "/stats", std::ios::app};
        if (!stat_stream.good()) {
                throw std::runtime_error{
                    std::string{"can't open "} + config_dir + "/stats" };
        }

        stat_stream << test_start_point << ' ' << test_size << ' '
                        << wpm << ' ' << duration << ' ' << accuracy << '\n';
}

double
statistics::get_best_for(unsigned test_size) const {

        std::string config_dir;
        try {
                config_dir = find_config_dir();
        } catch (...) { throw; }

        std::ifstream stat_stream{config_dir + "/stats"};
        if (!stat_stream.good()) {
                throw std::runtime_error{
                    std::string{"can't open "} + config_dir + "/stats" };
        }

        char entry_buffer[128];
        // iterate over stats entries... find best wpm

        double max_words_per_minute{};

        while (stat_stream.getline(entry_buffer, 128)) {
        
                std::string stats_entry{entry_buffer};
                if (stats_entry.empty()) {
                        continue;
                }
                
                std::vector<std::string> stats = split_string(stats_entry, ' ');

                // filter irrelevant entries (different test size)
                if (!(std::stoul(stats.at(1)) == test_size)) {
                        continue;
                }

                max_words_per_minute = std::max(max_words_per_minute,
                                                std::stod(stats.at(2)));
        }

        return max_words_per_minute;
}

}
