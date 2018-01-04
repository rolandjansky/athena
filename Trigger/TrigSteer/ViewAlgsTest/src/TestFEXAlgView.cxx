/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"

#include "TestFEXAlgView.h"

namespace AthViews {

TestFEXAlgView::TestFEXAlgView(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_inputContainer("RoIsContainer"), 
    m_outputClusterContainer("OutputClusters"), 
    m_outputClusterContainerAux("OutputClusterAux."),
    m_outputProxyContainer("Output"), 
    m_outputProxyContainerAux("OutputAux.") {
   
  declareProperty("RoIsContainer", m_inputContainer, "Input RoIs");
  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Output collection");
  declareProperty("OutputClusterContainerAux", m_outputClusterContainerAux, "");
  declareProperty("OutputProxyContainer", m_outputProxyContainer, "Output proxies - this is returned by each fex and can be used to access actual objects");
  declareProperty("OutputProxyContainerAux", m_outputProxyContainerAux, "");
}

StatusCode TestFEXAlgView::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TestFEXAlgView::execute() {

  if ( not m_inputContainer.isValid() ) {
    ATH_MSG_ERROR("No decisions object prom previous stage");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Running on " << m_inputContainer->size() << " RoIs");
  
  m_outputClusterContainer = CxxUtils::make_unique< TestClusterContainer >();
  m_outputClusterContainerAux = CxxUtils::make_unique< TestClusterAuxContainer>();  
  m_outputClusterContainer->setStore(m_outputClusterContainerAux.ptr());
  
  m_outputProxyContainer = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputProxyContainerAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputProxyContainer->setStore(m_outputProxyContainerAux.ptr());

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  // collect RoIs
  // do reco and produce (say clusters - that part is missing in this example)
  // produce outptu object now with links to the 
  size_t nRoI = 0;
  for ( const auto roi : *m_inputContainer.cptr() ) {
    auto objWithLinkToRoI = ViewAlgs::find(roi, ViewAlgs::HasObject("initialRoI"));

    if ( objWithLinkToRoI ) {
      auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");      

      ATH_MSG_DEBUG("Producing output");
      TestCluster * output  = new TestCluster();          
      m_outputClusterContainer->push_back(output);    
      float etVal = std::abs((*roi)->eta()*(*roi)->phi());      
      TestEDM::setClusterEt(output, etVal); // I know this is nonsens but somehow have to mimic reco
      TestEDM::setClusterEta(output, (*roi)->eta());
      TestEDM::setClusterPhi(output, (*roi)->phi());      
      ATH_MSG_DEBUG("Created cluster of Et " << etVal);
      xAOD::TrigComposite* proxy  = new xAOD::TrigComposite();          
      m_outputProxyContainer->push_back(proxy);

      // This retrieval and cast could use some protection, but do we actually need this alg?
      SG::View * myView = dynamic_cast< SG::View * >( ctx.getExtension<Atlas::ExtendedEventContext>()->proxy() );
      proxy->setObjectLink("cluster", ElementLink<TestClusterContainer>(m_outputClusterContainer.name(), nRoI, myView ) );
      proxy->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_inputContainer.name(), nRoI, myView ) );
    } else {
      ATH_MSG_DEBUG("No path to RoI object");
    }
    

    nRoI++;
  }

  return StatusCode::SUCCESS;
}



}
