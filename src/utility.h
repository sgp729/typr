// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include <string>
#include <vector>

namespace typr {

std::string find_config_dir();

std::vector<unsigned short>
partial_wrap_words(const std::string&, unsigned);

}
