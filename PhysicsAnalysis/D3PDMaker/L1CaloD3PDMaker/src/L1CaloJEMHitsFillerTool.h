// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOJEMHITSFILLERTOOL_H__
#define __D3PD_L1CALOJEMHITSFILLERTOOL_H__

/*
  D3PD::L1CaloJEMHitsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/JEMHits.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo JEM Hits
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloJEMHitsFillerTool : public BlockFillerTool< LVL1::JEMHits > {

    public:
      L1CaloJEMHitsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::JEMHits& p );

    private:
      int*                         m_crate;
      int*                         m_module;
      int*                         m_peak;
      unsigned int*                m_forward;
      unsigned int*                m_jetHits;
      std::vector<unsigned int>*   m_jetHitsVec;
      
  }; // class L1CaloJEMHitsFillerTool
} // namespace D3PD
#endif
