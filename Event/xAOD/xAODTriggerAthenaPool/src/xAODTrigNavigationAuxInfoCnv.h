// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigNavigationAuxInfoCnv.h 674249 2015-06-11 08:47:11Z krasznaa $
#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGNAVIGATIONAUXINFOCNV_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGNAVIGATIONAUXINFOCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/TrigNavigationAuxInfo.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigNavigationAuxInfo,
                               xAOD::TrigNavigationAuxInfo >
   xAODTrigNavigationAuxInfoCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigNavigationAuxInfo class
 *
 *         This converter takes care of applying future schema evolution
 *         to xAOD::TrigNavigationAuxInfo, and for applying navigation
 *         thinning while writing a file.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 *
 * $Revision: 674249 $
 * $Date: 2015-06-11 10:47:11 +0200 (Thu, 11 Jun 2015) $
 */
class xAODTrigNavigationAuxInfoCnv :
   public xAODTrigNavigationAuxInfoCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigNavigationAuxInfoCnv >;

protected:
   /// Converter constructor
   xAODTrigNavigationAuxInfoCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigNavigationAuxInfo*
   createPersistent( xAOD::TrigNavigationAuxInfo* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigNavigationAuxInfo* createTransient();

}; // class xAODTrigNavigationAuxInfoCnv

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGNAVIGATIONAUXINFOCNV_H
