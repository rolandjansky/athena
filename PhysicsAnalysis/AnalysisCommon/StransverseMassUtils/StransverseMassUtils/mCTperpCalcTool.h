/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MCTPERPCALCTOOL_H
#define STRANSVERSEMASSUTILS_MCTPERPCALCTOOL_H
////////////////////////////////////////////////////
//C S Cowden			4 Feb 2010
//An interface tool to the mCT_perp claculators.
////////////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "EventKernel/I4Momentum.h"

#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/MCTT_332_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

static const InterfaceID IID_mCTperpCalcTool("mCTperpCalcTool",1,0);

class mCTperpCalcTool : public AlgTool {

public:

  mCTperpCalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mCTperpCalcTool; }

  ~mCTperpCalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute the tool  ( inherited from AlgTool ) */
  virtual StatusCode execute();
  /** finalize the tool */
  virtual StatusCode finalize();

  /** set momentem for calculation, returns FAILURE if either vector is NULL pointer */
  inline StatusCode setMom( Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::Atl4Vec const utm);

  /** clear the event */
  inline void clear();

  /** calculate the observable */
  inline StatusCode calc( double & mct );

private:

  Mt2::LorentzTransverseVector *m_VecA, *m_VecB;
  Mt2::TwoVector *m_utm;

};


StatusCode mCTperpCalcTool::setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::Atl4Vec const utm){
  if( !a || !b || !utm ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_utm = new Mt2::TwoVector(utm->px(),utm->py());

  return StatusCode::SUCCESS;
}

void mCTperpCalcTool::clear(){
  if(m_VecA){
    delete m_VecA;
    m_VecA = NULL;
  }
  if(m_VecB){
    delete m_VecB;
    m_VecB = NULL;
  }
  if(m_utm){
    delete m_utm;
    m_utm=NULL;
  }
}

StatusCode mCTperpCalcTool::calc(double &mct){
  if( !m_VecA || !m_VecB || !m_utm ) return StatusCode::FAILURE;
  mct = Mt2::mctt_332(*m_VecA,*m_VecB,*m_utm);
  return StatusCode::SUCCESS;
} 


#endif
