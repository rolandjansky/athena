/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationChain_h
#define TrigEgammaEmulationChain_h

#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
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

      StatusCode initialize() override;

      asg::AcceptData emulate( const Trig::TrigData &input ) const override;

      std::string chain() const override {return m_chain;};

      std::string signature() const override {return m_signature;};

      
    private:


      ToolHandleArray< Trig::ITrigEgammaEmulationBaseHypoTool > m_steps{this, "Steps", {}, ""};

      ToolHandle<Trig::ITrigEgammaEmulationBaseHypoTool > m_l1Seed{this, "L1Seed", ""};
      /* chain name */
      Gaudi::Property< std::string> m_chain{this, "Chain", "", "Chain name"};
      /* trigger signature */
      Gaudi::Property< std::string> m_signature{this, "Signature", "", "Signature name"};

      /*! Accept */
      asg::AcceptInfo m_accept;
  };

}//namespace
#endif
