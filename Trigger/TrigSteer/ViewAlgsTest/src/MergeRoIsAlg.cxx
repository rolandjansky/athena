/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#undef NDEBUG 
//--> enable assert
//#include <cassert>

#include <sstream>
#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "ViewAlgs/ViewAlgs.h"

#include "./MergeRoIsAlg.h"

MergeRoIsAlg::MergeRoIsAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_useSuperRoI(true),
    m_inputRoisContainer("InputRoIs"), 
    m_outputRoisContainer("OutputRoIs")
{
  declareProperty("UseSuperRoI", m_useSuperRoI, "Set output to a composite suepr RoI");
  declareProperty("InputRoIsContainer",  m_inputRoisContainer, "Name of the input RoIs object");
  declareProperty("OutputRoIsContainer", m_outputRoisContainer, "Name of the RoIs object produced by the merging");
}




StatusCode MergeRoIsAlg::initialize() {
  return StatusCode::SUCCESS;
}



StatusCode MergeRoIsAlg::execute() {

 

  IProxyDict * view = 0;
  
  // redirect handles to that view
  CHECK(m_outputRoisContainer.setProxyDict(view));
  //  m_outputRoIsContainer->setStore(m_outputRoIsContainerAux.ptr());
// prepare output, no Aux for it, just interface container as this going
  // to be a view container
  //  m_outputRoIsContainer->clear(SG::VIEW_ELEMENTS);
  m_outputRoisContainer = CxxUtils::make_unique< TrigRoiDescriptorCollection >();  


  //  std::unique_ptr<TrigRoiDescriptor> superRoi = CxxUtils::make_unique<TrigRoiDescriptor>();
  if(m_useSuperRoI){
    auto superRoI = new TrigRoiDescriptor();
    superRoI->setComposite(true);
    m_outputRoisContainer->push_back(superRoI);

    for (auto roi: *m_inputRoisContainer.cptr()){
      superRoI->push_back(new TrigRoiDescriptor( *roi));
    }
    ATH_MSG_DEBUG("superRoi: " << *superRoI);
  }


  ATH_MSG_DEBUG("Ouput RoI container has "<<m_outputRoisContainer->size()<<" elements");

  return StatusCode::SUCCESS;
}



