/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverObj.h
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : February 2014
//
// DESCRIPTION
// 
// Tile Muon Receiver object
//
// Properties (JobOption Parameters):
//
// BUGS:
//  
// History:
//    
//****************************************************************************

#ifndef TILEEVENT_TILEMUONRECEIVEROBJ_H
#define TILEEVENT_TILEMUONRECEIVEROBJ_H

#include <vector>
#include <string> 

class TileMuonReceiverObj {

 public:
  
  // constructor(s)
  // 	
  // thresholds: [dim 4] d6L d6H d56L d56H		
  // decision  : [dim 4] 4-bit word
  // ene       : [dim 2] d6 d5+d6
  // time      : [dim 2] (t1+t2)/2 (t1+t2+t3+t4)/4 : 1..4 are the PMTs 1,2 > D6 and 3,4 > D5
  //

  TileMuonReceiverObj(): m_id(0), m_TileMuRcvDecision(), m_TileMuRcvThresholds(), m_TileMuRcvEne(), m_TileMuRcvTime() {}

  TileMuonReceiverObj( int id ) { m_id=id; }

  TileMuonReceiverObj( int id, std::vector<bool> &decision)
   { m_id=id; m_TileMuRcvDecision=decision; }

  TileMuonReceiverObj( int id, std::vector<float> &thresholds)
   { m_id=id; m_TileMuRcvThresholds=thresholds; }

  TileMuonReceiverObj( int id, std::vector<float> &ene, std::vector<float> &time)
   { m_id=id; m_TileMuRcvEne=ene; m_TileMuRcvTime=time; }

  TileMuonReceiverObj( int id, std::vector<float> &ene, std::vector<float> &time, std::vector<bool> &decision)
   { m_id=id; m_TileMuRcvEne=ene; m_TileMuRcvTime=time; m_TileMuRcvDecision=decision; }

  // destructor
  //
  virtual ~TileMuonReceiverObj(){}
  
  // set methods
  //
  void Set(int id, std::vector<bool> &decision)
   { m_id=id; m_TileMuRcvDecision=decision; }

  void Set(int id, std::vector<float> &thresholds)
   { m_id=id; m_TileMuRcvThresholds=thresholds; }

  void Set(int id, std::vector<float> &ene, std::vector<float> &time)
   { m_id=id; m_TileMuRcvEne=ene; m_TileMuRcvTime=time; }

  void Set(int id, std::vector<float> &ene, std::vector<float> &time, std::vector<bool> &decision)
   { m_id=id; m_TileMuRcvEne=ene; m_TileMuRcvTime=time; m_TileMuRcvDecision=decision; }

  // access methods
  //
  int GetID() const { return m_id; }

  inline int identify(void) const { return m_id; }

  const std::vector<bool>&  GetDecision() const { return m_TileMuRcvDecision; } // [dim 4] 
  const std::vector<float>& GetThresholds() const { return m_TileMuRcvThresholds; } // [dim 4] ... it is the same for every event
  const std::vector<float>& GetEne() const { return m_TileMuRcvEne; } // [dim 2] d6 and d5+d6 
  const std::vector<float>& GetTime() const { return m_TileMuRcvTime; } // [dim 2] (t1+t2)/2. and (t1+t2+t3+t4)/4. : 1..4 are the PMTs with 1,2 > D6 and 3,4 > D5

  // print methods
  //
  std::string whoami (void) const { return "TileMuonReceiverObj"; }

  void print(void) const;

  void clear(void);

  operator std::string() const;
 
 private:

  int m_id;
  std::vector<bool>  m_TileMuRcvDecision;
  std::vector<float> m_TileMuRcvThresholds;
  std::vector<float> m_TileMuRcvEne;
  std::vector<float> m_TileMuRcvTime;

};

#endif

