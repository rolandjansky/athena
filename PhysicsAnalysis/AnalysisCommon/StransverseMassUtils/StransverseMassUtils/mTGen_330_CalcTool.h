/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MTGEN_330_CALCTOOL_H
#define STRANSVERSEMASSUTILS_MTGEN_330_CALCTOOL_H

////////////////////////////////////////////////
//C S Cowden			Feb 2010
//Tool to calculate mTGen 330 using an analytic solution
////////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "EventKernel/I4Momentum.h"
#include "MissingETEvent/MissingET.h"

#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_MtGen_330_Calculator.h"
#include "Mt2/Analytic_Mt2_330_Calculator.h"
#include "Mt2/MtGen_330_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

static const InterfaceID IID_mTGen_330_CalcTool("mTGen_330_CalcTool",1,0);

class mTGen_330_CalcTool : public AlgTool {

public:
  
  mTGen_330_CalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mTGen_330_CalcTool; };

  ~mTGen_330_CalcTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  inline void clear();
  inline StatusCode push_back(Mt2::Atl4Vec const a);
  inline StatusCode calc(double &mt2);
  inline void setChi(double Chi){m_mChi=Chi;}

  inline StatusCode setType(Mt2::Mt2_330_CalculatorType T);

private:

  std::vector<Mt2::LorentzVector> m_theEvent;

  Mt2::Mt2_330_Calculator * m_mTGen_Calc;
  Mt2::Basic_MtGen_330_Calculator * m_mTGen;

  double m_mChi;

};

void mTGen_330_CalcTool::clear(){
  m_theEvent.clear();
}


StatusCode mTGen_330_CalcTool::push_back(Mt2::Atl4Vec const a){
  if( !a ) return StatusCode::FAILURE;

  Mt2::LorentzVector::InitEPxPyPz tmpVec(a->e(),a->px(),a->py(),a->pz());
  m_theEvent.push_back(Mt2::LorentzVector(tmpVec));

  return StatusCode::SUCCESS;
}


StatusCode mTGen_330_CalcTool::calc(double &mt2){
  mt2=m_mTGen->mtGen_330(m_theEvent,m_mChi);
  return StatusCode::SUCCESS;
}

StatusCode  mTGen_330_CalcTool::setType(Mt2::Mt2_330_CalculatorType T){
  if(m_mTGen_Calc){
    delete m_mTGen_Calc;
    m_mTGen_Calc=NULL;
  }
  if(m_mTGen){
    delete m_mTGen;
    m_mTGen=NULL;
  }

  if( T == Mt2::Analytic330 ){
    m_mTGen_Calc = new Mt2::Analytic_Mt2_330_Calculator();
    m_mTGen = new Mt2::Basic_MtGen_330_Calculator(*m_mTGen_Calc);
    return StatusCode::SUCCESS;
  } 
  else return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;
}



#endif
