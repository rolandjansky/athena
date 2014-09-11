/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MultiSVTag.cxx
***************************************************************************/
#include "JetTagTools/MultiSVTag.h"

//#include "JetEvent/Jet.h"
#include "GaudiKernel/IToolSvc.h"
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SVInfoBase.h"
#include "JetTagInfo/MultiSVInfoPlus.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/ITHistSvc.h"
#include "JetTagTools/HistoHelperRoot.h"

#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "xAODTracking/TrackParticle.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"

#include <string>
 
namespace Analysis
{

  MultiSVTag::MultiSVTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_secVxFinderName("InDetVKalVxInJetTool")
  {
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus",       m_runModus= "analysis");
    declareProperty("writeInfoBase",  m_writeInfoBase = true);
    declareProperty("writeInfoPlus",  m_writeInfoPlus = true);
    declareProperty("infoPlusName",   m_infoPlusName = "MutliSVInfoPlus");
    declareProperty("originalTPCollectionName", m_originalTPCollectionName = "TrackParticleCandidate");
    declareProperty("jetCollectionList", m_jetCollectionList);
    declareProperty("jetWithInfoPlus", m_jetWithInfoPlus);
    declareProperty("SecVxFinderName",m_secVxFinderName);
  }

  MultiSVTag::~MultiSVTag() {
  }

  StatusCode MultiSVTag::initialize() {
    return StatusCode::SUCCESS;                       
  }


  StatusCode MultiSVTag::finalize()
  {
 
    return StatusCode::SUCCESS;
  }

  void MultiSVTag::tagJet(xAOD::Jet& jetToTag){

    /* Do not keep the InfoPlus for all jet collection */
    bool keepInfoPlus = false;

    if(m_writeInfoPlus){
      std::string author = jetToTag.jetAuthor();
      if (std::find( m_jetWithInfoPlus.begin(), 
		     m_jetWithInfoPlus.end(), 
		     author ) != m_jetWithInfoPlus.end()) keepInfoPlus = true;

      const MultiSVInfoPlus *dummy = jetToTag.tagInfo<MultiSVInfoPlus>(m_infoPlusName);
      if(dummy) {
	ATH_MSG_VERBOSE("#BTAG# Previous MultiSVInfoPlus " << m_infoPlusName << " found...");
	keepInfoPlus = false;
      }
    }

    if(keepInfoPlus) ATH_MSG_VERBOSE("#BTAG# Writing infoPlus for this tagger and this jet type.");


    if(keepInfoPlus) {
      StatusCode sc = evtStore()->retrieve(m_originalTPCollection, m_originalTPCollectionName);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("#BTAG# " << m_originalTPCollectionName << " not found in StoreGate.");
	return;
      } 
      else{
	ATH_MSG_VERBOSE("#BTAG# TrackParticleContainer " << m_originalTPCollectionName << " found.");
      }
    }
    

   
    const ISvxAssociation* newSvxAssociation=jetToTag.getAssociation<ISvxAssociation>(m_secVxFinderName);
    if (newSvxAssociation==0) {
      ATH_MSG_DEBUG( "#BTAG# No VKalVrt vertex found, attached to the Jet as association. Not going on with tagging...");
      return;
    } 
    else {
      ATH_MSG_DEBUG( "#BTAG# VKalVrt vertex info " << m_secVxFinderName << " found.");
    }

    std::vector<const Trk::RecVertex*> secondaryRecVertices;
    MultiSVInfoPlus *multiSVInfoPlusObject=0;
    int npsec=0;
    int NSVPair=0;
    double jetenergy=0;

