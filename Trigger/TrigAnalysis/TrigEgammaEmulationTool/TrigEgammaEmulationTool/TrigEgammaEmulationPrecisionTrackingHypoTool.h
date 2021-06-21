/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationPrecisionTrackingHypoTool_h
#define TrigEgammaEmulationPrecisionTrackingHypoTool_h

#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{

  class TrigEgammaEmulationPrecisionTrackingHypoTool: public TrigEgammaEmulationBaseHypoTool,
                                                virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationPrecisionTrackingHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationPrecisionTrackingHypoTool(const std::string& myname);
      ~TrigEgammaEmulationPrecisionTrackingHypoTool()=default;

      virtual bool emulate( const TrigData &input, bool &pass) const override;
      
    private:

  };

}//namespace
#endif
