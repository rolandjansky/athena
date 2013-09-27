/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollection_tlp2
 *
 * @brief "top level" persistent partner for TrigVertexCollection
 *
 * @author Dmitry Emeliyanov
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_TLP2_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_TLP2_H

#include "TrigInDetEventTPCnv/TrigVertexCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertex_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p3.h"

class TrigVertexCollection_tlp2 
{
   
 public:
   
  TrigVertexCollection_tlp2(){}
  friend class TrigVertexCollectionCnv_tlp2;

 private:

  std::vector<TrigVertexCollection_p1>       m_TrigVertexCollection ;
  std::vector<TrigVertex_p2>                 m_Vertex               ;
  std::vector<TrigInDetTrack_p3>             m_Track                ;
  std::vector<TrigInDetTrackFitPar_p3>       m_TrackFitPar          ;

};//end of class definitions
 
#endif
