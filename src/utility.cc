// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "utility.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <filesystem>

namespace typr {

std::string find_config_dir() {

        char* config_dir_cstr = std::getenv("HOME");
        std::string config_dir;

        if (config_dir_cstr)
                config_dir = config_dir_cstr;

        if (config_dir.empty()) {
                throw std::runtime_error{"no home environment "
                                        "variable - can't find ~/"};
        }

        config_dir += "/.config/typr";
        if (!std::filesystem::exists(config_dir)) {
                throw std::runtime_error{"no ~/.config/typr directory"};
        }

        return config_dir;
}

std::vector<unsigned short>
partial_wrap_words(const std::string& text, unsigned width) {

        std::vector<unsigned short> lines;

        unsigned line_length{};
        unsigned word_length{};

        for (auto it = text.begin(), end = text.end(); it != end; ++it) {

                char c = *it;

                // only if the previous word is 2+ times
                // longer than the width
                if (line_length >= width) {

                        line_length -= width;
                        lines.push_back(width);
                        
                }
                
                ++word_length;
                if (c == ' ' || it + 1 == end) [[unlikely]] {
                        
                        if (word_length > width) {

                                lines.push_back(width);
                                line_length = word_length - width;
                        }
                        else if (line_length + word_length > width) {
                                lines.push_back(line_length);
                                line_length = word_length;
                        }
                        else {
                                line_length += word_length;
                        }
                
                        word_length = 0;
                }
        }

        lines.push_back(line_length);
        return lines;
}

std::vector<std::string> split_string(std::string& str, char delimiter) {

        std::vector<std::string> chunks;

        std::size_t delim_index{std::string::npos};

        while ((delim_index = str.find(delimiter)) != std::string::npos) {
                
                std::string chunk = str.substr(0, 
                        delim_index);
                if (!chunk.empty()) {
                        chunks.emplace_back(chunk);
                }
                str.erase(0, delim_index + 1);
        }

        if (!str.empty()) {
                chunks.emplace_back(str);
        }

        return chunks;
}

}
