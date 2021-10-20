/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEEXPERTTOOLEMSCALE_H
#define TILECONDITIONS_TILEEXPERTTOOLEMSCALE_H

/**
 * @brief (For experts only) Calibration of TileCal channels and access to calibration factors.
 * @author Lukas Pribyl <lukas.pribyl@cern.ch>
 *
 * @details This tool has the same functionality as the standard production TileCondToolEmscale
 * tool. For the description of TileExpertToolEmscale methods please consult the file
 * TileCondToolEmscale. The TileExpertToolEmscale tool is in addition able to switch on/off
 * individual calibration corrections. This is set via TileIdentifier/TileEmscaleCalibOptions
 * bitmap. If the calibration correction is switched on, the tool behaves exactly as TileCondToolEmscale,
 * from which it inherits most of the methods. If the calibration correction is switched of (the
 * relevant bit in m_emOptions bitmap is false), the relevant calibration method returns the input amplitude
 * without any change. The value of the bitmap m_emOptions is set only once at the constructor.
 */

// Tile includes
#include "TileConditions/TileCondToolEmscale.h"
#include "TileIdentifier/TileEmscaleCalibOptions.h"

class TileExpertToolEmscale: public TileCondToolEmscale {
  public:

    static const InterfaceID& interfaceID();
    TileExpertToolEmscale(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileExpertToolEmscale() {};

    float channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                       , TileRawChannelUnit::UNIT rawDataUnitIn, TileRawChannelUnit::UNIT rawDataUnitOut) const;

    float doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const;
    float doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;
    float doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const;
    float doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    float channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                          , float amplitude, TileRawChannelUnit::UNIT onlUnit) const;

    float doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;
    float doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;
    float doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;
    float doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    float undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                       , float amplitude, TileRawChannelUnit::UNIT onlUnit) const;

    float getLasPartition(unsigned int drawerIdx) const;
    void setEmOptions(const TileEmscaleCalibOptions& emOptions);

  private:

    bool m_setOnceCounter;
    TileEmscaleCalibOptions m_emOptions;

};

#endif
