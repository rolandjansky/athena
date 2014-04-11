// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv.h 592615 2014-04-11 14:36:58Z krasznaa $
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::L2StandAloneMuonAuxContainer,
                               xAOD::L2StandAloneMuonAuxContainer >
   xAODL2StandAloneMuonAuxContainerCnvBase;

/**
 * $Revision: 592615 $
 * $Date: 2014-04-11 16:36:58 +0200 (Fri, 11 Apr 2014) $
 */
class xAODL2StandAloneMuonAuxContainerCnv :
   public xAODL2StandAloneMuonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL2StandAloneMuonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODL2StandAloneMuonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::L2StandAloneMuonAuxContainer*
   createPersistent( xAOD::L2StandAloneMuonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::L2StandAloneMuonAuxContainer* createTransient();

}; // class xAODL2StandAloneMuonAuxContainerCnv

#endif // XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H
