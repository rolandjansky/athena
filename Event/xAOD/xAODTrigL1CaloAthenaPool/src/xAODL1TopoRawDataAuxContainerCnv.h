/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATAAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATAAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::L1TopoRawDataAuxContainer,
                               xAOD::L1TopoRawDataAuxContainer >
   xAODL1TopoRawDataAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::L1TopoRawDataAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::L1TopoRawDataContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Simon George <s.george@rhul.ac.uk>
 *
 * $Revision: 652829 $
 * $Date: 2015-03-10 00:52:50 +0100 (Tue, 10 Mar 2015) $
 */
class xAODL1TopoRawDataAuxContainerCnv :
   public xAODL1TopoRawDataAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODL1TopoRawDataAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODL1TopoRawDataAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::L1TopoRawDataAuxContainer*
   createPersistent( xAOD::L1TopoRawDataAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::L1TopoRawDataAuxContainer* createTransient();

}; // class xAODL1TopoRawDataAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATAAUXCONTAINERCNV_H
