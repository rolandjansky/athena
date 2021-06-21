/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationL1CaloHypoTool_h
#define TrigEgammaEmulationL1CaloHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"



namespace Trig{


  class TrigEgammaEmulationL1CaloHypoTool: public TrigEgammaEmulationBaseHypoTool,
                                           virtual public ITrigEgammaEmulationBaseHypoTool

  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationL1CaloHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationL1CaloHypoTool(const std::string& myname);
      ~TrigEgammaEmulationL1CaloHypoTool()=default;


      virtual bool emulate( const TrigData &, bool &) const override;

      
    private:
    
        bool decide(  const Trig::TrigData &input ) const;
        bool isolationL1(float min, float offset, float slope, float energy, float emE) const;
        bool variableEtL1(std::string L1item, float l1energy, float l1eta) const;
        float emulationL1V(std::string L1item, float l1eta) const;

        // L1 configuration parameters
        Gaudi::Property<std::string>  m_l1item{this, "L1Item", "", "" }; 
        Gaudi::Property<float>        m_l1threshold{this, "L1Thr", 0};
        Gaudi::Property<float>        m_hadCoreCutMin{this  , "HadCoreCutMin", 0  };
        Gaudi::Property<float>        m_hadCoreCutOff{this  , "HadCoreCutOff", 0  };
        Gaudi::Property<float>        m_hadCoreCutSlope{this, "HadCoreSlope" , 0  };
        Gaudi::Property<float>        m_emIsolCutMin{this   , "EmIsolCutMin" , 0  };
        Gaudi::Property<float>        m_emIsolCutOff{this   , "EmIsolCutOff" , 0  };
        Gaudi::Property<float>        m_emIsolCutSlope{this , "EmIsolSlope"  , 0  };
        Gaudi::Property<float>        m_isolMaxCut{this     , "IsolCutMax"   , 50 };

  };
}//namespace
#endif
