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

#include <gtest/gtest.h>

#include "vertexnova/utils/command_line_parser.h"

#include <memory>

using namespace vne::utils;

class CommandLineParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = std::make_unique<CommandLineParser>("TestApp", "1.0.0");

        // Add test options
        parser_->addOption(CommandLineOption("--width", "Window width", "pixels", "1920"));
        parser_->addOption(CommandLineOption("--height", "Window height", "pixels", "1080"));
        parser_->addOption(CommandLineOption("--fullscreen", "Run in fullscreen mode"));
        parser_->addOption(CommandLineOption("--vsync", "Enable vertical synchronization"));
    }

    void TearDown() override {
        parser_.reset();
    }

protected:
    std::unique_ptr<CommandLineParser> parser_;
};

TEST_F(CommandLineParserTest, Constructor) {
    CommandLineParser test_parser("TestApp", "2.0.0");
    EXPECT_FALSE(test_parser.isHelpRequested());
    EXPECT_FALSE(test_parser.isVersionRequested());
}

TEST_F(CommandLineParserTest, AddOption) {
    CommandLineOption option("--test", "Test option", "value", "default");
    parser_->addOption(option);

    // isSet should return false until the option is actually parsed
    EXPECT_FALSE(parser_->isSet("--test"));
    // But value should return the default value even if not parsed
    EXPECT_EQ(parser_->value("--test"), "default");
}

TEST_F(CommandLineParserTest, AddOptionAndParse) {
    CommandLineOption option("--test", "Test option", "value", "default");
    parser_->addOption(option);

    // Parse with the option
    std::vector<std::string> args = {"program", "--test", "custom_value"};
    EXPECT_TRUE(parser_->parse(args));

    // Now isSet should return true
    EXPECT_TRUE(parser_->isSet("--test"));
    EXPECT_EQ(parser_->value("--test"), "custom_value");
}

TEST_F(CommandLineParserTest, ParseSimpleOptions) {
    std::vector<std::string> args = {"program", "--width", "1280", "--height", "720"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isSet("--width"));
    EXPECT_TRUE(parser_->isSet("--height"));
    EXPECT_EQ(parser_->value("--width"), "1280");
    EXPECT_EQ(parser_->value("--height"), "720");
}

TEST_F(CommandLineParserTest, ParseBooleanOptions) {
    std::vector<std::string> args = {"program", "--fullscreen", "--vsync"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isSet("--fullscreen"));
    EXPECT_TRUE(parser_->isSet("--vsync"));
}

