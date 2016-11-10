/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETGoodness/MetGoodiesFiller.h"
#include "MissingETGoodness/EtmissGoodness.h"
#include "MissingETEvent/MissingET.h"

MET::Goodies& MetGoodiesFiller::s_goodies(MET::Goodies::instance());

MetGoodiesFiller::MetGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
{  
   //   declareProperty( "NameSecondaryMet", m_SecondaryMet = "" );
}

MetGoodiesFiller::~MetGoodiesFiller(){ 
} 

void MetGoodiesFiller::initGoodiesMap()
{
   ATH_MSG_DEBUG ("execute initGoodiesMap()");
}

StatusCode MetGoodiesFiller::initialize() {
   ATH_MSG_DEBUG ("initialize()");

   return StatusCode::SUCCESS;
}


StatusCode MetGoodiesFiller::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_DEBUG ("execute MetGoodiesFiller()");
   
   initGoodiesMap();
   //MET::EtmissGoodness mGood;   
   //MET::EtmissGoodnessManager& manager = MET::EtmissGoodnessManager::instance();

   //std::string metName(manager.GetMETKey()); CRASHES HERE
   std::string metNameSelected("MET_Topo"); //REMOVE HACK
    
   const MissingET* METSelected = 0;
   if (evtStore()->contains<MissingET>(metNameSelected)) {
      sc = evtStore()->retrieve( METSelected, metNameSelected );
      if ( sc.isFailure() ) {
	 ATH_MSG_WARNING("Couldn't retrieve MET collection "<<metNameSelected);
      } else {
	 s_goodies.setValue("EtoverRootSigmaEt", 
			  (double)sqrt((METSelected->etx()*METSelected->etx()+METSelected->ety()*METSelected->ety())
                                       / METSelected->sumet()));	 
      }
   }

   // Calculate and store MET difference between Track and Topo -  vector difference magnitude
   
   std::string metNameTopo("MET_Topo");
   std::string metNameTrack("MET_Track"); 
   const MissingET* METTopo = 0;
   const MissingET* METTrack = 0;
//   s_goodies.setValue("DeltaTrackTopo", -999); // set default value 
   StatusCode scA=StatusCode::FAILURE;
   StatusCode scB=StatusCode::FAILURE;
   if (evtStore()->contains<MissingET>(metNameTopo)) scA = evtStore()->retrieve( METTopo, metNameTopo );
   if (evtStore()->contains<MissingET>(metNameTrack)) scB = evtStore()->retrieve( METTrack, metNameTrack );
   if ( scA.isFailure() || scB.isFailure() || !METTopo || !METTrack ) {
      if ( scA.isFailure() || !METTopo ) ATH_MSG_WARNING("Couldn't retrieve MET collection "<<metNameTopo);
      if ( scB.isFailure() || !METTrack ) ATH_MSG_WARNING("Couldn't retrieve MET collection "<<metNameTrack);
      sc=StatusCode::SUCCESS;
      s_goodies.setValue("DeltaTrackTopo", -999); 
   }else{
      double Dx=(double)METTopo->etx()-(double)METTrack->etx();
      double Dy=(double)METTopo->ety()-(double)METTrack->ety();
      double D=sqrt( Dx*Dx + Dy*Dy); 
      s_goodies.setValue("DeltaTrackTopo", D); 
      if (!sc.isFailure()) sc=StatusCode::SUCCESS;
   }
   
   ATH_MSG_DEBUG ("execute() successful");
   return sc;
}


StatusCode MetGoodiesFiller::finalize() 
{
   ATH_MSG_INFO ("finalize()");
   return StatusCode::SUCCESS;
}
