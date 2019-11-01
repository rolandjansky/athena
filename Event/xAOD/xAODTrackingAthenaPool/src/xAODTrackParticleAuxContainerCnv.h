// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.h 789663 2016-12-14 14:48:57Z krasznaa $
#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrackingCnv/ITrackParticleCompressorTool.h"
#include "xAODTrackParticleAuxContainerCnv_v1.h"
#include "xAODTrackParticleAuxContainerCnv_v2.h"
#include "xAODTrackParticleAuxContainerCnv_v3.h"
#include "xAODTrackParticleAuxContainerCnv_v4.h"


/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv<xAOD::TrackParticleAuxContainer,
                                    xAODTrackParticleAuxContainerCnv_v4,
                                    xAODTrackParticleAuxContainerCnv_v3,
                                    xAODTrackParticleAuxContainerCnv_v2,
                                    xAODTrackParticleAuxContainerCnv_v1>
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
   public xAODTrackParticleAuxContainerCnvBase
{

public:
   /// Converter constructor
   xAODTrackParticleAuxContainerCnv( ISvcLocator* svcLoc );

private:

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackParticleAuxContainerCnv >;

   /// Function initialising the converter
   StatusCode initialize() override;

protected:
   /// Function preparing the container to be written out
   virtual xAOD::TrackParticleAuxContainer*
   createPersistentWithKey( xAOD::TrackParticleAuxContainer* trans,
                            const std::string& key ) override;

private:
   /// Compressor tool for the track particles
   ToolHandle< xAODMaker::ITrackParticleCompressorTool > m_compressorTool;

}; // class xAODTrackParticleAuxContainerCnv

#endif 
