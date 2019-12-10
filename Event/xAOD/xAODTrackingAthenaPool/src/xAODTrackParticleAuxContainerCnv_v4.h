// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V4_H
#define XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V4_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTracking/versions/TrackParticleAuxContainer_v4.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
	
/// Converter class used for reading xAOD::TrackParticleAuxContainer_v4
///
/// This converter implements the conversion from xAOD::TrackParticleAuxContainer_v4
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Edward.Moyse@cern.ch (copied from convertor from Attila)
///
/// $Revision: 751893 $
/// $Date$
///
class xAODTrackParticleAuxContainerCnv_v4 :
  public T_AthenaPoolTPCnvConstBase< xAOD::TrackParticleAuxContainer, xAOD::TrackParticleAuxContainer_v4 >
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;


  /// Default constructor
  xAODTrackParticleAuxContainerCnv_v4();

  /// Function converting from the old type to the current one
  virtual void persToTrans( const xAOD::TrackParticleAuxContainer_v4* oldObj,
                            xAOD::TrackParticleAuxContainer* newObj,
                            MsgStream& log ) const override;
  /// Dummy function inherited from the base class
  virtual void transToPers( const xAOD::TrackParticleAuxContainer*,
                            xAOD::TrackParticleAuxContainer_v4*,
                            MsgStream& log ) const override;

}; // class xAODTrackParticleAuxContainerCnv_v4
	
#endif // XAODTRACKINGATHENAPOOL_XAODTRACKPARTICLEAUXCONTAINERCNV_V4_H
