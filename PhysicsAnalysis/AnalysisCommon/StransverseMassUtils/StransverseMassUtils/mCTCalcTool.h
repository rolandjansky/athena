/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MCTCALCTOOL_H
#define STRANSVERSEMASSUTILS_MCTCALCTOOL_H
/////////////////////////////////////////////////////
//C S Cowden			4 February 2010
//Tool to calculate mCT.
/////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"

#include "StransverseMassUtils/mT2Defs.h"


#include "Mt2/MCT_330_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

static const InterfaceID IID_mCTCalcTool("mCTCalcTool",1,0);

class mCTCalcTool : public AlgTool {

public:

  mCTCalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mCTCalcTool; };

  ~mCTCalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute funtion (inherited from AlgTool) */
  virtual StatusCode execute();
  /** finalize tool ( delete pointers and stuff ) */
  virtual StatusCode finalize();

  /** perform calculation */
  inline StatusCode calc( double &mct );
  /** setMomentum for event. Return FAILURE if either Mt2::Atl4Vec are NULL */
  inline StatusCode setMom( Mt2::Atl4Vec const a, Mt2::Atl4Vec const b);

private:

  Mt2::LorentzTransverseVector *m_VecA,*m_VecB;

};


StatusCode mCTCalcTool::calc( double &mCT ) {
  if( !m_VecA || !m_VecB ) return StatusCode::FAILURE;
  mCT = Mt2::mct_330(*m_VecA,*m_VecB);
  return StatusCode::SUCCESS;
}


StatusCode mCTCalcTool::setMom( Mt2::Atl4Vec const a, Mt2::Atl4Vec const b){
  if( !a || !b ) return StatusCode::FAILURE;

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());

  return StatusCode::SUCCESS;
}


#endif
