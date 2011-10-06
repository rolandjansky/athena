/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_ASYMMETRIC_MT2_332_CALCTOOL_H
#define STRANSVERSEMASSUTILS_ASYMMETRIC_MT2_332_CALCTOOL_H

//////////////////////////////////////////////
//C S Cowden			5 Feb 2010
//Tool to calculate Assymetric mT2 
//////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_Mt2_Asymmetric332_Calculator.h"

#include<string>

static const InterfaceID IID_Asymmetric_mT2_332_CalcTool("Asymmetric_mT2_332_CalcTool",1,0);

class Asymmetric_mT2_332_CalcTool : public AlgTool {
public:

  Asymmetric_mT2_332_CalcTool(const std::string& type, const std::string& name, const IInterface *parent);

  static const InterfaceID& interfaceID() { return IID_Asymmetric_mT2_332_CalcTool; }

  ~Asymmetric_mT2_332_CalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute the tool */
  virtual StatusCode execute();
  /** finalize the tool */
  virtual StatusCode finalize();

  /** clear pointers */
  inline void clear();
  /** set Chi */
  inline void setChi( double Chi ){ m_Chi = Chi; }
  /** set momentum for the event */
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b,Mt2::AtlMET mpt); 
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b,Mt2::Atl4Vec const mpt);
  /** calculate Assymetric mT2 */
  inline StatusCode calc( double &mT2 );
  /** set ratio of masses */
  inline void setTheta(double theta){m_theta=theta;}

private:

  Mt2::Basic_Mt2_Asymmetric332_Calculator * m_calc;

  Mt2::LorentzTransverseVector *m_VecA, *m_VecB;
  Mt2::TwoVector *m_mpt;

  double m_Chi,m_theta;

};

void Asymmetric_mT2_332_CalcTool::clear(){
  if( m_VecA ){
    delete m_VecA;
    m_VecA=NULL;
  }
  if( m_VecB ){
    delete m_VecB;
    m_VecB=NULL;
  }
  if( m_mpt ){
    delete m_mpt;
    m_mpt=NULL;
  }
}


StatusCode Asymmetric_mT2_332_CalcTool::setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b,Mt2::AtlMET mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->etx(),mpt->ety());

  return StatusCode::SUCCESS;
}

StatusCode Asymmetric_mT2_332_CalcTool::setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::Atl4Vec const mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->px(),mpt->py());

  return StatusCode::SUCCESS;
}


StatusCode Asymmetric_mT2_332_CalcTool::calc(double &mT2){
  if( !m_VecA || !m_VecB || !m_mpt ) return StatusCode::FAILURE;
  mT2 = m_calc->mt2_Asymmetric332(m_theta,*m_VecA,*m_VecB,*m_mpt,m_Chi);
  return StatusCode::SUCCESS;
}


#endif
