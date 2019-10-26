// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysAuxContainerCnv.h
#ifndef xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H
#define xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrigBphysAuxContainer >
   xAODTrigBphysAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigBphysAuxContainer class
 *
 * @author Your name here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigBphysAuxContainerCnv :
   public xAODTrigBphysAuxContainerCnvBase
{
public:
  using xAODTrigBphysAuxContainerCnvBase::xAODTrigBphysAuxContainerCnvBase;

  /// Function preparing the container to be written out
  virtual xAOD::TrigBphysAuxContainer*
  createPersistentWithKey( xAOD::TrigBphysAuxContainer* trans,
                           const std::string& key) override;
}; // class xAODTrigBphysAuxContainerCnv


#endif // xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H
