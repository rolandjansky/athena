// Dear emacs, this is -*- c++ -*-
// $Id: xAODTrackCaloClusterAuxContainerCnv.h $
#ifndef XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
#define XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrackCaloClusterAuxContainer,
                               xAOD::TrackCaloClusterAuxContainer >
   xAODTrackCaloClusterAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrackCaloClusterAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TrackCaloClusterContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Noemi Calace <Noemi.Calace@cern.ch>
 */
class xAODTrackCaloClusterAuxContainerCnv :
   public xAODTrackCaloClusterAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrackCaloClusterAuxContainerCnv >;

   /// Function initialising the converter
   virtual StatusCode initialize() override;

protected:
   /// Converter constructor
   xAODTrackCaloClusterAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrackCaloClusterAuxContainer*
   createPersistent( xAOD::TrackCaloClusterAuxContainer* trans ) override;
   /// Function reading in the object from the input file
   virtual xAOD::TrackCaloClusterAuxContainer* createTransient() override;

private:

}; // class xAODTrackCaloClusterAuxContainerCnv

#endif // XAODPFLOWATHENAPOOL_XAODTRACKCALOCLUSTERAUXCONTAINERCNV_H
