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

#include "InDetZVTOPVxFinder/ZVTOP_Tool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetZVTOPVxFinder/ZVTOP_SpatialPointFinder.h"
#include "InDetZVTOPVxFinder/ZVTOP_TrkPartBaseVertexState.h"
#include "InDetZVTOPVxFinder/ZVTOP_VertexState.h"
#include "InDetZVTOPVxFinder/IZVTOP_TrkProbTubeCalc.h"
#include "InDetZVTOPVxFinder/IZVTOP_SimpleVtxProbCalc.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/LinkToTrack.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DataModel/DataVector.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <map>
#include <utility>
//================ Constructor =================================================

InDet::ZVTOP_Tool::ZVTOP_Tool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_iVertexFitter("Trk::FullVertexFitter"),
  m_iSpatialPointFinder("InDet::SpatialPointFinder"),
  m_iTrkProbTubeCalc("InDet::TrkProbTubeCalc"),
  m_iVtxProbCalc("InDet::VtxProbCalc"),
  m_iBeamCondSvc("BeamCondSvc",n)
{
  //  template for property declaration
  declareInterface<IVertexFinder>(this);
  declareProperty("VertexFitterTool",m_iVertexFitter);
  declareProperty("SpatialPointFinderTool",m_iSpatialPointFinder);
  declareProperty("TrkProbTubeCalcTool",m_iTrkProbTubeCalc);
  declareProperty("VtxProbCalcTool",m_iVtxProbCalc);
  declareProperty("ResolvingCutValue", m_resolvingCut = 0.6);
  declareProperty("ResolvingStepValue", m_resolvingStep = 0.3);
  declareProperty("VtxProbCut", m_vtxProb_cut = 0.00001);
  declareProperty("MaxChi2PerTrack", m_trk_chi2_cut = 5.);
  declareProperty("BeamPositionSvc", m_iBeamCondSvc);
}

//================ Destructor =================================================

