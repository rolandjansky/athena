/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLTIMING_H
#define TILECONDITIONS_TILECONDTOOLTIMING_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>

class TileCondToolTiming : public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolTiming(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolTiming();
  
    StatusCode initialize();
    StatusCode finalize();


    float getSignalPhase(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getChannelOffset(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
  
  private:

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryAdcOffset;
};


//
//____________________________________________________________________
inline float TileCondToolTiming::getSignalPhase(unsigned int drawerIdx, 
                                                unsigned int channel, 
                                                unsigned int adc) const {
  return getChannelOffset(drawerIdx, channel, adc);
}

#endif
