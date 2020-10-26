/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloClusterTool.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/PhotonContainer.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <string>

HLTCaloClusterTool::HLTCaloClusterTool(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent){
   declareProperty ("DoNtuple", m_ntuple = false);
   declareProperty("CaloOfflineClusterContainer", m_CaloClusterOfflineContainerName = "Photons");
   declareProperty("CaloEFClusterContainer", m_CaloClusterEFContainerName = "HLT_xAOD__PhotonContainer_egamma_Photons");
   declareProperty("CaloL2ClusterContainer", m_CaloClusterL2ContainerName = "HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma");
   declareProperty("MatchClusterL2EtCut", m_L2EtCut= 10000);
   declareProperty("MatchClusterL2dRCut", m_L2dRCut= 0.0375);
   declareProperty("MatchClusterEFEtCut", m_EFEtCut= 10000);
   declareProperty("MatchClusterEFdRCut", m_EFdRCut= 0.0375);

   return;
}

HLTCaloClusterTool::~HLTCaloClusterTool() {
}

StatusCode HLTCaloClusterTool::init() {
   ATH_MSG_DEBUG( "Initializing"  );
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
   if ( newRunFlag() ) {
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

   }else if ( newEventsBlockFlag() || newLumiBlockFlag() ){
     return StatusCode::SUCCESS;
   }
   ATH_MSG_DEBUG( "End of book"  );
   return StatusCode::SUCCESS;
}

