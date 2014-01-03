// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureGetterTool.h 282713 2010-03-04 13:21:48Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREGETTERTOOL_H
#define TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREGETTERTOOL_H

// Trigger include(s):
#include "TrigMuonEvent/CombinedMuonFeature.h"

// Local include(s):
#include "TriggerD3PDMaker/TriggerObjectGetterTool.h"

namespace D3PD {

   /**
    *  @short This tool can be used to select CMF objects from specific chains
    *
    *         If only the CombinedMuonFeature objects from specific chains have to be
    *         saved, then this is the tool to use.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 282713 $
    * $Date: 2010-03-04 14:21:48 +0100 (Thu, 04 Mar 2010) $
    */
   class CombinedMuonFeatureGetterTool :
      public TriggerObjectGetterTool< CombinedMuonFeature > {

   public:
      CombinedMuonFeatureGetterTool( const std::string& type, const std::string& name,
                                     const IInterface* parent )
         : TriggerObjectGetterTool< CombinedMuonFeature >( type, name, parent ) {}

   }; // class CombinedMuonFeatureTrigGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREGETTERTOOL_H
