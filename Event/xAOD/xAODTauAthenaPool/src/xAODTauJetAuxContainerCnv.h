// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.h 800296 2017-03-10 18:16:40Z griffith $
#ifndef XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
#define XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H

// stystem includes
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/TauJetAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TauJetAuxContainer,
                               xAOD::TauJetAuxContainer >
   xAODTauJetAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TauJetAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TauJetContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Michel Janus <janus@cern.ch>
 *
 * $Revision: 800296 $
 * $Date: 2017-03-10 19:16:40 +0100 (Fri, 10 Mar 2017) $
 */
class xAODTauJetAuxContainerCnv :
   public xAODTauJetAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTauJetAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTauJetAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TauJetAuxContainer*
   createPersistent( xAOD::TauJetAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TauJetAuxContainer* createTransient();

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

private:
  std::string m_key;


}; // class xAODTauJetAuxContainerCnv

#endif // XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
