/*
* Created 4/18/2025
* Author: Master801
*/

#include "cli.hpp"

void printInfo(NDSHeader* ndsHeader)
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
