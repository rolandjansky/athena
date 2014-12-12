// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V1_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTracking/versions/TrackParticleAuxContainer_v1.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
	
/// Converter class used for reading xAOD::TrackParticleAuxContainer_v1
///
/// This converter implements the conversion from xAOD::TrackParticleAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Edward.Moyse@cern.ch (copied from convertor from Attila)
///
/// $Revision$
/// $Date$
///
class xAODTrackParticleAuxContainerCnv_v1 :
  public T_AthenaPoolTPCnvBase< xAOD::TrackParticleAuxContainer, xAOD::TrackParticleAuxContainer_v1 > 
{
public:
  /// Default constructor
  xAODTrackParticleAuxContainerCnv_v1();

  /// Function converting from the old type to the current one
  virtual void persToTrans( const xAOD::TrackParticleAuxContainer_v1* oldObj,
                            xAOD::TrackParticleAuxContainer* newObj,
                            MsgStream& log );
  /// Dummy function inherited from the base class
  virtual void transToPers( const xAOD::TrackParticleAuxContainer*,
                            xAOD::TrackParticleAuxContainer_v1*,
                            MsgStream& log );

}; // class xAODTrackParticleAuxContainerCnv_v1
	
#endif // XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V1_H