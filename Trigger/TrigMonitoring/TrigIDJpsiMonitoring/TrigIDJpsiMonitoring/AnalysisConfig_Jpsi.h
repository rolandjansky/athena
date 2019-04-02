// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file AnalysisConfig_Jpsi.h */
/** @author Mark Sutton (sutt@cern.ch) **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/
/** @author Christian Gutschow (chris.g@cern.ch) **/

#ifndef TrigIDJpsiMonitoring_AnalysisConfig_Jpsi_H
#define TrigIDJpsiMonitoring_AnalysisConfig_Jpsi_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"

#include "TrigJPsiTools/MakeDiMuonsTool.h"

#include "TrigIDJpsiMonitoring/JpsiTrackSelector.h"

#include "TrigIDJpsiMonitoring/JpsiAnalysis.h"
#include "TrigIDJpsiMonitoring/DuplicateRemover.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
//#include "TrigInDetAnalysisUtils/TIDA_newtracking.h"

//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
//#include "TrigInDetTrackExtrapolator/TrigInDetTrackExtrapolator.h"

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"

namespace TrigInDetAnalysis {

  class AnalysisConfig_Jpsi : public T_AnalysisConfig<IHLTMonTool> { 
    
    public:
      
      // Full constructor: test/reference/selection
      // - analysisInstanceName: the name of the analysis chain being created
      // - xxxChainName: the name of the chain to be used as test/reference/selection;
      //                 must be "StoreGate" in case of direct access to SG containers
      // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
      // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
      // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths;
      //            in case the test chain is a fake chain, this is used for RoI position too
      // - all standard operations are performed in loops over 0=test 1=reference 2=selection
      AnalysisConfig_Jpsi(const std::string &analysisInstanceName, const std::string& testChainName, const std::string& testType,
                          const std::string& testKey, const std::string& referenceChainName, const std::string& referenceType,
                          const std::string& referenceKey, const std::string& selectionChainName, const std::string& selectionType,
                          const std::string& selectionKey, TIDARoiDescriptor* roiInfo, TrackFilter* testFilter,
                          TrackFilter* referenceFilter, TrackFilter* selectionFilter, TrackAssociator* associator,
                         TrackAnalysis* analysis) : T_AnalysisConfig<IHLTMonTool>( analysisInstanceName, testChainName,
                                                                                   testType, testKey, referenceChainName,
                                                                                   referenceType, referenceKey,
                                                                                   selectionChainName, selectionType,
                                                                                   selectionKey, roiInfo, testFilter,
                                                                                   referenceFilter, selectionFilter,
                                                                                   associator, analysis),
                                                    //m_iBeamCondSvc(0)
                                                    m_iBeamCondSvc("BeamCondSvc",analysisInstanceName),
                                                    m_toolSvc("ToolSvc",analysisInstanceName),
                                                    m_selectorTestA(0), m_selectorTestB(0) {
         // m_efExtrapolator("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
         // m_l2Extrapolator("TrigInDetTrackExtrapolator") {
         m_selectorJ = 0;   
      }
        
