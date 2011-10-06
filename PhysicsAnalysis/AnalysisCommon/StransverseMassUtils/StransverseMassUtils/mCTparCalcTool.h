/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MCTPARCALCTOOL_H
#define STRANSVERSEMASSUTILS_MCTPARCALCTOOL_H

///////////////////////////////////////////
//C S Cowden			5 Feb 2010
//interface tool to calculate mCT_parallel
///////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/MCTll_332_Calculator.h"

#include<string>

static const InterfaceID IID_mCTparCalcTool("mCTparCalcTool",1,0);

class mCTparCalcTool : public AlgTool {
public:

  mCTparCalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mCTparCalcTool; }

  ~mCTparCalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute the tool */
  virtual StatusCode execute();
  /** finalize the tool */
  virtual StatusCode finalize();

  /** clear pointers */
  inline void clear();
  /** set momentum */
  inline StatusCode setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::Atl4Vec const utm);
  /** calculate mCT_parallel */
  inline StatusCode calc(double &mCTpar );

private:

  Mt2::LorentzTransverseVector *m_VecA, *m_VecB;
  Mt2::TwoVector *m_utm;

};

void mCTparCalcTool::clear(){
  if( m_VecA ){
    delete m_VecA;
    m_VecA=NULL;
  }
  if( m_VecB ){
    delete m_VecB;
    m_VecB=NULL;
  }
  if( m_utm ){
    delete m_utm;
    m_utm=NULL;
  }
}

StatusCode mCTparCalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b, Mt2::Atl4Vec const utm){
  if( !a || !b || !utm ) return StatusCode::FAILURE;
  
  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_utm = new Mt2::TwoVector(utm->px(),utm->py());

  return StatusCode::SUCCESS;
}

StatusCode mCTparCalcTool::calc(double &mCTpar ){
  if( !m_VecA || !m_VecB || !m_utm ) return StatusCode::FAILURE;
  mCTpar = Mt2::mctll_332(*m_VecA,*m_VecB,*m_utm);
  return StatusCode::FAILURE;
}

#endif
