// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOJETELEMENTFILLERTOOL_H__
#define __D3PD_L1CALOJETELEMENTFILLERTOOL_H__

/*
  D3PD::L1CaloJetElementFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/JetElement.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo JEM Et Sums
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloJetElementFillerTool : public BlockFillerTool< LVL1::JetElement > {

    public:
      L1CaloJetElementFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::JetElement& p );
      
    protected:
      float AtlasPhi(const float& L1CaloPhi);      

    private:
      float*              m_eta;
      float*              m_l1caloPhi;
      float*              m_atlasPhi;
      int*                m_emEnergy;
      int*                m_hadEnergy;
      int*                m_energy;
      unsigned int*       m_isSaturated;
      unsigned int*       m_isEmSaturated;
      unsigned int*       m_isHadSaturated;
      int*                m_emError;
      int*                m_hadError;
      int*                m_linkError;
      int*                m_peak;
      std::vector<int>*   m_emEnergyVec;
      std::vector<int>*   m_hadEnergyVec;
      std::vector<int>*   m_emErrorVec;
      std::vector<int>*   m_hadErrorVec;
      std::vector<int>*   m_linkErrorVec;

      
  }; // class L1CaloJetElementFillerTool
} // namespace D3PD
#endif
