/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MT2_330_CALCTOOL_H
#define STRANSVERSEMASSUTILS_MT2_330_CALCTOOL_H

//////////////////////////////////////////////
//C S Cowden			7 Feb 2010
//Tool to calculate mT2 330 analytically
/////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "EventKernel/I4Momentum.h"
#include "MissingETEvent/MissingET.h"

#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Analytic_Mt2_330_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

static const InterfaceID IID_mT2_330_CalcTool("mT2_330_CalcTool",1,0);

class mT2_330_CalcTool : public AlgTool {

public:

  mT2_330_CalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mT2_330_CalcTool; };

  ~mT2_330_CalcTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  inline void clear();
  inline void setChi( double Chi){m_Chi=Chi; }
  inline StatusCode setType( Mt2::Mt2_330_CalculatorType T );
  inline StatusCode setMom( Mt2::Atl4Vec const a, Mt2::Atl4Vec const b);
  inline StatusCode calc(double &mt2);

  private:

  Mt2::Mt2_330_Calculator * m_calc;

  Mt2::LorentzTransverseVector *m_VecA, *m_VecB;  

  double m_Chi;
  

};

StatusCode mT2_330_CalcTool::calc(double &mt2){
  if( !m_VecA || !m_VecB ) return StatusCode::FAILURE;
  mt2 = m_calc->mt2_330(*m_VecA,*m_VecB,m_Chi);
  return StatusCode::SUCCESS;
}

void mT2_330_CalcTool::clear(){
  if(m_VecA){
    delete m_VecA;
    m_VecA=NULL;
  }
  if(m_VecB){
    delete m_VecB;
    m_VecB=NULL;
  }

}

StatusCode mT2_330_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b){
  if( !a || !b ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());

  return StatusCode::SUCCESS;
}


StatusCode mT2_330_CalcTool::setType( Mt2::Mt2_330_CalculatorType T){
  if( m_calc ){
    delete m_calc;
    m_calc=NULL;
  }

  if( T == Mt2::Analytic330 ){
    m_calc = new Mt2::Analytic_Mt2_330_Calculator();
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


#endif
