// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>
#pragma once



namespace typr {

struct color {
        unsigned red;
        unsigned green;
        unsigned blue;

        auto operator<=>(const color&) const = default;
};

struct colors {
        color typed{ 949, 953, 957 };
        color error{ 956, 262, 274 };
        color predicted{ 649, 653, 657 };
        color interface{ 949, 953, 957 };
};

enum class typing_mode {
        strict,
        relaxed,
        forgiving
};

}
