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
 * $Revision: 643853 $
 * $Date: 2015-02-03 16:27:45 +0100 (Tue, 03 Feb 2015) $
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

   /// Function to get the primary track particle, so we can set the charge from this...
   const ElementLink< xAOD::TrackParticleContainer >& getPrimaryTrackParticleLink(xAOD::MuonAuxContainer* aux,size_t i);


}; // class xAODMuonAuxContainerCnv

#endif // XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H
