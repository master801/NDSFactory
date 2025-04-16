/*
* Created 4/10/2025
* Author: Master801
*/

#include <iostream>
#include <format>
#include <filesystem>
namespace fs = std::filesystem;

#include <argparse/argparse.hpp>

#include "ndsfactory/ndsfactory.h"
#include "ndsfactory/ndsheader.h"

static void fatToolsPatch(NDSFactory* ndsFactory)
{
}

static void fatToolsBuild(NDSFactory* ndsFactory)
{
}

static void fatToolsDataExtract(NDSFactory* ndsFactory)
{
}

static void fatTools(NDSFactory* ndsFactory)
{
	//fatToolsDataExtract();
	//fatToolsBuild();
	//fatToolsPatch();
}

static void generateHeader(NDSHeader* ndsHeader)
{
}

static void packROM(NDSFactory* ndsFactory, fs::path fpROMOut, int pad, bool trim, fs::path fpHeader, fs::path fpARM9, fs::path fpARM7, fs::path fpFATNameTable, fs::path fpFAT, fs::path fpFATData, fs::path fpARM9Overlay, fs::path fpARM9OverlayData, fs::path fpARM7Overlay, fs::path fpARM7OverlayData, fs::path fpLogos)
{
}

static void unpackROM(const fs::path fpROM, NDSFactory* ndsFactory, NDSHeader* ndsHeader, bool overdumpARM9, fs::path fpHeader, fs::path fpARM9, fs::path fpARM7, fs::path fpFATNameTable, fs::path fpFAT, fs::path fpFATData, fs::path fpARM9Overlay, fs::path fpARM9OverlayData, fs::path fpARM7Overlay, fs::path fpARM7OverlayData, fs::path fpLogos)
{
	if (!(fpHeader.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpHeader.string(), 0, ndsHeader->HeaderSize);
		std::cout << std::format("Dumped Header to \"{}\"", fpHeader.string()) << std::endl;
	}

	uint32_t sizeARM9 = ndsHeader->Arm9Size;
	if (overdumpARM9) sizeARM9 += 12;
	if (!(fpARM9.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM9.string(), ndsHeader->Arm9RomAddr, sizeARM9);
		std::cout << std::format("Dumped ARM9 to \"{}\"", fpARM9.string()) << std::endl;
	}

	if (!(fpARM7.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM7.string(), ndsHeader->Arm7RomAddr, ndsHeader->Arm7Size);
		std::cout << std::format("Dumped ARM7 to \"{}\"", fpARM7.string()) << std::endl;
	}
	if (!(fpFATNameTable.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpFATNameTable.string(), ndsHeader->FilenameTableAddr, ndsHeader->FilenameSize);
		std::cout << std::format("Dumped FAT Name Table to \"{}\"", fpFATNameTable.string()) << std::endl;
	}
	if (!(fpFAT.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpFAT.string(), ndsHeader->FATAddr, ndsHeader->FATSize);
		std::cout << std::format("Dumped FAT to \"{}\"", fpFAT.string()) << std::endl;
	}

	uint32_t startAddrFATData = ndsHeader->IconTitleAddr + IconTitleSize;
	uint32_t sizeFATData = ndsHeader->RomSize - startAddrFATData;
	if (!(fpFATData.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpFATData.string(), startAddrFATData, sizeFATData);
		std::cout << std::format("Dumped FAT Data to \"{}\"", fpFATData.string()) << std::endl;
	}
	if (!(fpARM9Overlay.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM9Overlay.string(), ndsHeader->Arm9OverlayAddr, ndsHeader->Arm9OverlaySize);
		std::cout << std::format("Dumped ARM9 Overlay to \"{}\"", fpARM9Overlay.string()) << std::endl;
	}

	uint32_t startAddrARM9OverlayData = ndsHeader->Arm9OverlayAddr + ndsHeader->Arm9OverlaySize;
	uint32_t sizeARM9OverlayData = ndsHeader->FilenameTableAddr - startAddrARM9OverlayData;
	if (!(fpARM9OverlayData.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM9OverlayData.string(), startAddrARM9OverlayData, sizeARM9OverlayData);
		std::cout << std::format("Dumped ARM9 Overlay Data to \"{}\"", fpARM9OverlayData.string()) << std::endl;
	}
	if (!(fpARM7Overlay.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM7Overlay.string(), ndsHeader->Arm7OverlayAddr, ndsHeader->Arm7OverlaySize);
		std::cout << std::format("Dumped ARM7 Overlay to \"{}\"", fpARM7Overlay.string()) << std::endl;
	}

	uint32_t startAddrARM7OverlayData = ndsHeader->Arm7OverlayAddr + ndsHeader->Arm7OverlaySize;
	uint32_t sizeARM7OverlayData = ndsHeader->FilenameTableAddr - startAddrARM7OverlayData;
	if (!(fpARM7OverlayData.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpARM7OverlayData.string(), startAddrARM7OverlayData, sizeARM7OverlayData);
		std::cout << std::format("Dumped ARM7 Overlay Data to \"{}\"", fpARM7OverlayData.string()) << std::endl;
	}
	if (!(fpLogos.empty()))
	{
		ndsFactory->dumpDataFromFile(fpROM.string(), fpLogos.string(), ndsHeader->IconTitleAddr, IconTitleSize);
		std::cout << std::format("Dumped Logos to \"{}\"", fpLogos.string()) << std::endl;
	}
}

