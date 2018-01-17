/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetContainerHistoFiller.h"
#include "xAODEventInfo/EventInfo.h"

JetContainerHistoFiller::JetContainerHistoFiller(const std::string& n) : HistoGroupBase(n)
                                                                       , m_histoTools(this)
                                                                       , m_jetContainerName("")
{
  declareInterface<JetContainerHistoFiller>(this);

  declareProperty("JetContainer", m_jetContainerName="");
  declareProperty("HistoTools", m_histoTools);

}

StatusCode JetContainerHistoFiller::initialize() {

  CHECK( m_histoTools.retrieve() );

  if(m_jetContainerName=="") {
    ATH_MSG_ERROR("Jet Container name not set. Please set the JetContainer property");
    return StatusCode::FAILURE;
  }

  if(m_histoDir=="") m_histoDir =  m_jetContainerName+"/";

  return StatusCode::SUCCESS;
}



int JetContainerHistoFiller::fillHistos(){

  ATH_MSG_DEBUG ("Filling hists " << name() << "..." << m_jetContainerName);

  const xAOD::EventInfo* evtInfo;
  CHECK(evtStore()->retrieve( evtInfo ), 1);

  //LAr event veto: skip events rejected by LAr
  if(evtInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error){
    ATH_MSG_DEBUG("SKIP for LAR error");
    return 1;
  }
  
  const xAOD::JetContainer* jCont = 0;
  StatusCode sc=  evtStore()->retrieve(jCont, m_jetContainerName) ;
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG (" No container  " << m_jetContainerName<< " in Evt store. Returning.");
    return 0;
  }


  /// simply call fillHistosFromContainer() for each tool...
  int count = 0;
  for( auto jtool : m_histoTools){
    ATH_MSG_DEBUG ("Filling hists " << jtool->name() << "..." << jCont);

    count += jtool->fillHistosFromContainer(*jCont);
  }

  return count;    
}

void JetContainerHistoFiller::setInterval(Interval_t ityp, bool force ){
  // propagate interval to sub-tools
  for( auto jtool : m_histoTools){
    jtool->setInterval(ityp,force);
  }
}


int JetContainerHistoFiller::buildHistos(){
  int count=0;

  ATH_MSG_DEBUG ("Building hists " );

  // ask subtools to build their histos
  for( auto jtool : m_histoTools){
    count+=jtool->buildHistos();
    ATH_MSG_DEBUG (" *** Built hist :  "<<jtool->name()  );
    
    // keep a pointer to histos :
    const auto & hdata = jtool->bookedHistograms();
    for( const auto & hd : hdata ){ 
      m_vBookedHistograms.push_back(hd);
    }
  }
  return count;    
}


  int JetContainerHistoFiller::finalizeHistos(){
  int count = 0;
  for( auto jtool : m_histoTools){
    count+=jtool->finalizeHistos();
  }
  return count;
}

void JetContainerHistoFiller::prefixHistoDir(const std::string & preDir){
  for( auto jtool : m_histoTools){
    jtool->prefixHistoDir(preDir+m_histoDir);
  }
}
