// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_H
#define XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_H

#if !(defined(GENERATIONBASE) || defined(SIMULATIONBASE))

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJetTrigAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::JetTrigAuxContainer,
                                     xAODJetTrigAuxContainerCnv_v1 >
   xAODJetTrigAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetTrigAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JetContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 566967 $
 * $Date: 2013-10-24 15:24:31 +0200 (Thu, 24 Oct 2013) $
 */
class xAODJetTrigAuxContainerCnv :
   public xAODJetTrigAuxContainerCnvBase
{
public:
  using xAODJetTrigAuxContainerCnvBase::xAODJetTrigAuxContainerCnvBase;

  /// Function preparing the container to be written out
  virtual xAOD::JetTrigAuxContainer*
  createPersistentWithKey( xAOD::JetTrigAuxContainer* trans,
                           const std::string& key );

}; // class xAODJetTrigAuxContainerCnv
#endif // NOT SIMULATIONBASE OR GENERATIONBASE
#endif // XAODJETATHENAPOOL_XAODJETAUXCONTAINERCNV_H
