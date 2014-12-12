/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODSLOWMUONATHENAPOOL_XAODSLOWMUONAUXCONTAINERCNV_H
#define XAODSLOWMUONATHENAPOOL_XAODSLOWMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#define private public
#include "xAODMuon/SlowMuonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::SlowMuonAuxContainer,
                               xAOD::SlowMuonAuxContainer >
   xAODSlowMuonAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::SlowMuonAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::SlowMuonContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Sue Cheatham <susan.cheatham@cern.ch>
 * copied from MuonCnv by
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 589597 $
 * $Date: 2014-03-26 18:03:04 +0100 (Wed, 26 Mar 2014) $
 */
class xAODSlowMuonAuxContainerCnv :
   public xAODSlowMuonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODSlowMuonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODSlowMuonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::SlowMuonAuxContainer*
   createPersistent( xAOD::SlowMuonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::SlowMuonAuxContainer* createTransient();

}; // class xAODSlowMuonAuxContainerCnv

#endif // XAODSLOWMUONATHENAPOOL_XAODSLOWMUONAUXCONTAINERCNV_H
