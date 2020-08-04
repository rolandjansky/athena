/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/IBTagLightSecVertexing.h"
#include "BTagging/BTagLightSecVertexing.h"

#include "xAODCore/ShallowCopy.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/PrimaryVertexSelector.h"

//general interface for secondary vertex finders
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
//#include "JetTagTools/MSVVariablesFactory.h"

#include "xAODBTagging/BTagging.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h" 

#include "StoreGate/ReadDecorHandle.h"

namespace Analysis {

  BTagLightSecVertexing::BTagLightSecVertexing(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_JFvarFactory("Analysis::JetFitterVariablesFactory",this),
    //m_MSVvarFactory("Analysis::MSVVariablesFactory",this),
    m_vxPrimaryName("PrimaryVertices")
  {
    declareInterface<IBTagLightSecVertexing>(this);

    declareProperty("PrimaryVertexName",  m_vxPrimaryName);
    //List of the secondary vertex finders in jet to be used
    declareProperty("SecVtxFinderTrackNameList", m_secVertexFinderTrackNameList);
    declareProperty("SecVtxFinderxAODBaseNameList", m_secVertexFinderBaseNameList);
    declareProperty("JetFitterVariableFactory",          m_JFvarFactory);
    //declareProperty("MSVVariableFactory",          m_MSVvarFactory);
  }

  BTagLightSecVertexing::~BTagLightSecVertexing() {
  }

  StatusCode BTagLightSecVertexing::initialize()
  {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_VertexCollectionName.initialize() );
    ATH_CHECK( m_jetSVLinkName.initialize() );
    ATH_CHECK( m_jetJFVtxLinkName.initialize() );
    ATH_CHECK( m_VxSecVertexInfoNames.initialize() );

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if ( m_secVertexFinderBaseNameList.empty()) {
      ATH_MSG_ERROR("SecVtxBaseNameList is empty");
      return StatusCode::FAILURE;
    }

    if(m_secVertexFinderTrackNameList.size() != m_secVertexFinderBaseNameList.size()){
      ATH_MSG_ERROR("#BTAG# Size mismatch between secVertexFinderBaseName and track name list ");
      return StatusCode::FAILURE;
    }

    if ( m_JFvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_JFvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_JFvarFactory);
    }
    /*if ( m_MSVvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_MSVvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_MSVvarFactory);
    }*/

