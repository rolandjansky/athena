/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"

#include <iostream>
#include <set>
#include <tuple>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::getNewTruthInfo()
  {
    ///////////////////////////////
    // get truth info
    ///////////////////////////////

    ATH_MSG_DEBUG( "> VrtSecInclusive getNewTruthInfo()" );

    xAOD::TrackParticleContainer *m_selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(m_selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    // Truth Vertex
    const xAOD::TruthVertexContainer*   truthVertices ( nullptr );//SS has worked but test below // Hide: moved to local scope
    ATH_CHECK( evtStore()->retrieve( truthVertices, "TruthVertices") );
    
    const auto& recotrk_barcode_list = m_ntupleVars->get< vector<int> >( "RecoTrk_barcode" );
    const auto& seltrk_barcode_list  = m_ntupleVars->get< vector<int> >( "SelTrk_barcode" );

    /** Use McEventCollection container - already retrieved earlier */

    if( !m_importedFullTruthColl ) {
      ATH_MSG_WARNING( "No McEventCollection container in TDS" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "> getNewTruthInfo() : MCEventCollection Container size = " << m_importedFullTruthColl->size() );
    //
    // event primary vertex
    const xAOD::Vertex* evPV( nullptr );
    
    if( m_vertexTES ) {
      evPV = *( m_vertexTES->begin() );
      ATH_MSG_DEBUG( "> getNewTruthInfo() : In truth routine: PV x/y/z "
                     << evPV->position().x() << ","
                     << evPV->position().y() << ","
                     << evPV->position().z() );
    }
    else{
      ATH_MSG_INFO( "> getNewTruthInfo() : what happened to m_vertexTES? " << m_vertexTES );
    }
    
    // get some info about incident pion and where it interacts
    HepMC::FourVector IncPiPos;
    HepMC::FourVector PartProdVtx;
    HepMC::FourVector PartEndVtx;
    
    const Trk::TrackParameters* generatedTrackParam( nullptr );

    // for the rest try looping over vertices rather than tracks.
    //don't sc unless checking status code here 
    StatusCode sc;   
     
    bool FidCut=false; // vertex fiducial cut
    int vtxHasLifetime =0;
    int strangeBaryon  =0;
    int IncomingPID    =0;
    double pTincoming  =0.; // flags for the incident particle that makes the vertex
    double pZincoming  =0.;
    
    HepMC::FourVector PrimVrt;
    // since tracks have pT resoltion, allow for tracks to have somewhat lower momentum
    // set by m_mcTrkResolution
    
    double momLowerCut = m_TrkPtCut * ( 1 - m_mcTrkResolution ); // the 500 needs to be tied to reconstruction

    for ( auto *vtx : *truthVertices ) {


      if( vtx->barcode() == -1 ) {
        PrimVrt.setX( vtx->x() );
        PrimVrt.setY( vtx->y() );
        PrimVrt.setZ( vtx->z() );
        continue; // skip if this was the primary vertex
      }

      
      HepMC::FourVector Vpos; // = vtx->position();
      Vpos.setX( vtx->x() );
      Vpos.setY( vtx->y() );
      Vpos.setZ( vtx->z() );

      HepMC::ThreeVector Dist( PrimVrt.x()-Vpos.x(),PrimVrt.y()-Vpos.y(),PrimVrt.z()-Vpos.z() );
      if( Dist.r()<0.01 ) continue; // skip if this vertex is at the PV
      
      // if in the future, I require a cut on the SiHits or BLayerHits
      // I will have to change the logic
      //
      bool pixFid = ( Vpos.perp()<110. && fabs(Vpos.z())<620. ); // vtx within pixels
      bool sctFid = ( Vpos.perp()<500. && fabs(Vpos.z())<2500. ); // vtx within SCT
      if( m_CutPixelHits>0 ) FidCut = pixFid; 
      if( m_CutSctHits>0 ) FidCut   = sctFid;
      // now get barcodes of incident particle and outgoing particles
      //
      
      if( !FidCut ) continue; //skip if vertex is out of the pixel/sct fiducial region

      ATH_MSG_DEBUG( "> getNewTruthInfo() : barcode of vert/#partIn/Out "
                     <<  vtx->barcode()  <<  ","
                     <<  vtx->nIncomingParticles()  <<  ","
                     <<  vtx->nOutgoingParticles() );
      
      ATH_MSG_DEBUG( "> getNewTruthInfo() : XYZ of this vert/FidCut/Dist "
                     <<  Vpos.x()  <<  ","
                     <<  Vpos.y()  <<  ","
                     <<  Vpos.z()  <<  ","
                     <<  FidCut    <<  ","
                     <<  Dist.r() );

      

      // tag this SV if the incoming particle has lifetime
      // check for Ks, KL, strange baryons, charm meson/baryon, B meson/baryon

      for ( unsigned int iPart=0; iPart<vtx->nIncomingParticles(); ++iPart ) { 
	  const xAOD::TruthParticle* inP = vtx->incomingParticle( iPart ); 

	  vtxHasLifetime=0;
	  strangeBaryon=0;
	  //for( ; inP!=inPend; ++inP ) {        
	  //int pid = ( *inP)->pdg_id();
	  int pid = inP->pdgId();
	  int abspid = abs( pid );
	  IncomingPID = pid;
	  if( abspid==22 )      vtxHasLifetime=1;   // photon
	  if( abspid==211 )     vtxHasLifetime=2;   // pi+-
	  if( abspid==2212 )    vtxHasLifetime=12;  // proton
	  if( abspid==15 )      vtxHasLifetime=16;  // tau
	  if( abspid==2112)     vtxHasLifetime=17;  // (anti- )neutron
	  if( abspid==310 )     vtxHasLifetime=3;   // Ks
	  if( abspid==130 )     vtxHasLifetime=-3;  // K-long
	  if( abspid/100 ==3 && (abspid!=310 && abspid!= 130) ){
	    if( abspid==321 )   vtxHasLifetime=-13; // K+-
	    else              vtxHasLifetime=-16; // other strange mesons
	  }
	  if( abspid/1000==3 )  vtxHasLifetime=13;  // Strange baryons
	  if( abspid/100==4 )   vtxHasLifetime=4;   // charm mesons & baryons
	  if( abspid/1000==4 )  vtxHasLifetime=14;
	  if( abspid/100==5 )   vtxHasLifetime=5;   // b mesons & baryons
	  if( abspid/1000==5 )  vtxHasLifetime=15;
	  if(                  vtxHasLifetime==13 ) strangeBaryon=pid;
	  if( abspid==1000022 ) vtxHasLifetime=19;  // neutralino


	  //HepMC::FourVector iVec = ( *inP)->momentum();
	  HepMC::FourVector iVec;// = ( *inP)->p4(); 
	  iVec.setX( inP->px() );
	  iVec.setY( inP->py() );
	  iVec.setZ( inP->pz() );
	  iVec.setZ( inP->m() );
	  pZincoming = iVec.pz();
	  pTincoming = iVec.perp();
	

      } //end iPart loop
      
      
      ///////////////compiles and runs so far/////////////////


      // now get particles emerging from this vertex
      //this compiles and runs, but probably not correct since outP has to make sense too, ss

      int numOutP     =0;
      int numOutPLoPt =0; // useless variable. will comment out later.
      double sumTrkpt =0;
      double sumTrkpz =0;
      int sumChg      =0;
      int maxTrks     =10;
      double Trkpt[10]; // array size is same as maxTrks
      double Trkpx[10];
      double Trkpy[10];
      double Trkpz[10];
      int numReco     =0;
      int bcodeOfRecoTrks[10];
      
      for ( int i=0; i<maxTrks; ++i ) {
          Trkpt[i]=0;
          Trkpx[i]=0;
          Trkpy[i]=0;
          Trkpz[i]=0;
          bcodeOfRecoTrks[i]=-1;
      }

      int nOutgoing = vtx->nOutgoingParticles();
      for ( int iPart=0; iPart<nOutgoing; ++iPart )
	{
	  const xAOD::TruthParticle* outP = vtx->outgoingParticle( iPart );

	  // loop over outgoing particles
	  int id = outP->pdgId();
	  int absid = abs( id );

	  // only continue for charged stable particles
	  bool isTrack = ( absid==211 || absid==321 || absid==2212 || absid==11 || absid==13 );
	  isTrack = isTrack || absid == 1000010020; // add deuteron
	  if( !isTrack ) continue;
	  //
	  ATH_MSG_VERBOSE( "> getNewTruthInfo() : out track pid "  <<  absid );


	  double length=m_TruthTrkLen;
	  if ( iPart==nOutgoing )
	    {
	      //endv_barcode = endv->barcode();

	      //HepMC::FourVector end = endv->position();
	      HepMC::FourVector end; 
	      end.setX( vtx->x() );
	      end.setY( vtx->y() );
	      end.setZ( vtx->z() );
	      HepMC::ThreeVector len( Vpos.x()-end.x(),Vpos.y()-end.y(),Vpos.z()-end.z() );

	      length = len.r();

	    } //end vtx

	  ///////////////////compiles and runs so far//////////////////////////////
 
	  HepMC::FourVector pVec;// = outP->momentum();
	  pVec.setPx( outP->px() );
	  pVec.setPy( outP->py() );
	  pVec.setPz( outP->pz() );
	  //pVec.setM( outP->m() );


	  double tanth = pVec.perp()/pVec.pz();
	  double theta = atan( tanth );
	  theta += m_PI; // atan returns between -PI/2 and PI/2, whereas we want it between 0 and PI
	  if( theta<0 ) theta += m_PI;
	  if( theta>m_PI ) theta -= m_PI;
	  double peta = -1*log( tan(theta/2.) );
	  //
	  // look for matching reco trk
	  //

	  ///////////////compiles and runs so far ///////////////////////////////
        
          if( m_FillNtuple ) {
            for ( unsigned int ilist = 0; ilist<(recotrk_barcode_list.size()); ++ilist ) {
              
              ATH_MSG_VERBOSE( "> getNewTruthInfo() : ilist/m_RecoTrk/Truth_bcode "
                               <<  ilist  <<  ","
                               <<  recotrk_barcode_list.at( ilist )  <<  ","
                               <<  outP->barcode() );
              
              if(  recotrk_barcode_list.at(ilist) == (outP->barcode()) ) {
                numReco++;
                if( numReco<10) bcodeOfRecoTrks[numReco-1]= recotrk_barcode_list.at(ilist );
                ATH_MSG_VERBOSE( "> getNewTruthInfo() : ---- numReco/bcodeOf/ilist/m_RecoTrk "
                                 <<  numReco  <<  ","
                                 <<  bcodeOfRecoTrks[numReco-1]  <<  ","
                                 <<  ilist  <<  ","
                                 <<  recotrk_barcode_list.at( ilist) );
                break;
              }
            }
          }

	  //
	  // generate parameters of genParticle relative to ( 0,0,0 )
	  bool genPerigee=false;
	  if( m_extrapPV) generatedTrackParam = m_truthToTrack->makeProdVertexParameters(outP );
	  else generatedTrackParam = m_truthToTrack->makePerigeeParameters( outP );

	  //
	  double tempd0    = -9999.;
          double tempz0    = -9999.;
	  double tempp     =-10000.;
          double tempphi   =-10000;
	  double temptheta =-10000;
          double temppt    =-10000;
	  double tempeta   =-10000;
	  double temppz    =-10000;
	  //
	  if( generatedTrackParam ) {
	    genPerigee=true;
	    tempd0    = generatedTrackParam->parameters()[Trk::d0];
	    tempz0    = generatedTrackParam->parameters()[Trk::z0];
	    tempphi   = generatedTrackParam->parameters()[Trk::phi];
	    tempp     = generatedTrackParam->parameters()[Trk::qOverP];
	    temptheta = generatedTrackParam->parameters()[Trk::theta];
	    //
	    double inv=( 1./tempp );
	    temppt = inv*sin( temptheta );
	    temppz = sqrt( inv*inv - temppt*temppt );
	    tempeta = -1.*log(  tan(temptheta/2) );
	  }


	  double abstemppt=fabs( temppt );
	  if( abstemppt/1000.>200.) ATH_MSG_INFO("> getNewTruthInfo() : pt@0 is huge, barcode/pdgid " <<  abstemppt <<  "," <<  outP->barcode() <<  "," <<  absid );
	  // make cuts on tracks
	  //
	  bool etaFid = fabs( tempeta )<2.5;
	  bool momCut = abstemppt > momLowerCut &&(  abstemppt/1000<10000. ); //pathalogical
	  bool longTrk = length>= m_TruthTrkLen; // track must have length >= m_TruthTrkLen
	  // choose good tracks
	  ATH_MSG_DEBUG( "> getNewTruthInfo() : rad/ZSV, pid/bc/genPerigee,pt@Vrt/pt@0/pz@0/eta@vrt/eta@0/2dIP@0/zIP@0 "
			 <<  Vpos.perp()      <<  ","
			 <<  Vpos.z()         <<  ","
			 <<  absid            <<  ","
			 <<  outP->barcode()  <<  ","
			 <<  genPerigee       <<  ","
			 <<  pVec.perp()      <<  ","
			 <<  temppt           <<  ","
			 <<  temppz           <<  ","
			 <<  peta             <<  ","
			 <<  tempeta          <<  ","
			 <<  tempd0           <<  ","
			 <<  tempz0);
	  
	  if( longTrk && isTrack && etaFid && momCut ) {
	    //
	    if( !genPerigee) ATH_MSG_INFO("> getNewTruthInfo() : no Perigee " <<  outP->barcode() );
	    //
	    // add variables for each track into ntuple
	    // Add all vertices to same ntuple, and extract later in root macro 
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrk2dIP" ) .emplace_back(tempd0 ); // these five variables 
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkZIP" )  .emplace_back(tempz0 ); // are measured 
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkPt" )   .emplace_back(temppt );  // relative to 
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkEta" )  .emplace_back(tempeta ); 
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkPhi" )  .emplace_back(tempphi); // (0,0,0 )

	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkR" )    .emplace_back(Vpos.perp() );
	    m_ntupleVars->get< vector<double> >( "Truth_AllTrkZ" )    .emplace_back(Vpos.z() );
	    m_ntupleVars->get< vector<int> >   ( "Truth_AllTrkBC" )   .emplace_back( (outP->barcode()) );
	    //
	    numOutP++;
	    sumTrkpt += abstemppt; // these two are at ( 0,0,0 )
	    sumTrkpz += temppz;
	    if( numOutP<=maxTrks ) {
	      Trkpt[numOutP-1]=abstemppt; // @( 0,0,0 )
	      Trkpx[numOutP-1]=abstemppt*cos( tempphi ); 
	      Trkpy[numOutP-1]=abstemppt*sin( tempphi );
	      Trkpz[numOutP-1]=temppz;
	    }
	    
	    ATH_MSG_DEBUG( "> getNewTruthInfo() : After Cuts: Out part bc/id/length/numOutP/sumTrkpz/sumTrkpt "
			    <<  outP->barcode()  <<  ","  <<  outP->pdgId()  <<  ","  <<  length  <<  ","  <<  numOutP  <<  ","  <<  sumTrkpt  <<  ","  <<  sumTrkpz );
	    
	    //
	    int trkchg=1;
	    if( id<0 ) trkchg=-1; // pi-/K-/anti-p have id<0
	    if( id == -11 || id == -13 ) trkchg=1; // however, e+ and mu+ have id<0
	    sumChg += trkchg;
	  }


	  if( longTrk && isTrack && etaFid && abstemppt> m_LoPtCut ) numOutPLoPt++;
 
	} // loop over particles in vertex
      
      
      ATH_MSG_DEBUG( "> getNewTruthInfo() : # outParticles passing cuts (High Pt) " <<  numOutP );
      ATH_MSG_DEBUG( "> getNewTruthInfo() : # outParticles passing cuts (Lo Pt) "   <<  numOutPLoPt );
      
      //
      if( numOutP>=1) m_ntupleVars->get<int>( "Truth_nSVGe1HiPt" )++;
      int vtxType=0;
      if( numOutPLoPt == 1 ) vtxType=11;
      if( numOutPLoPt >  1 ) vtxType=12;
      if( numOutP     == 1 ) vtxType=1;
      if( numOutP     >  1 ) vtxType=2;

      //
      if( numOutP>= 1 ) { // only store vertex type 1 or 2, i.e., # HiPt tracks >=1
	// if( numOutP>= 1 || numOutPLoPt>=1 ) { // temporary test. by default this is >=2
        
        if( m_FillNtuple ) {
          m_ntupleVars->get<int>( "Truth_nSVHiPt" )++;
          //
          m_ntupleVars->get< vector<int>    >( "Truth_AllVtxType" )         .emplace_back(vtxType );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVX" )             .emplace_back(Vpos.x() );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVY" )             .emplace_back(Vpos.y() );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVZ" )             .emplace_back(Vpos.z() );
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVHasLifetime" )   .emplace_back(vtxHasLifetime );
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVStrangeBaryon" ) .emplace_back(strangeBaryon );
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVIncomingPid" )   .emplace_back(IncomingPID );
	
          if( vtx->nIncomingParticles()==1) ATH_MSG_DEBUG("> getNewTruthInfo() : Incoming PID/Lifetime flag "  <<  IncomingPID  <<  ","  <<  vtxHasLifetime );

          m_ntupleVars->get< vector<int>    >( "Truth_AllSVNumIncident" )   .emplace_back(vtx->nIncomingParticles() );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVPtIncident" )    .emplace_back(pTincoming );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVPzIncident" )    .emplace_back(pZincoming );
        }
	
	// first sort the track pt		
	//sort( Trkpt,Trkpt+maxTrks );					  
	sort( Trkpt,Trkpt+10 );					  
	ATH_MSG_DEBUG( "> getNewTruthInfo() : numTrksHi/Lo, re-orderered "
		        <<  numOutP  <<  ","
		        <<  numOutPLoPt  <<  ","
		        <<  Trkpt[maxTrks-1]  <<  ","
		        <<  Trkpt[maxTrks-2]  <<  ","
		        <<  Trkpt[maxTrks-3]  <<  ","
		        <<  Trkpt[maxTrks-4]  <<  ","
		        <<  Trkpt[maxTrks-5] );
	
        if( m_FillNtuple ) {
          //
          // TrkPt has been initialized to zero, 
          // so if there are only 2 tracks all other vectors will get filled with zero
          m_ntupleVars->get< vector<double> >( "Truth_AllSVTrk1Pt" )        .emplace_back(Trkpt[maxTrks-1] );		  
          m_ntupleVars->get< vector<double> >( "Truth_AllSVTrk2Pt" )        .emplace_back(Trkpt[maxTrks-2] );		  
          m_ntupleVars->get< vector<double> >( "Truth_AllSVTrk3Pt" )        .emplace_back(Trkpt[maxTrks-3] );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVTrk4Pt" )        .emplace_back(Trkpt[maxTrks-4] );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVTrk5Pt" )        .emplace_back(Trkpt[maxTrks-5] );
          //
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVNumTrks" )       .emplace_back(numOutP );
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVNumReTrks" )     .emplace_back(numReco );
          m_ntupleVars->get< vector<int>    >( "Truth_AllSVCharge" )        .emplace_back(sumChg );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVSumTrksPt" )     .emplace_back(sumTrkpt );
          m_ntupleVars->get< vector<double> >( "Truth_AllSVSumTrksPz" )     .emplace_back(sumTrkpz );
        }
	
	///////////////compiles and runs so far////////////////////////

	//
	// calculate opening angle between all 2-track pairs, and store the minimum
	//
	double minOpAng = 1000;
	double opAng=0;
	double p1=0;
	double p2=0;
	for ( int it1=0; it1<numOutP; ++it1 ) {
	  if( it1>=maxTrks ) continue;
	  p1 = sqrt( Trkpx[it1]*Trkpx[it1] + Trkpy[it1]*Trkpy[it1] + Trkpz[it1]*Trkpz[it1] );
	  for ( int it2=0; it2<numOutP; ++it2 ) {
	    if( it2>=maxTrks ) continue;
	    if( it1==it2 ) continue;
	    //
	    p2 = sqrt( Trkpx[it2]*Trkpx[it2] + Trkpy[it2]*Trkpy[it2] + Trkpz[it2]*Trkpz[it2] );
	    opAng = Trkpx[it1]*Trkpx[it2]+Trkpy[it1]*Trkpy[it2]+Trkpz[it1]*Trkpz[it2];
	    opAng = ( opAng/p1 )/p2;
	    ATH_MSG_DEBUG( "> getNewTruthInfo() : Opening Ang/ minOpAng "  <<  opAng  <<  ","  <<  minOpAng );
	    if( opAng < minOpAng ) minOpAng = opAng;
	  }
	}
	if( m_FillNtuple ) m_ntupleVars->get< vector<double> >( "Truth_AllSVMinOpAng" ).emplace_back(minOpAng );

	//
	// if there are >=2 matched reco trks at vertex, do a quick fit
	// 
	ATH_MSG_DEBUG( "> getNewTruthInfo() : numReco "   <<   numReco );
	ATH_MSG_DEBUG( "> getNewTruthInfo() : Selected trk size "   <<   m_selectedBaseTracks->size() );
	ATH_MSG_DEBUG( "> getNewTruthInfo() : size of SelTrk_barcode "   <<   seltrk_barcode_list.size() );
	//
	int RefitSt =0;
	int NumReTrk=0;
	double RefitSVX=0;
	double RefitSVY=0;
	double RefitSVZ=0;
	double RefitChi2=0;
	double RefitMass=0;
	//
	if( numReco>=2 && numReco<10 && m_selectedBaseTracks->size()>=2 ) {

	  vector<const xAOD::TrackParticle*>  ListMatBaseTracks( 0 );
	  vector<const xAOD::NeutralParticle*>  dummyNeutrals( 0 );
	  WrkVrt newvrt;
	  // 
	  // first fill vector with selected tracks.
	  for ( int itmp=0; itmp<numReco; ++itmp ) {

	    for(  auto itrk = m_selectedBaseTracks->begin(); itrk!= m_selectedBaseTracks->end(); ++itrk ) {
	      size_t isel = itrk - m_selectedBaseTracks->begin();
	      
	      ATH_MSG_DEBUG( "> getNewTruthInfo() : itmp/bcode of RecoTrks/SelTrk "
                             <<   itmp   <<   ","
                             <<   bcodeOfRecoTrks[itmp]   <<   ","
                             <<   isel   <<   ","
                             <<   seltrk_barcode_list.at(isel) );

	      // check if Selected reco trk passes all Si cuts
	      //

	      if( bcodeOfRecoTrks[itmp]==seltrk_barcode_list.at(isel) ) {

		ListMatBaseTracks.emplace_back( *(itrk) );
		//
	      }

	    } // loop over selected tracks

	  } // numReco loop
	  //
	  // check size of selected reco tracks
	  NumReTrk = ListMatBaseTracks.size();
	  if( (int) ListMatBaseTracks.size() <2 ) {
	    ATH_MSG_DEBUG( "> getNewTruthInfo() : mismatch numreco & re-fit size "  <<  numReco  <<  ","  <<  ListMatBaseTracks.size() );}
	  else{
	    ATH_MSG_DEBUG( "> getNewTruthInfo() : numreco & re-fit size "  <<  numReco  <<  ","  <<  ListMatBaseTracks.size() );
	    
	    m_fitSvc->setApproximateVertex( Vpos.x(), Vpos.y(), Vpos.z() );
            
	    sc=m_fitSvc->VKalVrtFit( ListMatBaseTracks,
                                     dummyNeutrals,
                                     newvrt.vertex,
                                     newvrt.vertexMom,
                                     newvrt.Charge,
                                     newvrt.vertexCov,
                                     newvrt.Chi2PerTrk,
                                     newvrt.TrkAtVrt,
                                     newvrt.Chi2 );
	    //
	    // check if refit is success
	    //
	    if( !sc) {RefitSt=-1; ATH_MSG_DEBUG("> getNewTruthInfo() : Refit of match Selected tracks failed " );}
	    //
	    else {
	      RefitSt=1;
	      ATH_MSG_DEBUG( "> getNewTruthInfo() : Refit of match Selected tracks successful " );
	      ATH_MSG_DEBUG( "> getNewTruthInfo() : MC vertex "  <<  Vpos.x()  <<  ","  <<  Vpos.y()  <<  ","  <<  Vpos.z() );
	      //
	      RefitSVX=newvrt.vertex[0];
              RefitSVY=newvrt.vertex[1];
              RefitSVZ=newvrt.vertex[2];
              RefitChi2=newvrt.Chi2;
	      // 
	      // now loop over tracks at vertex and get mass
	      double vx=0;
              double vy=0;
              double vz=0;
              double ve=0;
              
	      for( unsigned int itatv=0; itatv<ListMatBaseTracks.size(); ++itatv ) {
		double theta = newvrt.TrkAtVrt[itatv][1];
		double trk_p = fabs( 1./ newvrt.TrkAtVrt[itatv][2] );
		double trk_pt = trk_p*sin( theta );  
		double trk_px = trk_pt*cos( newvrt.TrkAtVrt[itatv][0] );  
		double trk_py = trk_pt*sin( newvrt.TrkAtVrt[itatv][0] );  
		double trk_pz = trk_p*cos( theta );  
		double trkp2 = trk_p*trk_p;
		double trk_e = sqrt( trkp2 + m_pi*m_pi ); 
		//
		vx += trk_px;
		vy += trk_py;
		vz += trk_pz;
		ve += trk_e; 
	      }
	      //
	      RefitMass = sqrt( ve*ve - vx*vx -vy*vy -vz*vz );

	      ATH_MSG_DEBUG( "> getNewTruthInfo() : Refit vertex "
                             <<  newvrt.vertex[0]  <<  ","
                             <<  newvrt.vertex[1]  <<  ","
                             <<  newvrt.vertex[2]  <<  ","
                             <<  RefitChi2         <<  ","
                             <<  RefitMass );
            
	    }//else 

	  } // successful re-fit

	} // if > 2 tracks with tigher cuts
	//////////////compiles and runs so far//////////////
	
          m_ntupleVars->get< vector<int>    >( "Truth_AllRefitSt" )   .emplace_back(RefitSt );
          m_ntupleVars->get< vector<int>    >( "Truth_AllRefitNTrk" ) .emplace_back(NumReTrk );
          m_ntupleVars->get< vector<double> >( "Truth_AllRefitSVX" )  .emplace_back(RefitSVX );
          m_ntupleVars->get< vector<double> >( "Truth_AllRefitSVY" )  .emplace_back(RefitSVY );
          m_ntupleVars->get< vector<double> >( "Truth_AllRefitSVZ" )  .emplace_back(RefitSVZ );
          m_ntupleVars->get< vector<double> >( "Truth_AllRefitChi2" ) .emplace_back(RefitChi2 );
          m_ntupleVars->get< vector<double> >( "Truth_AllRefitMass" ) .emplace_back(RefitMass );
          
          if( numOutPLoPt>= 2) m_ntupleVars->get<int>( "Truth_nSVLoPt" )++;

      }//end if nuOutP

    } // loop over vertices
    
    if( m_FillNtuple ) {
      ATH_MSG_DEBUG( "> getNewTruthInfo() : # SV with >= 1 Hi Pt outTrks passing cuts "   <<   m_ntupleVars->get<int>( "Truth_nSVHiPt" ) );
      ATH_MSG_DEBUG( "> getNewTruthInfo() : # SV with >= 1 Lo Pt outTrks passing cuts "   <<   m_ntupleVars->get<int>( "Truth_nSVLoPt" ) );
    }

    // delete object
    if( generatedTrackParam!=0 ) delete generatedTrackParam;
    
    if( sc.isFailure()) ATH_MSG_INFO("> getNewTruthInfo() : some status code failed. check. probably not serious " );

    return StatusCode::SUCCESS;
  }
  
  
  
  //____________________________________________________________________________________________________
  const xAOD::TruthParticle*  VrtSecInclusive::getTrkGenParticle ( const xAOD::TrackParticle *trkPart ) const 
  {
    typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
    static const char* NAME = "truthParticleLink";
    if(  ! trkPart->isAvailable< Link_t >( NAME ) ) {
      return 0;
    }
    const Link_t& link = trkPart->auxdata< Link_t >(  NAME );
    if(  ! link.isValid() ) {
      return 0;
    }
    return *link;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::categorizeVertexTruthTopology(  xAOD::Vertex *vertex )
  {
    
    enum vertexCatogory_tracks {
      allTruthAssociated,
      hasFakeTracks,
      allFakeTracks
    };
    
    enum vertexCategory_vertex {
      uniqueTruthVertex,
      multipleTruthVertices,
      noTruthVertex
    };
    
    
    // std::multiset allows to have multiplicity of the element
    multiset<const xAOD::TruthVertex*> truth_vertices;
    
    // std::multiset doesn't allow to have multiplicity of the element
    set<const xAOD::TruthVertex*>      truth_vertices_types;
    
    vector<const xAOD::TrackParticle*> reco_tracks;   // associated with truth and has prodVtx
    vector<const xAOD::TrackParticle*> orphan_tracks; // associated with truth, but does not have prodVtx
    vector<const xAOD::TrackParticle*> fake_tracks;   // no association with truth ( fake )
    
    // loop over tracks
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): loop over tracks" );
    for(  size_t itrk=0; itrk<vertex->nTrackParticles(); itrk++ ) {
      const auto *trk = vertex->trackParticle( itrk );
	
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): track loop itrk = " << itrk );
      typedef ElementLink<xAOD::TruthParticleContainer> truthLink;
      const truthLink& link = trk->auxdataConst< truthLink >( "truthParticleLink" );
      
      if (  ! link ) {
	fake_tracks.emplace_back(  trk );
	continue;
      }
      
      const xAOD::TruthParticle *truth = *link;
      if(  ! truth->hasProdVtx() ) {
	orphan_tracks.emplace_back(  trk );
	continue;
      }
      
      reco_tracks.emplace_back(  trk );
      
      truth_vertices_types .insert(  truth->prodVtx() );
      truth_vertices       .insert(  truth->prodVtx() );
	
    }
    
    
    // Add truth track pattern to the reco vertex
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add truth track pattern to the reco vertex" );
    if(  reco_tracks.size() == vertex->nTrackParticles() ) {
      vertex->auxdata<char>( "truth_trk_pattern" ) = allTruthAssociated;
    } else if(  fake_tracks.size() == vertex->nTrackParticles() ) {
      vertex->auxdata<char>( "truth_trk_pattern" ) = allFakeTracks;
    } else {
      vertex->auxdata<char>( "truth_trk_pattern" ) = hasFakeTracks;
    }
    
    
    // Histogramming - counting the number of appearing truth vertices connected
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Histogramming - counting the number of appearing truth vertices connected" );
    vector< tuple<const xAOD::TruthVertex*, size_t> > truth_vertex_histogram;
    for(  const auto *v : truth_vertices_types ) {
      size_t count = truth_vertices.count(  v );
      truth_vertex_histogram.emplace_back(  tuple<const xAOD::TruthVertex*, size_t>( v, count ) );
    }
    
    // Determine the truth vertex associated to this vertex by majority decision
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Determine the truth vertex associated to this vertex by majority decision" );
    tuple<const xAOD::TruthVertex*, size_t> tmp_tuple(  nullptr, 0 );
    for(  const auto& t : truth_vertex_histogram ) {
      const size_t& size_tmp  = get<1>( tmp_tuple );
      const size_t& size_this = get<1>( t );
      if(  size_tmp < size_this ) tmp_tuple = t;
    }
    
    // Add truth track pattern to the reco vertex
    ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add truth track pattern to the reco vertex" );
    char truth_vtx_pattern = 0;
    if(  truth_vertices_types.size() == 0 ) {
      truth_vtx_pattern = noTruthVertex;
    } else if(  truth_vertices_types.size() == 1 ) {
      truth_vtx_pattern = uniqueTruthVertex;
    } else {
      truth_vtx_pattern = multipleTruthVertices;
    }
    vertex->auxdata<char>( "truth_vtx_pattern" ) = truth_vtx_pattern;
    
    
    ElementLink<xAOD::TruthVertexContainer> vtx_link;
    if( noTruthVertex != truth_vtx_pattern ) {
        
      // Retrieve the truth vertex container for element link
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Retrieve the truth vertex container for element link" );
      const xAOD::TruthVertexContainer* truthVertexContainer (  nullptr );
      ATH_CHECK(  evtStore()->retrieve( truthVertexContainer, "TruthVertices") );
    
      // create the element link
      ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): create the element link" );
      const auto *theVertex = get<0>( tmp_tuple );
      if(  theVertex ) {
          // Add the truth vertex element link to the reco vertex
          vtx_link.toIndexedElement(*truthVertexContainer,theVertex->index());
          ATH_MSG_VERBOSE( "categorizeVertexTruthTopology(): Add the truth vertex element link to the reco vertex" );
      }
    } 
    // [JDC] a ElementLink decorator should be filled every event
    // although using a null link
    vertex->auxdata<ElementLink<xAOD::TruthVertexContainer> >( "truth_vtx_link" ) = vtx_link;
            
    return StatusCode::SUCCESS;
  }
  
} // end of namespace VKalVrtAthena



