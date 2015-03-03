// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsoMuonFeatureGetterTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_ISOMUONFEATUREGETTERTOOL_H
#define TRIGMUOND3PDMAKER_ISOMUONFEATUREGETTERTOOL_H

// Trigger include(s):
#include "TrigMuonEvent/IsoMuonFeature.h"

// Local include(s):
#include "TriggerD3PDMaker/TriggerObjectGetterTool.h"

namespace D3PD {

   /**
    *  @short This tool can be used to select IMF objects from specific chains
    *
    *         If only the IsoMuonFeature objects from specific chains have to be
    *         saved, then this is the tool to use.
    *
    * @author Takayuki Kanno.
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class IsoMuonFeatureGetterTool :
      public TriggerObjectGetterTool< IsoMuonFeature > {

   public:
      IsoMuonFeatureGetterTool( const std::string& type, const std::string& name,
                                     const IInterface* parent )
         : TriggerObjectGetterTool< IsoMuonFeature >( type, name, parent ) {}

   }; // class IsoMuonFeatureTrigGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_ISOMUONFEATUREGETTERTOOL_H
