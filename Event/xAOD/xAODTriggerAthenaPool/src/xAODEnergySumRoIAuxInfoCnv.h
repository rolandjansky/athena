/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EnergySumRoIAuxInfoCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef EnergySumRoIAuxInfoCnv_H
#define EnergySumRoIAuxInfoCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/EnergySumRoIAuxInfo.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::EnergySumRoIAuxInfo,
                               xAOD::EnergySumRoIAuxInfo >
   xAODEnergySumRoIAuxInfoCnvBase;

/**
 *  @short POOL converter for the xAOD::EnergySumRoIAuxInfo class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::EnergySumRoI
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Alan Watson <Alan.Watson@CERN.CH>
 *
 * $Revision$
 * $Date$
 */
class xAODEnergySumRoIAuxInfoCnv :
   public xAODEnergySumRoIAuxInfoCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODEnergySumRoIAuxInfoCnv >;

protected:
   /// Converter constructor
   xAODEnergySumRoIAuxInfoCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::EnergySumRoIAuxInfo*
   createPersistent( xAOD::EnergySumRoIAuxInfo* trans );
   /// Function reading in the object from the input file
   virtual xAOD::EnergySumRoIAuxInfo* createTransient();

}; // class xAODEnergySumRoIAuxInfoCnv


#endif

