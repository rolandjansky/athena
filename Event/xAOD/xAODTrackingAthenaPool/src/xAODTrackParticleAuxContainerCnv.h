// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.h 609811 2014-08-01 15:47:00Z ssnyder $
#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTracking/TrackParticleAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrackParticleAuxContainer,
                               xAOD::TrackParticleAuxContainer >
   xAODTrackParticleAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackParticleAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackParticleContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODTrackParticleAuxContainerCnv :
   public xAODTrackParticleAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackParticleAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrackParticleAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleAuxContainer*
   createPersistent( xAOD::TrackParticleAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrackParticleAuxContainer* createTransient();

}; // class xAODTrackParticleAuxContainerCnv

#endif 
