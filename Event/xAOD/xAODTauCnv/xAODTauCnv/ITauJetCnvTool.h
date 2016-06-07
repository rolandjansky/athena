// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITauJetCnvTool.h 730442 2016-03-16 15:21:06Z griffith $
#ifndef XAODTAUJETCNV_ITAUJETCNVTOOL_H
#define XAODTAUJETCNV_ITAUJETCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h" 

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "tauEvent/TauJetContainer.h"

// Forward declaration(s):
/* class taujet; */

namespace xAODMaker {

   /// The interface provided by ITauJetCnvTool
   static const InterfaceID
   IID_ITauJetCnvTool( "xAODMaker::ITauJetCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TauJetContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         TauJet Container into an xAOD::TauJetContainer.
    *
    * @author Michel Janus <janus@cern.ch>
    *
    * $Revision: 730442 $
    * $Date: 2016-03-16 16:21:06 +0100 (Wed, 16 Mar 2016) $
    */
   class ITauJetCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::TauJetContainer
     /* virtual StatusCode convert( const DataVector<taujet>* aod, */
     /*                             xAOD::TauJetContainer* xaod) const = 0; */
     virtual StatusCode convert(const Analysis::TauJetContainer* inputTaus,
				xAOD::TauJetContainer* xAODTauJets) = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_ITauJetCnvTool;
     }
     
   }; // class ITauJetCnvTool
 
} // namespace xAODMaker

#endif // XAODTAUJETCNV_ITAUJETCNVTOOL_H
