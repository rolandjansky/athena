/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MTGEN332CALCTOOL_H
#define STRANSVERSEMASSUTILS_MTGEN332CALCTOOL_H

//////////////////////////////////////////////////////////
//C S Cowden				17 August 2009
//tool to calculate mTGen; wrapper to external mT2 library
//This class is similar to the mT2CalcTool also in this 
//package.  It inherits from the AlgTool bas class and can 
//also be added to an Athena Algorithm by using the tool 
//handle method.  It can also be expanded to include new
//calculator methods.
//////////////////////////////////////////////////////////

#include "GaudiKernel/AlgTool.h"

#include "EventKernel/INavigable4Momentum.h"
#include "MissingETEvent/MissingET.h"

#include "StransverseMassUtils/mT2Defs.h"

#include "Mt2/Basic_MtGen_332_Calculator.h"
#include "Mt2/TemporaryZhenyuHanWrapper_Mt2_332_Calculator.h"
#include "Mt2/Basic_Mt2_332_Calculator.h"
#include "Mt2/Mt2_332_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<vector>
#include<string>


static const InterfaceID IID_mTGen_332_CalcTool("mTGen_332_CalcTool",1,0);

class mTGen_332_CalcTool : public AlgTool {

public:
  mTGen_332_CalcTool(const std::string& type, const std::string& name,
	const IInterface* parent);

  static const InterfaceID& interfaceID() { return IID_mTGen_332_CalcTool; };


  ~mTGen_332_CalcTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  inline void clear();
  /** add a four vector to the collection to be used in the calculation */
  inline StatusCode push_back(Mt2::Atl4Vec const a);
  /** add and MPT two vector */
  inline StatusCode push_mpt_back(Mt2::AtlMET MET);
  inline StatusCode push_mpt_back(Mt2::Atl4Vec const a);
  /** calculate mTGen */
  inline StatusCode calc(double &mt2);
  /** set the Chi guess mass */
  inline void setChi(double Chi){m_mChi=Chi;}

  /** set the type of calculator for mTGen to use */
  inline StatusCode setType(Mt2::Mt2_332_CalculatorType T);

private:

  std::vector<Mt2::LorentzTransverseVector> m_theEvent;
  Mt2::TwoVector *m_mpt;

  std::string m_mTGen_CalcType;


  Mt2::Mt2_332_Calculator * m_mTGen_Calc;
  Mt2::Basic_MtGen_332_Calculator * m_mTGen;

  double m_mChi;


};


void mTGen_332_CalcTool::clear(){
  m_theEvent.clear();
  if(m_mpt){
    delete m_mpt;
    m_mpt=0;
  }
}

StatusCode mTGen_332_CalcTool::push_back(Mt2::Atl4Vec const a){
  if( !a )return StatusCode::FAILURE;

  m_theEvent.push_back(Mt2::LorentzTransverseVector(a->et(),a->px(),a->py()));

  return StatusCode::SUCCESS;
}

StatusCode mTGen_332_CalcTool::push_mpt_back(Mt2::AtlMET MET){
  if( !MET )return StatusCode::FAILURE;

  if(m_mpt){
    delete m_mpt;
    m_mpt=NULL;
  }

  m_mpt = new Mt2::TwoVector(MET->etx(),MET->ety());

  return StatusCode::SUCCESS;
}



StatusCode mTGen_332_CalcTool::push_mpt_back(Mt2::Atl4Vec const a){
  if( !a ) return StatusCode::FAILURE;

  if(m_mpt){
    delete m_mpt;
    m_mpt = NULL;
  }

  m_mpt = new Mt2::TwoVector(a->px(),a->py());

  return StatusCode::SUCCESS;
}




StatusCode mTGen_332_CalcTool::calc(double &mt2){
  if( !m_mpt ) return StatusCode::FAILURE;
  mt2=m_mTGen->mtGen_332(m_theEvent,*m_mpt,m_mChi);
  return StatusCode::SUCCESS;
}



StatusCode mTGen_332_CalcTool::setType(Mt2::Mt2_332_CalculatorType T){
  if(m_mTGen_Calc){
    delete m_mTGen_Calc;
    m_mTGen_Calc=0;
  }
  if(m_mTGen){
    delete m_mTGen;
    m_mTGen=0;
  }


  if( T == Mt2::Basic332 ){
    m_mTGen_Calc = new Mt2::Basic_Mt2_332_Calculator();
    m_mTGen = new Mt2::Basic_MtGen_332_Calculator(*m_mTGen_Calc);
    return StatusCode::SUCCESS;
  } else if ( T == Mt2::Bisect ){
    m_mTGen_Calc = new Mt2::TemporaryZhenyuHanWrapper_Mt2_332_Calculator();
    m_mTGen = new Mt2::Basic_MtGen_332_Calculator(*m_mTGen_Calc);
    return StatusCode::SUCCESS;
  } 
  else return StatusCode::FAILURE;
  
 return StatusCode::FAILURE;
}


#endif
