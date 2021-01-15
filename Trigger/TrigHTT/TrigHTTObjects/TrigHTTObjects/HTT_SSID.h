/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTT_SSID.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date December 17th, 2019
 * @brief Manages the bitfield representation of superstrip IDs
 *
 * An SSID uniquely identifies a superstrip. It is a bitfield containing a module
 * id and local indices that identify the position in the module. The sizes of
 * the fields are given by the constants below.
 *
 *              MSB >> ----------|---------|-------- >> LSB
 *      pixel:         module id | y index | x index
 *      strip:         module id |      x index
 *
 * x and y are SS-coordinates, i.e. coordinates in units of the local superstrip width.
 *
 * Note that simulation SSIDs are different than what will be stored in the hardware.
 * Hardware SSIDs are constrained to 16 bits, and therefore must use local module ids
 * to conserve bits. For example, while there may be 100k modules in ATLAS (we'd need
 * 17 bits), a single PRM or TP may only care about 1k of them (we only need 10 bits).
 * The conversion from a global to local module IDs is handled by HTTModuleLUT.
 *
 * The simulation does not have a bit constraint, and so we use 32 bit SSIDs. This
 * allows us to use global module ids in the simulation, which decouples the simulation
 * from hardware configurations. The number of local bits is the same between simulation
 * and hardware.
 *
 * To convert from simulation to hardware SSID, simply do:
 *      HTT_SSID bitfield(simulation_ssid);
 *      bitfield.setModule(HTTRegionMap::getLocalID(bitfield.getModule()));
 *      bitfield.getHWSSID();
 *
 * We currently use the Athena hashID as the global module ID, and ??? as the local
 * module ID.
 */

#include "TrigHTTUtils/HTTFunctions.h"

// Uncomment this to see if anything is overflowing the bit sizes
//#define CHECK_BITSIZE

constexpr uint32_t NBITS_SSID_SW = 32;                            // total number of ssid bits in software
constexpr uint32_t NBITS_SSID_HW = 16;                            // total number of ssid bits in hardware
constexpr uint32_t NBITS_LOCAL = 6;                               // number of bits for local coordinates
constexpr uint32_t NBITS_MODULE_SW = NBITS_SSID_SW - NBITS_LOCAL; // number of ssid bits to devote to the module id
constexpr uint32_t NBITS_MODULE_HW = NBITS_SSID_HW - NBITS_LOCAL; // number of ssid bits to devote to the module id
constexpr uint32_t NBITS_ETA = 2;                                 // number of bits for eta (y) coordinate, pixels only
constexpr uint32_t NBITS_PHI = NBITS_LOCAL - NBITS_ETA;           // number of bits for phi (x) coordinate, pixels only

constexpr uint32_t MAX_STRIP_X = (1u << NBITS_LOCAL) - 1;         // maximum value for the strip coordinate
constexpr uint32_t MAX_PIXEL_X = (1u << NBITS_PHI) - 1;           // maximum value for a pixel x coordinate
constexpr uint32_t MAX_PIXEL_Y = (1u << NBITS_ETA) - 1;           // maximum value for a pixel y coordinate
constexpr uint32_t MAX_MODULE_SW = (1u << NBITS_MODULE_SW) - 1;   // maximum value for the module ID in software
constexpr uint32_t MAX_MODULE_HW = (1u << NBITS_MODULE_HW) - 1;   // maximum value for the module ID in hardware


/*
 * NB: Typical applications should use ssid_t defined in HTTTypes.h.
 * This class is specifically for instances where the SSID needs to be broken down
 * into its bitfields, i.e. in SSMap or for DC bits.
 *
 * This class handles all Gray-coding. Setters and getters pass binary values, not gray-coded values.
 *
 * Typical use case:
 *
 *      Packing:
 *              SSID ssid;
 *              ssid.setPixelX(x);
 *              ssid.setPixelY(y);
 *              ssid.setModule(module_id);
 *              ssid.getSSID();
 *
 *      Unpacking:
 *              SSID ssid(ssid_int);
 *              ssid.getPixelX();
 *              ssid.getPixelY();
 *              ssid.getModule();
 */

union HTT_SSID
{
    public:

        HTT_SSID(uint32_t ssid = 0) : ssid(ssid) {}

        ///////////////////////////////////////////////////////////////////////
        // Setters

        inline void setStripX(uint32_t x)
        {
#ifdef CHECK_BITSIZE
            if (x > MAX_STRIP_X)
                std::cerr << "WARNING HTT_SSID::setStripX() x value " << formatHex(x) << " too large to fit bitfield!" << std::endl;
#endif
            strip.x = binaryToGray(x);
        }

        inline void setPixelX(uint32_t x)
        {
#ifdef CHECK_BITSIZE
            if (x > MAX_PIXEL_X)
                std::cerr << "WARNING HTT_SSID::setPixelX() x value " << formatHex(x) << " too large to fit bitfield!" << std::endl;
#endif
            pixel.x = binaryToGray(x);
        }

        inline void setPixelY(uint32_t y)
        {
#ifdef CHECK_BITSIZE
            if (y > MAX_PIXEL_Y)
                std::cerr << "WARNING HTT_SSID::setPixelY() y value " << formatHex(y) << " too large to fit bitfield!" << std::endl;
#endif
            pixel.y = binaryToGray(y);
        }

        inline void setModule(uint32_t moduleID)
        {
#ifdef CHECK_BITSIZE
            if (moduleID > MAX_MODULE_HW) // Check for hardware module size only
                std::cerr << "WARNING HTT_SSID::setModule() value " << formatHex(moduleID) << " too large to fit bitfield!" << std::endl;
#endif
            strip.module = moduleID; // WARNING: Assumes the bit-field for strip modules and pixel modules have the same size
        }


        ///////////////////////////////////////////////////////////////////////
        // Getters


        inline uint32_t getSSID() const { return ssid; }

        inline uint16_t getHWSSID() const { return ssid_hw; }

        inline uint32_t getModule() const { return strip.module; }

        inline uint32_t getStripX() const { return grayToBinary32(strip.x); }

        inline uint32_t getPixelX() const { return grayToBinary32(pixel.x); }

        inline uint32_t getPixelY() const { return grayToBinary32(pixel.y); }

    private:

        // We don't need separate cases for module_hw, since we truncate when accessing
        // ssid_hw anyways.

        struct
        {
            uint32_t x : NBITS_LOCAL; // gray-coded
            uint32_t module : NBITS_MODULE_SW;
        } strip;

        struct
        {
            uint32_t x : NBITS_PHI; // gray-coded
            uint32_t y : NBITS_ETA; // gray-coded (separately from x!)
            uint32_t module : NBITS_MODULE_SW;
        } pixel;

        uint32_t ssid_hw : NBITS_SSID_HW;
        uint32_t ssid : NBITS_SSID_SW;
};
