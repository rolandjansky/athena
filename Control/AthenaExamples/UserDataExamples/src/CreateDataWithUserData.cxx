/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CreateDataWithUserData.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "AthContainers/UserDataStore.h"
#include "AthenaKernel/errorcheck.h"

using namespace UserDataExamples;

CreateDataWithUserData::CreateDataWithUserData( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_userDataSvc("UserDataSvc",name)
{
  declareProperty("UserDataSvc",m_userDataSvc);
  declareProperty("ContainerName",m_contName="");
  declareProperty("VecSize",m_vecSize=10);
  m_nEvents=0;
  m_nClusters=0;
  m_nAttributes=0;
}

// Destructor
///////////////
CreateDataWithUserData::~CreateDataWithUserData()
{}


StatusCode CreateDataWithUserData::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  StatusCode sc=m_userDataSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve UserDataSvc " << m_userDataSvc << endmsg;
    return sc;
  }



  return StatusCode::SUCCESS;
}

StatusCode CreateDataWithUserData::finalize() {
  msg(MSG::INFO) << "Counters: Events: " << m_nEvents << ", Clusters: " << m_nClusters << ", Attributes: " << m_nAttributes << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreateDataWithUserData::execute() { 
  
  std::vector<int32_t> vec(m_vecSize,-5);
  std::vector<uint32_t> vecu(m_vecSize,15);
  std::vector<float> vecf(m_vecSize,5.5);
  std::vector<double> vecd(m_vecSize,5.55);

  ++m_nEvents;
  CaloClusterContainer* clusterContainer=new CaloClusterContainer();

  StatusCode sc=evtStore()->record(clusterContainer,m_contName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CaloClusterContainer with name " << m_contName << endmsg;
    return sc;
  }
  std::string label;
  int counter=0;
  for(unsigned nClu=0;nClu<500;++nClu) {
    CaloCluster* cluster=new CaloCluster(1.0,1.0);
    clusterContainer->push_back(cluster);
    ++m_nClusters;
    //Create 20 attributes for caloCluster:
    for (unsigned i=0;i<2;++i) {
      std::stringstream sLabel;
      sLabel << "Counter_" << i;
      label=sLabel.str();
      sc=m_userDataSvc->decorateElement(*cluster,label,counter);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }

      int counterOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,counterOut));
      if (!checkvalues(counter,counterOut,label)) return StatusCode::FAILURE;
      
      //Same with double;
      double dbl=0.5+counter;
      label=sLabel.str()+"d";
      sc=m_userDataSvc->decorateElement(*cluster,label,dbl);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }

      double dblOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,dblOut));
      if (!checkvalues(dbl,dblOut,label)) return StatusCode::FAILURE;


      //Same with unsigned:
      uint32_t cnt_u=counter+1000;
      label=sLabel.str()+"u";
      sc=m_userDataSvc->decorateElement(*cluster,label,cnt_u);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }
      

      uint32_t cnt_uOut=0;
      CHECK(m_userDataSvc->retrieve(*cluster,label,cnt_uOut));
      if (!checkvalues(cnt_u,cnt_uOut,label)) return StatusCode::FAILURE;


      //Same with float:
      float cnt_f=counter*3.14;
      label=sLabel.str()+"f";
      sc=m_userDataSvc->decorateElement(*cluster,label,cnt_f);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }
      
      float cnt_fOut=0;
      CHECK(m_userDataSvc->getElementDecoration(*cluster,label,cnt_fOut));
      if (!checkvalues(cnt_fOut,cnt_fOut,label)) return StatusCode::FAILURE;


      //Same with bool
      bool b=(counter%2)==1;
      label=sLabel.str()+"b";
      sc=m_userDataSvc->decorateElement(*cluster,label,b);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }
      bool bout=false;
      CHECK(m_userDataSvc->getElementDecoration(*cluster,label,bout));
      if (!checkvalues(b,bout,label)) return StatusCode::FAILURE;


      void* x=(void*)(&vec);
      //std::string l=labelidx+"vVirt";
      label=sLabel.str()+"vVirt";
      if (m_userDataSvc->vdecorateElement(*cluster,label,typeid(std::vector<int32_t>),x)) {
	msg(MSG::ERROR) << "vdecorateElement failed" << endmsg;
	return StatusCode::FAILURE;
      }
      
      void* vecOut=0;
      int stat=m_userDataSvc->vgetElementDecoration(*cluster,label,typeid(std::vector<int32_t>),vecOut);
      if (stat) {
	msg(MSG::ERROR) << "vgetElementDecoration with label=" << label << " failed." << endmsg;
	return StatusCode::FAILURE;
      }
      
      std::vector<int32_t>* vptrOut=static_cast<std::vector<int32_t>* >(vecOut); 
      if (!checkvalues(vec,*vptrOut,label)) return StatusCode::FAILURE;
            

      label=sLabel.str()+"v";
      sc=m_userDataSvc->decorateElement(*cluster,label,vec);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }


      std::vector<int32_t> vecOut1;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecOut1));
      if (!checkvalues(vec,vecOut1,label)) return StatusCode::FAILURE;


      label=sLabel.str()+"vu";
      sc=m_userDataSvc->decorateElement(*cluster,label,vecu);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }

      std::vector<uint32_t> vecuOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecuOut));
      if (!checkvalues(vecu,vecuOut,label)) return StatusCode::FAILURE;


      label=sLabel.str()+"vf";
      sc=m_userDataSvc->decorateElement(*cluster,label,vecf);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }

      std::vector<float> vecfOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecfOut));
      if (!checkvalues(vecf,vecfOut,label)) return StatusCode::FAILURE;


      label=sLabel.str()+"vd";
      sc=m_userDataSvc->decorateElement(*cluster,label,vecd);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to decorate CaloCluster with an attribute called " << label << endmsg;
	return sc;
      }
      else {
	ATH_MSG_DEBUG("Successfully decorate CaloCluster with an attribute called " << label);
	++m_nAttributes;
      }

      std::vector<double> vecdOut;
      CHECK(m_userDataSvc->retrieve(*cluster,label,vecdOut));
      if (!checkvalues(vecd,vecdOut,label)) return StatusCode::FAILURE;
      
    }
    ++counter;
  } // loop over clusters

  label="EventDeco";
  sc=m_userDataSvc->decorateEvent(label,42.0);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record EventDecoration called " << label << endmsg;
  }

  double fOut = 0;
  CHECK(m_userDataSvc->retrieve(label,fOut));
  if (!checkvalues(42.0,fOut,label)) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

