// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventShapeCnv.h 632142 2014-11-29 12:59:50Z krasznaa $
#ifndef XAODEVENTSHAPEATHENAPOOL_XAODEVENTSHAPECNV_H
#define XAODEVENTSHAPEATHENAPOOL_XAODEVENTSHAPECNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEventShape/EventShape.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::EventShape, xAOD::EventShape >
   xAODEventShapeCnvBase;

/**
 *  @short POOL converter for the xAOD::EventShape class
 *
 *         A custom POOL converter is only needed to make sure that the
 *         EventShape object gets connected to its auxiliary store correctly
 *         in case it is read in using a DataLink from an input file.
 *
 * @author P-A Delsart 
 *
 * $Revision: 632142 $
 * $Date: 2014-11-29 13:59:50 +0100 (Sat, 29 Nov 2014) $
 */
class xAODEventShapeCnv : public xAODEventShapeCnvBase {

   // Declare the factory as a friend of this class:
   friend class CnvFactory< xAODEventShapeCnv >;

public:
   /// Converter constructor
   xAODEventShapeCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the object to be written out
   virtual xAOD::EventShape* createPersistent( xAOD::EventShape* trans );
   /// Function reading in the persistent object
   virtual xAOD::EventShape* createTransient();

private:
   /// StoreGate key of the object being read
   std::string m_key;

}; // class xAODEventShapeCnv

#endif // XAODEVENTSHAPEATHENAPOOL_XAODEVENTSHAPECNV_H
