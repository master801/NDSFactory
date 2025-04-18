#pragma once

#include <cstdint>

#define IconTitleSize 0xA00
#define Arm9FooterSize 0xC

#pragma pack(push, 1)
struct NDSHeader
{
    char GameTitle[0xC];
    char GameCode[0x4];
    unsigned char MakerCode[0x2];
    unsigned char UnitCode;//0x00=NDS, 0x02=NDS+DSi, 0x03=DSi
    unsigned char DeviceType;
    unsigned char DeviceSize;
    unsigned char Reserved1[0x9];
    unsigned char RomVersion;
    unsigned char Flags;

    uint32_t Arm9RomAddr;
    uint32_t Arm9EntryAddr;
    uint32_t Arm9RamAddr;
    uint32_t Arm9Size;

    uint32_t Arm7RomAddr;
    uint32_t Arm7EntryAddr;
    uint32_t Arm7RamAddr;
    uint32_t Arm7Size;

    uint32_t FilenameTableAddr;
    uint32_t FilenameSize;
    uint32_t FATAddr;
    uint32_t FATSize;

    uint32_t Arm9OverlayAddr;
    uint32_t Arm9OverlaySize;
    uint32_t Arm7OverlayAddr;
    uint32_t Arm7OverlaySize;

    uint32_t NormalCommandsSettings;
    uint32_t Key1CommandsSettings;

    uint32_t IconTitleAddr;

    uint16_t SecureAreaCRC16;
    uint16_t SecureAreaLoadingTimeout;

    uint32_t ARM9AutoLoadListRamAddr;
    uint32_t ARM7AutoLoadListRamAddr;

    uint64_t SecureAreaDisable;

    uint32_t RomSize;
    uint32_t HeaderSize;

    unsigned char Reserved2[0x38];

    unsigned char NintendoLogo[0x9C];
    uint16_t NintendoLogoCRC;
    uint16_t HeaderCRC16;

    //Debugger reserved section
    uint32_t DebugRomAddr;//0x160
    uint32_t DebugSize;
    uint32_t DebugRamAddr;
    unsigned char Reserved3[0x4];
    unsigned char Reserved4[0x10];

    //EXTENDED SECTION
    //https://dsibrew.org/wiki/DSi_cartridge_header
    //https://www.problemkaputt.de/gbatek-dsi-cartridge-header.htm
    //0x180

    uint32_t GlobalMBKSettings[0x4];
    uint32_t LocalMBKSettingsARM9[0x3];
    uint32_t LocalMBKSettingsARM7[0x3];
    uint32_t GlobalMBK9Setting;

    uint32_t RegionFlags;

    uint32_t AccessControl;

    uint32_t ARM7_SCFG_EXT_Mask;

    uint32_t BannerFlags;//0x1BC - ?

    uint32_t ARM9iRomOffset;//0x1C0
    uint8_t Reserved5[0x4];
    uint32_t ARM9iLoadAddress;
    uint32_t ARM9iSize;
    uint32_t ARM9iRomAddr;

    uint32_t ARM7iROMOffset;
    uint32_t ARM7iMMCDeviceListRAMAddr;//0x1D4
    uint32_t ARM7iLoadAddr;
    uint32_t ARM7iSize;

    uint32_t DigestNTRRegionOffset;//0x1E0
    uint32_t DigestNTRRegionSize;

    uint32_t DigestTWLRegionOffset;//0x1E8
    uint32_t DigestTWLRegionSize;//0x1E8

    uint32_t DigestSectorHashTableOffset;
    uint32_t DigestSectorHashTableSize;

    uint32_t DigestBlockHashTableOffset;
    uint32_t DigestBlockHashTableSize;

    uint32_t DigestSectorSize;
    uint32_t DigestBlockSectorCount;

    uint32_t IconBannerSize;//0x208 - nominally 0x23C0 for DSi

    uint8_t Reserved6[0x4];//Used by DSi

    uint32_t NTRTWLRegionROMSize;//0x210

    uint8_t Reserved7[0xC];//Used by DSi

    uint32_t ModcryptArea1Offset;
    uint32_t ModcryptArea1Size;

    uint32_t ModcryptArea2Offset;
    uint32_t ModcryptArea2Size;

    char TitleIDGameCode[4];
    uint8_t TitleIDFileType;//0x00=Cartridge, 0x04=DSiWare, 0x05=Systen Fun Tools(?), 0x15=System Base Tools, 0x17=System Menu
    uint8_t TitleIDZero;////0x00=Normal
    uint8_t TitleIDThree;//0x03=DSi
    uint8_t TitleIDTwo;//0x00=Normal

    uint32_t DSiWarePubSaveSize;
    uint32_t DSiWarePrivSaveSize;

    uint8_t Reserved8[0xB0];

    uint8_t ParentalControlAgeRatings[0x10];

    uint8_t ARM9SecSHA1HMACHash[0x14];
    uint8_t ARM7SHA1HMACHash[0x14];
    uint8_t DigestMasterSHA1HMACHash[0x14];
    uint8_t BannerSHA1HMACHash[0x14];
    uint8_t ARM9iDecSHA1HMACHash[0x14];
    uint8_t ARM7iDecSHA1HMACHash[0x14];

    uint8_t Reserved10[0x28];

    uint8_t ARM9UnsecSHA1HMACHash[0x14];

    uint8_t Reserved11[0xA4C];

    uint8_t Reserved12Debug[0x180];

    uint8_t RSASignature[0x80];

};
#pragma pack(pop)
