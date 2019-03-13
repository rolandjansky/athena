/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_Tool.cxx, (c) ATLAS Detector software
// begin   : 30-10-2006
// authors : Tatjana Lenz
// email   : tatjana.lenz@cern.ch
// changes :
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_SecVtxTool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetZVTOPVxFinder/IZVTOP_SpatialPointFinder.h"
#include "InDetZVTOPVxFinder/IZVTOP_TrkProbTubeCalc.h"
#include "InDetZVTOPVxFinder/IZVTOP_SimpleVtxProbCalc.h"
#include "InDetZVTOPVxFinder/IZVTOP_AmbiguitySolver.h"
#include "InDetZVTOPVxFinder/ZVTOP_TrkPartBaseVertexState.h"
#include "InDetZVTOPVxFinder/ZVTOP_VertexState.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/Track.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include <map>
#include <utility>
//================ Constructor =================================================

InDet::ZVTOP_SecVtxTool::ZVTOP_SecVtxTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_iVertexFitter("Trk::FullVertexFitter"),
  m_iSpatialPointFinder("InDet::SpatialPointFinder"),
  m_iTrkProbTubeCalc("InDet::TrkProbTubeCalc"),
  m_iVtxProbCalc("InDet::VtxProbCalc"),
	m_iAmbiguitySolver("InDet::ZVTOP_AmbiguitySolver")
{
  //  template for property declaration
  declareInterface<ISecVertexInJetFinder>(this);
  declareProperty("VertexFitterTool",m_iVertexFitter);
  declareProperty("SpatialPointFinderTool",m_iSpatialPointFinder);
  declareProperty("TrkProbTubeCalcTool",m_iTrkProbTubeCalc);
  declareProperty("VtxProbCalcTool",m_iVtxProbCalc);
  declareProperty("AmbiguitySolverTool",m_iAmbiguitySolver);
  declareProperty("ResolvingCutValue", m_resolvingCut = 0.6);
  declareProperty("ResolvingStepValue", m_resolvingStep = 0.3);
  declareProperty("VtxProbCut", m_vtxProb_cut = 0.00001);
  declareProperty("MaxChi2PerTrack", m_trk_chi2_cut = 5.);
}

//================ Destructor =================================================

InDet::ZVTOP_SecVtxTool::~ZVTOP_SecVtxTool(){}

//================ Initialisation =================================================

