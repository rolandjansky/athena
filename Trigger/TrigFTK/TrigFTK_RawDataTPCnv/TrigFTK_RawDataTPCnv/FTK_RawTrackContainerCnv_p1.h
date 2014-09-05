/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWTRACKTPCNV_FTK_RAWTRACKCONTAINERCNV_P1_H
#define TRIGFTK_RAWTRACKTPCNV_FTK_RAWTRACKCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigFTK_RawDataTPCnv/FTK_RawTrackCnv_p1.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_p1.h"

class MsgStream;

class FTK_RawTrackContainerCnv_p1 
  : public T_AthenaPoolTPCnvBase<FTK_RawTrackContainer, FTK_RawTrackContainer_p1>
{

 public:
  
  FTK_RawTrackContainerCnv_p1() {}

  virtual void persToTrans( const FTK_RawTrackContainer_p1*, FTK_RawTrackContainer*, MsgStream& );
  virtual void transToPers( const FTK_RawTrackContainer*,    FTK_RawTrackContainer_p1*, MsgStream& );
  
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
    m_FTK_RawTrackVectorCnv.setTopConverter( topCnv, 0 );
  }

 private:

 typedef T_AthenaPoolTPPtrVectorCnv< FTK_RawTrackContainer,
                                     std::vector<TPObjRef>,
                                     ITPConverterFor<FTK_RawTrack>  >   FTK_RawTrackVectorCnv_p1;

  FTK_RawTrackVectorCnv_p1 m_FTK_RawTrackVectorCnv;

};


#endif
