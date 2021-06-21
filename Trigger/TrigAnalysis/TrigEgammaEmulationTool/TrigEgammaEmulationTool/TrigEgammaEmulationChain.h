/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationChain_h
#define TrigEgammaEmulationChain_h

#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationInfo.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationChain.h"


namespace Trig{

  class TrigEgammaEmulationChain: 
          public asg::AsgTool,
          virtual public ITrigEgammaEmulationChain
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationChain , ITrigEgammaEmulationChain )

    public:

      TrigEgammaEmulationChain(const std::string& myname);
      ~TrigEgammaEmulationChain()=default;

      StatusCode initialize();

      asg::AcceptData emulate( const Trig::TrigData &input ) const;

      std::string getChain() const{return m_chain;};

      std::string getSignature() const{return m_signature;};

      std::string getSeed() const{return m_seed;};

      
    private:


      /*! L1Calo hypo tool (step 0)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_l1caloTool{this,"L1CaloTool", "Step 0"};
      /*! Fast Calo hypo tool (step 1)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_fastCaloTool{this,"FastCaloTool", "Step 1"};
      /*! Fast Electron/Photon hypo tool (step 2)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_fastTool{this,"FastTool", "Step 2"};    
      /*! Precision Calo hypo tool  (step 3)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_precisionCaloTool{this,"PrecisionCaloTool", "Step 3"};    
       /*! Precision Tracking hypo tool  (step 4)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_precisionTrackingTool{this,"PrecisionTrackingTool", "Step 4"};    
      /*! Precision Electron/Photon hypo tool (step 5)*/
      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool> m_precisionTool{this,"PrecisionTool", "Step 5"};          


      /* chain name */
      Gaudi::Property< std::string> m_chain{this, "Chain", "", "Chain name"};
      /* L1 Item */
      Gaudi::Property< std::string> m_seed{this, "L1Item", "", "L1 Item"};
      /* trigger signature */
      Gaudi::Property< std::string> m_signature{this, "Signature", "", "Signature name"};

      /*! Accept */
      asg::AcceptInfo m_accept;
  };

}//namespace
#endif
