/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <HIGlobal/HIEventShapeMaker.h>
#include <NavFourMom/INavigable4MomentumCollection.h>
#include <CaloEvent/CaloCell.h>
#include <Navigation/NavigationToken.h>
#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>

#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#include <TH2D.h>
#include <set>
#include <cmath>

HIEventShapeMaker::HIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTowerKey"        ,m_tower_container_key="CombinedTower"            );
  declareProperty("OutputContainerKey"   ,m_output_key         ="HIEventShapeContainer"    );
  declareProperty("OutputAuxContainerKey",m_output_aux_key     ="HIEventShapeContainerAux.");
}



StatusCode HIEventShapeMaker::initialize()
{
  ATH_MSG_DEBUG("Inside HIEventShapeMaker::initialize()");

  //Create the HIEventShapeFillerTool
  //m_HIEventShapeFillerTool.setTypeAndName("HIEventShapeFillerTool/EvtShapeModifierTool");
  //CHECK(m_HIEventShapeFillerTool.retrieve());


  //Create and Book Histograms
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  h2_eta_maxSamp= new TH2D("h2_eta_maxSamp","num max samplings vs eta",100,-5,5,24,-0.5,23.5);
  h2_eta_samp= new TH2D("h2_eta_samp","samplings vs eta",100,-5,5,24,-0.5,23.5);
  histSvc->regHist("/MYSTREAM/h2_eta_maxSamp",h2_eta_maxSamp).ignore(); //or check the statuscode
  histSvc->regHist("/MYSTREAM/h2_eta_samp"   ,h2_eta_samp   ).ignore(); //or check the statuscode


  return StatusCode::SUCCESS;
}



StatusCode HIEventShapeMaker::execute()
{
  ATH_MSG_DEBUG("Inside HIEventShapeMaker::execute()");

  //retrieve the tower container from store
  const INavigable4MomentumCollection* navInColl = 0;
  CHECK(evtStore()->retrieve(navInColl,m_tower_container_key));

  //
  // ARA
  //Will be replaced by dedicated tool
  xAOD::HIEventShapeContainer* evtShape=new xAOD::HIEventShapeContainer; 
  xAOD::HIEventShapeAuxContainer *evtShapeAux = new xAOD::HIEventShapeAuxContainer; 
  evtShape->setStore( evtShapeAux ); 
  CHECK(evtStore()->record(evtShape,m_output_key));
  CHECK(evtStore()->record(evtShapeAux,m_output_key+std::string("Aux.")));
  int N_ETA_SLICES = 100;
  int N_CALO_LAYERS = 24;
  int N_ORDERS = 4;

  for(int i=0; i<N_ETA_SLICES; i++){
    for(int j=0; j<N_CALO_LAYERS; j++){
      xAOD::HIEventShape *sh = new xAOD::HIEventShape();


      evtShape->push_back(sh);
     
      sh->setEt(0);
      sh->setArea(0);
      sh->setRho(0);
      sh->setNcells(0);

      std::vector<float> cos_et(N_ORDERS,0);
      std::vector<float> sin_et(N_ORDERS,0);
      sh->setEt_cos(cos_et);
      sh->setEt_sin(sin_et);
      sh->setLayer(j);
      sh->setEtaMin(HICaloHelper::GetBinLowEdgeEta(i));
      sh->setEtaMax(HICaloHelper::GetBinUpEdgeEta(i));

    }
  }
  //
  // \ARA
  //

  //CHECK(m_HIEventShapeFillerTool->InitializeCollection());
  //CHECK(m_HIEventShapeFillerTool->RecordCollection(m_output_key,m_output_aux_key));

  #define __HIEVENTSHAPEMAKER_DEBUG__
  #ifdef __HIEVENTSHAPEMAKER_DEBUG__
  std::cout<<"Printing ShapeInfo Before Loop Over Towers"<<std::endl;
  //PrintHIEventShapeContainer(m_HIEventShapeFillerTool->GetHIEventShapeContainer());
  #endif

  //loop on towers
  for(INavigable4MomentumCollection::const_iterator towerItr=navInColl->begin();
      towerItr!=navInColl->end(); towerItr++)
  {
    std::set<unsigned int> samps;
    //navigate back to cells
    //Default is to sort the cells by either pointer values leading to irreproducible output
    //CaloCellIDFcn ensures cells are ordered by their IDs
    NavigationToken<CaloCell,double,CaloCellIDFcn> cellToken;
    (*towerItr)->fillToken(cellToken,double(1.));

    //
    // ARA
    // Use eta/phi of tower in shape calculation
    float eta0=(*towerItr)->eta();
    float phi0=(*towerItr)->phi();

    if ( cellToken.size() == 0 ) continue;
    for(NavigationToken<CaloCell,double,CaloCellIDFcn>::const_iterator cellItr = cellToken.begin();
	cellItr != cellToken.end(); cellItr++ )
    {
      //Bad cell policy
      /*
      if(m_bad_cell_tool->SkipCell(*cellItr))
      {
      if(m_skipBadCells && (*cellItr)->badcell()) continue;
      }*/

      unsigned int sample = (CaloSampling::CaloSample) (*cellItr)->caloDDE()->getSampling();
      samps.insert(sample);
      h2_eta_samp->Fill((*towerItr)->eta(),sample);
      double geoWeight = cellToken.getParameter(*cellItr); 


      //
      // ARA
      // Use eta/phi of tower in shape calculation
      HICaloHelper::UpdateShape(evtShape,*cellItr,geoWeight,eta0,phi0);
      //CHECK(m_HIEventShapeFillerTool->UpdateWithCell(*cellItr,geoWeight));

    }//end cell loop
    h2_eta_maxSamp->Fill((*towerItr)->eta(),samps.size());

  }//end tower loop


  #ifdef __HIEVENTSHAPEMAKER_DEBUG__
  std::cout<<"Printing ShapeInfo After Loop Over Towers"<<std::endl;
  //PrintHIEventShapeContainer(m_HIEventShapeFillerTool->GetHIEventShapeContainer());
  #endif

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
   for(unsigned int i=0;i<size;i++){
      int NCells   =  Container->at(i)->nCells();
      int Layer    =  Container->at(i)->layer();
      float Et    =  Container->at(i)->Et();
      float EtaMin =  Container->at(i)->etaMin();
      float EtaMax =  Container->at(i)->etaMax();
        //sh->setEt_cos(cos_et);
        //sh->setEt_sin(sin_et);

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