static void printInfo(NDSHeader* ndsHeader)
{
	std::cout << std::format("Used ROM Size: 0x{:08X}", ndsHeader->RomSize) << std::endl;

	std::cout << std::format("Header Size: 0x{:04X}", ndsHeader->HeaderSize) << std::endl;
	std::cout << std::format("Header CRC16: 0x{:04X}", ndsHeader->HeaderCRC16) << std::endl;

	std::string stringHexNinLogo;
	for (int i = 0; i < sizeof(ndsHeader->NintendoLogo); i++)
	{
		stringHexNinLogo += std::format("{:02X}", ndsHeader->NintendoLogo[i]);
	}
	std::cout << "Nintendo Logo: 0x" << stringHexNinLogo << std::endl;
	//delete &stringHexNinLogo;
	std::cout << std::format("Nintendo CRC: 0x{:X}", ndsHeader->NintendoLogoCRC) << std::endl;

	std::cout << std::format("Game Title: {}", std::string(ndsHeader->GameTitle)) << std::endl;
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
}

int main(int argc, char* argv[])
{
	argparse::ArgumentParser program("NDSFactory");

	program
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.help("Target ROM file");

	argparse::ArgumentParser commandUnpack("unpack");
	commandUnpack.add_description("Unpack the target ROM file");
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
		.default_value(true)
		.implicit_value(false)
		.help("Dumps 12 extra bytes for a 1:1 file.\nEnabled by default\nUse this argument if you do not want this behavior to happen.");
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
		.add_argument("--header", "-h")
		.metavar("<header.bin>")
		.help("Header file");
	commandPack
		.add_argument("--arm9", "-a9")
		.metavar("<arm9.bin>")
		.help("ARM9 executable");
	commandPack
		.add_argument("--arm7", "-a7")
		.metavar("<arm7.bin>")
		.help("ARM7 executable");
	commandPack
		.add_argument("--fatnametable", "-fntbl")
		.metavar("<fnt.bin>")
		.help("FAT name table");
	commandPack
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.help("FAT file");
	commandPack
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
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
		.scan<'d', int>()
		.choices(0, 1)
		.default_value(1)
		.help("0 to pad ROM with 0x00s\n1 to pad ROM with 0xFFs\nMost commercial ROMs are padded with 0xFF\nBy default, pads with 0xFF\n");
	commandPack
		.add_argument("--trim", "-t")
		.flag()
		.help("Trim ROM after packing.\nBy default, the ROM is not trimmed.\n");

	argparse::ArgumentParser commandFATTools("fattools");
	commandFATTools.add_description("Tools for modifying FAT");
	//groupFATTools
	//	.add_argument("--extract", "-e")
	//	.flag()
	//	.help("Extracts FAT data <fat_data.bin>");
	//groupFATTools
	//	.add_argument("--build", "-b")
	//	.flag()
	//	.help("Builds FAT data <fat_data.bin>");
	//groupFATTools
	//	.add_argument("--patch", "-p")
	//	.flag()
	//	.help("Patches FAT data <fat_data.bin>");

	//argparse::ArgumentParser& groupFATToolsExtract = groupModeArguments
	//	.add_group("FAT Data Extract");
	//groupFATToolsExtract
	//	.add_argument("--fat", "-f")
	//	.metavar("<fat.bin>")
	//	.help("FAT bin file");
	//groupFATToolsExtract
	//	.add_argument("--fatdata", "-fd")
	//	.metavar("<fat_data.bin>")
	//	.help("FAT data bin file");
	//groupFATToolsExtract
	//	.add_argument("--filenametable", "-fnt")
	//	.metavar("<fnt.bin>")
	//	.help("FAT name table");
	//groupFATToolsExtract
	//	.add_argument("--fatdataaddr", "-fdadr")
	//	//.scan<'X', int>()
	//	.metavar("<Hex Integer>")
	//	.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");
	//groupFATToolsExtract
	//	.add_argument("--fatfileids", "-ffids")
	//	.flag()
	//	.help("Save file IDs to _file_IDs.txt (required for rebuilding FAT)");

	//argparse::ArgumentParser& groupFATToolsBuilder = groupModeArguments
	//	.add_group("FAT Data Builder");
	//groupFATToolsBuilder
	//	.add_argument("--fat", "-f")
	//	.help("Original <fat.bin> file");
	//groupFATToolsBuilder
	//	.add_argument("--fatdatadir", "-fats")
	//	.help("Directory where <fat_data.bin> was extracted to");
	//groupFATToolsBuilder
	//	.add_argument("--fatdataaddr", "-fdaddr")
	//	//.scan<'X', int>()
	//	.metavar("<Hex Integer>")
	//	.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");

	//argparse::ArgumentParser& groupFATToolsPatcher = groupModeArguments
	//	.add_group("FAT Data Patcher");
	//groupFATToolsPatcher
	//	.add_argument("--fat", "-f")
	//	.metavar("<fat.bin>")
	//	.help("FAT bin file to patch");
	//groupFATToolsPatcher
	//	.add_argument("--originalfatdataaddr", "-ofdaddr")
	//	//.scan<'X', int>()
	//	.metavar("<Hex Integer>")
	//	.help("Original address of the FAT Data in Hex.\nEx: 0xDEADBEEF");
	//groupFATToolsPatcher
	//	.add_argument("--newfatdataaddr", "-nfdaddr")
	//	//.scan<'X', int>()
	//	.metavar("<Hex Integer>")
	//	.help("New address of the FAT Data in Hex.\nEx: 0xDEADBEEF");

	program.add_subparser(commandUnpack);
	program.add_subparser(commandPack);
	program.add_subparser(commandFATTools);

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	NDSFactory ndsFactory;
	if (program.present("--rom"))
	{
		ndsFactory = NDSFactory();

		fs::path pathROM = fs::path(program.get<std::string>("--rom"));
		if (!fs::exists(pathROM) && (!program.is_subcommand_used("pack") && !program.is_subcommand_used("fattools")))
		{
			std::cout << std::format("File \"{}\" does not exist!", pathROM.string()) << std::endl;
			return 404;
		}
		
		if (program.is_subcommand_used("unpack") || program.is_subcommand_used("pack"))
		{
			fs::path fpHeader;
			fs::path fpARM9;
			fs::path fpARM7;
			fs::path fpFATNameTable;
			fs::path fpFAT;
			fs::path fpFATData;
			fs::path fpARM9Overlay;
			fs::path fpARM9OverlayData;
			fs::path fpARM7Overlay;
			fs::path fpARM7OverlayData;
			fs::path fpLogos;

			//TODO Make these checks
			/*if (!fs::exists(fpHeader.parent_path()) || (fs::exists(fpHeader.parent_path()) && !fs::is_directory(fpHeader.parent_path())))
			{
				std::cout << std::format("Cannot extract header due to directory \"{}\" being non-existant!", fs::absolute(fpHeader).string()) << std::endl;
				std::exit(10);
				return 10;
			}*/
			if (program.is_subcommand_used("unpack"))
			{
				std::cout << std::format("Unpacking \"{}\"...", pathROM.string()) << std::endl << std::endl;

				std::vector<char> romHeader;
				ndsFactory.loadRomHeader(fs::absolute(pathROM).string(), romHeader);
				NDSHeader* ndsHeader = reinterpret_cast<NDSHeader*>(romHeader.data());

				if (commandUnpack.present("--header")) fpHeader = fs::path(commandUnpack.get<std::string>("--header"));
				if (commandUnpack.present("--arm9")) fpARM9 = fs::path(commandUnpack.get<std::string>("--arm9"));
				if (commandUnpack.present("--arm7")) fpARM7 = fs::path(commandUnpack.get<std::string>("--arm7"));
				if (commandUnpack.present("--fatnametable")) fpFATNameTable = fs::path(commandUnpack.get<std::string>("--fatnametable"));
				if (commandUnpack.present("--fat")) fpFAT = fs::path(commandUnpack.get<std::string>("--fat"));
				if (commandUnpack.present("--fatdata")) fpFATData = fs::path(commandUnpack.get<std::string>("--fatdata"));
				if (commandUnpack.present("--arm9overlay")) fpARM9Overlay = fs::path(commandUnpack.get<std::string>("--arm9overlay"));
				if (commandUnpack.present("--arm9overlaydata")) fpARM9OverlayData = fs::path(commandUnpack.get<std::string>("--arm9overlaydata"));
				if (commandUnpack.present("--arm7overlay")) fpARM7Overlay = fs::path(commandUnpack.get<std::string>("--arm7overlay"));
				if (commandUnpack.present("--arm7overlaydata")) fpARM7OverlayData = fs::path(commandUnpack.get<std::string>("--arm7overlaydata"));
				if (commandUnpack.present("--logos")) fpLogos = fs::path(commandUnpack.get<std::string>("--logos"));

				if (fpHeader.empty())
				{
					std::cout << "Missing header argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpARM9.empty())
				{
					std::cout << "Missing ARM9 argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpARM7.empty())
				{
					std::cout << "Missing ARM7 argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpFATNameTable.empty())
				{
					std::cout << "Missing FAT Name Table argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpFAT.empty())
				{
					std::cout << "Missing FAT argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpFATData.empty())
				{
					std::cout << "Missing FAT Data argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (fpLogos.empty())
				{
					std::cout << "Missing Icon / Title logo argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}

				if (!fpARM9OverlayData.empty() && fpARM9Overlay.empty())
				{
					std::cout << "Was given ARM9 Overlay Data, but missing ARM9 Overlay argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (!fpARM7OverlayData.empty() && fpARM7Overlay.empty())
				{
					std::cout << "Was given ARM7 Overlay Data, but missing ARM7 Overlay argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}

				unpackROM(
					pathROM,
					&ndsFactory,
					ndsHeader,
					commandUnpack.get<bool>("--arm9overdump"),
					fpHeader,
					fpARM9,
					fpARM7,
					fpFATNameTable,
					fpFAT,
					fpFATData,
					fpARM9Overlay,
					fpARM9OverlayData,
					fpARM7Overlay,
					fpARM7OverlayData,
					fpLogos
				);

				std::cout << std::endl << "Done unpacking!" << std::endl;
			}
			else if (program.is_subcommand_used("pack"))
			{
				if (commandPack.present("--header")) fpHeader = fs::path(commandPack.get<std::string>("--header"));
				if (commandPack.present("--arm9")) fpARM9 = fs::path(commandPack.get<std::string>("--arm9"));
				if (commandPack.present("--arm7")) fpARM7 = fs::path(commandPack.get<std::string>("--arm7"));
				if (commandPack.present("--fatnametable")) fpFATNameTable = fs::path(commandPack.get<std::string>("--fatnametable"));
				if (commandPack.present("--fat")) fpFAT = fs::path(commandPack.get<std::string>("--fat"));
				if (commandPack.present("--fatdata")) fpFATData = fs::path(commandPack.get<std::string>("--fatdata"));
				if (commandPack.present("--arm9overlay")) fpARM9Overlay = fs::path(commandPack.get<std::string>("--arm9overlay"));
				if (commandPack.present("--arm9overlaydata")) fpARM9OverlayData = fs::path(commandPack.get<std::string>("--arm9overlaydata"));
				if (commandPack.present("--arm7overlay")) fpARM7Overlay = fs::path(commandPack.get<std::string>("--arm7overlay"));
				if (commandPack.present("--arm7overlaydata")) fpARM7OverlayData = fs::path(commandPack.get<std::string>("--arm7overlaydata"));
				if (commandPack.present("--logos")) fpLogos = fs::path(commandPack.get<std::string>("--logos"));

				int pad = commandPack.get<int>("--pad");
				bool trim = commandPack.get<bool>("--trim");

				packROM(
					&ndsFactory,
					pathROM,
					pad,
					trim,
					fpHeader,
					fpARM9,
					fpARM7,
					fpFATNameTable,
					fpFAT,
					fpFATData,
					fpARM9Overlay,
					fpARM9OverlayData,
					fpARM7Overlay,
					fpARM7OverlayData,
					fpLogos
				);

				std::cout << "Done packing!" << std::endl;
			}
		}
		else
		{
			std::vector<char> romHeader;
			ndsFactory.loadRomHeader(fs::absolute(pathROM).string(), romHeader);
			std::cout << std::format("Info of ROM \"{}\":", pathROM.string()) << std::endl << std::endl;
			printInfo(
				reinterpret_cast<NDSHeader*>(romHeader.data())
			);
			std::exit(0);
			return 0;
		}
	}
	else if (program.is_subcommand_used("fattools"))
	{
		//TODO
	}
	else
	{
		std::cout << program << std::endl;
		return 0;
	}
	return 0;
}
