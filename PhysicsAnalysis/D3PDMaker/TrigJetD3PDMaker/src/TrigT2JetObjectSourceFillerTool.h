/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETD3PDMAKER_TRIGT2JETOBJECTSOURCEFILLERTOOL_H
#define TRIGJETD3PDMAKER_TRIGT2JETOBJECTSOURCEFILLERTOOL_H

// EDM include(s):
#include "TrigCaloEvent/TrigT2Jet.h"

// D3PDMaker include(s):
#include "TriggerD3PDMaker/TriggerObjectSourceFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool for flagging EF jets based on their "source"
    *
    *         This tool can be used to attach flags to the TrigT2Jet
    *         objects based on which algorithm they were reconstructed
    *         with. See the documentation of the base class for more
    *         details.
    */
   class TrigT2JetObjectSourceFillerTool :
      public TriggerObjectSourceFillerTool< TrigT2Jet > {

   public:
      /// Regular AlgTool constructor
      TrigT2JetObjectSourceFillerTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

   }; // class TrigT2JetObjectSourceFillerTool

} // namespace D3PD

#endif // TRIGJETD3PDMAKER_TRIGT2JETOBJECTSOURCEFILLERTOOL_H
