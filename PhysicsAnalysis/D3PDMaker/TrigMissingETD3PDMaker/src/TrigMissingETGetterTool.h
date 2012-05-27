// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETGetterTool.h 278517 2010-02-10 16:46:44Z krasznaa $
#ifndef TRIGMISSINGETD3PDMAKER_TRIGMISSINGETGETTERTOOL_H
#define TRIGMISSINGETD3PDMAKER_TRIGMISSINGETGETTERTOOL_H

// EDM include(s):
#include "TrigMissingEtEvent/TrigMissingET.h"

// D3PD include(s):
#include "TriggerD3PDMaker/TriggerObjectGetterTool.h"

namespace D3PD {

   /**
    *  @short Tool retrieving TrigMissingET objects from specific HLT chains
    *
    *         This is just here in "experimental mode". By default the TrigMissingET
    *         objects are picked up from StoreGate directly. But this tool allows us
    *         to select TrigMissingET objects only from specific HLT chains.
    *
    * @author Attila Krasznahorkay Jr.
    * @author Diego Casadei
    *
    * $Revision: 278517 $
    * $Date: 2010-02-10 17:46:44 +0100 (Wed, 10 Feb 2010) $
    */
   class TrigMissingETGetterTool :
      public TriggerObjectGetterTool< TrigMissingET > {

   public:
      /// Regular AlgTool constructor
      TrigMissingETGetterTool( const std::string& type, const std::string& name,
                               const IInterface* parent )
         : TriggerObjectGetterTool< TrigMissingET >( type, name, parent ) {}

   }; // class TrigMissingETGetterTool

} // namespace D3PD

#endif // TRIGMISSINGETD3PDMAKER_TRIGMISSINGETGETTERTOOL_H
