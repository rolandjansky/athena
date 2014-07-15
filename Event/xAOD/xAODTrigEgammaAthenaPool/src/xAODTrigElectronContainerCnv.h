// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigElectronContainerCnv.h
#ifndef xAODTrigEgammaATHENAPOOL_XAODTrigElectronCONTAINERCNV_H
#define xAODTrigEgammaATHENAPOOL_XAODTrigElectronCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrigElectronContainer,
                               xAOD::TrigElectronContainer >
   xAODTrigElectronContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigElectronContainer class
 *
 *         Simple converter class making the xAOD::TrigElectronContainer
 *         class known to POOL.
 *
 * @author Your Name Here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigElectronContainerCnv : public xAODTrigElectronContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigElectronContainerCnv >;

public:
   /// Converter constructor
   xAODTrigElectronContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   /// Function preparing the container to be written out
   virtual xAOD::TrigElectronContainer*
   createPersistent( xAOD::TrigElectronContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::TrigElectronContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrigElectron* cluster ) const;
   /// StoreGate key for the container just being created
   std::string m_key;

}; // class xAODTrigElectronContainerCnv

#endif // xAODTrigEgammaATHENAPOOL_XAODTrigElectronCONTAINERCNV_H
