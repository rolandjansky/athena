// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureGetterTool.h 282498 2010-03-03 15:36:37Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_MUONFEATUREGETTERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATUREGETTERTOOL_H

// Trigger include(s):
#include "TrigMuonEvent/MuonFeature.h"

// Local include(s):
#include "TriggerD3PDMaker/TriggerObjectGetterTool.h"

namespace D3PD {

   /**
    *  @short This tool can be used to select MF objects from specific chains
    *
    *         If only the MuonFeature objects from specific chains have to be
    *         saved, then this is the tool to use.
    *
    * @author Attila Krasznahorkay
    *
    * $Revision: 282498 $
    * $Date: 2010-03-03 16:36:37 +0100 (Wed, 03 Mar 2010) $
    */
   class MuonFeatureGetterTool : public TriggerObjectGetterTool< MuonFeature > {

   public:
      MuonFeatureGetterTool( const std::string& type, const std::string& name,
                             const IInterface* parent )
         : TriggerObjectGetterTool< MuonFeature >( type, name, parent ) {}

   }; // class MuonFeatureTrigGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATUREGETTERTOOL_H
