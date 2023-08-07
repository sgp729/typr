// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include "extra.h"
#include "words.h"

#include <vector>
#include <string>

namespace typr {

class config {
public:
        config() = default;
        ~config() = default;
        
        unsigned get_test_size() const;
        const colors& get_colors() const;
        const std::vector<std::string>& get_words() const;
        typing_mode get_mode() const;

        std::vector<std::string> load();

        void invoke_command(std::string);

private:
        unsigned test_size{25};
        colors test_colors;
        words test_words;
        typing_mode mode{typing_mode::relaxed};
        std::string config_dir;

        void change_testsize(std::vector<std::string>&);

        void change_color(std::vector<std::string>&);

        void add_words(std::vector<std::string>&);
        void remove_words(std::vector<std::string>&);
        void load_words(std::vector<std::string>&);
        void save_words(std::vector<std::string>&);
        void new_words(std::vector<std::string>&);
        void change_mode(std::vector<std::string>&);

};

}
