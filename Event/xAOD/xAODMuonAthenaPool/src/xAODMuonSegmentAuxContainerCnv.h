/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONATHENAPOOL_XAODMUONSEGMENTAUXCONTAINERCNV_H
#define XAODMUONATHENAPOOL_XAODMUONSEGMENTAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMuon/MuonSegmentAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::MuonSegmentAuxContainer,
                               xAOD::MuonSegmentAuxContainer >
   xAODMuonSegmentAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MuonSegmentAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::MuonSegmentContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse
 */
class xAODMuonSegmentAuxContainerCnv :
   public xAODMuonSegmentAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMuonSegmentAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODMuonSegmentAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::MuonSegmentAuxContainer*
   createPersistent( xAOD::MuonSegmentAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::MuonSegmentAuxContainer* createTransient();

}; // class xAODMuonSegmentAuxContainerCnv

#endif // XAODMUONATHENAPOOL_XAODMUONSEGMENTAUXCONTAINERCNV_H
