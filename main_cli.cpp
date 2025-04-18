/*
* Created 4/10/2025
* Author: Master801
*/

#include <iostream>
#include <format>
#include <filesystem>

#include <argparse/argparse.hpp>

#include "ndsfactory/ndsfactory.h"
#include "ndsfactory/version.h"

#include "cli/cli.hpp"

static void printInfo(NDSHeader* ndsHeader)
{
	std::cout << std::format("Used ROM Size: 0x{:08X}", ndsHeader->RomSize) << std::endl;

	std::cout << std::format("Header Size: 0x{:04X}", ndsHeader->HeaderSize) << std::endl;
	std::cout << std::format("Header CRC16: 0x{:04X}", ndsHeader->HeaderCRC16) << std::endl;

	std::string stringHexNinLogo;
	for (int i = 0; i < sizeof(ndsHeader->NintendoLogo); i++) stringHexNinLogo += std::format("{:02X}", ndsHeader->NintendoLogo[i]);
	std::cout << "Nintendo Logo: 0x" << stringHexNinLogo << std::endl;
	std::cout << std::format("Nintendo CRC: 0x{:X}", ndsHeader->NintendoLogoCRC) << std::endl;

	std::cout << std::format("Game Title: \"{}\"", std::string(ndsHeader->GameTitle)) << std::endl;
	std::cout << std::format("Game Code: {}", std::string(ndsHeader->GameCode)) << std::endl;
	std::cout << std::format("Maker Code: {}", std::string(reinterpret_cast<char*>(ndsHeader->MakerCode))) << std::endl;
	std::cout << std::format("Unit Code: {}", ndsHeader->UnitCode) << std::endl;
	std::cout << std::format("Device Code: {}", ndsHeader->UnitCode) << std::endl;
	std::cout << std::format("Card Size: {}", ndsHeader->DeviceSize) << std::endl;
	std::cout << std::format("Card Info: {}", ndsHeader->RomVersion) << std::endl;
	std::cout << std::format("Flags: {}", ndsHeader->Flags) << std::endl;

	std::cout << std::format("ARM9 Rom Address: 0x{:08X}", ndsHeader->Arm9RomAddr) << std::endl;
	std::cout << std::format("ARM9 Entry Address: 0x{:08X}", ndsHeader->Arm9EntryAddr) << std::endl;
	std::cout << std::format("ARM9 RAM Address: 0x{:08X}", ndsHeader->Arm9RamAddr) << std::endl;
	std::cout << std::format("ARM9 Size: 0x{:08X}", ndsHeader->Arm9Size) << std::endl;

	std::cout << std::format("ARM9 Overlay Address: 0x{:08X}", ndsHeader->Arm9OverlayAddr) << std::endl;
	std::cout << std::format("ARM9 Overlay Size: 0x{:08X}", ndsHeader->Arm9OverlaySize) << std::endl;

	std::cout << std::format("ARM9 Auto Load List (AL) RAM Address: 0x{:08X}", ndsHeader->ARM9AutoLoadListRamAddr) << std::endl;

	std::cout << std::format("ARM7 Rom Address: 0x{:08X}", ndsHeader->Arm7RomAddr) << std::endl;
	std::cout << std::format("ARM7 Entry Address: 0x{:08X}", ndsHeader->Arm7EntryAddr) << std::endl;
	std::cout << std::format("ARM7 RAM Address: 0x{:08X}", ndsHeader->Arm7RamAddr) << std::endl;
	std::cout << std::format("ARM7 Size: 0x{:08X}", ndsHeader->Arm7Size) << std::endl;

	std::cout << std::format("ARM7 Overlay Address: 0x{:08X}", ndsHeader->Arm7OverlayAddr) << std::endl;
	std::cout << std::format("ARM7 Overlay Size: 0x{:08X}", ndsHeader->Arm7OverlaySize) << std::endl;

	std::cout << std::format("ARM7 Auto Load List (AL) RAM Address: 0x{:08X}", ndsHeader->ARM7AutoLoadListRamAddr) << std::endl;

	std::cout << std::format("Filename Table Address: 0x{:08X}", ndsHeader->FilenameTableAddr) << std::endl;
	std::cout << std::format("Filename Table Size: 0x{:08X}", ndsHeader->FilenameSize) << std::endl;
	std::cout << std::format("FAT Address: 0x{:08X}", ndsHeader->FATAddr) << std::endl;
	std::cout << std::format("FAT Size: 0x{:08X}", ndsHeader->FATSize) << std::endl;
	std::cout << std::format("FAT Files Address: 0x{:08X}", (ndsHeader->IconTitleAddr + IconTitleSize)) << std::endl;

	std::cout << std::format("Port 40001A4h NC: 0x{:08X}", ndsHeader->NormalCommandsSettings) << std::endl;
	std::cout << std::format("Port 40001A4h KC: 0x{:08X}", ndsHeader->Key1CommandsSettings) << std::endl;

	std::cout << std::format("Icon/Title Address: 0x{:08X}", ndsHeader->IconTitleAddr) << std::endl;

	std::cout << std::format("Secure Area CRC16: 0x{:04X}", ndsHeader->SecureAreaCRC16) << std::endl;
	std::cout << std::format("Secure Area Timeout: 0x{:04X}", ndsHeader->SecureAreaLoadingTimeout) << std::endl;
	std::cout << std::format("Secure Area Disable: 0x{:04X}", ndsHeader->SecureAreaDisable) << std::endl;

	std::cout << std::format("Debug ROM Address: 0x{:08X}", ndsHeader->DebugRomAddr) << std::endl;
	std::cout << std::format("Debug Size: 0x{:08X}", ndsHeader->DebugSize) << std::endl;
	std::cout << std::format("Debug RAM Address: 0x{:08X}", ndsHeader->DebugRamAddr) << std::endl;
	std::cout << std::format("Debug RAM Address: 0x{:08X}", ndsHeader->DebugRamAddr) << std::endl;
}

