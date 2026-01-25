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
