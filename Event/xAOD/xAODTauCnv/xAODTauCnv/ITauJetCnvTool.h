// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITauJetCnvTool.h 785754 2016-11-22 15:16:29Z ssnyder $
#ifndef XAODTAUJETCNV_ITAUJETCNVTOOL_H
#define XAODTAUJETCNV_ITAUJETCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h" 

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
    * $Revision: 785754 $
    * $Date: 2016-11-22 16:16:29 +0100 (Tue, 22 Nov 2016) $
    */
   class ITauJetCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::TauJetContainer
     /* virtual StatusCode convert( const DataVector<taujet>* aod, */
     /*                             xAOD::TauJetContainer* xaod) const = 0; */
     virtual StatusCode convert(const Analysis::TauJetContainer* inputTaus,
				xAOD::TauJetContainer* xAODTauJets) const = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_ITauJetCnvTool;
     }
     
   }; // class ITauJetCnvTool
 
} // namespace xAODMaker

#endif // XAODTAUJETCNV_ITAUJETCNVTOOL_H