int main(int argc, char* argv[])
{
	//Set up commands
	argparse::ArgumentParser program("NDSFactory", std::format("{}-{}", PROGRAM_VERSION, GIT_COMMIT_HASH));
	program
		.add_argument("--silent", "-s")
		.flag()
		.help("Suppress status messages in the console");

	argparse::ArgumentParser commandInfo("info");
	commandInfo.add_description("Gets info from the given ROM");
	commandInfo
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("ROM file to get info from");

	argparse::ArgumentParser commandUnpack("unpack");
	commandUnpack.add_description("Unpack the target ROM file");
	commandUnpack
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("ROM file to unpack");
	commandUnpack
		.add_argument("--header", "-h")
		.metavar("<header.bin>")
		.help("Header file");
	commandUnpack
		.add_argument("--arm9", "-a9")
		.metavar("<arm9.bin>")
		.help("ARM9 executable");
	commandUnpack
		.add_argument("--arm9overdump")
		.flag()
		.required()
		.help("Dumps 12 extra bytes for a 1:1 file");
	commandUnpack
		.add_argument("--arm7", "-a7")
		.metavar("<arm7.bin>")
		.help("ARM7 executable");
	commandUnpack
		.add_argument("--fatnametable", "-fntbl")
		.metavar("<fnt.bin>")
		.help("FAT name table");
	commandUnpack
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.help("FAT file");
	commandUnpack
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.help("FAT data file");
	commandUnpack
		.add_argument("--arm9overlay", "-a9o")
		.metavar("<a9ovr.bin>")
		.help("ARM9 overlay file");
	commandUnpack
		.add_argument("--arm9overlaydata", "-a9od")
		.metavar("<a9ovr_data.bin>")
		.help("ARM9 overlay data file");
	commandUnpack
		.add_argument("--arm7overlay", "-a7o")
		.metavar("<a7ovr.bin>")
		.help("ARM7 overlay file");
	commandUnpack
		.add_argument("--arm7overlaydata", "-a7od")
		.metavar("<a7ovr_data.bin>")
		.help("ARM7 overlay data file");
	commandUnpack
		.add_argument("--logos", "-l")
		.metavar("<itl.bin>")
		.help("Icon / Title Logos file");

	argparse::ArgumentParser commandPack("pack");
	commandPack.add_description("Pack a ROM file from extracted files");
	commandPack
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("Path where the packed ROM will be");
	commandPack
		.add_argument("--header", "-h")
		.metavar("<header.bin>")
		.required()
		.help("Header file");
	commandPack
		.add_argument("--arm9", "-a9")
		.metavar("<arm9.bin>")
		.required()
		.help("ARM9 executable");
	commandPack
		.add_argument("--arm7", "-a7")
		.metavar("<arm7.bin>")
		.required()
		.help("ARM7 executable");
	commandPack
		.add_argument("--fatnametable", "-fntbl")
		.metavar("<fnt.bin>")
		.required()
		.help("FAT name table");
	commandPack
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.required()
		.help("FAT file");
	commandPack
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.required()
		.help("FAT data file");
	commandPack
		.add_argument("--arm9overlay", "-a9o")
		.metavar("<a9ovr.bin>")
		.help("ARM9 overlay file");
	commandPack
		.add_argument("--arm9overlaydata", "-a9od")
		.metavar("<a9ovr_data.bin>")
		.help("ARM9 overlay data file");
	commandPack
		.add_argument("--arm7overlay", "-a7o")
		.metavar("<a7ovr.bin>")
		.help("ARM7 overlay file");
	commandPack
		.add_argument("--arm7overlaydata", "-a7od")
		.metavar("<a7ovr_data.bin>")
		.help("ARM7 overlay data file");
	commandPack
		.add_argument("--logos", "-l")
		.metavar("<itl.bin>")
		.help("Icon / Title Logos file");
	commandPack
		.add_argument("--pad", "-p")
		.metavar("<0 or 1>")
		.default_value(1)
		.choices(0, 1)
		.scan<'d', int>()
		.required()
		.help("0 to pad ROM with 0x00s\n1 to pad ROM with 0xFFs\nMost commercial ROMs are padded with 0xFF\nBy default, pads with 0xFF\n");
	commandPack
		.add_argument("--trim", "-t")
		.flag()
		.required()
		.help("Trim ROM after packing.\nBy default, the ROM is not trimmed.\n");
	commandPack
		.add_argument("--fixcrc", "-crc")
		.flag()
		.required()
		.help("Fixes the ROM's header CRC16.\nCommerical ROMs will not load if this is not enabled.\nDisabled by default.");

	argparse::ArgumentParser commandFATTools("fattools");
	commandFATTools.add_description("Tools for modifying the FAT");
	commandFATTools
		.add_argument("extract")
		.flag()
		.help("Extracts FAT data <fat_data.bin>");
	commandFATTools
		.add_argument("build")
		.flag()
		.help("Builds FAT data <fat_data.bin>");
	commandFATTools
		.add_argument("patch")
		.flag()
		.help("Patches FAT data <fat_data.bin>");

	argparse::ArgumentParser commandFATToolsExtract("extract");
	commandFATToolsExtract
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.required()
		.help("FAT bin file");
	commandFATToolsExtract
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.required()
		.help("FAT data bin file");
	commandFATToolsExtract
		.add_argument("--fatnametable", "-fnt")
		.metavar("<fnt.bin>")
		.required()
		.help("FAT name table");
	commandFATToolsExtract
		.add_argument("--fatdataaddress", "-fdaddr")
		.scan<'X', uint32_t>()
		.metavar("<Hex Integer>")
		.required()
		.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");
	commandFATToolsExtract
		.add_argument("--savefatfileids", "-saveids")
		.flag()
		.required()
		.help("Save file IDs to _file_IDs.txt (required for rebuilding FAT)");
	commandFATToolsExtract
		.add_argument("--fatdatadir", "-fats")
		.metavar("<fat_data>")
		.required()
		.help("The directory to output the extract FAT files to.");

	argparse::ArgumentParser commandFATToolsBuild("build");
	commandFATToolsBuild
		.add_argument("--fatoriginal", "-fo")
		.metavar("<fat.bin>")
		.help("Original <fat.bin> file\nOnly required if the ROM uses overlays");
	commandFATToolsBuild
		.add_argument("--fatdatadir", "-fats")
		.metavar("<fat_data.bin>")
		.required()
		.help("Directory where <fat_data.bin> was extracted to");
	commandFATToolsBuild
		.add_argument("--fatdataaddress", "-fdaddr")
		.scan<'X', uint32_t>()
		.metavar("<Hex Integer>")
		.required()
		.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");
	commandFATToolsBuild
		.add_argument("--fatdatadirout", "-fddo")
		.required()
		.help("The directory to write the new <fat.bin> and <fat_data.bin> to.");

	argparse::ArgumentParser commandFATToolsPatcher("patch");
	commandFATToolsPatcher
		.add_argument("--fat", "-f")
		.required()
		.metavar("<fat.bin>")
		.help("FAT bin file to patch");
	commandFATToolsPatcher
		.add_argument("--originalfatdataaddr", "-ofdaddr")
		.scan<'X', uint32_t>()
		.required()
		.metavar("<Hex Integer>")
		.help("Original address of the FAT Data in Hex.\nEx: 0xDEADBEEF");
	commandFATToolsPatcher
		.add_argument("--newfatdataaddr", "-nfdaddr")
		.scan<'X', uint32_t>()
		.required()
		.metavar("<Hex Integer>")
		.help("New address of the FAT Data in Hex.\nEx: 0xDEADBEEF");
	commandFATToolsPatcher
		.add_argument("--newfat", "-nf")
		.required()
		.metavar("<fat.new.bin>")
		.help("The new patched FAT bin file");

	program.add_subparser(commandInfo);
	program.add_subparser(commandUnpack);
	program.add_subparser(commandPack);

	commandFATTools.add_subparser(commandFATToolsExtract);
	commandFATTools.add_subparser(commandFATToolsBuild);
	commandFATTools.add_subparser(commandFATToolsPatcher);
	program.add_subparser(commandFATTools);

	//TODO Make arguments to edit header

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
		return 1;
	}

	const bool silent = program.get<bool>("--silent");

	NDSFactory ndsFactory = NDSFactory();
	NFResult nfResult;

	std::filesystem::path pathROM;
	if (program.is_subcommand_used("info"))
	{
		pathROM = std::filesystem::path(commandInfo.get<std::string>("--rom"));
		if (!std::filesystem::exists(pathROM))
		{
			std::cout << std::format("ROM file \"{}\" does not exist!", pathROM.string()) << std::endl;
			return 404;
		}

		std::vector<char> romHeader;
		nfResult = ndsFactory.loadRomHeader(std::filesystem::absolute(pathROM).string(), romHeader);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			std::exit(-1);
			return -1;
		}
		std::cout << std::format("Info of ROM \"{}\":", pathROM.string()) << std::endl << std::endl;
		printInfo(
			reinterpret_cast<NDSHeader*>(romHeader.data())
		);
		std::exit(0);
		return 0;
	}
	else if (program.is_subcommand_used("unpack") || program.is_subcommand_used("pack"))
	{
		std::filesystem::path pathHeader;
		std::filesystem::path pathARM9;
		std::filesystem::path pathARM7;
		std::filesystem::path pathFATNameTable;
		std::filesystem::path pathFAT;
		std::filesystem::path pathFATData;
		std::filesystem::path pathARM9Overlay;
		std::filesystem::path pathARM9OverlayData;
		std::filesystem::path pathARM7Overlay;
		std::filesystem::path pathARM7OverlayData;
		std::filesystem::path pathLogos;

		if (program.is_subcommand_used("unpack"))
		{
			pathROM = std::filesystem::path(commandUnpack.get<std::string>("--rom"));
			if (!std::filesystem::exists(pathROM))
			{
				std::cout << std::format("ROM file \"{}\" does not exist!", pathROM.string()) << std::endl;
				return 404;
			}

			if (!silent) std::cout << std::format("Unpacking \"{}\"...", pathROM.string()) << std::endl << std::endl;

			std::vector<char> romHeader;
			nfResult = ndsFactory.loadRomHeader(std::filesystem::absolute(pathROM).string(), romHeader);
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(-1);
				return -1;
			}
			NDSHeader* ndsHeader = reinterpret_cast<NDSHeader*>(romHeader.data());

			if (commandUnpack.present("--header")) pathHeader = std::filesystem::path(commandUnpack.get<std::string>("--header"));
			if (commandUnpack.present("--arm9")) pathARM9 = std::filesystem::path(commandUnpack.get<std::string>("--arm9"));
			if (commandUnpack.present("--arm7")) pathARM7 = std::filesystem::path(commandUnpack.get<std::string>("--arm7"));
			if (commandUnpack.present("--fatnametable")) pathFATNameTable = std::filesystem::path(commandUnpack.get<std::string>("--fatnametable"));
			if (commandUnpack.present("--fat")) pathFAT = std::filesystem::path(commandUnpack.get<std::string>("--fat"));
			if (commandUnpack.present("--fatdata")) pathFATData = std::filesystem::path(commandUnpack.get<std::string>("--fatdata"));
			if (commandUnpack.present("--arm9overlay")) pathARM9Overlay = std::filesystem::path(commandUnpack.get<std::string>("--arm9overlay"));
			if (commandUnpack.present("--arm9overlaydata")) pathARM9OverlayData = std::filesystem::path(commandUnpack.get<std::string>("--arm9overlaydata"));
			if (commandUnpack.present("--arm7overlay")) pathARM7Overlay = std::filesystem::path(commandUnpack.get<std::string>("--arm7overlay"));
			if (commandUnpack.present("--arm7overlaydata")) pathARM7OverlayData = std::filesystem::path(commandUnpack.get<std::string>("--arm7overlaydata"));
			if (commandUnpack.present("--logos")) pathLogos = std::filesystem::path(commandUnpack.get<std::string>("--logos"));

			if (pathHeader.empty())
			{
				std::cout << "Missing header argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathARM9.empty())
			{
				std::cout << "Missing ARM9 argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathARM7.empty())
			{
				std::cout << "Missing ARM7 argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathFATNameTable.empty())
			{
				std::cout << "Missing FAT Name Table argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathFAT.empty())
			{
				std::cout << "Missing FAT argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathFATData.empty())
			{
				std::cout << "Missing FAT Data argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathLogos.empty())
			{
				std::cout << "Missing Icon / Title logo argument!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}

			if (!pathARM9Overlay.empty() && pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay, but missing ARM9 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathARM9Overlay.empty() && !pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay Data, but missing ARM9 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (pathARM7Overlay.empty() && !pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay Data, but missing ARM7 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}
			if (!pathARM7Overlay.empty() && pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay, but missing ARM7 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(2);
				return 2;
			}

			bool properlyUnpacked = unpackROM(
				pathROM,
				&ndsFactory,
				silent,

				ndsHeader,
				commandUnpack.get<bool>("--arm9overdump"),
				pathHeader,
				pathARM9,
				pathARM7,
				pathFATNameTable,
				pathFAT,
				pathFATData,
				pathARM9Overlay,
				pathARM9OverlayData,
				pathARM7Overlay,
				pathARM7OverlayData,
				pathLogos
			);
			if (properlyUnpacked)
			{
				if (!silent) std::cout << std::endl << "Done unpacking!" << std::endl;
			}
			else
			{
				std::cout << std::endl << "Bad unpacking due to an error!" << std::endl;
			}
		}
		else if (program.is_subcommand_used("pack"))
		{
			pathROM = std::filesystem::path(commandPack.get<std::string>("--rom"));
			pathHeader = std::filesystem::path(commandPack.get<std::string>("--header"));
			pathARM9 = std::filesystem::path(commandPack.get<std::string>("--arm9"));
			pathARM7 = std::filesystem::path(commandPack.get<std::string>("--arm7"));
			pathFATNameTable = std::filesystem::path(commandPack.get<std::string>("--fatnametable"));
			pathFAT = std::filesystem::path(commandPack.get<std::string>("--fat"));
			pathFATData = std::filesystem::path(commandPack.get<std::string>("--fatdata"));
			if (commandPack.present("--arm9overlay")) pathARM9Overlay = std::filesystem::path(commandPack.get<std::string>("--arm9overlay"));
			if (commandPack.present("--arm9overlaydata")) pathARM9OverlayData = std::filesystem::path(commandPack.get<std::string>("--arm9overlaydata"));
			if (commandPack.present("--arm7overlay")) pathARM7Overlay = std::filesystem::path(commandPack.get<std::string>("--arm7overlay"));
			if (commandPack.present("--arm7overlaydata")) pathARM7OverlayData = std::filesystem::path(commandPack.get<std::string>("--arm7overlaydata"));
			if (commandPack.present("--logos")) pathLogos = std::filesystem::path(commandPack.get<std::string>("--logos"));

			int pad = commandPack.get<int>("--pad");
			bool trim = commandPack.get<bool>("--trim");
			bool shouldFixCRC = commandPack.get<bool>("--fixcrc");

			if (!silent) std::cout << "Packing ROM..." << std::endl << std::endl;
			int statusCode = packROM(
				&ndsFactory,
				silent,
				pathROM,
				pad,
				trim,
				shouldFixCRC,
				pathHeader,
				pathARM9,
				pathARM7,
				pathFATNameTable,
				pathFAT,
				pathFATData,
				pathARM9Overlay,
				pathARM9OverlayData,
				pathARM7Overlay,
				pathARM7OverlayData,
				pathLogos
			);
			if (!statusCode)
			{
				if (!silent) std::cout << "Done packing!" << std::endl;
			}
			else if (statusCode == 1)
			{
				std::cout << "Failed to pack due to given header file being invalid!" << std::endl;
				std::exit(statusCode);
			}
			else if (statusCode == 2)
			{
				std::cout << std::format("Failed to pack due to given ARM9 file \"{}\" being invalid!", pathARM9.string()) << std::endl;
				std::exit(statusCode);
			}
			else
			{
				std::cout << std::format("Failed to pack due to unknown status code \"{}\"!", statusCode) << std::endl;
				std::exit(statusCode);
			}
		}
	}
	else if (program.is_subcommand_used("fattools"))
	{
		std::filesystem::path pathFAT;
		if (commandFATTools.is_subcommand_used("extract"))
		{
			pathFAT = std::filesystem::path(commandFATToolsExtract.get<std::string>("--fat"));
			std::filesystem::path pathFATData = std::filesystem::path(commandFATToolsExtract.get<std::string>("--fatdata"));
			std::filesystem::path pathFATNameTable = std::filesystem::path(commandFATToolsExtract.get<std::string>("--fatnametable"));
			uint32_t addressFATData = commandFATToolsExtract.get<uint32_t>("--fatdataaddress");
			bool saveFATFileIDs = commandFATToolsExtract.get<bool>("--savefatfileids");
			std::filesystem::path pathDirFATData = std::filesystem::path(commandFATToolsExtract.get<std::string>("--fatdatadir"));

			if (!silent) std::cout << std::format("Extracting FAT Data \"{}\"...\n", pathFATData.string()) << std::endl;
			if (std::filesystem::exists(pathDirFATData))
			{
				if (!std::filesystem::is_directory(pathDirFATData))
				{
					std::cout << std::format("Given path for extracted FAT Data already exists, but is not a directory?! Path: \"{}\"", pathDirFATData.string()) << std::endl;
					return -1;
				}
			}
			else
			{
				if (!silent) std::cout << std::format("Directory \"{}\" does not exist.", pathDirFATData.string()) << std::endl << "Creating..." << std::endl;
				std::filesystem::create_directories(pathDirFATData);
				if (!silent) std::cout << "Done" << std::endl << std::endl;
				if (!silent) std::cout << "Actually extracting..." << std::endl;
			}

			nfResult = ndsFactory.extractFatData(pathFATData.string(), pathFAT.string(), pathFATNameTable.string(), addressFATData, pathDirFATData.string(), saveFATFileIDs);
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(-1);
				return -1;
			}

			if (!silent) std::cout << "Done extracting!" << std::endl;
		}
		else if (commandFATTools.is_subcommand_used("build"))
		{
			if (commandFATToolsBuild.present("--fatoriginal")) pathFAT = std::filesystem::path(commandFATToolsBuild.get<std::string>("--fatoriginal"));
			std::filesystem::path pathDirFATData = std::filesystem::path(commandFATToolsBuild.get<std::string>("--fatdatadir"));
			uint32_t addressFATData = commandFATToolsBuild.get<uint32_t>("--fatdataaddress");
			std::filesystem::path pathDirFATOut = std::filesystem::path(commandFATToolsBuild.get<std::string>("--fatdatadirout"));

			if (std::filesystem::exists(pathDirFATData))
			{
				if (!std::filesystem::is_directory(pathDirFATData))
				{
					std::cout << "Given path for FAT Data exists, but is not a directory?!" << std::endl << "Path: " << pathDirFATData.string() << std::endl;
					std::exit(-1);
					return -1;
				}
			}
			else
			{
				std::cout << "Given directory for FAT Data does not exist!" << std::endl << "Path: " << pathDirFATData.string() << std::endl;
				std::exit(-1);
				return -1;
			}

			if (!std::filesystem::exists(pathDirFATOut))
			{
				if (!silent) std::cout << "Directory \"" << pathDirFATOut.string() << "\" does not exist." << std::endl << "Creating..." << std::endl;
				std::filesystem::create_directories(pathDirFATOut);
				if (!silent) std::cout << "Done" << std::endl << std::endl;
			}
			else
			{
				if (!std::filesystem::is_directory(pathDirFATOut))
				{
					std::cout << "Given path for FAT Out exists, but is not a directory?!" << std::endl << "Path: " << pathDirFATOut.string() << std::endl;
					std::exit(-1);
					return -1;
				}
			}

			if (!silent) std::cout << std::format("Building new FATs to directory \"{}\"...", pathDirFATOut.string()) << std::endl << std::endl;
			nfResult = ndsFactory.buildFatData(pathDirFATData.string(), pathFAT.string(), addressFATData, pathDirFATOut.string());
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(-1);
				return -1;
			}

			if (!silent) std::cout << "Done building!" << std::endl;
		}
		else if (commandFATTools.is_subcommand_used("patch"))
		{
			pathFAT = std::filesystem::path(commandFATToolsPatcher.get<std::string>("--fat"));
			uint32_t addrOriginalFATData = commandFATToolsPatcher.get<uint32_t>("--originalfatdataaddr");
			uint32_t addrNewFATData = commandFATToolsPatcher.get<std::uint32_t>("--newfatdataaddr");
			std::filesystem::path pathFATNew = std::filesystem::path(commandFATToolsPatcher.get<std::string>("--newfat"));

			uint32_t posDiff = posDiff = addrOriginalFATData - addrNewFATData;
			if (addrOriginalFATData < addrNewFATData) posDiff = addrNewFATData - addrOriginalFATData;

			if (!silent) std::cout << std::format("Patching FAT Data file \"{}\"...", pathFAT.string()) << std::endl << std::endl;
			nfResult = ndsFactory.patchFat(pathFAT.string(), posDiff, pathFATNew.string());
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				return -1;
			}

			if (!silent) std::cout << "Done patching!" << std::endl;
		}
		else
		{
			std::cout << "Bad subcommand used for fattools?!" << std::endl << std::endl;
			std::cout << program << std::endl;
			std::exit(3);
			return 3;
		}
	}
	else
	{
		std::cout << program << std::endl;
	}
	return 0;
}
