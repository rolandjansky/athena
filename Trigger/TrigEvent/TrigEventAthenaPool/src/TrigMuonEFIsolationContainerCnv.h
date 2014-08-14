/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMUONEFISOLATIONCONTAINERCNV_H__
#define TRIGEVENTATHENAPOOL_TRIGMUONEFISOLATIONCONTAINERCNV_H__

/**
 * @class TrigMuonEFIsolationContainerCnv
 * 
 * @author Mark Owen <markowen@cern.ch>
 * @brief Class to convert between persistent & transisent versions of TrigMuonEFIsolationContainer
 */

// transient class
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"

// typedef to the latest persistent version
typedef TrigMuonEFIsolationContainer_p2 TrigMuonEFIsolationContainer_PERS;


typedef T_AthenaPoolCustomCnv<TrigMuonEFIsolationContainer, TrigMuonEFIsolationContainer_PERS> TrigMuonEFIsolationContainerCnvBase;

class TrigMuonEFIsolationContainerCnv : public TrigMuonEFIsolationContainerCnvBase
{

  friend class CnvFactory<TrigMuonEFIsolationContainerCnv>;

 protected:

  TrigMuonEFIsolationContainerCnv(ISvcLocator* svcloc);

  TrigMuonEFIsolationContainer_PERS* createPersistent(TrigMuonEFIsolationContainer* transCont);

  TrigMuonEFIsolationContainer*      createTransient();



};//class TrigMuonEFIsolationContainerCnv

#endif //TRIGEVENTATHENAPOOL_TRIGMUONEFISOLATIONCONTAINERCNV_H__
