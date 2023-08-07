// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include <string>
#include <vector>

namespace typr {

class words {
public:
        words() = default;
        ~words() = default;

        void load();
        void save();

        void clear();

        void add_words(std::vector<std::string>&);
        void remove_words(std::vector<std::string>&);

        const std::vector<std::string>& get_words() const;

        const std::string& get_words_filename() const;
        void set_words_filename(std::string);

private:
        std::string words_filename;
        std::vector<std::string> words_base;

};

}
