#pragma once
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

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vne {
namespace utils {

/**
 * @brief Represents a command line option.
 *
 * Defines an option that can be parsed from the command line,
 * including its name(s), description, and optional default value.
 */
class CommandLineOption {
public:
    /**
     * @brief Construct a command line option with a single name.
     * @param name The option name (e.g., "--help", "-h")
     * @param description Description of what the option does
     * @param value_name Optional value name for options that take values
     * @param default_value Optional default value
     */
    CommandLineOption(const std::string& name,
                      const std::string& description,
                      const std::string& value_name = "",
                      const std::string& default_value = "");

    /**
     * @brief Construct a command line option with multiple names.
     * @param names Vector of option names (e.g., {"--help", "-h"})
     * @param description Description of what the option does
     * @param value_name Optional value name for options that take values
     * @param default_value Optional default value
     */
    CommandLineOption(const std::vector<std::string>& names,
                      const std::string& description,
                      const std::string& value_name = "",
                      const std::string& default_value = "");

    /**
     * @brief Get the primary name of the option.
     * @return The first name in the names list
     */
    const std::string& getName() const;

    /**
     * @brief Get all names for this option.
     * @return Vector of all option names
     */
    const std::vector<std::string>& getNames() const;

    /**
     * @brief Get the description of the option.
     * @return Description string
     */
    const std::string& getDescription() const;

    /**
     * @brief Get the value name (for options that take values).
     * @return Value name string
     */
    const std::string& getValueName() const;

    /**
     * @brief Get the default value.
     * @return Default value string
     */
    const std::string& getDefaultValue() const;

    /**
     * @brief Check if this option takes a value.
     * @return true if the option takes a value
     */
    bool takesValue() const;

    /**
     * @brief Check if this option has a default value.
     * @return true if the option has a default value
     */
    bool hasDefaultValue() const;

private:
    std::vector<std::string> names_;
    std::string description_;
    std::string value_name_;
    std::string default_value_;
    static const std::string kEmptyString;
};

/**
 * @brief Command line argument parser.
 *
 * Parses command line arguments and provides access to option values
 * and positional arguments.
 */
class CommandLineParser {
public:
    /**
     * @brief Construct a command line parser.
     * @param application_name Name of the application (used in help text)
     * @param application_version Version of the application
     */
    explicit CommandLineParser(const std::string& application_name = "",
                               const std::string& application_version = "");

    ~CommandLineParser();

    /**
     * @brief Set the application name.
     * @param name Application name
     */
    void setApplicationName(const std::string& name);

    /**
     * @brief Set the application version.
     * @param version Application version
     */
    void setApplicationVersion(const std::string& version);

    /**
     * @brief Add a command line option.
     * @param option The option to add
     */
    void addOption(const CommandLineOption& option);

    /**
     * @brief Add a command line option with a callback.
     * @param option The option to add
     * @param callback Function to call when this option is found
     */
    void addOption(const CommandLineOption& option,
                   std::function<void(const std::string&)> callback);

    /**
     * @brief Parse the command line arguments.
     * @param argc Number of arguments
     * @param argv Array of argument strings
     * @return true if parsing was successful
     */
    bool parse(int argc, const char* const argv[]);

    /**
     * @brief Parse the command line arguments from a vector.
     * @param args Vector of argument strings
     * @return true if parsing was successful
     */
    bool parse(const std::vector<std::string>& args);

    /**
     * @brief Check if an option was specified.
     * @param name Option name
     * @return true if the option was found
     */
    bool isSet(const std::string& name) const;

    /**
     * @brief Get the value of an option.
     * @param name Option name
     * @return The value of the option, or empty string if not found
     */
    std::string value(const std::string& name) const;

    /**
     * @brief Get all values for an option.
     * @param name Option name
     * @return Vector of all values for the option
     */
    std::vector<std::string> values(const std::string& name) const;

    /**
     * @brief Get positional arguments.
     * @return Vector of positional arguments
     */
    const std::vector<std::string>& positionalArguments() const;

    /**
     * @brief Show help text and optionally exit.
     * @param exit_code Exit code to use (-1 to not exit)
     */
    void showHelp(int exit_code = 0) const;

    /**
     * @brief Show version information and optionally exit.
     * @param exit_code Exit code to use (-1 to not exit)
     */
    void showVersion(int exit_code = 0) const;

    /**
     * @brief Check if help was requested.
     * @return true if --help or -h was specified
     */
    bool isHelpRequested() const;

    /**
     * @brief Check if version was requested.
     * @return true if --version or -v was specified
     */
    bool isVersionRequested() const;

    /**
     * @brief Get error message if parsing failed.
     * @return Error message string
     */
    const std::string& getErrorMessage() const;

    /**
     * @brief Clear all parsed options and arguments.
     */
    void clear();

private:
    const CommandLineOption* findOption(const std::string& name) const;
    std::string generateHelpText() const;
    void setError(const std::string& message);

    std::string application_name_;
    std::string application_version_;
    std::vector<CommandLineOption> options_;
    std::map<std::string, std::function<void(const std::string&)>> callbacks_;
    std::vector<std::string> positional_args_;
    std::map<std::string, std::vector<std::string>> option_values_;
    std::string error_message_;
};

}  // namespace utils
}  // namespace vne
