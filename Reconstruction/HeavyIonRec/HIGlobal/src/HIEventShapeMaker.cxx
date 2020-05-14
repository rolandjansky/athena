/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <HIGlobal/HIEventShapeMaker.h>
#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>

#include <GaudiKernel/ServiceHandle.h>
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

HIEventShapeMaker::HIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)//,
{
}


StatusCode HIEventShapeMaker::initialize()
{
  ATH_MSG_INFO("Inside HIEventShapeMaker::initialize()");

	//First we initialize keys - after initialization they are frozen
  ATH_CHECK( m_towerContainerKey.initialize(!m_useCaloCell) );
  ATH_CHECK( m_cellContainerKey.initialize(m_useCaloCell) );
  ATH_CHECK( m_outputKey.initialize(!m_summaryOnly) );
  ATH_CHECK( m_readExistingKey.initialize(m_summaryOnly) );
  ATH_CHECK( m_summaryKey.initialize(!m_summaryKey.key().empty()) );

  //Create the HIEventShapeFillerTool
  if(!m_summaryOnly)
  {
    ATH_CHECK(m_HIEventShapeFillerTool.retrieve());
    m_HIEventShapeFillerTool->setContainerName(m_outputKey.key());
    //should be configured so that tool and alg never have different # orders
    //print warning if this is the case
    ATH_CHECK(m_HIEventShapeFillerTool->setNumOrders(m_numOrders));
  }
  if( m_summaryKey.key().compare("") != 0 ) ATH_CHECK(m_summaryTool->initialize());

  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeMaker::execute()
{
  ATH_MSG_INFO("Inside HIEventShapeMaker::execute()");

  const xAOD::HIEventShapeContainer* evtShape_const=nullptr;
  if( m_summaryOnly ){
     SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_readExistingKey );
     if (!readHandleEvtShape.isValid()) {
        ATH_MSG_FATAL( "Could not find HI event shape! (" << m_readExistingKey.key() << ")." );
        return(StatusCode::FAILURE);
     }
     evtShape_const = readHandleEvtShape.cptr();
  }
  else
  {
    ATH_MSG_INFO("Making HIEventShapeContainer with key " << m_outputKey.key());
    SG::WriteHandle<xAOD::HIEventShapeContainer> writeHandleEvtShape ( m_outputKey );
    ATH_CHECK( writeHandleEvtShape.record ( std::make_unique<xAOD::HIEventShapeContainer>(),
                                            std::make_unique<xAOD::HIEventShapeAuxContainer>()));
    ATH_CHECK(m_HIEventShapeFillerTool->initializeCollection(writeHandleEvtShape.ptr()));
    evtShape_const=writeHandleEvtShape.cptr();

    ATH_MSG_INFO("Calling filler tool with m_useCaloCell= " << m_useCaloCell);
    if(m_useCaloCell) ATH_CHECK(m_HIEventShapeFillerTool->fillCollectionFromCells ( m_cellContainerKey ));
    else ATH_CHECK(m_HIEventShapeFillerTool->fillCollectionFromTowers( m_towerContainerKey ));
  }

  //PrintHIEventShapeContainer( evtShape_const );

  if(!m_summaryKey.key().empty())
  {
    SG::WriteHandle<xAOD::HIEventShapeContainer> write_handle_esSummary ( m_summaryKey );
    ATH_CHECK( write_handle_esSummary.record ( std::make_unique<xAOD::HIEventShapeContainer> (),
                                               std::make_unique<xAOD::HIEventShapeAuxContainer>()));
    if(m_summaryOnly) ATH_CHECK(m_summaryTool->summarize(evtShape_const,write_handle_esSummary.ptr()));
    else ATH_CHECK(m_summaryTool->summarize(evtShape_const,write_handle_esSummary.ptr()));
  }
  return StatusCode::SUCCESS;
}



StatusCode HIEventShapeMaker::finalize()
{
  return StatusCode::SUCCESS;
}


void HIEventShapeMaker::PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container)
{
   for(int i=0;i<40;i++){std::cout<<"#";};
   std::cout<<std::endl;
      std::cout<<"|"<<std::setw(10)<<"EtaMin"<<"  |"\
                    <<std::setw(10)<<"EtaMax"<<"  |"\
                    <<std::setw(10)<<"Layer" <<"  |"\
                    <<std::setw(10)<<"NCells"<<"  |"\
                    <<std::setw(10)<<"Et"<<std::endl;
   unsigned int size=Container->size();
   if(size>10) size=10;
   for(unsigned int i=0;i<size;i++){
      const xAOD::HIEventShape *sh=Container->at(i);
      int NCells   =  sh->nCells();
      int Layer    =  sh->layer();
      float Et     =  sh->et();
      float EtaMin =  sh->etaMin();
      float EtaMax =  sh->etaMax();

      if(Et==0) continue;
      std::cout<<"|"<<std::setw(10)<<EtaMin<<"  |"\
                    <<std::setw(10)<<EtaMax<<"  |"\
                    <<std::setw(10)<<Layer <<"  |"\
                    <<std::setw(10)<<NCells<<"  |"\
                    <<std::setw(10)<<Et<<std::endl;
   }
   for(int i=0;i<40;i++){std::cout<<"#";};
   std::cout<<std::endl;
}
