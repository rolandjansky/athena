// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.h 757270 2016-06-23 13:52:41Z krasznaa $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// Forward declaration(s):
class IxAODClusterCompressor;

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CaloClusterAuxContainer,
                               xAOD::CaloClusterAuxContainer >
   xAODCaloClusterAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloClusterAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution. Because ROOT can't handle the change in inheritance
 *         that was done between _v1 and _v2.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 757270 $
 * $Date: 2016-06-23 15:52:41 +0200 (Thu, 23 Jun 2016) $
 */
class xAODCaloClusterAuxContainerCnv : public xAODCaloClusterAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODCaloClusterAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CaloClusterAuxContainer*
   createPersistent( xAOD::CaloClusterAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CaloClusterAuxContainer* createTransient();

private:
#ifndef XAOD_ANALYSIS
  /// AlgTool compressing the cluster for storage on disk
  ToolHandle<IxAODClusterCompressor> m_compressor;
#endif

  /// Flag set to false if the retrieval of the compression tool failed
  bool m_doCompression;

}; // class xAODCaloClusterAuxContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
