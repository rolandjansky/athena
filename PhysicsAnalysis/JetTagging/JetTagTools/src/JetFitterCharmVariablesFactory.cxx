/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterVariableFactory.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : March 2007
///
/// DESCRIPTION:
///
/// This class is a "variable factory". It generates, starting from the VxJetCandidate, all 
/// the variables actually used by the JetFitterTag.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <algorithm>
#include <numeric>

#include "boost/ptr_container/ptr_vector.hpp"

#include "CLHEP/Vector/LorentzVector.h"

#include <TMath.h>
#include <cmath> 

#include "TrkParameters/TrackParameters.h"
//#include "TrkNeutralParameters/MeasuredNeutralPerigee.h"
#include "TrkTrackLink/ITrackLink.h"

#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/SelectedTracksInJet.h"

#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"

#include "JetTagTools/JetFitterCharmVariablesFactory.h"
#include "JetTagInfo/JetFitterGenericTagInfo.h"
#include "JetTagInfo/JetFitterTagInfo.h"
//#include "JetEvent/Jet.h"
#include "JetTagEvent/ISvxAssociation.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"


namespace Analysis {


  JetFitterCharmVariablesFactory::JetFitterCharmVariablesFactory(const std::string& name,
								 const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_secVxFinderName("InDetJetFitterVxFinder"), 
    m_jetFitterInstance("JetFitterCharm"),
    m_addNegativeTracksToPrimaryVertex(false),
    m_usePtCorrectedEnergy(false),
    m_usePtCorrectedMass(false),
    m_useSingleTracksAlsoForMass(false),
    m_revertFromPositiveToNegativeTags(false), 
    m_buildOldInfoObject(false)
  {
    declareProperty("secVxFinderName",m_secVxFinderName);
    declareProperty("JetFitterInstance",m_jetFitterInstance);
    declareProperty("addNegativeTracksToPrimaryVertex",m_addNegativeTracksToPrimaryVertex);
    declareProperty("usePtCorrectedEnergy",m_usePtCorrectedEnergy);
    declareProperty("usePtCorrectedMass",m_usePtCorrectedMass);
    declareProperty("useSingleTracksAlsoForMass",m_useSingleTracksAlsoForMass);
    declareProperty("revertFromPositiveToNegativeTags",m_revertFromPositiveToNegativeTags);
    declareProperty("buildOldInfoObject",m_buildOldInfoObject); 
    
    declareInterface<IJetFitterVariablesFactory>(this);
  }  

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

JetFitterCharmVariablesFactory::~JetFitterCharmVariablesFactory() {}

StatusCode JetFitterCharmVariablesFactory::initialize() {
  ATH_MSG_INFO(" Initialization of JetFitterCharmVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

StatusCode JetFitterCharmVariablesFactory::finalize() {
  ATH_MSG_INFO(" Finalization of JetFitterCharmVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

  JetFitterCharmVariablesFactory::JFInfo 
  JetFitterCharmVariablesFactory::get_jf_info(const xAOD::Jet& jetToTag) const { 
    const Trk::SelectedTracksInJet* mySelectedTracksInJet=0;
    const Trk::VxJetCandidate* myVxJetCandidate=0; 
    const ISvxAssociation* newSvxAssociation=jetToTag.getAssociation<ISvxAssociation>(m_secVxFinderName);

    if (newSvxAssociation==0) {
      ATH_MSG_DEBUG( " No JetFitter vertex found, attached to the"
		     " Jet as association. Not going on with tagging...");
      return std::make_pair(mySelectedTracksInJet, myVxJetCandidate); 
    }

    const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();
    if (myVertexInfo==0) {
      ATH_MSG_WARNING(" Could not get the vertex info from"
		      " the JetAssociation! Strange...");
      return std::make_pair(mySelectedTracksInJet, myVxJetCandidate); 
    }

    const std::vector<Trk::VxCandidate*>& 
      myVertices = myVertexInfo->vertices();
    myVxJetCandidate=dynamic_cast<Trk::VxJetCandidate*>(myVertices[0]);

    if (myVxJetCandidate==0) {
      ATH_MSG_WARNING(" No correct VxJetCandidate could be retrieved."
		      " JetFitter is not doing any tag on this jet");
      return std::make_pair(mySelectedTracksInJet, myVxJetCandidate); 
    }

    const Trk::VxJetFitterVertexInfo* myJetFitterInfo=dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo);

    
    if (myJetFitterInfo!=0) 
    {
      mySelectedTracksInJet=myJetFitterInfo->getSelectedTracksInJet();
      if (mySelectedTracksInJet==0) {
        ATH_MSG_WARNING(" No selected tracks in Jet found. Skipping...");
      }
    }
    else
    {
      ATH_MSG_DEBUG(" No new JetFitter Vertex Info found:"
		    " normal if running JetFitter version 1.");
    }
    return std::make_pair(mySelectedTracksInJet, myVxJetCandidate); 
  }



  IJetFitterTagInfo* 
  JetFitterCharmVariablesFactory::getITagInfoObject(const xAOD::Jet & myJet) const {
    
    JFInfo jfinfo = get_jf_info(myJet); 
    if (!jfinfo.second || !jfinfo.first) return 0; 
    const Trk::VxJetCandidate& vxJetCandidate = *jfinfo.second; 
    const Trk::SelectedTracksInJet* mySelectedTracksInJet = jfinfo.first; 
    

    //create new JetFitterTagInfo object
    IJetFitterTagInfo* tagInfo = 0; 
    if (m_buildOldInfoObject) 
      tagInfo = new JetFitterTagInfo(m_jetFitterInstance); 
    else
      tagInfo = new JetFitterGenericTagInfo(m_jetFitterInstance);

    //put all needed information inside :-)
    
    const double s_massks=497.648;
    const double s_pion=139.57018;//hard coded pion mass ;-)
    
    int ntrackPrimaryVtx=vxJetCandidate.getPrimaryVertex()->getTracksAtVertex().size();
    double energyFromPrimary=0.;
    double energyFromSecondary=0.;

    int nVTX(0);
    int nTracksAtVtx(0);
    int nSingleTracks(0);
    double energyFraction(0);
    double mass(0);
    double significance3d(0);
    double deltaphi(0.);
    double deltaeta(0.);

    if (mySelectedTracksInJet!=0) 
    {
      ATH_MSG_DEBUG(" Adding the tracks from primary vertex information ");
      const std::vector<const Trk::ITrackLink*> & myPrimaryLinks=mySelectedTracksInJet->getPrimaryTrackLinks();
      
      std::vector<const Trk::ITrackLink*>::const_iterator myPrimaryLinksBegin=myPrimaryLinks.begin();
      std::vector<const Trk::ITrackLink*>::const_iterator myPrimaryLinksEnd=myPrimaryLinks.end();

      for(std::vector<const Trk::ITrackLink*>::const_iterator myPrimaryLinksIter=myPrimaryLinksBegin;
          myPrimaryLinksIter!=myPrimaryLinksEnd;
          ++myPrimaryLinksIter)
      {
        ntrackPrimaryVtx+=1;
        const Trk::TrackParameters * myParameters=(*myPrimaryLinksIter)->parameters();
        if (myParameters)
        {
          energyFromPrimary+=TMath::Sqrt(s_pion*s_pion+myParameters->momentum().mag2());
        }
        else
        {
          ATH_MSG_WARNING(" no perigee in track for energy computation. Skipping primary track...");
        }
      }
    } else
    {
      ATH_MSG_DEBUG(" No information about further primary tracks available. Normal in JetFitter vs. 1");
    }
    
    
    const Trk::RecVertexPositions & recVertexPositions=vxJetCandidate.getRecVertexPositions();
    const Amg::VectorX & vertexPosition=recVertexPositions.position();
    const AmgSymMatrix(6) & vertexCovMatrix = recVertexPositions.covariancePosition();
    
    Amg::Vector3D primaryPos(vertexPosition[Trk::jet_xv],
			  vertexPosition[Trk::jet_yv],
			  vertexPosition[Trk::jet_zv]);

    Amg::Vector3D jetDir(1,1,1);//has to be different from 0
    Amg::setPhi(jetDir, vertexPosition[Trk::jet_phi]);
    Amg::setTheta(jetDir,vertexPosition[Trk::jet_theta]);

    xAOD::IParticle::FourMom_t JetVector = myJet.p4();
    
    //loop over primary vertex
    const std::vector<Trk::VxTrackAtVertex*> & TracksAtPrimary=vxJetCandidate.getPrimaryVertex()->getTracksAtVertex();
    const std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryBegin=TracksAtPrimary.begin();
    const std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryEnd=TracksAtPrimary.end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryIter=TracksAtPrimaryBegin;
 TracksAtPrimaryIter!=TracksAtPrimaryEnd;
	 ++TracksAtPrimaryIter) {

      if (dynamic_cast<const Trk::Perigee*>((*TracksAtPrimaryIter)->perigeeAtVertex())!=0)
      {
        
        energyFromPrimary+=
            TMath::Sqrt(s_pion*s_pion+
                        (*TracksAtPrimaryIter)->perigeeAtVertex()->momentum().mag2());
      }

      //WARNING/TODO: commented out until vertex supports neutral parameters
//      else if (dynamic_cast<const Trk::MeasuredNeutralPerigee*>((*TracksAtPrimaryIter)->perigeeAtVertex())!=0)
//      {
//        ATH_MSG_VERBOSE(" Found KS pointing to primary vertex. Considering the correct mass. ");
//        energyFromPrimary+=
//            TMath::Sqrt(s_massks*s_massks+
//                        (*TracksAtPrimaryIter)->perigeeAtVertex()->momentum().mag2());
//      }
      else
      {
        ATH_MSG_ERROR("NeutralPerigee commented out from JetFitterCharmVariablesFactory::getITagInfoObject until vertex supports neutralparameters");
        ATH_MSG_ERROR(" Track is not a normal track neither a KS. This is an ERROR (ask developer to fix it). Skipping track... ");
      }
    }
    

    
    
    Amg::Vector3D sumPAllVertices(0.,0.,0.);
    CLHEP::HepLorentzVector massVector(0,0,0,0);
    double sumPtAdd(0.);

    double dist(0.);
    double inverrordist(0.);
    
    //now access the vertices on the jet axis info...
    std::vector<Trk::VxVertexOnJetAxis*> vectorOfVertices=vxJetCandidate.getVerticesOnJetAxis();
    
    //then you have to order them...
    std::vector<Trk::VxVertexOnJetAxis*>::iterator vectorOfClustersOfTrackBegin=vectorOfVertices.begin();
    std::vector<Trk::VxVertexOnJetAxis*>::iterator vectorOfClustersOfTrackEnd=vectorOfVertices.end();

    // store all secondary vertex positions 
    SecVertices secondary_vertices;  

    for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator vectorOfClustersOfTrackIter=vectorOfClustersOfTrackBegin;
	 vectorOfClustersOfTrackIter!=vectorOfClustersOfTrackEnd;
	 ++vectorOfClustersOfTrackIter) {
      
      const std::vector<Trk::VxTrackAtVertex*> & tracksOfVertex=(*vectorOfClustersOfTrackIter)->getTracksAtVertex();
      
      int vertexSize=tracksOfVertex.size();
      int vx_index=(*vectorOfClustersOfTrackIter)->getNumVertex()+5;//gets the right component (should add EDM method which does 
      if (is_vertex_positive(vertexPosition[vx_index])) { 
	if (vertexSize>1) {
	  nVTX+=1;
	  nTracksAtVtx+=vertexSize;

	  // store the vertex 
	  // the constructor that takes a VxJetCandidate knows about the silly offset
	  int sec_vx_index = vx_index - 5; 
	  secondary_vertices.push_back( SecVxDisplacement(vxJetCandidate, sec_vx_index) ); 

	} else {
	  nSingleTracks+=1;
	}
      }
    }

    // store all track 4-vectors
    FourMomenta all_track_4vectors; 
    
    
    for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator vectorOfClustersOfTrackIter=vectorOfClustersOfTrackBegin;
	 vectorOfClustersOfTrackIter!=vectorOfClustersOfTrackEnd;
	 ++vectorOfClustersOfTrackIter) {

      const std::vector<Trk::VxTrackAtVertex*> & tracksOfVertex=(*vectorOfClustersOfTrackIter)->getTracksAtVertex();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackBegin=tracksOfVertex.begin();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackEnd=tracksOfVertex.end();
      
      int vertexSize=tracksOfVertex.size();
      
      int vx_index=(*vectorOfClustersOfTrackIter)->getNumVertex() + 5;
      //gets the right component (should add EDM method which does 
      // this nasty little addition of 5...)

      if (!is_vertex_positive(vertexPosition[vx_index]) ) {      
        if (m_addNegativeTracksToPrimaryVertex) {
          ntrackPrimaryVtx+=vertexSize;
          for (std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackIter=clustersOfTrackBegin;
               clustersOfTrackIter!=clustersOfTrackEnd;++clustersOfTrackIter) {
            
            energyFromPrimary+=
                TMath::Sqrt(s_pion*s_pion+pow((*clustersOfTrackIter)->perigeeAtVertex()->momentum().mag(),2));
          }
        }
      } else {
	
	if ( (nVTX>0 && vertexSize>1) || nVTX==0 ) {
	  dist+=fabs(vertexPosition[vx_index])/vertexCovMatrix(vx_index,vx_index);
	  inverrordist+=1./vertexCovMatrix(vx_index,vx_index);
	}
	
	Amg::Vector3D sumP(0.,0.,0.);
	CLHEP::HepLorentzVector massThisCluster(0.,0.,0.,0.);
	
	//in case it's a real seconday vertex track...
	
	for (std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackIter=clustersOfTrackBegin;
	     clustersOfTrackIter!=clustersOfTrackEnd;
	     ++clustersOfTrackIter) {
	  
//	  const Trk::MeasuredPerigee* aMeasPer=static_cast<const Trk::MeasuredPerigee*>((*clustersOfTrackIter)->perigeeAtVertex());
          const Trk::TrackParameters* aMeasPer=(*clustersOfTrackIter)->perigeeAtVertex();
//	  CLHEP::HepVector perigeeParms = aMeasPer->parameters();
          
	  Amg::Vector3D mytrack(aMeasPer->momentum());
	  sumP+=mytrack;
          if (dynamic_cast<const Trk::Perigee*>((*clustersOfTrackIter)->perigeeAtVertex())!=0)
          {
            massThisCluster+=CLHEP::HepLorentzVector(mytrack.x(), mytrack.y(), mytrack.z(),TMath::Sqrt(s_pion*s_pion+mytrack.mag()*mytrack.mag()));
	    all_track_4vectors.push_back
	      (CLHEP::HepLorentzVector(mytrack.x(), mytrack.y(), mytrack.z(),TMath::Sqrt(s_pion*s_pion+mytrack.mag()*mytrack.mag())));
          }
//          else if (dynamic_cast<const Trk::MeasuredNeutralPerigee*>((*clustersOfTrackIter)->perigeeAtVertex())!=0)
//          {
//            ATH_MSG_VERBOSE(" Found KS in one vertex. Adding the correct KS mass! ");
//            massThisCluster+=CLHEP::HepLorentzVector(TMath::Sqrt(s_massks*s_massks+mytrack.mag()*mytrack.mag()),mytrack);
//	    // FIXME: this is a bit confusing
//	    all_track_4vectors.push_back
//	      (CLHEP::HepLorentzVector(TMath::Sqrt(s_massks*s_massks+mytrack.mag()*mytrack.mag()),mytrack));
//            if (nVTX>0)//if there is at least a vertex with already two tracks in the event
//            {
//              nTracksAtVtx+=1;
//            }
//          }
          //WARNING/TODO: neutralparameters case commented out until vertex supports neutral parameters
          else
          {
            ATH_MSG_ERROR(" neutral parameters case in JetFitterCharmVariablesFactory.cxx commented out until vertex supports neutral parameters");
            ATH_MSG_ERROR(" Track is not a normal track neither a KS. This is an ERROR (ask developer to fix it). Skipping track... ");
          }

	}

	sumPAllVertices+=sumP;
	double ptadd=sumP.perp(jetDir.unit());
	double masswithneutrals=TMath::Sqrt(massThisCluster.mag2()+ptadd*ptadd)+ptadd;

        if (m_useSingleTracksAlsoForMass)
        {
            massVector+=massThisCluster;
        }
        else
        {
          if ( (nVTX>0 && vertexSize>1) || nVTX==0 ) {
            massVector+=massThisCluster;
            sumPtAdd+=ptadd;
          }
        }
        
	
        if (m_usePtCorrectedEnergy)
        {
          energyFromSecondary+=TMath::Sqrt(masswithneutrals*masswithneutrals+
                                           sumP.mag2());
        }
        else
        {
          energyFromSecondary+=TMath::Sqrt(massThisCluster.mag2()+
                                           sumP.mag2());
        }
        

	
	
      }//end if dist<0
    }//end vectorOfVerteces
    
    if (energyFromSecondary+energyFromPrimary>0) {
      energyFraction=energyFromSecondary/(energyFromSecondary+energyFromPrimary);
    }
    
    if (massVector.mag()>0) {
      if (m_usePtCorrectedMass) 
      {
        mass=TMath::Sqrt(massVector.mag2()+sumPtAdd*sumPtAdd)+sumPtAdd;
      }
      else
      {
        mass=massVector.mag();
      }

      //port range of mass to maximum 10000.
      if (mass>5000.) {
	mass = 
	  5000.+5000./TMath::Pi()*2.*TMath::ATan(TMath::Pi()/2./5000.*(mass-5000.));
      }
    }
    
    if (inverrordist!=0) {
      significance3d=dist/TMath::Sqrt(inverrordist);
      //port range of significance 3d to maximum 100.
      significance3d=100./(TMath::Pi()/2.)*TMath::ATan(TMath::Pi()/2./100.*significance3d);
    }
    
    if (fabs(sumPAllVertices.mag())>1e-7) {
      deltaeta=sumPAllVertices.eta()-JetVector.Eta();
      deltaphi=Amg::deltaPhi(sumPAllVertices,Amg::Vector3D(JetVector.Px(),JetVector.Py(),JetVector.Pz()));
    } else {
      deltaphi=-10.;
      deltaeta=-10.;
    }
   
    //now store information in the JetFitterTagInfo object for later usage...
    tagInfo->setnVTX(nVTX);
    tagInfo->setnSingleTracks(nSingleTracks);
    tagInfo->setnTracksAtVtx(nTracksAtVtx);
    tagInfo->setMass(mass);
    tagInfo->setEnergyFraction(energyFraction);
    tagInfo->setSignificance3d(significance3d);
    tagInfo->setDeltaeta(deltaeta);
    tagInfo->setDeltaphi(deltaphi);


    // ------ if we're using the old info class, this won't cast and the old object will be returned
    JetFitterGenericTagInfo* generic_tag_info = dynamic_cast<JetFitterGenericTagInfo*>(tagInfo); 
    if (!generic_tag_info) 
      return tagInfo; 


    //=======================================================
    // ========= calculate additional quantities ============
    //=======================================================

    add_rap_ptrel(generic_tag_info, 
		  Amg::Vector3D(massVector.x(),massVector.y(),massVector.z()),
		  all_track_4vectors); 

    add_vertex_info(generic_tag_info, secondary_vertices); 

    assert(secondary_vertices.size() == nVTX); 

    return generic_tag_info;
  }

  void JetFitterCharmVariablesFactory::add_vertex_info(JetFitterGenericTagInfo* generic_tag_info, 
						       const SecVertices& secondary_vertices) const
  {
    const int N_VX_TO_COUNT = 2; 
    std::vector<double> vx_rho(N_VX_TO_COUNT,-1.0); 
    std::vector<double> vx_z(N_VX_TO_COUNT,-1.0);
    SecVertices closest_vx(N_VX_TO_COUNT); 
    std::partial_sort_copy(secondary_vertices.begin(), secondary_vertices.end(), 
			   closest_vx.begin(), closest_vx.end()); 
    
    int n_vx = std::min(secondary_vertices.size(), closest_vx.size()); 
    for (int i = 0; i < n_vx; i++) { 
      vx_rho.at(i) = closest_vx.at(i).m_vector3.perp();
      vx_z.at(i) = fabs(closest_vx.at(i).m_vector3.z());
    }
    generic_tag_info->setDouble("maxSecondaryVertexRho", vx_rho.at(0)); 
    generic_tag_info->setDouble("maxSecondaryVertexZ"  , vx_z.at(0)); 
    generic_tag_info->setDouble("subMaxSecondaryVertexRho", vx_rho.at(1)); 
    generic_tag_info->setDouble("subMaxSecondaryVertexZ"  , vx_z.at(1)); 
    
  }


  void JetFitterCharmVariablesFactory::add_rap_ptrel(JetFitterGenericTagInfo* generic_tag_info, 
						     const Amg::Vector3D& the_B_direction,
						     const FourMomenta& all_track_4vectors) const { 

    // currently the b-tagging software is setup to export a flat set of variables for each jet. 
    // as a result we'll have to make due with summery variables (rather than all the track info) 
    // unless we want to rewrite a lot of code
    std::vector<double> trackRapidities; 
    std::vector<double> trackPtRels; 

    // loop over all 4 vectors and calculate rapidity and pt_rel with respect to the B
    for (FourMomenta::const_iterator itr = all_track_4vectors.begin(); 
	 itr != all_track_4vectors.end(); itr++){ 
      if (itr->isTimelike() && !itr->isLightlike(0.0000000001)) { 
	double track_rapidity_along_B = itr->rapidity(CLHEP::Hep3Vector(the_B_direction.x(),the_B_direction.y(),the_B_direction.z()));
	trackRapidities.push_back(track_rapidity_along_B); 
      }
      else { 
	ATH_MSG_WARNING("found track with non-timelike 4-vector, skipping..."); 
      }
      
      if (the_B_direction.mag() && itr->vect().mag()) { 
	double track_pt_rel2 = itr->perp2(CLHEP::Hep3Vector(the_B_direction.x(),the_B_direction.y(),the_B_direction.z()));
	double track_pt_rel = track_pt_rel2 > 0.0 ? 
	  std::sqrt(track_pt_rel2) : 0.0; 
	trackPtRels.push_back(track_pt_rel); 
      }
    }


    // ===================================================
    
    // --- include a bunch of other track parameters
    int n_rap = trackRapidities.size(); 
    generic_tag_info->setInt("nTracks",n_rap); 

    double min_rapidity = -1;
    double max_rapidity = -1;
    double mean_rapidity = -1; 
    if (n_rap) { 
      min_rapidity = *std::min_element(trackRapidities.begin(), 
				       trackRapidities.end()); 
      max_rapidity = *std::max_element(trackRapidities.begin(), 
				       trackRapidities.end()); 

      double sum_rapidity = std::accumulate(trackRapidities.begin(), 
					    trackRapidities.end(), 0.0); 
      mean_rapidity = sum_rapidity / float(n_rap); 

    }
    generic_tag_info->setDouble("minTrackRapidity",min_rapidity); 
    generic_tag_info->setDouble("maxTrackRapidity",max_rapidity); 
    generic_tag_info->setDouble("meanTrackRapidity",mean_rapidity); 


    int n_ptrel = trackPtRels.size(); 
    double min_pt_rel   = -1;
    double max_pt_rel   = -1;
    double mean_pt_rel = -1; 
    if (n_ptrel) { 
      min_pt_rel   = *std::min_element(trackPtRels.begin(), 
				       trackPtRels.end());         
      max_pt_rel   = *std::max_element(trackPtRels.begin(), 
				       trackPtRels.end());         
      double sum_pt_rel   = std::accumulate(trackPtRels.begin(), 
					    trackPtRels.end(), 0.0);         
      mean_pt_rel   = sum_pt_rel   / float(n_ptrel); 
    }
    generic_tag_info->setDouble("minTrackPtRel",min_pt_rel); 
    generic_tag_info->setDouble("maxTrackPtRel",max_pt_rel); 
    generic_tag_info->setDouble("meanTrackPtRel",mean_pt_rel); 
    
  }


  bool JetFitterCharmVariablesFactory::is_vertex_positive(double vertex_position) const 
  {
    if (m_revertFromPositiveToNegativeTags) { 
      return vertex_position <= 0; 
    }
    else {
      return vertex_position > 0; 
    }
  }


  JetFitterCharmVariablesFactory::SecVxDisplacement::SecVxDisplacement() {

  }
  JetFitterCharmVariablesFactory::SecVxDisplacement::SecVxDisplacement(const Trk::VxJetCandidate& vxJetCandidate,
								       int index)
  { 
    m_vector3 = Amg::Vector3D(1.,0.,0.);
    const Amg::VectorX& vx_vector = vxJetCandidate.getRecVertexPositions().position();
    // vx_vector is a very silly container: first 5 components are 
    // primary vertex, after that they are displacements of the secondary vertices 
    double magnitude = vx_vector[index + 5]; 
    Amg::setMag(m_vector3, magnitude);
    Amg::setPhi(m_vector3, vx_vector[Trk::jet_phi]);
    Amg::setTheta(m_vector3, vx_vector[Trk::jet_theta]);
  }
  bool JetFitterCharmVariablesFactory::SecVxDisplacement::operator<(const SecVxDisplacement& rhs) const { 
    // this operator puts bigger values first, to arrange using stl algs
    return m_vector3.perp2() > rhs.m_vector3.perp2();
  }

  bool has_lower_pt(const CLHEP::HepLorentzVector& v1, const CLHEP::HepLorentzVector& v2)
  {
    return v1.perp2() < v2.perp2(); 
  }


}//end Analysis namespace
