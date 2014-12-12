/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKINGATHENAPOOL_XAODPREPRAWDATACONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODPREPRAWDATACONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackMeasurementValidationContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::TrackMeasurementValidationContainer,
                               xAOD::TrackMeasurementValidationContainer >
   xAODTrackMeasurementValidationContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackMeasurementValidationContainer class
 *
 *         Simple converter class making the xAOD::TrackMeasurementValidationContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 */
class xAODTrackMeasurementValidationContainerCnv : public xAODTrackMeasurementValidationContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackMeasurementValidationContainerCnv >;

public:
   /// Converter constructor
   xAODTrackMeasurementValidationContainerCnv( ISvcLocator* svcLoc );
   
   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
   
   /// Function preparing the container to be written out
   virtual xAOD::TrackMeasurementValidationContainer* createPersistent( xAOD::TrackMeasurementValidationContainer* trans );
   
   /// Function reading in the persistent object
   virtual xAOD::TrackMeasurementValidationContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::TrackMeasurementValidation* np ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODTrackMeasurementValidationContainerCnv

#endif 
