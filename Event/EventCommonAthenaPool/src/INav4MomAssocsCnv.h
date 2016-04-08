///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv.h 
// Header file for class INav4MomAssocsCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONATHENAPOOL_INAV4MOMASSOCSCNV_H 
#define EVENTCOMMONATHENAPOOL_INAV4MOMASSOCSCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocs_p3.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p1.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p2.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p3.h"

// Forward declaration
class StoreGateSvc;

// the latest persistent representation type of INav4MomAssocs
typedef INav4MomAssocs_p3  INav4MomAssocs_PERS;

class INav4MomAssocsCnv: public T_AthenaPoolCustomCnv<
                                          INav4MomAssocs, 
                                          INav4MomAssocs_PERS 
                                        > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<INav4MomAssocsCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  INav4MomAssocsCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual INav4MomAssocs_PERS*
    createPersistent( INav4MomAssocs* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual INav4MomAssocs* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Pointer to the StoreGateSvc
   */
  StoreGateSvc* m_storeGate;

  INav4MomAssocsCnv_p3 m_tpConverter_p3;
  INav4MomAssocsCnv_p2 m_tpConverter_p2;
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> EVENTCOMMONATHENAPOOL_INAV4MOMASSOCSCNV_H
