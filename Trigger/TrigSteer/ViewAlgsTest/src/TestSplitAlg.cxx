/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "ViewAlgs/ViewAlgs.h"

#include "./TestSplitAlg.h"

TestSplitAlg::TestSplitAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_inputProxyContainer("InputProxyContainer"), 
    m_referenceProxyContainer("ReferenceProxyContainer"), 
    //    m_outputCollNames("OutputCollNames")
    m_outputProxyContainer("SplitProxyContainer"), 
    m_outputProxyAuxContainer("SplitProxyContainerAux.")
{
  declareProperty("InputProxyContainer", m_inputProxyContainer, "Input Proxy Container");
  declareProperty("ReferenceProxyContainer", m_referenceProxyContainer, "Reference Proxy Container");
  // declareProperty("OutputCollNames", m_outputCollNames, "List of Names of output collections");
  declareProperty("SplitProxyContainer", m_outputProxyContainer, "Name of the decisions object (wiht links to specific RoIs)");
  declareProperty("SplitProxyContainerAux", m_outputProxyAuxContainer, "");
}

StatusCode TestSplitAlg::initialize() {
// ATH_CHECK( m_objKey.initialize() );
//   m_obj = new Obj(...);
//CHECK(m_hypos.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TestSplitAlg::execute() {

  if ( not m_inputProxyContainer.isValid() ) {
    ATH_MSG_ERROR("No decisions object from previous stage");
    return StatusCode::FAILURE;
  }

  unsigned int ref_key = ElementLink<xAOD::TrigCompositeContainer>(m_referenceProxyContainer.name(),0).key();
  ATH_MSG_DEBUG("Running on " << m_inputProxyContainer->size() << " Proxies, searching for "<<ref_key<<" key");
  
  //copying from Tomasz'
  // IProxyDict * view = 0;
  // // redirect handles to that view
  // CHECK(m_outputRoisContainer.setStore(view));

  m_outputProxyContainer = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputProxyAuxContainer = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputProxyContainer->setStore(m_outputProxyAuxContainer.ptr());

  if ( m_inputProxyContainer->size() == 0)  return StatusCode::SUCCESS;

  if ( m_inputProxyContainer->size() != 1) {
    ATH_MSG_ERROR("Input Collection has more than one element. Check your configuration! ");
    return StatusCode::FAILURE;
  }

  //  std::string ref_key=m_referenceProxyContainer.key();//collection stored with a name
  // link stored with a hash (unsigned) key and dataID=name of the collection the object is on
  // they both indicate the collection (by name or by key)


  // get links of children
  for (const auto& outp : *m_inputProxyContainer) {
    ElementLinkVector<xAOD::TrigCompositeContainer> linkVector;
    bool hasChildren = ViewAlgs::recursiveGetObjectLinks( outp, linkVector);
    if (! hasChildren ){
      ATH_MSG_ERROR("No children ObjectLinks found during readback");
      continue;
    }

    for (auto link: linkVector ){  
      //search element of that key
      if (link.key() != ref_key) {
        //  ATH_MSG_DEBUG("Found differnt key "<<link.key()<<", continue");
        continue;
      }
      xAOD::TrigComposite* proxy  = new xAOD::TrigComposite(); 

      /* use this when use vector of writehandles
      std::string collName=std::to_string(link.key());
      //decide which container
      SG::WriteHandle< xAOD::TrigCompositeContainer > container(collName);
      std::vector< SG::WriteHandle< xAOD::TrigCompositeContainer > >::iterator cont_it;
      cont_it = std::find(m_outputVectorProxyColl.begin(), m_outputVectorProxyColl.end(), container);
      if (cont_it == m_outputVectorProxyColl.end() ){
        //create new WriteHandle
        container = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
        SG::WriteHandle< xAOD::TrigCompositeAuxContainer > containerAux;
        containerAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
        container->setStore(containerAux.ptr());
        m_outputVectorProxyColl.push_back( container);
        m_outputVectorProxyAuxColl.push_back( containerAux);//needed?
        cont_it = m_outputVectorProxyColl.end();
        m_outputCollNames+=collName+",";
      }
      (*cont_it).ptr()->push_back(proxy);
      */

      m_outputProxyContainer->push_back(proxy);
      proxy->setObjectLink("seed", link ); // set ref. back to the proxy  
      // set the name of the proxy as the name of the container ?
      auto compByElementLink = *link;
      auto objWithLinkToRoI = ViewAlgs::find(compByElementLink, ViewAlgs::HasObject("initialRoI"));  
      if ( objWithLinkToRoI ) {
        auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");     
        double eta = (*roi)->eta();
        double phi = (*roi)->phi();
        ATH_MSG_DEBUG("Added Proxy from collection "<< link.key()<<" dataID="<< link.dataID()<< " and link to this RoI: eta="<<eta<<" phi="<<phi);
      }
      else{
        ATH_MSG_DEBUG("This Children Proxy from collection "<< link.key()<<" has no link to any RoI");
      }   
    }

  }

  ATH_MSG_DEBUG("Created container from key " << ref_key <<" with "<< m_outputProxyContainer.cptr()->size()<<" elements ");
  //  if (m_outputProxyContainer.cptr()->size() != 0){
    //  ATH_MSG_DEBUG("ref_key="<<ref_key <<" new key="<<ElementLink<xAOD::TrigCompositeContainer>(m_outputProxyContainer.name(),0).key());
  //  }

  //  ATH_MSG_DEBUG("Created " << m_outputVectorProxyColl.size() <<" proxy containers ");
    


  
  return StatusCode::SUCCESS;
}



