/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/IntersectionPos.h"
#include "VrtSecInclusive/NtupleVars.h"

#include <iostream>
#include <vector>


#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::setupNtupleVariables() {
    
    m_ntupleVars->addNewVar<unsigned int>     ( "RunNumber"                );
    m_ntupleVars->addNewVar<unsigned int>     ( "Event"                    );
    m_ntupleVars->addNewVar<unsigned int>     ( "Time"                     );
    m_ntupleVars->addNewVar<unsigned int>     ( "LumiBlock"                );
    m_ntupleVars->addNewVar<unsigned int>     ( "BCID"                     );
    m_ntupleVars->addNewVar<unsigned int>     ( "LVL1ID"                   );
    
    m_ntupleVars->addNewVar<unsigned int>     ( "NumAllTrks"               );
    m_ntupleVars->addNewVar<unsigned int>     ( "NumSelTrks"               );
    m_ntupleVars->addNewVar<unsigned int>     ( "SizeIncomp"               );
    
    m_ntupleVars->addNewVar<unsigned int>     ( "NumPV"                    );
    m_ntupleVars->addNewVar<unsigned int>     ( "NTrksPV"                  );
    m_ntupleVars->addNewVar<unsigned int>     ( "PVType"                   );
    m_ntupleVars->addNewVar<double>           ( "PVX"                      );
    m_ntupleVars->addNewVar<double>           ( "PVY"                      );
    m_ntupleVars->addNewVar<double>           ( "PVZ"                      );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "NdofTrksPV"               );
    m_ntupleVars->addNewVar< vector<double> > ( "PVZpile"                  );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_id"               );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_pT"               );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_2dIPPV"           );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_ZIPPV"            );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_2dIP"             );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_ZIP"              );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_phi"              );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_eta"              );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_chi2"             );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_eta"              );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_BLayHits"         );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_PixHits"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_SCTHits"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_TRTHits"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_PixBar1"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_PixBar2"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_barcode"          );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_matchPr"          );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_2dIPErr"          );
    m_ntupleVars->addNewVar< vector<double> > ( "RecoTrk_ZIPErr"           );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_PixShare"         );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_SCTShare"         );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_TrkAuth"          );
    m_ntupleVars->addNewVar< vector<int> >    ( "RecoTrk_TrkLowPt"         );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "SelTrk_barcode"           );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_id"                 );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_pT"                 );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_p"                  );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_phi"                );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_eta"                );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_2dIP"               );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_ZIP"                );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_delp"               );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_del2dIP"            );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_delzIP"             );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_eta"                );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_BLayHits"           );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_PixHits"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_SCTHits"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_TRTHits"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_barcode"            );
    m_ntupleVars->addNewVar< vector<double> > ( "SVTrk_matchPr"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_TrkAuth"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SVTrk_TrkLowPt"           );
    
    m_ntupleVars->addNewVar< unsigned int >   ( "All2TrkVrtNum"            );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtMass"           );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtPt"             );
    m_ntupleVars->addNewVar< vector<int> >    ( "All2TrkVrtCharge"         );
    m_ntupleVars->addNewVar< vector<int> >    ( "All2TrkSumBLHits"         );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtX"              );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtY"              );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtZ"              );
    m_ntupleVars->addNewVar< vector<double> > ( "All2TrkVrtChiSq"          );
    
    m_ntupleVars->addNewVar< unsigned int >   ( "AfFakVrtNum"              );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtMass"             );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtPt"               );
    m_ntupleVars->addNewVar< vector<int> >    ( "AfFakVrtCharge"           );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtX"                );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtY"                );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtZ"                );
    m_ntupleVars->addNewVar< vector<double> > ( "AfFakVrtChiSq"            );
    
    m_ntupleVars->addNewVar< unsigned int >   ( "NumInitSecVrt"            );
    m_ntupleVars->addNewVar< unsigned int >   ( "NumRearrSecVrt"           );
    m_ntupleVars->addNewVar< unsigned int >   ( "NumSecVrt"                );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtxX"                  );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtxY"                  );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtxZ"                  );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_NumTrks"           );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Mass"              );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Mass_electron"     );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Chi2"              );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_pT"                );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_pZ"                );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_Charge"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_SumBLayHits"       );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_AllTrksBLayHits"   );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_MinOpAng"          );
    
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkPt"             );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkPhi"            );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkEta"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_TrkBLay"           );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_TrkPixExclBLay"    );
    m_ntupleVars->addNewVar< vector<int> >    ( "SecVtx_TrkSCT"            );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Trk2dIP"           );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkZIP"            );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkdelP"           );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Trkdel2dIP"        );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkdelZIP"         );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkPtWrtSV"        );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkPhiWrtSV"       );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkEtaWrtSV"       );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Trk2dIPWrtSV"      );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkZIPWrtSV"       );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkdelPWrtSV"      );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_Trkdel2dIPWrtSV"   );
    m_ntupleVars->addNewVar< vector<double> > ( "SecVtx_TrkdelZIPWrtSV"    );

    m_ntupleVars->addNewVar<double>           ( "Truth_SV1X"               );
    m_ntupleVars->addNewVar<double>           ( "Truth_SV1Y"               );
    m_ntupleVars->addNewVar<double>           ( "Truth_SV1Z"               );
    m_ntupleVars->addNewVar<double>           ( "Truth_PiEta"              );
    m_ntupleVars->addNewVar<double>           ( "Truth_PiPhi"              );
    m_ntupleVars->addNewVar<double>           ( "Truth_PiPt"               );
    m_ntupleVars->addNewVar<int>              ( "Truth_PiInt"              );
    m_ntupleVars->addNewVar<int>              ( "Truth_nSVHiPt"            );
    m_ntupleVars->addNewVar<int>              ( "Truth_nSVLoPt"            );
    m_ntupleVars->addNewVar<int>              ( "Truth_nSVGe1HiPt"         );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllVtxType"         );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVX"             );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVY"             );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVZ"             );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVTrk1Pt"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVTrk2Pt"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVTrk3Pt"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVTrk4Pt"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVTrk5Pt"        );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVNumTrks"       );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVNumReTrks"     );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVCharge"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVSumTrksPt"     );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVSumTrksPz"     );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVHasLifetime"   );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVStrangeBaryon" );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVIncomingPid"   );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllSVNumIncident"   );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVPtIncident"    );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVPzIncident"    );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllSVMinOpAng"      );
    
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrk2dIP"         );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkZIP"          );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkPt"           );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkEta"          );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkPhi"          );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkR"            );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllTrkZ"            );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllTrkBC"           );
    
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllRefitSt"         );
    m_ntupleVars->addNewVar< vector<int> >    ( "Truth_AllRefitNTrk"       );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllRefitChi2"       );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllRefitSVX"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllRefitSVY"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllRefitSVZ"        );
    m_ntupleVars->addNewVar< vector<double> > ( "Truth_AllRefitMass"       );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::deleteNtupleVariables() {
    
    m_ntupleVars->deleteNtupleVariables();
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::setupNtuple() {
    
    m_ntupleVars->branchNtuple( m_tree_Vert );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::clearNtupleVariables() {
    
    m_ntupleVars->clear();
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::addEventInfo() {
    // add event level variables
    //
    ATH_MSG_DEBUG( " > addEventInfo: in addEventInfo");
    
    // this code has been taken from AnalysisExamples/VFitZmmOnAOD
    // 
    //get EventInfo for run and event number
    
    const xAOD::EventInfo* eventInfo;
    ATH_CHECK( evtStore()->retrieve(eventInfo) );
    
    //
    //m_ntupleVars->m_runNumber   = eventInfo->runNumber();
    m_ntupleVars->get<unsigned int>( "RunNumber" )  = eventInfo->runNumber();
    m_ntupleVars->get<unsigned int>( "Event")       = eventInfo->eventNumber();
    m_ntupleVars->get<unsigned int>( "Time" )       = eventInfo->timeStamp() ; 
    m_ntupleVars->get<unsigned int>( "LumiBlock" )  = eventInfo->lumiBlock() ;
    m_ntupleVars->get<unsigned int>( "BCID" )       = eventInfo->bcid();
    
    ATH_MSG_DEBUG( " > addEventInfo: event "<< m_ntupleVars->get<unsigned int>( "Event" ) );
    
    return StatusCode::SUCCESS;
  }  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::fillAANT_SelectedBaseTracks() {
    
    for ( const auto* trk : *m_selectedTracks ) {
      
      if( m_jp.FillNtuple ) {
        uint8_t tmpT;
        if( !(trk->summaryValue(tmpT,xAOD::numberOfPixelHits)) )  tmpT=0;
        m_ntupleVars->get< vector<int> >( "SVTrk_PixHits" ).emplace_back( (int) tmpT);
        
        if( !(trk->summaryValue(tmpT,xAOD::numberOfSCTHits)) )  tmpT=0;
        m_ntupleVars->get< vector<int> >( "SVTrk_SCTHits" ).emplace_back( (int) tmpT);
        
        if( !(trk->summaryValue(tmpT,xAOD::numberOfTRTHits)) )  tmpT=0;
        m_ntupleVars->get< vector<int> >( "SVTrk_TRTHits" ).emplace_back( (int) tmpT);
        
        if( !(trk->summaryValue(tmpT,xAOD::numberOfInnermostPixelLayerHits)) )  tmpT=0;
        m_ntupleVars->get< vector<int> >( "SVTrk_BLayHits" ).emplace_back( (int) tmpT);
      }
      
      
      //
      // get perigee params
      //
      const auto& perigee = trk->perigeeParameters();
      
      if( m_jp.FillNtuple ) {
      
        double phi       = perigee.parameters()[Trk::phi];
        double theta     = perigee.parameters()[Trk::theta];
        double d0        = perigee.parameters()[Trk::d0];
        double qOverP    = perigee.parameters()[Trk::qOverP];
        double errqOverP = (*(perigee.covariance()))(Trk::qOverP,Trk::qOverP);
        double errd0     = (*(perigee.covariance()))(Trk::d0,Trk::d0);
        double ptrk      = (1./qOverP);
        double pT        = ptrk*sin(theta);
        double trketa    = -1.*log( tan(theta/2) );
      
        // here we have to look at the original recotrack id to establish cross-link
        // between "SVTrk" vars and "RecoTrk" vars:
        m_ntupleVars->get< vector<int> >( "SVTrk_id" )      .emplace_back( trk->auxdataConst<unsigned long>("trk_id") );
      
        m_ntupleVars->get< vector<double> >( "SVTrk_pT" )   .emplace_back(pT);
        m_ntupleVars->get< vector<double> >( "SVTrk_p" )    .emplace_back(ptrk);
        m_ntupleVars->get< vector<double> >( "SVTrk_phi" )  .emplace_back(phi);
        m_ntupleVars->get< vector<double> >( "SVTrk_eta" )  .emplace_back(trketa);
        m_ntupleVars->get< vector<double> >( "SVTrk_2dIP" ) .emplace_back(d0);
        m_ntupleVars->get< vector<double> >( "SVTrk_ZIP" )  .emplace_back(perigee.parameters()[Trk::z0]);
      
        double matchProb = -1;
        int barcode = 0;
        if(m_jp.doTruth) 
          {  
            //HepMC::GenParticle* aTemp_truth = getTrkGenParticle(trk, matchProb);
            const xAOD::TruthParticle* aTemp_truth = getTrkGenParticle( trk );
            if( aTemp_truth )
              {
                barcode = aTemp_truth->barcode();
                matchProb= trk->auxdataConst< float >( "truthMatchProbability" );;    
              }
          }
      
        m_ntupleVars->get< vector<int>    >( "SVTrk_barcode" ) .emplace_back( barcode );
        m_ntupleVars->get< vector<double> >( "SVTrk_matchPr" ) .emplace_back( matchProb );
      
        ATH_MSG_DEBUG(" > fillAANT_SelectedBaseTracks: Sel Trk d0/pT/eta/match bc/pr "
                      << d0      << ","
                      << pT      << ","
                      << trketa  << ","
                      << barcode << ","
                      << matchProb );
      
        double errp = ptrk*ptrk*errqOverP;
      
        m_ntupleVars->get< vector<double> >( "SVTrk_delp" )    .emplace_back( errp );
        m_ntupleVars->get< vector<double> >( "SVTrk_del2dIP" ) .emplace_back( errd0 );
        m_ntupleVars->get< vector<double> >( "SVTrk_delzIP" )  .emplace_back( (*(perigee.covariance()))(Trk::z0,Trk::z0) );
      
      }
      
      
    } // end of selected tracks
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::fillAANT_SecondaryVertices( xAOD::VertexContainer *vertices ) {
    
    // Loop over vertices
    for( xAOD::Vertex *vertex : *vertices ) {
      
      // Track Loop
      for(size_t itrk=0; itrk<vertex->nTrackParticles(); itrk++) {
        
        // Here trk is not const - will augment SV perigee variables in the loop.
        const xAOD::TrackParticle* trk = vertex->trackParticle( itrk );
        
        //
        // add variables for each track into ntuple
        // Add all vertices to same ntuple, and extract later in root macro 
        //
        m_ntupleVars->get< vector<double> >( "SecVtx_TrkPt" )      .emplace_back( trk->pt() );
        m_ntupleVars->get< vector<double> >( "SecVtx_TrkPhi" )     .emplace_back( trk->phi() );
        m_ntupleVars->get< vector<double> >( "SecVtx_TrkEta" )     .emplace_back( trk->eta() );
        m_ntupleVars->get< vector<double> >( "SecVtx_Trk2dIP" )    .emplace_back( trk->d0() );
        m_ntupleVars->get< vector<double> >( "SecVtx_TrkZIP" )     .emplace_back( trk->z0() );
        m_ntupleVars->get< vector<double> >( "SecVtx_Trkdel2dIP" ) .emplace_back( trk->definingParametersCovMatrix()(Trk::d0, Trk::d0) );
        m_ntupleVars->get< vector<double> >( "SecVtx_TrkdelZIP" )  .emplace_back( trk->definingParametersCovMatrix()(Trk::z0, Trk::z0) );
        
        track_summary trk_summary;
        fillTrackSummary( trk_summary, trk );
        
        m_ntupleVars->get< vector<int> >( "SecVtx_TrkBLay" )        .emplace_back( trk_summary.numIBLHits );
        m_ntupleVars->get< vector<int> >( "SecVtx_TrkPixExclBLay" ) .emplace_back( trk_summary.numPixelHits - trk_summary.numIBLHits );
        m_ntupleVars->get< vector<int> >( "SecVtx_TrkSCT" )         .emplace_back( trk_summary.numSctHits );
        
        ATH_MSG_VERBOSE(" >> fillAANT_SecondaryVertices : filling track vars wrt. SV");
        if( trk->isAvailable<float>("pt_wrtSV")    &&
            trk->isAvailable<float>("eta_wrtSV")   &&
            trk->isAvailable<float>("phi_wrtSV")   &&
            trk->isAvailable<float>("d0_wrtSV")    &&
            trk->isAvailable<float>("z0_wrtSV")    &&
            trk->isAvailable<float>("errP_wrtSV")  &&
            trk->isAvailable<float>("errd0_wrtSV") &&
            trk->isAvailable<float>("errz0_wrtSV")          ) {
          
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkPtWrtSV" )      .emplace_back( trk->auxdataConst<float>( "pt_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkEtaWrtSV" )     .emplace_back( trk->auxdataConst<float>( "eta_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkPhiWrtSV" )     .emplace_back( trk->auxdataConst<float>( "phi_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_Trk2dIPWrtSV" )    .emplace_back( trk->auxdataConst<float>( "d0_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkZIPWrtSV" )     .emplace_back( trk->auxdataConst<float>( "z0_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkdelPWrtSV" )    .emplace_back( trk->auxdataConst<float>( "errP_wrtSV" ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_Trkdel2dIPWrtSV" ) .emplace_back( trk->auxdataConst<float>( "errd0_wrtSV"  ) );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkdelZIPWrtSV" )  .emplace_back( trk->auxdataConst<float>( "errz0_wrtSV"  ) );
          
        } else {
          
          ATH_MSG_VERBOSE(" >> fillAANT_SecondaryVertices : filling track vars wrt. SV (invalid values)");
          
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkPtWrtSV" )      .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkEtaWrtSV" )     .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkPhiWrtSV" )     .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_Trk2dIPWrtSV" )    .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkZIPWrtSV" )     .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkdelPWrtSV" )    .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_Trkdel2dIPWrtSV" ) .emplace_back( AlgConsts::invalidFloat );
          m_ntupleVars->get< vector<double> >( "SecVtx_TrkdelZIPWrtSV" )  .emplace_back( AlgConsts::invalidFloat );
          
        }
        
      } // loop over tracks in vertex
      
      ATH_MSG_DEBUG(" >> fillAANT_SecondaryVertices : Track loop end. ");
      
      ATH_MSG_VERBOSE(" >> fillAANT_SecondaryVertices : filling vertex vars");
      
      m_ntupleVars->get< vector<int>    >( "SecVtx_NumTrks" )         .emplace_back( vertex->nTrackParticles()   );
      m_ntupleVars->get< vector<double> >( "SecVtx_Chi2" )            .emplace_back( vertex->chiSquared()   );
      m_ntupleVars->get< vector<double> >( "SecVtxX" )                .emplace_back( vertex->x()     );
      m_ntupleVars->get< vector<double> >( "SecVtxY" )                .emplace_back( vertex->y()     );
      m_ntupleVars->get< vector<double> >( "SecVtxZ" )                .emplace_back( vertex->z()     );
      m_ntupleVars->get< vector<double> >( "SecVtx_Mass" )            .emplace_back( vertex->isAvailable<float>("mass")            ? vertex->auxdataConst<float>("mass")            : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<double> >( "SecVtx_Mass_electron" )   .emplace_back( vertex->isAvailable<float>("mass_e")          ? vertex->auxdataConst<float>("mass_e")          : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<double> >( "SecVtx_pT" )              .emplace_back( vertex->isAvailable<float>("pT")              ? vertex->auxdataConst<float>("pT")              : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<double> >( "SecVtx_pZ" )              .emplace_back( vertex->isAvailable<float>("pz")              ? vertex->auxdataConst<float>("pz")              : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<int> >( "SecVtx_Charge" )             .emplace_back( vertex->isAvailable<float>("vtx_charge")      ? vertex->auxdataConst<float>("vtx_charge")      : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<int>    >( "SecVtx_SumBLayHits" )     .emplace_back( vertex->isAvailable<float>("sumBLayHits")     ? vertex->auxdataConst<float>("sumBLayHits")     : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<int>    >( "SecVtx_AllTrksBLayHits" ) .emplace_back( vertex->isAvailable<float>("allTrksBLayHits") ? vertex->auxdataConst<float>("allTrksBLayHits") : AlgConsts::invalidFloat);
      m_ntupleVars->get< vector<double> >( "SecVtx_MinOpAng" )        .emplace_back( vertex->isAvailable<float>("minOpAng")        ? vertex->auxdataConst<float>("minOpAng")        : AlgConsts::invalidFloat);
      
      
    } // loop over vertices
    
    return StatusCode::SUCCESS;
  }
  
} // end of namespace VKalVrtAthena

