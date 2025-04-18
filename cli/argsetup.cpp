/*
* Created 4/18/2025
* Author: Master801
*/

#include <argparse/argparse.hpp>

void setUpArguments(
	argparse::ArgumentParser& program,
	argparse::ArgumentParser& argInfo,
	argparse::ArgumentParser& argUnpack,
	argparse::ArgumentParser& argPack,
	argparse::ArgumentParser& argFATTools,
	argparse::ArgumentParser& argFATToolsExtract,
	argparse::ArgumentParser& argFATToolsBuild,
	argparse::ArgumentParser& argFATToolsPatch
)
{
	//Info
	argInfo.add_description("Gets info from the given ROM");
	argInfo
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("ROM file to get info from");

	//Unpack ROM
	argUnpack.add_description("Unpack the target ROM file");
	argUnpack
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("ROM file to unpack");
	argUnpack
		.add_argument("--header", "-h")
		.metavar("<header.bin>")
		.help("Header file");
	argUnpack
		.add_argument("--arm9", "-a9")
		.metavar("<arm9.bin>")
		.help("ARM9 executable");
	argUnpack
		.add_argument("--arm9overdump")
		.flag()
		.required()
		.help("Dumps 12 extra bytes for a 1:1 file");
	argUnpack
		.add_argument("--arm7", "-a7")
		.metavar("<arm7.bin>")
		.help("ARM7 executable");
	argUnpack
		.add_argument("--fatnametable", "-fntbl")
		.metavar("<fnt.bin>")
		.help("FAT name table");
	argUnpack
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.help("FAT file");
	argUnpack
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.help("FAT data file");
	argUnpack
		.add_argument("--arm9overlay", "-a9o")
		.metavar("<a9ovr.bin>")
		.help("ARM9 overlay file");
	argUnpack
		.add_argument("--arm9overlaydata", "-a9od")
		.metavar("<a9ovr_data.bin>")
		.help("ARM9 overlay data file");
	argUnpack
		.add_argument("--arm7overlay", "-a7o")
		.metavar("<a7ovr.bin>")
		.help("ARM7 overlay file");
	argUnpack
		.add_argument("--arm7overlaydata", "-a7od")
		.metavar("<a7ovr_data.bin>")
		.help("ARM7 overlay data file");
	argUnpack
		.add_argument("--logos", "-l")
		.metavar("<itl.bin>")
		.help("Icon / Title Logos file");

	//Pack ROM
	argPack.add_description("Pack a ROM file from extracted files");
	argPack
		.add_argument("--rom", "-r")
		.metavar("<ROM>")
		.required()
		.help("Path where the packed ROM will be");
	argPack
		.add_argument("--header", "-h")
		.metavar("<header.bin>")
		.required()
		.help("Header file");
	argPack
		.add_argument("--arm9", "-a9")
		.metavar("<arm9.bin>")
		.required()
		.help("ARM9 executable");
	argPack
		.add_argument("--arm7", "-a7")
		.metavar("<arm7.bin>")
		.required()
		.help("ARM7 executable");
	argPack
		.add_argument("--fatnametable", "-fntbl")
		.metavar("<fnt.bin>")
		.required()
		.help("FAT name table");
	argPack
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.required()
		.help("FAT file");
	argPack
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.required()
		.help("FAT data file");
	argPack
		.add_argument("--arm9overlay", "-a9o")
		.metavar("<a9ovr.bin>")
		.help("ARM9 overlay file");
	argPack
		.add_argument("--arm9overlaydata", "-a9od")
		.metavar("<a9ovr_data.bin>")
		.help("ARM9 overlay data file");
	argPack
		.add_argument("--arm7overlay", "-a7o")
		.metavar("<a7ovr.bin>")
		.help("ARM7 overlay file");
	argPack
		.add_argument("--arm7overlaydata", "-a7od")
		.metavar("<a7ovr_data.bin>")
		.help("ARM7 overlay data file");
	argPack
		.add_argument("--logos", "-l")
		.metavar("<itl.bin>")
		.help("Icon / Title Logos file");
	argPack
		.add_argument("--pad", "-p")
		.metavar("<0 or 1>")
		.default_value(1)
		.choices(0, 1)
		.scan<'d', int>()
		.required()
		.help("0 to pad ROM with 0x00s\n1 to pad ROM with 0xFFs\nMost commercial ROMs are padded with 0xFF\nBy default, pads with 0xFF\n");
	argPack
		.add_argument("--trim", "-t")
		.flag()
		.required()
		.help("Trim ROM after packing.\nBy default, the ROM is not trimmed.\n");
	argPack
		.add_argument("--fixcrc", "-crc")
		.flag()
		.required()
		.help("Fixes the ROM's header CRC16.\nCommerical ROMs will not load if this is not enabled.\nDisabled by default.");

	//FAT Tools
	argFATTools.add_description("Tools for modifying the FAT");
	argFATTools
		.add_argument("extract")
		.flag()
		.help("Extracts FAT data <fat_data.bin>");
	argFATTools
		.add_argument("build")
		.flag()
		.help("Builds FAT data <fat_data.bin>");
	argFATTools
		.add_argument("patch")
		.flag()
		.help("Patches FAT data <fat_data.bin>");

	//FAT Tools Extract
	argFATToolsExtract
		.add_argument("--fat", "-f")
		.metavar("<fat.bin>")
		.required()
		.help("FAT bin file");
	argFATToolsExtract
		.add_argument("--fatdata", "-fd")
		.metavar("<fat_data.bin>")
		.required()
		.help("FAT data bin file");
	argFATToolsExtract
		.add_argument("--fatnametable", "-fnt")
		.metavar("<fnt.bin>")
		.required()
		.help("FAT name table");
	argFATToolsExtract
		.add_argument("--fatdataaddress", "-fdaddr")
		.scan<'X', uint32_t>()
		.metavar("<Hex Integer>")
		.required()
		.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");
	argFATToolsExtract
		.add_argument("--savefatfileids", "-saveids")
		.flag()
		.required()
		.help("Save file IDs to _file_IDs.txt (required for rebuilding FAT)");
	argFATToolsExtract
		.add_argument("--fatdatadir", "-fats")
		.metavar("<fat_data>")
		.required()
		.help("The directory to output the extract FAT files to.");

	//FAT Tools Build
	argFATToolsBuild
		.add_argument("--fatoriginal", "-fo")
		.metavar("<fat.bin>")
		.help("Original <fat.bin> file\nOnly required if the ROM uses overlays");
	argFATToolsBuild
		.add_argument("--fatdatadir", "-fats")
		.metavar("<fat_data.bin>")
		.required()
		.help("Directory where <fat_data.bin> was extracted to");
	argFATToolsBuild
		.add_argument("--fatdataaddress", "-fdaddr")
		.scan<'X', uint32_t>()
		.metavar("<Hex Integer>")
		.required()
		.help("Original address of FAT data in Hex.\nEx: 0xDEADBEEF");
	argFATToolsBuild
		.add_argument("--fatdatadirout", "-fddo")
		.required()
		.help("The directory to write the new <fat.bin> and <fat_data.bin> to.");

	//FAT Tools Patch
	argFATToolsPatch
		.add_argument("--fat", "-f")
		.required()
		.metavar("<fat.bin>")
		.help("FAT bin file to patch");
	argFATToolsPatch
		.add_argument("--originalfatdataaddr", "-ofdaddr")
		.scan<'X', uint32_t>()
		.required()
		.metavar("<Hex Integer>")
		.help("Original address of the FAT Data in Hex.\nEx: 0xDEADBEEF");
	argFATToolsPatch
		.add_argument("--newfatdataaddr", "-nfdaddr")
		.scan<'X', uint32_t>()
		.required()
		.metavar("<Hex Integer>")
		.help("New address of the FAT Data in Hex.\nEx: 0xDEADBEEF");
	argFATToolsPatch
		.add_argument("--newfat", "-nf")
		.required()
		.metavar("<fat.new.bin>")
		.help("The new patched FAT bin file");


	//Add the arguments to the main "program"
	program.add_subparser(argInfo);
	program.add_subparser(argUnpack);
	program.add_subparser(argPack);

	argFATTools.add_subparser(argFATToolsExtract);
	argFATTools.add_subparser(argFATToolsBuild);
	argFATTools.add_subparser(argFATToolsPatch);
	program.add_subparser(argFATTools);
	program.add_subparser(argInfo);
}
