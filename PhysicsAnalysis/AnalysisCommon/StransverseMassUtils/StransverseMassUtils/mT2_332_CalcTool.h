/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MT2_332_CALCTOOL_H
#define STRANSVERSEMASSUTILS_MT2_332_CALCTOOL_H

/////////////////////////////////////////////////////////
//C S Cowden                        17 August 2009
//Tool to calculate mT2; act as a wrapper to external
//mT2 library.
//This class inherits from the AlgTool base class and can
//be added to an Athena Algorithm in a standard way using
//the tool handles.  It should be fairly easy to expand
//this class as new mT2 calculators are introduced by users
//and developers.
/////////////////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"
#include "EventKernel/I4Momentum.h"
#include "MissingETEvent/MissingET.h"

#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_Mt2_332_Calculator.h"
#include "Mt2/TemporaryZhenyuHanWrapper_Mt2_332_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

static const InterfaceID IID_mT2_332_CalcTool("mT2_332_CalcTool",1,0);

class mT2_332_CalcTool : public AlgTool {

public:

  mT2_332_CalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mT2_332_CalcTool; };

  ~mT2_332_CalcTool();

  virtual StatusCode initialize();
  /** initialize the tool with an event in mind and Chi mass */
  StatusCode initialize(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::AtlMET mpt,double Chi); 
  virtual StatusCode execute();
  virtual StatusCode finalize();

  /** calculate mt2 */
  inline StatusCode calc(double &mt2);


  /** set the Chi guess mass */
  inline void setChi(double Chi){ m_Chi=Chi; }
  /** set event momenta */
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b,Mt2::AtlMET mpt);
  inline StatusCode setMom(Mt2::Atl4Vec const a, Mt2::Atl4Vec const b,Mt2::Atl4Vec mpt);
  /** unset event momenta */
  inline void clear();

  /** set the type of mt2 calculator 
   type are defined in mT2Defs.h*/
  inline StatusCode setType(Mt2::Mt2_332_CalculatorType T);

private:

  Mt2::Basic_Mt2_332_Calculator *m_Basic_mt2;
  Mt2::TemporaryZhenyuHanWrapper_Mt2_332_Calculator *m_Bisect_mt2;

  Mt2::Mt2_332_Calculator *m_calc;

  Mt2::LorentzTransverseVector *m_VecA,*m_VecB;
  Mt2::TwoVector *m_mpt;

  double m_Chi;


};


StatusCode mT2_332_CalcTool::calc(double& mt2){
  if( !m_VecA || !m_VecB || !m_mpt ) return StatusCode::FAILURE;
  mt2 = m_calc->mt2_332(*m_VecA,*m_VecB,*m_mpt,m_Chi);
  return StatusCode::SUCCESS;
}

void mT2_332_CalcTool::clear(){
  if(m_VecA){
    delete m_VecA;
    m_VecA=NULL;
  }
  if(m_VecB){
    delete m_VecB;
    m_VecB=NULL;
  }
  if(m_mpt){
    delete m_mpt;
    m_mpt=NULL;
  }
}

StatusCode mT2_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::AtlMET mpt){

  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();

  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->etx(),mpt->ety());

  return StatusCode::SUCCESS;
}

StatusCode mT2_332_CalcTool::setMom(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::Atl4Vec const mpt){

  if( !a || !b || !mpt ) return StatusCode::FAILURE;

  this->clear();
 
  m_VecA = new Mt2::LorentzTransverseVector(a->et(),a->px(),a->py());
  m_VecB = new Mt2::LorentzTransverseVector(b->et(),b->px(),b->py());
  m_mpt = new Mt2::TwoVector(mpt->px(),mpt->py());

  return StatusCode::SUCCESS;
}

StatusCode mT2_332_CalcTool::setType(Mt2::Mt2_332_CalculatorType T){

  if( m_calc ){
    delete m_calc;
    m_calc=NULL;
  }

  if( T == Mt2::Basic332 ){
    m_calc = new Mt2::Basic_Mt2_332_Calculator();
    //m_calc=m_Basic_mt2;
    return StatusCode::SUCCESS;
  }
  else if ( T == Mt2::Bisect ){ 
    m_calc = new Mt2::TemporaryZhenyuHanWrapper_Mt2_332_Calculator();
    //m_calc=m_Bisect_mt2;
    return StatusCode::SUCCESS;
  }
  else return StatusCode::FAILURE;
}


#endif


