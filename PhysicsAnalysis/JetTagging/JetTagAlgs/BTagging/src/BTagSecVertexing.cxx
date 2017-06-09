/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/IBTagSecVertexing.h"
#include "BTagging/BTagSecVertexing.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/PrimaryVertexSelector.h" // new since rel 17.2

// migration to new ParticleJet class
//#include "JetEvent/Jet.h"

//general interface for secondary vertex finders
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
 
#include "Particle/TrackParticle.h"
#include "JetTagEvent/ISvxAssociation.h"
#include "JetTagEvent/TrackAssociation.h"

#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/SelectedTracksInJet.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "JetTagTools/JetFitterVariablesFactory.h"
#include "JetTagTools/MSVVariablesFactory.h"

#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
//#include "xAODBTagging/BTaggingAuxContainer.h"
//#include "xAODBTagging/SecVtxHelper.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
//#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h" 

namespace Analysis {

  BTagSecVertexing::BTagSecVertexing(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_secVertexFinderToolsHandleArray(),
    m_JFvarFactory("Analysis::JetFitterVariablesFactory"),
    m_MSVvarFactory("Analysis::MSVVariablesFactory"),
    m_vxPrimaryName("PrimaryVertices")
  {
    declareInterface<IBTagSecVertexing>(this);

    declareProperty("PrimaryVertexName",  m_vxPrimaryName);
    //List of the secondary vertex finders in jet to be run
    declareProperty("SecVtxFinderList",          m_secVertexFinderToolsHandleArray);
    declareProperty("SecVtxFinderTrackNameList", m_secVertexFinderTrackNameList);
    declareProperty("SecVtxFinderxAODBaseNameList", m_secVertexFinderBaseNameList);
    declareProperty("JetFitterVariableFactory",          m_JFvarFactory);
    declareProperty("MSVVariableFactory",          m_MSVvarFactory);
  }

  BTagSecVertexing::~BTagSecVertexing() {
  }

  StatusCode BTagSecVertexing::initialize()
  {
    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if(m_secVertexFinderTrackNameList.size() != m_secVertexFinderToolsHandleArray.size()){
      ATH_MSG_ERROR("#BTAG# Size mismatch between secVertexFinderToolsHandleArray and track name list ");
      return StatusCode::FAILURE;
    }

    if(m_secVertexFinderBaseNameList.size() != m_secVertexFinderToolsHandleArray.size()){
      ATH_MSG_ERROR("#BTAG# Size mismatch between secVertexFinderToolsHandleArray and base name list ");
      return StatusCode::FAILURE;
    }

    if ( m_secVertexFinderToolsHandleArray.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_secVertexFinderToolsHandleArray);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_secVertexFinderToolsHandleArray);
    }
    if ( m_JFvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_JFvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_JFvarFactory);
    }
    if ( m_MSVvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_MSVvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_MSVvarFactory);
    }

    return StatusCode::SUCCESS;
  }



  StatusCode BTagSecVertexing::finalize()
  {
      return StatusCode::SUCCESS;
  }

  StatusCode BTagSecVertexing::BTagSecVtx_exec(xAOD::Jet& myJet, xAOD::BTagging* newBTag, xAOD::VertexContainer* bTagVertexContainer, xAOD::BTagVertexContainer* bTagJFVertexContainer, const xAOD::Vertex* vtx) const {

    const xAOD::Vertex* primaryVertex(0);
    StatusCode sc = StatusCode::SUCCESS;
    
    if (vtx) {
      primaryVertex = vtx;
      if (sc.isFailure()) {
         ATH_MSG_WARNING("#BTAG# Primary vertex not passed through interface");
         return StatusCode::SUCCESS;
      }

    } else {
      //retrieve primary vertex 
      const xAOD::VertexContainer* vxContainer(0);
      
      sc = evtStore()->retrieve(vxContainer, m_vxPrimaryName);
      
      if (sc.isFailure()) {
	ATH_MSG_WARNING("#BTAG# Primary vertex coll " << m_vxPrimaryName << " not found");
	return StatusCode::SUCCESS;
      }
    
      if (vxContainer->size()==0) {
	ATH_MSG_DEBUG("#BTAG#  Vertex container is empty");
	return StatusCode::SUCCESS;
      }
      
      for (xAOD::VertexContainer::const_iterator fz = vxContainer->begin(); fz != vxContainer->end(); ++fz) {
	if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	  primaryVertex = *fz;
	  break;
	}
      }
      
      if (! primaryVertex) {
	ATH_MSG_DEBUG("#BTAG#  No vertex labeled as VxType::PriVtx!");
	xAOD::VertexContainer::const_iterator fz = vxContainer->begin();
        primaryVertex = *fz;
	if (primaryVertex->nTrackParticles() == 0) {
	  ATH_MSG_DEBUG("#BTAG#  PV==BeamSpot: probably poor tagging");
        }
      }
    }
    
    const xAOD::Vertex& PrimaryVtx = *primaryVertex;

    ToolHandleArray< InDet::ISecVertexInJetFinder >::const_iterator itSecVtxFinders = m_secVertexFinderToolsHandleArray.begin();
    ToolHandleArray< InDet::ISecVertexInJetFinder >::const_iterator itSecVtxFindersEnd = m_secVertexFinderToolsHandleArray.end();
    int nameiter = 0;

    const xAOD::TrackParticleContainer* theTrackParticleContainer = 0;

    for (  ; itSecVtxFinders != itSecVtxFindersEnd; ++itSecVtxFinders ) {

      if (*itSecVtxFinders==0) {
	ATH_MSG_WARNING("#BTAG#  could not run the SecVertexFinder:" <<  *itSecVtxFinders);
        ++nameiter;
	continue;
      }

      std::string trackname = m_secVertexFinderTrackNameList[nameiter];
      std::string basename =  m_secVertexFinderBaseNameList[nameiter];
      ++nameiter;

      std::vector<ElementLink< xAOD::TrackParticleContainer > > tracksInJet;        
        
      tracksInJet = newBTag->auxdata< std::vector<ElementLink< xAOD::TrackParticleContainer > > >(trackname);

      std::vector<const xAOD::IParticle*> inputIParticles;

      std::vector<ElementLink< xAOD::TrackParticleContainer > >::iterator itEL = tracksInJet.begin();
      std::vector<ElementLink< xAOD::TrackParticleContainer > >::iterator itELend = tracksInJet.end();

      if(tracksInJet.size()==0){
	ATH_MSG_DEBUG("#BTAG# no tracks associated to the jet");
        if("SV1" == basename){
          std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkList;
          newBTag->setSV1_TrackParticleLinks(TrkList);
          std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
          newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
      	  newBTag->setDynTPELName(basename, "badTracksIP");
          std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
          newBTag->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", SVertexLinks);
          newBTag->setDynVxELName(basename, "vertices");		
        }
	else if("MSV" == basename){ 
          std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
          newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
      	  newBTag->setDynTPELName(basename, "badTracksIP");
          std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
          newBTag->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", SVertexLinks);
          newBTag->setDynVxELName(basename, "vertices");
	}
	else if("JetFitter" == basename) {
          std::vector< ElementLink< xAOD::TrackParticleContainer > > tracksAtPVlinks;
          newBTag->setVariable<std::vector< ElementLink< xAOD::TrackParticleContainer > > >(basename, "tracksAtPVlinks", tracksAtPVlinks);  
          newBTag->setDynTPELName(basename, "tracksAtPVlinks");
	}	
        
	return StatusCode::SUCCESS;
      }    
	
      for (  ; itEL != itELend; ++itEL ) {

	const xAOD::TrackParticle* const *inputTrackParticle ;
	inputTrackParticle = (*itEL).cptr(); //ElementConstPointer cptr
       
	/// warning -> will not work if at some point we decide to associate to several track collections at the same time (in the same assoc object)
        
	theTrackParticleContainer = (*itEL).getStorableObjectPointer();
         
	inputIParticles.push_back(*inputTrackParticle);
      }
        

      ATH_MSG_DEBUG("#BTAG#  Running " << *itSecVtxFinders);
     
                
      const Trk::VxSecVertexInfo* myVertexInfo = (*itSecVtxFinders)->findSecVertex(PrimaryVtx, myJet.p4(), inputIParticles);

      ATH_MSG_DEBUG("#BTAG#  Storing result ");


      if (const Trk::VxSecVKalVertexInfo* myVertexInfoVKal = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(myVertexInfo)) {
	ATH_MSG_DEBUG("#BTAG# Found VKalVertexInfo information");
	sc = fillVkalVariables(myJet, newBTag, bTagVertexContainer, myVertexInfoVKal, theTrackParticleContainer, PrimaryVtx, basename);
	if(sc.isFailure()){
	  ATH_MSG_ERROR("#BTAG# error filling variables from VxSecVKalVertexInfo for tool " << *itSecVtxFinders);
          delete myVertexInfo;
	  return sc;
	}
      } else if (const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter = dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo)) {
  	ATH_MSG_DEBUG("#BTAG# Found VxJetFitterVertexInfo information");
  	sc = fillJFVariables(myJet, newBTag, bTagJFVertexContainer, myVertexInfoJetFitter, theTrackParticleContainer, basename);
  	if(sc.isFailure()){
  	  ATH_MSG_ERROR("#BTAG# error filling variables from VxJetFitterVertexInfo for tool " << *itSecVtxFinders);
          delete myVertexInfo;
  	  return sc;
  	}
      } else {
	ATH_MSG_WARNING("#BTAG# VxSecVertexInfo pointer cannot be interpreted for tool " << *itSecVtxFinders);
      }

      delete myVertexInfo;
         
    } /// for loop on vertex tools

    
    return StatusCode::SUCCESS;

  }


  StatusCode BTagSecVertexing::fillVkalVariables(xAOD::Jet& myJet,
                         xAOD::BTagging* newBTag, 
						 xAOD::VertexContainer* bTagVertexContainer, 
						 const Trk::VxSecVKalVertexInfo* myVertexInfoVKal,
						 const xAOD::TrackParticleContainer* theTrackParticleContainer,
                         const xAOD::Vertex& PrimaryVtx,
						 std::string basename) const {

    std::vector<xAOD::Vertex*>::const_iterator verticesBegin = myVertexInfoVKal->vertices().begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd   = myVertexInfoVKal->vertices().end();


    std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkList;
    float    mass = -1., energyfrc = -1., energyTrk=-1, dsttomatlayer = -1; 
    int  n2trk = -1, npsec = -1; //npprm = -1;

    std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
    
    if(myVertexInfoVKal->vertices().size()){
      npsec=0;
    }
    
    if(basename.find("MSV") == 0){
      StatusCode sc = m_MSVvarFactory->fillMSVVariables(myJet, newBTag, myVertexInfoVKal, bTagVertexContainer, PrimaryVtx ,basename);
      if(sc.isFailure()){
        ATH_MSG_ERROR("#BTAG# error filling variables in MSVVariablesFactory" );
        return sc;
      }     
    }else{
      for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin; verticesIter!=verticesEnd;++verticesIter) {
        xAOD::Vertex* btagVtx = *verticesIter;
        bTagVertexContainer->push_back(btagVtx);
             
        ElementLink< xAOD::VertexContainer> linkBTagVertex;
        linkBTagVertex.toContainedElement(*bTagVertexContainer, btagVtx);
        SVertexLinks.push_back(linkBTagVertex);

        std::vector<ElementLink<xAOD::TrackParticleContainer> > theseTracks = (*verticesIter)->trackParticleLinks();
        npsec += theseTracks.size();
        for (std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator itr=theseTracks.begin();itr!=theseTracks.end();itr++){
	      TrkList.push_back(*itr);
        }
      } 
              
      newBTag->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", SVertexLinks);
      newBTag->setDynVxELName(basename, "vertices");

      if(SVertexLinks.size()){
        mass = myVertexInfoVKal->mass();
        energyfrc = myVertexInfoVKal->energyFraction();
        n2trk = myVertexInfoVKal->n2trackvertices();
        energyTrk =  myVertexInfoVKal->energyTrkInJet();
	dsttomatlayer= myVertexInfoVKal->dstToMatLay();
      }



      newBTag->setVariable<float>(basename, "energyTrkInJet", energyTrk);
      newBTag->setVariable<float>(basename, "dstToMatLay", dsttomatlayer);

      if("SV1" == basename){
        //newBTag->setTaggerInfo(npprm, xAOD::BTagInfo::SV0_NGTinJet);
        newBTag->setTaggerInfo(mass, xAOD::BTagInfo::SV1_masssvx);
        newBTag->setTaggerInfo(energyfrc, xAOD::BTagInfo::SV1_efracsvx);
        newBTag->setTaggerInfo(n2trk, xAOD::BTagInfo::SV1_N2Tpair);
        newBTag->setTaggerInfo(npsec, xAOD::BTagInfo::SV1_NGTinSvx);
        newBTag->setSV1_TrackParticleLinks(TrkList);
      }
      else if ("SV0" == basename){
        //newBTag->setTaggerInfo(npprm, xAOD::BTagInfo::SV0_NGTinJet);
        newBTag->setTaggerInfo(mass, xAOD::BTagInfo::SV0_masssvx);
        newBTag->setTaggerInfo(energyfrc, xAOD::BTagInfo::SV0_efracsvx);
        newBTag->setTaggerInfo(n2trk, xAOD::BTagInfo::SV0_N2Tpair);
        newBTag->setTaggerInfo(npsec, xAOD::BTagInfo::SV0_NGTinSvx);
        newBTag->setSV0_TrackParticleLinks(TrkList);
      } 
      else{
        // newBTag->setVariable<int>(basename, "NGTinJet", npprm);
        newBTag->setVariable<float>(basename, "masssvx", mass);
        newBTag->setVariable<float>(basename, "efracsvx", energyfrc);
        newBTag->setVariable<int>(basename, "N2Tpair", n2trk);
        newBTag->setVariable<int>(basename, "NGTinSvx", npsec);
        newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "TrackParticleLinks", TrkList);
        newBTag->setDynTPELName(basename, "TrackParticleLinks");
      }
    }//no msv
    if(theTrackParticleContainer){
      std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
      std::vector<const xAOD::IParticle*> btip =  myVertexInfoVKal->badTracksIP();

      std::vector<const xAOD::IParticle*>::iterator ipBegin = btip.begin();
      std::vector<const xAOD::IParticle*>::iterator ipEnd   = btip.end();
      for (std::vector<const xAOD::IParticle*>::iterator ipIter=ipBegin; ipIter!=ipEnd; ++ipIter) {
	    const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(*ipIter);
	    if (!tp) {
	      ATH_MSG_WARNING("#BTAG# bad track IParticle is not a TrackParticle");
	      continue;
	    }
	    ElementLink<xAOD::TrackParticleContainer> tpel;
	    tpel.toContainedElement(*theTrackParticleContainer, tp);
	    badtrackEL.push_back(tpel);
      }
      newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
      newBTag->setDynTPELName(basename, "badTracksIP");
      // newBTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename+"_badTracksIP") = badtrackEL;
    } else {
      ATH_MSG_WARNING("#BTAG# pointer to track particle container not available -> can't create EL to bad tracks IP");
    }

    return StatusCode::SUCCESS;

  }

  StatusCode BTagSecVertexing::fillJFVariables(xAOD::Jet& myJet,
					       xAOD::BTagging* newBTag,
					       xAOD::BTagVertexContainer* bTagJFVertexContainer,
					       const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter,
					       const xAOD::TrackParticleContainer* theTrackParticleContainer,
					       std::string basename) const {
     //twotrackVerticesInJet   
     const Trk::TwoTrackVerticesInJet* TwoTrkVtxInJet =  myVertexInfoJetFitter->getTwoTrackVerticesInJet();

     const std::vector< const xAOD::Vertex*> vecTwoTrkVtx =  TwoTrkVtxInJet->getTwoTrackVertice();

     int N2TrkVtx = vecTwoTrkVtx.size();
     if("JetFitter" == basename){
        newBTag->setTaggerInfo(N2TrkVtx, xAOD::BTagInfo::JetFitter_N2Tpair);
     }
     else{
      newBTag->setVariable<int>(basename, "N2Tpair", N2TrkVtx);
     }
         
     //list of JFvertices
     const std::vector<Trk::VxJetCandidate*> JFvertices =  myVertexInfoJetFitter->verticesJF();
  
     int nVtx = 0;
     if (JFvertices.size() > 0) {
       Trk::VxJetCandidate* vxjetcand = dynamic_cast< Trk::VxJetCandidate*>(JFvertices[0]);
       if (!vxjetcand) {
         ATH_MSG_WARNING("#BTAG# bad VxCandidate is not a VxJetCandidate");
         return StatusCode::SUCCESS;
       }

       //compatibility with others SV
       const Trk::VxClusteringTable* ClusteringTable = vxjetcand->getClusteringTable();
       typedef std::map<Trk::VxVertexOnJetAxis*, std::vector<std::pair<Trk::VxVertexOnJetAxis*,float> > > map2vtx;
       map2vtx inverseCompMap;

       if (ClusteringTable !=0) {
         const std::map<float,Trk::PairOfVxVertexOnJetAxis>& mapPairVtxOnJetAxis = ClusteringTable->getCompatibilityPairOfVertices();
         std::map<float,Trk::PairOfVxVertexOnJetAxis>::const_iterator mapiter = mapPairVtxOnJetAxis.begin();
         for(;mapiter != mapPairVtxOnJetAxis.end(); ++mapiter){

           Trk::VxVertexOnJetAxis* vtx1 = mapiter->second.first;
           Trk::VxVertexOnJetAxis* vtx2 = mapiter->second.second;
           float comp = mapiter->first;

           std::pair<Trk::VxVertexOnJetAxis*,float> pair1(vtx1, comp);
           std::pair<Trk::VxVertexOnJetAxis*,float> pair2(vtx1, comp);

           inverseCompMap[vtx1].push_back(pair2);
           inverseCompMap[vtx2].push_back(pair1);

         }
       } 

       //vtx on jet axis
       const std::vector<Trk::VxVertexOnJetAxis*> Vtxonjetaxes = vxjetcand->getVerticesOnJetAxis();
     
       std::vector<Trk::VxVertexOnJetAxis*>::const_iterator iterBegin = Vtxonjetaxes.begin();
       std::vector<Trk::VxVertexOnJetAxis*>::const_iterator iterEnd   = Vtxonjetaxes.end();  
       float comptoPV(0.);
       double chi2(0.);
       int   ndf(0);
       //ATH_MSG_INFO("#BTAGJetFitter# size of Vtxonjetaxes: " <<  Vtxonjetaxes.size());
       nVtx = Vtxonjetaxes.size(); 
       std::vector< ElementLink< xAOD::BTagVertexContainer > > JFVerticesLinks;
       std::vector<xAOD::BTagVertex*> JFVertices;
       std::map<Trk::VxVertexOnJetAxis*, ElementLink< xAOD::BTagVertexContainer> > oldnewmap;

       for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator it=iterBegin; it!=iterEnd; ++it) {
         //ndf, chi2 , comptoPV   
         const Trk::FitQuality& fitquality = (*it)->fitQuality();
         chi2 =  fitquality.chiSquared();
         ndf  =  fitquality.numberDoF();
         comptoPV =  (*it)->getCompatibilityToPrimaryVtx();
         const std::vector<Trk::VxTrackAtVertex*> & trackatVtx = (*it)->getTracksAtVertex();
         std::vector<Trk::VxTrackAtVertex*>::const_iterator itrBegin = trackatVtx.begin();
         std::vector<Trk::VxTrackAtVertex*>::const_iterator itrEnd   = trackatVtx.end();
         std::vector< float>  tmpchi2;
         std::vector< float>  tmpndf;
         std::vector< ElementLink< xAOD::TrackParticleContainer > > trkLinks;
         std::vector< float>  tmpRefPx;
         std::vector< float>  tmpRefPy;
         std::vector< float>  tmpRefPz;
  
         //loop in trks 
         for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr=itrBegin; itr!=itrEnd; ++itr) {
           //ndf, chi2   
           const Trk::FitQuality& trkquality = (*itr)->trackQuality();
           double tpchi2 = trkquality.chiSquared();
           int tpndf = trkquality.numberDoF();
           tmpchi2.push_back(float(tpchi2));
           tmpndf.push_back(float(tpndf));
           //tracks links
           Trk::ITrackLink* trklinks = (*itr)->trackOrParticleLink();
           const Trk::LinkToXAODTrackParticle* trkLinkTPxAOD=dynamic_cast<const Trk::LinkToXAODTrackParticle *>(trklinks);
           if (!trkLinkTPxAOD) {
             ATH_MSG_WARNING("#BTAG# bad ITrackLink is not a LinkToXAODTrackParticle");
             continue;
           }
           const xAOD::TrackParticle* myTrklink = **trkLinkTPxAOD;
           ElementLink<xAOD::TrackParticleContainer> tpel;
           tpel.toContainedElement(*theTrackParticleContainer, myTrklink);
           trkLinks.push_back(tpel);
         
           //reffited momentum   
           const Trk::TrackParameters* trkperigee = (*itr)->perigeeAtVertex();
           Amg::Vector3D mytrack = trkperigee->momentum();
           tmpRefPx.push_back(mytrack.x());
           tmpRefPy.push_back(mytrack.y());
           tmpRefPz.push_back(mytrack.z());

         }
         xAOD::BTagVertex* newbtagVtx = new xAOD::BTagVertex();
	     JFVertices.push_back(newbtagVtx);

         bTagJFVertexContainer->push_back(newbtagVtx);
    
         newbtagVtx->setNDF(ndf);
         newbtagVtx->setChi2(chi2);
         newbtagVtx->setPV_compatibility(comptoPV);
         newbtagVtx->setTrack_chi2(tmpchi2);
         newbtagVtx->setTrack_NDF(tmpndf);
         newbtagVtx->setTrack_links(trkLinks);
         newbtagVtx->setTrack_refPx(tmpRefPx);
         newbtagVtx->setTrack_refPy(tmpRefPy);
         newbtagVtx->setTrack_refPz(tmpRefPz);

         ElementLink< xAOD::BTagVertexContainer> linkBTagVertex;
         linkBTagVertex.toContainedElement(*bTagJFVertexContainer, newbtagVtx);
         JFVerticesLinks.push_back(linkBTagVertex);
	     oldnewmap.insert(std::make_pair(*it,linkBTagVertex));
      }

       //CompToOtherSecVertices
      
     if(ClusteringTable !=0){
       //int btagvxi=0;
	   for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator it=iterBegin; it!=iterEnd; ++it) {

	       std::vector< ElementLink< xAOD::BTagVertexContainer > > JFCompLinks;
	       std::vector<float> JFCompValues;

	     bool foundmap=true;
	     map2vtx::iterator itmap = inverseCompMap.find(*it);  
	     if(itmap == inverseCompMap.end()){
	       ATH_MSG_DEBUG("#BTAG# JF compatibility value in map2vtx map not found ");
	       foundmap=false;
	     }

	     if(foundmap){

	       std::vector<std::pair<Trk::VxVertexOnJetAxis*,float> > tmpvectpair = itmap->second;

	       for(unsigned int icomppair=0; icomppair<tmpvectpair.size(); ++icomppair){
	        std::map<Trk::VxVertexOnJetAxis*, ElementLink< xAOD::BTagVertexContainer> >::iterator JFOlNewIter = oldnewmap.find(tmpvectpair[icomppair].first);
	 
	         if(JFOlNewIter == oldnewmap.end()){
		       ATH_MSG_DEBUG("#BTAG# JF compatibility vector not found ");
		       continue;
	         }

	       JFCompLinks.push_back(JFOlNewIter->second);
	       JFCompValues.push_back(tmpvectpair[icomppair].second);
	       }

	     } /// found map
	 
	   //JFVertices[btagvxi]->setCompToOtherSV(JFCompValues);
	   //JFVertices[btagvxi]->setReco_vertexLink(JFCompLinks);
	   //++btagvxi;
	   }
     }
     typedef std::vector<ElementLink<xAOD::BTagVertexContainer> > BTagVertices;
     ATH_MSG_DEBUG("#BTAGJF# filling vertices for basename:" << basename);
      newBTag->setVariable<std::vector<ElementLink<xAOD::BTagVertexContainer> > >(basename, "JFvertices", JFVerticesLinks);
      newBTag->setDynBTagVxELName(basename, "JFvertices");
      ATH_MSG_DEBUG("#BTAGJF# n vertices: " << newBTag->auxdata<BTagVertices>(basename + "_JFvertices").size());


      const Trk::RecVertexPositions& recVtxposition = vxjetcand->getRecVertexPositions();
      const Amg::VectorX& vtxPositions = recVtxposition.position();
      const Amg::MatrixX& vtxCovMatrix = recVtxposition.covariancePosition();
      ATH_MSG_DEBUG("#BTAGJF# size vtxPosition "<<vtxPositions.size());
      std::vector< float > fittedPosition = std::vector<float>(nVtx+5,-1);
      std::vector< float > fittedCov = std::vector<float>(nVtx+5,-1); //only store the diagonal terms
      if(vtxPositions.rows()>4 ) {
        fittedPosition[0] = vtxPositions[Trk::jet_xv]; //position x,y,z of PV
        fittedPosition[1] = vtxPositions[Trk::jet_yv]; 
        fittedPosition[2] = vtxPositions[Trk::jet_zv]; 
        fittedPosition[3] = vtxPositions[Trk::jet_phi];  // direction of the jet axis
        fittedPosition[4] = vtxPositions[Trk::jet_theta];
              
        fittedCov[0] = vtxCovMatrix(0,0);
        fittedCov[1] = vtxCovMatrix(1,1);
        fittedCov[2] = vtxCovMatrix(2,2);
        fittedCov[3] = vtxCovMatrix(3,3);
        fittedCov[4] = vtxCovMatrix(4,4);
        
      }

      for(int i=0; i<nVtx; ++i){
        fittedPosition[i+5] = vtxPositions[i+5]; //dist of vtxi on jet axis from PV
        fittedCov[i+5] = vtxCovMatrix(i+5,i+5);
      }
      
      newBTag->setVariable<std::vector< float > >(basename, "fittedPosition", fittedPosition);
      newBTag->setVariable<std::vector< float > >(basename, "fittedCov", fittedCov);
    
      //trks at PV
      const std::vector<Trk::VxTrackAtVertex*> & trackatPV = vxjetcand->getPrimaryVertex()->getTracksAtVertex();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irBegin = trackatPV.begin();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irEnd   = trackatPV.end();
      std::vector< float > tracksAtPVchi2;
      std::vector< float > tracksAtPVndf;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > tracksAtPVlinks;

      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator it=irBegin; it!=irEnd; ++it) {
        const Trk::FitQuality& trkquality = (*it)->trackQuality();
        double tmpchi2 = trkquality.chiSquared();
        int tmpndf = trkquality.numberDoF();
        tracksAtPVchi2.push_back(float(tmpchi2));
        tracksAtPVndf.push_back(float(tmpndf));
        //links
        Trk::ITrackLink* trklinks = (*it)->trackOrParticleLink();
        const Trk::LinkToXAODTrackParticle* trkLinkTPxAOD = dynamic_cast<const Trk::LinkToXAODTrackParticle *>(trklinks);
        if (!trkLinkTPxAOD) {
          ATH_MSG_WARNING("#BTAG# bad ITrackLink is not a LinkToXAODTrackParticle");
          continue;
        }
        const xAOD::TrackParticle* myTrklink = **trkLinkTPxAOD;
        ElementLink<xAOD::TrackParticleContainer> tpel;
        tpel.toContainedElement(*theTrackParticleContainer, myTrklink);
        tracksAtPVlinks.push_back(tpel);
      }
    
      newBTag->setVariable<std::vector< float > >(basename, "tracksAtPVchi2", tracksAtPVchi2);
      newBTag->setVariable<std::vector< float > >(basename, "tracksAtPVndf", tracksAtPVndf);
      newBTag->setVariable<std::vector< ElementLink< xAOD::TrackParticleContainer > > >(basename, "tracksAtPVlinks", tracksAtPVlinks);  
      newBTag->setDynTPELName(basename, "tracksAtPVlinks");

      
    }
    else { //No JF vertices
      ATH_MSG_ERROR("#BTAG# No JF vertices. Minimum 1");
    }

    StatusCode sc = m_JFvarFactory->fillJetFitterVariables(myJet, newBTag, myVertexInfoJetFitter, basename);
    if(sc.isFailure()){
      ATH_MSG_ERROR("#BTAG# error filling variables in JetFitterVariablesFactory" );
      return sc;
    }

    return StatusCode::SUCCESS;


  }



} // namespace
