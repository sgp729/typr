// typr Copyright (C) 2023 Georgiy Smykov <sgp@alram.in>

#include "config.h"
#include "utility.h"

#include <iostream>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <array>

namespace typr {

unsigned config::get_test_size() const {

        return test_size;
}

const colors& config::get_colors() const {

        return test_colors;
}

const std::vector<std::string>& config::get_words() const {

        return test_words.get_words();
}

typing_mode config::get_mode() const {

        return mode;
}

std::vector<std::string> config::load() {

        if (config_dir.empty()) {

                try {
                        config_dir = find_config_dir();
                } catch (...) { throw; }
        }

        if (!std::filesystem::exists(config_dir)) {
                throw std::runtime_error{"no ~/.config/typr directory"};
        }

        std::ifstream typrrc_stream{config_dir + "/typrrc"};
        if (!typrrc_stream.good()) {
                throw std::runtime_error{
                        "cat't read ~/.config/typr/typrrc"};
        }

        std::vector<std::string> commands;
        char* buffer = new char[512];

        while (typrrc_stream.getline(buffer, 512)) {
                
                std::string command = buffer;
                if (!command.empty()) {
                        commands.push_back(command);
                }
        }

        delete[] buffer;

        return commands;
}

void config::invoke_command(std::string command) {

        // split into tokens
        // resolve command
        // reflow to corresponding functions
        // or throw a bad command runtime_error

        if (command.empty()) {
                return;
        }

        std::size_t space_index{};
        std::vector<std::string> tokens;
        std::vector<std::string> file_commands;

        while ((space_index = command.find(' ')) != std::string::npos) {
                
                std::string token = command.substr(0, space_index);
                if (!token.empty()) {
                        tokens.push_back(token);
                }
                
                command.erase(0, space_index + sizeof(' '));
        }

        if (!command.empty()) {
                tokens.push_back(command);
        }

        std::string opcode = tokens.at(0);
        tokens.erase(tokens.begin());

        try {
        if (opcode == "q" || opcode == "quit") {
                throw 0;

        } else if (opcode == "ts" || opcode == "testsize") {
                change_testsize(tokens);

        } else if (opcode == "c" || opcode == "color") {
                change_color(tokens);

        } else if (opcode == "addw" || opcode == "addwords") {
                add_words(tokens);

        } else if (opcode == "rmw" || opcode == "removewords") {
                remove_words(tokens);

        } else if (opcode == "ldc" || opcode == "loadconfig") {
                file_commands = load();
                for (std::string& subcommand : file_commands) {
                        // prevent recursive load call
                        if (subcommand == "ldc") {
                                continue;
                        }
                        invoke_command(subcommand);
                }

        } else if (opcode == "ldw" || opcode == "loadwords") {
                load_words(tokens);

        } else if (opcode == "svw" || opcode == "savewords") {
                save_words(tokens);

        } else if (opcode == "nw" || opcode == "newwords") {
                new_words(tokens);

        } else if (opcode == "m" || opcode == "mode") {
                change_mode(tokens);

        } else {
                throw std::runtime_error{
                  std::string{"expected opcode, got "} + opcode };
        }

        } catch (...) { throw; }
}

void config::change_testsize(std::vector<std::string>& args) {

        if (args.size() != 1) {
                throw std::runtime_error{
                        std::string{"expected 1 argument, got "}
                        + std::to_string(args.size()) };
        }

        unsigned new_test_size{};
        try {
                new_test_size = std::stoul(args.at(0));
        
        } catch (...) {
        
                throw std::runtime_error{
                std::string{"expected integer (0 .. 4,294,967,295), got "}
                        + std::string{args.at(0)} };
        }

        test_size = new_test_size;
}

void config::change_color(std::vector<std::string>& args) {

        if (args.size() != 4) {
                throw std::runtime_error{
                        std::string{"expected 4 arguments, got "}
                        + std::to_string(args.size()) };
        }

        std::string color_group = args.at(0);
        args.erase(args.begin());

        std::array<unsigned, 3>  new_color;
        for (int i = 0; i < 3; ++i) {
                
                unsigned color_component;
                try {
                        color_component = std::stoul(args.at(i));
                } catch (...) {
                        throw std::runtime_error{
                       std::string{"no known conversion to integer from "}
                                + std::string{args.at(i)} };
                }

                if (color_component > 1000) {
                        throw std::runtime_error{
                         std::string{"expected integer (0 .. 1000), got "}
                                + std::string{args.at(i)} };
                }
        
                new_color.at(i) = color_component;
        }

        unsigned red{ new_color.at(0) };
        unsigned green{ new_color.at(1) };
        unsigned blue{ new_color.at(2) };

        if (color_group == "t" || color_group == "typed") {
                test_colors.typed = {red, green, blue};

        } else if (color_group == "e" || color_group == "error") {
                test_colors.error = {red, green, blue};

        } else if (color_group == "p" || color_group == "predicted") {
                test_colors.predicted = {red, green, blue};

        } else if (color_group == "i" || color_group == "interface") {
                test_colors.interface = {red, green, blue};

        } else {
                throw std::runtime_error{
                        std::string{"expected color group, got "}
                        + std::string{color_group} };
        }
}

void config::add_words(std::vector<std::string>& args) {

        test_words.add_words(args);
}

void config::remove_words(std::vector<std::string>& args) {

        test_words.remove_words(args);
}

void config::load_words(std::vector<std::string>& args) {

        if (args.size() > 1) {
                throw std::runtime_error{
                        std::string{"expected 0 .. 1 arguments, got "}
                        + std::to_string(args.size()) };
        }

        if (args.empty()) {
                if (test_words.get_words_filename().empty()) {
                        test_words.set_words_filename("words");
                }
        }
        else {
                test_words.set_words_filename(args.at(0));
        }

        try {
                test_words.load();
        } catch (...) { throw; }
}

void config::save_words(std::vector<std::string>& args) {

        if (args.size() > 1) {
                throw std::runtime_error{
                        std::string{"expected 0 .. 1 arguments, got "}
                        + std::to_string(args.size()) };

        }

        // if no filename in the argument and no already loaded file
        if (args.empty()) {
                if (test_words.get_words_filename().empty()) {
                        test_words.set_words_filename("words");
                }
        }
        else {
                test_words.set_words_filename(args.at(0));
        }

        try {
                test_words.save();
        } catch (...) { throw; }
}

void config::new_words(std::vector<std::string>& args) {

        if (args.size() > 1) {
                throw std::runtime_error{
                        std::string{"expected 0 .. 1 arguments, got "}
                        + std::to_string(args.size()) };
        }

        if (!args.empty()) {

                test_words.set_words_filename(args.at(0));
        }

        test_words.clear();
}

void config::change_mode(std::vector<std::string>& args) {

        if (args.size() != 1) {
                throw std::runtime_error{
                        std::string{"expected 1 argument, got "}
                        + std::to_string(args.size()) };
        }

        std::string& new_mode = args.at(0);

        if (new_mode == "r" || new_mode == "relaxed") {
                mode = typing_mode::relaxed;

        } else if (new_mode == "s" || new_mode == "strict") {
                mode = typing_mode::strict;

        } else if (new_mode == "f" || new_mode == "forgiving") {
                mode = typing_mode::forgiving;

        } else {
                throw std::runtime_error{
                        std::string{"expected typing mode, got "}
                        + std::string{args.at(0)} };
        }
}

} // namespace
