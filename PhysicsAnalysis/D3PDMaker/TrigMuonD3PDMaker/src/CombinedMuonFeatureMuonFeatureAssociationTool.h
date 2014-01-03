// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureMuonFeatureAssociationTool.h 282713 2010-03-04 13:21:48Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREMUONFEATUREASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREMUONFEATUREASSOCIATIONTOOL_H

// EDM include(s):
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    *  @short Tool "associating" MuonFeature objects to CombinedMuonFeature ones
    *
    *         Each CombinedMuonFeature objects points to a MuonFeature object. This
    *         relationship can be expressed towards the D3PDMaker code using this
    *         associator tool. It tells the framework which MuonFeature object
    *         belongs to a given CombinedMuonFeature object.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 282713 $
    * $Date: 2010-03-04 14:21:48 +0100 (Thu, 04 Mar 2010) $
    */
   class CombinedMuonFeatureMuonFeatureAssociationTool :
      public SingleAssociationTool< CombinedMuonFeature, MuonFeature > {

   public:
      /// Regular AlgTool constructor
      CombinedMuonFeatureMuonFeatureAssociationTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent );

      /// Function performing the association
      virtual const MuonFeature* get( const CombinedMuonFeature& muon );

   }; // class CombinedMuonFeatureMuonFeatureAssociationTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREMUONFEATUREASSOCIATIONTOOL_H
