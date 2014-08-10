/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGL2PILEUP_TRIGPILEUPINFO_H 
#define TRIGL2PILEUP_TRIGPILEUPINFO_H 

#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>


class TrigPileUpInfo{

 public:

  TrigPileUpInfo();

  void clear();
 
  int numFJ(){ return (int) m_FJ.size(); }
  int numBJ(){ return (int) m_BJ.size(); }
  int numPV(){ return (int) m_PV_Track.size(); }

  void addFJ(int);
  void addBJ(int);
  void addPV(int);

  int getFJ(unsigned int index) const;
  int getBJ(unsigned int index) const;
  int getPV(unsigned int index) const;

  std::vector<int> getFJVec() const;
  std::vector<int> getBJVec() const;
  std::vector<int> getPVVec() const;

 private:

  std::vector<int> m_FJ; // Forward Jet thresholds
  std::vector<int> m_BJ; // Backward Jet thresholds
  std::vector<int> m_PV_Track; // Number of Primarty Vertices

};


CLASS_DEF( TrigPileUpInfo , 95781919 , 1 )
CLASS_DEF( DataVector<TrigPileUpInfo> , 182719150 , 1 )


class TrigPileUpInfoContainer : public DataVector<TrigPileUpInfo> { };

CLASS_DEF( TrigPileUpInfoContainer , 1255908109 , 1 )

SG_BASE(TrigPileUpInfoContainer, DataVector<TrigPileUpInfo>);


#endif
