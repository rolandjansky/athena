// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventInfoContainerCnv.h 594317 2014-04-25 17:36:58Z krasznaa $
#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEventInfo/EventInfoContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::EventInfoContainer,
                               xAOD::EventInfoContainer >
   xAODEventInfoContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::EventInfoContainer class
 *
 *         A custom POOL converter is only needed to make sure that the
 *         EventInfoContainer gets connected to its auxiliary store correctly
 *         in case it is read in using a DataLink/ElementLink from an input
 *         file.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 594317 $
 * $Date: 2014-04-25 19:36:58 +0200 (Fri, 25 Apr 2014) $
 */
class xAODEventInfoContainerCnv : public xAODEventInfoContainerCnvBase {

   // Declare the factory as a friend of this class:
   friend class CnvFactory< xAODEventInfoContainerCnv >;

public:
   /// Converter constructor
   xAODEventInfoContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the object to be written out
   virtual xAOD::EventInfoContainer*
   createPersistent( xAOD::EventInfoContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::EventInfoContainer* createTransient();

private:
   /// StoreGate key of the container being read
   std::string m_key;

}; // class xAODEventInfoContainerCnv

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H
