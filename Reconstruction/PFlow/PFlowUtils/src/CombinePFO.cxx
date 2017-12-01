#include "CombinePFO.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODCore/ShallowCopy.h"

CombinePFO::CombinePFO(const std::string& name, ISvcLocator* pSvcLocator) :  AthAlgorithm(name, pSvcLocator) {}

StatusCode CombinePFO::initialize(){

  ATH_CHECK(m_PFOContainerReadHandleKeyA.initialize());
  ATH_CHECK(m_PFOContainerReadHandleKeyB.initialize());
  ATH_CHECK(m_PFOContainerWriteHandleKey.initialize());

  return StatusCode::SUCCESS;  
}

StatusCode CombinePFO::execute(){

  SG::WriteHandle<xAOD::PFOContainer> PFOContainerWriteHandle(m_PFOContainerWriteHandleKey);

  ATH_CHECK(PFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(SG::VIEW_ELEMENTS)));
  
  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandleA(m_PFOContainerReadHandleKeyA);
  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandleB(m_PFOContainerReadHandleKeyB);

  std::pair< xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > shallowCopyOfPFOContainerA =  xAOD::shallowCopyContainer(*PFOContainerReadHandleA);    
  std::pair< xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > shallowCopyOfPFOContainerB =  xAOD::shallowCopyContainer(*PFOContainerReadHandleB);    

  PFOContainerWriteHandle->assign(shallowCopyOfPFOContainerA.first->begin(),shallowCopyOfPFOContainerA.first->end());
  PFOContainerWriteHandle->insert(PFOContainerWriteHandle->end(),shallowCopyOfPFOContainerB.first->begin(),shallowCopyOfPFOContainerB.first->end());
  
  return StatusCode::SUCCESS;  
  
}

StatusCode CombinePFO::finalize(){
  return StatusCode::SUCCESS;
}
