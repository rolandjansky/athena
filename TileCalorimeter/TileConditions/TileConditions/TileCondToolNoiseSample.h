/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLNOISESAMPLE_H
#define TILECONDITIONS_TILECONDTOOLNOISESAMPLE_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileConditions/ITileCondToolNoise.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>


// Forward declaration
class TileCalibDrawerFlt;
class TileCondToolEmscale;

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

    //  void  getAutoCorr(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
    //		    std::vector<float>& vec) const;

    float getOnlinePedestalDifference(unsigned int drawerIdx, unsigned int channel, unsigned int adc, 
                                      TileRawChannelUnit::UNIT onlineUnit = TileRawChannelUnit::OnlineADCcounts) const;


    float getNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
        TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const {
      return getHfn(drawerIdx, channel, adc, unit);
    }

  private:

    //=== used tools
    ToolHandle<TileCondToolEmscale> m_tileToolEms;

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryNoiseSample;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOnlineNoiseSample;
    //  ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryNoiseAutoCr;
    bool m_useOnlineNoise;
};

#endif
