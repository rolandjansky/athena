/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <HIGlobal/HIEventShapeTriggerFast.h>
#include <CaloEvent/CaloCell.h>
#include <CaloEvent/CaloCellContainer.h>
#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>

#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#include <TH1D.h>
#include <cmath>

HIEventShapeTriggerFast::HIEventShapeTriggerFast(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)
{
  declareProperty("InputCellKey"         ,m_cell_container_key ="AllCalo"                    );
  declareProperty("OutputContainerKey"   ,m_output_key         ="HIEventShapeTriggerFast"    );
  declareProperty("OutputAuxContainerKey",m_outputaux_key      ="HIEventShapeTriggerFastAux" );
  HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast constructor\n");
}



StatusCode HIEventShapeTriggerFast::initialize()
{
  HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::initialize()\n");


  #ifdef __HIEVENTSHAPETRIGGERFAST_DEBUG__
  //Create and Book Histograms
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  for(int ihar=0;ihar<N_HAR;ihar++){
     char name1[400],name2[400];
     sprintf(name1,"h2_EbeV%d",ihar+2);
     h_EbeVn[ihar]=new TH1D(name1,"",200,0,0.4);
     sprintf(name2,"/MYSTREAM/%s",name1);
     CHECK(histSvc->regHist(name2,h_EbeVn[ihar]));
  }
  #endif

  return StatusCode::SUCCESS;
}



StatusCode HIEventShapeTriggerFast::execute()
{
   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute()\n");

   //retrieve the tower container from store
   const CaloCellContainer* CellContainer = 0;
   if(evtStore()->retrieve(CellContainer,m_cell_container_key).isFailure())
   {
     ATH_MSG_ERROR(" Could not retrieve CaloCell container with key "<< m_cell_container_key);
     return StatusCode::FAILURE;
   }


   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute() :: Starting loop over cells\n");
   double Total_Et  [N_DET]      = {0};
   double cos_moment[N_DET][N_HAR]={{0}};
   double sin_moment[N_DET][N_HAR]={{0}};
   //loop on Cells
   for(CaloCellContainer::const_iterator CellItr=CellContainer->begin();
       CellItr!=CellContainer->end(); CellItr++)
   {
       //Bad cell policy
       /*
       if(m_bad_cell_tool->SkipCell(*cellItr))
       {
       if(m_skipBadCells && (*cellItr)->badcell()) continue;
       }*/

      CaloCell *cell=(*CellItr);
      float eta     = cell->eta();
      float phi     = cell->phi();
      //int   layer   = cell->caloDDE()->getSampling();
      float cell_et = cell->et();

      int i_det=FULLCAL;      
      if(fabs(eta)>3.2) i_det=FCAL;
  
      double cos_angle=cos(phi);
      double sin_angle=sin(phi);
      double cos_i    =cos_angle*cos_angle - sin_angle*sin_angle;//cos 2phi
      double sin_i    =2*sin_angle*cos_angle;//sin 2phi

      Total_Et  [i_det]      +=cell_et;
      cos_moment[i_det][HAR2]+=cos_i*cell_et;
      sin_moment[i_det][HAR2]+=sin_i*cell_et;
      for(int i=HAR3; i<N_HAR; i++){
         cos_i=cos_i*cos_angle - sin_i*sin_angle;
         sin_i=sin_i*cos_angle + cos_i*sin_angle;
         cos_moment[i_det][i]+=cos_i*cell_et;
         sin_moment[i_det][i]+=sin_i*cell_et;
      }
   }//end Cell Loop
   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute() :: Finished loop over cells\n");
   

   //Add FCal to FULLCAL
   Total_Et[FULLCAL]+=Total_Et[FCAL];
   for(int i=HAR2; i<N_HAR; i++){
      cos_moment[FULLCAL][i]+=cos_moment[FCAL][i];
      sin_moment[FULLCAL][i]+=sin_moment[FCAL][i];
   }


   //Make new HIEventShape object
   xAOD::HIEventShapeContainer    *evtShape    =new xAOD::HIEventShapeContainer;
   xAOD::HIEventShapeAuxContainer *evtShapeAux =new xAOD::HIEventShapeAuxContainer;
   evtShape->setStore( evtShapeAux );
   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute() :: Finished Setting up containers\n");

   #ifdef __HIEVENTSHAPETRIGGERFAST_DEBUG__
   std::cout<<"Printing ShapeInfo Before Loop Over Towers"<<std::endl;
   PrintHIEventShapeContainer(evtShape);
   #endif

   for(int i_det=0;i_det<N_DET;i_det++){
      xAOD::HIEventShape *sh = new xAOD::HIEventShape();
      evtShape->push_back(sh);//Is it necessary to push in before accesing ??
      std::vector<float> cos_etsum;
      std::vector<float> sin_etsum;
      for(int ihar = 0; ihar<N_HAR; ihar++){
         cos_etsum.push_back(cos_moment[i_det][ihar]);
         sin_etsum.push_back(sin_moment[i_det][ihar]);
      }
      sh->setEt(Total_Et  [i_det]);
      sh->setEt_cos(cos_etsum);
      sh->setEt_sin(sin_etsum);
   }   
   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute() :: Filled up EventShape\n");

   CHECK(evtStore()->record(evtShape   ,m_output_key   ));
   CHECK(evtStore()->record(evtShapeAux,m_outputaux_key));
   HIEventShapeTriggerFast_MSG_DEBUG("Inside HIEventShapeTriggerFast::execute() :: Recorded the container\n");

   #ifdef __HIEVENTSHAPETRIGGERFAST_DEBUG__
   std::cout<<"Printing ShapeInfo After Loop Over Towers"<<std::endl;
   PrintHIEventShapeContainer(evtShape);
   #endif

  return StatusCode::SUCCESS;
}



StatusCode HIEventShapeTriggerFast::finalize()
{
  return StatusCode::SUCCESS;
}


#ifdef __HIEVENTSHAPETRIGGERFAST_DEBUG__
void HIEventShapeTriggerFast::PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container)
{
   for(int i=0;i<40;i++){std::cout<<"#";};
   std::cout<<std::endl;
      std::cout<<"|"<<std::setw(10)<<"EtaMin"<<"  |"\
                    <<std::setw(10)<<"EtaMax"<<"  |"\
                    <<std::setw(10)<<"Layer" <<"  |"\
                    <<std::setw(10)<<"NCells"<<"  |"\
                    <<std::setw(10)<<"Etsum"<<std::endl;
   unsigned int size=Container->size();
   for(unsigned int i=0;i<size;i++){
      const xAOD::HIEventShape *sh=Container->at(i);
      int NCells   =  sh->ncells();
      int Layer    =  sh->layer();
      float Etsum  =  sh->Etsum();
      float EtaMin =  sh->eta_min();
      float EtaMax =  sh->eta_max();

      if(Etsum==0) continue;
      std::cout<<"|"<<std::setw(10)<<EtaMin<<"  |"\
                    <<std::setw(10)<<EtaMax<<"  |"\
                    <<std::setw(10)<<Layer <<"  |"\
                    <<std::setw(10)<<NCells<<"  |"\
                    <<std::setw(10)<<Etsum<<std::endl;
   }
   for(int i=0;i<40;i++){std::cout<<"#";};
   std::cout<<std::endl;
}
#endif

