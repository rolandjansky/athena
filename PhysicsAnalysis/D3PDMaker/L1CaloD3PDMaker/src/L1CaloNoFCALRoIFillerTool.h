// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloNoFCALRoIFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @date  October 2011
  * @brief L1Calo RoIs without the FCAL
  */

#ifndef _L1CALOD3PDMAKER_L1CALONOFCALROIFILLERTOOL_H_
#define _L1CALOD3PDMAKER_L1CALONOFCALROIFILLERTOOL_H_


#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"


#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigT1CaloToolInterfaces/IL1RoITools.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"

namespace D3PD{

  class L1CaloNoFCALRoIFillerTool : public BlockFillerTool<void>
  {
  public:
    L1CaloNoFCALRoIFillerTool (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    /// Standard Gaudi initialize method.
    virtual StatusCode initialize();

    virtual StatusCode book();
    virtual StatusCode fill();

  private:

    ToolHandle<LVL1::IL1RoITools>        m_roiTool;
    std::string                          m_triggerTowerLocation;

    double                               m_fcalEta;
    
    // Energy ROI
    uint32_t*                            m_roi_energy_roiWord0;
    uint32_t*                            m_roi_energy_roiWord1;
    uint32_t*                            m_roi_energy_roiWord2;
    float*                               m_roi_energy_eXmiss;
    float*                               m_roi_energy_eYmiss;
    float*                               m_roi_energy_energyT;
    unsigned int*                        m_roi_energy_overflowX;
    unsigned int*                        m_roi_energy_overflowY;
    unsigned int*                        m_roi_energy_overflowT;
  };
} // end of D3PD namespace
#endif
