/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_M2C_332_CALCTOOL_H
#define STRANSVERSEMASSUTILS_M2C_332_CALCTOOL_H

//////////////////////////////////////////////
//C S Cowden			8 Feb 2010
//Interface tool to calculate m_2C
/////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_M2C_332_Calculator.h"

#include<string>

static const InterfaceID IID_M2C_332_CalcTool("M2C_332_CalcTool",1,0);

class M2C_332_CalcTool : public AlgTool {
public:

  M2C_332_CalcTool(const std::string& type,const std::string& name, const IInterface *parent);

  static const InterfaceID& interfaceID(){return IID_M2C_332_CalcTool; }

  ~M2C_332_CalcTool();

  /** initialize the tool */
  virtual StatusCode initialize();
  /** execute the tool */
  virtual StatusCode execute();
  /** finalize the tool */
  virtual StatusCode finalize();
  
  /** clear pointers */
  inline void clear();
  /** set momentum of event */
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::AtlMET mpt);
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b, Mt2::Atl4Vec const mpt);
  /** set Delta M (mass difference read from three body decay end-point ) */
  inline void setDeltaM(double &Dm){m_DM=Dm;}
  /** calculate m_2C */
  inline StatusCode calc( double &m2c );

private:
  Mt2::LorentzTransverseVector *m_VecA,*m_VecB;
  Mt2::TwoVector *m_mpt;

  double m_DM;

};

  
void M2C_332_CalcTool::clear(){
  if( m_VecA ) {
    delete m_VecA;
    m_VecA=NULL;
  }
  if( m_VecB ) {
    delete m_VecB;
    m_VecB=NULL;
  }
  if( m_mpt ){
    delete m_mpt;
    m_mpt=NULL;
  }
}

StatusCode M2C_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::AtlMET mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->etx(),mpt->ety());

  return StatusCode::SUCCESS;
}

StatusCode M2C_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::Atl4Vec const mpt){
  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->px(),mpt->py());

  return StatusCode::SUCCESS;
}

StatusCode M2C_332_CalcTool::calc( double &m2c ){
  if( !m_VecA || !m_VecB || !m_mpt ) return StatusCode::FAILURE;
  m2c = M2C_332Calculator(*m_VecA,*m_VecB,*m_mpt,m_DM);
  return StatusCode::SUCCESS;
}
  

#endif
