// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCMMJETHITSFILLERTOOL_H__
#define __D3PD_L1CALOCMMJETHITSFILLERTOOL_H__

/*
  D3PD::L1CaloCMMJetHitsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CMMJetHits.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CMM Jet Hits
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCMMJetHitsFillerTool : public BlockFillerTool< LVL1::CMMJetHits > {

    public:
      L1CaloCMMJetHitsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CMMJetHits& p );

    private:
      int*                         m_crate;
      int*                         m_dataID;
      int*                         m_peak;
      unsigned int*                m_hits;
      int*                         m_error;
      std::vector<unsigned int>*   m_hitsVec;
      std::vector<int>*            m_errorVec;
      
  }; // class L1CaloCMMJetHitsFillerTool
} // namespace D3PD
#endif
