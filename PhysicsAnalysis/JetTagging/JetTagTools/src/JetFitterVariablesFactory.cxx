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
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/SelectedTracksInJet.h"

#include "TrkParameters/TrackParameters.h"
//#include "TrkNeutralParameters/MeasuredNeutralPerigee.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"

#include "TrkTrackLink/ITrackLink.h"

#include <TMath.h>

#include "CLHEP/Vector/LorentzVector.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "JetTagTools/JetFitterVariablesFactory.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include <vector>


namespace Analysis {

  JetFitterVariablesFactory::JetFitterVariablesFactory(const std::string& name,
						       const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_secVxFinderName("InDetJetFitterVxFinder"), 
    m_jetFitterInstance("JetFitterTag"),
    m_addNegativeTracksToPrimaryVertex(false),
    m_usePtCorrectedEnergy(false),
    m_useSingleTracksAlsoForMass(false),
    m_revertFromPositiveToNegativeTags(false)
  {
    declareProperty("secVxFinderName",m_secVxFinderName);
    declareProperty("JetFitterInstance",m_jetFitterInstance);
    declareProperty("addNegativeTracksToPrimaryVertex",m_addNegativeTracksToPrimaryVertex);
    declareProperty("usePtCorrectedEnergy",m_usePtCorrectedEnergy);
    declareProperty("useSingleTracksAlsoForMass",m_useSingleTracksAlsoForMass);
    declareProperty("revertFromPositiveToNegativeTags",m_revertFromPositiveToNegativeTags);
    
    declareInterface<IJetFitterVariablesFactory>(this);
  }  

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

  JetFitterVariablesFactory::~JetFitterVariablesFactory() {}

StatusCode JetFitterVariablesFactory::initialize() {
  ATH_MSG_INFO(" Initialization of JetFitterVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

StatusCode JetFitterVariablesFactory::finalize() {
  ATH_MSG_INFO(" Finalization of JetFitterVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

  StatusCode JetFitterVariablesFactory::fillJetFitterVariables(const xAOD::Jet &myJet, xAOD::BTagging* BTag, const Trk::VxJetFitterVertexInfo* myJetFitterInfo, std::string basename) const{

    //VALERIO NASTY HACK!!!!
    if ( basename.find("Flip")!=std::string::npos) m_revertFromPositiveToNegativeTags=true;
    else m_revertFromPositiveToNegativeTags=false;
    
    int nVTX(0);
    int nTracksAtVtx(0);
    int nSingleTracks(0);
    float energyFraction(0);
    float mass(0);
    float mass_uncorr(0);
    float significance3d(0);
    float deltaphi(0.);
    float deltaeta(0.);
    float chi2(0.);
    int ndof(0);
    float deltaRFlightDir(0.);

    std::vector<Trk::VxJetCandidate*> myVertices;
    Trk::VxJetCandidate* myVxJetCandidate = nullptr;
    if (myJetFitterInfo) myVertices = myJetFitterInfo->verticesJF();
    if(myVertices.size() == 0){
      ATH_MSG_DEBUG("#BTAG# Trk::VxJetCandidate not found for jet fitter ");
      fill(BTag, basename, mass_uncorr, nVTX, nSingleTracks, nTracksAtVtx, mass, energyFraction, significance3d, deltaeta, deltaphi, chi2, ndof, deltaRFlightDir);
      return StatusCode::SUCCESS;
    }
    if(myVertices.size() > 0) myVxJetCandidate=dynamic_cast<Trk::VxJetCandidate*>(myVertices[0]);
    if (myVxJetCandidate==0) {
      ATH_MSG_WARNING("#BTAG# No correct VxJetCandidate could be retrieved." );
      fill(BTag, basename, mass_uncorr, nVTX, nSingleTracks, nTracksAtVtx, mass, energyFraction, significance3d, deltaeta, deltaphi, chi2, ndof, deltaRFlightDir);
      return StatusCode::SUCCESS;
    }

    const Trk::VxJetCandidate& myJetCandidate =*myVxJetCandidate;

    const Trk::SelectedTracksInJet* mySelectedTracksInJet = myJetFitterInfo->getSelectedTracksInJet();
    
        
    //put all needed information inside :-)
    
    //const double s_massks=497.648;
    const double s_pion=139.57018;//hard coded pion mass ;-)
    
    int ntrackPrimaryVtx=myJetCandidate.getPrimaryVertex()->getTracksAtVertex().size();
    double energyFromPrimary=0.;
    double energyFromSecondary=0.;

    // get fit quality variables for the PV of jetfitter
    const Trk::VxVertexOnJetAxis* pvtxjet = myVxJetCandidate->getPrimaryVertex();
    const Trk::FitQuality& fitquality = pvtxjet->fitQuality();
    chi2 = fitquality.chiSquared();
    ndof = fitquality.numberDoF();

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
        const Trk::TrackParameters* myParameters=(*myPrimaryLinksIter)->parameters();
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
    
    
    const Trk::RecVertexPositions & recVertexPositions=myJetCandidate.getRecVertexPositions();
    const Amg::VectorX & vertexPosition=recVertexPositions.position();
    const Amg::MatrixX & vertexCovMatrix = recVertexPositions.covariancePosition();
    
    Amg::Vector3D primaryPos(vertexPosition[Trk::jet_xv],
			  vertexPosition[Trk::jet_yv],
			  vertexPosition[Trk::jet_zv]);

    Amg::Vector3D flightAxis(1,1,1);//has to be different from 0
    Amg::setPhi(flightAxis, vertexPosition[Trk::jet_phi]);
    Amg::setTheta(flightAxis, vertexPosition[Trk::jet_theta]);

    xAOD::IParticle::FourMom_t JetVector = myJet.p4();
    
    //loop over primary vertex
    const std::vector<Trk::VxTrackAtVertex*> & TracksAtPrimary=myJetCandidate.getPrimaryVertex()->getTracksAtVertex();
    const std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryBegin=TracksAtPrimary.begin();
    const std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryEnd=TracksAtPrimary.end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator TracksAtPrimaryIter=TracksAtPrimaryBegin;
	 TracksAtPrimaryIter!=TracksAtPrimaryEnd;
	 ++TracksAtPrimaryIter) {

        //FIXME: dynamic cast necessary? neutral perigee commented out, fix when vertex supports neutral
      if (dynamic_cast<const Trk::Perigee*>((*TracksAtPrimaryIter)->perigeeAtVertex())!=0)
      {
        
        energyFromPrimary+=
            TMath::Sqrt(s_pion*s_pion+
                        (*TracksAtPrimaryIter)->perigeeAtVertex()->momentum().mag2());
      }
//      else if (dynamic_cast<const Trk::MeasuredNeutralPerigee*>((*TracksAtPrimaryIter)->perigeeAtVertex())!=0)
//      {
//        ATH_MSG_VERBOSE(" Found KS pointing to primary vertex. Considering the correct mass. ");
//        energyFromPrimary+=
//            TMath::Sqrt(s_massks*s_massks+
//                        (*TracksAtPrimaryIter)->perigeeAtVertex()->momentum().mag2());
//      }
      else
      {
        ATH_MSG_ERROR(" FIXME: VERTEX DOESN'T SUPPORT NEUTRAL PERIGEE, commented out in line 163");
        ATH_MSG_ERROR(" Track is not a normal track neither a KS. This is an ERROR (ask developer to fix it). Skipping track... ");
      }
    }
    

    
    
    Amg::Vector3D sumPAllVertices(0.,0.,0.);
    CLHEP::HepLorentzVector massVector(0,0,0,0);
    double sumPtAdd(0.);

    double dist(0.);
    double inverrordist(0.);
    
    //now access the vertices on the jet axis info...
    std::vector<Trk::VxVertexOnJetAxis*> vectorOfVertices=myJetCandidate.getVerticesOnJetAxis();
    
    //then you have to order them...
    std::vector<Trk::VxVertexOnJetAxis*>::iterator vectorOfClustersOfTrackBegin=vectorOfVertices.begin();
    std::vector<Trk::VxVertexOnJetAxis*>::iterator vectorOfClustersOfTrackEnd=vectorOfVertices.end();

    for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator vectorOfClustersOfTrackIter=vectorOfClustersOfTrackBegin;
	 vectorOfClustersOfTrackIter!=vectorOfClustersOfTrackEnd;
	 ++vectorOfClustersOfTrackIter) {
      
      const std::vector<Trk::VxTrackAtVertex*> & tracksOfVertex=(*vectorOfClustersOfTrackIter)->getTracksAtVertex();
      
      int vertexSize=tracksOfVertex.size();
      int ntrack=(*vectorOfClustersOfTrackIter)->getNumVertex()+5;//gets the right component (should add EDM method which does 
      if (!m_revertFromPositiveToNegativeTags)
      {
        if (vertexPosition[ntrack]>0) {     
          if (vertexSize>1) {
            nVTX+=1;
            nTracksAtVtx+=vertexSize;
          } else {
            nSingleTracks+=1;
          }
        }
      }
      else
      {
        if (vertexPosition[ntrack]<=0) {     
          if (vertexSize>1) {
            nVTX+=1;
            nTracksAtVtx+=vertexSize;
          } else {
            nSingleTracks+=1;
          }
        }
      }        
    }
    
    for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator vectorOfClustersOfTrackIter=vectorOfClustersOfTrackBegin;
	 vectorOfClustersOfTrackIter!=vectorOfClustersOfTrackEnd;
	 ++vectorOfClustersOfTrackIter) {

      const std::vector<Trk::VxTrackAtVertex*> & tracksOfVertex=(*vectorOfClustersOfTrackIter)->getTracksAtVertex();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackBegin=tracksOfVertex.begin();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackEnd=tracksOfVertex.end();
      
      int vertexSize=tracksOfVertex.size();
      
      int ntrack=(*vectorOfClustersOfTrackIter)->getNumVertex()+5;//gets the right component (should add EDM method which does 
      // this nasty little addition of 5...)


      if ((vertexPosition[ntrack]<0 && (!m_revertFromPositiveToNegativeTags))||(vertexPosition[ntrack]>=0 && m_revertFromPositiveToNegativeTags)) {      
        if (m_addNegativeTracksToPrimaryVertex) 
        {
          ntrackPrimaryVtx+=vertexSize;
          for (std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackIter=clustersOfTrackBegin;
               clustersOfTrackIter!=clustersOfTrackEnd;++clustersOfTrackIter) {
            
            energyFromPrimary+=
                TMath::Sqrt(s_pion*s_pion+pow((*clustersOfTrackIter)->perigeeAtVertex()->momentum().mag(),2));
          }
        }
      } else {
	
	if ( (nVTX>0 && vertexSize>1) || nVTX==0 ) {
	  dist+=fabs(vertexPosition[ntrack])/vertexCovMatrix(ntrack,ntrack);
          if (vertexCovMatrix(ntrack,ntrack)>0)
          {
            inverrordist+=1./vertexCovMatrix(ntrack,ntrack);
          }
          else
          {
            ATH_MSG_WARNING("The diagonal element of the vertex cov matrix ("<<ntrack<<","<<ntrack<<") is "<<vertexCovMatrix(ntrack,ntrack)<<". It should be positive... Ignoring vertex when computing L/sigma(L)");
          }
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
            massThisCluster+=CLHEP::HepLorentzVector(mytrack.x(),mytrack.y(),mytrack.z(),TMath::Sqrt(s_pion*s_pion+mytrack.mag()*mytrack.mag()));
          }
          //WARNING/TODO: commented out until after vertex has been rewritten
//          else if (dynamic_cast<const Trk::NeutralPerigee*>((*clustersOfTrackIter)->perigeeAtVertex())!=0)
//          {
//            ATH_MSG_VERBOSE(" Found KS in one vertex. Adding the correct KS mass! ");
//            massThisCluster+=CLHEP::HepLorentzVector(TMath::Sqrt(s_massks*s_massks+mytrack.mag()*mytrack.mag()),mytrack);
//            if (nVTX>0)//if there is at least a vertex with already two tracks in the event
//            {
//              nTracksAtVtx+=1;
//            }
//          }
          else
          {
            ATH_MSG_ERROR("Neutral parameter has been taken out until Vertex has been rewritten to support neutral perigee again. ");
            ATH_MSG_ERROR(" Track is not a normal track neither a KS. This is an ERROR (ask developer to fix it). Skipping track... ");
          }
	}

	
	sumPAllVertices+=sumP;
	double ptadd=sumP.perp(flightAxis.unit());
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
         mass=TMath::Sqrt(massVector.mag2()+sumPtAdd*sumPtAdd)+sumPtAdd;
	 mass_uncorr=massVector.mag();
      //port range of mass to maximum 10000.
      if (mass>5000.) {
	mass = 
	  5000.+(5000./M_PI)*2.*TMath::ATan((M_PI/2./5000.)*(mass-5000.));
      }
      if (mass_uncorr>5000.) {
	mass_uncorr = 
	  5000.+(5000./M_PI)*2.*TMath::ATan((M_PI/2./5000.)*(mass_uncorr-5000.));
      }
    }
    
    if (inverrordist!=0) {
      significance3d=dist/TMath::Sqrt(inverrordist);
      //port range of significance 3d to maximum 100.
      significance3d=100./(M_PI/2.)*TMath::ATan((M_PI/2./100.)*significance3d);
    }
    
    if (fabs(sumPAllVertices.mag())>1e-7) {
      deltaphi=sumPAllVertices.eta()-JetVector.Eta();
      deltaeta=sumPAllVertices.deltaPhi(Amg::Vector3D(JetVector.Px(), JetVector.Py(), JetVector.Pz()));
      deltaRFlightDir = TMath::Sqrt(sumPAllVertices.deltaPhi(flightAxis)*sumPAllVertices.deltaPhi(flightAxis) + (sumPAllVertices.eta()-flightAxis.eta())*(sumPAllVertices.eta()-flightAxis.eta()));
    } else {
      deltaphi=-10.;
      deltaeta=-10.;
      deltaRFlightDir = -10;
    }


    fill(BTag, basename, mass_uncorr, nVTX, nSingleTracks, nTracksAtVtx, mass, energyFraction, significance3d, deltaeta, deltaphi, chi2, ndof, deltaRFlightDir);

    return StatusCode::SUCCESS;

  
  }

void
JetFitterVariablesFactory::fill(xAOD::BTagging* BTag, const std::string& basename, float mass_uncorr,
                                int nVTX, int nSingleTracks, int nTracksAtVtx, float mass, float energyFraction,
                                float significance3d, float deltaeta, float deltaphi, float chi2, int ndof, float deltaRFlightDir) const {

  BTag->setVariable<float>(basename, "massUncorr", mass_uncorr);
  BTag->setVariable<float>(basename, "chi2", chi2);
  BTag->setVariable<int>(basename, "ndof", ndof);
  BTag->setVariable<float>(basename, "dRFlightDir", deltaRFlightDir);

  if (basename == "JetFitter"){
     BTag->setTaggerInfo(nVTX, xAOD::BTagInfo::JetFitter_nVTX);
     BTag->setTaggerInfo(nSingleTracks, xAOD::BTagInfo::JetFitter_nSingleTracks);
     BTag->setTaggerInfo(nTracksAtVtx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
     BTag->setTaggerInfo(mass, xAOD::BTagInfo::JetFitter_mass);
     BTag->setTaggerInfo(energyFraction, xAOD::BTagInfo::JetFitter_energyFraction);
     BTag->setTaggerInfo(significance3d, xAOD::BTagInfo::JetFitter_significance3d);
     BTag->setTaggerInfo(deltaeta, xAOD::BTagInfo::JetFitter_deltaeta);
     BTag->setTaggerInfo(deltaphi, xAOD::BTagInfo::JetFitter_deltaphi);
  } else {
     BTag->setVariable<int>(basename, "nVTX", nVTX);
     BTag->setVariable<int>(basename, "nSingleTracks", nSingleTracks);
     BTag->setVariable<int>(basename, "nTracksAtVtx", nTracksAtVtx);
     BTag->setVariable<float>(basename, "mass", mass);
     BTag->setVariable<float>(basename, "energyFraction", energyFraction);
     BTag->setVariable<float>(basename, "significance3d", significance3d);
     BTag->setVariable<float>(basename, "deltaeta", deltaeta);
     BTag->setVariable<float>(basename, "deltaphi", deltaphi);
  }
}

}//end Analysis namespace