    const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();
    if(myVertexInfo){
      const Trk::VxSecVKalVertexInfo* myVKalVertexInfo=dynamic_cast<const Trk::VxSecVKalVertexInfo*>(myVertexInfo);
      if (myVKalVertexInfo==0) {
	ATH_MSG_WARNING( "#BTAG# Could not cast to a VKalVrt Info object ");
	return;
      } 

      NSVPair=(int)(myVKalVertexInfo->n2trackvertices());
      jetenergy= myVKalVertexInfo->energyTrkInJet();

      const std::vector<Trk::VxCandidate*> & myVertices=myVKalVertexInfo->vertices();

      const std::vector<Trk::VxCandidate*>::const_iterator verticesBegin=myVertices.begin();
      const std::vector<Trk::VxCandidate*>::const_iterator verticesEnd=myVertices.end();
      for (std::vector<Trk::VxCandidate*>::const_iterator verticesIter=verticesBegin;
	   verticesIter!=verticesEnd;verticesIter++) {
	if ((*verticesIter)==0){
	    ATH_MSG_ERROR("#BTAG# Secondary vertex from InDetVKalVxInJetFinder has zero pointer. Skipping... ");
	    continue;
	  }

	secondaryRecVertices.push_back(&((*verticesIter)->recVertex()));

	if(!keepInfoPlus) continue;

	double sumpx = 0.0; 
	double sumpy = 0.0; 
	double sumpz = 0.0; 
	double sume = 0.0; 

	std::vector<const xAOD::TrackParticle*> TrkList;
	const std::vector<Trk::VxTrackAtVertex*>* myTracks=(*verticesIter)->vxTrackAtVertex();
	if (myTracks!=0) {
	  npsec+=myTracks->size();
	  const std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksBegin=myTracks->begin();
	  const std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksEnd=myTracks->end();
	  for (std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksIter=tracksBegin;
	       tracksIter!=tracksEnd;++tracksIter) {
	    const Trk::ITrackLink* myTrackLink=((*tracksIter)->trackOrParticleLink());
	    //
	    if (myTrackLink==0) {
	      ATH_MSG_WARNING("#BTAG# No ITrack Link attached to the track at the sec vertex... ");
	    }
	    const Trk::LinkToTrackParticleBase* myTPLink=
	      dynamic_cast<const Trk::LinkToTrackParticleBase*>(myTrackLink);
	    if (myTPLink!=0) {
	      TrkList.push_back(**myTPLink);
	    } else {
	      ATH_MSG_WARNING("#BTAG# Cannot cast to TrackParticlBase. Running on tracks not yet foreseen... ");
	    }

	    const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*>((*tracksIter)->perigeeAtVertex());
	    if(perigee){
	      sumpx += perigee->momentum().x();
	      sumpy += perigee->momentum().y();
	      sumpz += perigee->momentum().z();
	      sume +=sqrt(perigee->momentum().mag()*perigee->momentum().mag() + 139.5702*139.5702 );
	    }
	    else{
	      ATH_MSG_WARNING("#BTAG# perigee for VxTrackAtVertex not found");
	    }

	  }
	}

	if(!multiSVInfoPlusObject){
	  multiSVInfoPlusObject=new MultiSVInfoPlus(m_infoPlusName);
	}
	MSVVtxInfo* vtxinfo = new MSVVtxInfo();

	CLHEP::HepLorentzVector vtxp4(sumpx,sumpy,sumpz,sume);
	double efrac = (jetenergy>0)?vtxp4.e()/jetenergy:0;

	vtxinfo->setRecSvx((*verticesIter)->recVertex());
	vtxinfo->setEnergyFraction(efrac);
	vtxinfo->setMass(vtxp4.m());
	vtxinfo->setPt(vtxp4.perp());
	vtxinfo->setEta(vtxp4.eta());
	vtxinfo->setPhi(vtxp4.phi());

	std::vector<const Trk::RecVertex*> vtxholder;
	vtxholder.push_back(&((*verticesIter)->recVertex()));
	double localdistnrm=0;
	if (m_priVtx){
	    //TODO: getting Amg::Vector3D from HepLorentzVector is ugly. At least like this.
	  localdistnrm=get3DSignificance(m_priVtx->recVertex(),
					 vtxholder,
					 Amg::Vector3D(jetToTag.p4().Vect().x(), jetToTag.p4().Vect().y(), jetToTag.p4().Vect().z()));
	}
	vtxinfo->setNormDist(localdistnrm);
	for (uint itk = 0; itk < TrkList.size(); itk++) {
	  SVTrackInfo tinfo(m_originalTPCollection, TrkList[itk]);
	  vtxinfo->addTrackInfo(tinfo);
	}
	multiSVInfoPlusObject->addVtxInfo(vtxinfo); /// no need to delete; MultiSVInfoPlus will take care
      }

    }   


    double distnrm=0.;
    if (m_priVtx){
        //TODO: getting Amg::Vector3D from HepLorentzVector is ugly. At least like this.
      distnrm=get3DSignificance(m_priVtx->recVertex(),
				secondaryRecVertices,
				Amg::Vector3D(jetToTag.p4().Px(), jetToTag.p4().Py(), jetToTag.p4().Pz()));
    }
    else{
      ATH_MSG_WARNING("#BTAG# Tagging requested, but no primary vertex supplied.");
    }
    
    
    std::string iname( this->name().substr(8) );
    std::string instanceName(iname);
    std::string::size_type pos = iname.find("Tag");
    if ( pos!=std::string::npos ) {
      std::string prefix = iname.substr(0,pos);
      std::string posfix = iname.substr(pos+3);
      instanceName = prefix;
      instanceName += posfix;
    }
    SVInfoBase *mySVInfoBase(0);
    if (m_writeInfoBase) {
      ATH_MSG_VERBOSE("#BTAG# Filling SVInfoBase " << instanceName);
      mySVInfoBase = new SVInfoBase(instanceName);
      std::vector<double> probi;
      probi.push_back(distnrm);
      mySVInfoBase->setTagLikelihood(probi);
      jetToTag.addInfo(mySVInfoBase);
      mySVInfoBase->makeValid();
    }
   

