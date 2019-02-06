// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file xAODTriggerAthenaPool/xAODTrigCompositeAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2018
 * @brief 
 */


#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H

// stystem includes
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/TrigCompositeAuxContainer.h"


/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigCompositeAuxContainer,
                               xAOD::TrigCompositeAuxContainer >
   xAODTrigCompositeAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigCompositeAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package via the Aux container
 *
 */
class xAODTrigCompositeAuxContainerCnv :
   public xAODTrigCompositeAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigCompositeAuxContainerCnv >;

protected:
   /// Converter constructor
public:
   xAODTrigCompositeAuxContainerCnv( ISvcLocator* svcLoc );
protected:

   /// Function preparing the container to be written out
   virtual xAOD::TrigCompositeAuxContainer*
   createPersistent( xAOD::TrigCompositeAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigCompositeAuxContainer* createTransient();

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

private:
  std::string m_key;


}; // class xAODTrigCompositeAuxContainerCnv

#endif // not XAODTRIGGERATHENAPOOL_XAODTRIGCOMPOSITEAUXCONTAINER_H
