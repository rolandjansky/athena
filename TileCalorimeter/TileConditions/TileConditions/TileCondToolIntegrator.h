/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLINTEGRATOR_H
#define TILECONDITIONS_TILECONDTOOLINTEGRATOR_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>

class TileCondToolIntegrator: public AthAlgTool {

  public:

    static const InterfaceID& interfaceID();
    TileCondToolIntegrator(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolIntegrator();

    StatusCode initialize();
    StatusCode finalize();

    float getGain(      unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getGainError( unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getChi2(      unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getPedestal(  unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getDACForPed( unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getSigmaOfPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getRMSOfPed(  unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getSigmaOfRMS(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

  private:

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryIntegrator;

};

#endif
