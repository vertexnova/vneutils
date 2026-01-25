/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2025
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "vertexnova/utils/command_line_parser.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vne {
namespace utils {

// Static member initialization
const std::string CommandLineOption::kEmptyString = "";

//==============================================================================
// CommandLineOption Implementation
//==============================================================================

CommandLineOption::CommandLineOption(const std::string& name,
                                     const std::string& description,
                                     const std::string& value_name,
                                     const std::string& default_value)
    : names_({name})
    , description_(description)
    , value_name_(value_name)
    , default_value_(default_value) {
}

CommandLineOption::CommandLineOption(const std::vector<std::string>& names,
                                     const std::string& description,
                                     const std::string& value_name,
                                     const std::string& default_value)
    : names_(names)
    , description_(description)
    , value_name_(value_name)
    , default_value_(default_value) {
}

const std::string& CommandLineOption::getName() const {
    return names_.empty() ? kEmptyString : names_[0];
}

const std::vector<std::string>& CommandLineOption::getNames() const {
    return names_;
}

const std::string& CommandLineOption::getDescription() const {
    return description_;
}

const std::string& CommandLineOption::getValueName() const {
    return value_name_;
}

const std::string& CommandLineOption::getDefaultValue() const {
    return default_value_;
}

bool CommandLineOption::takesValue() const {
    return !value_name_.empty();
}

bool CommandLineOption::hasDefaultValue() const {
    return !default_value_.empty();
}

//==============================================================================
// CommandLineParser Implementation
//==============================================================================

CommandLineParser::CommandLineParser(const std::string& application_name,
                                     const std::string& application_version)
    : application_name_(application_name)
    , application_version_(application_version) {
    // Add default help and version options
    addOption(CommandLineOption(std::vector<std::string>{"--help", "-h"}, "Show this help message"));
    addOption(CommandLineOption(std::vector<std::string>{"--version", "-v"}, "Show version information"));
}

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::setApplicationName(const std::string& name) {
    application_name_ = name;
}

void CommandLineParser::setApplicationVersion(const std::string& version) {
    application_version_ = version;
}

void CommandLineParser::addOption(const CommandLineOption& option) {
    options_.push_back(option);
}

void CommandLineParser::addOption(const CommandLineOption& option,
                                  std::function<void(const std::string&)> callback) {
    options_.push_back(option);
    for (const auto& name : option.getNames()) {
        callbacks_[name] = callback;
    }
}

bool CommandLineParser::parse(int argc, const char* const argv[]) {
    if (argc <= 0 || !argv) {
        setError("Invalid arguments");
        return false;
    }

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    return parse(args);
}

bool CommandLineParser::parse(const std::vector<std::string>& args) {
    clear();

    if (args.empty()) {
        return true;
    }

    // Skip the first argument (program name)
    size_t start_index = 1;

    for (size_t i = start_index; i < args.size(); ++i) {
        const std::string& arg = args[i];

        // Check if it's an option (starts with - or --)
        if (arg.length() >= 2 && (arg[0] == '-' || arg[0] == '/')) {
            std::string option_name = arg;
            std::string option_value;

            // Handle options with values (--option=value or -o=value)
            size_t equal_pos = arg.find('=');
            if (equal_pos != std::string::npos) {
                option_name = arg.substr(0, equal_pos);
                option_value = arg.substr(equal_pos + 1);
            }

            // Find the option
            const CommandLineOption* option = findOption(option_name);
            if (!option) {
                setError("Unknown option: " + option_name);
                return false;
            }

            // Check if option needs a value
            if (option->takesValue()) {
                if (option_value.empty()) {
                    // Value should be in the next argument
                    if (i + 1 >= args.size()) {
                        setError("Option " + option_name + " requires a value");
                        return false;
                    }
                    option_value = args[++i];  // Consume the next argument
                }
            } else if (!option_value.empty()) {
                setError("Option " + option_name + " does not take a value");
                return false;
            }

            // Store the option value
            option_values_[option_name].push_back(option_value);

            // Call callback if registered
            auto callback = callbacks_.find(option_name);
            if (callback != callbacks_.end()) {
                callback->second(option_value);
            }
        } else {
            // It's a positional argument
            positional_args_.push_back(arg);
        }
    }

    return true;
}

bool CommandLineParser::isSet(const std::string& name) const {
    return option_values_.find(name) != option_values_.end();
}

std::string CommandLineParser::value(const std::string& name) const {
    auto it = option_values_.find(name);
    if (it != option_values_.end() && !it->second.empty()) {
        return it->second[0];
    }

    // Check if option has a default value
    const CommandLineOption* option = findOption(name);
    if (option && option->hasDefaultValue()) {
        return option->getDefaultValue();
    }

    return "";
}

std::vector<std::string> CommandLineParser::values(const std::string& name) const {
    auto it = option_values_.find(name);
    if (it != option_values_.end()) {
        return it->second;
    }

    // Check if option has a default value
    const CommandLineOption* option = findOption(name);
    if (option && option->hasDefaultValue()) {
        return {option->getDefaultValue()};
    }

    return {};
}

const std::vector<std::string>& CommandLineParser::positionalArguments() const {
    return positional_args_;
}

void CommandLineParser::showHelp(int exit_code) const {
    std::cout << generateHelpText() << std::endl;
    if (exit_code >= 0) {
        std::exit(exit_code);
    }
}

void CommandLineParser::showVersion(int exit_code) const {
    if (!application_name_.empty()) {
        std::cout << application_name_;
        if (!application_version_.empty()) {
            std::cout << " " << application_version_;
        }
        std::cout << std::endl;
    }
    if (exit_code >= 0) {
        std::exit(exit_code);
    }
}

bool CommandLineParser::isHelpRequested() const {
    return isSet("--help") || isSet("-h");
}

bool CommandLineParser::isVersionRequested() const {
    return isSet("--version") || isSet("-v");
}

const std::string& CommandLineParser::getErrorMessage() const {
    return error_message_;
}

void CommandLineParser::clear() {
    option_values_.clear();
    positional_args_.clear();
    error_message_.clear();
}

const CommandLineOption* CommandLineParser::findOption(const std::string& name) const {
    for (const auto& option : options_) {
        for (const auto& option_name : option.getNames()) {
            if (option_name == name) {
                return &option;
            }
        }
    }
    return nullptr;
}

std::string CommandLineParser::generateHelpText() const {
    std::ostringstream oss;

    // Application header
    if (!application_name_.empty()) {
        oss << application_name_;
        if (!application_version_.empty()) {
            oss << " " << application_version_;
        }
        oss << std::endl << std::endl;
    }

    // Usage
    oss << "Usage: ";
    if (!application_name_.empty()) {
        oss << application_name_;
    } else {
        oss << "program";
    }
    oss << " [options]";

    if (!positional_args_.empty()) {
        oss << " [arguments]";
    }
    oss << std::endl << std::endl;

    // Options
    if (!options_.empty()) {
        oss << "Options:" << std::endl;

        // Find the maximum option name length for alignment
        size_t max_option_length = 0;
        for (const auto& option : options_) {
            std::string option_text;
            for (size_t i = 0; i < option.getNames().size(); ++i) {
                if (i > 0) {
                    option_text += ", ";
                }
                option_text += option.getNames()[i];
                if (option.takesValue()) {
                    option_text += " <" + option.getValueName() + ">";
                }
            }
            max_option_length = std::max(max_option_length, option_text.length());
        }

        // Print options
        for (const auto& option : options_) {
            std::string option_text;
            for (size_t i = 0; i < option.getNames().size(); ++i) {
                if (i > 0) {
                    option_text += ", ";
                }
                option_text += option.getNames()[i];
                if (option.takesValue()) {
                    option_text += " <" + option.getValueName() + ">";
                }
            }

            oss << "  " << std::left << std::setw(static_cast<int>(max_option_length + 2))
                << option_text << option.getDescription();

            if (option.hasDefaultValue()) {
                oss << " (default: " << option.getDefaultValue() << ")";
            }
            oss << std::endl;
        }
        oss << std::endl;
    }

    return oss.str();
}

void CommandLineParser::setError(const std::string& message) {
    error_message_ = message;
}

}  // namespace utils
}  // namespace vne
