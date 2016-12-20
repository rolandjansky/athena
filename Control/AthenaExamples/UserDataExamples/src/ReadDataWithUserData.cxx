/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadDataWithUserData.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "AthContainers/UserDataStore.h"
#include "AthenaKernel/errorcheck.h"

using namespace UserDataExamples;

ReadDataWithUserData::ReadDataWithUserData( const std::string& name, 
					    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_userDataSvc("UserDataSvc",name)
{
  declareProperty("UserDataSvc",m_userDataSvc);
  declareProperty("ContainerName",m_contName="");
  m_events=0;
  m_nAttributes=0;
}

// Destructor
///////////////
ReadDataWithUserData::~ReadDataWithUserData()
{}


StatusCode ReadDataWithUserData::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  StatusCode sc=m_userDataSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve UserDataSvc " << m_userDataSvc << endmsg;
    return sc;
  }



  return StatusCode::SUCCESS;
}

StatusCode ReadDataWithUserData::finalize() {
  //msg(MSG::INFO) << "Counters: Events: " << m_events << ", Clusters: " << nCluster << ", Attributes: " << nAttibutes << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode ReadDataWithUserData::execute() { 
  
  ++m_events;
  const CaloClusterContainer* clusterContainer=NULL;

  StatusCode sc=evtStore()->retrieve(clusterContainer,m_contName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve CaloClusterContainer with name " << m_contName << endmsg;
    return sc;
  }
  

  std::string label;
  int counter=0;
  size_t vidx=0;
  unsigned nCluster=0;
  CaloClusterContainer::const_iterator cit=clusterContainer->begin();
  CaloClusterContainer::const_iterator cit_e=clusterContainer->end();
  for(;cit!=cit_e;++cit) {
    const CaloCluster* cluster=*cit;
    ++nCluster;
    for (unsigned i=0;i<2;++i) {
      std::stringstream sLabel;
      sLabel << "Counter_" << i;
      label=sLabel.str();
      int counterOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,counterOut));
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' value=" << counterOut << endmsg;          

      //Same with double;
      label=sLabel.str()+"d";
      double dblOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,dblOut));
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' value=" << dblOut << endmsg;          

      //Same with unsigned:
      label=sLabel.str()+"u";
      uint32_t cnt_uOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,cnt_uOut));
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' value=" << cnt_uOut << endmsg;

      //Same with float:
      label=sLabel.str()+"f";
      float cnt_fOut=0;
      CHECK(m_userDataSvc->getElementDecoration(*cluster,label,cnt_fOut));
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' value=" << cnt_fOut << endmsg;


      label=sLabel.str()+"b";
      bool bout=false;
      CHECK(m_userDataSvc->getElementDecoration(*cluster,label,bout));
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' value=" << bout << endmsg;


      label=sLabel.str()+"vVirt";
      void* vecOutvoid=0;
      if (m_userDataSvc->vgetElementDecoration(*cluster,label,typeid(std::vector<int32_t>),vecOutvoid)) {
	msg(MSG::ERROR) << "vgetElementDecoration failed" << endmsg;
	return StatusCode::FAILURE;
      }
      std::vector<int32_t>* vptrOut=static_cast<std::vector<int32_t>* >(vecOutvoid); 
      vidx=m_events%(vptrOut->size()-1);
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' size=" << vptrOut->size() << "value[" << vidx << "]=" << vptrOut->at(vidx) << endmsg;
            

      label=sLabel.str()+"v";
      std::vector<int32_t> vecOut1;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecOut1));
      vidx=m_events%(vecOut1.size()-1);
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' size=" << vecOut1.size() << "value[" << vidx << "]=" << vecOut1[vidx] << endmsg;


      label=sLabel.str()+"vu";
      std::vector<uint32_t> vecuOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecuOut));
      vidx=m_events%(vecuOut.size()-1);
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' size=" << vecuOut.size() << "value[" << vidx << "]=" << vecuOut[vidx] << endmsg;


      label=sLabel.str()+"vf";
      std::vector<float> vecfOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecfOut));
      vidx=m_events%(vecfOut.size()-1);
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' size=" << vecfOut.size() << "value[" << vidx << "]=" << vecfOut[vidx] << endmsg;


      label=sLabel.str()+"vd";
      std::vector<double> vecdOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecdOut));
      vidx=m_events%(vecfOut.size()-1);
      msg(MSG::DEBUG) << "Cluster #" << nCluster << ", label '" << label << "' size=" << vecdOut.size() << "value[" << vidx << "]=" << vecdOut[vidx] << endmsg;      
    }
    ++counter;
  } // loop over clusters

  label="EventDeco";
  double fOut;
  CHECK(m_userDataSvc->retrieve(label,fOut));
  msg(MSG::DEBUG) << "EventDeco with label '" << label << "' value=" << fOut << endmsg;

  return StatusCode::SUCCESS;
}

