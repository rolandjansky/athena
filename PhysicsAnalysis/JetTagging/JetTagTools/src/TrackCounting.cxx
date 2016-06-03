/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/TrackCounting.h"
 
#include "GaudiKernel/IToolSvc.h"

//#include "JetEvent/Jet.h"
#include "JetTagEvent/TrackAssociation.h"
#include "JetTagInfo/TrackCountingInfo.h"
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/TrackGrade.h"
#include "JetTagInfo/TrackGradesDefinition.h"
#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/GradedTrack.h"
#include "JetTagTools/SVForIPTool.h"
#include "JetTagTools/ITrackGradeFactory.h"

#include "Navigation/NavigationToken.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include <cmath>
#include <sstream>
#include <algorithm>

namespace Analysis {

  typedef std::vector<double> FloatVec;
  typedef std::vector<double>::iterator FloatVecIter;
  
  TrackCounting::TrackCounting(const std::string& name, const std::string& n, const IInterface* p)
    : AthAlgTool(name,n,p),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_trackSelectorTool("Analysis::TrackSelector"),
      m_secVxFinderNameForV0Removal("InDetVKalVxInJetTool"),
      m_secVxFinderNameForIPSign("InDetVKalVxInJetTool"),
      m_SVForIPTool("Analysis::SVForIPTool"),
      m_trackGradeFactory("Analysis::BasicTrackGradeFactory"),
      m_unbiasIPEstimation(false){
    
    declareInterface<ITagTool>(this);
    // global configuration:
    declareProperty("SignWithSvx", m_SignWithSvx = false);
    declareProperty("SVForIPTool", m_SVForIPTool);

    // track categories:
    declareProperty("trackGradePartitions", m_trackGradePartitionsDefinition);
    m_trackGradePartitionsDefinition.push_back("Good");
    declareProperty("RejectBadTracks", m_RejectBadTracks = false);
    declareProperty("flipIPSign", m_flipIP = false);

    // tools:
    declareProperty("trackSelectorTool", m_trackSelectorTool);
    declareProperty("trackToVertexTool", m_trackToVertexTool);
    declareProperty("trackGradeFactory", m_trackGradeFactory);
    
    // information to persistify:
    declareProperty("writeInfo", m_writeInfo = true);

    declareProperty("truthMatchingName", m_truthMatchingName = "TruthInfo");
    declareProperty("purificationDeltaR", m_purificationDeltaR = 0.8);

    declareProperty("SecVxFinderNameForV0Removal", m_secVxFinderNameForV0Removal);
    declareProperty("SecVxFinderNameForIPSign", m_secVxFinderNameForIPSign);

    declareProperty("TrackToVertexIPEstimator",m_trackToVertexIPEstimator);
    declareProperty("unbiasIPEstimation",m_unbiasIPEstimation);
  }

