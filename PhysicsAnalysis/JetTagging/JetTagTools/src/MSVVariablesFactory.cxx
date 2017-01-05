/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : MSVVariableFactory.h
/// Created : sept 14
///
/// DESCRIPTION:
///
/// This class is a "variable factory". It generates all
/// the variables per vertex to MSV.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxVertex/VxTrackAtVertex.h"
//#include "VxJetVertex/VxJetCandidate.h"
//#include "VxJetVertex/SelectedTracksInJet.h"

#include "TrkParameters/TrackParameters.h"
//#include "TrkNeutralParameters/MeasuredNeutralPerigee.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"

//#include "TrkTrackLink/ITrackLink.h"

#include <TMath.h>

#include "CLHEP/Vector/LorentzVector.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "JetTagTools/MSVVariablesFactory.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include <vector>
//
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODBTagging/SecVtxHelper.h"
#include <string>

namespace Analysis {

  MSVVariablesFactory::MSVVariablesFactory(const std::string& name,
						       const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p)
//    m_secVxFinderName("InDetVKalVxInJetTool"),

  {
//    declareProperty("secVxFinderName",m_secVxFinderName);
    declareInterface<IMSVVariablesFactory>(this);
  }

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

  MSVVariablesFactory::~MSVVariablesFactory() {}

StatusCode MSVVariablesFactory::initialize() {
  ATH_MSG_DEBUG(" Initialization of MSVVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

StatusCode MSVVariablesFactory::finalize() {
  ATH_MSG_DEBUG(" Finalization of MSVVariablesFactory succesfull");
  return StatusCode::SUCCESS;
}

  StatusCode MSVVariablesFactory::fillMSVVariables(const xAOD::Jet &myJet, xAOD::BTagging* BTag, const Trk::VxSecVKalVertexInfo* myVertexInfoVKal, xAOD::VertexContainer* VertexContainer,const xAOD::Vertex& PrimaryVtx,  std::string basename) const {
    //...
    int nvsec = 0;
    float jetenergy=0.;
    int n2t = 0;
    float distnrm=0.;
    const xAOD::Vertex* priVtx = &PrimaryVtx;
    std::vector< ElementLink< xAOD::VertexContainer > > MSVVertexLinks;
    const std::vector<xAOD::Vertex*> myVertices = myVertexInfoVKal->vertices();
    if(myVertices.size() == 0){
      ATH_MSG_DEBUG("#BTAG# no MSV vertices...fill default values only... ");
      BTag->setVariable<int>(basename, "N2Tpair", n2t);
      BTag->setVariable<float>(basename, "energyTrkInJet", jetenergy);
      BTag->setVariable<int>(basename, "nvsec", nvsec);
      BTag->setVariable<float>(basename, "normdist", distnrm);
      BTag->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", MSVVertexLinks);
      BTag->setDynVxELName(basename, "vertices");
      xAOD::Vertex* Vertex = new xAOD::Vertex();
      VertexContainer->push_back(Vertex);
      xAOD::SecVtxHelper::setVertexMass(Vertex, -9.);
      xAOD::SecVtxHelper::setEnergyFraction(Vertex, -9.);
      xAOD::SecVtxHelper::setVtxNtrk(Vertex, -9);
      xAOD::SecVtxHelper::setVtxpt(Vertex, -9.);
      xAOD::SecVtxHelper::setVtxeta(Vertex, -9.);
      xAOD::SecVtxHelper::setVtxphi(Vertex, -9.);
      xAOD::SecVtxHelper::setVtxnormDist(Vertex, -9.);      
      return StatusCode::SUCCESS;

    }
    std::vector<xAOD::Vertex*>::const_iterator verticesBegin = myVertexInfoVKal->vertices().begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd   = myVertexInfoVKal->vertices().end();

    jetenergy = myVertexInfoVKal->energyTrkInJet();
    n2t = myVertexInfoVKal->n2trackvertices();
    BTag->setVariable<int>(basename, "N2Tpair", n2t);
    BTag->setVariable<float>(basename, "energyTrkInJet", jetenergy);

    std::vector<const xAOD::Vertex*> vecVertices;
    for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin; verticesIter!=verticesEnd;++verticesIter) {

      xAOD::Vertex* Vertex = *verticesIter;
      VertexContainer->push_back(Vertex);
      //additional info per vertex
      vecVertices.push_back(*verticesIter);
      double sumpx = 0.0;
      double sumpy = 0.0;
      double sumpz = 0.0;
      double sume = 0.0;
      const std::vector<ElementLink<xAOD::TrackParticleContainer> > myTrackLinks = (*verticesIter)->trackParticleLinks();
      if (myTrackLinks.size()==0) {
        ATH_MSG_WARNING("#BTAG# No Track Links attached to the track at the sec vertex... ");
      }
      int npsec = 0;
      const std::vector<Trk::VxTrackAtVertex> myTracks=(*verticesIter)->vxTrackAtVertex();
      if (myTracks.size()!=0) {
        npsec=myTracks.size();
        const std::vector<Trk::VxTrackAtVertex>::const_iterator tracksBegin=myTracks.begin();
        const std::vector<Trk::VxTrackAtVertex>::const_iterator tracksEnd=myTracks.end();
        for (std::vector<Trk::VxTrackAtVertex>::const_iterator tracksIter=tracksBegin; tracksIter!=tracksEnd;++tracksIter) {
          const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*>((*tracksIter).perigeeAtVertex());
          if(perigee){
            sumpx += perigee->momentum().x();
            sumpy += perigee->momentum().y();
            sumpz += perigee->momentum().z();
            sume +=sqrt(perigee->momentum().mag()*perigee->momentum().mag() + 139.5702*139.5702 );
          }else{
             ATH_MSG_WARNING("#BTAG# perigee for VxTrackAtVertex not found");
          }
        }
      }
      CLHEP::HepLorentzVector vtxp4(sumpx,sumpy,sumpz,sume);
      float efrac = (jetenergy>0)?vtxp4.e()/jetenergy:0;
      xAOD::SecVtxHelper::setVertexMass(Vertex, vtxp4.m());
      xAOD::SecVtxHelper::setEnergyFraction(Vertex, efrac);
      xAOD::SecVtxHelper::setVtxNtrk(Vertex, npsec);
      xAOD::SecVtxHelper::setVtxpt(Vertex, vtxp4.perp());
      xAOD::SecVtxHelper::setVtxeta(Vertex, vtxp4.eta());
      xAOD::SecVtxHelper::setVtxphi(Vertex, vtxp4.phi());

      ATH_MSG_DEBUG("#BTAG# mass per vertex = "<<vtxp4.m());
      double localdistnrm=0;
      std::vector<const xAOD::Vertex*> vecVtxHolder;
      vecVtxHolder.push_back(*verticesIter);

         ATH_MSG_DEBUG("Factory PVX x = " << priVtx->x() << " y = " << priVtx->y() << " z = " << priVtx->z());
      if (priVtx) {
        localdistnrm=get3DSignificance(priVtx, vecVtxHolder, Amg::Vector3D(myJet.p4().Px(),myJet.p4().Py(),myJet.p4().Pz()));
      } else {
        ATH_MSG_WARNING("#BTAG# Tagging requested, but no primary vertex supplied.");
        localdistnrm=0.;
      }
      xAOD::SecVtxHelper::setVtxnormDist(Vertex, localdistnrm);
      //track links,
      Vertex->setTrackParticleLinks(myTrackLinks);

      ElementLink< xAOD::VertexContainer> linkBTagVertex;
      linkBTagVertex.toContainedElement(*VertexContainer, Vertex);
      MSVVertexLinks.push_back(linkBTagVertex);
    } //end loop vertexcontainer

    BTag->setVariable<std::vector<ElementLink<xAOD::VertexContainer> > >(basename, "vertices", MSVVertexLinks);
    BTag->setDynVxELName(basename, "vertices");

    if (priVtx) {
      distnrm=get3DSignificance(priVtx, vecVertices, Amg::Vector3D(myJet.p4().Px(),myJet.p4().Py(),myJet.p4().Pz()));
    } else {
      ATH_MSG_WARNING("#BTAG# Tagging requested, but no primary vertex supplied.");
      distnrm=0.;
    }
    nvsec = vecVertices.size();
    BTag->setVariable<int>(basename, "nvsec", nvsec);
    BTag->setVariable<float>(basename, "normdist", distnrm);

    //...
    return StatusCode::SUCCESS;


  }

  double MSVVariablesFactory::get3DSignificance(const xAOD::Vertex* priVertex,
                                       std::vector<const xAOD::Vertex*>& secVertex,
                                        const Amg::Vector3D jetDirection) const {
    if(!secVertex.size()) return 0;
    std::vector<Amg::Vector3D> positions;
    std::vector<AmgSymMatrix(3)> weightMatrices;
    std::vector<const xAOD::Vertex*>::const_iterator secEnd = secVertex.end();
    for (std::vector<const xAOD::Vertex*>::const_iterator secIter = secVertex.begin(); secIter != secEnd; ++secIter){
      positions.push_back((*secIter)->position());
      weightMatrices.push_back((*secIter)->covariancePosition().inverse());
    }
    Amg::Vector3D weightTimesPosition(0.,0.,0.);
    AmgSymMatrix(3) sumWeights;
    sumWeights.setZero();

    int count=0;
    for (std::vector<const xAOD::Vertex*>::const_iterator secIter = secVertex.begin(); secIter != secEnd; ++secIter) {
      weightTimesPosition+=(weightMatrices[count])*positions[count];
      sumWeights+=(weightMatrices[count]);
      ++count;
    }
    bool invertible;
    AmgSymMatrix(3) meanCovariance;
    meanCovariance.setZero();
    sumWeights.computeInverseWithCheck(meanCovariance, invertible);
    if (! invertible) {
      ATH_MSG_WARNING("#BTAG# Could not invert sum of sec vtx matrices");
      return 0.;
    }
    Amg::Vector3D meanPosition=meanCovariance*weightTimesPosition;
    AmgSymMatrix(3) covariance = meanCovariance + priVertex->covariancePosition();

    double Lx = meanPosition[0]-priVertex->position().x();
    double Ly = meanPosition[1]-priVertex->position().y();
    double Lz = meanPosition[2]-priVertex->position().z();

    const double decaylength = sqrt(Lx*Lx + Ly*Ly + Lz*Lz);
    const double inv_decaylength = 1. / decaylength;
    double dLdLx = Lx * inv_decaylength;
    double dLdLy = Ly * inv_decaylength;
    double dLdLz = Lz * inv_decaylength;
    double decaylength_err = sqrt(dLdLx*dLdLx*covariance(0,0) +
                             dLdLy*dLdLy*covariance(1,1) +
                             dLdLz*dLdLz*covariance(2,2) +
                             2.*dLdLx*dLdLy*covariance(0,1) +
                             2.*dLdLx*dLdLz*covariance(0,2) +
                             2.*dLdLy*dLdLz*covariance(1,2));

   double decaylength_significance = 0.;
   if (decaylength_err != 0.) decaylength_significance = decaylength/decaylength_err;
   double L_proj_jetDir = jetDirection.x()*Lx + jetDirection.y()*Ly + jetDirection.z()*Lz;
   if (L_proj_jetDir < 0.) decaylength_significance *= -1.;

   return decaylength_significance;


  }

}//end Analysis namespace
