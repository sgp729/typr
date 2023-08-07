// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "words.h"
#include "utility.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <exception>
#include <algorithm>

namespace typr {

void words::load() {
        
        std::string config_dir;
        try {
                config_dir = find_config_dir();
        } catch (...) { throw; }

        std::ifstream words_stream{config_dir + '/' + words_filename};
        if (!words_stream.good()) {
                throw std::runtime_error{
                        std::string{"can't read ~/.config/typr/"}
                        + words_filename };
        }

        char* buffer = new char[8192];

        while (words_stream.getline(buffer, 8192)) {
                
                std::string word{buffer};
                if (!word.empty()) {
                        words_base.push_back(word);
                }
        }

        delete[] buffer;
}

void words::save() {

        std::string config_dir;
        try {
                config_dir = find_config_dir();
        } catch (...) { throw; }

        std::ofstream words_stream{config_dir + '/' + words_filename};
        if (!words_stream.good()) {
                throw std::runtime_error{
                        std::string{"can't write ~/.config/typr/"}
                        + words_filename};
        }

        for (std::string& word : words_base) {
                words_stream << word << '\n';
        }
}

void words::clear() {

        words_base.clear();
}

void words::add_words(std::vector<std::string>& args) {

        for (std::string& word : args) {
                if (std::find(words_base.begin(), words_base.end(), word)
                                == words_base.end()) {
                        words_base.push_back(word);
                }
        }
}

void words::remove_words(std::vector<std::string>& args) {

        auto erase_condition = [&args](std::string& rhs) -> bool {
                auto position = args.end();
                if ((position = std::find(args.begin(), args.end(), rhs))
                                != args.end()) {
                        args.erase(position);
                        return true;
                }
                else {
                        return false;
                }
        };

        std::erase_if(words_base, erase_condition);
}

const std::vector<std::string>& words::get_words() const {
        return words_base;
}

const std::string& words::get_words_filename() const {
        return words_filename;
}

void words::set_words_filename(std::string name) {
        words_filename = name;
}

} // namespace
