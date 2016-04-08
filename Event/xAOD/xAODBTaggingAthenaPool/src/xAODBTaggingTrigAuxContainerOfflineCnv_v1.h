// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerOfflineCnv_v1.h 705820 2015-11-04 14:54:05Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"
#include "xAODBTagging/BTaggingTrigAuxContainer.h"

/// Converter class used to read xAOD::BTaggingAuxContainer_v1
///
/// Since we switched from using the offline xAOD::BTaggingAuxContainer
/// type to using a trigger specific xAOD::BTaggingTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 705820 $
/// $Date: 2015-11-04 15:54:05 +0100 (Wed, 04 Nov 2015) $
///
class xAODBTaggingTrigAuxContainerOfflineCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::BTaggingTrigAuxContainer,
                                 xAOD::BTaggingAuxContainer_v1 > {

public:
   /// Default constructor
   xAODBTaggingTrigAuxContainerOfflineCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::BTaggingAuxContainer_v1* oldObj,
                             xAOD::BTaggingTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::BTaggingTrigAuxContainer*,
                             xAOD::BTaggingAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODBTaggingTrigAuxContainerOfflineCnv_v1

#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H