InDet::ZVTOP_Tool::~ZVTOP_Tool()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_Tool::initialize()
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

  //Beam Spot
  sc = m_iBeamCondSvc.retrieve();
  if (sc.isFailure())
  {
    msg (MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
    return sc;
  }
  //VxFitter
  if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_Tool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================
//VxContainer* InDet::ZVTOP_Tool::findVertex(const TrackCollection* trackTES) --David S.
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDet::ZVTOP_Tool::findVertex(const TrackCollection* trackTES)
{
//VxContainer* newVxContainer = new VxContainer; --David S.
xAOD::VertexContainer* newVertexContainer = new xAOD::VertexContainer;
xAOD::VertexAuxContainer* newVertexAuxContainer = new xAOD::VertexAuxContainer;
newVertexContainer->setStore( newVertexAuxContainer );
if (trackTES->size() != 0) {
  //some variables
  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;
  const Trk::RecVertex beam_spot = m_iBeamCondSvc->beamVtx();

  // new Fitter EDM takes xAOD::Vertex instead of Trk::RecVertex as beam_spot constraint, so create one out of beamVtx() --David S.
  xAOD::Vertex theconstraint = xAOD::Vertex(); // Default constructor creates a private store
  theconstraint.setPosition( beam_spot.position() );
  theconstraint.setCovariancePosition( beam_spot.covariancePosition() );
  theconstraint.setFitQuality( beam_spot.fitQuality().chiSquared(), beam_spot.fitQuality().doubleNumberDoF() );

  std::vector<const Trk::Track*> trkColl(0); // the collection of tracks, which are assigned to one spatial point
  std::vector<InDet::ZVTOP_VertexState*> vtxState_org(0);// vector of tracks and vertices, tracks can be assigned to more than one vertex, these ambiguities will be resolved later
  std::multimap<double, InDet::ZVTOP_VertexState*> vtxProb_map;
  //---------------------------------------------------------------------------//  
  //-------step1: find spatial points & calculate vtx probabilities-------//
  //--------------------------------------------------------------------------//

  std::vector< std::vector<InDet::ZVTOP_VertexState*> > vertex_objects(0); //vector of vtx candidate & track collection pairs
  
  if (msgLvl(MSG::DEBUG)) msg() << "step ONE search for the spatial points, number of tracks = "<<(*trackTES).size() <<  endreq;
  for (TrackDataVecIter itr_1  = (*trackTES).begin(); itr_1 != (*trackTES).end()-1; itr_1++)
  {
    double sum_TrkProbTube = 0.;
    double sum2_TrkProbTube = 0.;
    double vtxProb = 0.;
    // trk-trk combination
    for (TrackDataVecIter itr_2 = itr_1+1; itr_2 != (*trackTES).end(); itr_2++)
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
            bool beam_spot = false;
            if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_VertexState(vtxProb, *spatialPoint, beam_spot, trkColl ));
            trkColl.clear();
         }
      }
      delete spatialPoint;
    }
    //trk-IP combination
    Trk::Vertex* spatialPoint = 0;
    spatialPoint = m_iSpatialPointFinder->findSpatialPoint(beam_spot,(*itr_1));
    if (spatialPoint != 0) 
    {
      double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(beam_spot,*spatialPoint);
      double TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
      sum_TrkProbTube = BeamProbTube + TrkProbTube;
      sum2_TrkProbTube = pow(TrkProbTube,2.) + pow(BeamProbTube,2.);
      if (sum_TrkProbTube != 0. ) 
      {
         vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
         trkColl.push_back((*itr_1));
         bool beam_spot = true;
         if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_VertexState(vtxProb, *spatialPoint, beam_spot, trkColl ));
         trkColl.clear();
      }
    }
    delete spatialPoint;
  }
  if (msgLvl(MSG::DEBUG)) msg() << " number of spatial points = "<<vtxState_org.size()<<endreq;
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
            else {
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
    if (msgLvl(MSG::DEBUG)) msg() << " step TWO vertex clustering, number of reduced vertices = "<<vtxState.size()<< endreq;
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
                  for (TrackDataVecIter trk_itr = (*trackTES).begin(); trk_itr != (*trackTES).end(); trk_itr++)
                  {
	              double TrkProbTube = 0.;
                      TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*trk_itr),SP_line);
                       sum_TrkProbTube += TrkProbTube;
                       sum2_TrkProbTube += pow(TrkProbTube,2.);
                   }
                   double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(beam_spot,SP_line);
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

    if (msgLvl(MSG::DEBUG)) msg() << " step THREE, vertex fit, vertex_objects size = "<<vertex_objects.size()<< endreq;
    //std::vector<Trk::VxCandidate* > theVxContainer; --David S.
    std::vector< xAOD::Vertex* > theVertexContainer;
    //prepare trk coll --> remove double tracks
    std::vector<std::vector<InDet::ZVTOP_VertexState*> >::iterator vtx_itr = vertex_objects.begin();
    for (; vtx_itr!= vertex_objects.end(); vtx_itr++)
    {
      bool with_beam_spot = false;
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
       if ((*itr)->beam_spot()) with_beam_spot = true;
       }
       //call the fitter
       //Trk::VxCandidate * myVxCandidate(0); --David S.
       xAOD::Vertex * myxAODVertex(0);
       //if (with_beam_spot) myVxCandidate = m_iVertexFitter->fit(trk_coll,beam_spot); --David S.
       if (with_beam_spot) myxAODVertex = m_iVertexFitter->fit(trk_coll,theconstraint);
       //else myVxCandidate = m_iVertexFitter->fit(trk_coll); --David S.
       else myxAODVertex = m_iVertexFitter->fit(trk_coll);
       bool bad_chi2 = true;
       //if (myVxCandidate) { --David S.
       if (myxAODVertex) {
          while (bad_chi2)
          {
             //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(myVxCandidate->vxTrackAtVertex()); --David S.
             std::vector< Trk::VxTrackAtVertex > trkAtVtx = myxAODVertex->vxTrackAtVertex();
             //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
             std::vector< Trk::VxTrackAtVertex >::iterator trkAtVtx_Iter = trkAtVtx.begin();
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
                    //if (with_beam_spot) myVxCandidate = m_iVertexFitter->fit(trk_coll, beam_spot); --David S.
                    if (with_beam_spot) myxAODVertex = m_iVertexFitter->fit(trk_coll, theconstraint);
                    //else myVxCandidate = m_iVertexFitter->fit(trk_coll); --David S.
                    else myxAODVertex = m_iVertexFitter->fit(trk_coll);
                  }
                  //if (myVxCandidate == 0) break; --David S.
                  if (myxAODVertex == 0) break;
               }
             } else bad_chi2 = false;
           }
      }
      //if (myVxCandidate && bad_chi2 == false && with_beam_spot) newVxContainer->push_back(myVxCandidate); --David S.
      if (myxAODVertex && bad_chi2 == false && with_beam_spot) newVertexContainer->push_back(myxAODVertex);
      //if (myVxCandidate && bad_chi2 == false && !with_beam_spot) theVxContainer.push_back(myVxCandidate); --David S.
      if (myxAODVertex && bad_chi2 == false && !with_beam_spot) theVertexContainer.push_back(myxAODVertex);
    }
    //if (msgLvl(MSG::DEBUG)) msg() <<"vertex container size = "<<theVxContainer.size()<<endreq; --David S.
    if (msgLvl(MSG::DEBUG)) msg() << "vertex container size = " << theVertexContainer.size() << endreq;

   //ambiguity solving for two track vertices
   //typedef std::vector<Trk::VxCandidate* >::iterator theVxContainerIter; --David S.
   typedef std::vector< xAOD::Vertex* >::iterator theVertexContainerIter;
   //theVxContainerIter iter = theVxContainer.begin(); --David S.
   theVertexContainerIter iter = theVertexContainer.begin();
   //theVxContainerIter iter_end = theVxContainer.end(); --David S.
   theVertexContainerIter iter_end = theVertexContainer.end();
   //std::vector<Trk::VxCandidate*> twoTrkContainer(0); --David S.
   std::vector< xAOD::Vertex* > twoTrkContainer(0);
   for (; iter!= iter_end; iter++)
   {
     //Trk::VxCandidate* theVxCandidate(*iter); --David S.
     xAOD::Vertex* thexAODVertex(*iter);
     //if (theVxCandidate->vxTrackAtVertex()->size() == 2 ) twoTrkContainer.push_back(theVxCandidate); --David S.
     if (thexAODVertex->vxTrackAtVertex().size() == 2 ) twoTrkContainer.push_back(thexAODVertex);
     //else newVxContainer->push_back(theVxCandidate); --David S.
     else newVertexContainer->push_back(thexAODVertex);
   }
   //std::vector<Trk::VxCandidate*>::iterator twoTrk_itr = twoTrkContainer.begin(); --David S.
   std::vector< xAOD::Vertex* >::iterator twoTrk_itr = twoTrkContainer.begin();
   for (; twoTrk_itr!= twoTrkContainer.end(); twoTrk_itr++)
   {
     // TODO: make sure that links are actually set in VxTrackAtVertices! --David S.
     //Trk::ITrackLink* trklink1 = (*(*twoTrk_itr)->vxTrackAtVertex())[0]->trackOrParticleLink(); --David S.
     Trk::ITrackLink* trklink1 = (*twoTrk_itr)->vxTrackAtVertex()[0].trackOrParticleLink();
     Trk::LinkToTrack* linkToTrack1 = dynamic_cast<Trk::LinkToTrack*>(trklink1);
     const Trk::Track* first_trk = linkToTrack1->cachedElement();
     //double first_trk_chi2 = (*(*twoTrk_itr)->vxTrackAtVertex())[0]->trackQuality().chiSquared(); --David S.
     double first_trk_chi2 = (*twoTrk_itr)->vxTrackAtVertex()[0].trackQuality().chiSquared();
     //Trk::ITrackLink* trklink2 = (*(*twoTrk_itr)->vxTrackAtVertex())[1]->trackOrParticleLink(); --David S.
     Trk::ITrackLink* trklink2 = (*twoTrk_itr)->vxTrackAtVertex()[1].trackOrParticleLink();
     Trk::LinkToTrack* linkToTrack2 = dynamic_cast<Trk::LinkToTrack*>(trklink2);
     const Trk::Track* second_trk = linkToTrack2->cachedElement();
     //double second_trk_chi2 = (*(*twoTrk_itr)->vxTrackAtVertex())[1]->trackQuality().chiSquared(); --David S.
     double second_trk_chi2 = (*twoTrk_itr)->vxTrackAtVertex()[1].trackQuality().chiSquared();
     
     //VxContainer::iterator Citer = newVxContainer->begin(); --David S.
     xAOD::VertexContainer::iterator Citer = newVertexContainer->begin();
     //VxContainer::iterator Citer_end = newVxContainer->end(); --David S.
     xAOD::VertexContainer::iterator Citer_end = newVertexContainer->end();
     bool first_found = false;
     bool second_found = false;
     ////loop over all VxCandidates --David S.
     //loop over all Vertices
     for (; Citer != Citer_end; Citer++)
     {
       //unsigned int size = (*Citer)->vxTrackAtVertex()->size(); --David S.
       unsigned int size = (*Citer)->vxTrackAtVertex().size();
       for (unsigned int counter = 0; counter < size; counter++)
       {  
         // TODO: make sure that links are actually set in VxTrackAtVertices! --David S.
         //Trk::ITrackLink* trklink = (*(*Citer)->vxTrackAtVertex())[counter]->trackOrParticleLink(); --David S.
         Trk::ITrackLink* trklink = (*Citer)->vxTrackAtVertex()[counter].trackOrParticleLink();
         Trk::LinkToTrack* linkToTrack = dynamic_cast<Trk::LinkToTrack*>(trklink);
         const Trk::Track* trk= linkToTrack->cachedElement();
         //double trk_chi2 = (*(*Citer)->vxTrackAtVertex())[counter]->trackQuality().chiSquared(); --David S.
         double trk_chi2 = (*Citer)->vxTrackAtVertex()[counter].trackQuality().chiSquared();
         if (trk == first_trk && trk_chi2 < first_trk_chi2 ) first_found = true;
         if (trk == second_trk && trk_chi2 < second_trk_chi2) second_found = true;
        }
     }
     //if (first_found==false && second_found==false) newVxContainer->push_back(*twoTrk_itr); --David S.
     if (first_found==false && second_found==false) newVertexContainer->push_back(*twoTrk_itr);
     else delete *twoTrk_itr;
   //}//end loop over two track Vx Candidates --David S.
   }//end loop over two track Vertices
   //if (msgLvl(MSG::DEBUG)) msg() <<"new VxContainer size = "<<newVxContainer->size()<<endreq; --David S.
   if (msgLvl(MSG::DEBUG)) msg() << "new VertexContainer size = " << newVertexContainer->size() << endreq;
   for (Sp_Iter iter = vtxState_org.begin(); iter != vtxState_org.end(); iter++) delete *iter;
} else {
    if (msgLvl(MSG::DEBUG)) msg() <<"No tracks in this event, provide to the next event" <<  endreq;
  }

