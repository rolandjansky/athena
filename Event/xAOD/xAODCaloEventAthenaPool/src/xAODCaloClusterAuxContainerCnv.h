// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.h 757270 2016-06-23 13:52:41Z krasznaa $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloClusterAuxContainerCnv_v1.h"
#ifndef XAOD_ANALYSIS
#include "CaloInterface/IxAODClusterCompressor.h"
#endif

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::CaloClusterAuxContainer,
                                     xAODCaloClusterAuxContainerCnv_v1 >
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
public:
   xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc );
protected:

   /// Function preparing the container to be written out
   virtual xAOD::CaloClusterAuxContainer*
   createPersistentWithKey( xAOD::CaloClusterAuxContainer* trans,
                            const std::string& key );

private:
#ifndef XAOD_ANALYSIS
  /// Service compressing the cluster for storage on disk
  ServiceHandle<IxAODClusterCompressor> m_compressor;
#endif

  /// Flag set to false if the retrieval of the compression service failed
  bool m_doCompression;

}; // class xAODCaloClusterAuxContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
