// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCPMTOWERFILLERTOOL_H__
#define __D3PD_L1CALOCPMTOWERFILLERTOOL_H__

/*
  D3PD::L1CaloCPMTowerFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CPMTower.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CPM Tower
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCPMTowerFillerTool : public BlockFillerTool< LVL1::CPMTower > {

    public:
      L1CaloCPMTowerFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CPMTower& p );

    protected:
      float AtlasPhi(const float& L1CaloPhi);

    private:
      int*               m_emEnergy;
      int*               m_hadEnergy;
      std::vector<int>*  m_emEnergyVec;
      std::vector<int>*  m_hadEnergyVec;
      int*               m_peak;
      int*               m_emError;
      int*               m_hadError;
      std::vector<int>*  m_emErrorVec;
      std::vector<int>*  m_hadErrorVec;

      float*             m_eta;
      float*             m_l1caloPhi;
      float*             m_atlasPhi;
      
  }; // class L1CaloCPMTowerFillerTool
} // namespace D3PD
#endif
