/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLINTEGRATOR_H
#define TILECONDITIONS_TILECONDTOOLINTEGRATOR_H

// Tile includes
#include "TileConditions/TileCalibData.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"


#include <vector>

class TileCondToolIntegrator: public AthAlgTool {

  public:

    static const InterfaceID& interfaceID();
    TileCondToolIntegrator(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolIntegrator();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    float getGain(      unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getGainError( unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getChi2(      unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getPedestal(  unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getDACForPed( unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getSigmaOfPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getRMSOfPed(  unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getSigmaOfRMS(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

  private:

    //=== TileCalibData
    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibIntegratorKey{this,
        "TileIntegrator", "TileIntegrator", "Input Tile integrator constants"};


};

#endif
