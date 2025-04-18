/*
* Created 4/17/2025
* Author: Master801
*/

#ifndef CLI_HPP
#define CLI_HPP
#endif

#include <iostream>
#include <filesystem>

#include <argparse/argparse.hpp>

#include "../ndsfactory/ndsfactory.h"

#define EXIT_CODE_FS_NOT_FOUND 404
#define EXIT_CODE_FS_BAD_IO 1
#define EXIT_CODE_BAD_ARGUMENT 2
#define EXIT_CODE_NF_RESULT_BAD -1

void setUpArguments(
	argparse::ArgumentParser& program,
	argparse::ArgumentParser& argInfo,
	argparse::ArgumentParser& argUnpack,
	argparse::ArgumentParser& argPack,
	argparse::ArgumentParser& argFATTools,
	argparse::ArgumentParser& argFATToolsExtract,
	argparse::ArgumentParser& argFATToolsBuild,
	argparse::ArgumentParser& argFATToolsPatch
);

void printInfo(NDSHeader *ndsHeader);

bool unpackROM(
	const std::filesystem::path &fpROM,
	NDSFactory *ndsFactory,
	const bool silent,
	NDSHeader *ndsHeader,
	const bool overdumpARM9,
	std::filesystem::path &pathHeader,
	std::filesystem::path &pathARM9,
	std::filesystem::path &pathARM7,
	std::filesystem::path &pathFATNameTable,
	std::filesystem::path &pathFAT,
	std::filesystem::path &pathFATData,
	std::filesystem::path &pathARM9Overlay,
	std::filesystem::path &pathARM9OverlayData,
	std::filesystem::path &pathARM7Overlay,
	std::filesystem::path &pathARM7OverlayData,
	std::filesystem::path &pathLogos
);

int packROM(
	NDSFactory *ndsFactory,
	const bool silent,
	std::filesystem::path& pathROMOut,
	const int pad,
	const bool trim,
	const bool shouldFixCRC,
	std::filesystem::path& pathHeader,
	std::filesystem::path& pathARM9,
	std::filesystem::path& pathARM7,
	std::filesystem::path& pathFATNameTable,
	std::filesystem::path& pathFAT,
	std::filesystem::path& pathFATData,
	std::filesystem::path& pathARM9Overlay,
	std::filesystem::path& pathARM9OverlayData,
	std::filesystem::path& pathARM7Overlay,
	std::filesystem::path& pathARM7OverlayData,
	std::filesystem::path& pathLogos
);