TEST_F(CommandLineParserTest, ParseOptionsWithEquals) {
    std::vector<std::string> args = {"program", "--width=1280", "--height=720"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_EQ(parser_->value("--width"), "1280");
    EXPECT_EQ(parser_->value("--height"), "720");
}

TEST_F(CommandLineParserTest, ParsePositionalArguments) {
    std::vector<std::string> args = {"program", "--width", "1280", "file1.txt", "file2.txt"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_EQ(parser_->value("--width"), "1280");

    const auto& positional = parser_->positionalArguments();
    EXPECT_EQ(positional.size(), 2);
    EXPECT_EQ(positional[0], "file1.txt");
    EXPECT_EQ(positional[1], "file2.txt");
}

TEST_F(CommandLineParserTest, ParseHelpOption) {
    std::vector<std::string> args = {"program", "--help"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isHelpRequested());
}

TEST_F(CommandLineParserTest, ParseVersionOption) {
    std::vector<std::string> args = {"program", "--version"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isVersionRequested());
}

TEST_F(CommandLineParserTest, ParseShortHelpOption) {
    std::vector<std::string> args = {"program", "-h"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isHelpRequested());
}

TEST_F(CommandLineParserTest, ParseShortVersionOption) {
    std::vector<std::string> args = {"program", "-v"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isVersionRequested());
}

TEST_F(CommandLineParserTest, ParseUnknownOption) {
    std::vector<std::string> args = {"program", "--unknown"};

    EXPECT_FALSE(parser_->parse(args));
    EXPECT_FALSE(parser_->getErrorMessage().empty());
}

TEST_F(CommandLineParserTest, ParseMissingValue) {
    std::vector<std::string> args = {"program", "--width"};

    EXPECT_FALSE(parser_->parse(args));
    EXPECT_FALSE(parser_->getErrorMessage().empty());
}

TEST_F(CommandLineParserTest, ParseInvalidValueFormat) {
    std::vector<std::string> args = {"program", "--fullscreen=value"};

    EXPECT_FALSE(parser_->parse(args));
    EXPECT_FALSE(parser_->getErrorMessage().empty());
}

TEST_F(CommandLineParserTest, DefaultValues) {
    std::vector<std::string> args = {"program"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_EQ(parser_->value("--width"), "1920");
    EXPECT_EQ(parser_->value("--height"), "1080");
}

TEST_F(CommandLineParserTest, MultipleValues) {
    // Add an option that can appear multiple times
    parser_->addOption(CommandLineOption("--file", "Input file", "filename"));

    std::vector<std::string> args = {"program", "--file", "file1.txt", "--file", "file2.txt"};

    EXPECT_TRUE(parser_->parse(args));
    auto vals = parser_->values("--file");
    EXPECT_EQ(vals.size(), 2);
    EXPECT_EQ(vals[0], "file1.txt");
    EXPECT_EQ(vals[1], "file2.txt");
}

TEST_F(CommandLineParserTest, Clear) {
    std::vector<std::string> args = {"program", "--width", "1280"};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->isSet("--width"));

    parser_->clear();
    EXPECT_FALSE(parser_->isSet("--width"));
    EXPECT_TRUE(parser_->positionalArguments().empty());
}

TEST_F(CommandLineParserTest, ParseArgcArgv) {
    const char* argv[] = {"program", "--width", "1280", "--height", "720"};
    int argc = 5;

    EXPECT_TRUE(parser_->parse(argc, argv));
    EXPECT_EQ(parser_->value("--width"), "1280");
    EXPECT_EQ(parser_->value("--height"), "720");
}

TEST_F(CommandLineParserTest, ParseEmptyArgs) {
    std::vector<std::string> args = {};

    EXPECT_TRUE(parser_->parse(args));
    EXPECT_TRUE(parser_->positionalArguments().empty());
}

TEST_F(CommandLineParserTest, ParseNullArgs) {
    EXPECT_FALSE(parser_->parse(0, nullptr));
    EXPECT_FALSE(parser_->getErrorMessage().empty());
}

TEST_F(CommandLineParserTest, ParseInvalidArgc) {
    const char* argv[] = {"program"};

    EXPECT_FALSE(parser_->parse(-1, argv));
    EXPECT_FALSE(parser_->getErrorMessage().empty());
}

//==============================================================================
// CommandLineOption Tests
//==============================================================================

TEST(CommandLineOptionTest, SingleNameConstructor) {
    CommandLineOption option("--test", "Test description", "value", "default");

    EXPECT_EQ(option.getName(), "--test");
    EXPECT_EQ(option.getNames().size(), 1);
    EXPECT_EQ(option.getDescription(), "Test description");
    EXPECT_EQ(option.getValueName(), "value");
    EXPECT_EQ(option.getDefaultValue(), "default");
    EXPECT_TRUE(option.takesValue());
    EXPECT_TRUE(option.hasDefaultValue());
}

TEST(CommandLineOptionTest, MultipleNamesConstructor) {
    CommandLineOption option(std::vector<std::string>{"--verbose", "-v"}, "Enable verbose mode");

    EXPECT_EQ(option.getName(), "--verbose");
    EXPECT_EQ(option.getNames().size(), 2);
    EXPECT_EQ(option.getNames()[0], "--verbose");
    EXPECT_EQ(option.getNames()[1], "-v");
    EXPECT_EQ(option.getDescription(), "Enable verbose mode");
    EXPECT_TRUE(option.getValueName().empty());
    EXPECT_TRUE(option.getDefaultValue().empty());
    EXPECT_FALSE(option.takesValue());
    EXPECT_FALSE(option.hasDefaultValue());
}

TEST(CommandLineOptionTest, EmptyNamesVector) {
    CommandLineOption option(std::vector<std::string>{}, "Empty names option");

    // getName should return empty string for empty names vector
    EXPECT_TRUE(option.getName().empty());
    EXPECT_TRUE(option.getNames().empty());
}

//==============================================================================
// CommandLineParser Standalone Tests
//==============================================================================

TEST(CommandLineParserStandaloneTest, SetApplicationNameAndVersion) {
    CommandLineParser parser;

    parser.setApplicationName("MyApp");
    parser.setApplicationVersion("2.0.0");

    // Verify by checking help text generation (which uses these values)
    testing::internal::CaptureStdout();
    parser.showHelp(-1);  // -1 means don't exit
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("MyApp") != std::string::npos);
    EXPECT_TRUE(output.find("2.0.0") != std::string::npos);
}

TEST(CommandLineParserStandaloneTest, ShowVersionNoExit) {
    CommandLineParser parser("TestApp", "3.0.0");

    testing::internal::CaptureStdout();
    parser.showVersion(-1);  // -1 means don't exit
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("TestApp") != std::string::npos);
    EXPECT_TRUE(output.find("3.0.0") != std::string::npos);
}

TEST(CommandLineParserStandaloneTest, ShowVersionEmptyName) {
    CommandLineParser parser;

    testing::internal::CaptureStdout();
    parser.showVersion(-1);
    std::string output = testing::internal::GetCapturedStdout();

    // Empty app name should produce no output
    EXPECT_TRUE(output.empty());
}

