// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCMMETSUMSFILLERTOOL_H__
#define __D3PD_L1CALOCMMETSUMSFILLERTOOL_H__

/*
  D3PD::L1CaloCMMEtSumsFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CMMEtSums.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CMM Et Sums
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCMMEtSumsFillerTool : public BlockFillerTool< LVL1::CMMEtSums > {

    public:
      L1CaloCMMEtSumsFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CMMEtSums& p );

    private:
      int*                         m_crate;
      int*                         m_dataID;
      int*                         m_peak;
      unsigned int*                m_et;
      unsigned int*                m_ex;
      unsigned int*                m_ey;
      int*                         m_etError;
      int*                         m_exError;
      int*                         m_eyError;
      std::vector<unsigned int>*   m_etVec;
      std::vector<unsigned int>*   m_exVec;
      std::vector<unsigned int>*   m_eyVec;
      std::vector<int>*            m_etErrorVec;
      std::vector<int>*            m_exErrorVec;
      std::vector<int>*            m_eyErrorVec;
      
  }; // class L1CaloCMMEtSumsFillerTool
} // namespace D3PD
#endif
