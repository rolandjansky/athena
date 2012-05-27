// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectDecisionFillerTool.h 307763 2010-06-29 14:25:58Z krasznaa $
#ifndef TRIGJETD3PDMAKER_JETOBJECTDECISIONFILLERTOOL_H
#define TRIGJETD3PDMAKER_JETOBJECTDECISIONFILLERTOOL_H

// EDM include(s):
#include "JetEvent/Jet.h"

// D3PDMaker include(s):
#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool recording which chains a Jet object passed
    *
    * $Revision: 307763 $
    * $Date: 2010-06-29 16:25:58 +0200 (Tue, 29 Jun 2010) $
    */
   class JetObjectDecisionFillerTool :
      public TriggerObjectDecisionFillerTool< Jet > {

   public:
      /// Regular AlgTool constructor
      JetObjectDecisionFillerTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

   }; // class JetObjectDecisionFillerTool

} // namespace D3PD

#endif // TRIGJETD3PDMAKER_JETOBJECTDECISIONFILLERTOOL_H
