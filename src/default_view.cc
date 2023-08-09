// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "default_view.h"
#include "utility.h"

#include <unistd.h>
#include <termios.h>
#include <functional>
#include <charconv>
#include <iostream>

#define COLOR_NONE -1
#define COLOR_TYPED 9
#define COLOR_ERROR 10
#define COLOR_PREDICT 11
#define COLOR_INTERFACE 12

#define TYPED_PAIR 1
#define ERROR_PAIR 2
#define ERROR_PAIR_INVERT 3
#define PREDICTED_PAIR 4
#define INTERFACE_PAIR 5

namespace typr {

default_view::default_view() {

        std::cout << "\e[?1049h\e[H";
        initscr();
        
        cbreak();
        noecho();

        if (has_colors()) {        
                start_color();
                use_default_colors();
        }

        curs_set(0);
        
        window_width = COLS;
        window_height = LINES;

        header = newwin(2, window_width, 0, 0);
        body = newwin(window_height - 5, window_width - 10, 3, 5);
        command_line = newwin(1, window_width, window_height - 1, 0);
        
        keypad(body, true);
        

        init_pair(TYPED_PAIR, COLOR_TYPED, COLOR_NONE);
        init_pair(ERROR_PAIR, COLOR_ERROR, COLOR_NONE);
        init_pair(ERROR_PAIR_INVERT, COLOR_TYPED, COLOR_ERROR);
        init_pair(PREDICTED_PAIR, COLOR_PREDICT, COLOR_NONE);
        init_pair(INTERFACE_PAIR, COLOR_INTERFACE, COLOR_NONE);

}

default_view::~default_view() {

        delwin(header);
        delwin(body);
        delwin(command_line);
        
        endwin();

        std::cout << "\e[?1049l";
}

void default_view::refresh(std::string& test_text,
                        std::vector<bool>& typed, config& options) {

        // refresh colors
        const colors& colors = options.get_colors();

        init_color(COLOR_TYPED, colors.typed.red,
                colors.typed.green, colors.typed.blue);

        init_color(COLOR_ERROR, colors.error.red,
                colors.error.green, colors.error.blue);

        init_color(COLOR_PREDICT, colors.predicted.red,
                colors.predicted.green, colors.predicted.blue);

        init_color(COLOR_INTERFACE, colors.interface.red,
                colors.interface.green, colors.interface.blue);

        // update LINES, COLS
        wrefresh(stdscr);

        size_t new_text_hash = std::hash<std::string>{}(test_text);
        
        // if window resized or text changed from last update
        if (new_text_hash != text_hash
           || window_width != COLS
           || window_height != LINES) [[unlikely]] {

                window_width = COLS;
                window_height = LINES;

                body_width = window_width - 10;
                body_height = window_height - 5;

                text_hash = new_text_hash;

                helper_lines_length = partial_wrap_words(
                                test_text, body_width);
       
                // needs to be cleared before calling delwin 
                werase(header);
                werase(body);
                werase(command_line);

                wrefresh(command_line);

                delwin(header);
                delwin(body);
                delwin(command_line);
        
                header = newwin(2, window_width, 0, 0);
                body = newwin(window_height - 5, window_width - 10, 3, 5);
                command_line = newwin(1, window_width,
                        window_height - 1, 0);

                keypad(body, true);
                
                wattron(header, COLOR_PAIR(INTERFACE_PAIR));

                mvwaddstr(header, 0, 3, "typr");
                mvwaddstr(header, 0, window_width - 39,
                        "press : to enter command line");

                wmove(header, 1, 0);
                for (int i = 0; i < window_width; ++i) {
                        waddch(header, '=');
                }

                mvwaddstr(command_line, 0, 1, ":_");

                wrefresh(header);
                wrefresh(command_line);
        }
        
        refresh_body(test_text, typed);
}

void default_view::refresh_body(std::string& test_text,
                std::vector<bool>& typed) {

        werase(body);

        unsigned focused_letter = typed.size();
        unsigned focused_line{};

        if (helper_lines_length.empty()) {

                return;
        }

        // getting line where latest typed letter positioned
        while (focused_letter > helper_lines_length.at(focused_line)) {

                focused_letter -= helper_lines_length.at(focused_line);
                ++focused_line;
        }
        
        // gap -> minimum distance between focused line and body bottom
        unsigned gap = body_height / 3;

        // if focused line lower than the no_scroll_limit
        // we need to scroll down line_offset lines
        unsigned line_offset{};
        unsigned no_scroll_limit = body_height - gap;
       
        if (focused_line > no_scroll_limit) {
                line_offset = focused_line - no_scroll_limit;
        }

        // get first letter to print
        unsigned letter{};
        for (int i = 0; i < line_offset; ++i) {
                letter += helper_lines_length.at(i);
        }

        unsigned size{};

        // size till end of the body or end of the text
        for (int i = line_offset,
             end = 
             (helper_lines_length.size() < line_offset + body_height
             ? helper_lines_length.size()
             : body_height + line_offset);
             i < end; ++i) {

                size += helper_lines_length.at(i);
        }

        size += letter;

        unsigned window_line{};
        unsigned window_cursor{};

        for (; letter < size; ++letter, ++window_cursor) {

                char text_letter{ test_text.at(letter) };

                if (window_cursor + 1 >
                    helper_lines_length.at(window_line + line_offset)) {
                        ++window_line;
                        window_cursor = 0;
                }

                if (typed.size() < letter + 1) {
                        wattron(body, COLOR_PAIR(PREDICTED_PAIR));
                }
                else if (!typed.at(letter)) {
                        // mark wrong spaces as underscores
                        if (text_letter == ' ') {
                                text_letter = '_';
                        }
                        wattron(body, COLOR_PAIR(ERROR_PAIR));
                }
                else {
                        wattron(body, COLOR_PAIR(TYPED_PAIR));
                }

                mvwaddch(body, window_line, window_cursor,
                         text_letter);
        }

        wrefresh(body);
}
int default_view::wait_for_input() {

        return wgetch(body);
}

std::string default_view::wait_for_command() {

        // enable interface color
        // erase underscore after colon
        wattron(command_line, COLOR_PAIR(INTERFACE_PAIR));
        werase(command_line);

        mvwaddch(command_line, 0, 1, ':');
        mvwaddstr(command_line, 0, body_width - 6, "test paused..");

        wrefresh(command_line);

        // show typed letters
        // show cursor
        echo();
        curs_set(1);

        // get command in buffer
        char* buffer = new char[128];
        mvwgetnstr(command_line, 0, 2, buffer, 128);
        
        std::string command{buffer};

        // clear typed letters

        wmove(command_line, 0, 0);
        for (int i = 0, end = window_width; i < end; ++i) {
                waddch(command_line, ' ');
        }

        // restore cmdline state
        mvwaddstr(command_line, 0, 1, ":_");
        wrefresh(command_line);

        // don't show typed letters
        // hide cursor
        noecho();
        curs_set(0);

        delete[] buffer;

        return command;
}

void default_view::error(std::string message, [[maybe_unused]] config& opts) {

        wattron(command_line, COLOR_PAIR(ERROR_PAIR_INVERT));
        
        wmove(command_line, 0, 0);
        for (int i = 0; i < window_width; ++i) {
                waddch(command_line, ' ');
        }

        mvwaddstr(command_line, 0, 2, message.c_str());
        wrefresh(command_line);
}

void default_view::display_stats(double duration_sec,
    double words_per_minute, double letter_per_second,
    double accuracy_percent) {

        werase(body);
        wattron(body, COLOR_PAIR(INTERFACE_PAIR));

        char buffer[8]{};

        std::to_chars(&buffer[0], &buffer[7],
                      duration_sec, std::chars_format::fixed, 2);

        mvwaddstr(body, 0, 0, "time: ");
        waddstr(body, buffer);
        waddch(body, 's');

        std::to_chars(&buffer[0], &buffer[7],
                      words_per_minute, std::chars_format::fixed, 2);

        mvwaddstr(body, 1, 0, "wpm: ");
        waddstr(body, buffer);

        std::to_chars(&buffer[0], &buffer[7],
                      letter_per_second, std::chars_format::fixed, 2);

        mvwaddstr(body, 2, 0, "lps: ");
        waddstr(body, buffer);

        std::to_chars(&buffer[0], &buffer[7],
                      accuracy_percent, std::chars_format::fixed, 2);
               
        mvwaddstr(body, 3, 0, "acc: ");
        waddstr(body, buffer);
        waddch(body, '%');

        mvwaddstr(body, 5, 0, "press any key");
        wrefresh(body);
        
        sleep(1);
        // ignore input received while sleeping
        tcflush(0, TCIFLUSH);
        wgetch(body);
}

}
