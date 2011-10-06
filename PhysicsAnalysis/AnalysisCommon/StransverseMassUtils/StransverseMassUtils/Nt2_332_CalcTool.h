/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_NT2_332_CALCTOOL_H
#define STRANSVERSEMASSUTILS_NT2_332_CALCTOOL_H

///////////////////////////////////////////////
//C S Cowden			8 Feb 2010
//Interface AlgTool to compute Nt2
///////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_Nt2_332_Calculator.h"

#include<string>

static const InterfaceID IID_Nt2_332_CalcTool("Nt2_332_CalcTool",1,0);

class Nt2_332_CalcTool : public AlgTool {
public:
  
  Nt2_332_CalcTool(const std::string& type, const std::string& name, const IInterface *parent);

  static const InterfaceID& interfaceID() { return IID_Nt2_332_CalcTool; }

  ~Nt2_332_CalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute the tool */
  virtual StatusCode execute();
  /** finalize the tool */
  virtual StatusCode finalize();

  /** clear pointers */
  inline void clear();
  /** set momentum for the event */
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::AtlMET mpt);
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::Atl4Vec const mpt);
  /** calculate Nt2 */
  inline StatusCode calc( double &nT2 );
  /** set parent1 mass */
  inline void setP1Mass( double &m1 ){ m_P1 = m1; }
  /** set parent2 mass */
  inline void setP2Mass( double &m2 ){ m_P2 = m2; }
  /** set masses */
  inline void setMass(double &m1, double &m2 ){ m_P1=m1; m_P2=m2; }

private:
  Mt2::Basic_Nt2_332_Calculator *m_calc;

  Mt2::LorentzTransverseVector *m_VecA,*m_VecB;
  Mt2::TwoVector *m_mpt;

  double m_P1,m_P2;

};





void Nt2_332_CalcTool::clear(){
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




StatusCode Nt2_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::AtlMET mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->etx(),mpt->ety());

  return StatusCode::SUCCESS;
}  

StatusCode Nt2_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::Atl4Vec const mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->px(),mpt->py());

  return StatusCode::SUCCESS;
}


StatusCode Nt2_332_CalcTool::calc( double &nT2 ){
  if( !m_VecA || !m_VecB || !m_mpt) return StatusCode::FAILURE;
  nT2 = m_calc->nt2_332(*m_VecA,*m_VecB,*m_mpt,m_P1,m_P2);
  return StatusCode::SUCCESS;
}

#endif
