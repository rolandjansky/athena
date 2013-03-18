// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOJEMETSUMSFILLERTOOL_H__
#define __D3PD_L1CALOJEMETSUMSFILLERTOOL_H__

/*
  D3PD::L1CaloJEMEtSumsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/JEMEtSums.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo JEM Et Sums
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloJEMEtSumsFillerTool : public BlockFillerTool< LVL1::JEMEtSums > {

    public:
      L1CaloJEMEtSumsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::JEMEtSums& p );

    private:
      int*                         m_crate;
      int*                         m_module;
      int*                         m_peak;
      unsigned int*                m_forward;
      unsigned int*                m_et;
      unsigned int*                m_ex;
      unsigned int*                m_ey;
      std::vector<unsigned int>*   m_etVec;
      std::vector<unsigned int>*   m_exVec;
      std::vector<unsigned int>*   m_eyVec;

      
  }; // class L1CaloJEMEtSumsFillerTool
} // namespace D3PD
#endif
