/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"
#include "VrtSecInclusive/Tools.h"

#include "TrkTrackSummary/TrackSummary.h"


#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLorentzVector.h"

#include <iostream>
#include <algorithm>

//-------------------------------------------------

using namespace std;

namespace Trk {
  extern 
    int  pgraphm_(
        long int *weit, long int *edges, long int *nodes,
        long int *set, long int *nptr,  long int *nth);
}

namespace VKalVrtAthena {


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::extractIncompatibleTracks( std::vector<int>& Incomp )
  {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from WrkVrtSet to xAOD::Vertex here.
    // The supposed form of the function will be as follows:

    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    xAOD::VertexContainer *m_twoTrksVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( m_twoTrksVertexContainer, "VrtSecInclusive_All2TrksVertices" ) );
    
    // Work variables
    vector<const xAOD::TrackParticle*>    ListBaseTracks;
    vector<const xAOD::NeutralParticle*>  dummyNeutrals(0);
    vector<double> Results;
    Amg::Vector3D               FitVertex;
    TLorentzVector              Momentum;
    vector< vector<double> > TrkAtVrt; 
    vector<double> Chi2PerTrk,ErrorMatrix;
    long int           Charge;
    double             Chi2 = 0.;
    
    ATH_MSG_DEBUG(" > extractIncompatibleTracks: Selected Tracks = "<< selectedBaseTracks->size());


    // first make all 2-track vertices
    for( auto itrk = selectedBaseTracks->begin(); itrk != selectedBaseTracks->end(); ++itrk ) {
      for( auto jtrk = itrk; jtrk != selectedBaseTracks->end(); ++jtrk ) {

        if( itrk == jtrk ) continue;

        int itrk_id = itrk - selectedBaseTracks->begin();
        int jtrk_id = jtrk - selectedBaseTracks->begin();

        // Attempt to think the combination is incompatible by default
        Incomp.emplace_back( itrk_id );
        Incomp.emplace_back( jtrk_id );

        ListBaseTracks.clear();
        ListBaseTracks.emplace_back( *itrk );
        ListBaseTracks.emplace_back( *jtrk );

        // new code to find initial approximate vertex
        Amg::Vector3D IniVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( ListBaseTracks, IniVertex );/* Fast crude estimation */
        if(sc.isFailure()) ATH_MSG_DEBUG(" > extractIncompatibleTracks: fast crude estimation fails ");

        m_fitSvc->setApproximateVertex( IniVertex.x(), IniVertex.y(), IniVertex.z() );

        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( ListBaseTracks,
            dummyNeutrals,
            FitVertex, Momentum, Charge,
            ErrorMatrix, Chi2PerTrk,
            TrkAtVrt, Chi2  );

        if( sc.isFailure() )  continue;          /* No fit */ 

        // Compatibility to the primary vertex.
        Amg::Vector3D vDist = FitVertex - m_thePV->position();
        double vPos=(vDist.x()*Momentum.Px()+vDist.y()*Momentum.Py()+vDist.z()*Momentum.Pz())/Momentum.Rho();
        
        if( m_FillNtuple ) {
          // Fill the 2-track vertex properties to AANT
          m_ntupleVars->get<unsigned int>( "All2TrkVrtNum" )++;
          m_ntupleVars->get< vector<double> >( "All2TrkVrtMass" )   .emplace_back(Momentum.M());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtPt" )     .emplace_back(Momentum.Perp());
          m_ntupleVars->get< vector<int> >   ( "All2TrkVrtCharge" ) .emplace_back(Charge);
          m_ntupleVars->get< vector<double> >( "All2TrkVrtX" )      .emplace_back(FitVertex.x());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtY" )      .emplace_back(FitVertex.y());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtZ" )      .emplace_back(FitVertex.z());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtChiSq" )  .emplace_back(Chi2);
        }


        // Create a xAOD::Vertex instance
        xAOD::Vertex *vertex = new xAOD::Vertex;
        m_twoTrksVertexContainer->emplace_back( vertex );

        for( auto *trk: ListBaseTracks ) {

          // Acquire link to the track
          ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *selectedBaseTracks, trk->index() );

          // Register link to the vertex
          vertex->addTrackAtVertex( trackElementLink, 1. );
        }

        vertex->setVertexType( xAOD::VxType::SecVtx );
        vertex->setPosition( FitVertex );
        vertex->setFitQuality( Chi2, 1 ); // Ndof is always 1

        vertex->auxdata<float>("mass")   = Momentum.M();
        vertex->auxdata<float>("pT")     = Momentum.Perp();
        vertex->auxdata<float>("charge") = Charge;
        vertex->auxdata<float>("vPos")   = vPos;
        vertex->auxdata<bool>("isFake")  = true;


        /////////////////////////////

