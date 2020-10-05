/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLNOISESAMPLE_H
#define TILECONDITIONS_TILECONDTOOLNOISESAMPLE_H

// Tile includes
#include "TileConditions/ITileCondToolNoise.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/TileEMScale.h"
#include "TileIdentifier/TileRawChannelUnit.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>


class TileCondToolNoiseSample: public AthAlgTool
                             , virtual public ITileCondToolNoise {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolNoiseSample(const std::string& type, const std::string& name,
        const IInterface* parent);
    virtual ~TileCondToolNoiseSample();

    StatusCode initialize();
    StatusCode finalize();

    float getPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                 TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const;

    float getHfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                 TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const;

    float getLfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                 TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const;

    float getHfn1(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfn2(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfnNorm(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getOnlinePedestalDifference(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                      TileRawChannelUnit::UNIT onlineUnit = TileRawChannelUnit::OnlineADCcounts) const;


    float getNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
        TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const {
      return getHfn(drawerIdx, channel, adc, unit);
    }

  private:

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibSampleNoiseKey{this,
        "TileSampleNoise", "TileSampleNoise", "Input Tile sample noise constants"};

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibOnlineSampleNoiseKey{this,
        "TileOnlineSampleNoise", "TileOnlineSampleNoise", "Input Tile online noise sample constants"};

    SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
        "TileEMScale", "TileEMScale", "Input Tile EMS conditions"};
};

#endif
