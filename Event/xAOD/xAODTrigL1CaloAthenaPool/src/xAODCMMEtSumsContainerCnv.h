/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMEtSumsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMMEtSumsContainer,
                               xAOD::CMMEtSumsContainer >
   xAODCMMEtSumsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CMMEtSumsContainer class
 *
 *         Simple converter class making the xAOD::CMMEtSumsContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCMMEtSumsContainerCnv : public xAODCMMEtSumsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCMMEtSumsContainerCnv >;

public:
   /// Converter constructor
   xAODCMMEtSumsContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );    
   /// Function preparing the container to be written out
   virtual xAOD::CMMEtSumsContainer*
   createPersistent( xAOD::CMMEtSumsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CMMEtSumsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CMMEtSums* cluster ) const;
   
   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODCMMEtSumsContainerCnv

#endif 
