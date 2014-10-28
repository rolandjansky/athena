// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2ZdcSignalsAuxContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsAUXCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigT2ZdcSignalsAuxContainer,
                               xAOD::TrigT2ZdcSignalsAuxContainer >
   xAODTrigT2ZdcSignalsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigT2ZdcSignalsAuxContainer class
 * @author Gordon Watts 
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigT2ZdcSignalsAuxContainerCnv :
   public xAODTrigT2ZdcSignalsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigT2ZdcSignalsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigT2ZdcSignalsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigT2ZdcSignalsAuxContainer*
   createPersistent( xAOD::TrigT2ZdcSignalsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigT2ZdcSignalsAuxContainer* createTransient();

}; // class xAODTrigT2ZdcSignalsAuxContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigT2ZdcSignalsAUXCONTAINERCNV_H