      // Partial constructor: test/reference
      // - analysisInstanceName: the name of the analysis chain being created
      // - xxxChainName: the name of the chain to be used as test/reference;
      // must be "StoreGate" in case of direct access to SG containers
      // - xxxType: the type of tracks to be retrieved from the test/reference chain or container
      // - xxxKey:  the key for tracks to be retrieved from the test/reference chain or container
      // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths;
      //            in case the test chain is a fake chain, this is used for RoI position too
      // - all standard operations are performed in loops over 0=test 1=reference 2=selection
      AnalysisConfig_Jpsi(const std::string &analysisInstanceName, const std::string &testChainName, const std::string &testType,
                          const std::string &testKey, const std::string &referenceChainName, const std::string &referenceType,
                          const std::string &referenceKey, TIDARoiDescriptor* roiInfo, TrackFilter* testFilter,
                          TrackFilter* referenceFilter, TrackAssociator* associator, TrackAnalysis* analysis) :
                          T_AnalysisConfig<IHLTMonTool>( analysisInstanceName, testChainName, testType, testKey,
                                                         referenceChainName, referenceType, referenceKey, roiInfo,
                                                         testFilter, referenceFilter, associator, analysis),
                          //m_iBeamCondSvc(0),
                          m_iBeamCondSvc("BeamCondSvc",analysisInstanceName),
                          m_toolSvc("ToolSvc",analysisInstanceName),
                          m_jpsi(0), m_remover(0), m_selectorTestA(0), m_selectorTestB(0) {
        // m_efExtrapolator("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
        // m_l2Extrapolator("TrigInDetTrackExtrapolator") {

        if(m_selectorRef) {
          delete m_selectorRef;
          m_selectorRef = new JpsiTrackSelector( referenceFilter );
        }

        if(m_selectorTest) {
          delete m_selectorTest;
          m_selectorTest = new JpsiTrackSelector( testFilter );
        }
        m_selectorJ = 0;          
        m_jpsi = new MakeDiMuonsTool();
        m_remover = new DuplicateRemover();
        m_selectorTestA = new TrigTrackSelector( testFilter );
        m_selectorTestB = new TrigTrackSelector( testFilter );
        //if(m_l2Extrapolator.retrieve().isFailure())  cout << ">>> Unable to locate L2 extrapol tool!!" << endl;
        //if(m_efExtrapolator.retrieve().isFailure())  cout << ">>> Unable to locate EF extrapol tool!!" << endl;
      } 


      virtual ~AnalysisConfig_Jpsi() { 
        delete m_jpsi;
        delete m_remover;
        delete m_analysis;
        delete m_selectorTestA;
        delete m_selectorTestB;
        //if ( m_associator ){delete m_associator; m_associator=NULL;}
      }

      virtual void book();
      virtual void finalize();
        
      // helper functions
      float deltaR(  float eta1, float phi1, float eta2, float phi2);
      float deltaPhi(float phi1, float phi2);
      float fixPhi(  float phi);

      //analysis

      JpsiAnalysis* m_analysisJ; 

