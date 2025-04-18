/*
* Created 4/17/2025
* Author: Master801
*/

#ifndef CLI_HPP
#define CLI_HPP
#endif

#include <iostream>
#include <filesystem>

#include "../ndsfactory/ndsfactory.h"

bool unpackROM(const std::filesystem::path fpROM, NDSFactory* ndsFactory, const bool silent, NDSHeader* ndsHeader, bool overdumpARM9, std::filesystem::path pathHeader, std::filesystem::path pathARM9, std::filesystem::path pathARM7, std::filesystem::path pathFATNameTable, std::filesystem::path pathFAT, std::filesystem::path pathFATData, std::filesystem::path pathARM9Overlay, std::filesystem::path pathARM9OverlayData, std::filesystem::path pathARM7Overlay, std::filesystem::path pathARM7OverlayData, std::filesystem::path pathLogos);

int packROM(NDSFactory* ndsFactory, const bool silent, std::filesystem::path pathROMOut, int pad, bool trim, bool shouldFixCRC, std::filesystem::path pathHeader, std::filesystem::path pathARM9, std::filesystem::path pathARM7, std::filesystem::path pathFATNameTable, std::filesystem::path pathFAT, std::filesystem::path pathFATData, std::filesystem::path pathARM9Overlay, std::filesystem::path pathARM9OverlayData, std::filesystem::path pathARM7Overlay, std::filesystem::path pathARM7OverlayData, std::filesystem::path pathLogos);