  TrackCounting::~TrackCounting(){
    for (size_t i = 0; i < m_trackGradePartitions.size(); i++)
      delete m_trackGradePartitions.at(i);
  }

  
  StatusCode TrackCounting::initialize() {
    
    /** retrieving TrackToVertex: */
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexTool);
    }

    if(m_SVForIPTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_SVForIPTool);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_SVForIPTool);
    }

    if(m_trackToVertexIPEstimator.retrieve().isFailure()) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_trackToVertexIPEstimator);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
    }
    
    /** creation of TrackSelector: (private instance) */
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_trackSelectorTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackSelectorTool);
    }
    
    /** retrieving the track grade factory */
    if ( m_trackGradeFactory.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_trackGradeFactory);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackGradeFactory);
    }
    
    /** prepare the track partitions: */
    int nbPart = m_trackGradePartitionsDefinition.size();
    ATH_MSG_DEBUG("#BTAG# Defining " << nbPart <<" track partitions: ");
    for(int i=0;i<nbPart;i++) {
      TrackGradePartition* part(0);
      try {
        part = new TrackGradePartition(m_trackGradePartitionsDefinition[i], *m_trackGradeFactory);
      }
      catch(std::string error) {
        ATH_MSG_ERROR("#BTAG# Reported error " << error);
        ATH_MSG_ERROR("#BTAG# List of categories provided to TrackCounting by jO : ");

        for (int l=0;l<nbPart;l++) {
          ATH_MSG_ERROR("#BTAG# string " << m_trackGradePartitionsDefinition[l]);
        }
 
        ATH_MSG_ERROR("#BTAG# List of categories provided by the TrackGradeFactory " << m_trackGradeFactory);

        const TrackGradesDefinition &trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
        const std::vector<TrackGrade> &gradeList = trackFactoryGradesDefinition.getList();

        std::vector<TrackGrade>::const_iterator listIter=gradeList.begin();
        std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();

        for ( ; listIter !=listEnd ; ++listIter ) {
          ATH_MSG_ERROR("#BTAG# n. " << (*listIter).gradeNumber() << " string " << (*listIter).gradeString());
        }

        ATH_MSG_ERROR("#BTAG# Terminating now... ");
	return StatusCode::FAILURE;
      }      
      ATH_MSG_INFO("#BTAG# " << (*part));
      m_trackGradePartitions.push_back(part);
    }
    return StatusCode::SUCCESS;
  }


  StatusCode TrackCounting::finalize() {
    
    return StatusCode::SUCCESS;
  }


  void TrackCounting::tagJet(xAOD::Jet& jetToTag) {
    
    /** author to know which jet algorithm: */
    std::string author = jetToTag.jetAuthor();
    ATH_MSG_VERBOSE("#BTAG# Using jet type " << author);

    m_tracksInJet.clear();
    int nbPart = m_trackGradePartitionsDefinition.size();

    std::vector<const Trk::Track*> TrkFromV0;
    Amg::Vector3D SvxDirection;
    bool canUseSvxDirection=false;

    /** getting sign from SecVxFinderTool */
    if (m_SignWithSvx) {
      m_SVForIPTool->getDirectionFromSecondaryVertexInfo(SvxDirection,canUseSvxDirection,//output
                                                         jetToTag,m_secVxFinderNameForIPSign,m_priVtx->recVertex());//input
    }
    
    /** getting Bad Tracks from SecVxFinderTool */
    if (m_RejectBadTracks) {
      m_SVForIPTool->getTrkFromV0FromSecondaryVertexInfo(TrkFromV0,//output
                                                         jetToTag,m_secVxFinderNameForV0Removal);//input
    }
     
    ATH_MSG_VERBOSE("#BTAG# TrkFromV0 : number of reconstructed bad tracks: " << TrkFromV0.size());
 
    /** extract the TrackParticles from the jet and apply track selection: */
    int nbTrack = 0;
    m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
    m_trackSelectorTool->prepare();
    std::vector<const xAOD::TrackParticle*>* trackVector = NULL;
    const Analysis::TrackAssociation *trkInJet = jetToTag.getAssociation<TrackAssociation>("Tracks");
    if(!trkInJet){
      ATH_MSG_WARNING("#BTAG#  Could not find tracks in jets");
    }
    else{
      trackVector = jetToTag.getAssociation<TrackAssociation>("Tracks")->tracks();
      std::vector<const xAOD::TrackParticle*>::iterator jetItr;
      for( jetItr = trackVector->begin(); jetItr != trackVector->end() ; ++jetItr ) {
	const xAOD::TrackParticle * aTemp = *jetItr;
	nbTrack++;
	if( m_trackSelectorTool->selectTrack(aTemp) ) {
	  
	  TrackGrade * theGrade = m_trackGradeFactory->getGrade(*aTemp,
								jetToTag.p4() );
	  
	  ATH_MSG_VERBOSE("#BTAG# result of selectTrack is OK, grade= " << (std::string)(*theGrade));
	  
	  bool tobeUsed = false;
	  for(int i=0;i<nbPart;i++) {
	    if (std::find( (m_trackGradePartitions[i]->grades()).begin(), 
			   (m_trackGradePartitions[i]->grades()).end(), 
			   *theGrade ) 
		!= (m_trackGradePartitions[i]->grades()).end()) tobeUsed = true;
	  }
	  // is it a bad track ?
	  if (std::find(TrkFromV0.begin(),TrkFromV0.end(),*aTemp->track()) != TrkFromV0.end()) {
	    ATH_MSG_VERBOSE("#BTAG# Bad track in jet, pt = " << aTemp->pt() << " eta = " << aTemp->eta() << " phi = " << aTemp->phi());
	    if (m_RejectBadTracks) tobeUsed = false;
	  }
	  if (tobeUsed) m_tracksInJet.push_back(GradedTrack(aTemp, *theGrade));
	  delete theGrade;
	  theGrade=0;
	}
      } // end loop on trakparticles

      delete trackVector; 
    }

    ATH_MSG_VERBOSE("#BTAG# #tracks = " << nbTrack);
    ATH_MSG_VERBOSE("#BTAG# the z of the primary Vertex= " << m_priVtx->recVertex().position().z());

    /** jet direction: */
    Amg::Vector3D jetDirection(jetToTag.px(),jetToTag.py(),jetToTag.pz());
    Amg::Vector3D unit = jetDirection.unit();
    if (m_SignWithSvx && canUseSvxDirection) {
      unit = SvxDirection.unit();
      ATH_MSG_DEBUG("#BTAG# Using direction from sec vertex finder tool '"
		    << m_secVxFinderNameForIPSign << "': "
		    << " phi: " << unit.phi() 
		    << " theta: " << unit.theta() 
		    << " instead of jet direction phi: " << jetDirection.phi() 
		    << " theta: " << jetDirection.theta());
    }

    FloatVec vectD0Signi;
    FloatVec vectD0Signi_abs;
    // reserve approximate space (optimization):
    const int nbTrackMean = 3;
    vectD0Signi.reserve(nbTrackMean);
    vectD0Signi_abs.reserve(nbTrackMean);

    for (std::vector<GradedTrack>::iterator trkItr = m_tracksInJet.begin(); 
         trkItr != m_tracksInJet.end(); ++trkItr) {
      const xAOD::TrackParticle* trk = (*trkItr).track;

      double d0wrtPriVtx(0.);
      double d0ErrwrtPriVtx(1.);
      double signOfIP(1.);

      /* use new Tool for "unbiased" IP estimation */
      const Trk::ImpactParametersAndSigma* myIPandSigma = m_trackToVertexIPEstimator->estimate(*trk->track(),m_priVtx,m_unbiasIPEstimation);
      if(myIPandSigma==0) {
	ATH_MSG_WARNING("#BTAG# trackToVertexIPEstimator failed !");
      } else {
	d0wrtPriVtx=myIPandSigma->IPd0;
	d0ErrwrtPriVtx=myIPandSigma->sigmad0;
	delete myIPandSigma;
	myIPandSigma=0;
      }
      
     /** sign of the impact parameter */ 
      signOfIP=m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack(trk->perigeeParameters(),unit,m_priVtx->recVertex());

      if (m_flipIP) signOfIP = -signOfIP; // A.X.

      /** signed ip and significances */
      double sd0             = signOfIP*fabs(d0wrtPriVtx);
      double sd0significance = signOfIP*fabs(d0wrtPriVtx/d0ErrwrtPriVtx);

      vectD0Signi.push_back(sd0significance);
      vectD0Signi_abs.push_back( fabs(sd0significance) );
      
      ATH_MSG_VERBOSE("#BTAG# TrackCounting: Trk: grade= " << (std::string)(*trkItr).grade
		      << " Eta= " << trk->eta() << " Phi= " << trk->phi() << " pT= " << trk->pt()
		      << " d0= " << sd0
		      << "+-" << d0ErrwrtPriVtx
		      << " d0sig= " << sd0significance );

    } // end loop on gradedtracks

    
    /** sort vector of significances in descending order */
    sort( vectD0Signi.begin(), vectD0Signi.end(), m_greater<float> );
    sort( vectD0Signi_abs.begin(), vectD0Signi_abs.end(), m_greater<float> );

 
    /** give information to the info class. */
    std::string instanceName("TrackCounting2D");
    if (m_flipIP) instanceName += "Neg";// A.X.
    TrackCountingInfo* infoTrackCounting = NULL;
  
    if(m_writeInfo) {
      infoTrackCounting = new TrackCountingInfo(instanceName);
    
      int ntrk = vectD0Signi.size();
      infoTrackCounting->setnTracks( ntrk );

      ATH_MSG_VERBOSE("#BTAG# Filling TrackCountingInfo...");
      ATH_MSG_DEBUG("#BTAG# " << ntrk << " tracks used for TrackCounting, ordered ip2d significance / |ip2d significance|:" );
      for(uint i=0; i<vectD0Signi.size(); i++){
	ATH_MSG_DEBUG("#BTAG# trk " << i+1 << "/" << ntrk << ": " << vectD0Signi.at(i) << " / " << vectD0Signi_abs.at(i));
      }
      if( ntrk>=2 ){
	ATH_MSG_DEBUG("#BTAG# filling 2nd / 2nd abs: " << vectD0Signi.at(1) << " / " << vectD0Signi_abs.at(1));
 	infoTrackCounting->setd0sig_2nd( vectD0Signi.at(1) );
 	infoTrackCounting->setd0sig_abs_2nd( vectD0Signi_abs.at(1) );
      }
      if( ntrk>=3 ){
	ATH_MSG_DEBUG("#BTAG# filling 3rd / 3rd abs: " << vectD0Signi.at(2) << " / " << vectD0Signi_abs.at(2));
 	infoTrackCounting->setd0sig_3rd( vectD0Signi.at(2) );
 	infoTrackCounting->setd0sig_abs_3rd( vectD0Signi_abs.at(2) );
      }
    }

    /** tagging done. Fill the JetTag and return ...
	fill most powerfull discriminating variable
	it's not a likelihood, but this can directly be accessed from the Jet 
	without changing the getFlavourTagWeight() method */
    if(infoTrackCounting){
      std::vector<double> v_tofill;
      double tofill = exp(-20.);
      if(vectD0Signi.size()>=2){
	tofill = exp(vectD0Signi.at(1));
	infoTrackCounting->makeValid();
      }
      v_tofill.push_back( tofill );
      v_tofill.push_back(1.);
      infoTrackCounting->setTagLikelihood( v_tofill );
      jetToTag.addInfo( infoTrackCounting );
    }
    
    m_tracksInJet.clear();
    
    return;
  }
 
}//end namespace



