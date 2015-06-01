/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H
#define XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#define private public
#include "xAODMuon/MuonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::MuonAuxContainer,
                               xAOD::MuonAuxContainer >
   xAODMuonAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MuonAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::MuonContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 671383 $
 * $Date: 2015-06-01 16:41:07 +0200 (Mon, 01 Jun 2015) $
 */
class xAODMuonAuxContainerCnv :
   public xAODMuonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMuonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODMuonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::MuonAuxContainer*
   createPersistent( xAOD::MuonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::MuonAuxContainer* createTransient();

}; // class xAODMuonAuxContainerCnv

#endif // XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H
