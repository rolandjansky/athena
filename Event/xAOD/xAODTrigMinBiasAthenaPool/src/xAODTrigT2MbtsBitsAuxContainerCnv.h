// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2MbtsBitsAuxContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsAUXCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigT2MbtsBitsAuxContainer,
                               xAOD::TrigT2MbtsBitsAuxContainer >
   xAODTrigT2MbtsBitsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigT2MbtsBitsAuxContainer class
 * @author Gordon Watts 
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigT2MbtsBitsAuxContainerCnv :
   public xAODTrigT2MbtsBitsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigT2MbtsBitsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigT2MbtsBitsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigT2MbtsBitsAuxContainer*
   createPersistent( xAOD::TrigT2MbtsBitsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigT2MbtsBitsAuxContainer* createTransient();

}; // class xAODTrigT2MbtsBitsAuxContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigT2MbtsBitsAUXCONTAINERCNV_H
