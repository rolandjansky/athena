/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLDSPTHRESHOLD_H
#define TILECONDITIONS_TILECONDTOOLDSPTHRESHOLD_H

// Tile includes
#include "TileConditions/ITileCondToolDspThreshold.h"
#include "TileConditions/ITileCondProxy.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

class TileCalibDrawerFlt;

class TileCondToolDspThreshold: public AthAlgTool, virtual public ITileCondToolDspThreshold {

  public:

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolDspThreshold("TileCondToolDspThreshold", 1, 0);    
      return IID_TileCondToolDspThreshold;
    };

    TileCondToolDspThreshold(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolDspThreshold();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual float getMinimumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const override;
    virtual float getMaximumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const override;
    virtual void getAmplitudeThresholds(unsigned int drawerIdx, unsigned int channel, unsigned int adc, 
                                float& minimumThreshold, float& maximumThreshold) const override;
    virtual float getDspThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const override;

  private:

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryDspThreshold;

};

#endif // TILECONDITIONS_TILECONDTOOLDSPTHRESHOLD_H
