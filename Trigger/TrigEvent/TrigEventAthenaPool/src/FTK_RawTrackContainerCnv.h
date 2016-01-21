/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigInDetTrackCollectionCnv
 *
 * @brief transient persistent converter for TrigInDetTrackCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: FTK_RawTrackContainerCnv.h 716421 2016-01-05 14:33:14Z schaffer $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_FTKRAWTRACKCONTAINERCNV_H
#define TRIGEVENTATHENAPOOL_FTKRAWTRACKCONTAINERCNV_H

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class   FTK_RawTrackContainer_tlp1;
class   FTK_RawTrackContainerCnv_tlp1;
typedef FTK_RawTrackContainer_tlp1  FTK_RawTrackContainer_PERS;
typedef FTK_RawTrackContainerCnv_tlp1  FTK_RawTrackContainerCnv_PERS;

typedef T_AthenaPoolCustomCnv<FTK_RawTrackContainer, FTK_RawTrackContainer_PERS > FTK_RawTrackContainerCnvBase; 

class MsgStream;

class FTK_RawTrackContainerCnv : public FTK_RawTrackContainerCnvBase{

  friend class CnvFactory<FTK_RawTrackContainerCnv>;

 protected:

  FTK_RawTrackContainerCnv(ISvcLocator* svcloc);
  ~FTK_RawTrackContainerCnv();

  virtual FTK_RawTrackContainer_PERS*  createPersistent(FTK_RawTrackContainer* transObj);
  virtual FTK_RawTrackContainer*       createTransient ();

 private:

  FTK_RawTrackContainerCnv_PERS* m_tlp_Converter;

};


#endif
