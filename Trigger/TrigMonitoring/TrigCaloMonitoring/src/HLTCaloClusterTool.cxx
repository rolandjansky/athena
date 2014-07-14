/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloClusterTool.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "StoreGate/StoreGate.h" 
#include "StoreGate/DataHandle.h" 
#include "egammaEvent/EMShower.h"
#include "egammaEvent/PhotonContainer.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <string>

HLTCaloClusterTool::HLTCaloClusterTool(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent){
   declareProperty ("DoNtuple", m_ntuple = false);
   declareProperty("CaloOfflineClusterContainer", m_CaloClusterOfflineContainerName = "PhotonCollection");
   declareProperty("CaloEFClusterContainer", m_CaloClusterEFContainerName = "HLT_egamma_Photons");
   declareProperty("CaloL2ClusterContainer", m_CaloClusterL2ContainerName = "HLT_TrigT2CaloEgamma");
   declareProperty("MatchClusterL2EtCut", m_L2EtCut= 10000);
   declareProperty("MatchClusterL2dRCut", m_L2dRCut= 0.0375);
   declareProperty("MatchClusterEFEtCut", m_EFEtCut= 10000);
   declareProperty("MatchClusterEFdRCut", m_EFdRCut= 0.0375);

   return;
}

HLTCaloClusterTool::~HLTCaloClusterTool() {
}

