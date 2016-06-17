/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EnergySumRoICnv.h
 */

#ifndef EnergySumRoICnv_H
#define EnergySumRoICnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODTrigger/EnergySumRoI.h"

typedef T_AthenaPoolCustomCnv< xAOD::EnergySumRoI,
                               xAOD::EnergySumRoI >
   xAODEnergySumRoICnvBase;

/**
 *  @short POOL converter for the xAOD::EnergySumRoI class
 *
 *         Simple converter class making the xAOD::EnergySumRoI
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Alan Watson <Alan.Watson@CERN.CH>
 *
 */
class xAODEnergySumRoICnv : public xAODEnergySumRoICnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODEnergySumRoICnv >;

public:
   /// Converter constructor
   xAODEnergySumRoICnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::EnergySumRoI*
   createPersistent( xAOD::EnergySumRoI* trans );
   /// Function reading in the persistent object
   virtual xAOD::EnergySumRoI* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::EnergySumRoI* roi ) const;

}; // class xAODEnergySumRoICnv


#endif

