// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "test_controller.h"
#include <iostream>

int main() {

        // switch to alternate buffer
        std::cout << "\e[?1049h\e[H";

        typr::test_controller test;
        test.main_loop();

        // restore original buffer
        std::cout << "\e[?1049l";
        
        return 0;
}
