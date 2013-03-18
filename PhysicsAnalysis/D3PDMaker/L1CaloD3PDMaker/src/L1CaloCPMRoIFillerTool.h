// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCPMROIFILLERTOOL_H__
#define __D3PD_L1CALOCPMROIFILLERTOOL_H__

/*
  D3PD::L1CaloCPMRoIFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloEvent/CPMTowerCollection.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CPM RoI
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCPMRoIFillerTool : public BlockFillerTool< LVL1::CPMRoI > {

    public:
      L1CaloCPMRoIFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      /// Standard Gaudi initialize method.
      virtual StatusCode initialize();

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CPMRoI& p );

    protected:
      float AtlasPhi(const float& L1CaloPhi);

    private:
      ToolHandle<LVL1::IL1EmTauTools> m_emTauTool;
      std::string m_CPMTowersLocation;
      
      int*       m_crate;
      int*       m_cpm;
      int*       m_chip;
      int*       m_location;
      int*       m_hits;
      int*       m_error;
      uint32_t*  m_roiWord;
      float*     m_eta;
      float*     m_l1caloPhi;
      float*     m_atlasPhi;

      int* m_core;
      int* m_emClus;
      int* m_tauClus;
      int* m_emIsol;
      int* m_hadIsol;
      int* m_hadVeto;
      unsigned int* m_isEtMax;
      
  }; // class L1CaloCPMRoIFillerTool
} // namespace D3PD
#endif