TEST(CommandLineParserStandaloneTest, ShowHelpNoExit) {
    CommandLineParser parser("HelpTest", "1.0.0");
    parser.addOption(CommandLineOption("--output", "Output file", "file", "out.txt"));

    testing::internal::CaptureStdout();
    parser.showHelp(-1);  // -1 means don't exit
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("HelpTest") != std::string::npos);
    EXPECT_TRUE(output.find("Usage:") != std::string::npos);
    EXPECT_TRUE(output.find("Options:") != std::string::npos);
    EXPECT_TRUE(output.find("--output") != std::string::npos);
    EXPECT_TRUE(output.find("Output file") != std::string::npos);
    EXPECT_TRUE(output.find("(default: out.txt)") != std::string::npos);
}

TEST(CommandLineParserStandaloneTest, ShowHelpEmptyAppName) {
    CommandLineParser parser;

    testing::internal::CaptureStdout();
    parser.showHelp(-1);
    std::string output = testing::internal::GetCapturedStdout();

    // Should show "program" as default
    EXPECT_TRUE(output.find("Usage: program") != std::string::npos);
}

TEST(CommandLineParserStandaloneTest, AddOptionWithCallback) {
    CommandLineParser parser("CallbackTest", "1.0.0");

    std::string captured_value;
    parser.addOption(
        CommandLineOption("--config", "Config file", "file"),
        [&captured_value](const std::string& value) {
            captured_value = value;
        });

    std::vector<std::string> args = {"program", "--config", "settings.json"};
    EXPECT_TRUE(parser.parse(args));

    EXPECT_EQ(captured_value, "settings.json");
}

TEST(CommandLineParserStandaloneTest, ValuesWithDefaultValue) {
    CommandLineParser parser("DefaultTest", "1.0.0");
    parser.addOption(CommandLineOption("--level", "Log level", "level", "info"));

    std::vector<std::string> args = {"program"};
    EXPECT_TRUE(parser.parse(args));

    // values() should return default value when option not set
    auto vals = parser.values("--level");
    EXPECT_EQ(vals.size(), 1);
    EXPECT_EQ(vals[0], "info");
}

TEST(CommandLineParserStandaloneTest, ValuesNonExistentOption) {
    CommandLineParser parser("Test", "1.0.0");

    std::vector<std::string> args = {"program"};
    EXPECT_TRUE(parser.parse(args));

    // values() for non-existent option should return empty vector
    auto vals = parser.values("--nonexistent");
    EXPECT_TRUE(vals.empty());
}

TEST(CommandLineParserStandaloneTest, ValueNonExistentOption) {
    CommandLineParser parser("Test", "1.0.0");

    std::vector<std::string> args = {"program"};
    EXPECT_TRUE(parser.parse(args));

    // value() for non-existent option should return empty string
    EXPECT_TRUE(parser.value("--nonexistent").empty());
}

TEST(CommandLineParserStandaloneTest, HelpTextWithMultipleNames) {
    CommandLineParser parser("MultiNameTest", "1.0.0");
    parser.addOption(CommandLineOption(
        std::vector<std::string>{"--output", "-o"},
        "Output file",
        "file"));

    testing::internal::CaptureStdout();
    parser.showHelp(-1);
    std::string output = testing::internal::GetCapturedStdout();

    // Should show both option names
    EXPECT_TRUE(output.find("--output") != std::string::npos);
    EXPECT_TRUE(output.find("-o") != std::string::npos);
}

TEST(CommandLineParserStandaloneTest, DefaultConstructor) {
    CommandLineParser parser;

    // Should work with empty app name
    std::vector<std::string> args = {"program", "--help"};
    EXPECT_TRUE(parser.parse(args));
    EXPECT_TRUE(parser.isHelpRequested());
}

TEST(CommandLineParserStandaloneTest, PositionalArgumentsRef) {
    CommandLineParser parser("Test", "1.0.0");

    std::vector<std::string> args = {"program", "arg1", "arg2", "arg3"};
    EXPECT_TRUE(parser.parse(args));

    const auto& positional = parser.positionalArguments();
    EXPECT_EQ(positional.size(), 3);
    EXPECT_EQ(positional[0], "arg1");
    EXPECT_EQ(positional[1], "arg2");
    EXPECT_EQ(positional[2], "arg3");
}

TEST(CommandLineParserStandaloneTest, SlashOptionPrefix) {
    CommandLineParser parser("Test", "1.0.0");
    parser.addOption(CommandLineOption("/debug", "Enable debug mode"));

    std::vector<std::string> args = {"program", "/debug"};
    EXPECT_TRUE(parser.parse(args));
    EXPECT_TRUE(parser.isSet("/debug"));
}
