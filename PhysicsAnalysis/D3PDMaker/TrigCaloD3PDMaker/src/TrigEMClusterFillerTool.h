// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterFillerTool.h 497206 2012-04-23 13:13:27Z krasznaa $
#ifndef __D3PD_TrigEMClusterFillerTool_h__
#define __D3PD_TrigEMClusterFillerTool_h__
/*
  D3PD::TrigEMClusterFillerTool.h
*/
#include <vector>
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigCaloEvent/TrigEMCluster.h"

namespace D3PD {

   class TrigEMClusterFillerTool : public D3PD::BlockFillerTool<TrigEMCluster> {
   public:
      TrigEMClusterFillerTool(const std::string& type, const std::string& name, 
                              const IInterface* parent);

      StatusCode book();
      StatusCode fill(const TrigEMCluster& x);

   private:
      // Properties
      bool mWriteBasics;
      bool mWriteHypoVars;
      bool mWriteDetails;

      // D3PD variables
      float* m_energy;
      float* m_et;
      std::vector<float>* m_energyInSample;
      float* m_eta;
      float* m_phi;
      float* m_e237;
      float* m_e277;
      float* m_fracs1;
      float* m_weta2;
      float* m_ehad1;
      float* m_Eta1;
      float* m_emaxs1;
      float* m_e2tsts1;
      float* m_e233;
      float* m_wstot;

   }; // class TrigEMClusterFillerTool

} // namespace D3PD

#endif // __D3PD_TrigEMClusterFillerTool_h__

