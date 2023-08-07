// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once

#include <chrono>
#include <ctime>
#include <vector>

namespace typr {

class statistics {
public:
        statistics() = default;
        ~statistics() = default;

        void reset();

        void start_timer();
        void stop_timer();

        bool is_waiting() const;

        std::chrono::milliseconds get_duration() const;
        double get_avg_typing_speed(unsigned) const;
        double get_avg_words_per_minute(unsigned) const;
        time_t get_test_start_point() const;
        double get_accuracy(std::vector<bool>&) const;

        void save(unsigned, double, double, double) const;

private:
        bool timer_status{};

        time_t test_start_point{};

        std::chrono::time_point<
            std::chrono::steady_clock
        > timer_start_point{};

        std::chrono::milliseconds test_duration{};

};

}
