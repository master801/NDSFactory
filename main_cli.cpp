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

static int packROM(NDSFactory* ndsFactory, fs::path pathROMOut, int pad, bool trim, bool shouldFixCRC, fs::path pathHeader, fs::path pathARM9, fs::path pathARM7, fs::path pathFATNameTable, fs::path pathFAT, fs::path pathFATData, fs::path pathARM9Overlay, fs::path pathARM9OverlayData, fs::path pathARM7Overlay, fs::path pathARM7OverlayData, fs::path pathLogos)
{
	NFResult nfResult;

	std::vector<char> romHeaderBuffer(sizeof(NDSHeader));
	nfResult = ndsFactory->loadRomHeader(pathHeader.string(), romHeaderBuffer);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return 1;
	}
	
	NDSHeader* ndsHeader = reinterpret_cast<NDSHeader*>(romHeaderBuffer.data());

	//TODO Add arguments to modify the ROM header

	if (shouldFixCRC)
	{
		ndsHeader->HeaderCRC16 = ndsFactory->calcHeaderCrc16(romHeaderBuffer);
		std::cout << "Fixed header CRC16" << std::endl;
	}

	bool hasARM9Footer = ndsFactory->checkArm9FooterPresence(pathARM9.string(), ndsHeader->Arm9Size);

	char paddingType;
	if (pad == 0)
	{
		paddingType = '\x00';
	}
	else if (pad == 1)
	{
		paddingType = '\xFF';
	}
	
	uint32_t addrStart;
	uint32_t size;

	//Write header
	nfResult = ndsFactory->writeBytesToFile(romHeaderBuffer, pathROMOut.string(), 0, sizeof(NDSHeader));
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write header padding
	addrStart = sizeof(NDSHeader);
	size = ndsHeader->Arm9RomAddr - addrStart;
	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM9 bin
	addrStart = ndsHeader->Arm9RomAddr;
	size = ndsHeader->Arm9Size + (hasARM9Footer ? Arm9FooterSize : 0);
	nfResult = ndsFactory->writeSectionToFile(pathARM9.string(), pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM9 padding
	addrStart = ndsHeader->Arm9RomAddr + ndsHeader->Arm9Size;
	if (ndsHeader->Arm9OverlayAddr)
	{
		size = ndsHeader->Arm9OverlayAddr - addrStart;
	}
	else
	{
		size = ndsHeader->Arm7RomAddr - addrStart;
	}
	if (hasARM9Footer)
	{
		addrStart += Arm9FooterSize;
		size -= Arm9FooterSize;
	}
	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM9 overlay
	if (ndsHeader->Arm9OverlayAddr)
	{
		//TODO
		std::cout << "Hit!";
	}

	//Write ARM7
	nfResult = ndsFactory->writeSectionToFile(pathARM7.string(), pathROMOut.string(), ndsHeader->Arm7RomAddr, ndsHeader->Arm7Size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM7 padding
	addrStart = ndsHeader->Arm7RomAddr + ndsHeader->Arm7Size;
	if (ndsHeader->Arm7OverlayAddr)
	{
		size = ndsHeader->Arm7OverlayAddr - addrStart;
	}
	else
	{
		size = ndsHeader->FilenameTableAddr - addrStart;
	}
	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM7 overlay
	if (ndsHeader->Arm7OverlayAddr)
	{
		//TODO
		std::cout << "Hit!";
	}

	//Write FAT Name Table
	nfResult = ndsFactory->writeSectionToFile(pathFATNameTable.string(), pathROMOut.string(), ndsHeader->FilenameTableAddr, ndsHeader->FilenameSize);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write FAT Name Table padding
	addrStart = ndsHeader->FilenameTableAddr + ndsHeader->FilenameSize;
	size = ndsHeader->FATAddr - addrStart;
	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write FAT
	nfResult = ndsFactory->writeSectionToFile(pathFAT.string(), pathROMOut.string(), ndsHeader->FATAddr, ndsHeader->FATSize);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write FAT padding
	addrStart = ndsHeader->FATAddr + ndsHeader->FATSize;
	size = ndsHeader->IconTitleAddr - addrStart;
	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write Icon / Title logo
	nfResult = ndsFactory->writeSectionToFile(pathLogos.string(), pathROMOut.string(), ndsHeader->IconTitleAddr, IconTitleSize);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write FAT Data
	addrStart = ndsHeader->IconTitleAddr + IconTitleSize;
	size = ndsHeader->RomSize - addrStart;
	nfResult = ndsFactory->writeSectionToFile(pathFATData.string(), pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ROM padding
	if (!trim)
	{
		addrStart = ndsHeader->RomSize;
		size = ndsFactory->getCardSizeInBytes(ndsHeader->DeviceSize) - addrStart;
		nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
	}

	return 0;
}

static void unpackROM(const fs::path fpROM, NDSFactory* ndsFactory, NDSHeader* ndsHeader, bool overdumpARM9, fs::path pathHeader, fs::path pathARM9, fs::path pathARM7, fs::path pathFATNameTable, fs::path pathFAT, fs::path pathFATData, fs::path pathARM9Overlay, fs::path pathARM9OverlayData, fs::path pathARM7Overlay, fs::path pathARM7OverlayData, fs::path pathLogos)
{
	NFResult nfResult;

	if (!(pathHeader.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathHeader.string(), 0, ndsHeader->HeaderSize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped Header to \"{}\"", pathHeader.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}

	if (!(pathARM9.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9.string(), ndsHeader->Arm9RomAddr, ndsHeader->Arm9Size + (overdumpARM9 ? Arm9FooterSize : 0));
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM9 to \"{}\"", pathARM9.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}

	if (!(pathARM7.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7.string(), ndsHeader->Arm7RomAddr, ndsHeader->Arm7Size);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM7 to \"{}\"", pathARM7.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}
	if (!(pathFATNameTable.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFATNameTable.string(), ndsHeader->FilenameTableAddr, ndsHeader->FilenameSize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped FAT Name Table to \"{}\"", pathFATNameTable.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}
	if (!(pathFAT.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFAT.string(), ndsHeader->FATAddr, ndsHeader->FATSize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped FAT to \"{}\"", pathFAT.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}

	uint32_t startAddrFATData = ndsHeader->IconTitleAddr + IconTitleSize;
	uint32_t sizeFATData = ndsHeader->RomSize - startAddrFATData;
	if (!(pathFATData.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFATData.string(), startAddrFATData, sizeFATData);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped FAT Data to \"{}\"", pathFATData.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}
	if (!(pathARM9Overlay.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9Overlay.string(), ndsHeader->Arm9OverlayAddr, ndsHeader->Arm9OverlaySize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM9 Overlay to \"{}\"", pathARM9Overlay.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}

	uint32_t startAddrARM9OverlayData = ndsHeader->Arm9OverlayAddr + ndsHeader->Arm9OverlaySize;
	uint32_t sizeARM9OverlayData = ndsHeader->FilenameTableAddr - startAddrARM9OverlayData;
	if (!(pathARM9OverlayData.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9OverlayData.string(), startAddrARM9OverlayData, sizeARM9OverlayData);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM9 Overlay Data to \"{}\"", pathARM9OverlayData.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}
	if (!(pathARM7Overlay.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7Overlay.string(), ndsHeader->Arm7OverlayAddr, ndsHeader->Arm7OverlaySize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM7 Overlay to \"{}\"", pathARM7Overlay.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}

	uint32_t startAddrARM7OverlayData = ndsHeader->Arm7OverlayAddr + ndsHeader->Arm7OverlaySize;
	uint32_t sizeARM7OverlayData = ndsHeader->FilenameTableAddr - startAddrARM7OverlayData;
	if (!(pathARM7OverlayData.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7OverlayData.string(), startAddrARM7OverlayData, sizeARM7OverlayData);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped ARM7 Overlay Data to \"{}\"", pathARM7OverlayData.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
	}
	if (!(pathLogos.empty()))
	{
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathLogos.string(), ndsHeader->IconTitleAddr, IconTitleSize);
		if (nfResult.result)
		{
			std::cout << std::format("Dumped Logos to \"{}\"", pathLogos.string()) << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
		}
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
	//Set up commands
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
		.required()
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
		.default_value(true)
		.implicit_value(false)
		.required()
		.help("Fixes the ROM's header CRC.\nCommerical ROMs will not load if this is not enabled.\nEnabled by default.");

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

	program.add_subparser(commandUnpack);
	program.add_subparser(commandPack);
	program.add_subparser(commandFATTools);
	commandFATTools.add_subparser(commandFATToolsExtract);
	commandFATTools.add_subparser(commandFATToolsBuild);
	commandFATTools.add_subparser(commandFATToolsPatcher);

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

	NDSFactory ndsFactory;
	NFResult nfResult;
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
			fs::path pathHeader;
			fs::path pathARM9;
			fs::path pathARM7;
			fs::path pathFATNameTable;
			fs::path pathFAT;
			fs::path pathFATData;
			fs::path pathARM9Overlay;
			fs::path pathARM9OverlayData;
			fs::path pathARM7Overlay;
			fs::path pathARM7OverlayData;
			fs::path pathLogos;

			if (program.is_subcommand_used("unpack"))
			{
				std::cout << std::format("Unpacking \"{}\"...", pathROM.string()) << std::endl << std::endl;

				std::vector<char> romHeader;
				nfResult = ndsFactory.loadRomHeader(fs::absolute(pathROM).string(), romHeader);
				if (!nfResult.result)
				{
					std::cout << nfResult.message << std::endl;
					std::exit(-1);
					return -1;
				}
				NDSHeader* ndsHeader = reinterpret_cast<NDSHeader*>(romHeader.data());

				if (commandUnpack.present("--header")) pathHeader = fs::path(commandUnpack.get<std::string>("--header"));
				if (commandUnpack.present("--arm9")) pathARM9 = fs::path(commandUnpack.get<std::string>("--arm9"));
				if (commandUnpack.present("--arm7")) pathARM7 = fs::path(commandUnpack.get<std::string>("--arm7"));
				if (commandUnpack.present("--fatnametable")) pathFATNameTable = fs::path(commandUnpack.get<std::string>("--fatnametable"));
				if (commandUnpack.present("--fat")) pathFAT = fs::path(commandUnpack.get<std::string>("--fat"));
				if (commandUnpack.present("--fatdata")) pathFATData = fs::path(commandUnpack.get<std::string>("--fatdata"));
				if (commandUnpack.present("--arm9overlay")) pathARM9Overlay = fs::path(commandUnpack.get<std::string>("--arm9overlay"));
				if (commandUnpack.present("--arm9overlaydata")) pathARM9OverlayData = fs::path(commandUnpack.get<std::string>("--arm9overlaydata"));
				if (commandUnpack.present("--arm7overlay")) pathARM7Overlay = fs::path(commandUnpack.get<std::string>("--arm7overlay"));
				if (commandUnpack.present("--arm7overlaydata")) pathARM7OverlayData = fs::path(commandUnpack.get<std::string>("--arm7overlaydata"));
				if (commandUnpack.present("--logos")) pathLogos = fs::path(commandUnpack.get<std::string>("--logos"));

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

				if (!pathARM9OverlayData.empty() && pathARM9Overlay.empty())
				{
					std::cout << "Was given ARM9 Overlay Data, but missing ARM9 Overlay argument!" << std::endl << std::endl;
					std::cout << program << std::endl;
					std::exit(2);
					return 2;
				}
				if (!pathARM7OverlayData.empty() && pathARM7Overlay.empty())
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

				std::cout << std::endl << "Done unpacking!" << std::endl;
			}
			else if (program.is_subcommand_used("pack"))
			{
				pathHeader = fs::path(commandPack.get<std::string>("--header"));
				pathARM9 = fs::path(commandPack.get<std::string>("--arm9"));
				pathARM7 = fs::path(commandPack.get<std::string>("--arm7"));
				pathFATNameTable = fs::path(commandPack.get<std::string>("--fatnametable"));
				pathFAT = fs::path(commandPack.get<std::string>("--fat"));
				pathFATData = fs::path(commandPack.get<std::string>("--fatdata"));
				if (commandPack.present("--arm9overlay")) pathARM9Overlay = fs::path(commandPack.get<std::string>("--arm9overlay"));
				if (commandPack.present("--arm9overlaydata")) pathARM9OverlayData = fs::path(commandPack.get<std::string>("--arm9overlaydata"));
				if (commandPack.present("--arm7overlay")) pathARM7Overlay = fs::path(commandPack.get<std::string>("--arm7overlay"));
				if (commandPack.present("--arm7overlaydata")) pathARM7OverlayData = fs::path(commandPack.get<std::string>("--arm7overlaydata"));
				if (commandPack.present("--logos")) pathLogos = fs::path(commandPack.get<std::string>("--logos"));

				int pad = commandPack.get<int>("--pad");
				bool trim = commandPack.get<bool>("--trim");
				bool shouldFixCRC = commandPack.get<bool>("--fixcrc");

				int statusCode = packROM(
					&ndsFactory,
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
					std::cout << "Done packing!" << std::endl;
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
		else
		{
			std::vector<char> romHeader;
			nfResult = ndsFactory.loadRomHeader(fs::absolute(pathROM).string(), romHeader);
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
	}
	else if (program.is_subcommand_used("fattools"))
	{
		fs::path pathFAT;
		if (commandFATTools.is_subcommand_used("extract"))
		{
			pathFAT = fs::path(commandFATToolsExtract.get<std::string>("--fat"));
			fs::path pathFATData = fs::path(commandFATToolsExtract.get<std::string>("--fatdata"));
			fs::path pathFATNameTable = fs::path(commandFATToolsExtract.get<std::string>("--fatnametable"));
			uint32_t addressFATData = commandFATToolsExtract.get<uint32_t>("--fatdataaddress");
			bool saveFATFileIDs = commandFATToolsExtract.get<bool>("--savefatfileids");
			fs::path pathDirFATData = fs::path(commandFATToolsExtract.get<std::string>("--fatdatadir"));

			std::cout << std::format("Extracting FAT Data \"{}\"...\n", pathFATData.string()) << std::endl;
			if (fs::exists(pathDirFATData))
			{
				if (!fs::is_directory(pathDirFATData))
				{
					std::cout << std::format("Given path for extracted FAT Data already exists, but is not a directory?! Path: \"{}\"", pathDirFATData.string()) << std::endl;
					return -1;
				}
			}
			else
			{
				std::cout << std::format("Directory \"{}\" does not exist.", pathDirFATData.string()) << std::endl << "Creating..." << std::endl;
				fs::create_directories(pathDirFATData);
				std::cout << "Done" << std::endl << std::endl;
				std::cout << "Actually extracting..." << std::endl;
			}
			nfResult = ndsFactory.extractFatData(pathFATData.string(), pathFAT.string(), pathFATNameTable.string(), addressFATData, pathDirFATData.string(), saveFATFileIDs);

			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(-1);
				return -1;
			}

			std::cout << "Done extracting!" << std::endl;
		}
		else if (commandFATTools.is_subcommand_used("build"))
		{
			if (commandFATToolsBuild.present("--fatoriginal")) pathFAT = fs::path(commandFATToolsBuild.get<std::string>("--fatoriginal"));
			fs::path pathDirFATData = fs::path(commandFATToolsBuild.get<std::string>("--fatdatadir"));
			uint32_t addressFATData = commandFATToolsBuild.get<uint32_t>("--fatdataaddress");
			fs::path pathDirFATOut = fs::path(commandFATToolsBuild.get<std::string>("--fatdatadirout"));

			if (fs::exists(pathDirFATData))
			{
				if (!fs::is_directory(pathDirFATData))
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

			std::cout << std::format("Building new FAT Data file \"{}\"...", pathDirFATOut.string()) << std::endl << std::endl;
			nfResult = ndsFactory.buildFatData(pathDirFATData.string(), pathFAT.string(), addressFATData, pathDirFATOut.string());
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(-1);
				return -1;
			}

			std::cout << "Done building!" << std::endl;
		}
		else if (commandFATTools.is_subcommand_used("patch"))
		{
			pathFAT = fs::path(commandFATToolsPatcher.get<std::string>("--fat"));
			uint32_t addrOriginalFATData = commandFATToolsPatcher.get<uint32_t>("--originalfatdataaddr");
			uint32_t addrNewFATData = commandFATToolsPatcher.get<std::uint32_t>("--newfatdataaddr");
			fs::path pathFATNew = fs::path(commandFATToolsPatcher.get<std::string>("--newfat"));

			uint32_t posDiff = posDiff = addrOriginalFATData - addrNewFATData;
			if (addrOriginalFATData < addrNewFATData) posDiff = addrNewFATData - addrOriginalFATData;

			std::cout << std::format("Patching FAT Data file \"{}\"...", pathFAT.string()) << std::endl << std::endl;
			nfResult = ndsFactory.patchFat(pathFAT.string(), posDiff, pathFATNew.string());

			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				return -1;
			}

			std::cout << "Done patching!" << std::endl;
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
		return 0;
	}
	return 0;
}