      template<class TrackCollection> void selectTracks(const std::string& key="", unsigned index=0,const std::string& chain="") {   
        m_provider->msg(MSG::DEBUG) << "Starting selectTracks ..." << endmsg;

        Trig::ExpertMethods* em = (*m_tdt)->ExperimentalAndExpertMethods();
        em->enable();
        const HLT::NavigationCore* nc = em->getNavigation();

        Trig::FeatureContainer fcont = (*m_tdt)->features( chain, TrigDefs::alsoDeactivateTEs );
        const std::vector< Trig::Combination >& comb = fcont.getCombinations();
        std::vector< Trig::Combination >::const_iterator c_itr = comb.begin();
        std::vector< Trig::Combination >::const_iterator c_end = comb.end();

        std::cout << "choosing ID chain name..." << std::endl;

        std::string idChainName;
        if(chain.find("2mu4T_")     != std::string::npos) idChainName = "L2_mu4T_IDTrkNoCut";
        //if(chain.find("2mu4T_")     != std::string::npos) idChainName = "L2_muon_standalone_mu4T";
        //if(chain.find("2mu4T_")     != std::string::npos) idChainName = "***REMOVED***";
        if(chain.find("2mu4_")      != std::string::npos) idChainName = "L2_mu4_IDTrkNoCut";
        if(chain.find("2mu13_")     != std::string::npos) idChainName = "L2_mu13_IDTrkNoCut";
        if(chain.find("e4_etcut_Jpsi") != std::string::npos) idChainName = "L2_e4_etcutid";
        else if(chain.find("e5_tight1_") != std::string::npos) idChainName = "L2_e5_tight1id";
        if(chain.find("e24vh_")     != std::string::npos) idChainName = "L2_e15_NoCutid";
        if(chain.find("mu24i_")     != std::string::npos) idChainName = ""; //L2_mu24_tight";
        if(chain.find("_mu20")      != std::string::npos) idChainName = "";
        //if(chain.find("mu24i_")     != std::string::npos) idChainName = " L2_muon_standalone_mu18_tight";

        m_provider->msg(MSG::DEBUG) << "chain= " << chain 
                                    << " idChainName= " << idChainName 
                                    << " key= " << key << endmsg;

        for( ; c_itr != c_end; ++c_itr ) {
          // Now we iterate over accepted "combinations". (Single or multiple
          // RoIs that passed the trigger.)
          /*if(index==0)       cout << "A Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;
          if(index==1)       cout << "B Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;
          if(index==2)       cout << "C Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;*/
          //cout << "   header - before get<...>" << std::endl;
          std::vector< Trig::Feature< TrackCollection > > probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          //cout << "   header - after get<...>" << probeTracks.size() << std::endl;

          // This will probably be a single-element vector if the chain used a
          // single TE. But if it was constructed from multiple TEs, we should
          // have multiple track collections here.
          // cout << "\n# combinations = " << comb.size() << " # collections = " << probeTracks.size() << "\n" << std::endl;
          
          for(unsigned ifeat = 0; ifeat < probeTracks.size(); ++ifeat) {
      
            Trig::Feature<TrackCollection> trackfeature = probeTracks.at(ifeat);

            Trig::Feature<TrigRoiDescriptor> trackroi = (*m_tdt)->ancestor<TrigRoiDescriptor>(trackfeature);
            const TrigRoiDescriptor* roid1 = trackroi.cptr();
            //	cout << "ID track RoI ID = " << roid1->roiId() << std::endl;
            /// if there are > 3 combinations, roiID can be > 1; ie. there are > 2 RoIs
            /// currently we ignore them
            int roiID = roid1->roiId();
            m_provider->msg(MSG::DEBUG) << "roi ID: " << roiID << endmsg;

            const HLT::TriggerElement* te = trackfeature.te();
            std::string TEName = "";
            TEName = (Trig::getTEName(*te).c_str());
            m_provider->msg(MSG::DEBUG) << "TE name: " << TEName << endmsg;
              
            // get all the named collections attached to the TE
            //if(TEName == "L2_mu4T_IDTrkNoCut"){
            std::vector< const TrackCollection* > collectionVector;
            if( !const_cast<HLT::NavigationCore *>(nc)->getFeatures( te, collectionVector, key ) ) return;
            
            for(unsigned iv = 0; iv < collectionVector.size(); iv++ ) {
              // collectionVector holds different algorithms (StratA, B, C)
              // index = 0, 1, 2 is the index where the algorithm sits
              if(index != iv) continue; // only consider one algorithm
        
              ///finally, extract the tracks from this RoI
              if(collectionVector[iv]) { 
                //if(count == ifeat){
                //cout << "V1 - RoI num: " << roiID << std::endl;
                getL2tracks<TrackCollection>(collectionVector[iv], roiID);
                //cout << "AC_Jpsi collectionVector[" << iv << "]->size() " << collectionVector[iv]->size() << " roiID " << roiID << std::endl;
                //}
                //if(roiID == 0) selectorA->selectTracks(collectionVector[iv], truthmap);
                //if(roiID == 1) selectorB->selectTracks(collectionVector[iv], truthmap);
                //if(roiID != 0 && roiID != 1) cout << "ID tracks roiID = " << roiID << std::endl;
              }
            }
            //}//specify TEname
          }
        }
        m_provider->msg(MSG::DEBUG) << "... end of selectTracks." << endmsg;
      }

