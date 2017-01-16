/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"

#include "./CopyPassing.h"



CopyPassing::CopyPassing(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_all("AllHasToBeSet"),
    m_passing("PassingHasToBeSet"),
    m_output("OutputHasToBeSet")//,
    //    m_outputAux("OutputHasToBeSetAux.")
{
  declareProperty("CollectionBeforeMenu", m_all, "Handle of the TrigComposites before menu selection");
  declareProperty("CollectionAfterMenu", m_passing, "Handle of the TrigComposites after menu selection");
  declareProperty("Output", m_output, "Handle of the TrigComposites copied from the all objects, but only these passing the menu selection");
  //  declareProperty("OutputAux", m_outputAux, "Handle of the TrigComposites Aux store fro OutputObjects");  
}
StatusCode CopyPassing::initialize() {
  CHECK( m_output.initialize() );  
  return StatusCode::SUCCESS;
}

StatusCode CopyPassing::execute() {
  // prepare output
  if ( not m_all.isValid() ) {
    ATH_MSG_ERROR("Can not pull container wihth object before the decision");
    return StatusCode::FAILURE;
  }

  if ( not m_passing.isValid() ) {
    ATH_MSG_ERROR("Can not pull container wihth object after the decision");
    return StatusCode::FAILURE;
  }



  ATH_MSG_DEBUG("all->size()="<<m_all->size()<<" pass->size()="<<m_passing->size());

  auto output = CxxUtils::make_unique< ConstDataVector<xAOD::TrigCompositeContainer> >();
  output->clear(SG::VIEW_ELEMENTS);
  //  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  //  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer >();
  //  m_output->setStore(m_outputAux.ptr());
  
  const std::set<const xAOD::TrigComposite*> allSet( m_all.cptr()->begin(), m_all.cptr()->end() );
  for ( auto ptr: allSet) 
    ATH_MSG_DEBUG("In the set " << ptr);
  
  

  for ( size_t i=0; i < m_passing->size(); i++ ) {    
    const xAOD::TrigComposite* tcSeed = m_passing->at(i)->object<xAOD::TrigComposite>("seed"); 
    
    //    const xAOD::TrigComposite* tcSeed = tcSeedZero->object<xAOD::TrigComposite>("seed"); 
    ATH_MSG_DEBUG("Obtained seed " << tcSeed);
    if ( allSet.find(tcSeed) != allSet.end() ) {
      output->push_back(tcSeed);
      //xAOD::TrigComposite * refToPassing  = new xAOD::TrigComposite();
      //    m_output->push_back(refToPassing);
      //m_output->push_back(refToPassing);
      //  refToPassing
      ATH_MSG_DEBUG("copying object");      
    } else {
      ATH_MSG_DEBUG("skipping object");
    }
  }

  ATH_MSG_DEBUG("output has " << output->size()<<" elements");
  SG::WriteHandle<ConstDataVector<xAOD::TrigCompositeContainer> > handle( m_output );
  CHECK(handle.record( std::move(output) ));

  return StatusCode::SUCCESS;
}