StatusCode InDet::ZVTOP_SecVtxTool::initialize()
{
  StatusCode sc = AlgTool::initialize();

  msg (MSG::INFO) << name() << " initialize()" << endreq;
  if (sc.isFailure()) return sc;

  /* Retrieve Tools*/
  //SpatialPointFinder
  if ( m_iSpatialPointFinder.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iSpatialPointFinder << endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iSpatialPointFinder << endreq;

  //Gaussian Probability Tube for the Track Trajectory
  if ( m_iTrkProbTubeCalc.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iTrkProbTubeCalc<< endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iTrkProbTubeCalc << endreq;

  //Vertex Probability Function
  if ( m_iVtxProbCalc.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iVtxProbCalc<< endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iVtxProbCalc << endreq;

  //VxFitter
  if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;

  if ( m_iAmbiguitySolver.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iAmbiguitySolver << endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iAmbiguitySolver << endreq;

  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================================

StatusCode InDet::ZVTOP_SecVtxTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//===========================================================================================
// TrackParticle
//===========================================================================================
const Trk::VxSecVertexInfo* InDet::ZVTOP_SecVtxTool::findSecVertex(const Trk::RecVertex & primaryVertex,
								const TLorentzVector & jetMomentum,
								const std::vector<const Trk::TrackParticleBase*> & inputTracks) const
{
  //std::vector<Trk::VxCandidate*> secVertices; --David S.
  std::vector<xAOD::Vertex*> secVertices;
  //std::vector<Trk::VxCandidate*> new_secVertices(0); --David S.
  std::vector<xAOD::Vertex*> new_secVertices(0);
  //if (msgLvl(MSG::DEBUG)) msg () << "jet momentum Et s= "<<jetMomentum.et() <<  endreq; --David S.
  if (msgLvl(MSG::DEBUG)) msg () << "jet momentum Et s= "<<jetMomentum.Et() <<  endreq;
  if (inputTracks.size() != 0) {
    //some variables
    typedef std::vector<const Trk::TrackParticleBase*>::const_iterator TrkPartVecIter;
    std::vector<const Trk::TrackParticleBase*> trkColl; // the collection of tracks, which are assigned to one spatial point
    std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> vtxState_org;// vector of tracks and vertices, tracks can be assigned to more than one vertex, these ambiguities will be resolved later
    std::multimap<double, InDet::ZVTOP_TrkPartBaseVertexState*> vtxProb_map;
    //---------------------------------------------------------------------------//  
    //-------step1: find spatial points & calculate vtx probabilities-------//
    //--------------------------------------------------------------------------//
    std::vector< std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> > vertex_objects; //vector of vtx candidate & track collection pairs
    if (msgLvl(MSG::DEBUG)) msg () << "step ONE search for the spatial points, number of tracks = "<<inputTracks.size() <<  endreq;
    for (TrkPartVecIter itr_1  = inputTracks.begin(); itr_1 != inputTracks.end()-1; itr_1++)
  {
    double sum_TrkProbTube = 0.;
    double sum2_TrkProbTube = 0.;
    double vtxProb = 0.;
    // trk-trk combination
    for (TrkPartVecIter itr_2 = itr_1+1; itr_2 != inputTracks.end(); itr_2++)
    {
      Trk::Vertex* spatialPoint;
      spatialPoint = m_iSpatialPointFinder->findSpatialPoint((*itr_1),(*itr_2));
      if (spatialPoint != 0) 
      {
	double TrkProbTube_1 = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
	double TrkProbTube_2 = m_iTrkProbTubeCalc->calcProbTube(*(*itr_2),*spatialPoint);
	sum_TrkProbTube = TrkProbTube_1 + TrkProbTube_2;
	sum2_TrkProbTube = pow(TrkProbTube_1,2.) + pow(TrkProbTube_2,2.);
	if (sum_TrkProbTube != 0. )
	  { 
            vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
            trkColl.push_back((*itr_1));
            trkColl.push_back((*itr_2));
            bool IP = false;
            if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_TrkPartBaseVertexState(vtxProb, *spatialPoint, IP, trkColl ));
            trkColl.clear();
	  }
      }
      delete spatialPoint;
    }
    //trk-IP combination
    Trk::Vertex* spatialPoint = 0;
    spatialPoint = m_iSpatialPointFinder->findSpatialPoint(primaryVertex,(*itr_1));
    if (spatialPoint != 0) 
    {
      double IPprobTube = m_iTrkProbTubeCalc->calcProbTube(primaryVertex,*spatialPoint);
      double TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
      sum_TrkProbTube = IPprobTube + TrkProbTube;
      sum2_TrkProbTube = pow(TrkProbTube,2.) + pow(IPprobTube,2.);
      if (sum_TrkProbTube != 0. ) 
      {
         vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
         trkColl.push_back((*itr_1));
         bool IP = true;
         if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_TrkPartBaseVertexState(vtxProb, *spatialPoint, IP, trkColl ));
         trkColl.clear();
      }
    }
    delete spatialPoint;
  }
  if (msgLvl(MSG::DEBUG)) msg () << " number of spatial points = "<<vtxState_org.size()<<endreq;
  //reduce the spatial point collection
  typedef std::vector<InDet::ZVTOP_TrkPartBaseVertexState*>::iterator Sp_Iter;
  std::vector< InDet::ZVTOP_TrkPartBaseVertexState*> vtxState;
  for (Sp_Iter itr1 = vtxState_org.begin(); itr1 != vtxState_org.end(); itr1++)
  {

    if (vtxState.size() == 0) vtxState.push_back(*itr1);
    else 
      {
	Trk::Vertex vtx_new = (*itr1)->vertex();
	bool can_be_resolved = false;
	for (Sp_Iter itr2 = vtxState.begin(); itr2 != vtxState.end(); itr2++)
	  {
	    Trk::Vertex vtx_in = (*itr2)->vertex();
            double r_diff = sqrt(pow(vtx_new.position().x(),2.) + pow(vtx_new.position().y(),2.)) - sqrt(pow(vtx_in.position().x(),2.) + pow(vtx_in.position().y(),2.));
            double z_diff = vtx_new.position().z() - vtx_in.position().z();
            if (fabs(r_diff) > 0.001 && fabs(z_diff) > 0.001 && r_diff != 0. && z_diff != 0.) can_be_resolved = true;
            else 
	      {
                for (unsigned int trk_itr = 0; trk_itr < (*itr1)->tracks().size(); trk_itr++) (*itr2)->tracks().push_back((*itr1)->tracks()[trk_itr]); 
                can_be_resolved = false;
                break;
	      }
	  }
	if (can_be_resolved)  vtxState.push_back(*itr1);
      }
  }
  for (Sp_Iter itr_sort = vtxState.begin();  itr_sort!= vtxState.end(); itr_sort++)  vtxProb_map.insert(std::multimap<double, InDet::ZVTOP_TrkPartBaseVertexState*>::value_type((*itr_sort)->vtx_prob(), *itr_sort));


//-------------------------------------------------------------------------------//
//----step2: vertex clustering, aim: to cluster all vertices together, ----//
//----which can not be resolved. The found vertex seeds and ---------//
//----associated tracks are taken as an input for the vertex fit. --------//
//------------------------------------------------------------------------------//

  if (vtxState.size() != 0 ){
    if (msgLvl(MSG::DEBUG)) msg () << " step TWO vertex clustering, number of reduced vertices = "<<vtxState.size()<< endreq;
    //sort the vtxState collection, starting with a highest vtx probability
    std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> vtxState_sorted;
    std::multimap<double, InDet::ZVTOP_TrkPartBaseVertexState*>::reverse_iterator s= vtxProb_map.rbegin();
    for (;  s!=vtxProb_map.rend();  s++)  {
      InDet::ZVTOP_TrkPartBaseVertexState* vtx_state = (*s).second;
      vtxState_sorted.push_back(vtx_state);
    }
    //store first element in the vertex_objects ---->vector<vector<VertexState>>
    std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> vtx_coll;
    vtx_coll.push_back(*(vtxState_sorted.begin()));
    vertex_objects.push_back(vtx_coll);//store first seed
    std::vector< InDet::ZVTOP_TrkPartBaseVertexState*>  vtxState_new(vtxState_sorted); //copy of the vtxState_sorted
    vtxState_new.erase(vtxState_new.begin()); // without first element
    //loop over vtxState_sorted
    for (Sp_Iter org_itr= vtxState_sorted.begin(); org_itr != vtxState_sorted.end(); org_itr++)
    {
      Trk::Vertex seed = (*org_itr)->vertex();
      //found where the seed is already stored
      bool vtx_is_stored = false;
      unsigned int vertex_objectsPosition = 0; 
      for (unsigned int vertex_objectsItr = 0; vertex_objectsItr!= vertex_objects.size(); vertex_objectsItr++)
        {
          //stored vertices in the line vertex_objectsItr
          std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> stored_vertices = vertex_objects[vertex_objectsItr];
           //---->inner loop
          for (unsigned int stored_vtx_itr = 0; stored_vtx_itr!= stored_vertices.size(); stored_vtx_itr++)
          {
	    Trk::Vertex storedVtx = stored_vertices[stored_vtx_itr]->vertex();
	    double diff = ((*org_itr)->vertex().position().x() - storedVtx.position().x()) + ((*org_itr)->vertex().position().y() - storedVtx.position().y()) + ((*org_itr)->vertex().position().z() - storedVtx.position().z());
	    if (fabs(diff) < 0.0001)
              {
		vertex_objectsPosition = vertex_objectsItr;
		vtx_is_stored = true;
		break; // break inner loop if found
              }
	  }
	  if (vtx_is_stored == true) break; // break outer loop if found
	}
      if (!vtx_is_stored) {
	//if not stored
	std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> new_vtx_coll;
	new_vtx_coll.push_back(*org_itr);
	vertex_objects.push_back(new_vtx_coll);
	vertex_objectsPosition = vertex_objects.size() - 1;
      }
      for (Sp_Iter new_itr = vtxState_new.begin(); new_itr!= vtxState_new.end(); new_itr++)
	{
	  Trk::Vertex cand = (*new_itr)->vertex();
	  //calculate the vtx prob function for this seed
	  std::multimap<double, Trk::Vertex > vtx_prob_coll;
	  vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type((*new_itr)->vtx_prob(),(*new_itr)->vertex()));
	  vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type((*org_itr)->vtx_prob(),(*org_itr)->vertex()));
	  for (double alpha = m_resolvingStep; alpha <1.; alpha += m_resolvingStep)
	    {
	      Trk::Vertex SP_line = Trk::Vertex((*org_itr)->vertex().position() + alpha*((*new_itr)->vertex().position() - (*org_itr)->vertex().position()));
	      double sum_TrkProbTube = 0.;
	      double sum2_TrkProbTube = 0.;
	      for (TrkPartVecIter trk_itr = inputTracks.begin(); trk_itr != inputTracks.end(); trk_itr++)
		{
		  double TrkProbTube = 0.;
		  TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*trk_itr),SP_line);
		  sum_TrkProbTube += TrkProbTube;
		  sum2_TrkProbTube += pow(TrkProbTube,2.);
		}
	      double IPprobTube = m_iTrkProbTubeCalc->calcProbTube(primaryVertex,SP_line);
	      sum_TrkProbTube += IPprobTube;
	      sum2_TrkProbTube += pow(IPprobTube,2.);
	      double vtx_prob = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
	      vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type(vtx_prob, SP_line));
	    }
	  double vtx_prob_ratio = (*vtx_prob_coll.begin()).first/(*new_itr)->vtx_prob();
	  if (vtx_prob_ratio >= m_resolvingCut)  {
	    //check if the vertices can be resolved
	    vertex_objects[vertex_objectsPosition].push_back(*new_itr);
	    vtxState_new.erase(new_itr);
	    if (new_itr != vtxState_new.end()) --new_itr;
	    if (new_itr == vtxState_new.end()) break;
	  }
	}
    }//loop over orig reduced coll
  }//if spatial point collection size > 0

  
   //--------------------------------------------------------------------------------//
  //--------------------step3: call vertex fitter----------------------------------//
  //--------------------------------------------------------------------------------//

  if (msgLvl(MSG::DEBUG)) msg () << " step THREE, vertex fit, vertex_objects size = "<<vertex_objects.size()<< endreq;
  //std::vector<Trk::VxCandidate* > theVxContainer; --David S.
  std::vector<xAOD::Vertex*> theVertexContainer;
  //prepare trk coll --> remove double tracks
  std::vector<std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> >::iterator vtx_itr = vertex_objects.begin();
  for (; vtx_itr!= vertex_objects.end(); vtx_itr++)
    {
      bool with_IP = false;
      std::vector <const Trk::TrackParticleBase*> trk_coll(0); //--->trk_coll for the fit
      std::vector<InDet::ZVTOP_TrkPartBaseVertexState*>::iterator itr = (*vtx_itr).begin();
      trk_coll.push_back((*itr)->tracks()[0]);
      for (; itr != (*vtx_itr).end(); itr++)
	{ 
	  for ( std::vector <const Trk::TrackParticleBase*>::iterator vs_itr = ((*itr)->tracks()).begin(); vs_itr!= ((*itr)->tracks()).end(); vs_itr++)
	    {
	      if (msgLvl(MSG::DEBUG)) msg () <<"old trk coll size = "<<trk_coll.size()<<", new track = "<<(*vs_itr)<<endreq;
	      bool found = false;
	      for (std::vector <const Trk::TrackParticleBase*>::iterator trk_itr = trk_coll.begin();  trk_itr!= trk_coll.end(); trk_itr++)  {
		if (*vs_itr == *trk_itr)  found = true;
	      }
	      if (!found)  trk_coll.push_back(*vs_itr);
	    }
	  if ((*itr)->beam_spot()) with_IP = true;
	  if (msgLvl(MSG::DEBUG)) msg () <<"new track collection size = "<<trk_coll.size()<<endreq;
	}
      //call the fitter
      //Trk::VxCandidate * myVxCandidate(0); --David S.
      xAOD::Vertex * myxAODVertex(0);
      //const Amg::Vector3D p(0.,0.,0.); --David S.
      const Amg::Vector3D startingPoint(0.,0.,0.);
      //Trk::Vertex startingPoint(p); --David S.
      //if (!with_IP) myVxCandidate = m_iVertexFitter->fit(trk_coll,startingPoint); --David S.
      if (!with_IP) myxAODVertex = m_iVertexFitter->fit(trk_coll,startingPoint);
      bool bad_chi2 = true;
      //if (myVxCandidate) --David S.
      if (myxAODVertex)
	{
	  while (bad_chi2)
	    {
	      //if (msgLvl(MSG::DEBUG)) msg () <<"rec Vertex = "<<myVxCandidate->recVertex().position()<<endreq; --David S.
	      if (msgLvl(MSG::DEBUG)) msg () << "Vertex pos = " << myxAODVertex->position() << endreq;
	      //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(myVxCandidate->vxTrackAtVertex()); --David S.
	      std::vector<Trk::VxTrackAtVertex> trkAtVtx = myxAODVertex->vxTrackAtVertex();
	      //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
	      std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
	      std::vector< const Trk::TrackParticleBase*>::iterator trk_Iter = trk_coll.begin();
	      double largest_chi2 = 0.;
	      std::vector< const Trk::TrackParticleBase*>::iterator index;
	      for (; trkAtVtx_Iter!= trkAtVtx.end(); trkAtVtx_Iter++)
		{
		  double chi2 = (*trkAtVtx_Iter).trackQuality().chiSquared();
		  if (chi2 > largest_chi2) {
		    largest_chi2 = chi2;
		    index = trk_Iter;
		  }
		  trk_Iter++;
		}
	      if (largest_chi2 > m_trk_chi2_cut)
		{
		  if (trk_coll.size() < 3) break;
		  else {
		    trk_coll.erase(index);
		    if (trk_coll.size() >= 2) {
		      //if (myVxCandidate!=0) { delete myVxCandidate; myVxCandidate=0; } --David S.
		      if (myxAODVertex!=0) { delete myxAODVertex; myxAODVertex=0; }
		      //myVxCandidate = m_iVertexFitter->fit(trk_coll, startingPoint); --David S.
		      myxAODVertex = m_iVertexFitter->fit(trk_coll, startingPoint);
		    }
		    //if (myVxCandidate == 0) break; --David S.
		    if (myxAODVertex == 0) break;
		  }
		} else bad_chi2 = false;
	    }
	}
      //if (myVxCandidate && bad_chi2 == false) secVertices.push_back(myVxCandidate); --David S.
      if (myxAODVertex && bad_chi2 == false) secVertices.push_back(myxAODVertex);
    }
  new_secVertices = m_iAmbiguitySolver->solveAmbiguities(secVertices);
  if (msgLvl(MSG::DEBUG)) msg () <<"vertex container size = "<<secVertices.size()<<endreq;
  for (Sp_Iter iter = vtxState_org.begin(); iter != vtxState_org.end(); iter++) delete *iter;
  
  } else {
    if (msgLvl(MSG::DEBUG)) msg () <<"No tracks in this event, provide to the next event" <<  endreq;
  }
  
  return new Trk::VxSecVertexInfo(secVertices);
}

