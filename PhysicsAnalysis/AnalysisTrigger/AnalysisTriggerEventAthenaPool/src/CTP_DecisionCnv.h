/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTATHENAPOOL_CTP_DECISIONCNV_H
#define ANALYSISTRIGGEREVENTATHENAPOOL_CTP_DECISIONCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AnalysisTrigger include(s):
#include "AnalysisTriggerEvent/CTP_Decision.h"

// Local include(s):
#include "AnalysisTriggerEventTPCnv/CTP_Decision_p2.h"

// Define the latest persistent representation of CTP_Decision:
typedef CTP_Decision_p2 CTP_Decision_PERS;
typedef T_AthenaPoolCustomCnv< CTP_Decision, CTP_Decision_PERS > CTP_DecisionCnvBase;

/**
 *   @short POOL converter for CTP_Decision
 *
 *          A custom POOL converter for CTP_Decision that knows about the persistent version(s)
 *          of the object, and can read that as well.
 *
 * @version $Revision: 1.3 $
 *    @date $Date: 2007-07-10 17:47:30 $
 *  @author Attila Krasznahorkay Jr.
 */
class CTP_DecisionCnv : public CTP_DecisionCnvBase {

  friend class CnvFactory< CTP_DecisionCnv >;

protected:
  CTP_DecisionCnv( ISvcLocator* svcloc ) : CTP_DecisionCnvBase( svcloc ) {}

  virtual CTP_Decision_PERS* createPersistent( CTP_Decision* transObj );
  virtual CTP_Decision*      createTransient();

}; // class CTP_DecisionCnv

#endif // ANALYSISTRIGGEREVENTATHENAPOOL_CTP_DECISIONCNV_H
