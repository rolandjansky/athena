// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_MUONFEATUREDETAILSGETTERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATUREDETAILSGETTERTOOL_H

// Trigger include(s):
#include "TrigMuonEvent/MuonFeatureDetails.h"

// Local include(s):
#include "TriggerD3PDMaker/TriggerObjectGetterTool.h"

namespace D3PD {

   /**
    *  @short This tool can be used to select MFD objects from specific chains
    *
    *         If only the MuonFeatureDetails objects from specific chains have to be
    *         saved, then this is the tool to use.
    *
    * @author Takayuki Kanno
    *
    * $Revision: 314014 $
    * $Date$
    */
   class MuonFeatureDetailsGetterTool : public TriggerObjectGetterTool< MuonFeatureDetails > {

   public:
      MuonFeatureDetailsGetterTool( const std::string& type, const std::string& name,
                                    const IInterface* parent )
         : TriggerObjectGetterTool< MuonFeatureDetails >( type, name, parent ) {}

   }; // class MuonFeatureDetailsTrigGetterTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATUREDETAILSGETTERTOOL_H
