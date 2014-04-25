// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventInfoCnv.h 592114 2014-04-09 12:00:50Z krasznaa $
#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::EventInfo, xAOD::EventInfo >
   xAODEventInfoCnvBase;

/**
 *  @short POOL converter for the xAOD::EventInfo class
 *
 *         A custom POOL converter is only needed to make sure that the
 *         EventInfo object gets connected to its auxiliary store correctly
 *         in case it is read in using a DataLink from an input file.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 592114 $
 * $Date: 2014-04-09 14:00:50 +0200 (Wed, 09 Apr 2014) $
 */
class xAODEventInfoCnv : public xAODEventInfoCnvBase {

   // Declare the factory as a friend of this class:
   friend class CnvFactory< xAODEventInfoCnv >;

public:
   /// Converter constructor
   xAODEventInfoCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the object to be written out
   virtual xAOD::EventInfo* createPersistent( xAOD::EventInfo* trans );
   /// Function reading in the persistent object
   virtual xAOD::EventInfo* createTransient();

private:
   /// StoreGate key of the object being read
   std::string m_key;

}; // class xAODEventInfoCnv

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H
