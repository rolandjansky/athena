/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <HIGlobal/HIEventShapeMaker.h>
#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>

#include <GaudiKernel/ServiceHandle.h>


HIEventShapeMaker::HIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator),
											  m_HIEventShapeFillerTool(this),
											  m_summary_tool(this)
{
  declareProperty("InputTowerKey"         ,m_tower_container_key="CombinedTower"            );
  declareProperty("InputCellKey"          ,m_cell_container_key ="AllCalo"                  );
  declareProperty("UseCaloCell"           ,m_use_calo_cell      = false                     );
  declareProperty("OutputContainerKey"    ,m_output_key         ="HIEventShapeContainer"    );
  declareProperty("HIEventShapeFillerTool",m_HIEventShapeFillerTool                         );
  declareProperty("OrderOfFlowHarmonics"  ,m_NumOrders          =7                          );
  declareProperty("SummaryTool"           ,m_summary_tool                                   );
  declareProperty("SummaryContainerKey"   ,m_summary_key=""                                 );
  declareProperty("SummaryOnly"           , m_summary_only=false                            );
}



StatusCode HIEventShapeMaker::initialize()
{
  ATH_MSG_DEBUG("Inside HIEventShapeMaker::initialize()");

  //Create the HIEventShapeFillerTool
  if(!m_summary_only)
  {
    CHECK(m_HIEventShapeFillerTool.retrieve());
    m_HIEventShapeFillerTool->SetContainerName(m_output_key);
    //should be configured so that tool and alg never have different # orders
    //print warning if this is the case
    CHECK(m_HIEventShapeFillerTool->SetNumOrders(m_NumOrders));
  }
  if(m_summary_key.compare("")!=0) CHECK(m_summary_tool->initialize());

  return StatusCode::SUCCESS;
}



StatusCode HIEventShapeMaker::execute()
{
  ATH_MSG_DEBUG("Inside HIEventShapeMaker::execute()");

  //Create HIEventShape Collection and ask tool to initialize it
  xAOD::HIEventShapeContainer* evtShape=nullptr;
  const xAOD::HIEventShapeContainer* evtShape_const=nullptr;
  if(m_summary_only) CHECK(evtStore()->retrieve(evtShape_const,m_output_key));
  else
  {
    evtShape=new xAOD::HIEventShapeContainer; 
    xAOD::HIEventShapeAuxContainer *evtShapeAux = new xAOD::HIEventShapeAuxContainer; 
    evtShape->setStore( evtShapeAux ); 
    ATH_MSG_DEBUG("Making HIEventShapeContainer with key " << m_output_key);

    CHECK(evtStore()->record(evtShape,m_output_key));
    CHECK(evtStore()->record(evtShapeAux,m_output_key+std::string("Aux.")));
    CHECK(m_HIEventShapeFillerTool->InitializeCollection(evtShape));

    ATH_MSG_DEBUG("Calling filler tool with m_use_calo_cell= " << m_use_calo_cell);
    if(m_use_calo_cell) CHECK(m_HIEventShapeFillerTool->FillCollectionFromCells (m_cell_container_key ));
    else CHECK(m_HIEventShapeFillerTool->FillCollectionFromTowers(m_tower_container_key));
  }

  if(m_summary_key.compare("")!=0)
  {
    xAOD::HIEventShapeContainer* es_summary=new xAOD::HIEventShapeContainer; 
    xAOD::HIEventShapeAuxContainer *es_summary_aux = new xAOD::HIEventShapeAuxContainer; 
    es_summary->setStore( es_summary_aux ); 
    CHECK(evtStore()->record(es_summary,m_summary_key));
    CHECK(evtStore()->record(es_summary_aux,m_summary_key+std::string("Aux.")));
    if(m_summary_only)CHECK(m_summary_tool->summarize(evtShape_const,es_summary));
    else CHECK(m_summary_tool->summarize(evtShape,es_summary));
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


