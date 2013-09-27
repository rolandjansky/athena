/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollection_tlp1
 *
 * @brief "top level" persistent partner for TrigVertexCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCollection_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_TLP1_H

#include "TrigInDetEventTPCnv/TrigVertexCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"

class TrigVertexCollection_tlp1 
{
   
 public:
   
  TrigVertexCollection_tlp1(){}
  friend class TrigVertexCollectionCnv_tlp1;

 private:

  std::vector<TrigVertexCollection_p1>       m_TrigVertexCollection ;
  std::vector<TrigVertex_p1>                 m_Vertex               ;
  std::vector<TrigInDetTrack_p1>             m_Track                ;
  std::vector<TrigInDetTrackFitPar_p1>       m_TrackFitPar          ;

};//end of class definitions
 
#endif
