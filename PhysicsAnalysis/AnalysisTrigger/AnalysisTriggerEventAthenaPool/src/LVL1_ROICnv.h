/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTATHENAPOOL_LVL1_ROICNV_H
#define ANALYSISTRIGGEREVENTATHENAPOOL_LVL1_ROICNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AnalysisTrigger include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "AnalysisTriggerEventTPCnv/LVL1_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/LVL1_ROICnv_p1.h"

// Define the latest persistent representation of LVL1_ROI:
typedef LVL1_ROI_p1 LVL1_ROI_PERS;
typedef T_AthenaPoolCustomCnv< LVL1_ROI, LVL1_ROI_PERS > LVL1_ROICnvBase;

/**
 *   @short POOL converter for LVL1_ROI
 *
 *          A custom POOL converter for LVL1_ROI that knows about the persistent version(s)
 *          of the object, and can read that as well.
 *
 * @version $Revision: 1.2 $
 *    @date $Date: 2007-07-10 17:47:30 $
 *  @author Attila Krasznahorkay Jr.
 */
class LVL1_ROICnv : public LVL1_ROICnvBase {

  friend class CnvFactory< LVL1_ROICnv >;

protected:
  LVL1_ROICnv( ISvcLocator* svcloc ) : LVL1_ROICnvBase( svcloc ) {}

  virtual LVL1_ROI_PERS* createPersistent( LVL1_ROI* transObj );
  virtual LVL1_ROI*      createTransient();

}; // class LVL1_ROICnv

#endif // ANALYSISTRIGGEREVENTATHENAPOOL_LVL1_ROICNV_H
