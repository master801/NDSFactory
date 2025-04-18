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

int main(int argc, char* argv[])
{
	//Set up commands
	argparse::ArgumentParser program("NDSFactory", std::format("{}-{}", PROGRAM_VERSION, GIT_COMMIT_HASH));
	program
		.add_argument("--silent", "-s")
		.flag()
		.help("Suppress status messages in the console");
	
	argparse::ArgumentParser subcommandInfo("info"), subcommandUnpack("unpack"), subcommandPack("pack");
	argparse::ArgumentParser subcommandFATTools("fattools"), subsubcommandFATToolsExtract("extract"), subsubcommandFATToolsBuild("build"), subsubcommandFATToolsPatch("patch");
	setUpArguments(
		program,
		subcommandInfo,
		subcommandUnpack,
		subcommandPack,
		subcommandFATTools,
		subsubcommandFATToolsExtract,
		subsubcommandFATToolsBuild,
		subsubcommandFATToolsPatch
	);

	//TODO Make arguments to edit header

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(EXIT_CODE_BAD_ARGUMENT);
	}

	const bool silent = program.get<const bool>("--silent");

	NDSFactory ndsFactory = NDSFactory();
	NFResult nfResult;

	std::filesystem::path pathROM;
	if (program.is_subcommand_used("info"))
	{
		pathROM = std::filesystem::path(subcommandInfo.get<std::string>("--rom"));
		if (!std::filesystem::exists(pathROM))
		{
			std::cout << std::format("ROM file \"{}\" does not exist!", pathROM.string()) << std::endl;
			std::exit(EXIT_CODE_FS_NOT_FOUND);
		}

		std::vector<char> romHeader;
		nfResult = ndsFactory.loadRomHeader(std::filesystem::absolute(pathROM).string(), romHeader);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			std::exit(EXIT_CODE_NF_RESULT_BAD);
		}
		std::cout << std::format("Info of ROM \"{}\":", pathROM.string()) << std::endl << std::endl;
		printInfo(
			reinterpret_cast<NDSHeader*>(romHeader.data())
		);
		std::exit(0);
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
			pathROM = std::filesystem::path(subcommandUnpack.get<std::string>("--rom"));
			if (!std::filesystem::exists(pathROM))
			{
				std::cout << std::format("ROM file \"{}\" does not exist!", pathROM.string()) << std::endl;
				return EXIT_CODE_FS_NOT_FOUND;
			}

			if (!silent) std::cout << std::format("Unpacking \"{}\"...", pathROM.string()) << std::endl << std::endl;

			std::vector<char> romHeader;
			nfResult = ndsFactory.loadRomHeader(std::filesystem::absolute(pathROM).string(), romHeader);
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(EXIT_CODE_NF_RESULT_BAD);
			}
			NDSHeader *ndsHeader = reinterpret_cast<NDSHeader*>(romHeader.data());

			if (subcommandUnpack.present("--header")) pathHeader = std::filesystem::path(subcommandUnpack.get<std::string>("--header"));
			if (subcommandUnpack.present("--arm9")) pathARM9 = std::filesystem::path(subcommandUnpack.get<std::string>("--arm9"));
			if (subcommandUnpack.present("--arm7")) pathARM7 = std::filesystem::path(subcommandUnpack.get<std::string>("--arm7"));
			if (subcommandUnpack.present("--fatnametable")) pathFATNameTable = std::filesystem::path(subcommandUnpack.get<std::string>("--fatnametable"));
			if (subcommandUnpack.present("--fat")) pathFAT = std::filesystem::path(subcommandUnpack.get<std::string>("--fat"));
			if (subcommandUnpack.present("--fatdata")) pathFATData = std::filesystem::path(subcommandUnpack.get<std::string>("--fatdata"));
			if (subcommandUnpack.present("--arm9overlay")) pathARM9Overlay = std::filesystem::path(subcommandUnpack.get<std::string>("--arm9overlay"));
			if (subcommandUnpack.present("--arm9overlaydata")) pathARM9OverlayData = std::filesystem::path(subcommandUnpack.get<std::string>("--arm9overlaydata"));
			if (subcommandUnpack.present("--arm7overlay")) pathARM7Overlay = std::filesystem::path(subcommandUnpack.get<std::string>("--arm7overlay"));
			if (subcommandUnpack.present("--arm7overlaydata")) pathARM7OverlayData = std::filesystem::path(subcommandUnpack.get<std::string>("--arm7overlaydata"));
			if (subcommandUnpack.present("--logos")) pathLogos = std::filesystem::path(subcommandUnpack.get<std::string>("--logos"));

			if (!pathARM9Overlay.empty() && pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay, but missing ARM9 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM9Overlay.empty() && !pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay Data, but missing ARM9 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (!pathARM7Overlay.empty() && pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay, but missing ARM7 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM7Overlay.empty() && !pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay Data, but missing ARM7 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}

			bool properlyUnpacked = unpackROM(
				pathROM,
				&ndsFactory,
				silent,

				ndsHeader,
				subcommandUnpack.get<const bool>("--arm9overdump"),
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
			pathROM = std::filesystem::path(subcommandPack.get<std::string>("--rom"));
			pathHeader = std::filesystem::path(subcommandPack.get<std::string>("--header"));
			pathARM9 = std::filesystem::path(subcommandPack.get<std::string>("--arm9"));
			pathARM7 = std::filesystem::path(subcommandPack.get<std::string>("--arm7"));
			pathFATNameTable = std::filesystem::path(subcommandPack.get<std::string>("--fatnametable"));
			pathFAT = std::filesystem::path(subcommandPack.get<std::string>("--fat"));
			pathFATData = std::filesystem::path(subcommandPack.get<std::string>("--fatdata"));
			if (subcommandPack.present("--arm9overlay")) pathARM9Overlay = std::filesystem::path(subcommandPack.get<std::string>("--arm9overlay"));
			if (subcommandPack.present("--arm9overlaydata")) pathARM9OverlayData = std::filesystem::path(subcommandPack.get<std::string>("--arm9overlaydata"));
			if (subcommandPack.present("--arm7overlay")) pathARM7Overlay = std::filesystem::path(subcommandPack.get<std::string>("--arm7overlay"));
			if (subcommandPack.present("--arm7overlaydata")) pathARM7OverlayData = std::filesystem::path(subcommandPack.get<std::string>("--arm7overlaydata"));
			if (subcommandPack.present("--logos")) pathLogos = std::filesystem::path(subcommandPack.get<std::string>("--logos"));

			const int pad = subcommandPack.get<int>("--pad");
			const bool trim = subcommandPack.get<bool>("--trim");
			const bool shouldFixCRC = subcommandPack.get<bool>("--fixcrc");

			if (pathHeader.empty())
			{
				std::cout << "Missing header!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM9.empty())
			{
				std::cout << "Missing ARM9!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM7.empty())
			{
				std::cout << "Missing ARM7!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathFATNameTable.empty())
			{
				std::cout << "Missing FAT Name Table!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathFAT.empty())
			{
				std::cout << "Missing FAT!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathFATData.empty())
			{
				std::cout << "Missing FAT Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathLogos.empty())
			{
				std::cout << "Missing Icon / Title logo!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}

			if (!pathARM9Overlay.empty() && pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay, but missing ARM9 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM9Overlay.empty() && !pathARM9OverlayData.empty())
			{
				std::cout << "Was given ARM9 Overlay Data, but missing ARM9 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (!pathARM7Overlay.empty() && pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay, but missing ARM7 Overlay Data!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}
			if (pathARM7Overlay.empty() && !pathARM7OverlayData.empty())
			{
				std::cout << "Was given ARM7 Overlay Data, but missing ARM7 Overlay!" << std::endl << std::endl;
				std::cout << program << std::endl;
				std::exit(EXIT_CODE_BAD_ARGUMENT);
			}

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
			else
			{
				std::cout << "Failed to pack ROM!" << std::endl;
				std::exit(statusCode);
			}
		}
	}
	else if (program.is_subcommand_used("fattools"))
	{
		std::filesystem::path pathFAT;
		if (subcommandFATTools.is_subcommand_used("extract"))
		{
			pathFAT = std::filesystem::path(subsubcommandFATToolsExtract.get<std::string>("--fat"));
			std::filesystem::path pathFATData = std::filesystem::path(subsubcommandFATToolsExtract.get<std::string>("--fatdata"));
			std::filesystem::path pathFATNameTable = std::filesystem::path(subsubcommandFATToolsExtract.get<std::string>("--fatnametable"));
			uint32_t addressFATData = subsubcommandFATToolsExtract.get<uint32_t>("--fatdataaddress");
			bool saveFATFileIDs = subsubcommandFATToolsExtract.get<bool>("--savefatfileids");
			std::filesystem::path pathDirFATData = std::filesystem::path(subsubcommandFATToolsExtract.get<std::string>("--fatdatadir"));

			if (!silent) std::cout << std::format("Extracting FAT Data \"{}\"...\n", pathFATData.string()) << std::endl;
			if (std::filesystem::exists(pathDirFATData))
			{
				if (!std::filesystem::is_directory(pathDirFATData))
				{
					std::cout << std::format("Given path for extracted FAT Data already exists, but is not a directory?! Path: \"{}\"", pathDirFATData.string()) << std::endl;
					std::exit(EXIT_CODE_FS_BAD_IO);
				}
			}
			else
			{
				if (!silent) std::cout << std::format("Directory \"{}\" does not exist.", pathDirFATData.string()) << std::endl << "Creating..." << std::endl;
				if (!std::filesystem::create_directories(pathDirFATData))
				{
					std::cout << std::format("Failed to create directory \"{}\"!", pathDirFATData.string()) << std::endl;
					std::exit(EXIT_CODE_FS_BAD_IO);
				}
				if (!silent) std::cout << "Done" << std::endl << std::endl;
				if (!silent) std::cout << "Actually extracting..." << std::endl;
			}

			nfResult = ndsFactory.extractFatData(pathFATData.string(), pathFAT.string(), pathFATNameTable.string(), addressFATData, pathDirFATData.string(), saveFATFileIDs);
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(EXIT_CODE_NF_RESULT_BAD);
			}

			if (!silent) std::cout << "Done extracting!" << std::endl;
		}
		else if (subcommandFATTools.is_subcommand_used("build"))
		{
			if (subsubcommandFATToolsBuild.present("--fatoriginal")) pathFAT = std::filesystem::path(subsubcommandFATToolsBuild.get<std::string>("--fatoriginal"));
			std::filesystem::path pathDirFATData = std::filesystem::path(subsubcommandFATToolsBuild.get<std::string>("--fatdatadir"));
			uint32_t addressFATData = subsubcommandFATToolsBuild.get<uint32_t>("--fatdataaddress");
			std::filesystem::path pathDirFATOut = std::filesystem::path(subsubcommandFATToolsBuild.get<std::string>("--fatdatadirout"));

			if (std::filesystem::exists(pathDirFATData))
			{
				if (!std::filesystem::is_directory(pathDirFATData))
				{
					std::cout << "Given path for FAT Data exists, but is not a directory?!" << std::endl << "Path: " << pathDirFATData.string() << std::endl;
					std::exit(EXIT_CODE_FS_BAD_IO);
				}
			}
			else
			{
				std::cout << "Given directory for FAT Data does not exist!" << std::endl << "Path: " << pathDirFATData.string() << std::endl;
				std::exit(EXIT_CODE_FS_BAD_IO);
			}

			if (!std::filesystem::exists(pathDirFATOut))
			{
				if (!silent) std::cout << "Directory \"" << pathDirFATOut.string() << "\" does not exist." << std::endl << "Creating..." << std::endl;
				if (std::filesystem::create_directories(pathDirFATOut))
				{
					std::cout << std::format("Failed to create directory \"{}\"!", pathDirFATOut.string()) << std::endl;
					std::exit(EXIT_CODE_FS_BAD_IO);
				}
				if (!silent) std::cout << "Done" << std::endl << std::endl;
			}
			else
			{
				if (!std::filesystem::is_directory(pathDirFATOut))
				{
					std::cout << "Given path for FAT Out exists, but is not a directory?!" << std::endl << "Path: " << pathDirFATOut.string() << std::endl;
					std::exit(EXIT_CODE_FS_BAD_IO);
				}
			}

			if (!silent) std::cout << std::format("Building new FATs to directory \"{}\"...", pathDirFATOut.string()) << std::endl << std::endl;
			nfResult = ndsFactory.buildFatData(pathDirFATData.string(), pathFAT.string(), addressFATData, pathDirFATOut.string());
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(EXIT_CODE_NF_RESULT_BAD);
			}

			if (!silent) std::cout << "Done building!" << std::endl;
		}
		else if (subcommandFATTools.is_subcommand_used("patch"))
		{
			pathFAT = std::filesystem::path(subsubcommandFATToolsPatch.get<std::string>("--fat"));
			uint32_t addrOriginalFATData = subsubcommandFATToolsPatch.get<uint32_t>("--originalfatdataaddr");
			uint32_t addrNewFATData = subsubcommandFATToolsPatch.get<std::uint32_t>("--newfatdataaddr");
			std::filesystem::path pathFATNew = std::filesystem::path(subsubcommandFATToolsPatch.get<std::string>("--newfat"));

			uint32_t posDiff = posDiff = addrOriginalFATData - addrNewFATData;
			if (addrOriginalFATData < addrNewFATData) posDiff = addrNewFATData - addrOriginalFATData;

			if (!silent) std::cout << std::format("Patching FAT Data file \"{}\"...", pathFAT.string()) << std::endl << std::endl;
			nfResult = ndsFactory.patchFat(pathFAT.string(), posDiff, pathFATNew.string());
			if (!nfResult.result)
			{
				std::cout << nfResult.message << std::endl;
				std::exit(EXIT_CODE_NF_RESULT_BAD);
			}

			if (!silent) std::cout << "Done patching!" << std::endl;
		}
		else
		{
			std::cout << "Bad subcommand used for fattools?!" << std::endl << std::endl;
			std::cout << program << std::endl;
			std::exit(EXIT_CODE_BAD_ARGUMENT);
		}
	}
	else
	{
		std::cout << program << std::endl;
	}
	return 0;
}
