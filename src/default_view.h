// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include "base_view.h"

#include <curses.h>

namespace typr {

class default_view : public base_view {
public:
        default_view();
        ~default_view();

        void refresh(std::string&, 
                std::vector<bool>&, config&);

        char wait_for_input();

        std::string wait_for_command();

        void error(std::string, config&);

        void display_stats(double, double, double, double);

private:
        WINDOW* header{};
        WINDOW* body{};
        WINDOW* command_line{};
        
        unsigned window_width;
        unsigned window_height;

        unsigned body_width;
        unsigned body_height;

        std::vector<unsigned short> helper_lines_length;

        void refresh_body(std::string&, std::vector<bool>&);

        std::size_t text_hash{};
};

}
