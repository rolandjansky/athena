// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id$
#ifndef XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_V1_H
#define XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v1.h"
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"
       
/// Converter class used for reading xAOD::TrackCaloClusterAuxContainer_v1
///
/// This converter implements the conversion from xAOD::TrackCaloClusterAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Noemi Calace <Noemi.Calace@cern.ch> (copied from convertor from Attila)
///
/// $Revision$
/// $Date$
///
class xAODTrackCaloClusterAuxContainerCnv_v1 :
  public T_AthenaPoolTPCnvConstBase< xAOD::TrackCaloClusterAuxContainer, xAOD::TrackCaloClusterAuxContainer_v1 > 
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  /// Default constructor
  xAODTrackCaloClusterAuxContainerCnv_v1();

  /// Function converting from the old type to the current one
  virtual void persToTrans( const xAOD::TrackCaloClusterAuxContainer_v1* oldObj,
                            xAOD::TrackCaloClusterAuxContainer* newObj,
                            MsgStream& log ) const override;
  /// Dummy function inherited from the base class
  virtual void transToPers( const xAOD::TrackCaloClusterAuxContainer*,
                            xAOD::TrackCaloClusterAuxContainer_v1*,
                            MsgStream& log ) const override;

}; // class xAODTrackCaloClusterAuxContainerCnv_v1
       
#endif // XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_V1_H
