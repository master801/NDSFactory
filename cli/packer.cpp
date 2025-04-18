/*
* Created 4/17/2025
* Author: Master801
*/

#include "cli.hpp"

int packROM(NDSFactory* ndsFactory, const bool silent, std::filesystem::path pathROMOut, int pad, bool trim, bool shouldFixCRC, std::filesystem::path pathHeader, std::filesystem::path pathARM9, std::filesystem::path pathARM7, std::filesystem::path pathFATNameTable, std::filesystem::path pathFAT, std::filesystem::path pathFATData, std::filesystem::path pathARM9Overlay, std::filesystem::path pathARM9OverlayData, std::filesystem::path pathARM7Overlay, std::filesystem::path pathARM7OverlayData, std::filesystem::path pathLogos)
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
		if (!silent) std::cout << "Fixed header CRC16" << std::endl << std::endl;
	}

	bool hasARM9Footer = ndsFactory->checkArm9FooterPresence(pathARM9.string(), ndsHeader->Arm9Size);

	char paddingType = '\xFF';
	if (pad == 0) paddingType = '\x00';


	uint32_t addrStart;
	uint32_t size;

	//Write header
	if (!silent) std::cout << "Writing header \"" << pathHeader.string() << "\"..." << std::endl;
	nfResult = ndsFactory->writeBytesToFile(romHeaderBuffer, pathROMOut.string(), 0, sizeof(NDSHeader));
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write header padding
	addrStart = sizeof(NDSHeader);
	size = ndsHeader->Arm9RomAddr - addrStart;
	nfResult = ndsFactory->writePaddingToFile('\x00', pathROMOut.string(), addrStart, size);//Padding is always 0x00 for header
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write ARM9 bin
	if (!silent) std::cout << "Writing ARM9 \"" << pathARM9.string() << "\"..." << std::endl;
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
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write ARM9 Overlay
	if (ndsHeader->Arm9OverlayAddr)
	{
		if (pathARM9Overlay.empty())
		{
			if (!silent) std::cout << "Header specifies a ARM9 Overlay, but no ARM9 Overlay file was given!" << std::endl;
			return -1;
		}
		if (pathARM9OverlayData.empty())
		{
			if (!silent) std::cout << "Header specifies a ARM9 Overlay, but no ARM9 Overlay Data file was given!" << std::endl;
			return -1;
		}

		//Actually write ARM9 Overlay
		if (!silent) std::cout << "Writing ARM9 Overlay \"" << pathARM9Overlay.string() <<"\"..." << std::endl;
		nfResult = ndsFactory->writeSectionToFile(pathARM9Overlay.string(), pathROMOut.string(), ndsHeader->Arm9OverlayAddr, ndsHeader->Arm9OverlaySize);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
		if (!silent) std::cout << "Done" << std::endl << std::endl;

		//Write ARM9 Overlay Data
		if (!silent) std::cout << "Writing ARM9 Overlay Data \"" << pathARM9OverlayData.string() << "\"..." << std::endl;
		addrStart = ndsHeader->Arm9OverlayAddr + ndsHeader->Arm9OverlaySize;
		size = ndsHeader->Arm7RomAddr - addrStart;
		nfResult = ndsFactory->writeSectionToFile(pathARM9OverlayData.string(), pathROMOut.string(), addrStart, size);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
		if (!silent) std::cout << "Done" << std::endl << std::endl;
	}


	//Write ARM7
	if (!silent) std::cout << "Writing ARM7 \"" << pathARM7.string() << "\"..." << std::endl;
	nfResult = ndsFactory->writeSectionToFile(pathARM7.string(), pathROMOut.string(), ndsHeader->Arm7RomAddr, ndsHeader->Arm7Size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}

	//Write ARM7 padding
	addrStart = ndsHeader->Arm7RomAddr + ndsHeader->Arm7Size;
	size = ndsHeader->FilenameTableAddr - addrStart;
	if (ndsHeader->Arm7OverlayAddr) size = ndsHeader->Arm7OverlayAddr - addrStart;

	nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write ARM7 Overlay
	if (ndsHeader->Arm7OverlayAddr)
	{
		if (pathARM7Overlay.empty())
		{
			std::cout << "Header specifies a ARM7 Overlay, but no ARM7 Overlay file was given!" << std::endl << std::endl;
			return -1;
		}
		if (pathARM7OverlayData.empty())
		{
			std::cout << "Header specifies a ARM7 Overlay, but no ARM7 Overlay Data file was given!" << std::endl << std::endl;
			return -1;
		}

		//Actually write ARM7 Overlay
		if (!silent) std::cout << "Writing ARM7 Overlay \"" << pathARM7Overlay.string() << "\"..." << std::endl;
		nfResult = ndsFactory->writeSectionToFile(pathARM7Overlay.string(), pathROMOut.string(), ndsHeader->Arm7OverlayAddr, ndsHeader->Arm7OverlaySize);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
		if (!silent) std::cout << "Done" << std::endl << std::endl;

		//Write ARM7 Overlay Data
		if (!silent) std::cout << "Writing ARM7 Overlay Data \"" << pathARM7OverlayData.string() << "\"..." << std::endl;
		addrStart = ndsHeader->Arm7OverlayAddr + ndsHeader->Arm7OverlaySize;
		size = ndsHeader->FilenameSize - addrStart;
		nfResult = ndsFactory->writeSectionToFile(pathARM7OverlayData.string(), pathROMOut.string(), addrStart, size);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
		if (!silent) std::cout << "Done" << std::endl << std::endl;
	}


	//Write FAT Name Table
	if (!silent) std::cout << "Writing FAT Name Table \"" << pathFATNameTable.string() << "\"..." << std::endl;
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
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write FAT
	if (!silent) std::cout << "Writing FAT \"" << pathFAT.string() << "\"..." << std::endl;
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
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write Icon / Title logo
	if (!silent) std::cout << "Writing Icon / Title logo \"" << pathLogos.string() << "\"..." << std::endl;
	nfResult = ndsFactory->writeSectionToFile(pathLogos.string(), pathROMOut.string(), ndsHeader->IconTitleAddr, IconTitleSize);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write FAT Data
	if (!silent) std::cout << "Writing FAT Data \"" << pathFATData.string() << "\"..." << std::endl;
	addrStart = ndsHeader->IconTitleAddr + IconTitleSize;
	size = ndsHeader->RomSize - addrStart;
	nfResult = ndsFactory->writeSectionToFile(pathFATData.string(), pathROMOut.string(), addrStart, size);
	if (!nfResult.result)
	{
		std::cout << nfResult.message << std::endl;
		return -1;
	}
	if (!silent) std::cout << "Done" << std::endl << std::endl;


	//Write ROM padding
	if (!trim)
	{
		if (!silent) std::cout << "Writing ROM padding..." << std::endl;
		addrStart = ndsHeader->RomSize;
		size = ndsFactory->getCardSizeInBytes(ndsHeader->DeviceSize) - addrStart;
		nfResult = ndsFactory->writePaddingToFile(paddingType, pathROMOut.string(), addrStart, size);
		if (!nfResult.result)
		{
			std::cout << nfResult.message << std::endl;
			return -1;
		}
		if (!silent) std::cout << "Done" << std::endl << std::endl;
	}
	return 0;
}
