/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIEventShapeFillerTool.h"


//#define __HIEventShapeFillerTool__DEBUG
#ifdef __HIEventShapeFillerTool__DEBUG
  #include "stdio.h"
  #define HIEventShapeFillerTool_MSG_DEBUG(...) {printf("HIEventShapeFillerTool  ::   ");printf(__VA_ARGS__);}
#else
  #define HIEventShapeFillerTool_MSG_DEBUG(...) {while(0);}
#endif

HIEventShapeFillerTool::HIEventShapeFillerTool(const std::string& myname)  : asg::AsgTool(myname) 
{
#ifdef ASGTOOL_ATHENA
   declareInterface<IHIEventShapeFiller>(this);
#endif

   //declareProperty("EventShapeKey", m_event_shape_key,"Name of event shape container in SG");//Now Set by caller function not by joboption

   evtShape=0;
   evtShapeAux=0;

}


StatusCode HIEventShapeFillerTool::InitializeCollection()
{
   ATH_MSG_DEBUG("INSIDE InitializeCollection");
   HIEventShapeFillerTool_MSG_DEBUG("%s\n","HIEventShapeFillerTool::InitializeCollection()");

   //Make new HIEventShape object
   evtShape=new xAOD::HIEventShapeContainer;
   evtShapeAux =new xAOD::HIEventShapeAuxContainer;
   evtShape->setStore( evtShapeAux );

   for(int i=0; i<N_ETA_SLICES; i++){
     for(int j=0; j<N_CALO_LAYERS; j++){
	xAOD::HIEventShape *sh = new xAOD::HIEventShape();
	evtShape->push_back(sh);//necessary to push in before accesing
	std::vector<float> cos_etsum;
	std::vector<float> sin_etsum;
        cos_etsum.assign(N_ORDERS,0.0);
        sin_etsum.assign(N_ORDERS,0.0);
	sh->setNcells(0);
	sh->setLayer(j);
	sh->setEt(0);
	sh->setEtaMin(HICaloHelper::GetBinLowEdgeEta(i));
	sh->setEtaMax(HICaloHelper::GetBinUpEdgeEta(i));
	sh->setEt_cos(cos_etsum);
	sh->setEt_sin(sin_etsum);
     }
   }


   return StatusCode::SUCCESS;
}



StatusCode HIEventShapeFillerTool::RecordCollection(const std::string &event_shape_key, const std::string &event_shapeaux_key)
{
   ATH_MSG_DEBUG("INSIDE RecordCollection"    );
   HIEventShapeFillerTool_MSG_DEBUG("%s\n","HIEventShapeFillerTool::RecordCollection");

   //record in storegate
   CHECK(evtStore()->record(evtShape   ,event_shape_key   ));
   CHECK(evtStore()->record(evtShapeAux,event_shapeaux_key));

   return StatusCode::SUCCESS;
}



StatusCode HIEventShapeFillerTool::UpdateWithCell(const CaloCell* theCell, float geoWeight)     
{
   ATH_MSG_DEBUG("INSIDE UpdateWithCell"     );
   //HIEventShapeFillerTool_MSG_DEBUG("%s\n","HIEventShapeFillerTool::UpdateWithCell");

   float eta     = theCell->eta();
   float phi     = theCell->phi();
   int   layer   = theCell->caloDDE()->getSampling();
   float cell_et_geoWeight = theCell->et()*geoWeight;

   int eta_bin   = HICaloHelper::FindBinEta(eta); 
   int index     = eta_bin*N_CALO_LAYERS + layer;
   xAOD::HIEventShape *sh=evtShape->at(index);

   int ncells    = sh->nCells();
   float etsum   = sh->Et();

   sh->setNcells(ncells+1);
   sh->setEt(etsum + cell_et_geoWeight);

   for(int i=0; i<N_ORDERS; i++){
      //std::vector<float> &c1= (std::vector<float>&) (sh->Reference_Etsum_cos());
      //std::vector<float> &s1= (std::vector<float>&) (sh->Reference_Etsum_sin());
      std::vector<float> &c1= sh->Et_cos();
      std::vector<float> &s1= sh->Et_sin();

      float tmp_cos = c1.at(i);
      float tmp_sin = s1.at(i);

      c1.at(i) = tmp_cos + cell_et_geoWeight*cos((i+1)*phi);
      s1.at(i) = tmp_sin + cell_et_geoWeight*sin((i+1)*phi);

      if(sh->Et_cos().at(i) != c1.at(i) )       ATH_MSG_ERROR(tmp_cos<<"  "<<sh->Et_cos().at(i)<<"  "<<c1.at(i));
      else  HIEventShapeFillerTool_MSG_DEBUG("ALL OK %f %f %f\n",tmp_cos       ,sh->Et_cos().at(i)       ,c1.at(i));

   }
   return StatusCode::SUCCESS;
} 
