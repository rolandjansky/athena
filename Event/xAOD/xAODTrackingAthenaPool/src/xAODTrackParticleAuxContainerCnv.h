// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.h 789663 2016-12-14 14:48:57Z krasznaa $
#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrackingCnv/ITrackParticleCompressorTool.h"

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

   /// Function initialising the converter
   StatusCode initialize() override;

protected:
   /// Converter constructor
   xAODTrackParticleAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleAuxContainer*
   createPersistent( xAOD::TrackParticleAuxContainer* trans ) override;
   /// Function reading in the object from the input file
   virtual xAOD::TrackParticleAuxContainer* createTransient() override;

private:
   /// Compressor tool for the track particles
   ToolHandle< xAODMaker::ITrackParticleCompressorTool > m_compressorTool;

}; // class xAODTrackParticleAuxContainerCnv

#endif 
