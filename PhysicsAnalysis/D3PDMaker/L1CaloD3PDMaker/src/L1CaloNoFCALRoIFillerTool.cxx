// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloNoFCALRoIFillerTool.cxx
  * @author John Morris <john.morris@cern.ch>
  * @date  October 2011
  * @brief L1Calo RoIs without the FCAL
  */

#include "AthenaKernel/errorcheck.h"
#include "L1CaloNoFCALRoIFillerTool.h"
#include <iostream>

namespace D3PD{


  L1CaloNoFCALRoIFillerTool::L1CaloNoFCALRoIFillerTool (const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
    m_roiTool("LVL1::L1RoITools/L1RoITools"),  
    m_triggerTowerLocation("TriggerTowers"),  
    m_fcalEta(3.2)
    {     
      declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
      declareProperty("FcalEta",m_fcalEta);

      book().ignore(); // Avoid coverity warnings.
    }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloNoFCALRoIFillerTool::initialize()
  {
    CHECK( m_roiTool.retrieve() );
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloNoFCALRoIFillerTool::book()
  {
    CHECK( addVariable ("RoIWord0",m_roi_energy_roiWord0) );
    CHECK( addVariable ("RoIWord1",m_roi_energy_roiWord1) );
    CHECK( addVariable ("RoIWord2",m_roi_energy_roiWord2) );
    CHECK( addVariable ("ExMiss",m_roi_energy_eXmiss) );
    CHECK( addVariable ("EyMiss",m_roi_energy_eYmiss) );
    CHECK( addVariable ("energyT",m_roi_energy_energyT) );
    CHECK( addVariable ("overflowX",m_roi_energy_overflowX) );
    CHECK( addVariable ("overflowY",m_roi_energy_overflowY) );
    CHECK( addVariable ("overflowT",m_roi_energy_overflowT) );

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloNoFCALRoIFillerTool::fill()
  {
    StatusCode sc;

    // Load Trigger Towers from StoreGate
    const TriggerTowerCollection* tts = 0;
    sc = evtStore()->retrieve(tts,m_triggerTowerLocation);
    if(sc.isFailure()){ATH_MSG_INFO("Failed to load Trigger Towers");return sc;}

    // Make new Trigger Tower Collection
    // Fill with TTs that are not in the FCAL
    TriggerTowerCollection* tts_tmp = new TriggerTowerCollection;
    typedef TriggerTowerCollection::const_iterator Itr;
    for(Itr i=tts->begin();i!=tts->end();++i){
      if( fabs( (*i)->eta() ) < m_fcalEta ){
        tts_tmp->push_back( (*i) );
      }
    }
    const TriggerTowerCollection* tts_noFcal = (const TriggerTowerCollection*)tts_tmp;

    // Make new RoIs based on Trigger Tower Collection which has no FCAL TTs
    LVL1_ROI L1Calo_rois_noFcal;
    m_roiTool->findROIs(tts_noFcal,L1Calo_rois_noFcal);
    std::vector<EnergySum_ROI> energy_rois_noFcal = L1Calo_rois_noFcal.getEnergySumROIs();

    // Dump info to D3PD
    if( energy_rois_noFcal.size() == 1){
      EnergySum_ROI roi = energy_rois_noFcal.at(0);

      *m_roi_energy_roiWord0 = roi.getROIWord0();
      *m_roi_energy_roiWord1 = roi.getROIWord1();
      *m_roi_energy_roiWord2 = roi.getROIWord2();
      *m_roi_energy_eXmiss = roi.getExMiss();
      *m_roi_energy_eYmiss = roi.getEyMiss();
      *m_roi_energy_energyT = roi.getEnergyT();

      bool overX = roi.getOverflowX();
      bool overY = roi.getOverflowY();
      bool overT = roi.getOverflowT();

      if( overX == true ){*m_roi_energy_overflowX = 1;}
      if( overX == false ){*m_roi_energy_overflowX = 0;}

      if( overY == true ){*m_roi_energy_overflowY = 1;}
      if( overY == false ){*m_roi_energy_overflowY = 0;}

      if( overT == true ){*m_roi_energy_overflowT = 1;}
      if( overT == false ){*m_roi_energy_overflowT = 0;}

    }    

    return sc;
  }
  
} // end of D3PD namespace
