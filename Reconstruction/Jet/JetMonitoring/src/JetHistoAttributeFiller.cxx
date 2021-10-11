/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoAttributeFiller.h"

#include "JetMonitoring/JetMonitoringAlg.h"


namespace {

  
  //***********************************************
  // A set of helper functions to implement the details of filling
  // the jet variable in histograms.
  // The details depend if we fill 1D or 2D histograms and if some of the
  // variables are of type <vector>
  
  int processX(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX,  const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Group gr( grpT, sX);
    gr.setAutoFill(false);  // important in case there are 0 jets

    int nfill=0;
    //std::cout<< grpT->name() << "  "<< vX->name() <<  "  ffffffffff " << vX->m_index << std::endl;
    for(const xAOD::Jet* j:jets){
      //std::cout<< "  ----> " << vX->value(*j) <<  "  "<< vX->m_index << std::endl;
      sX = vX->value(*j);
      gr.fill(); nfill++;
    }
    return nfill;
  }    

  void processXVec(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX,  const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Group gr( grpT, sX);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      auto vec = vX->vector(*j);      
      size_t N= vec.size();
      for(size_t i=0;i<N;i++) {sX=vec[i]; gr.fill(); }          
    }        
  }    

  void processXY(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX, const JetVar::Variable *vY, const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Scalar<float> sY(vY->name());  
    Monitored::Group gr( grpT, sX, sY);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      sX = vX->value(*j);
      sY = vY->value(*j);
      gr.fill();
    }        
  }    

  void processXVecY(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX, const JetVar::Variable *vY, const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Scalar<float> sY(vY->name());  
    Monitored::Group gr( grpT, sX, sY);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      sY = vY->value(*j);
      auto vec = vX->vector(*j);
      size_t N= vec.size();
      for(size_t i=0;i<N;i++) {sX=vec[i]; gr.fill(); }          
    }        
  }    


  void processXYVec(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX, const JetVar::Variable *vY, const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Scalar<float> sY(vY->name());  
    Monitored::Group gr( grpT, sX, sY);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      sX = vX->value(*j);
      auto vec = vY->vector(*j);
      size_t N= vec.size();
      for(size_t i=0;i<N;i++) {sY=vec[i]; gr.fill(); }          
    }        
  }    

  void processXVecYVec(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX, const JetVar::Variable *vY, const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Scalar<float> sY(vY->name());  
    Monitored::Group gr( grpT, sX, sY);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      auto vecY = vY->vector(*j);
      auto vecX = vY->vector(*j);
      size_t Nx= vecX.size();
      size_t Ny= vecX.size();
      size_t N =  Nx<Ny  ? Nx : Ny;
      for(size_t i=0;i<N;i++) {sX=vecX[i];sY=vecY[i]; gr.fill(); }          
    }        
  }    
  

  void processXYZ(ToolHandle<GenericMonitoringTool> &grpT, const JetVar::Variable *vX, const JetVar::Variable *vY, const JetVar::Variable *vZ, const xAOD::JetContainer & jets) {
    Monitored::Scalar<float> sX(vX->name());  
    Monitored::Scalar<float> sY(vY->name());  
    Monitored::Scalar<float> sZ(vZ->name());  
    Monitored::Group gr( grpT, sX, sY, sZ);
    gr.setAutoFill(false);

    for(const xAOD::Jet* j:jets){
      sX = vX->value(*j);
      sY = vY->value(*j);
      sZ = vZ->value(*j);
      gr.fill();
    }        
  }    

  
}




JetHistoAttributeFiller::JetHistoAttributeFiller( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
  , m_varX(this)
  , m_varY(this)
  , m_varZ(this)  

{
  declareInterface<IJetHistoFiller>(this);

  declareProperty("VarX",m_varX);
  declareProperty("VarY",m_varY);
  declareProperty("VarZ",m_varZ);
  
}


StatusCode JetHistoAttributeFiller::initialize() {
  ATH_MSG_INFO("  initialize "  <<  m_varX.isEnabled());

  ATH_CHECK(m_varX.retrieve() );
  
  m_nVar = 1;
  if( ! m_varY.isEnabled() ){
    ATH_MSG_INFO( "Filling 1 var X=("<< m_varX->describe() << ")");
  }else { // has Y variable
    ATH_CHECK(m_varY.retrieve() );
    m_nVar = 2;
    if ( ! m_varZ.isEnabled()) {
      ATH_MSG_INFO( "Filling 2 vars X=("<< m_varX->describe() << ") Y=("<<m_varY->describe() << ")");
    }else{
      ATH_CHECK(m_varZ.retrieve() );
      m_nVar = 3;
      ATH_MSG_INFO( "Filling 3 vars X=("<< m_varX->describe() << ") Y=("<<m_varY->describe() << ") Z=("<<m_varZ->describe() << ")");
    }
    
  }  
  
  return StatusCode::SUCCESS;
}


StatusCode JetHistoAttributeFiller::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ) const {
  if(jets.empty()) return StatusCode::SUCCESS;
  const JetVar::Variable * vX = m_varX->variable();
  ToolHandle<GenericMonitoringTool> grpT = parentAlg.getGroup(m_group); 

  if(m_nVar==1){
    if( vX->isVector() ) processXVec(grpT, vX,  jets) ;      
    else processX(grpT, vX, jets) ;      

  }else if(m_nVar==2) { // 

    const JetVar::Variable * vY = m_varY->variable();

    int c =  int(vX->isVector() | vY->isVector()<<1) ; // encode what to do so we can use a switch()
    switch( c ){
    case 0: processXY(grpT, vX,vY, jets) ; break ;
    case 1: processXVecY(grpT, vX,vY, jets) ; break ;
    case 2: processXYVec(grpT, vX,vY, jets) ; break ;
    case 3: processXVecYVec(grpT, vX,vY, jets) ; break ;  
      
    }    
  } else { // then m_nVar == 3
    const JetVar::Variable * vY = m_varY->variable();
    const JetVar::Variable * vZ = m_varZ->variable();
    processXYZ(grpT, vX,vY, vZ, jets) ;
  }
  
  
  return StatusCode::SUCCESS;
}