        uint8_t trkiBLHit,trkjBLHit;
        if( !((*itrk)->summaryValue( trkiBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkiBLHit=0;
        if( !((*jtrk)->summaryValue( trkjBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkjBLHit=0;

        if( m_FillNtuple ) m_ntupleVars->get< vector<int> >( "All2TrkSumBLHits" ).emplace_back( trkiBLHit + trkjBLHit );

        // track chi2 cut
        if(Chi2>m_SelVrtChi2Cut)           continue;          /* Bad Chi2 */
        ATH_MSG_VERBOSE(" > extractIncompatibleTracks: 2-track vrt mass/pt "<<Momentum.M()<<","<<Momentum.Perp());


        // fake rejection cuts with track hit pattern consistencies
        if( m_removeFakeVrt && !(this->passedFakeReject( FitVertex, (*itrk), (*jtrk) )) ) continue;


        if( m_FillNtuple ) {
          // Fill AANT for vertices after fake rejection
          m_ntupleVars->get< unsigned int >( "AfFakVrtNum" )++;
          m_ntupleVars->get< vector<double> >( "AfFakVrtMass" )   .emplace_back(Momentum.M());
          m_ntupleVars->get< vector<double> >( "AfFakVrtPt" )     .emplace_back(Momentum.Perp());
          m_ntupleVars->get< vector<int> >   ( "AfFakVrtCharge" ) .emplace_back(Charge);
          m_ntupleVars->get< vector<double> >( "AfFakVrtX" )      .emplace_back(FitVertex.x());
          m_ntupleVars->get< vector<double> >( "AfFakVrtY" )      .emplace_back(FitVertex.y());
          m_ntupleVars->get< vector<double> >( "AfFakVrtZ" )      .emplace_back(FitVertex.z());
          m_ntupleVars->get< vector<double> >( "AfFakVrtChiSq" )  .emplace_back(Chi2);
        }

        // The vertex passed the quality cut: overwrite isFake to false
        vertex->auxdata<bool>("isFake")  = false;


        if(vPos<-20.) continue;

        // Removefrom the incompatible track list only after passing all criteria
        Incomp.pop_back();
        Incomp.pop_back();
      }
    }

    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reconstruct2TrackVertices( std::vector<int>& Incomp, std::vector<WrkVrt> *WrkVrtSet )
  {
    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): begin");

    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    // Graph method: Trk::pgraphm_()
    // used in order to find compatible sub-graphs from the incompatible graph

    // set of vertices created in the following while loop.
    //vector<WrkVrt> *WrkVrtSet= new vector<WrkVrt>;

    // List of edgeds between imcompatible nodes
    long int* weit     = new long int[ Incomp.size() ];
    for(size_t i=0; i<Incomp.size(); i++) {
      weit[i] = static_cast<long int>( Incomp[i]+ 1 ); /* +1 is needed for PGRAPH*/
    }

    // Solution of the graph method routine (minimal covering of the graph)
    // The size of the solution is returned by NPTR (see below)
    long int* Solution = new long int[ selectedBaseTracks->size() ];

    // Number of edges in the list
    // This is two-folded (a pair of incompatible tracks composes an edge).
    long int edges = Incomp.size()/2; 

    // input number of nodes in the graph.
    long int NTracks = static_cast<long int>( selectedBaseTracks->size() );

    // Input variable; the threshold. Solutions shorter than nth are not returned (ignored).
    long int nth=2;    //VK some speed up

    // NPTR: I/O variable (Destructive FORTRAN Style!!!)
    // - on input:   =0 for initialization, >0 to get next solution
    // - on output:  >0 : length of the solution stored in set; =0 : no more solutions can be found
    long int NPTR=0;

    // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()
    vector<const xAOD::TrackParticle*>    ListBaseTracks;
    vector<const xAOD::NeutralParticle*>  dummyNeutrals(0);

    m_fitSvc->setDefault();

    // Main iteration
    while(true) {

      // varaible of new vertex
      WrkVrt newvrt;

      // Find a solution from the given set of incompatible tracks (==weit)
      Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);

      ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Trk::pgraphm_() output: NPTR = " << NPTR );

      if(NPTR <= 0)  break;      // No more solutions ==> Exit
      if(NPTR == 1)  continue;   // i.e. single node  ==> Not a good solution

      ListBaseTracks.clear();

      // Try to compose a new vertex using the solution nodes
      // Here the track ID is labelled with array
      newvrt.Good = true;
      newvrt.SelTrk.clear();
      for(int i=0; i<NPTR; i++) {
        newvrt.SelTrk.emplace_back(Solution[i]-1);
        ListBaseTracks.emplace_back( selectedBaseTracks->at(Solution[i]-1) );
      }

      // Perform vertex fitting
      StatusCode sc =m_fitSvc->VKalVrtFit(ListBaseTracks,dummyNeutrals,
          newvrt.vertex,
          newvrt.vertexMom,
          newvrt.Charge,
          newvrt.vertexCov,
          newvrt.Chi2PerTrk, 
          newvrt.TrkAtVrt,
          newvrt.Chi2);   

      ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): FoundAppVrt="<<NPTR<<", "<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<
          ", "<<newvrt.vertex[2]<<", "<<newvrt.Chi2);

      if( sc.isFailure() )  {
        ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): VKalVrtFit failed; continue.");
        continue;   /* Bad fit - goto next solution */
      }

      ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): VKalVrtFit succeeded; register the vertex to the list.");
      newvrt.Good         = true;
      newvrt.nCloseVrt    = 0;
      newvrt.dCloseVrt    = 1000000.;
      WrkVrtSet->emplace_back(newvrt);

    } 

    //-------------------------------------------------------
    // Iterative cleanup algorithm

    //-Remove worst track from vertices with very bad Chi2
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove worst track from vertices with very bad Chi2.");
    for(int iv=0; iv< static_cast<int>( (*WrkVrtSet).size() ); iv++) {
      if( (*WrkVrtSet)[iv].Chi2 > (4.*(*WrkVrtSet)[iv].SelTrk.size()) )
        ATH_CHECK( DisassembleVertex( WrkVrtSet, iv, selectedBaseTracks ) );
    }

    //-Remove vertices fully contained in other vertices 
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove vertices fully contained in other vertices .");
    while( (*WrkVrtSet).size() > 1 ) {
      int tmpN = static_cast<int>( (*WrkVrtSet).size() );

      int iv = 0;
      for(; iv<tmpN-1; iv++) {
        int jv = iv+1;
        for(; jv<tmpN; jv++) {
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);

          if(      nTCom==(int)(*WrkVrtSet)[iv].SelTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+iv); break; }
          else if( nTCom==(int)(*WrkVrtSet)[jv].SelTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+jv); break; }

        }
        if(jv!=tmpN)   break;  // One vertex is erased. Restart check
      }
      if(iv==tmpN-1) break;  // No vertex deleted
    }

    //-Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging)
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging).");
    for(int iv=0; iv< static_cast<int>( (*WrkVrtSet).size() ); iv++ ){
      if( (*WrkVrtSet)[iv].SelTrk.size() != 2 ) continue;
      if( (*WrkVrtSet)[iv].Chi2 > 15.) (*WrkVrtSet)[iv].Good=false;
    }

    //-Remove all bad vertices from the working set    
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove all bad vertices from the working set.");
    int tmpV=0;
    while( tmpV < static_cast<int>( (*WrkVrtSet).size() ) ) {

      if( !(*WrkVrtSet)[tmpV].Good ) { 
        (*WrkVrtSet).erase((*WrkVrtSet).begin()+tmpV);
      } else {
        tmpV++;
      }

    }

    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): Found Initial Solution Set");
    if( m_FillNtuple) m_ntupleVars->get<unsigned int>( "NumInitSecVrt" ) = WrkVrtSet->size();

    delete[] weit;
    delete[] Solution; 

    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): end.");

    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reconstructNTrackVertices( std::vector<WrkVrt> *WrkVrtSet )
  {
    
    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    //
    //  Rearrangement of solutions
    //
    const long int NTracks = static_cast<long int>( selectedBaseTracks->size() );

    // TrkInVrt has IDs of each track which can contain array of vertices.
    // e.g. TrkInVrt->at( track_id ).size() gives the number of vertices which use the track [track_id].
    vector< deque<long int> > *TrkInVrt  = new vector< deque<long int> >(NTracks);  

    // FoundMax: unit in [chi2 per track]
    double FoundMax;

    long int SelectedTrack;
    long int SelectedVertex;
    double foundMinVrtDst;
    int foundV1;
    int foundV2;

    // Fill TrkInVrt with vertex IDs of each track
    TrackClassification( WrkVrtSet, TrkInVrt );

    m_fitSvc->setDefault();
    while( (FoundMax=MaxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0 ) {
      // std::cout << "MAX="<<FoundMax<<", "<<SelectedTrack<<", "<<SelectedVertex<<'\n';
      // std::cout << "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2<<'\n';
      // printWrkSet(WrkVrtSet,"Interm");

      foundMinVrtDst = 10000000.;
      if(FoundMax<m_TrackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

      //
      //Choice of action
      if( FoundMax < m_TrackDetachCut && foundMinVrtDst < m_VertexMergeCut ) {

        MergeVertices( WrkVrtSet, foundV1, foundV2 );

        StatusCode sc = RefitVertex( WrkVrtSet->at(foundV1), selectedBaseTracks);
        if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */

        while ( minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2) < m_VertexMergeCut) {
          MergeVertices( WrkVrtSet, foundV1, foundV2);
          sc = RefitVertex( WrkVrtSet->at(foundV1), selectedBaseTracks);
          if( sc.isFailure() )  break;                            /* Bad fit - goto next solution */
        }

        delete TrkInVrt;
        TrkInVrt  = new vector< deque<long int> >(NTracks);  

        TrackClassification( WrkVrtSet, TrkInVrt);

      }else{

        RemoveTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);

        StatusCode sc = RefitVertex( WrkVrtSet->at(SelectedVertex), selectedBaseTracks);
        if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */
      }

    }

    //
    // Try to improve vertices with big Chi2
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {

      if(!(*WrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
      if( (*WrkVrtSet)[iv].SelTrk.size()<3 )      continue;

      double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*(*WrkVrtSet)[iv].SelTrk.size()-3 ); //Chi2 of the original vertex

      if(tmpProb<0.001){
        tmpProb=improveVertexChi2( WrkVrtSet, iv, selectedBaseTracks);
        if(tmpProb<0.001)(*WrkVrtSet)[iv].Good=false;
      }

    }
    //
    //-----------------------------------------------------------------------------------------------
    //
    if( m_FillNtuple ) {
      m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" )=WrkVrtSet->size();
      ATH_MSG_DEBUG(" > reconstructNTrackVertices: Size of Solution Set: "<< m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" ));
    }

    delete TrkInVrt; 

    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::refitAndSelectGoodQualityVertices( std::vector<WrkVrt> *WrkVrtSet )
  {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from WrkVrtSet to xAOD::Vertex here.
    // The supposed form of the function will be as follows:

    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );

    xAOD::TrackParticleContainer *associableTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(associableTracks, "VrtSecInclusive_AssociableParticles") );

    xAOD::VertexContainer *m_secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( m_secondaryVertexContainer, "VrtSecInclusive_SecondaryVertices" ) );
    
    // A weight parameter used in the algorithm.
    const double w_1 = 1.0;
    
    //----------------------------------------------------------

    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);

    // Loop over vertices
    for( auto WrkVrt : *WrkVrtSet ) {

      WrkVrt.Good = false;
      int nth = WrkVrt.SelTrk.size();

      if(nth <= 1) continue;               /* Bad vertices */

      StatusCode sc = RefitVertex( WrkVrt, selectedBaseTracks);
      if( sc.isFailure() )  continue;   /* Bad fit - goto next solution */


      ///////////////////////////////////////////////////////////
      // Here after only good quality vertices

      if(m_FillHist) {
        m_hb_2Ddist->Fill( sqrt( WrkVrt.vertex[0]*WrkVrt.vertex[0]
              +WrkVrt.vertex[1]*WrkVrt.vertex[1]),w_1);
        if( nth==2 && WrkVrt.Charge==0) m_hb_massPiPi->Fill(WrkVrt.vertexMom.M(),w_1);
      }

      WrkVrt.Good = true;          /* Good vertex */


      //
      //  Store good vertices into StoreGate 
      //
      if( m_FillNtuple ) m_ntupleVars->get<unsigned int>( "NumSecVrt" )++;

      std::vector <double> CovFull;
      sc = m_fitSvc->VKalGetFullCov( static_cast<long int>( nth ), CovFull); 

      if( sc.isFailure() )  ATH_MSG_VERBOSE(" here 6 ");
      

      double vert_mass=0; double vert_pt=0; double vert_pz=0;
      double vert_masse=0; //assuming tracks are electrons
      double vert_massp=0; //assuming tracks are protons
      double vx=0; double vy=0; double vz=0; double ve=0; double vee=0; double vep=0;
      int AllBLay = 1;
      int SumBLay=0;

      // Pre-check before storing vertex if the SV perigee is available
      bool good_flag = true;
      for(int itrk=0; itrk<nth; itrk++) {
        xAOD::TrackParticle* trk = selectedBaseTracks->at( WrkVrt.SelTrk[itrk] );
        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, WrkVrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_INFO(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          good_flag = false;
        }
      }
      if( !good_flag ) {
        continue;
      }
      
      std::vector<xAOD::TrackParticle*> tracks;
      for( const auto& index : WrkVrt.SelTrk )   tracks.emplace_back( selectedBaseTracks->at( index ) );
      for( const auto& index : WrkVrt.assocTrk ) tracks.emplace_back( associableTracks  ->at( index ) );

      // loop over vertex tracks
      ATH_MSG_DEBUG(" > refitAndSelectGoodQualityVertices: Track loop: size = " << nth );
      for( auto *trk : tracks ) {

        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": start." );

        track_summary trk_summary;
        fillTrackSummary( trk_summary, trk );

        if( ! (trk->hitPattern() & (1<<Trk::pixelBarrel0)) ) AllBLay = 0;

        //
        // calculate mass/pT of tracks and track parameters
        //

        double trk_phi   = trk->phi();
        double trk_theta = trk->theta();
        double trk_eta   = trk->eta();

        double trk_p  = 1.0/fabs( trk->qOverP() );
        double trk_pz = trk_p*cos(trk_theta);  
        double trk_pt = trk->pt();
        double trk_px = trk_pt*cos( trk_phi );  
        double trk_py = trk_pt*sin( trk_phi);  
        double trkp2  = trk_p*trk_p;

        double trk_e  = sqrt(trkp2 + m_pi*m_pi); 
        //double trk_ee = sqrt(trkp2 + m_e*m_e);  // not using

        //double trk_d0 = trk->d0();
        //double trk_z0 = trk->z0();

        //double trk_errd0     = trk->definingParametersCovMatrix()(Trk::d0, Trk::d0);
        //double trk_errz0     = trk->definingParametersCovMatrix()(Trk::z0, Trk::z0);
        //double trk_errQoverP = trk->definingParametersCovMatrix()(Trk::qOverP, Trk::qOverP);
        //double trk_errP      = trk_p*trk_p * trk_errQoverP;

        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": in vrt chg/px/py/pz/pt/e/phi/eta = "
            << trk->charge() <<","
            <<trk_px<<","
            <<trk_py<<","
            <<trk_pz<<","
            <<trk_pt<<","
            <<trk_e<<","
            <<trk_phi<<","
            <<trk_eta);

        /////////////////////////////////////////////
        // Get the perigee of the track at the vertex
        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Get the prigee of the track at the vertex." );

        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, WrkVrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_WARNING(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );

          trk->auxdata<float>( "pt_wrtSV" )     = -9999.;
          trk->auxdata<float>( "eta_wrtSV" )    = -9999.;
          trk->auxdata<float>( "phi_wrtSV" )    = -9999.;
          trk->auxdata<float>( "d0_wrtSV" )     = -9999.;
          trk->auxdata<float>( "z0_wrtSV" )     = -9999.;
          trk->auxdata<float>( "errP_wrtSV" )   = -9999.;
          trk->auxdata<float>( "errd0_wrtSV" )  = -9999.;
          trk->auxdata<float>( "errz0_wrtSV" )  = -9999.;
          continue;
        }

        double qOverP_wrtSV    = sv_perigee->parameters() [Trk::qOverP];
        double theta_wrtSV     = sv_perigee->parameters() [Trk::theta];
        double p_wrtSV         = 1.0 / fabs( qOverP_wrtSV );
        double pt_wrtSV        = p_wrtSV * sin( theta_wrtSV );
        double eta_wrtSV       = -log( tan( theta_wrtSV/2. ) );
        double phi_wrtSV       = sv_perigee->parameters() [Trk::phi];
        double d0_wrtSV        = sv_perigee->parameters() [Trk::d0];
        double z0_wrtSV        = sv_perigee->parameters() [Trk::z0];
        double errd0_wrtSV     = (*sv_perigee->covariance())( Trk::d0 );
        double errz0_wrtSV     = (*sv_perigee->covariance())( Trk::z0 );
        //double errQoverP_wrtSV = (*sv_perigee->covariance())( Trk::qOverP );
        double errP_wrtSV      = (*sv_perigee->covariance())( Trk::qOverP );

        // xAOD::Track augmentation
        trk->auxdata<float>( "pt_wrtSV" )     = pt_wrtSV;
        trk->auxdata<float>( "eta_wrtSV" )    = eta_wrtSV;
        trk->auxdata<float>( "phi_wrtSV" )    = phi_wrtSV;
        trk->auxdata<float>( "d0_wrtSV" )     = d0_wrtSV;
        trk->auxdata<float>( "z0_wrtSV" )     = z0_wrtSV;
        trk->auxdata<float>( "errP_wrtSV" )   = errP_wrtSV;
        trk->auxdata<float>( "errd0_wrtSV" )  = errd0_wrtSV;
        trk->auxdata<float>( "errz0_wrtSV" )  = errz0_wrtSV;


        vert_pt += pt_wrtSV;
        vert_pz += p_wrtSV * cos(theta_wrtSV);
        vx      += p_wrtSV * sin(theta_wrtSV) * cos(phi_wrtSV);
        vy      += p_wrtSV * sin(theta_wrtSV) * sin(phi_wrtSV);
        vz      += p_wrtSV * cos(theta_wrtSV);
        ve      += sqrt( p_wrtSV*p_wrtSV + m_pi*m_pi );
        vee     += sqrt( p_wrtSV*p_wrtSV + m_e*m_e );
        vep     += sqrt( p_wrtSV*p_wrtSV + m_proton*m_proton );

        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": end." );
      } // loop over tracks in vertex

      ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: Track loop end. ");

      //nVrtVx->emplace_back(tmpVx);

      // Make vertex mass
      vert_mass  = sqrt(ve*ve - vx*vx -vy*vy -vz*vz);
      vert_masse = sqrt(vee*vee - vx*vx -vy*vy -vz*vz);
      vert_massp = sqrt(vep*vep - vx*vx -vy*vy -vz*vz);


      ATH_MSG_DEBUG(" > refitAndSelectGoodQualityVertices: Final Sec.Vertex="<<nth<<", "
          <<WrkVrt.vertex[0]<<", "<<WrkVrt.vertex[1]<<", "
          <<WrkVrt.vertex[2]<<","<<vert_mass<<","<<vert_pt<<","<<vert_masse);


      //
      // calculate opening angle between all 2-track pairs, and store the minimum
      //
      double minOpAng = 1000;
      std::vector<double> opAngles;
      
      for( auto itr1 = tracks.begin(); itr1 != tracks.end(); ++itr1 ) {
        for( auto itr2 = std::next( itr1 ); itr2 != tracks.end(); ++itr2 ) {
          const auto& p1 = (*itr1)->p4().Vect();
          const auto& p2 = (*itr2)->p4().Vect();
          auto cos = p1 * p2 / p1.Mag() / p2.Mag();
          opAngles.emplace_back( cos );
        }
      }
      minOpAng = *( std::max_element( opAngles.begin(), opAngles.end() ) );
      if( m_FillNtuple ) m_ntupleVars->get< vector<double> >( "SecVtx_MinOpAng" ).emplace_back(minOpAng);
        
      /*
      for (int it1=0; it1<nth; ++it1) {
        if(it1>=10) continue;
        p1 = sqrt(trkpx[it1]*trkpx[it1] + trkpy[it1]*trkpy[it1] + trkpz[it1]*trkpz[it1]);
        for (int it2=0; it2<nth; ++it2) {
          if(it2>=10) continue;
          if(it1==it2) continue;
          //
          p2 = sqrt(trkpx[it2]*trkpx[it2] + trkpy[it2]*trkpy[it2] + trkpz[it2]*trkpz[it2]);
          opAng = trkpx[it1]*trkpx[it2]+trkpy[it1]*trkpy[it2]+trkpz[it1]*trkpz[it2];
          opAng = (opAng/p1)/p2;
          ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: Final Sec Vtx Opening Ang/ minOpAng "<<opAng<<","<<minOpAng);
          if(opAng < minOpAng) minOpAng = opAng;
        }
      }
      if( m_FillNtuple ) m_ntupleVars->get< vector<double> >( "SecVtx_MinOpAng" ).emplace_back(minOpAng);
      */



      ///////////////////////////////////////////////////
      // Data filling to xAOD container

      // Firstly store the new vertex to the container before filling properties.
      // (This is the feature of xAOD.)
      xAOD::Vertex* vertex = new xAOD::Vertex;
      m_secondaryVertexContainer->emplace_back( vertex );

      // Registering the vertex position to xAOD::Vertex
      vertex->setPosition( WrkVrt.vertex );

      // Registering the vertex type: SV
      vertex->setVertexType( xAOD::VxType::SecVtx );

      // Registering the vertex chi2 and Ndof
      int ndof = 2*( WrkVrt.SelTrk.size() + WrkVrt.assocTrk.size() ) - 3;
      vertex->setFitQuality( WrkVrt.Chi2, ndof );

      // Registering the vertex covariance matrix
      std::vector<float> fCov(WrkVrt.vertexCov.cbegin(), WrkVrt.vertexCov.cend());
      vertex->setCovariance(fCov);

      // Registering the vertex momentum and charge
      vertex->auxdata<float>("vtx_px")		= WrkVrt.vertexMom.Px();
      vertex->auxdata<float>("vtx_py")		= WrkVrt.vertexMom.Py();
      vertex->auxdata<float>("vtx_pz")		= WrkVrt.vertexMom.Pz();

      vertex->auxdata<float>("vtx_mass")	= WrkVrt.vertexMom.M();
      vertex->auxdata<float>("vtx_charge")	= WrkVrt.Charge;

      // Other SV properties
      vertex->auxdata<float>("mass")		= vert_mass;
      vertex->auxdata<float>("mass_e")		= vert_masse;
      vertex->auxdata<float>("mass_proton")	= vert_massp;
      vertex->auxdata<float>("pT")		= vert_pt;
      vertex->auxdata<float>("pz")		= vert_pz;
      vertex->auxdata<float>("sumBLayHits")     = SumBLay;
      vertex->auxdata<float>("allTrksBLayHits") = AllBLay;
      vertex->auxdata<float>("minOpAng")        = minOpAng;
      vertex->auxdata<float>("num_trks")        = WrkVrt.SelTrk.size();
      vertex->auxdata<float>("dCloseVrt")       = WrkVrt.dCloseVrt;

      // Registering tracks comprising the vertex to xAOD::Vertex
      // loop over the tracks comprising the vertex
      for( auto trk_id : WrkVrt.SelTrk ) {

        const xAOD::TrackParticle *trk = selectedBaseTracks->at( trk_id );

        // Acquire link the track to the vertex
        ElementLink<xAOD::TrackParticleContainer> link_trk( *selectedBaseTracks, static_cast<long unsigned int>(trk->index()) );

        // Register the link to the vertex
        vertex->addTrackAtVertex( link_trk, 1. );

      }
      
      for( auto trk_id : WrkVrt.assocTrk ) {

        const xAOD::TrackParticle *trk = associableTracks->at( trk_id );

        // Acquire link the track to the vertex
        ElementLink<xAOD::TrackParticleContainer> link_trk( *associableTracks, static_cast<long unsigned int>(trk->index()) );

        // Register the link to the vertex
        vertex->addTrackAtVertex( link_trk, 1. );

      }

      
      if( m_doMapToLocal ) {
        // Obtain the local mapping of the reconstructed vertex
        Trk::MappedVertex mappedVtx = m_vertexMapper->mapToLocal( WrkVrt.vertex );
        if( mappedVtx.valid ) {
          vertex->auxdata<int>("local_identifierHash") = mappedVtx.identifierHash;
          vertex->auxdata<int>("local_layerIndex")     = mappedVtx.layerIndex;
          vertex->auxdata<float>("local_posX")         = mappedVtx.localPosition.x();
          vertex->auxdata<float>("local_posY")         = mappedVtx.localPosition.y();
          vertex->auxdata<float>("local_posZ")         = mappedVtx.localPosition.z();
        } else {
          vertex->auxdata<int>("local_identifierHash") = -9999;
          vertex->auxdata<int>("local_layerIndex")     = -9999;
          vertex->auxdata<float>("local_posX")         = -9999.;
          vertex->auxdata<float>("local_posY")         = -9999.;
          vertex->auxdata<float>("local_posZ")         = -9999.;
        }
      }


      // For MC, try to trace down to the truth particles,
      // and depending on the topology, categorize the label of the reconstructed vertex.
      if( m_doTruth ) {
        ATH_CHECK( categorizeVertexTruthTopology( vertex ) );
      }

    } // loop over vertices


    if( m_FillNtuple ) {
      ATH_CHECK( fillAANT_SecondaryVertices( m_secondaryVertexContainer ) );
    }

    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reassembleVertices( std::vector<WrkVrt>* workVerticesContainer )
  {
    // Here, the supposed issue is that, the position of the reconstructed vertex may be significantly
    // displaced from its truth position, even if the constituent tracks are all from that truth.
    // The fundamental reason of this is speculated that the VKalVrt vertex fitting could fall in
    // a local minimum. This function attempts to improve the situation, given that N-track vertices
    // are already reconstructed, by attempting to asociate a track of a small multiplicity vertex
    // to another large multiplicity vertex.
    
    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    // First, sort WrkVrt by the track multiplicity
    std::sort( workVerticesContainer->begin(), workVerticesContainer->end(), [](WrkVrt& v1, WrkVrt& v2) { return v1.SelTrk.size() < v2.SelTrk.size(); } );
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": #vertices = " << workVerticesContainer->size() );
    // Loop over vertices (small -> large Ntrk order)
    for( auto& workVertex : *workVerticesContainer ) {
      if( !workVertex.Good               ) continue;
      if(  workVertex.SelTrk.size() <= 1 ) continue;
      
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": vertex " << &workVertex << " #tracks = " << workVertex.SelTrk.size() );
      std::map<unsigned, std::vector<WrkVrt>::reverse_iterator> mergiableVertex;
      std::set<std::vector<WrkVrt>::reverse_iterator> mergiableVerticesSet;
      
      for( auto& index : workVertex.SelTrk ) {
        
        const xAOD::TrackParticle* trk = selectedBaseTracks->at( index );
        
        mergiableVertex[index] = workVerticesContainer->rend();
        
        std::vector<double> distances;
        
        // Reverse iteration: large Ntrk -> small Ntrk order
        for( auto ritr = workVerticesContainer->rbegin(); ritr != workVerticesContainer->rend(); ++ritr ) {
          auto& vertexToAttach = *ritr;
          
          if( &workVertex == &vertexToAttach ) continue;
          if( workVertex.SelTrk.size() >= vertexToAttach.SelTrk.size() ) continue;
          if( !checkTrackHitPatternToVertex( trk, vertexToAttach.vertex ) ) continue;
          
          // Get the closest approach
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
        
          m_fitSvc->VKalGetImpact(trk, vertexToAttach.vertex, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
          
          const auto& distance = hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) );
          distances.emplace_back( distance );
          
          if( distance > 1.0 ) continue;
          
          mergiableVertex[index] = ritr;
          mergiableVerticesSet.emplace( ritr );
          
        }
        
        auto min_distance = distances.size() > 0 ? *(std::min_element( distances.begin(), distances.end() )) : 99999.;
        
        if( mergiableVertex[index] == workVerticesContainer->rend() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": track " << trk << " --> none : min distance = " << min_distance );
        } else {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": track " << trk << " --> " << &( *(mergiableVertex[index]) ) << " --> size = " << mergiableVertex[index]->SelTrk.size() << ": min distance = " << min_distance );
        }
        
      }
      
      size_t count_mergiable = std::count_if( mergiableVertex.begin(), mergiableVertex.end(),
                                               [&](std::pair<unsigned, std::vector<WrkVrt>::reverse_iterator> p ) {
                                                 return p.second != workVerticesContainer->rend(); } );
      
      if( mergiableVerticesSet.size() == 1 && count_mergiable == workVertex.SelTrk.size() ) {
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": identified a unique association destination vertex" );
        
        WrkVrt& destination = *( mergiableVertex.begin()->second );
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": destination #tracks before merging = " << destination.SelTrk.size() );
        
        MergeVertices( destination, workVertex );
        StatusCode sc = RefitVertex( destination, selectedBaseTracks );
        if( sc.isFailure() ) {}
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": destination #tracks after merging = " << destination.SelTrk.size() );
        
      }
          
    }
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::associateNonSelectedTracks( std::vector<WrkVrt>* workVerticesContainer )
  {
    
    const xAOD::TrackParticleContainer *allTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(allTracks, "InDetTrackParticles") );
    
    xAOD::TrackParticleContainer *selectedTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    xAOD::TrackParticleContainer *associableTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(associableTracks, "VrtSecInclusive_AssociableParticles") );
    
    const xAOD::VertexContainer *pvs (nullptr);
    ATH_CHECK( evtStore()->retrieve( pvs, "PrimaryVertices") );
    
    ATH_MSG_DEBUG( " >> associateNonSeletedTracks: #verticess = " << workVerticesContainer->size() );
    
    // Loop over vertices
    for( auto& workVertex : *workVerticesContainer ) {
      
      if( !workVertex.Good               ) continue;
      if(  workVertex.SelTrk.size() <= 1 ) continue;
      
      auto& vertexPos = workVertex.vertex;
      
      
      std::vector<double> distanceToPVs;
      
      for( auto* pv : *pvs ) {
        distanceToPVs.emplace_back( VKalVrtAthena::vtxVtxDistance( vertexPos, pv->position() ) );
      }
      const auto& minDistance = *( std::min_element( distanceToPVs.begin(), distanceToPVs.end() ) );
      
      if( minDistance < 1.0 ) continue;
      
      
      ATH_MSG_DEBUG( " >> associateNonSeletedTracks: vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << "), "
                     "#selected = " << workVertex.SelTrk.size() << ", #assoc = " << workVertex.assocTrk.size() );
      
      std::vector<const xAOD::TrackParticle*> candidates;
      
      // Search for candidate tracks
      for( auto itr = allTracks->begin(); itr != allTracks->end(); ++itr ) {
        auto* trk = *itr;
        
        {
          auto result = std::find_if( selectedTracks->begin(), selectedTracks->end(),
                                      [&] (xAOD::TrackParticle* strk) { return trk->index() == strk->auxdata<unsigned long>("trk_id"); } );
          if( result != selectedTracks->end() ) continue;
        }
        
        {
          auto result = std::find_if( associableTracks->begin(), associableTracks->end(),
                                      [&] (xAOD::TrackParticle* atrk) { return trk->index() == atrk->auxdata<unsigned long>("trk_id"); } );
          if( result != associableTracks->end() ) continue;
        }
        
        if( !checkTrackHitPatternToVertex( trk, vertexPos ) ) continue;
        
        // Get the closest approach
        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        m_fitSvc->VKalGetImpact(trk, vertexPos, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
        
        enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
        
        if( hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) ) > 1.0 ) continue;
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: trk " << trk
                       << ": d0 to vtx = " << impactParameters.at(k_d0)
                       << ", z0 to vtx = " << impactParameters.at(k_z0)
                       << ", distance to vtx = " << hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) ) );
        
        candidates.emplace_back( trk );
        
      }
      
      ATH_MSG_DEBUG( " >> associateNonSeletedTracks: number of candidate tracks = " << candidates.size() );
      
      // Attempt to add the track to the vertex and try fitting
      for( const auto* trk : candidates ) {
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: attempting to associate track = " << trk );
        
        m_fitSvc->setDefault();
        m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
        
        std::vector<const xAOD::TrackParticle*>   baseTracks;
        std::vector<const xAOD::NeutralParticle*> dummyNeutrals;
        
        workVertex.Chi2PerTrk.clear();
        
        for( const auto& index : workVertex.SelTrk ) {
          baseTracks.emplace_back( selectedTracks->at( index ) );
          workVertex.Chi2PerTrk.emplace_back( 100. );
        }
        
        baseTracks.emplace_back( trk );
        workVertex.Chi2PerTrk.emplace_back( 100. );
        
        Amg::Vector3D initPos;
        
        {
          StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initPos );/* Fast crude estimation */
        
          if( sc.isFailure() ) ATH_MSG_DEBUG(" >> associateNonSeletedTracks: fast crude estimation failed.");
        
          const auto& diffPos = initPos = vertexPos;
        
          if( diffPos.x()*diffPos.x() + diffPos.y()*diffPos.y() + diffPos.z()*diffPos.z() > 100. ) {
        
            ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: approx vertex as original" );
            m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
          
          } else {
          
            ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: approx vertex set to (" << initPos.x() << ", " << initPos.y() << ", " << initPos.z() << ")" );
            m_fitSvc->setApproximateVertex( initPos.x(), initPos.y(), initPos.z() );
          
          }
        }
        
        
        ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: now vertex fitting..." );
        
        StatusCode sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                             workVertex.vertex,
                                             workVertex.vertexMom,
                                             workVertex.Charge,
                                             workVertex.vertexCov,
                                             workVertex.Chi2PerTrk, 
                                             workVertex.TrkAtVrt,
                                             workVertex.Chi2            );
        
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" >> associateNonSeletedTracks: VKalVrtFit failure ");
          continue;
        }
        
        auto& cov = workVertex.vertexCov;
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: succeeded in associating. New vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << ")" );
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );
        
        workVertex.assocTrk.emplace_back( associableTracks->size() );
        
        auto *a_trk = new xAOD::TrackParticle;
        associableTracks->emplace_back( a_trk );
        *a_trk = *trk;
        a_trk->auxdata<unsigned long>("trk_id")  = trk->index();
        a_trk->auxdata<bool>("is_associated")  = true;
        
      }

    }
    
    return StatusCode::SUCCESS;
  }
    
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeFinalVertices( std::vector<WrkVrt> *WrkVrtSet )
  {
    
    xAOD::TrackParticleContainer *selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );

    while (true) {
      int foundV1(-9999), foundV2(-9999);
      //double VrtSecInclusive::minVrtVrtDist( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
      //{  
      //
      //  Minimal vertex-vertex distance
      //
      for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) { (*WrkVrtSet)[iv].dCloseVrt=1000000.; (*WrkVrtSet)[iv].nCloseVrt=0;}

      double foundMinVrtDst=1000000.;
      for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet).at(iv).SelTrk.size()< 2) continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)WrkVrtSet->size(); jv++) {
          if( (*WrkVrtSet).at(jv).SelTrk.size()< 2) continue;   /* Bad vertices */
          double distx = (*WrkVrtSet)[iv].vertex.x() - (*WrkVrtSet)[jv].vertex.x();
          //ATH_MSG_DEBUG( "(*WrkVrtSet)[" << iv << "].vertex.x() = " << (*WrkVrtSet)[iv].vertex.x() );
          //ATH_MSG_DEBUG( "(*WrkVrtSet)[" << iv << "].vertex[0] = " << (*WrkVrtSet)[iv].vertex[0] );
          double disty = (*WrkVrtSet)[iv].vertex.y() - (*WrkVrtSet)[jv].vertex.y();
          double distz = (*WrkVrtSet)[iv].vertex.z() - (*WrkVrtSet)[jv].vertex.z();
          //double tmp = fabs(distx) + fabs(disty) + fabs(distz);
          //if(tmp > 15.) continue;
          double tmpDst = sqrt(distx*distx + disty*disty + distz*distz);
          if(tmpDst > 15.) continue;
          if( tmpDst < foundMinVrtDst){foundMinVrtDst = tmpDst; foundV1=iv; foundV2=jv;} 
          if( tmpDst < (*WrkVrtSet)[iv].dCloseVrt ) {(*WrkVrtSet)[iv].dCloseVrt=tmpDst; (*WrkVrtSet)[iv].nCloseVrt=jv;}
          if( tmpDst < (*WrkVrtSet)[jv].dCloseVrt ) {(*WrkVrtSet)[jv].dCloseVrt=tmpDst; (*WrkVrtSet)[jv].nCloseVrt=iv;}
        }
      }
      //  return foundMinVrtDst;
      //}
      
      if( foundV1 < 0 or foundV2 < 0 ) break;
      
      if( foundMinVrtDst >  m_VertexMergeFinalDistCut ) {
        ATH_MSG_DEBUG( "Vertices " << foundV1 << " and " << foundV2
            <<" are separated by distance " << foundMinVrtDst );
        return StatusCode::SUCCESS;
      }

      if( foundMinVrtDst < m_VertexMergeFinalDistCut) {
        ATH_MSG_DEBUG( "Merging FINAL vertices " << foundV1 << " and " << foundV2
            <<" which are separated by distance "<< foundMinVrtDst );
        MergeVertices( WrkVrtSet, foundV1, foundV2 );
        StatusCode sc = RefitVertex( WrkVrtSet->at(foundV1), selectedBaseTracks);
        if( sc.isFailure() ) {
          //ATH_MSG_WARNING("Final merging vertex refit failed!!!!!");
          //return StatusCode::FAILURE;
          continue;
          //break;                            /* Bad fit - goto next solution */
        }
      } // end if foundMinVrtDst < m_VertexMergeFinalDistCut
    } // end of while statement

    return StatusCode::SUCCESS;

  } // end of mergeFinalVertices

} // end of namespace VKalVrtAthena
