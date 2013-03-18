// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCPMHITSFILLERTOOL_H__
#define __D3PD_L1CALOCPMHITSFILLERTOOL_H__

/*
  D3PD::L1CaloCPMHitsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CPMHits.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CPM Hits
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCPMHitsFillerTool : public BlockFillerTool< LVL1::CPMHits > {

    public:
      L1CaloCPMHitsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CPMHits& p );

    private:
      int*                         m_crate;
      int*                         m_module;
      int*                         m_peak;
      unsigned int*                m_hitWord0;
      unsigned int*                m_hitWord1;
      std::vector <unsigned int>*  m_hitsVec0;
      std::vector <unsigned int>*  m_hitsVec1;
      
  }; // class L1CaloCPMHitsFillerTool
} // namespace D3PD
#endif
