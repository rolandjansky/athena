// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCMMCPHITSFILLERTOOL_H__
#define __D3PD_L1CALOCMMCPHITSFILLERTOOL_H__

/*
  D3PD::L1CaloCMMCPHitsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CMMCPHits.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CMM CP Hits
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCMMCPHitsFillerTool : public BlockFillerTool< LVL1::CMMCPHits > {

    public:
      L1CaloCMMCPHitsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CMMCPHits& p );

    private:
      int*                         m_crate;
      int*                         m_dataID;
      unsigned int*                m_hitWord0;
      unsigned int*                m_hitWord1;
      int*                         m_error0;
      int*                         m_error1;
      std::vector<unsigned int>*   m_hitsVec0;
      std::vector<unsigned int>*   m_hitsVec1;
      std::vector<int>*            m_errorVec0;
      std::vector<int>*            m_errorVec1;
      int*                         m_peak;
      
  }; // class L1CaloCMMCPHitsFillerTool
} // namespace D3PD
#endif