    return StatusCode::SUCCESS;
  }

  StatusCode BTagLightSecVertexing::fillVkalVariables(const xAOD::Jet& myJet,
             xAOD::BTagging* newBTag,
						 const Trk::VxSecVKalVertexInfo* myVertexInfoVKal,
						 const xAOD::TrackParticleContainer* theTrackParticleContainer,
						 std::string basename) const {

    
    SG::ReadDecorHandle<xAOD::JetContainer, std::vector<ElementLink< xAOD::VertexContainer> > > h_jetSVLinkName (m_jetSVLinkName);
    std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
    if (myVertexInfoVKal) {
      if (!h_jetSVLinkName.isAvailable()) {
        ATH_MSG_ERROR( " cannot retrieve vertex container EL decoration with key " << m_jetSVLinkName.key()  );
        return StatusCode::FAILURE;
      }
      SVertexLinks = h_jetSVLinkName(myJet);
    }

    std::vector<xAOD::Vertex*> vertices;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkList;
    float    mass = 0, energyfrc = NAN, energyTrk = 0, dsttomatlayer = NAN;
    int  n2trk = 0, npsec = 0;

    if(basename.find("MSV") != 0){
      for (std::vector<xAOD::Vertex*>::const_iterator verticesIter = vertices.begin();
           verticesIter!=vertices.end();++verticesIter) {
        std::vector<ElementLink<xAOD::TrackParticleContainer> > theseTracks = (*verticesIter)->trackParticleLinks();
        npsec += theseTracks.size();
        for (std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator itr=theseTracks.begin();itr!=theseTracks.end();itr++){
	        TrkList.push_back(*itr);
        }
      }

      ATH_MSG_DEBUG("#BTAG# Size of the sec vertex linked to the BTagging: " << SVertexLinks.size());
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
        newBTag->setTaggerInfo(mass, xAOD::BTagInfo::SV1_masssvx);
        newBTag->setTaggerInfo(energyfrc, xAOD::BTagInfo::SV1_efracsvx);
        newBTag->setTaggerInfo(n2trk, xAOD::BTagInfo::SV1_N2Tpair);
        newBTag->setTaggerInfo(npsec, xAOD::BTagInfo::SV1_NGTinSvx);
        newBTag->setSV1_TrackParticleLinks(TrkList);
      }
      else if ("SV0" == basename){
        newBTag->setTaggerInfo(mass, xAOD::BTagInfo::SV0_masssvx);
        newBTag->setTaggerInfo(energyfrc, xAOD::BTagInfo::SV0_efracsvx);
        newBTag->setTaggerInfo(n2trk, xAOD::BTagInfo::SV0_N2Tpair);
        newBTag->setTaggerInfo(npsec, xAOD::BTagInfo::SV0_NGTinSvx);
        newBTag->setSV0_TrackParticleLinks(TrkList);
      } else{
        newBTag->setVariable<float>(basename, "masssvx", mass);
        newBTag->setVariable<float>(basename, "efracsvx", energyfrc);
        newBTag->setVariable<int>(basename, "N2Tpair", n2trk);
        newBTag->setVariable<int>(basename, "NGTinSvx", npsec);
        newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "TrackParticleLinks", TrkList);
        newBTag->setDynTPELName(basename, "TrackParticleLinks");
      }
    }//no msv
    std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
    if(theTrackParticleContainer && myVertexInfoVKal){
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
    }
    newBTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
    newBTag->setDynTPELName(basename, "badTracksIP");

    return StatusCode::SUCCESS;

  }

  StatusCode BTagLightSecVertexing::fillJFVariables(const xAOD::Jet& myJet,
					       xAOD::BTagging* newBTag,
					       const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter,
					       const xAOD::TrackParticleContainer* theTrackParticleContainer,
					       std::string basename) const {

    SG::ReadDecorHandle<xAOD::JetContainer, std::vector<ElementLink< xAOD::BTagVertexContainer> > > h_jetJFVtxLinkName (m_jetJFVtxLinkName);
    std::vector< ElementLink< xAOD::BTagVertexContainer > > JFVerticesLinks;

    //twotrackVerticesInJet
    std::vector< const xAOD::Vertex*> vecTwoTrkVtx;
    if (myVertexInfoJetFitter) {
      const Trk::TwoTrackVerticesInJet* TwoTrkVtxInJet = myVertexInfoJetFitter->getTwoTrackVerticesInJet();
      vecTwoTrkVtx = TwoTrkVtxInJet->getTwoTrackVertice();
      if (!h_jetJFVtxLinkName.isAvailable()) {
        ATH_MSG_ERROR( " cannot retrieve vertex container EL decoration with key " << m_jetJFVtxLinkName.key()  );
        return StatusCode::FAILURE;
      }
      JFVerticesLinks = h_jetJFVtxLinkName(myJet);
    }

    int N2TrkVtx = vecTwoTrkVtx.size();
    if("JetFitter" == basename){
      newBTag->setTaggerInfo(N2TrkVtx, xAOD::BTagInfo::JetFitter_N2Tpair);
    }
    else{
      newBTag->setVariable<int>(basename, "N2Tpair", N2TrkVtx);
    }

    //list of JFvertices
    std::vector<Trk::VxJetCandidate*> JFvertices;
    if (myVertexInfoJetFitter) JFvertices = myVertexInfoJetFitter->verticesJF();

    int nVtx = 0;
    Trk::VxJetCandidate* vxjetcand = nullptr;
    std::vector<Trk::VxVertexOnJetAxis*> Vtxonjetaxes;
    if (JFvertices.size() > 0) {
      vxjetcand = dynamic_cast< Trk::VxJetCandidate*>(JFvertices[0]);
      if (!vxjetcand) {
        ATH_MSG_WARNING("#BTAG# bad VxCandidate is not a VxJetCandidate");
        return StatusCode::SUCCESS;
      }

      //vtx on jet axis
      Vtxonjetaxes = vxjetcand->getVerticesOnJetAxis();
    }

    nVtx = Vtxonjetaxes.size();
    typedef std::vector<ElementLink<xAOD::BTagVertexContainer> > BTagVertices;
    ATH_MSG_DEBUG("#BTAGJF# filling vertices for basename: " << basename);
    newBTag->setVariable<std::vector<ElementLink<xAOD::BTagVertexContainer> > >(basename, "JFvertices", JFVerticesLinks);
    newBTag->setDynBTagVxELName(basename, "JFvertices");
    ATH_MSG_DEBUG("#BTAGJF# n vertices: " << newBTag->auxdata<BTagVertices>(basename + "_JFvertices").size());

    Amg::VectorX vtxPositions = Amg::VectorX::Zero(5);
    Amg::MatrixX vtxCovMatrix = Amg::MatrixX::Zero(5,5);
    if (nVtx > 0){
      const Trk::RecVertexPositions& recVtxposition = vxjetcand->getRecVertexPositions();
      vtxPositions = recVtxposition.position();
      vtxCovMatrix = recVtxposition.covariancePosition();
      ATH_MSG_DEBUG("#BTAGJF# size vtxPosition "<<vtxPositions.size());
    }
    std::vector< float > fittedPosition = std::vector<float>(nVtx+5,-1);
    std::vector< float > fittedCov = std::vector<float>(nVtx+5,-1); //only store the diagonal terms
    if(vtxPositions.rows()>4 ) {
      fittedPosition[0] = vtxPositions[Trk::jet_xv]; //position x,y,z of PV
      fittedPosition[1] = vtxPositions[Trk::jet_yv];
      fittedPosition[2] = vtxPositions[Trk::jet_zv];
      fittedPosition[3] = nVtx > 0 ? vtxPositions[Trk::jet_phi] : NAN;  // direction of the jet axis
      fittedPosition[4] = nVtx > 0 ? vtxPositions[Trk::jet_theta] : NAN;

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
    std::vector<Trk::VxTrackAtVertex*> trackatPV;
    std::vector< float > tracksAtPVchi2;
    std::vector< float > tracksAtPVndf;
    std::vector< ElementLink< xAOD::TrackParticleContainer > > tracksAtPVlinks;
    if (vxjetcand) {
      trackatPV = vxjetcand->getPrimaryVertex()->getTracksAtVertex();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irBegin = trackatPV.begin();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irEnd   = trackatPV.end();
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
    }

    newBTag->setVariable<std::vector< float > >(basename, "tracksAtPVchi2", tracksAtPVchi2);
    newBTag->setVariable<std::vector< float > >(basename, "tracksAtPVndf", tracksAtPVndf);
    newBTag->setVariable<std::vector< ElementLink< xAOD::TrackParticleContainer > > >(basename, "tracksAtPVlinks", tracksAtPVlinks);
    newBTag->setDynTPELName(basename, "tracksAtPVlinks");

    StatusCode sc = m_JFvarFactory->fillJetFitterVariables(myJet, newBTag, myVertexInfoJetFitter, basename);
    if(sc.isFailure()){
      ATH_MSG_ERROR("#BTAG# error filling variables in JetFitterVariablesFactory" );
      return sc;
    }

    return StatusCode::SUCCESS;

  }

  StatusCode BTagLightSecVertexing::BTagSecVertexing_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) const {

    const xAOD::Vertex* primaryVertex(0);

    //retrieve primary vertex
    SG::ReadHandle<xAOD::VertexContainer> h_VertexCollectionName (m_VertexCollectionName);
    if (!h_VertexCollectionName.isValid()) {
        ATH_MSG_ERROR( " cannot retrieve primary vertex container with key " << m_VertexCollectionName.key()  );
        return StatusCode::FAILURE;
    }
    unsigned int nVertexes = h_VertexCollectionName->size();
    if (nVertexes == 0) {
      ATH_MSG_DEBUG("#BTAG#  Vertex container is empty");
      return StatusCode::SUCCESS;
    }
    for (xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin(); fz != h_VertexCollectionName->end(); ++fz) {
      if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	      primaryVertex = *fz;
	      break;
      }
    }


    if (! primaryVertex) {
      ATH_MSG_DEBUG("#BTAG#  No vertex labeled as VxType::PriVtx!");
      xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin();
      primaryVertex = *fz;
      if (primaryVertex->nTrackParticles() == 0) {
	      ATH_MSG_DEBUG("#BTAG#  PV==BeamSpot: probably poor tagging");
      }
    }

    int nameiter = 0;
    for(SG::ReadHandleKey<Trk::VxSecVertexInfoContainer> infoCont : m_VxSecVertexInfoNames) {
      SG::ReadHandle<Trk::VxSecVertexInfoContainer> h_VxSecVertexInfoName(infoCont);
      if (h_VxSecVertexInfoName.isValid()) {
        if (h_VxSecVertexInfoName->size() != jetContainer->size()) {
          ATH_MSG_ERROR("Size of jet container different than size of VxSecVertexInfo: " << infoCont.key());
          return StatusCode::FAILURE;
        }

        Trk::VxSecVertexInfoContainer::const_iterator infoSVIter = h_VxSecVertexInfoName->begin();
        xAOD::BTaggingContainer::iterator btagIter=btaggingContainer->begin();

        for (xAOD::JetContainer::const_iterator jetIter = jetContainer->begin(); jetIter != jetContainer->end(); ++jetIter, ++btagIter, ++infoSVIter) {
          const xAOD::Jet& jetToTag = **jetIter;
          const Trk::VxSecVertexInfo& myVertexInfo = **infoSVIter;

          const xAOD::TrackParticleContainer* theTrackParticleContainer = nullptr;

          std::string trackname = m_secVertexFinderTrackNameList[nameiter];
          std::string basename =  m_secVertexFinderBaseNameList[nameiter];

          std::vector<ElementLink< xAOD::TrackParticleContainer > > tracksInJet;
          tracksInJet = (*btagIter)->auxdata< std::vector<ElementLink< xAOD::TrackParticleContainer > > >(trackname);

          std::vector<ElementLink< xAOD::TrackParticleContainer > >::iterator itEL = tracksInJet.begin();
          std::vector<ElementLink< xAOD::TrackParticleContainer > >::iterator itELend = tracksInJet.end();

          if(tracksInJet.size()==0){
            ATH_MSG_DEBUG("#BTAG# no tracks associated to the jet. Set some with the track selection tool " << trackname << " for VertexFinderxAODBaseName "<< basename);
            if("SV1" == basename){
              std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkList;
              (*btagIter)->setSV1_TrackParticleLinks(TrkList);
              std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
              (*btagIter)->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
              (*btagIter)->setDynTPELName(basename, "badTracksIP");
              std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
              (*btagIter)->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", SVertexLinks);
              (*btagIter)->setDynVxELName(basename, "vertices");
            }
	          else if("MSV" == basename){
              std::vector<ElementLink<xAOD::TrackParticleContainer> > badtrackEL;
              (*btagIter)->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(basename, "badTracksIP", badtrackEL);
              (*btagIter)->setDynTPELName(basename, "badTracksIP");
              std::vector< ElementLink< xAOD::VertexContainer > > SVertexLinks;
              (*btagIter)->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", SVertexLinks);
              (*btagIter)->setDynVxELName(basename, "vertices");
	          }
	          else if("JetFitter" == basename) {
              std::vector< ElementLink< xAOD::TrackParticleContainer > > tracksAtPVlinks;
              (*btagIter)->setVariable<std::vector< ElementLink< xAOD::TrackParticleContainer > > >(basename, "tracksAtPVlinks", tracksAtPVlinks);
              (*btagIter)->setDynTPELName(basename, "tracksAtPVlinks");
            }
          }

          for (  ; itEL != itELend; ++itEL ) {
	          /// warning -> will not work if at some point we decide to associate to several track collections at the same time (in the same assoc object)

            theTrackParticleContainer = (*itEL).getStorableObjectPointer();
          }

          if (basename == "SV1") {
            const Trk::VxSecVKalVertexInfo* myVertexInfoVKal = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(&myVertexInfo);
	          ATH_MSG_DEBUG("#BTAG# Found valid VKalVertexInfo information: " << infoCont.key());
	          StatusCode sc = fillVkalVariables(**jetIter, *btagIter, myVertexInfoVKal, theTrackParticleContainer, basename);
	          if(sc.isFailure()){
	            ATH_MSG_ERROR("#BTAG# error filling variables from VxSecVKalVertexInfo for " << basename);
	            return sc;
	          }
          } else if (basename == "JetFitter") {
            const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter = dynamic_cast<const Trk::VxJetFitterVertexInfo*>(&myVertexInfo);
            ATH_MSG_DEBUG("#BTAG# Found valid VxJetFitterVertexInfo information: " << infoCont.key());
            StatusCode sc = fillJFVariables(jetToTag, *btagIter, myVertexInfoJetFitter, theTrackParticleContainer, basename);
            if(sc.isFailure()){
              ATH_MSG_ERROR("#BTAG# error filling variables from VxJetFitterVertexInfo for " << basename);
              return sc;
            }
          } else {
	          ATH_MSG_DEBUG("#BTAG# VxSecVertexInfo pointer cannot be interpreted for " << basename);
          }
        } /// for loop on jets
      }
      else {
        ATH_MSG_DEBUG("#BTAG# VxJetFitterVertexInfo container " << infoCont.key() << " is not valid");
      }
      ++nameiter;
    }// for loop on sec vertex info containers

    return StatusCode::SUCCESS;
  }

} // namespace