StatusCode HLTCaloClusterTool::fill() {

   std::vector<std::string> clustertypes;
   clustertypes.push_back("EgammaL2");
   clustertypes.push_back("EgammaEF");	
   std::string EFPath= "HLT/CaloClusterMon/SlwEF";
   std::string L2Path= "HLT/CaloClusterMon/L2EM";
   std::string MainPath= "HLT/CaloClusterMon";
   setCurrentMonGroup(MainPath);

   //Offline
   const xAOD::PhotonContainer* caloOFF = 0;
   if( (evtStore()->retrieve(caloOFF, m_CaloClusterOfflineContainerName)).isFailure() ) {
      ATH_MSG_DEBUG( "No Offline Calo Cluster Container found" );
      return StatusCode::SUCCESS;
   }
   xAOD::PhotonContainer::const_iterator OFFbeg;
   xAOD::PhotonContainer::const_iterator OFFend;

   if(caloOFF->size()!=0) ATH_MSG_DEBUG( "Got Offline Cluster Container Size:  "<<caloOFF->size() );


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

         const xAOD::TrigEMClusterContainer* EMColl = 0;
         if( (evtStore()->retrieve(EMColl, m_CaloClusterL2ContainerName)).isFailure() ) {
            ATH_MSG_DEBUG( "No TrigT2CaloEgamma Cluster Container found" );
            return StatusCode::SUCCESS;
         }


         hist(("NOnCluster"+(*ii)).c_str())->Fill(EMColl->size());
         if ( EMColl->size()==0 || caloOFF->size()==0 ){
            ATH_MSG_DEBUG( "Not possible to perform comparison"  );
            continue;
         }



         xAOD::TrigEMClusterContainer::const_iterator ONEMbeg= EMColl->begin();
         xAOD::TrigEMClusterContainer::const_iterator ONEMend = EMColl->end();
         xAOD::TrigEMClusterContainer::const_iterator MatchEM;
         for(;OFFbeg!=OFFend;++OFFbeg){
            ONEMbeg = EMColl->begin();
            MatchEM= EMColl->begin();
            float deltaRMatchEM=999;
            for(;ONEMbeg!=ONEMend;++ONEMbeg){
               float deltaRTemp = deltaR((*ONEMbeg)->phi(),(*OFFbeg)->caloCluster()->phi(),(*ONEMbeg)->eta(),(*OFFbeg)->caloCluster()->eta()); 
               if(deltaRTemp<deltaRMatchEM) {
                  deltaRMatchEM = deltaRTemp;
                  MatchEM=ONEMbeg;
               }
               hist(("DEtaUnmatchedClusters"+(*ii)).c_str())->Fill(std::abs((*ONEMbeg)->eta()-(*OFFbeg)->caloCluster()->eta()));	
               hist(("DPhiUnmatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*ONEMbeg)->phi(),(*OFFbeg)->caloCluster()->phi()));	
               hist2(("EtEtUnmatched"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoUnmatchedClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
               }
            }
            hist(("DEtaCloseClusters"+(*ii)).c_str())->Fill(std::abs((*MatchEM)->eta()-(*OFFbeg)->caloCluster()->eta()));
            hist(("DPhiCloseClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->caloCluster()->phi()));
            hist2(("EtEtClose"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->caloCluster()->et());
            if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
               hist(("ResoCloseClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
               hist2(("EtFracEtaClose"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
               hist2(("EtFracPhiClose"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
            }
            if((*OFFbeg)->caloCluster()->et()>m_L2EtCut){
               if (deltaPhi((*MatchEM)->phi(),(*OFFbeg)->caloCluster()->phi())>3.1){
                 ATH_MSG_DEBUG( "The Online Cluster Et:  "<< (*MatchEM)->et()  << "  eta:  " << (*MatchEM)->eta()  <<  "  phi:  " << (*MatchEM)->phi()  );
                 ATH_MSG_DEBUG( "The Offline Cluster Et:  "<< (*OFFbeg)->caloCluster()->et()  << "  eta:  " << (*OFFbeg)->caloCluster()->eta()  <<  "  phi:  " << (*OFFbeg)->caloCluster()->phi()  );
                 ATH_MSG_DEBUG( "Offline Cluster Container Size:  "<<caloOFF->size()<< "Online Cluster Container Size:  "<<EMColl->size() );
               }


               hist(("DEtaClosePtCutClusters"+(*ii)).c_str())->Fill(std::abs((*MatchEM)->eta()-(*OFFbeg)->caloCluster()->eta()));
               hist(("DPhiClosePtCutClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->caloCluster()->phi()));
               hist2(("EtEtClosePtCut"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoClosePtCutClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
               }
            }
            if((*OFFbeg)->caloCluster()->et()>m_L2EtCut&&deltaR((*MatchEM)->phi(),(*OFFbeg)->caloCluster()->phi(),(*MatchEM)->eta(),(*OFFbeg)->caloCluster()->eta())<m_L2dRCut){
	      
               float OFFbeg_emaxs1, OFFbeg_e2ts1, OFFbeg_e277, OFFbeg_e237;
               bool  OFFbeg_shower = true;

               if(!(*OFFbeg)->showerShapeValue(OFFbeg_emaxs1,xAOD::EgammaParameters::emaxs1)) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e2ts1, xAOD::EgammaParameters::e2ts1 )) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e277,  xAOD::EgammaParameters::e277  )) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e237,  xAOD::EgammaParameters::e237  )) OFFbeg_shower = false;
               
               if(OFFbeg_shower){
                  float RstripOff=OFFbeg_emaxs1+OFFbeg_e2ts1;
                  if (RstripOff>0.2)
                     RstripOff=(OFFbeg_emaxs1-OFFbeg_e2ts1)/RstripOff;
                  else RstripOff=-1.0;
                  float RetaOff=-1.0;
                  if (OFFbeg_e277 >0.2)
                     RetaOff=OFFbeg_e237/OFFbeg_e277;
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

                  ATH_MSG_DEBUG( "RetaOnline:  "<<RetaOn );
                  ATH_MSG_DEBUG( "RstripOnline:  "<<RstripOn );
                  ATH_MSG_DEBUG( "RetaOffline:  "<<RetaOff );
                  ATH_MSG_DEBUG( "RstripOffline:  "<<RstripOff );
               }
               if((*MatchEM)->et()/(*OFFbeg)->caloCluster()->et()<0.7||(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et()>1.3) hist2(("etaphiDifferent"+(*ii)).c_str())->Fill((*MatchEM)->eta(),(*MatchEM)->phi());
               if (m_ntuple) ((TNtuple*)tree("Details"))->Fill((*MatchEM)->et(),(*MatchEM)->eta(),(*MatchEM)->phi(),(*OFFbeg)->caloCluster()->et(),(*OFFbeg)->caloCluster()->eta(),(*OFFbeg)->caloCluster()->phi() ); 
               hist(("DEtaMatchedClusters"+(*ii)).c_str())->Fill(std::abs((*MatchEM)->eta()-(*OFFbeg)->caloCluster()->eta()));
               hist(("DPhiMatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*MatchEM)->phi(),(*OFFbeg)->caloCluster()->phi()));
               hist2(("EtEtMatched"+(*ii)).c_str())->Fill((*MatchEM)->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoMatchedClusters"+(*ii)).c_str())->Fill(100*((*MatchEM)->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaMatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiMatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*MatchEM)->et()/(*OFFbeg)->caloCluster()->et());
               }
            }
         }




      }

      /////////////////Cal Jets //////////////
      if((*ii).compare("EgammaEF")==0){
         const xAOD::PhotonContainer* caloON = 0;
         if( (evtStore()->retrieve(caloON, m_CaloClusterEFContainerName)).isFailure()) {
            ATH_MSG_DEBUG( "No Online Calo Cluster Container found" );
            return StatusCode::SUCCESS;
         }





         hist(("NOnCluster"+(*ii)).c_str())->Fill(caloON->size());

         if ( caloON->size()==0 || caloOFF->size()==0 ){
            ATH_MSG_DEBUG( "Not possible to perform comparison"  );
            continue;
         }
         xAOD::PhotonContainer::const_iterator ONbeg = caloON->begin();
         xAOD::PhotonContainer::const_iterator ONend = caloON->end();
         OFFbeg = caloOFF->begin();
         OFFend = caloOFF->end();
         xAOD::PhotonContainer::const_iterator Match;



         for(;OFFbeg!=OFFend;++OFFbeg){
            ONbeg = caloON->begin();
            Match= caloON->begin();
            float deltaRMatch=999;
            for(;ONbeg!=ONend;++ONbeg){

               float deltaRTemp = deltaR((*ONbeg)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi(),(*ONbeg)->caloCluster()->eta(),(*OFFbeg)->caloCluster()->eta()); 
               if(deltaRTemp<deltaRMatch) {
                  deltaRMatch = deltaRTemp;
                  Match=ONbeg;
               }
               hist(("DEtaUnmatchedClusters"+(*ii)).c_str())->Fill(std::abs((*ONbeg)->caloCluster()->eta()-(*OFFbeg)->caloCluster()->eta()));	
               hist(("DPhiUnmatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*ONbeg)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi()));	
               hist2(("EtEtUnmatched"+(*ii)).c_str())->Fill((*Match)->caloCluster()->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoUnmatchedClusters"+(*ii)).c_str())->Fill(100*((*Match)->caloCluster()->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiUnmatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
               }
            }
            hist(("DEtaCloseClusters"+(*ii)).c_str())->Fill(std::abs((*Match)->caloCluster()->eta()-(*OFFbeg)->caloCluster()->eta()));
            hist(("DPhiCloseClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi()));
            hist2(("EtEtClose"+(*ii)).c_str())->Fill((*Match)->caloCluster()->et(),(*OFFbeg)->caloCluster()->et());
            if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
               hist(("ResoCloseClusters"+(*ii)).c_str())->Fill(100*((*Match)->caloCluster()->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
               hist2(("EtFracEtaClose"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
               hist2(("EtFracPhiClose"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
            }
            if((*OFFbeg)->caloCluster()->et()>m_EFEtCut){
               if (deltaPhi((*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi())>3.1){
                 ATH_MSG_DEBUG( "The Online Cluster Et:  "<< (*Match)->caloCluster()->et()  << "  eta:  " << (*Match)->caloCluster()->eta()  <<  "  phi:  " << (*Match)->caloCluster()->phi()  );
                 ATH_MSG_DEBUG( "The Offline Cluster Et:  "<< (*OFFbeg)->caloCluster()->et()  << "  eta:  " << (*OFFbeg)->caloCluster()->eta()  <<  "  phi:  " << (*OFFbeg)->caloCluster()->phi()  );
                 ATH_MSG_DEBUG( "Offline Cluster Container Size:  "<<caloOFF->size()<< "Online Cluster Container Size:  "<<caloON->size() );
               }


               hist(("DEtaClosePtCutClusters"+(*ii)).c_str())->Fill(std::abs((*Match)->caloCluster()->eta()-(*OFFbeg)->caloCluster()->eta()));
               hist(("DPhiClosePtCutClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi()));
               hist2(("EtEtClosePtCut"+(*ii)).c_str())->Fill((*Match)->caloCluster()->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoClosePtCutClusters"+(*ii)).c_str())->Fill(100*((*Match)->caloCluster()->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiCloseEtCut"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
               }
            }	


            if((*OFFbeg)->caloCluster()->et()>m_EFEtCut&&deltaR((*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi(),(*Match)->caloCluster()->eta(),(*OFFbeg)->caloCluster()->eta())<m_EFdRCut){
	      /*
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
	      */

               float OFFbeg_emaxs1, OFFbeg_e2ts1, OFFbeg_e277, OFFbeg_e237;
               bool  OFFbeg_shower = true;

               if(!(*OFFbeg)->showerShapeValue(OFFbeg_emaxs1,xAOD::EgammaParameters::emaxs1)) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e2ts1, xAOD::EgammaParameters::e2ts1 )) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e277,  xAOD::EgammaParameters::e277  )) OFFbeg_shower = false;
               if(!(*OFFbeg)->showerShapeValue(OFFbeg_e237,  xAOD::EgammaParameters::e237  )) OFFbeg_shower = false;

               float Match_emaxs1, Match_e2ts1, Match_e277, Match_e237;
               bool  Match_shower = true;

               if(!(*Match)->showerShapeValue(Match_emaxs1,xAOD::EgammaParameters::emaxs1)) Match_shower = false;
               if(!(*Match)->showerShapeValue(Match_e2ts1, xAOD::EgammaParameters::e2ts1 )) Match_shower = false;
               if(!(*Match)->showerShapeValue(Match_e277,  xAOD::EgammaParameters::e277  )) Match_shower = false;
               if(!(*Match)->showerShapeValue(Match_e237,  xAOD::EgammaParameters::e237  )) Match_shower = false;

               if(OFFbeg_shower&&Match_shower){
                  float RstripOff=OFFbeg_emaxs1+OFFbeg_e2ts1;
                  if(RstripOff>0.2) RstripOff=(OFFbeg_emaxs1-OFFbeg_e2ts1)/RstripOff;
                  else RstripOff=-1.0;
                  float RetaOff=-1.0;
                  if (OFFbeg_e277>0.2) RetaOff = OFFbeg_e237/OFFbeg_e277;
                  float RstripOn=Match_emaxs1+Match_e2ts1;
                  if(RstripOn>0.2) RstripOn=(Match_emaxs1-Match_e2ts1)/RstripOn;
                  float RetaOn=-1.0;
                  if ( Match_e277>0.2) RetaOn = Match_e237/Match_e277;

                  hist(("RetaOnline"+(*ii)).c_str())->Fill(RetaOn);
                  hist(("RstripOnline"+(*ii)).c_str())->Fill(RstripOn);
                  hist(("RetaOffline"+(*ii)).c_str())->Fill(RetaOff);
                  hist(("RstripOffline"+(*ii)).c_str())->Fill(RstripOff);

                  hist2(("RetaOnOff"+(*ii)).c_str())->Fill(RetaOn,RetaOff);
                  hist2(("RstripOnOff"+(*ii)).c_str())->Fill(RstripOn,RstripOff);

                  ATH_MSG_DEBUG( "RetaOnline:  "<<RetaOn );
                  ATH_MSG_DEBUG( "RstripOnline:  "<<RstripOn );
                  ATH_MSG_DEBUG( "RetaOffline:  "<<RetaOff );
                  ATH_MSG_DEBUG( "RstripOffline:  "<<RstripOff );

               }
               if((*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et()<0.7||(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et()>1.3) hist2(("etaphiDifferent"+(*ii)).c_str())->Fill((*Match)->caloCluster()->eta(),(*Match)->caloCluster()->phi());
               if (m_ntuple) ((TNtuple*)tree("Details"))->Fill((*Match)->caloCluster()->et(),(*Match)->caloCluster()->eta(),(*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->et(),(*OFFbeg)->caloCluster()->eta(),(*OFFbeg)->caloCluster()->phi() ); 
               hist(("DEtaMatchedClusters"+(*ii)).c_str())->Fill(std::abs((*Match)->caloCluster()->eta()-(*OFFbeg)->caloCluster()->eta()));
               hist(("DPhiMatchedClusters"+(*ii)).c_str())->Fill(deltaPhi((*Match)->caloCluster()->phi(),(*OFFbeg)->caloCluster()->phi()));
               hist2(("EtEtMatched"+(*ii)).c_str())->Fill((*Match)->caloCluster()->et(),(*OFFbeg)->caloCluster()->et());
               if ( fabs((*OFFbeg)->caloCluster()->et())>0.2 ){
                  hist(("ResoMatchedClusters"+(*ii)).c_str())->Fill(100*((*Match)->caloCluster()->et()-(*OFFbeg)->caloCluster()->et())/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracEtaMatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->eta(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
                  hist2(("EtFracPhiMatched"+(*ii)).c_str())->Fill((*OFFbeg)->caloCluster()->phi(),(*Match)->caloCluster()->et()/(*OFFbeg)->caloCluster()->et());
               }
            }
         }
      }

   }

   // Ok, reached the method end
   return StatusCode::SUCCESS;
}