StatusCode HLTCaloClusterTool::init() {
   (*m_log) << MSG::DEBUG << "Initializing" << endreq;
   return StatusCode::SUCCESS;

}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloClusterTool::book() {
#else
StatusCode HLTCaloClusterTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

   std::string EFPath= "HLT/CaloClusterMon/SlwEF";
   std::string L2Path= "HLT/CaloClusterMon/L2EM";
   std::string MainPath= "HLT/CaloClusterMon";


   addMonGroup( new MonGroup(this,EFPath.c_str(),run) );
   addMonGroup( new MonGroup(this,L2Path.c_str(),run) );
   addMonGroup( new MonGroup(this,MainPath.c_str(),run) );

   setCurrentMonGroup(MainPath);
   if ( newRun ) {
      std::vector<std::string> clustertypes;
      clustertypes.push_back("EgammaL2");	
      clustertypes.push_back("EgammaEF");
      addHistogram(new TH1F("NOffCluster","Number of Offline Clusters;Number of Clusters",20,0,20) );
      for(std::vector<std::string>::iterator ii=clustertypes.begin();ii!=clustertypes.end(); ++ii){	

         if((*ii).compare("EgammaL2")==0) setCurrentMonGroup(L2Path);
         else if((*ii).compare("EgammaEF")==0) setCurrentMonGroup(EFPath);
         else setCurrentMonGroup(MainPath);

         addHistogram(new TH1F(("NOnCluster"+(*ii)).c_str(),("Number of "+(*ii)+" Clusters;Number of Clusters").c_str(),20,0,20) );

         addHistogram(new TH1F(("DEtaUnmatchedClusters"+(*ii)).c_str(),("\\Delta\\eta between offline and all "+(*ii)+" cluster ;\\Delta\\eta;Number of Entries ").c_str(),100,0,5) );
         addHistogram(new TH1F(("DEtaMatchedClusters"+(*ii)).c_str(),("\\Delta\\eta between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut and \\Delta R<0.0375 ;\\Delta\\eta;Number of Entries").c_str(),100,0,0.7) );
         addHistogram(new TH1F(("DEtaCloseClusters"+(*ii)).c_str(),("\\Delta\\eta between offline and closest "+(*ii)+" cluster with E_{T}(Offline);\\Delta\\eta;Number of Entries").c_str(),100,0,5) );
         addHistogram(new TH1F(("DEtaClosePtCutClusters"+(*ii)).c_str(),("\\Delta\\eta between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut;\\Delta\\eta;Number of Entries").c_str(),100,0,5) );

         addHistogram(new TH1F(("DPhiUnmatchedClusters"+(*ii)).c_str(),("\\Delta\\phi between offline and all "+(*ii)+" cluster;\\Delta\\phi;Number of Entries ").c_str(),60,0,4) );
         addHistogram(new TH1F(("DPhiMatchedClusters"+(*ii)).c_str(),("\\Delta\\phi between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut and \\Delta R<0.0375 ;\\Delta\\phi;Number of Entries ").c_str(),100,0,0.7) );
         addHistogram(new TH1F(("DPhiCloseClusters"+(*ii)).c_str(),("\\Delta\\phi between offline and closest "+(*ii)+" cluster with E_{T}(Offline);\\Delta\\phi;Number of Entries ").c_str(),60,0,4) );
         addHistogram(new TH1F(("DPhiClosePtCutClusters"+(*ii)).c_str(),("\\Delta\\phi between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut;\\Delta\\phi;Number of Entries ").c_str(),60,0,4) );

         addHistogram(new TH1F(("ResoUnmatchedClusters"+(*ii)).c_str(),("The Resolution between offline and all "+(*ii)+" cluster;(E_{T}("+(*ii)+")-E_{T}(Offline))/E_{T}(Offline) (%) ; Number of Entries ").c_str(),100,-10,10) );
         addHistogram(new TH1F(("ResoMatchedClusters"+(*ii)).c_str(),("The Resolution between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut and \\Delta R<0.0375;(E_{T}("+(*ii)+")-E_{T}(Offline))/E_{T}(Offline) (%); Number of Entries ").c_str(),100,-10,10) );
         addHistogram(new TH1F(("ResoCloseClusters"+(*ii)).c_str(),("The Resolution between offline and closest "+(*ii)+" cluster with E_{T}(Offline);(E_{T}("+(*ii)+")-E_{T}(Offline))/E_{T}(Offline) (%) ; Number of Entries ").c_str(),100,-10,10) );
         addHistogram(new TH1F(("ResoClosePtCutClusters"+(*ii)).c_str(),("The Resolution between offline and closest "+(*ii)+" cluster with E_{T}(Offline) cut;(E_{T}("+(*ii)+")-E_{T}(Offline))/E_{T}(Offline)} (%) ; Number of Entries ").c_str(),100,-10,10) );

         addHistogram(new TH2F(("EtEtUnmatched"+(*ii)).c_str(),("Comparison between offline versus all "+(*ii)+" cluster's E_{T}; E_{T}("+(*ii)+") MeV; E_{T}(Offline) MeV").c_str(),100,0,100000,100,0,100000) );
         addHistogram(new TH2F(("EtEtMatched"+(*ii)).c_str(),("Comparison between offline (E_{T} cut) versus closest "+(*ii)+" cluster's E_{T} with \\Delta R<0.0375; E_{T}("+(*ii)+") MeV; E_{T}(Offline) MeV").c_str(),100,0,100000,100,0,100000) );
         addHistogram(new TH2F(("EtEtClose"+(*ii)).c_str(),("Comparison between offline versus closest "+(*ii)+" cluster's E_{T}; E_{T}("+(*ii)+") MeV; E_{T}(Offline) MeV").c_str(),100,0,100000,100,0,100000) );
         addHistogram(new TH2F(("EtEtClosePtCut"+(*ii)).c_str(),("Comparison between offline (E_{T} cut) versus closest "+(*ii)+" cluster's E_{T}; E_{T}("+(*ii)+") MeV; E_{T}(Offline) MeV").c_str(),100,0,100000,100,0,100000) );

         addHistogram(new TH2F(("EtFracEtaUnmatched"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\eta for all clusters; \\eta; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3,3,100,0,1.4) );
         addHistogram(new TH2F(("EtFracEtaMatched"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\eta with E_{T}(Offline) cut and \\delta R<0.0375;\\eta ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3,3,100,0,1.4) );
         addHistogram(new TH2F(("EtFracEtaClose"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\eta between closest clusters;\\eta ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3,3,100,0,1.4) );
         addHistogram(new TH2F(("EtFracEtaCloseEtCut"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\eta between closest clusters with E_{T}(Offline) cut;\\eta ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3,3,100,0,1.4) );

         addHistogram(new TH2F(("EtFracPhiUnmatched"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\phi for all clusters;\\phi ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3.5,3.5,100,0,1.4) );
         addHistogram(new TH2F(("EtFracPhiMatched"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\phi with E_{T}(Offline) cut and \\delta R<0.0375;\\phi ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3.5,3.5,100,0,1.4) );
         addHistogram(new TH2F(("EtFracPhiClose"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\phi between closest clusters;\\phi ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3.5,3.5,100,0,1.4) );
         addHistogram(new TH2F(("EtFracPhiCloseEtCut"+(*ii)).c_str(),("#frac{E_{T}("+(*ii)+")}{E_{T}(Offline)} versus \\phi between closest clusters with E_{T}(Offline) cut;\\phi ; #frac{E_{T}("+(*ii)+")}{E_{T}(Offline)}").c_str(),100,-3.5,3.5,100,0,1.4) );

         addHistogram(new TH2F(("etaphiDifferent"+(*ii)).c_str(),("This is the etaphi of clusters ("+(*ii)+") with important differences; \\eta; \\phi").c_str(),100,-5.,5.,64,-3.2,3.2) );

         addHistogram(new TH1F(("RetaOnline"+(*ii)).c_str(),("R\\eta of the Online("+(*ii)+") cluster;R\\eta;Number of Entries").c_str(),40,-0.5,1.5));
         addHistogram(new TH1F(("RstripOnline"+(*ii)).c_str(),("Rstrip of the Online("+(*ii)+") cluster;Rstrip;Number of Entries").c_str(),40,-0.5,1.5));
         addHistogram(new TH1F(("RetaOffline"+(*ii)).c_str(),"R\\eta of the Offline cluster;R\\eta;Number of Entries",40,-0.5,1.5));
         addHistogram(new TH1F(("RstripOffline"+(*ii)).c_str(),"Rstrip of the Offline cluster;Rstrip;Number of Entries",40,-0.5,1.5));

         addHistogram(new TH2F(("RetaOnOff"+(*ii)).c_str(),("R\\eta of the Online("+(*ii)+") cluster vs the R\\eta of the Offline cluster;R\\eta (ON);R\\eta(OFF)").c_str(),40,-0.5,1.5,40,-0.5,1.5));
         addHistogram(new TH2F(("RstripOnOff"+(*ii)).c_str(),("Rstrip of the Online("+(*ii)+") cluster vs the Rstrip of the Offline cluster;Rstrip (ON);Rstrip (OFF)").c_str(),40,-0.5,1.5,40,-0.5,1.5));
      }//end of loop over different containers


      if ( m_ntuple ) 
         addTree( new TNtuple("Details","Details","et:eta:phi:tet:teta:tphi") );

   }else if ( newEventsBlock || newLumiBlock ){
     return StatusCode::SUCCESS;
   }
   msg(MSG::DEBUG)<< "End of book" << endreq;
   return StatusCode::SUCCESS;
}

StatusCode HLTCaloClusterTool::fill() {

   int msglevel = (*m_log).level();
   std::vector<std::string> clustertypes;
   clustertypes.push_back("EgammaL2");
   clustertypes.push_back("EgammaEF");	
   std::string EFPath= "HLT/CaloClusterMon/SlwEF";
   std::string L2Path= "HLT/CaloClusterMon/L2EM";
   std::string MainPath= "HLT/CaloClusterMon";
   setCurrentMonGroup(MainPath);

   //Offline
   const PhotonContainer* caloOFF = 0;
   if( (evtStore()->retrieve(caloOFF, m_CaloClusterOfflineContainerName)).isFailure() ) {
      msg(MSG::DEBUG)<< "No Offline Calo Cluster Container found"<< endreq;
      return StatusCode::SUCCESS;
   }
   PhotonContainer::const_iterator OFFbeg;
   PhotonContainer::const_iterator OFFend;

   if(caloOFF->size()!=0) msg(MSG::DEBUG)<< "Got Offline Cluster Container Size:  "<<caloOFF->size()<< endreq;


   hist("NOffCluster")->Fill(caloOFF->size());

   //Loop over Cluster Types
   for(std::vector<std::string>::iterator ii=clustertypes.begin();ii!=clustertypes.end(); ++ii){	

      if((*ii).compare("EgammaL2")==0) setCurrentMonGroup(L2Path);
      else if((*ii).compare("EgammaEF")==0) setCurrentMonGroup(EFPath);
      else setCurrentMonGroup(MainPath);

      // looking at the egamma
      if((*ii).compare("EgammaL2")==0){
         OFFbeg = caloOFF->begin();
         OFFend = caloOFF->end();

         const TrigEMClusterContainer* EMColl = 0;
         if( (evtStore()->retrieve(EMColl, m_CaloClusterL2ContainerName)).isFailure() ) {
            msg(MSG::DEBUG)<< "No TrigT2CaloEgamma Cluster Container found"<< endreq;
            return StatusCode::SUCCESS;
         }


         hist(("NOnCluster"+(*ii)).c_str())->Fill(EMColl->size());
         if ( EMColl->size()==0 || caloOFF->size()==0 ){
            msg(MSG::DEBUG)<< "Not possible to perform comparison" << endreq;
            continue;
         }



         TrigEMClusterContainer::const_iterator ONEMbeg= EMColl->begin();
         TrigEMClusterContainer::const_iterator ONEMend = EMColl->end();
         TrigEMClusterContainer::const_iterator MatchEM;
         for(;OFFbeg!=OFFend;++OFFbeg){
            ONEMbeg = EMColl->begin();
            MatchEM= EMColl->begin();
            float deltaRMatchEM=999;
            for(;ONEMbeg!=ONEMend;++ONEMbeg){
               float deltaRTemp = deltaR((*ONEMbeg)->phi(),(*OFFbeg)->cluster()->phi(),(*ONEMbeg)->eta(),(*OFFbeg)->cluster()->eta()); 
               if(deltaRTemp<deltaRMatchEM) {
                  deltaRMatchEM = deltaRTemp;
                  MatchEM=ONEMbeg;
               }
               hist(("DEtaUnmatchedClusters"+(*ii)).c_str())->Fill(fabsf((*ONEMbeg)->eta()-(*OFFbeg)->cluster()->eta()));	
               hist(("DPhiUnmatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*ONEMbeg)->phi(),(*OFFbeg)->cluster()->phi()));	
               hist2(("EtEtUnmatched"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoUnmatchedClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
               }
            }
            hist(("DEtaCloseClusters"+(*ii)).c_str())->Fill(fabsf((*MatchEM)->eta()-(*OFFbeg)->cluster()->eta()));
            hist(("DPhiCloseClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->cluster()->phi()));
            hist2(("EtEtClose"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->cluster()->et());
            if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
               hist(("ResoCloseClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
               hist2(("EtFracEtaClose"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
               hist2(("EtFracPhiClose"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
            }
            if((*OFFbeg)->cluster()->et()>m_L2EtCut){
               if (deltaPhi((*MatchEM)->phi(),(*OFFbeg)->cluster()->phi())>3.1){
                  if ( msglevel <= MSG::DEBUG ){
                     msg(MSG::DEBUG)<< "The Online Cluster Et:  "<< (*MatchEM)->et()  << "  eta:  " << (*MatchEM)->eta()  <<  "  phi:  " << (*MatchEM)->phi() << endreq;	
                     msg(MSG::DEBUG)<< "The Offline Cluster Et:  "<< (*OFFbeg)->cluster()->et()  << "  eta:  " << (*OFFbeg)->cluster()->eta()  <<  "  phi:  " << (*OFFbeg)->cluster()->phi() << endreq;	
                     msg(MSG::DEBUG)<< "Offline Cluster Container Size:  "<<caloOFF->size()<< "Online Cluster Container Size:  "<<EMColl->size()<< endreq;
                  }
               }


               hist(("DEtaClosePtCutClusters"+(*ii)).c_str())->Fill(fabsf((*MatchEM)->eta()-(*OFFbeg)->cluster()->eta()));
               hist(("DPhiClosePtCutClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->cluster()->phi()));
               hist2(("EtEtClosePtCut"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoClosePtCutClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
               }
            }	
            if((*OFFbeg)->cluster()->et()>m_L2EtCut&&deltaR((*MatchEM)->phi(),(*OFFbeg)->cluster()->phi(),(*MatchEM)->eta(),(*OFFbeg)->cluster()->eta())<m_L2dRCut){
               const EMShower* eshow = (*OFFbeg)->detail<EMShower>("egDetailContainer");
               if(eshow!=NULL){
                  float RstripOff=(eshow->parameter(egammaParameters::emaxs1)+eshow->parameter(egammaParameters::e2ts1));
                  if (RstripOff>0.2)
                     RstripOff=(eshow->parameter(egammaParameters::emaxs1)-eshow->parameter(egammaParameters::e2ts1))/RstripOff;
                  else RstripOff=-1.0;
                  float RetaOff=-1.0;
                  if ( eshow->parameter(egammaParameters::e277) >0.2)
                     RetaOff=eshow->parameter(egammaParameters::e237)/eshow->parameter(egammaParameters::e277);
                  float RstripOn=((*MatchEM)->emaxs1()+(*MatchEM)->e2tsts1());
                  if ( RstripOn>0.2) RstripOn=((*MatchEM)->emaxs1()-(*MatchEM)->e2tsts1())/RstripOn;
                  else RstripOn=-1.0;
                  float RetaOn=-1.0;
                  if ( (*MatchEM)->e277()>0.2) RetaOn=(*MatchEM)->e237()/(*MatchEM)->e277();
                  hist(("RetaOnline"+(*ii)).c_str())->Fill(RetaOn);
                  hist(("RstripOnline"+(*ii)).c_str())->Fill(RstripOn);
                  hist(("RetaOffline"+(*ii)).c_str())->Fill(RetaOff);
                  hist(("RstripOffline"+(*ii)).c_str())->Fill(RstripOff);

                  hist2(("RetaOnOff"+(*ii)).c_str())->Fill(RetaOn,RetaOff);
                  hist2(("RstripOnOff"+(*ii)).c_str())->Fill(RstripOn,RstripOff);

                  msg(MSG::DEBUG)<< "RetaOnline:  "<<RetaOn<< endreq;
                  msg(MSG::DEBUG)<< "RstripOnline:  "<<RstripOn<< endreq;
                  msg(MSG::DEBUG)<< "RetaOffline:  "<<RetaOff<< endreq;
                  msg(MSG::DEBUG)<< "RstripOffline:  "<<RstripOff<< endreq;
               }
               if((*MatchEM)->et()/(*OFFbeg)->cluster()->et()<0.7||(*MatchEM)->et()/(*OFFbeg)->cluster()->et()>1.3) hist2(("etaphiDifferent"+(*ii)).c_str())->Fill((*MatchEM)->eta(),(*MatchEM)->phi());
               if (m_ntuple) ((TNtuple*)tree("Details"))->Fill((*MatchEM)->et(),(*MatchEM)->eta(),(*MatchEM)->phi(),(*OFFbeg)->cluster()->et(),(*OFFbeg)->cluster()->eta(),(*OFFbeg)->cluster()->phi() ); 
               hist(("DEtaMatchedClusters"+(*ii)).c_str())->Fill(fabsf((*MatchEM)->eta()-(*OFFbeg)->cluster()->eta()));
               hist(("DPhiMatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->cluster()->phi()));
               hist2(("EtEtMatched"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoMatchedClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaMatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiMatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->cluster()->et());
               }
            }
         }




      }

      /////////////////Cal Jets //////////////
      if((*ii).compare("EgammaEF")==0){
         const egammaContainer* caloON = 0;
         if( (evtStore()->retrieve(caloON, m_CaloClusterEFContainerName)).isFailure()) {
            msg(MSG::DEBUG)<< "No Online Calo Cluster Container found"<< endreq;
            return StatusCode::SUCCESS;
         }





         hist(("NOnCluster"+(*ii)).c_str())->Fill(caloON->size());

         if ( caloON->size()==0 || caloOFF->size()==0 ){
            msg(MSG::DEBUG)<< "Not possible to perform comparison" << endreq;
            continue;
         }
         egammaContainer::const_iterator ONbeg = caloON->begin();
         egammaContainer::const_iterator ONend = caloON->end();
         OFFbeg = caloOFF->begin();
         OFFend = caloOFF->end();
         egammaContainer::const_iterator Match;



         for(;OFFbeg!=OFFend;++OFFbeg){
            ONbeg = caloON->begin();
            Match= caloON->begin();
            float deltaRMatch=999;
            for(;ONbeg!=ONend;++ONbeg){

               float deltaRTemp = deltaR((*ONbeg)->cluster()->phi(),(*OFFbeg)->cluster()->phi(),(*ONbeg)->cluster()->eta(),(*OFFbeg)->cluster()->eta()); 
               if(deltaRTemp<deltaRMatch) {
                  deltaRMatch = deltaRTemp;
                  Match=ONbeg;
               }
               hist(("DEtaUnmatchedClusters"+(*ii)).c_str())->Fill(fabsf((*ONbeg)->cluster()->eta()-(*OFFbeg)->cluster()->eta()));	
               hist(("DPhiUnmatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*ONbeg)->cluster()->phi(),(*OFFbeg)->cluster()->phi()));	
               hist2(("EtEtUnmatched"+(*ii)).c_str())->Fill((*Match)->cluster()->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoUnmatchedClusters"+(*ii)).c_str())->Fill(100*((*Match)->cluster()->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
               }
            }
            hist(("DEtaCloseClusters"+(*ii)).c_str())->Fill(fabsf((*Match)->cluster()->eta()-(*OFFbeg)->cluster()->eta()));
            hist(("DPhiCloseClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->cluster()->phi(),(*OFFbeg)->cluster()->phi()));
            hist2(("EtEtClose"+(*ii)).c_str())->Fill((*Match)->cluster()->et(),(*OFFbeg)->cluster()->et());
            if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
               hist(("ResoCloseClusters"+(*ii)).c_str())->Fill(100*((*Match)->cluster()->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
               hist2(("EtFracEtaClose"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
               hist2(("EtFracPhiClose"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
            }
            if((*OFFbeg)->cluster()->et()>m_EFEtCut){
               if (deltaPhi((*Match)->cluster()->phi(),(*OFFbeg)->cluster()->phi())>3.1){
                  if ( msglevel <= MSG::DEBUG ){
                     msg(MSG::DEBUG)<< "The Online Cluster Et:  "<< (*Match)->cluster()->et()  << "  eta:  " << (*Match)->cluster()->eta()  <<  "  phi:  " << (*Match)->cluster()->phi() << endreq;	
                     msg(MSG::DEBUG)<< "The Offline Cluster Et:  "<< (*OFFbeg)->cluster()->et()  << "  eta:  " << (*OFFbeg)->cluster()->eta()  <<  "  phi:  " << (*OFFbeg)->cluster()->phi() << endreq;	
                     msg(MSG::DEBUG)<< "Offline Cluster Container Size:  "<<caloOFF->size()<< "Online Cluster Container Size:  "<<caloON->size()<< endreq;
                  }
               }


               hist(("DEtaClosePtCutClusters"+(*ii)).c_str())->Fill(fabsf((*Match)->cluster()->eta()-(*OFFbeg)->cluster()->eta()));
               hist(("DPhiClosePtCutClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->cluster()->phi(),(*OFFbeg)->cluster()->phi()));
               hist2(("EtEtClosePtCut"+(*ii)).c_str())->Fill((*Match)->cluster()->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoClosePtCutClusters"+(*ii)).c_str())->Fill(100*((*Match)->cluster()->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
               }
            }	


            if((*OFFbeg)->cluster()->et()>m_EFEtCut&&deltaR((*Match)->cluster()->phi(),(*OFFbeg)->cluster()->phi(),(*Match)->cluster()->eta(),(*OFFbeg)->cluster()->eta())<m_EFdRCut){
               const EMShower* eshow = (*OFFbeg)->detail<EMShower>("egDetailContainer");
               if (eshow) {
                  // std::cout << "About to print out Offline shower" << std::endl;
                  // eshow->print();
               } else ATH_MSG_WARNING("offline object has no shower container");
               ATH_MSG_DEBUG("offline with author " << (*OFFbeg)->author() << " has number of details: " << (*OFFbeg)->nDetails());
               for (int i = 0; i < (*OFFbeg)->nDetails(); i++) {
                  ATH_MSG_DEBUG("offline detail: " << (*OFFbeg)->detailName(i));
               }
               ATH_MSG_DEBUG("");

               const EMShower* EFeshow = (*Match)->detail<EMShower>();
               if (EFeshow) {
                  //EFeshow->print();
               } else ATH_MSG_WARNING("egamma object has no shower container");
               ATH_MSG_DEBUG("egamma with author " << (*Match)->author() << " has number of details: " << (*Match)->nDetails());
               for (int i = 0; i < (*Match)->nDetails(); i++) {
                  ATH_MSG_DEBUG("egamma detail: " << (*Match)->detailName(i));
               }
               ATH_MSG_DEBUG("");

               if(eshow!=NULL&&EFeshow!=NULL){
                  float RstripOff=(eshow->parameter(egammaParameters::emaxs1)+eshow->parameter(egammaParameters::e2ts1));
                  if(RstripOff>0.2) RstripOff=(eshow->parameter(egammaParameters::emaxs1)-eshow->parameter(egammaParameters::e2ts1))/RstripOff;
                  else RstripOff=-1.0;
                  float RetaOff=-1.0;
                  if ( eshow->parameter(egammaParameters::e277)>0.2 ) RetaOff = eshow->parameter(egammaParameters::e237)/eshow->parameter(egammaParameters::e277);
                  float RstripOn=(EFeshow->parameter(egammaParameters::emaxs1)+EFeshow->parameter(egammaParameters::e2ts1));
                  if(RstripOn>0.2) RstripOn=(EFeshow->parameter(egammaParameters::emaxs1)-EFeshow->parameter(egammaParameters::e2ts1))/RstripOn;
                  float RetaOn=-1.0;
                  if ( EFeshow->parameter(egammaParameters::e277)>0.2) RetaOn = EFeshow->parameter(egammaParameters::e237)/EFeshow->parameter(egammaParameters::e277);

                  hist(("RetaOnline"+(*ii)).c_str())->Fill(RetaOn);
                  hist(("RstripOnline"+(*ii)).c_str())->Fill(RstripOn);
                  hist(("RetaOffline"+(*ii)).c_str())->Fill(RetaOff);
                  hist(("RstripOffline"+(*ii)).c_str())->Fill(RstripOff);

                  hist2(("RetaOnOff"+(*ii)).c_str())->Fill(RetaOn,RetaOff);
                  hist2(("RstripOnOff"+(*ii)).c_str())->Fill(RstripOn,RstripOff);

                  msg(MSG::DEBUG)<< "RetaOnline:  "<<RetaOn<< endreq;
                  msg(MSG::DEBUG)<< "RstripOnline:  "<<RstripOn<< endreq;
                  msg(MSG::DEBUG)<< "RetaOffline:  "<<RetaOff<< endreq;
                  msg(MSG::DEBUG)<< "RstripOffline:  "<<RstripOff<< endreq;

               }
               if((*Match)->cluster()->et()/(*OFFbeg)->cluster()->et()<0.7||(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et()>1.3) hist2(("etaphiDifferent"+(*ii)).c_str())->Fill((*Match)->cluster()->eta(),(*Match)->cluster()->phi());
               if (m_ntuple) ((TNtuple*)tree("Details"))->Fill((*Match)->cluster()->et(),(*Match)->cluster()->eta(),(*Match)->cluster()->phi(),(*OFFbeg)->cluster()->et(),(*OFFbeg)->cluster()->eta(),(*OFFbeg)->cluster()->phi() ); 
               hist(("DEtaMatchedClusters"+(*ii)).c_str())->Fill(fabsf((*Match)->cluster()->eta()-(*OFFbeg)->cluster()->eta()));
               hist(("DPhiMatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->cluster()->phi(),(*OFFbeg)->cluster()->phi()));
               hist2(("EtEtMatched"+(*ii)).c_str())->Fill((*Match)->cluster()->et(),(*OFFbeg)->cluster()->et());
               if ( fabs((*OFFbeg)->cluster()->et())>0.2 ){
                  hist(("ResoMatchedClusters"+(*ii)).c_str())->Fill(100*((*Match)->cluster()->et()-(*OFFbeg)->cluster()->et())/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracEtaMatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->eta(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
                  hist2(("EtFracPhiMatched"+(*ii)).c_str())->Fill((*OFFbeg)->cluster()->phi(),(*Match)->cluster()->et()/(*OFFbeg)->cluster()->et());
               }
            }
         }
      }

   }

   // Ok, reached the method end
   return StatusCode::SUCCESS;
}

