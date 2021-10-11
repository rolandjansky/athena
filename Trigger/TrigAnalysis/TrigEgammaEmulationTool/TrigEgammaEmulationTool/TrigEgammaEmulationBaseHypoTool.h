/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationBaseHypoTool_h
#define TrigEgammaEmulationBaseHypoTool_h

#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "GaudiKernel/SystemOfUnits.h"


namespace Trig{


  class TrigEgammaEmulationBaseHypoTool: public asg::AsgTool,
                                         virtual public ITrigEgammaEmulationBaseHypoTool
  { 

    ASG_TOOL_CLASS( TrigEgammaEmulationBaseHypoTool, ITrigEgammaEmulationBaseHypoTool )

    public:

      TrigEgammaEmulationBaseHypoTool(const std::string& myname);
      ~TrigEgammaEmulationBaseHypoTool()=default;

      virtual StatusCode initialize() override;

      virtual bool emulate( const TrigData &, bool &) const override {return true;};

    protected:
      
      float avgmu() const ;

      /*! Trigger decision tool */
      ToolHandle<Trig::TrigDecisionTool> m_trigdec;
      ToolHandle<ILumiBlockMuTool>       m_lumiBlockMuTool;

  };


}//namespace
#endif