//return newVxContainer; --David S.
return std::make_pair(newVertexContainer, newVertexAuxContainer);
}
//////////////////////////////////////////////////
///run on AOD
/////////////////////////////////////////////////
//VxContainer* InDet::ZVTOP_Tool::findVertex(const Trk::TrackParticleBaseCollection* trackTES) --David S.
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDet::ZVTOP_Tool::findVertex(const Trk::TrackParticleBaseCollection* trackTES)
{
//VxContainer* newVxContainer = new VxContainer; --David S.
xAOD::VertexContainer* newVertexContainer = new xAOD::VertexContainer;
xAOD::VertexAuxContainer* newVertexAuxContainer = new xAOD::VertexAuxContainer;
newVertexContainer->setStore( newVertexAuxContainer );
if (trackTES->size() != 0) {
  //some variables
  typedef Trk::TrackParticleBaseCollection::const_iterator TrackDataVecIter;
  const Trk::RecVertex beam_spot = m_iBeamCondSvc->beamVtx();

  // new Fitter EDM takes xAOD::Vertex instead of Trk::RecVertex as beam_spot constraint, so create one out of beamVtx() --David S.
  xAOD::Vertex theconstraint = xAOD::Vertex(); // Default constructor creates a private store
  theconstraint.setPosition( beam_spot.position() );
  theconstraint.setCovariancePosition( beam_spot.covariancePosition() );
  theconstraint.setFitQuality( beam_spot.fitQuality().chiSquared(), beam_spot.fitQuality().doubleNumberDoF() );

  std::vector<const Trk::TrackParticleBase*> trkColl(0); // the collection of tracks, which are assigned to one spatial point
  std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> vtxState_org(0);// vector of tracks and vertices, tracks can be assigned to more than one vertex, these ambiguities will be resolved later
  std::multimap<double, InDet::ZVTOP_TrkPartBaseVertexState*> vtxProb_map;
  //---------------------------------------------------------------------------//  
  //-------step1: find spatial points & calculate vtx probabilities-------//
  //--------------------------------------------------------------------------//

  std::vector< std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> > vertex_objects(0); //vector of vtx candidate & track collection pairs
  
  if (msgLvl(MSG::DEBUG)) msg() << "step ONE search for the spatial points, number of tracks = "<<(*trackTES).size() <<  endreq;
  for (TrackDataVecIter itr_1  = (*trackTES).begin(); itr_1 != (*trackTES).end()-1; itr_1++)
  {
    double sum_TrkProbTube = 0.;
    double sum2_TrkProbTube = 0.;
    double vtxProb = 0.;
    // trk-trk combination
    for (TrackDataVecIter itr_2 = itr_1+1; itr_2 != (*trackTES).end(); itr_2++)
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
            bool beam_spot = false;
            if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_TrkPartBaseVertexState(vtxProb, *spatialPoint, beam_spot, trkColl ));
            trkColl.clear();
         }
      }
      delete spatialPoint;
    }
    //trk-IP combination
    Trk::Vertex* spatialPoint = 0;
    spatialPoint = m_iSpatialPointFinder->findSpatialPoint(beam_spot,(*itr_1));
    if (spatialPoint != 0) 
    {
      double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(beam_spot,*spatialPoint);
      double TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*itr_1),*spatialPoint);
      sum_TrkProbTube = BeamProbTube + TrkProbTube;
      sum2_TrkProbTube = pow(TrkProbTube,2.) + pow(BeamProbTube,2.);
      if (sum_TrkProbTube != 0. ) 
      {
         vtxProb = m_iVtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
         trkColl.push_back((*itr_1));
         bool beam_spot = true;
         if (vtxProb > m_vtxProb_cut) vtxState_org.push_back(new InDet::ZVTOP_TrkPartBaseVertexState(vtxProb, *spatialPoint, beam_spot, trkColl ));
         trkColl.clear();
      }
    }
    delete spatialPoint;
  }
  if (msgLvl(MSG::DEBUG)) msg() << " number of spatial points = "<<vtxState_org.size()<<endreq;
  //reduce the spatial point collection
  typedef std::vector<InDet::ZVTOP_TrkPartBaseVertexState*>::iterator Sp_Iter;
  std::vector< InDet::ZVTOP_TrkPartBaseVertexState*> vtxState;
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
            else {
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
    if (msgLvl(MSG::DEBUG)) msg() << " step TWO vertex clustering, number of reduced vertices = "<<vtxState.size()<< endreq;
    //sort the vtxState collection, starting with a highest vtx probability
    std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> vtxState_sorted;
    unsigned int IP_counter = 0;
    std::multimap<double, InDet::ZVTOP_TrkPartBaseVertexState*>::reverse_iterator s= vtxProb_map.rbegin();
    for (;  s!=vtxProb_map.rend();  s++)  {
           InDet::ZVTOP_TrkPartBaseVertexState* vtx_state = (*s).second;
           if (vtx_state->beam_spot()) IP_counter += 1;
           if (IP_counter > 1) vtx_state->set_beam_spot(false);
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
                  for (TrackDataVecIter trk_itr = (*trackTES).begin(); trk_itr != (*trackTES).end(); trk_itr++)
                  {
	              double TrkProbTube = 0.;
                      TrkProbTube = m_iTrkProbTubeCalc->calcProbTube(*(*trk_itr),SP_line);
                       sum_TrkProbTube += TrkProbTube;
                       sum2_TrkProbTube += pow(TrkProbTube,2.);
                   }
                   double BeamProbTube = m_iTrkProbTubeCalc->calcProbTube(beam_spot,SP_line);
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

    if (msgLvl(MSG::DEBUG)) msg() << " step THREE, vertex fit, vertex_objects size = "<<vertex_objects.size()<< endreq;
    //std::vector<Trk::VxCandidate* > theVxContainer; --David S.
    std::vector< xAOD::Vertex* > theVertexContainer;
    //prepare trk coll --> remove double tracks
    std::vector<std::vector<InDet::ZVTOP_TrkPartBaseVertexState*> >::iterator vtx_itr = vertex_objects.begin();
    for (; vtx_itr!= vertex_objects.end(); vtx_itr++)
    {
      bool with_beam_spot = false;
      std::vector <const Trk::TrackParticleBase*> trk_coll(0); //--->trk_coll for the fit
      std::vector<InDet::ZVTOP_TrkPartBaseVertexState*>::iterator itr = (*vtx_itr).begin();
      trk_coll.push_back((*itr)->tracks()[0]);
      for (; itr != (*vtx_itr).end(); itr++)
      { 
        for ( std::vector <const Trk::TrackParticleBase*>::iterator vs_itr = ((*itr)->tracks()).begin(); vs_itr!= ((*itr)->tracks()).end(); vs_itr++)
        {
          bool found = false;
	  for (std::vector <const Trk::TrackParticleBase*>::iterator trk_itr = trk_coll.begin();  trk_itr!= trk_coll.end(); trk_itr++)  {
             if (*vs_itr == *trk_itr)  found = true;
          }
          if (!found)  trk_coll.push_back(*vs_itr);
        }
       if ((*itr)->beam_spot()) with_beam_spot = true;
       }
       //call the fitter
       //Trk::VxCandidate * myVxCandidate(0); --David S.
       xAOD::Vertex * myxAODVertex(0);
       //const Amg::Vector3D p(0.,0.,0.); --David S.
       const Amg::Vector3D startingPoint(0.,0.,0.);
       //Trk::Vertex startingPoint(p); --David S.
       //if (with_beam_spot) myVxCandidate = m_iVertexFitter->fit(trk_coll,beam_spot); --David S.
       if (with_beam_spot) myxAODVertex = m_iVertexFitter->fit(trk_coll,theconstraint);
       //else myVxCandidate = m_iVertexFitter->fit(trk_coll,startingPoint); --David S.
       else myxAODVertex = m_iVertexFitter->fit(trk_coll,startingPoint);
       bool bad_chi2 = true;
       //if (myVxCandidate) { --David S.
       if (myxAODVertex) {
          while (bad_chi2)
          {
             //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(myVxCandidate->vxTrackAtVertex()); --David S.
             std::vector< Trk::VxTrackAtVertex > trkAtVtx = myxAODVertex->vxTrackAtVertex();
             //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
             std::vector< Trk::VxTrackAtVertex >::iterator trkAtVtx_Iter = trkAtVtx.begin();
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
                    //if (with_beam_spot) myVxCandidate = m_iVertexFitter->fit(trk_coll, beam_spot); --David S.
                    if (with_beam_spot) myxAODVertex = m_iVertexFitter->fit(trk_coll, theconstraint);
                    //else myVxCandidate = m_iVertexFitter->fit(trk_coll,startingPoint); --David S.
                    else myxAODVertex = m_iVertexFitter->fit(trk_coll,startingPoint);
                  }
                  //if (myVxCandidate == 0) break; --David S.
                  if (myxAODVertex == 0) break;
               }
             } else bad_chi2 = false;
           }
      }
      //if (myVxCandidate && bad_chi2 == false && with_beam_spot) newVxContainer->push_back(myVxCandidate); --David S.
      if (myxAODVertex && bad_chi2 == false && with_beam_spot) newVertexContainer->push_back(myxAODVertex);
      //if (myVxCandidate && bad_chi2 == false && !with_beam_spot) theVxContainer.push_back(myVxCandidate); --David S.
      if (myxAODVertex && bad_chi2 == false && !with_beam_spot) theVertexContainer.push_back(myxAODVertex);
    }
    //if (msgLvl(MSG::DEBUG)) msg() <<"vertex container size = "<<theVxContainer.size()<<endreq; --David S.
    if (msgLvl(MSG::DEBUG)) msg() << "vertex container size = " << theVertexContainer.size() << endreq;

   //ambiguity solving for two track vertices
   //typedef std::vector<Trk::VxCandidate* >::iterator theVxContainerIter; --David S.
   typedef std::vector< xAOD::Vertex* >::iterator theVertexContainerIter;
   //theVxContainerIter iter = theVxContainer.begin(); --David S.
   theVertexContainerIter iter = theVertexContainer.begin();
   //theVxContainerIter iter_end = theVxContainer.end(); --David S.
   theVertexContainerIter iter_end = theVertexContainer.end();
   //std::vector<Trk::VxCandidate*> twoTrkContainer(0); --David S.
   std::vector< xAOD::Vertex* > twoTrkContainer(0);
   for (; iter!= iter_end; iter++)
   {
     //Trk::VxCandidate* theVxCandidate(*iter); --David S.
     xAOD::Vertex* thexAODVertex(*iter);
     //if (theVxCandidate->vxTrackAtVertex()->size() == 2 ) twoTrkContainer.push_back(theVxCandidate); --David S.
     if (thexAODVertex->vxTrackAtVertex().size() == 2 ) twoTrkContainer.push_back(thexAODVertex);
     //else newVxContainer->push_back(theVxCandidate); --David S.
     else newVertexContainer->push_back(thexAODVertex);
   }
   //std::vector<Trk::VxCandidate*>::iterator twoTrk_itr = twoTrkContainer.begin(); --David S.
   std::vector< xAOD::Vertex* >::iterator twoTrk_itr = twoTrkContainer.begin();
   for (; twoTrk_itr!= twoTrkContainer.end(); twoTrk_itr++)
   {
     // TODO: make sure that links are actually set in VxTrackAtVertices! --David S.
     //Trk::ITrackLink* trklink1 = (*(*twoTrk_itr)->vxTrackAtVertex())[0]->trackOrParticleLink(); --David S.
     Trk::ITrackLink* trklink1 = (*twoTrk_itr)->vxTrackAtVertex()[0].trackOrParticleLink();
     Trk::LinkToTrackParticleBase* linkToTrack1 = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink1);
     const Trk::TrackParticleBase* first_trk = linkToTrack1->cachedElement();
     //double first_trk_chi2 = (*(*twoTrk_itr)->vxTrackAtVertex())[0]->trackQuality().chiSquared(); --David S.
     double first_trk_chi2 = (*twoTrk_itr)->vxTrackAtVertex()[0].trackQuality().chiSquared();
     //Trk::ITrackLink* trklink2 = (*(*twoTrk_itr)->vxTrackAtVertex())[1]->trackOrParticleLink(); --David S.
     Trk::ITrackLink* trklink2 = (*twoTrk_itr)->vxTrackAtVertex()[1].trackOrParticleLink();
     Trk::LinkToTrackParticleBase* linkToTrack2 = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink2);
     const Trk::TrackParticleBase* second_trk = linkToTrack2->cachedElement();
     //double second_trk_chi2 = (*(*twoTrk_itr)->vxTrackAtVertex())[1]->trackQuality().chiSquared(); --David S.
     double second_trk_chi2 = (*twoTrk_itr)->vxTrackAtVertex()[1].trackQuality().chiSquared();
     
     //VxContainer::iterator Citer = newVxContainer->begin(); --David S.
     xAOD::VertexContainer::iterator Citer = newVertexContainer->begin();
     //VxContainer::iterator Citer_end = newVxContainer->end(); --David S.
     xAOD::VertexContainer::iterator Citer_end = newVertexContainer->end();
     bool first_found = false;
     bool second_found = false;
     ////loop over all VxCandidates --David S.
     //loop over all Vertices
     for (; Citer != Citer_end; Citer++)
     {
       //unsigned int size = (*Citer)->vxTrackAtVertex()->size(); --David S.
       unsigned int size = (*Citer)->vxTrackAtVertex().size();
       for (unsigned int counter = 0; counter < size; counter++)
       {  
         // TODO: make sure that links are actually set in VxTrackAtVertices! --David S.
         //Trk::ITrackLink* trklink = (*(*Citer)->vxTrackAtVertex())[counter]->trackOrParticleLink(); --David S.
         Trk::ITrackLink* trklink = (*Citer)->vxTrackAtVertex()[counter].trackOrParticleLink();
         Trk::LinkToTrackParticleBase* linkToTrack = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink);
         const Trk::TrackParticleBase* trk= linkToTrack->cachedElement();
         //double trk_chi2 = (*(*Citer)->vxTrackAtVertex())[counter]->trackQuality().chiSquared(); --David S.
         double trk_chi2 = (*Citer)->vxTrackAtVertex()[counter].trackQuality().chiSquared();
         if (trk == first_trk && trk_chi2 < first_trk_chi2 ) first_found = true;
         if (trk == second_trk && trk_chi2 < second_trk_chi2) second_found = true;
        }
     }
     //if (first_found==false && second_found==false) newVxContainer->push_back(*twoTrk_itr); --David S.
     if (first_found == false && second_found == false) newVertexContainer->push_back(*twoTrk_itr);
     else delete *twoTrk_itr;
   //}//end loop over two track Vx Candidates --David S.
   }//end loop over two track Vertices
   //if (msgLvl(MSG::DEBUG)) msg() <<"new VxContainer size = "<<newVxContainer->size()<<endreq; --David S.
   if (msgLvl(MSG::DEBUG)) msg() << "new VertexContainer size = " << newVertexContainer->size() << endreq;
   for (Sp_Iter iter = vtxState_org.begin(); iter != vtxState_org.end(); iter++) delete *iter;
} else {
    if (msgLvl(MSG::DEBUG)) msg() <<"No tracks in this event, provide to the next event" <<  endreq;
  }

//return newVxContainer; --David S.
return std::make_pair(newVertexContainer, newVertexAuxContainer);
}


std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>  InDet::ZVTOP_Tool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
{
     if(msgLvl(MSG::DEBUG)){ 
 	msg(MSG::DEBUG) << "N="<<trackParticles->size()<<" xAOD::TrackParticles found" << endreq; 
 	msg(MSG::DEBUG) << "No ZVTOP_Tool implementation for xAOD::TrackParticle" << endreq; 
     } 
     xAOD::VertexContainer *xAODContainer(0);
     xAOD::VertexAuxContainer *xAODAuxContainer(0);   
     return std::make_pair(xAODContainer, xAODAuxContainer); 
}

