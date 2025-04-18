/*
* Created 4/17/2025
* Author: Master801
*/

#include "cli.hpp"

bool unpackROM(const std::filesystem::path fpROM, NDSFactory* ndsFactory, const bool silent, NDSHeader* ndsHeader, bool overdumpARM9, std::filesystem::path pathHeader, std::filesystem::path pathARM9, std::filesystem::path pathARM7, std::filesystem::path pathFATNameTable, std::filesystem::path pathFAT, std::filesystem::path pathFATData, std::filesystem::path pathARM9Overlay, std::filesystem::path pathARM9OverlayData, std::filesystem::path pathARM7Overlay, std::filesystem::path pathARM7OverlayData, std::filesystem::path pathLogos)
{
	NFResult nfResult;
	bool unpackedProperly = true;

	if (!(pathHeader.empty()))
	{
		if (!silent) std::cout << "Dumping Header..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathHeader.string(), 0, ndsHeader->HeaderSize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped Header to \"{}\"", pathHeader.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	if (!(pathARM9.empty()))
	{
		if (!silent) std::cout << "Dumping ARM9..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9.string(), ndsHeader->Arm9RomAddr, ndsHeader->Arm9Size + (overdumpARM9 ? Arm9FooterSize : 0));
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM9 to \"{}\"", pathARM9.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	if (!(pathARM7.empty()))
	{
		if (!silent) std::cout << "Dumping ARM7..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7.string(), ndsHeader->Arm7RomAddr, ndsHeader->Arm7Size);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM7 to \"{}\"", pathARM7.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}
	if (!(pathFATNameTable.empty()))
	{
		if (!silent) std::cout << "Dumping FAT Name Table..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFATNameTable.string(), ndsHeader->FilenameTableAddr, ndsHeader->FilenameSize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped FAT Name Table to \"{}\"", pathFATNameTable.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}
	if (!(pathFAT.empty()))
	{
		if (!silent) std::cout << "Dumping FAT..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFAT.string(), ndsHeader->FATAddr, ndsHeader->FATSize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped FAT to \"{}\"", pathFAT.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	uint32_t startAddrFATData = ndsHeader->IconTitleAddr + IconTitleSize;
	uint32_t sizeFATData = ndsHeader->RomSize - startAddrFATData;
	if (!(pathFATData.empty()))
	{
		if (!silent) std::cout << "Dumping FAT Data..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathFATData.string(), startAddrFATData, sizeFATData);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped FAT Data to \"{}\"", pathFATData.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}
	if (!(pathARM9Overlay.empty()))
	{
		if (!silent) std::cout << "Dumping ARM9 Overlay..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9Overlay.string(), ndsHeader->Arm9OverlayAddr, ndsHeader->Arm9OverlaySize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM9 Overlay to \"{}\"", pathARM9Overlay.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	uint32_t startAddrARM9OverlayData = ndsHeader->Arm9OverlayAddr + ndsHeader->Arm9OverlaySize;
	uint32_t sizeARM9OverlayData = ndsHeader->FilenameTableAddr - startAddrARM9OverlayData;
	if (!(pathARM9OverlayData.empty()))
	{
		if (!silent) std::cout << "Dumping ARM9 Overlay Data..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM9OverlayData.string(), startAddrARM9OverlayData, sizeARM9OverlayData);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM9 Overlay Data to \"{}\"", pathARM9OverlayData.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl << std::endl;
			unpackedProperly &= false;
		}
	}
	if (!(pathARM7Overlay.empty()))
	{
		if (!silent) std::cout << "Dumping ARM7 Overlay..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7Overlay.string(), ndsHeader->Arm7OverlayAddr, ndsHeader->Arm7OverlaySize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM7 Overlay to \"{}\"", pathARM7Overlay.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	uint32_t startAddrARM7OverlayData = ndsHeader->Arm7OverlayAddr + ndsHeader->Arm7OverlaySize;
	uint32_t sizeARM7OverlayData = ndsHeader->FilenameTableAddr - startAddrARM7OverlayData;
	if (!(pathARM7OverlayData.empty()))
	{
		if (!silent) std::cout << "Dumping ARM7 Overlay Data..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathARM7OverlayData.string(), startAddrARM7OverlayData, sizeARM7OverlayData);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped ARM7 Overlay Data to \"{}\"", pathARM7OverlayData.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}
	if (!(pathLogos.empty()))
	{
		if (!silent) std::cout << "Dumping Icon / Title Logo..." << std::endl;
		nfResult = ndsFactory->dumpDataFromFile(fpROM.string(), pathLogos.string(), ndsHeader->IconTitleAddr, IconTitleSize);
		if (nfResult.result)
		{
			if (!silent) std::cout << std::format("Dumped Logos to \"{}\"", pathLogos.string()) << std::endl << std::endl;
		}
		else
		{
			std::cout << nfResult.message << std::endl;
			unpackedProperly &= false;
		}
	}

	return unpackedProperly;
}