/////////////////////////////////////////////////////////////////////////////////////////
//for xAOD::IParticle --David S.
//Added purely to satisfy new inheritance in ISecVertexInJetFinder, not yet implemented
/////////////////////////////////////////////////////////////////////////////////////////
const Trk::VxSecVertexInfo* InDet::ZVTOP_SecVtxTool::findSecVertex(const xAOD::Vertex & /*primaryVertex*/, const TLorentzVector & /*jetMomentum*/,const std::vector<const xAOD::IParticle*> & /*inputTracks*/) const {

  if(msgLvl(MSG::DEBUG)){
    msg(MSG::DEBUG) << "No ZVTOP_SecVtxTool implementation for xAOD::IParticle" << endreq;
    msg(MSG::DEBUG) << "Returning null VxSecVertexInfo*" << endreq;
  }

  Trk::VxSecVertexInfo* returnInfo(0);
  return returnInfo;

}

////////////////////////////////////////////////////////////
//for Trk::Track
///////////////////////////////////////////////////////////
const Trk::VxSecVertexInfo* InDet::ZVTOP_SecVtxTool::findSecVertex(const Trk::RecVertex & primaryVertex,const TLorentzVector & jetMomentum, const std::vector<const Trk::Track*> & inputTracks) const
{
  //std::vector<Trk::VxCandidate*> secVertices; --David S.
  std::vector<xAOD::Vertex*> secVertices;
  //if (msgLvl(MSG::DEBUG)) msg () << "jet momentum Et = "<<jetMomentum.et() <<  endreq; --David S.
  if (msgLvl(MSG::DEBUG)) msg () << "jet momentum Et = "<<jetMomentum.Et() <<  endreq;
  if (inputTracks.size() != 0) {
    //some variables
    typedef std::vector<const Trk::Track*>::const_iterator TrackDataVecIter;
    std::vector<const Trk::Track*> trkColl(0); // the collection of tracks, which are assigned to one spatial point
    std::vector<InDet::ZVTOP_VertexState*> vtxState_org(0);// vector of tracks and vertices, tracks can be assigned to more than one vertex, these ambiguities will be resolved later
    std::multimap<double, InDet::ZVTOP_VertexState*> vtxProb_map;
    //---------------------------------------------------------------------------//  
    //-------step1: find spatial points & calculate vtx probabilities-------//
    //--------------------------------------------------------------------------//

    std::vector< std::vector<InDet::ZVTOP_VertexState*> > vertex_objects(0); //vector of vtx candidate & track collection pairs
    
    if (msgLvl(MSG::DEBUG)) msg () << "step ONE search for the spatial points, number of tracks = "<<inputTracks.size() <<  endreq;
    for (TrackDataVecIter itr_1  = inputTracks.begin(); itr_1 != inputTracks.end()-1; itr_1++)
      {
	double sum_TrkProbTube = 0.;
	double sum2_TrkProbTube = 0.;
	double vtxProb = 0.;
	// trk-trk combination
	for (TrackDataVecIter itr_2 = itr_1+1; itr_2 != inputTracks.end(); itr_2++)
	  {
	    Trk::Vertex* spatialPoint;
	    spatialPoint = m_iSpatialPointFinder->findSpatialPoint((*itr_1),(*itr_2));
	    if (spatialPoint != 0) 
	      {
		double TrkProbTube_1 = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
		double TrkProbTube_2 = m_iTrkProbTubeCalc->calcProbTube(*(*itr_2),*spatialPoint);
		sum_TrkProbTube = TrkProbTube_1 + TrkProbTube_2;
		sum2_TrkProbTube = pow(TrkProbTube_1,2.) + pow(TrkProbTube_2,2.);
		if (sum_TrkProbTube != 0. )
		  { 
		    vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
		    trkColl.push_back((*itr_1));
		    trkColl.push_back((*itr_2));
		    bool IP = false;
		    if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_VertexState(vtxProb, *spatialPoint, IP, trkColl ));
		    trkColl.clear();
		  }
	      }
	    delete spatialPoint;
	  }
	//trk-IP combination
	Trk::Vertex* spatialPoint = 0;
	spatialPoint = m_iSpatialPointFinder->findSpatialPoint(primaryVertex,(*itr_1));
	if (spatialPoint != 0) 
	  {
	    double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(primaryVertex,*spatialPoint);
	    double TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
	    sum_TrkProbTube = BeamProbTube + TrkProbTube;
	    sum2_TrkProbTube = pow(TrkProbTube,2.) + pow(BeamProbTube,2.);
	    if (sum_TrkProbTube != 0. ) 
	      {
		vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
		trkColl.push_back((*itr_1));
		bool IP = true;
		if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_VertexState(vtxProb, *spatialPoint, IP, trkColl ));
		trkColl.clear();
	      }
	  }
	delete spatialPoint;
      }
    if (msgLvl(MSG::DEBUG)) msg () << " number of spatial points = "<<vtxState_org.size()<<endreq;
    //reduce the spatial point collection
    typedef std::vector<InDet::ZVTOP_VertexState*>::iterator Sp_Iter;
    std::vector< InDet::ZVTOP_VertexState*> vtxState;
    for (Sp_Iter itr1 = vtxState_org.begin(); itr1 != vtxState_org.end(); itr1++)
      {
	
	if (vtxState.size() == 0) vtxState.push_back(*itr1);
	else {
	  Trk::Vertex vtx_new = (*itr1)->vertex();
	  bool can_be_resolved = false;
	  for (Sp_Iter itr2 = vtxState.begin(); itr2 != vtxState.end(); itr2++)
	    {
	      Trk::Vertex vtx_in = (*itr2)->vertex();
	      double r_diff = sqrt(pow(vtx_new.position().x(),2.) + pow(vtx_new.position().y(),2.)) - sqrt(pow(vtx_in.position().x(),2.) + pow(vtx_in.position().y(),2.));
	      double z_diff = vtx_new.position().z() - vtx_in.position().z();
	      if (fabs(r_diff) > 0.001 && fabs(z_diff) > 0.001 && r_diff != 0. && z_diff != 0.) can_be_resolved = true;
	      else 
		{
		  for (unsigned int trk_itr = 0; trk_itr < (*itr1)->tracks().size(); trk_itr++) (*itr2)->tracks().push_back((*itr1)->tracks()[trk_itr]); 
		  can_be_resolved = false;
		  break;
		}
	    }
	  if (can_be_resolved)  vtxState.push_back(*itr1);
	}
      }
    for (Sp_Iter itr_sort = vtxState.begin();  itr_sort!= vtxState.end(); itr_sort++)  vtxProb_map.insert(std::multimap<double, InDet::ZVTOP_VertexState*>::value_type((*itr_sort)->vtx_prob(), *itr_sort));


    //-------------------------------------------------------------------------------//
    //----step2: vertex clustering, aim: to cluster all vertices together, ----//
    //----which can not be resolved. The found vertex seeds and ---------//
    //----associated tracks are taken as an input for the vertex fit. --------//
    //------------------------------------------------------------------------------//


    if (vtxState.size() != 0 ){
      if (msgLvl(MSG::DEBUG)) msg () << " step TWO vertex clustering, number of reduced vertices = "<<vtxState.size()<< endreq;
      //sort the vtxState collection, starting with a highest vtx probability
      std::vector<InDet::ZVTOP_VertexState*> vtxState_sorted;
      unsigned int IP_counter = 0;
      std::multimap<double, InDet::ZVTOP_VertexState*>::reverse_iterator s= vtxProb_map.rbegin();
      for (;  s!=vtxProb_map.rend();  s++)  {
	InDet::ZVTOP_VertexState* vtx_state = (*s).second;
	if (vtx_state->beam_spot()) IP_counter += 1;
	if (IP_counter > 1) vtx_state->set_beam_spot(false);
	vtxState_sorted.push_back(vtx_state);
      }
      //store first element in the vertex_objects ---->vector<vector<VertexState>>
      std::vector<InDet::ZVTOP_VertexState*> vtx_coll;
      vtx_coll.push_back(*(vtxState_sorted.begin()));
      vertex_objects.push_back(vtx_coll);//store first seed
      std::vector< InDet::ZVTOP_VertexState*>  vtxState_new(vtxState_sorted); //copy of the vtxState_sorted
      vtxState_new.erase(vtxState_new.begin()); // without first element
      //loop over vtxState_sorted
      for (Sp_Iter org_itr= vtxState_sorted.begin(); org_itr != vtxState_sorted.end(); org_itr++)
	{
	  Trk::Vertex seed = (*org_itr)->vertex();
	  //found where the seed is already stored
	  bool vtx_is_stored = false;
	  unsigned int vertex_objectsPosition = 0; 
	  for (unsigned int vertex_objectsItr = 0; vertex_objectsItr!= vertex_objects.size(); vertex_objectsItr++)
	    {
	      //stored vertices in the line vertex_objectsItr
	      std::vector<InDet::ZVTOP_VertexState*> stored_vertices = vertex_objects[vertex_objectsItr];
	      //---->inner loop
	      for (unsigned int stored_vtx_itr = 0; stored_vtx_itr!= stored_vertices.size(); stored_vtx_itr++)
		{
		  Trk::Vertex storedVtx = stored_vertices[stored_vtx_itr]->vertex();
		  double diff = ((*org_itr)->vertex().position().x() - storedVtx.position().x()) + ((*org_itr)->vertex().position().y() - storedVtx.position().y()) + ((*org_itr)->vertex().position().z() - storedVtx.position().z());
		  if (fabs(diff) < 0.0001)
		    {
		      vertex_objectsPosition = vertex_objectsItr;
		      vtx_is_stored = true;
		      break; // break inner loop if found
		    }
		}
	      if (vtx_is_stored == true) break; // break outer loop if found
	    }
	  if (!vtx_is_stored) {
	    //if not stored
	    std::vector<InDet::ZVTOP_VertexState*> new_vtx_coll;
	    new_vtx_coll.push_back(*org_itr);
	    vertex_objects.push_back(new_vtx_coll);
	    vertex_objectsPosition = vertex_objects.size() - 1;
	  }
	  for (Sp_Iter new_itr = vtxState_new.begin(); new_itr!= vtxState_new.end(); new_itr++)
	    {
	      Trk::Vertex cand = (*new_itr)->vertex();
	      //calculate the vtx prob function for this seed
	      std::multimap<double, Trk::Vertex > vtx_prob_coll;
	      vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type((*new_itr)->vtx_prob(),(*new_itr)->vertex()));
	      vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type((*org_itr)->vtx_prob(),(*org_itr)->vertex()));
	      for (double alpha = m_resolvingStep; alpha <1.; alpha += m_resolvingStep)
		{
		  Trk::Vertex SP_line = Trk::Vertex((*org_itr)->vertex().position() + alpha*((*new_itr)->vertex().position() - (*org_itr)->vertex().position()));
                  double sum_TrkProbTube = 0.;
                  double sum2_TrkProbTube = 0.;
                  for (TrackDataVecIter trk_itr = inputTracks.begin(); trk_itr != inputTracks.end(); trk_itr++)
		    {
	              double TrkProbTube = 0.;
                      TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*trk_itr),SP_line);
		      sum_TrkProbTube += TrkProbTube;
		      sum2_TrkProbTube += pow(TrkProbTube,2.);
		    }
		  double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(primaryVertex,SP_line);
		  sum_TrkProbTube += BeamProbTube;
		  sum2_TrkProbTube += pow(BeamProbTube,2.);
		  double vtx_prob = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
		  vtx_prob_coll.insert(std::multimap<double, Trk::Vertex >::value_type(vtx_prob, SP_line));
		}
              double vtx_prob_ratio = (*vtx_prob_coll.begin()).first/(*new_itr)->vtx_prob();
	      if (vtx_prob_ratio >= m_resolvingCut)  {
		//check if the vertices can be resolved
		vertex_objects[vertex_objectsPosition].push_back(*new_itr);
		vtxState_new.erase(new_itr);
		if (new_itr != vtxState_new.end()) --new_itr;
		if (new_itr == vtxState_new.end()) break;
	      }
	    }
	}//loop over orig reduced coll
    }//if spatial point collection size > 0

    
    //--------------------------------------------------------------------------------//
    //--------------------step3: call vertex fitter----------------------------------//
    //--------------------------------------------------------------------------------//
    
    if (msgLvl(MSG::DEBUG)) msg () << " step THREE, vertex fit, vertex_objects size = "<<vertex_objects.size()<< endreq;
    //std::vector<Trk::VxCandidate* > theVxContainer; --David S.
    std::vector<xAOD::Vertex*> theVertexContainer;
    //prepare trk coll --> remove double tracks
    std::vector<std::vector<InDet::ZVTOP_VertexState*> >::iterator vtx_itr = vertex_objects.begin();
    for (; vtx_itr!= vertex_objects.end(); vtx_itr++)
      {
	bool with_IP = false;
	std::vector <const Trk::Track*> trk_coll(0); //--->trk_coll for the fit
	std::vector<InDet::ZVTOP_VertexState*>::iterator itr = (*vtx_itr).begin();
	trk_coll.push_back((*itr)->tracks()[0]);
	for (; itr != (*vtx_itr).end(); itr++)
	  { 
	    for ( std::vector <const Trk::Track*>::iterator vs_itr = ((*itr)->tracks()).begin(); vs_itr!= ((*itr)->tracks()).end(); vs_itr++)
	      {
		bool found = false;
		for (std::vector <const Trk::Track*>::iterator trk_itr = trk_coll.begin();  trk_itr!= trk_coll.end(); trk_itr++)  {
		  if (*vs_itr == *trk_itr)  found = true;
		}
		if (!found)  trk_coll.push_back(*vs_itr);
	      }
	    if ((*itr)->beam_spot()) with_IP = true;
	  }
	//call the fitter
	//Trk::VxCandidate * myVxCandidate(0); --David S.
	xAOD::Vertex * myxAODVertex(0);
	//if (!with_IP) myVxCandidate = m_iVertexFitter->fit(trk_coll); --David S.
	if (!with_IP) myxAODVertex = m_iVertexFitter->fit(trk_coll);
	bool bad_chi2 = true;
	//if (myVxCandidate) { --David S.
	if (myxAODVertex) {
          while (bad_chi2)
	    {
	      //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(myVxCandidate->vxTrackAtVertex()); --David S.
	      std::vector<Trk::VxTrackAtVertex> trkAtVtx = myxAODVertex->vxTrackAtVertex();
	      //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
	      std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
	      std::vector< const Trk::Track*>::iterator trk_Iter = trk_coll.begin();
	      double largest_chi2 = 0.;
	      std::vector< const Trk::Track*>::iterator index;
	      for (; trkAtVtx_Iter!= trkAtVtx.end(); trkAtVtx_Iter++)
		{
		  double chi2 = (*trkAtVtx_Iter).trackQuality().chiSquared();
		  if (chi2 > largest_chi2) {
		    largest_chi2 = chi2;
		    index = trk_Iter;
		  }
		  trk_Iter++;
		}
	      if (largest_chi2 > m_trk_chi2_cut)
		{
		  if (trk_coll.size() < 3) break;
		  else {
		    trk_coll.erase(index);
		    if (trk_coll.size() >= 2) {
		      //if (myVxCandidate!=0) { delete myVxCandidate; myVxCandidate=0; } --David S.
		      if (myxAODVertex!=0) { delete myxAODVertex; myxAODVertex=0; }
		      //myVxCandidate = m_iVertexFitter->fit(trk_coll); --David S.
		      myxAODVertex = m_iVertexFitter->fit(trk_coll);
		    }
		    //if (myVxCandidate == 0) break; --David S.
		    if (myxAODVertex == 0) break;
		  }
		} else bad_chi2 = false;
	    }
	}
	//if (myVxCandidate && bad_chi2 == false) secVertices.push_back(myVxCandidate); --David S.
	if (myxAODVertex && bad_chi2 == false) secVertices.push_back(myxAODVertex);
      }
    if (msgLvl(MSG::DEBUG)) msg () <<"vertex container size = "<<secVertices.size()<<endreq;
    for (Sp_Iter iter = vtxState_org.begin(); iter != vtxState_org.end(); iter++) delete *iter;
  } else {
    if (msgLvl(MSG::DEBUG)) msg () <<"No tracks in this event, provide to the next event" <<  endreq;
  }
  
  return new Trk::VxSecVertexInfo(secVertices);
  

}

