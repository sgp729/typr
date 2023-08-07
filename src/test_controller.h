// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include "test_model.h"
#include "default_view.h"

namespace typr {

class test_controller {
public:
        test_controller() = default;
        ~test_controller() = default;

        void main_loop();
        
        void new_test();

private:
        default_view view;
        test_model model;

        void init();

};

}
