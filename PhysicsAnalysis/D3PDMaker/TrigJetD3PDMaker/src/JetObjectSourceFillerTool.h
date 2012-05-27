// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectSourceFillerTool.h 358948 2011-04-15 09:05:30Z krasznaa $
#ifndef TRIGJETD3PDMAKER_JETOBJECTSOURCEFILLERTOOL_H
#define TRIGJETD3PDMAKER_JETOBJECTSOURCEFILLERTOOL_H

// EDM include(s):
#include "JetEvent/Jet.h"

// D3PDMaker include(s):
#include "TriggerD3PDMaker/TriggerObjectSourceFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool for flagging EF jets based on their "source"
    *
    *         This tool can be used to attach flags to the EF jet
    *         objects based on which algorithm they were reconstructed
    *         with. See the documentation of the base class for more
    *         details.
    *
    * @author Attila Krasznahroay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 358948 $
    * $Date: 2011-04-15 11:05:30 +0200 (Fri, 15 Apr 2011) $
    */
   class JetObjectSourceFillerTool :
      public TriggerObjectSourceFillerTool< Jet > {

   public:
      /// Regular AlgTool constructor
      JetObjectSourceFillerTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

   }; // class JetObjectSourceFillerTool

} // namespace D3PD

#endif // TRIGJETD3PDMAKER_JETOBJECTSOURCEFILLERTOOL_H
