// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include <string>
#include <vector>

#include "extra.h"
#include "config.h"
#include "statistics.h"

namespace typr {

class base_view {
public:
        base_view() = default;
        virtual ~base_view() = default;

        /*
        * called when app requests a refresh on the screen
        * arguments are:
        *       - std::string& test_text
        *       - std::vector<bool>& typed (vector of typed characters,
        *              correct - true, incorrect - false)
        *       - config& opts (for details see ./config.h)
        */

        virtual void 
        refresh(std::string&, std::vector<bool>&, config&) = 0;

        /*
        * called when app waits for a character to be typed
        * return value: [int] typed character
        */

        virtual int wait_for_input() = 0;

        /*
        * called when app waits for command from command line
        * return value: [std::string] entered command
        */

        virtual std::string wait_for_command() = 0;

        /*
        * called to notify the user about an error
        * arguments are:
        *       - std::string error_message
        *       - config& opts (for details see ./config.h)
        */

        virtual void error(std::string, config&) = 0;

        /*
        * called when test is over and stats are collected
        * arguments are:
        *       - double test_duration (in seconds)
        *       - double words_per_minute
        *       - double letters_per_second
        *       - double accuracy (percentage)
        */

        virtual void 
        display_stats(double, double, double, double) = 0;
};

}
