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
#include "ViewAlgs/ViewAlgs.h"

#include "./MergeRoIsAlg.h"

MergeRoIsAlg::MergeRoIsAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_useSuperRoI(true),
    //    m_inputRoisContainer("InputRoIs"), 
    m_outputRoIContainer("OutputRoIs")
{
  declareProperty("UseSuperRoI", m_useSuperRoI, "Set output to a composite suepr RoI");
  declareProperty("InputRoIContainers",  m_inputRoIContainerKeys, "Name of the input RoIs object");
  declareProperty("OutputRoIContainer", m_outputRoIContainer, "Name of the RoIs object produced by the merging");
}




StatusCode MergeRoIsAlg::initialize() {
  return StatusCode::SUCCESS;
}



StatusCode MergeRoIsAlg::execute() {

 

  IProxyDict * view = 0;
  
  // redirect handles to that view
  CHECK(m_outputRoIContainer.setProxyDict(view));
  //  m_outputRoIsContainer->setStore(m_outputRoIsContainerAux.ptr());
  // prepare output, no Aux for it, just interface container as this going
  // to be a view container
  //  m_outputRoIsContainer->clear(SG::VIEW_ELEMENTS);
  m_outputRoIContainer = CxxUtils::make_unique< TrigRoiDescriptorCollection >();  


  std::vector<SG::ReadHandle<TrigRoiDescriptorCollection>> input = m_inputRoIContainerKeys.makeHandles();
  for ( auto & col: input )
    CHECK( col.isValid() );





  //  std::unique_ptr<TrigRoiDescriptor> superRoi = CxxUtils::make_unique<TrigRoiDescriptor>();
  if(m_useSuperRoI){
    auto superRoI = new TrigRoiDescriptor();
    superRoI->setComposite(true);
    m_outputRoIContainer->push_back(superRoI);

    for (auto & roiColl: input){
      for ( auto roi: *roiColl.cptr() ) {
	superRoI->push_back(new TrigRoiDescriptor( *roi));
      }
    }
    ATH_MSG_DEBUG("superRoi: " << *superRoI);
  } else {
    for (auto & roiColl: input){
      for ( auto roi: *roiColl.cptr() ) {
	m_outputRoIContainer->push_back(new TrigRoiDescriptor( *roi));
      }
    }
  }


  ATH_MSG_DEBUG("Ouput RoI container has "<<m_outputRoIContainer->size()<<" elements");

  return StatusCode::SUCCESS;
}