    if(multiSVInfoPlusObject){
      multiSVInfoPlusObject->makeValid();
      multiSVInfoPlusObject->setNGTrackInJet(-1);//// not available for the moment
      multiSVInfoPlusObject->setNGTrackInSvx(npsec);
      multiSVInfoPlusObject->setN2T(NSVPair);
      multiSVInfoPlusObject->setNormDist(distnrm);
      jetToTag.addInfo(multiSVInfoPlusObject);
    }

  }



  double MultiSVTag::get3DSignificance(const Trk::RecVertex & priVertex,
                                  std::vector<const Trk::RecVertex* > & secVertex,
                                  const Amg::Vector3D jetDirection) {

    if(!secVertex.size()) return 0;
    
    std::vector<Amg::Vector3D> positions;
    std::vector<Amg::MatrixX> weightMatrices;

    std::vector<const Trk::RecVertex* >::const_iterator secBegin=secVertex.begin();
    std::vector<const Trk::RecVertex* >::const_iterator secEnd=secVertex.end();

    for (std::vector<const Trk::RecVertex* >::const_iterator secIter=secBegin;
         secIter!=secEnd;++secIter)
      {
	Amg::Vector3D position;
	position[0]=(*secIter)->position().x();
	position[1]=(*secIter)->position().y();
	position[2]=(*secIter)->position().z();
	positions.push_back(position);
      
	weightMatrices.push_back((*secIter)->covariancePosition().inverse());
      }
 

    // If multiple secondary vertices were reconstructed, then a common (weighted) position will be used
    // in the signed decay length significance calculation

    Amg::Vector3D weightTimesPosition(0.,0.,0.);
    AmgSymMatrix(3) sumWeights;
    sumWeights.setZero();

    int count=0;
    for (std::vector<const Trk::RecVertex* >::const_iterator secIter=secBegin;
         secIter!=secEnd;++secIter)
      {

	weightTimesPosition+=(weightMatrices[count])*positions[count];
	sumWeights+=(weightMatrices[count]);
      
	count+=1;
      }

    // now we have the sum of the weights, let's invert this matrix to get the mean covariance matrix

//    int failed(0);
//    AmgSymMatrix(3) meanCovariance=sumWeights.inverse();
    //undefined return value of not invertible matrix
//    if (failed!=0) {
//      ATH_MSG_ERROR("#BTAG# Could not invert sum of sec vtx matrices");
//      return 0.;
//    }
    bool invertible; 
    AmgSymMatrix(3) meanCovariance; 
    sumWeights.computeInverseWithCheck(meanCovariance, invertible); 
    if (not invertible) { 
       ATH_MSG_ERROR("#BTAG# Could not invert sum of sec vtx matrices"); 
       return 0.; 
    } 

    // calculate the weighted mean secondary vertex position
    Amg::Vector3D meanPosition=meanCovariance*weightTimesPosition;

    // add the mean covariance matrix of the secondary vertices to that of the primary vertex
    // this is the covariance matrix for the decay length
    AmgSymMatrix(3) covariance=meanCovariance
      +priVertex.covariancePosition();
    
    // ********
    // Calculate the signed decay length significance
    // ********

    double Lx = meanPosition[0]-priVertex.position().x();
    double Ly = meanPosition[1]-priVertex.position().y();
    double Lz = meanPosition[2]-priVertex.position().z();
    
    double decaylength = sqrt(Lx*Lx + Ly*Ly + Lz*Lz);
    
    double dLdLx = Lx/decaylength;
    double dLdLy = Ly/decaylength;
    double dLdLz = Lz/decaylength;
    double decaylength_err = sqrt(dLdLx*dLdLx*covariance(0,0) +
				  dLdLy*dLdLy*covariance(1,1) +
				  dLdLz*dLdLz*covariance(2,2) +
				  2.*dLdLx*dLdLy*covariance(0,1) +
				  2.*dLdLx*dLdLz*covariance(0,2) +
				  2.*dLdLy*dLdLz*covariance(1,2));
    
    double decaylength_significance = 0.;
    if (decaylength_err != 0.) decaylength_significance = decaylength/decaylength_err;

    // get sign from projection on jet axis
    double L_proj_jetDir = jetDirection.x()*Lx + jetDirection.y()*Ly + jetDirection.z()*Lz;
    if (L_proj_jetDir < 0.) decaylength_significance *= -1.;
    
    return decaylength_significance;
  }

  void MultiSVTag::finalizeHistos() {
    /// implementation for Analysis::ITagTool::finalizeHistos
  }

}