      template<class TrackCollection> void selectTracks0(const std::string &key="", unsigned index=0, const std::string& chain="") {   
        m_provider->msg(MSG::DEBUG) << "Starting selectTracks0 ..." << endmsg;

        Trig::ExpertMethods* em = (*m_tdt)->ExperimentalAndExpertMethods();
        em->enable();
        const HLT::NavigationCore* nc = em->getNavigation();

        std::cout << "Chain: " << chain << std::endl;
        Trig::FeatureContainer fcont = (*m_tdt)->features( chain, TrigDefs::alsoDeactivateTEs );
        const std::vector< Trig::Combination >& comb = fcont.getCombinations();
        std::vector< Trig::Combination >::const_iterator c_itr = comb.begin();
        std::vector< Trig::Combination >::const_iterator c_end = comb.end();

        std::string idChainName;
        if( (chain.find("2mu4T_")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EFID_mu4T_IDTrkNoCut";
          else idChainName = "L2_mu4T_IDTrkNoCut";
        }
        if( (chain.find("2mu4_")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EFID_mu4_IDTrkNoCut";
          else idChainName = "L2_mu4_IDTrkNoCut";
        }

        if( (chain.find("2mu13_")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EFID_mu13_IDTrkNoCut";
          else idChainName = "L2_mu4_IDTrkNoCut";
        }
        
        if( (chain.find("mu24i_")!=std::string::npos) ) {
          if( key.find("InDetTrig") != std::string::npos) idChainName = "";
          else idChainName = "L2_mu24_tight";
        }
        if( (chain.find("_mu20")!=std::string::npos) ) {
          if( key.find("InDetTrig") != std::string::npos) idChainName = "";
          else idChainName = "L2_mu24_tight";
        }

        if( (chain.find("e4_etcut_Jpsi")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EF_e4_etcutid";
          else idChainName = "L2_e4_etcutid";
        }
        else if( (chain.find("e5_tight1_")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EF_e5_tight1id";
          else idChainName = "L2_e5_tight1id";
        }

       if( (chain.find("e24vh_")!=std::string::npos) ){ 
          if( key.find("InDetTrig") != std::string::npos) idChainName = "EF_e15_NoCutid1";
          else idChainName = "L2_e15_NoCutcl";
        }

        m_provider->msg(MSG::DEBUG) << "EF idChainName = " << idChainName  << "  key = " << key << endmsg;

        for( ; c_itr != c_end; ++c_itr ) {
          // Now we iterate over accepted "combinations". (Single or multiple
          // RoIs that passed the trigger.)
          //      if(index==0)       std::cout << "A Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;
          /*      if(index==1)       std::cout << "B Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;
          if(index==2)       std::cout << "C Before " << idChainName << " HLTTruncated = " << HLTTruncated << std::endl;*/
          std::cout << "selectTracks0 BEFORE c_itr->get<...>(...)" << std::endl;
          std::vector< Trig::Feature< TrackCollection > > probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "selectTracks0 AFTER c_itr->get<...>(...)" << std::endl;
          /// if empty, probably 2011... so try this one
          if(chain.find("mu24i") != std::string::npos && !probeTracks.size()) {
            if(chain.find("2mu4T_") != std::string::npos)  idChainName = "EF_EFID_mu4T_IDTrkNoCut";
            if(chain.find("2mu13_") != std::string::npos)  idChainName = "EF_EFID_mu13_IDTrkNoCut";
            probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          }
          /*      std::cout << "After after " << probeTracks.size() << std::endl;
          if(probeTracks.size() == 0) idChainName = "EF_trt_mu4T_IDTrkNoCut";
          probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "Next try " << probeTracks.size() << std::endl;
          if(probeTracks.size() == 0) idChainName = "EF_MS_mu4T_IDTrkNoCut";
          probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "And again " << probeTracks.size() << std::endl;
          if(probeTracks.size() == 0) idChainName = "EF_TB_mu4T_IDTrkNoCut";
          probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "And and again " << probeTracks.size() << std::endl;
          if(probeTracks.size() == 0) idChainName = "EF_SA_mu4T_IDTrkNoCut";
          probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "And and and again " << probeTracks.size() << std::endl;
          if(probeTracks.size() == 0) idChainName = "EF_mu4T_IDTrkNoCut";
          probeTracks = c_itr->get< TrackCollection >(key, TrigDefs::alsoDeactivateTEs, idChainName);
          std::cout << "Last try " << probeTracks.size() << std::endl;*/

          // This will probably be a single-element vector if the chain used a
          // single TE. But if it was constructed from multiple TEs, we should
          // have multiple track collections here.
          //std::cout << "\n# combinations = " << comb.size() << " # collections = " << probeTracks.size()	<< "\n" << std::endl;
          
          for(unsigned ifeat = 0; ifeat < probeTracks.size(); ++ifeat) {
      
            Trig::Feature<TrackCollection> trackfeature = probeTracks[ifeat];

            Trig::Feature<TrigRoiDescriptor> trackroi = (*m_tdt)->ancestor<TrigRoiDescriptor>(trackfeature);
            const TrigRoiDescriptor* roid1 = trackroi.cptr();
            //std::cout << "ID track RoI ID = " << roid1->roiId() << std::endl;

            // if there are > 3 combinations, roiID can be > 1; ie. there are > 2 RoIs
            // currently we ignore them
            int roiID = roid1->roiId();
            m_provider->msg(MSG::DEBUG) << "EF roi ID: " << roiID << "eta = " << roid1->eta() << " phi = " << roid1->phi() << endmsg;

            const HLT::TriggerElement* te = trackfeature.te();
            std::string TEName = "";
            TEName = (Trig::getTEName(*te).c_str());
            m_provider->msg(MSG::DEBUG) << "EF TE name: " << TEName << endmsg;
        
            // get all the named collections attached to the TE
            std::vector<const TrackCollection*> collectionVector;
            if( !const_cast<HLT::NavigationCore*>(nc)->getFeatures(te, collectionVector, key) )  return;
      
            for( unsigned iv = 0; iv < collectionVector.size(); iv++ ) {
              // collectionVector holds different algorithms
              // for EF, index is always 0
              if(index != iv) continue; // only consider one algorithm
              //finally, extract the tracks from this RoI
              if(collectionVector[iv]) { 
                //if(count == ifeat) {
                //std::cout << "V2 - RoI num: " << roiID << std::endl;
                getEFtracks<TrackCollection>(collectionVector[iv], roiID);
                //std::cout << "AC_Jpsi collectionVector[" << iv << "]->size() " << collectionVector[iv]->size() << std::endl;
                //}
                //if(roiID == 0) selectorA->selectTracks(collectionVector[iv], truthmap);
                //if(roiID == 1) selectorB->selectTracks(collectionVector[iv], truthmap);
                //if(roiID != 0 && roiID != 1) std::cout << "ID tracks roiID = " << roiID << std::endl;
              }
            }
          }
        }
        m_provider->msg(MSG::DEBUG) << "... end of selectTracks0." << endmsg;
      }

      ///PW modified for 2 RoIs
      /*  template<class TrackCollection> 
      void selectTracks( TrigTrackSelector* selector, const std::string& key="", const TrigInDetTrackTruthMap* truthmap=0, unsigned index=0, 
             const std::string& chain="") {   

        ///get the all-powerful expert methods
        Trig::ExpertMethods* expert = (*m_tdt)->ExperimentalAndExpertMethods();
        expert->enable();

        ///retrieve all trigger elements for defined subchain, eg. for a primary trigger "EF_2mu4T_Jpsimumu_IDTrkNoCut", 
        ///chain = "L2_mu4T_IDTrkNoCut"
        std::vector<HLT::TriggerElement*> tes;
        expert->getNavigation()->getAllOfType(chain, tes, true);

        ///this loop should iterate once / RoI
        for(std::vector<HLT::TriggerElement*>::const_iterator te = tes.begin(); te!=tes.end(); te++){
        //    std::vector<HLT::TriggerElement*>::const_iterator te = tes.begin();

          std::string TEName = "";
          TEName = (Trig::getTEName(*(*te))).c_str();
          std::cout << "TE name = " << TEName << std::endl;

          ///using the ancestor method gives us access to all the tracks - for leading and 
          ///test algorithms - in the RoI
          std::vector< Trig::Feature<TrackCollection> > featureVector = (*m_tdt)->ancestors<TrackCollection>(*te, key);

          ///this loop should iterate once / track collection, eg. MuonA, MuonB, etc...
          for( unsigned iv=0; iv<featureVector.size(); iv++){
      if( index!=iv ) continue;
      const TrackCollection* idTracks = featureVector[iv].cptr();

      ///finally, extract the tracks from this RoI
      std::string algo;
      if(index==0) algo = "A"; 
      if(index==1) algo = "B"; 
      if(index==2) algo = "C"; 
      std::cout << "key = " << key << algo << std::endl;
      if(idTracks) selector->selectTracks(idTracks, truthmap);

          }
        }  
      }*/

      protected:

        virtual void loop();

        template<class TrackCollection> void getL2tracks(const TrackCollection *collection, int roi) {

          m_provider->msg(MSG::DEBUG) << "Starting getL2tracks ..." << endmsg;
          typename TrackCollection::const_iterator trackitr = collection->begin();
          typename TrackCollection::const_iterator trackend = collection->end();
          for( ; trackitr != trackend; ++trackitr) {
            const TrigInDetTrack* track = dynamic_cast<const TrigInDetTrack*>( *trackitr );
            if(track) {
              double eta      = track->param()->eta();
              double phi      = track->param()->phi0();
              double z0       = track->param()->z0(); 
              double pT       = track->param()->pT(); 
              double d0       = track->param()->a0();
              double deta     = track->param()->eeta();
              double dphi     = track->param()->ephi0();
              double dz0      = track->param()->ez0(); 
              double dpT      = track->param()->epT(); 
              double dd0      = track->param()->ea0();

              int algoid      = track->algorithmId(); 	      
              int nBlayerHits = (track->HitPattern() & 0x1);
              int nPixelHits  = 2 * track->NPixelSpacePoints();  // NB: for comparison with offline 
              int nSctHits    = 2 * track->NSCT_SpacePoints();   //     a spacepoint is 2 "hits"
              int nStrawHits  = track->NStrawHits();
              int nTrHits     = track->NTRHits();
              int nSiHits     = nPixelHits + nSctHits;

              bool expectBL   = false;                           //not filled in 

              unsigned int hitPattern = track->HitPattern();
              unsigned int multiPattern = 0;

              double chi2 = track->chi2();

              bool truth        = false;
              int match_barcode = -1;
      
              m_trigTracks.push_back( new TIDA::Track(eta, phi, z0, d0, pT, chi2, 
                                                                   deta, dphi, dz0, dd0, dpT, 
                                                                   nBlayerHits, nPixelHits, nSctHits,
                                                                   nSiHits, nStrawHits, nTrHits, 
                                                                   hitPattern, multiPattern, 
                                                                   algoid, truth, -1, match_barcode, expectBL) ); 
          
              /*double phiAtCalo = 0., etaAtCalo = 0.;
              // arguments: track, RCalBarrelFace, ZCalEndcapFace, extrapolatated phi, extrapolated phi
              StatusCode sc = m_l2Extrapolator->extrapolateToCalo((*trackitr), 1470.0*CLHEP::mm, 3800.0*CLHEP::mm, phiAtCalo, etaAtCalo);
              if(sc.isFailure())  std::cout << "L2 extrapolator failed " << std::endl;   
              m_phiAtCalo[ int(m_trigTracks.size() - 1) ] = phiAtCalo;
              m_etaAtCalo[ int(m_trigTracks.size() - 1) ] = etaAtCalo;*/
              m_trackRois[ int(m_trigTracks.size() - 1) ] = roi;
              //std::cout << "SUTT ID track " << *t << "\t0x" << std::hex << track->HitPattern() << std::dec << std::endl;
              //if( !addTrack( t ) ) delete t;
            } // if(track)
          } // loop over collection
          m_provider->msg(MSG::DEBUG) << "... end of getL2tracks." << endmsg;
        }

        template<class TrackCollection> void getEFtracks(const TrackCollection *collection, int roi) {
          m_provider->msg(MSG::DEBUG) << "Starting getEFtracks ..." << endmsg;
    
          typename TrackCollection::const_iterator trackitr = collection->begin();
          typename TrackCollection::const_iterator trackend = collection->end();
          for( ; trackitr != trackend; ++trackitr) {
            const Rec::TrackParticle* track = (*trackitr);
            if(track) {
              static int hpmap[] = { 0, 1, 2, 7, 8, 9, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };


#ifndef TIDA_NEWTRACKING_H
              const Trk::MeasuredPerigee* measPer = track->measuredPerigee();
#else
              const Trk::Perigee* measPer = track->measuredPerigee();
#endif
              
              double pT    = measPer->pT(); 
              double eta   = measPer->eta();
              double phi   = measPer->parameters()[Trk::phi0];
              double z0    = measPer->parameters()[Trk::z0];
              double d0    = measPer->parameters()[Trk::d0];

              m_provider->msg(MSG::DEBUG) << "current track -- eta = " << eta << " phi = " << phi << endmsg;

              if( measPer->parameters()[Trk::qOverP]<0 ) pT *= -1;

              double deta = 1;
              double dphi = 1;
              double dz0  = 1;
              double dd0  = 1;
              double dpT  = 1;	 

              // Check number of hits
              // NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
              // 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
              // stereo clusters making a spacepoint are two "hits"
              const Trk::TrackSummary *summary = track->trackSummary();
              int nBlayerHits = 2*summary->get(Trk::numberOfBLayerHits); 
              int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
              int nSctHits    = summary->get(Trk::numberOfSCTHits); 
              int nStrawHits  = summary->get(Trk::numberOfTRTHits);
              int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);

              int nSiHits     = nPixelHits + nSctHits;
              bool expectBL   = summary->get(Trk::expectBLayerHit);

              const Trk::FitQuality *quality = track->fitQuality();
              double chi2 = quality->chiSquared();

              unsigned bitmap = 0;
              
              for(int ih = 0; ih < 20; ih++ ) {
                if( summary->isHit(Trk::DetectorType(ih)) )  bitmap |= ( 1<<hpmap[ih] ); 	
              }

              // now some *ridiculous* code to get the author of the 
              // TrackParticle (offline) tracks
              
              int fitter = track->info().trackFitter();
              std::string dumpinfo = track->info().dumpInfo();
              
              int trackAuthor = -1; 
              if( fitter > 0 && fitter<Trk::TrackInfo::NumberOfTrackFitters ) {
                if(      dumpinfo.find("TRTStandalone"       ) != std::string::npos)  trackAuthor = 2;
                else if( dumpinfo.find("TRTSeededTrackFinder") != std::string::npos)  trackAuthor = 1;
                else                                                             trackAuthor = 0;
              }

              m_trigTracks.push_back( new TIDA::Track(eta, phi, z0, d0, pT, chi2,
                                                                   deta, dphi, dz0, dd0, dpT,
                                                                   nBlayerHits, nPixelHits, nSctHits,
                                                                   nSiHits, nStrawHits, nTrHits,
                                                                   bitmap, 0, trackAuthor, expectBL) );  

              const Trk::TrackParticleBase trk = (Trk::TrackParticleBase)*track;
              m_provider->msg(MSG::DEBUG) << "original track?? " << trk.originalTrack() << endmsg;

/* #ifndef TIDA_NEWTRACKING_H
              double phiAtCalo = eta, etaAtCalo = phi; //(*trackitr),
              const Trk::ParametersBase* result = m_efExtrapolator->extrapolate(trk,
                                                    ((fabs(eta)<1.5)? CaloCell_ID::EMB2 : CaloCell_ID::EME2),
                                                    0.0, Trk::alongMomentum, Trk::undefined);
#else
              const Trk::TrackParameters* result = m_efExtrapolator->extrapolate(trk,
                                                    ((fabs(eta)<1.5)? CaloCell_ID::EMB2 : CaloCell_ID::EME2),
                                                    0.0, Trk::alongMomentum, Trk::undefined);
#endif
              if(!result)  std::cout << "EF extrapolator failed " << std::endl;
              else {
                phiAtCalo = result->position().phi();
                etaAtCalo = result->position().eta();
              }
              m_provider->msg(MSG::DEBUG) << "key: " << int(m_trigTracks.size() - 1) << " track: " << track
                                          << " trackitr: " << (*trackitr) << " r& = " << result
                                          << " phi = " << phiAtCalo  << " eta = " << etaAtCalo << endmsg;
              m_phiAtCalo[ int(m_trigTracks.size() - 1) ] = phiAtCalo;
              m_etaAtCalo[ int(m_trigTracks.size() - 1) ] = etaAtCalo; --> see also line w/ delete!!*/
              m_trackRois[ int(m_trigTracks.size() - 1) ] = roi;
              //std::cout << "SUTT ID track " << *t << "\t0x" << std::hex << track->HitPattern() << std::dec << std::endl;
              //if( !addTrack( t ) ) delete t;
              //if(result)  delete result;
            } // if(track)
          } // end of loop over collection
          //delete m_test;
          m_provider->msg(MSG::DEBUG) << "... end of getEFtracks." << endmsg;
        }

        void makeClean() {
          m_provider->msg(MSG::DEBUG) << "Start cleaning... " << endmsg;
          for(unsigned i=0; i < m_trigTracks.size(); i++) delete m_trigTracks[i];
          m_trigTracks.clear();
          m_trackRois.clear();
          //m_etaAtCalo.clear();
          //m_phiAtCalo.clear();
          if(m_selectorJ)  m_selectorJ->clear();
          if(m_selectorTestA)  m_selectorTestA->clear();
          if(m_jpsi)  m_jpsi->clear();
          m_provider->msg(MSG::DEBUG) << "... all clear." << endmsg;
        }

      private:

        // Beamspot
        ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;
        ServiceHandle<IToolSvc> m_toolSvc;
        //IBeamCondSvc* m_iOnlineBeamCondSvc;

        //ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; // pointer to beam condition service
        //Trk::VxCandidate* m_beamSpotVxCand;
          
        //TrigJpsiTools
        std::string m_stream;
        std::string m_tracksName;
        std::string m_triggerChainName;
        std::string m_muonCollection;
        double m_jpsiPDGmass;
        double m_jpsiwidth;
        double m_ptcut;
        double m_tagptcut;
        double m_etacut;
        double m_tagetacut;
        double m_muonMass;
        double m_plusMass;
        double m_minusMass;
        int m_checkrate;
        int m_events;
        bool m_TagAndProbe;
        bool m_useTrigger;  
        bool m_runOnline;

        unsigned int m_count_before, m_count_after;

        MakeDiMuonsTool* m_jpsi;
        DuplicateRemover* m_remover;

        TrigTrackSelector* m_selectorTestA;
        TrigTrackSelector* m_selectorTestB;
        //ToolHandle<IExtrapolateToCaloTool>      m_efExtrapolator;
        //ToolHandle<ITrigInDetTrackExtrapolator> m_l2Extrapolator;

        ///temp
        std::vector<int> m_testtrackRoIs;
        std::vector<int> m_reftrackRoIs;
        ///temp
        //TFile  *fOut;
        //TH1F   *my_hist;
        bool   /*my_check,*/ m_isMuon, m_isJpsi;
        unsigned int m_trigDefs;

        //map<int, std::vector<Track*> > m_trigTracks;
        std::map<int, int> m_trackRois;
        //map<int, double> m_phiAtCalo, m_etaAtCalo;
        std::vector<TIDA::Track*> m_trigTracks;

        // SG
        std::string m_truthCollection;
        
        //selector
        JpsiTrackSelector* m_selectorJ;

        //associator
        Associator_BestDeltaRMatcher* m_associatorJ;

        //event info
        const DataHandle<EventInfo> m_eventInfo;

  };
}

#endif  // TrigIDJpsiMonitoring_AnalysisConfig_Jpsi_H

