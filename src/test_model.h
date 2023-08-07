// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include <string>
#include <vector>
#include "config.h"
#include "statistics.h"

namespace typr {

class test_model {
public:
        test_model() = default;
        ~test_model() = default;

        config options;
        std::string test_text;
        statistics stats;

        std::vector<bool> typed;

};

}
