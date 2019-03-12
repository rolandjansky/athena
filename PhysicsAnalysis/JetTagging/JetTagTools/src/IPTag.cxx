/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/IPTag.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SvxSummary.h"
#include "JetTagTools/NewLikelihoodTool.h"
#include "JetTagTools/LikelihoodComponents.h"
#include "JetTagTools/HistoHelperRoot.h"
#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/GradedTrack.h"
#include "JetTagTools/SVForIPTool.h"
#include "JetTagInfo/TrackGrade.h"
#include "JetTagInfo/TrackGradesDefinition.h"
#include "JetTagTools/ITrackGradeFactory.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "Navigation/NavigationToken.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "JetTagTools/JetTagUtils.h"
#include "ParticleJetTools/JetFlavourInfo.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "TH1.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

#include "TLorentzVector.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool xaodTrackPtSorting(const xAOD::TrackParticle *track1, const xAOD::TrackParticle *track2) { 
//  return track1->pt()>track2->pt(); 
//}

namespace Analysis {

  struct myIPxDinfo {
    ElementLink<xAOD::TrackParticleContainer> trk;
    double d0;
    double d0sig;
    double z0;
    double z0sig;
    TrackGrade grade;
    const xAOD::TrackParticle* trkP;
    bool fromV0;
    float wB;
    float wC;
    float wU;
  } ;
  
  bool StructPTsorting( myIPxDinfo objA, myIPxDinfo objB) {
    return (objA.trkP)->pt() > (objB.trkP)->pt();
  }
  
  bool StructD0Sigsorting( myIPxDinfo objA, myIPxDinfo objB) {
    return fabs(objA.d0sig) > fabs(objB.d0sig);
  }

  bool StructZ0D0Sigsorting( myIPxDinfo objA, myIPxDinfo objB) {
    return pow(objA.d0sig,2)+pow(objA.z0sig,2) > pow(objB.d0sig,2)+pow(objB.z0sig,2);
  }
  

  /** 
      @class IPTag 
      b-jet tagger based on 2D or 3D impact parameter 
      @author CPPM Marseille
  */


  IPTag::IPTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_runModus("analysis"),
      m_histoHelper(0),
      m_sortPt(false),
      m_sortD0sig(false),
      m_sortZ0D0sig(false),
      m_unbiasIPEstimation(true),
      m_secVxFinderName("InDetVKalVxInJetTool"),
      m_trackSelectorTool("Analysis::TrackSelector", this),
      m_likelihoodTool("Analysis::NewLikelihoodTool", this),
      m_SVForIPTool("Analysis::SVForIPTool", this),
      m_trackGradeFactory("Analysis::BasicTrackGradeFactory", this),
      m_trackToVertexIPEstimator(this),
      m_InDetTrackSelectorTool("InDet::InDetTrackSelectionTool", this),
      m_TightTrackVertexAssociationTool("CP::TightTrackVertexAssociationTool", this)
  {
    
    declareInterface<ITagTool>(this);
    
    // global configuration:
    declareProperty("Runmodus"      , m_runModus);
    declareProperty("xAODBaseName"  , m_xAODBaseName);
    
    declareProperty("flipIPSign"          , m_flipIP              = false);
    declareProperty("flipZIPSign"         , m_flipZIP             = false);
    declareProperty("usePosIP"            , m_usePosIP            = true);
    declareProperty("useNegIP"            , m_useNegIP            = true);
    declareProperty("useZIPSignForPosNeg" , m_useZIPSignForPosNeg = false);
    declareProperty("use2DSignForIP3D"    , m_use2DSignForIP3D    = false);
    declareProperty("useD0SignForZ0"      , m_useD0SignForZ0      = false);
    declareProperty("RejectBadTracks"     , m_RejectBadTracks     = true);
    declareProperty("SignWithSvx"         , m_SignWithSvx         = false);
    declareProperty("checkOverflows"      , m_checkOverflows      = false);
    declareProperty("useForcedCalibration", m_doForcedCalib       = false);
    declareProperty("UseCHypo"            , m_useCHypo            = true);
    declareProperty("unbiasIPEstimation"  , m_unbiasIPEstimation);

    declareProperty("trackAssociationName"    , m_trackAssociationName = "BTagTrackToJetAssociator");
    declareProperty("originalTPCollectionName", m_originalTPCollectionName = "InDetTrackParticles");
    declareProperty("jetCollectionList"       , m_jetCollectionList);
    declareProperty("useVariables"            , m_useVariables);
    declareProperty("impactParameterView"     , m_impactParameterView = "2D");
    declareProperty("ForcedCalibrationName"   , m_ForcedCalibName = "Cone4H1Tower");
    
    declareProperty("trackGradePartitions"    , m_trackGradePartitionsDefinition);
    m_trackGradePartitionsDefinition.push_back("Good");

    declareProperty("referenceType"           , m_referenceType = "ALL"); // B, UDSG, ALL
    declareProperty("truthMatchingName"       , m_truthMatchingName = "TruthInfo");
    declareProperty("purificationDeltaR"      , m_purificationDeltaR = 0.8);
    declareProperty("jetPtMinRef"             , m_jetPtMinRef = 15.*Gaudi::Units::GeV);

    declareProperty("SecVxFinderName"         ,m_secVxFinderName);

    declareProperty("NtrkMin"                 , m_NtrkMin =6 );
    declareProperty("NtrkMax"                 , m_NtrkMax =6 );
    declareProperty("NtrkFract"               , m_trkFract=1.0 );
    declareProperty("SortingMode"             , m_sortOption="None"); //"None");
#define PROP(name, def) declareProperty( #name, m_ ## name = def)
    PROP(storeTrackParticles, true);
    PROP(storeIpValues, false);
    PROP(storeTrackParameters, true);
#undef PROP

    // tools:
    declareProperty("trackSelectorTool"         , m_trackSelectorTool              );
    declareProperty("LikelihoodTool"            , m_likelihoodTool                 );
    declareProperty("SVForIPTool"               , m_SVForIPTool                    );  
    declareProperty("trackGradeFactory"         , m_trackGradeFactory              );
    declareProperty("TrackToVertexIPEstimator"  , m_trackToVertexIPEstimator       );
    declareProperty("InDetTrackSelectionTool"   , m_InDetTrackSelectorTool         ); //
    declareProperty("TrackVertexAssociationTool", m_TightTrackVertexAssociationTool); //
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  IPTag::~IPTag() {
    delete m_histoHelper;
    for (size_t i = 0; i < m_trackGradePartitions.size(); i++)
      delete m_trackGradePartitions[i];
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  StatusCode IPTag::initialize() {
    
    // VD: deciding the track sorting mode
    if ( m_sortOption=="None" ) {
      ATH_MSG_DEBUG("#BTAG# No sorting applied ");
      m_sortPt     =false;
      m_sortD0sig  =false;
      m_sortZ0D0sig=false;
    } else if ( m_sortOption=="SortPt" ) {
      ATH_MSG_DEBUG("#BTAG# Tracks will be sorted by Pt ");
      m_sortPt     =true;
      m_sortD0sig  =false;
      m_sortZ0D0sig=false;
    } else if ( m_sortOption=="SortD0" ) {
      ATH_MSG_DEBUG("#BTAG# Tracks will be sorted by |d0Sig| ");
      m_sortPt     =false;
      m_sortD0sig  =true;
      m_sortZ0D0sig=false;
    } else if ( m_sortOption=="SortZ0D0" ) {
      ATH_MSG_DEBUG("#BTAG# Tracks will be sorted by sqrt( |d0Sig|^2 + |z0Sig|^2 ) ");
      m_sortPt     =false;
      m_sortD0sig  =false;
      m_sortZ0D0sig=true;
    } else {
      ATH_MSG_ERROR("#BTAG# sorting option NOT recognised (please use: <None>, <SortPt>, <SortD0> ");
      return StatusCode::FAILURE;
    }

    if ( m_sortOption=="None" ) {
      m_NtrkMax =1000;
      m_NtrkMin =m_NtrkMax;
      m_trkFract=1.0;
    } else {
      if (m_trkFract!=1.0) {
	ATH_MSG_DEBUG("#BTAG# Considering only first "<< m_trkFract << " % of the tracks in the jet but keeping at least " << m_NtrkMin);     
      } else {
	ATH_MSG_DEBUG("#BTAG# Considering only maximum "<< m_NtrkMax << " per jet");     
      }
    }


    m_hypotheses.push_back("B");
    m_hypotheses.push_back("U");
    if(m_useCHypo){
      m_hypotheses.push_back("C");
    }

    // FF: comment out V0 finding
    if (m_SVForIPTool.retrieve().isFailure() )  {
       ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_SVForIPTool);
       return StatusCode::FAILURE;
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_SVForIPTool);
    }
    if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexIPEstimator);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
    }
    

    /** creation of TrackSelector: (private instance) */
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackSelectorTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackSelectorTool);
    }

    /** retrieving histoHelper: */
    ITHistSvc* myHistoSvc;
    if( service( "THistSvc", myHistoSvc ).isSuccess() ) {
      ATH_MSG_DEBUG("#BTAG# HistoSvc loaded successfully.");
      m_histoHelper = new HistoHelperRoot(myHistoSvc);
      m_histoHelper->setCheckOverflows(m_checkOverflows);
    } else {
      ATH_MSG_ERROR("#BTAG# HistoSvc could NOT bo loaded.");
    }

    /** retrieving the track grade factory */
    if ( m_trackGradeFactory.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackGradeFactory);
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
        part = new TrackGradePartition(m_trackGradePartitionsDefinition[i],
                                       *m_trackGradeFactory);
      }
      catch (std::string error) {
        ATH_MSG_ERROR("#BTAG# Reported error " << error);
        ATH_MSG_ERROR("#BTAG# List of categories provided to IPTag by jO : ");
        for (int l=0;l<nbPart;l++) {
          ATH_MSG_ERROR("#BTAG#  string " << m_trackGradePartitionsDefinition[l]);
        }
        ATH_MSG_ERROR("#BTAG# List of categories provided by the TrackGradeFactory " << m_trackGradeFactory);

        const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
        const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();
        std::vector<TrackGrade>::const_iterator listIter=gradeList.begin();
        std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();
        if (msgLvl(MSG::ERROR)) {
          for ( ; listIter !=listEnd ; ++listIter ) {
            ATH_MSG_ERROR("#BTAG# n. " << (*listIter).gradeNumber() << " string " << (*listIter).gradeString());
          }
        }
        ATH_MSG_ERROR("#BTAG# Terminating now... ");
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG((*part));
      m_trackGradePartitions.push_back(part);
    }

    /** configure likelihood: */
    if( m_runModus == "analysis" ) {
      if ( m_likelihoodTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_likelihoodTool);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_likelihoodTool);
      }
      m_likelihoodTool->defineHypotheses(m_hypotheses);
      m_likelihoodTool->printStatus();
    }

    const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
    const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();
    std::vector<TrackGrade>::const_iterator listBegin=gradeList.begin();
    std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();

    /** book calibration histograms if needed */
    if( m_runModus == "reference" ) {
      ATH_MSG_DEBUG("#BTAG# running IPTag in reference mode");
     for(uint j=0;j<m_jetCollectionList.size();j++) {

        //int nbGrades=trackFactoryGradesDefinition.numberOfGrades();

        for (std::vector<TrackGrade>::const_iterator listIter=listBegin ; listIter !=listEnd ; ++listIter ) {
          const TrackGrade & grd = (*listIter);
	  if(m_impactParameterView=="1D") {
	    for(uint ih=0;ih<m_hypotheses.size();ih++) {
	      std::string hName = "/RefFile/IP1D/" + m_jetCollectionList[j] + "/"
		+ m_hypotheses[ih] + "/" + grd.gradeString() + "/SipZ0";
	      ATH_MSG_VERBOSE("#BTAG# booking for IP1D: " << hName);
	      m_histoHelper->bookHisto(hName,"Signed Impact Parameter (z)",400,-40.,60.);
	    }
	  }
	  if(m_impactParameterView=="2D") {
	    for(uint ih=0;ih<m_hypotheses.size();ih++) {
	      std::string hName = "/RefFile/IP2D/" + m_jetCollectionList[j] + "/"
		+ m_hypotheses[ih] + "/" + grd.gradeString() + "/SipA0";
	      ATH_MSG_VERBOSE("#BTAG# booking for IP2D: " << hName);
	      m_histoHelper->bookHisto(hName,"Signed Impact Parameter (rphi)",400,-40.,60.);
	    }
	  }
	  if(m_impactParameterView=="3D") {
	    const int nbx  = 36;
	    const int nby  = 21;
	    double xbi[nbx] = {-40.0,-20.0,-15.0,-10.0,-8.0,-6.0,-5.5,-5.0,-4.5,-4.0,-3.5,-3.0,-2.5,-2.0,-1.5,-1.0,-0.5,
			       0.0,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,5.5,6.0,8.0,10.0,15.0,20.0,40.0,60.0};
	    double ybi[nby] = {-40.,-20.,-12.,-8.,-4.,-3.,-2.,-1.,-0.5,0.,0.5,1.,2.,3.,4.,6.,8.,12.,20.,40.,60.};
	    for(uint ih=0;ih<m_hypotheses.size();ih++) {
	      std::string hName = "/RefFile/IP3D/" + m_jetCollectionList[j] + "/"
                + m_hypotheses[ih] + "/" + grd.gradeString() + "/Sip3D";
	      ATH_MSG_VERBOSE("#BTAG# booking for IP3D: " << hName);
	      m_histoHelper->bookHisto(hName,"Signed IP Z0 vs (rphi)",nbx-1,xbi,nby-1,ybi);
	    }
	  }
	} // endloop on partitions

	}
      // ms m_histoHelper->print();
    }

    if (m_impactParameterView=="3D") { 
      if ( m_InDetTrackSelectorTool.retrieve().isFailure() )  {
	ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " <<  m_InDetTrackSelectorTool);
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG("#BTAG# Retrieved tool " <<  m_InDetTrackSelectorTool);
      }
      if ( m_TightTrackVertexAssociationTool.retrieve().isFailure() )  {
	ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " <<  m_TightTrackVertexAssociationTool);
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG("#BTAG# Retrieved tool " <<  m_TightTrackVertexAssociationTool);
      }
    }
    else {
      m_InDetTrackSelectorTool.disable();
      m_TightTrackVertexAssociationTool.disable();
    }

    return StatusCode::SUCCESS;
  }


  StatusCode IPTag::finalize() {
    if( m_runModus == "reference" ) {
      ATH_MSG_INFO("#BTAG# Number of jets used for calibration for reference "
		   << m_referenceType << " : #b= " << m_nbjet << " #light= " << m_nljet  << " #charm= " << m_ncjet );
    }
    return StatusCode::SUCCESS;
  }


  StatusCode IPTag::tagJet(const xAOD::Jet * jetToTag, xAOD::BTagging* BTag) {

    ATH_MSG_VERBOSE("#BTAG# m_impactParameterView = " << m_impactParameterView );
    /** author to know which jet algorithm: */
    std::string author = JetTagUtils::getJetAuthor(jetToTag);
    if (m_doForcedCalib) author = m_ForcedCalibName;
    ATH_MSG_VERBOSE("#BTAG# Using jet type " << author << " for calibrations.");

    /** for the reference mode we need the true label: */
    int label = -1;
    //std::string label = "N/A";
    std::string pref  = "";
    // FF: Disable reference mode running for now
    if( m_runModus == "reference" ) {
      // here we require a jet selection:
      if( jetToTag->pt()>m_jetPtMinRef && fabs(jetToTag->eta())<2.5 ) {
	label = xAOD::jetFlavourLabel(jetToTag);
	double deltaRtoClosestB = 999., deltaRtoClosestC = 999., deltaRtoClosestT = 999.;
    	double deltaRmin(0.);
        if (jetToTag->getAttribute("TruthLabelDeltaR_B",deltaRtoClosestB)) {
    	  // for purification: require no b or c quark closer than dR=m_purificationDeltaR
	  jetToTag->getAttribute("TruthLabelDeltaR_C",deltaRtoClosestC);
	  jetToTag->getAttribute("TruthLabelDeltaR_T",deltaRtoClosestT);
    	  deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
          deltaRmin = deltaRtoClosestT < deltaRmin ? deltaRtoClosestT : deltaRmin;
        } else {
          ATH_MSG_ERROR("#BTAG# No TruthInfo ! Cannot run in reference mode !");
          return StatusCode::FAILURE;
        }
    	if ( (    "B"==m_referenceType &&   5==label ) ||  // b-jets    
    	     ( "UDSG"==m_referenceType &&   0==label ) ||  // light jets
    	     (  "ALL"==m_referenceType && // all jets: b + purified light jets
    		( 5==label || 4==label || ( 0==label && deltaRmin > m_purificationDeltaR ) ) )
    	     ) {
          if (5==label) {
            pref = m_hypotheses[0];
            m_nbjet++;
          } else if (0==label) {
            pref = m_hypotheses[1];
            m_nljet++;
          } else if (4==label && m_useCHypo) {
            pref = m_hypotheses[2];
            m_ncjet++;
    	  }
    	}
      }
    }
    
    m_tracksInJet.clear();
    int nbPart = m_trackGradePartitionsDefinition.size();

    std::vector<const xAOD::TrackParticle*> TrkFromV0;
    Amg::Vector3D SvxDirection;
    bool canUseSvxDirection=false;
    
    if (m_SignWithSvx) {
      m_SVForIPTool->getDirectionFromSecondaryVertexInfo(SvxDirection,canUseSvxDirection,//output
                                                         BTag,m_secVxFinderName,*m_priVtx);//input
    }
    
    // bad tracks from V0s, conversions, interactions:
    m_SVForIPTool->getTrkFromV0FromSecondaryVertexInfo(TrkFromV0,//output
                                                       BTag,m_secVxFinderName);//input
    if (TrkFromV0.size()!=0)  ATH_MSG_DEBUG("#BTAG# TrkFromV0 : number of reconstructed bad tracks: " << TrkFromV0.size());

    /** extract the TrackParticles from the jet and apply track selection: */
    int nbTrak = 0;
    m_trackSelectorTool->primaryVertex(m_priVtx->position());
    m_trackSelectorTool->prepare();
    std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks = 
      BTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_trackAssociationName);
    double sumTrkpT = 0; unsigned ntrk=0;
    TLorentzVector pseudoTrackJet(0,0,0,0);
    if( associationLinks.size() == 0 ) {
        ATH_MSG_VERBOSE("#BTAG#  Could not find tracks associated with BTagging object as " << m_trackAssociationName);
    } else {
      
      std::vector< ElementLink< xAOD::TrackParticleContainer> >::const_iterator trkIter;
      // We loop over the tracks twice: the first time is to compute the summed pt of all tracks satisfying
      // the "normal" criteria; the second time a possibly tighter pt cut may be applied
      for( trkIter = associationLinks.begin(); trkIter != associationLinks.end() ; ++trkIter ) {
	const xAOD::TrackParticle* aTemp = **trkIter;
	
	if (m_impactParameterView=="3D") { 
	  if (m_InDetTrackSelectorTool->accept(*aTemp, m_priVtx) ) {
	    if (m_TightTrackVertexAssociationTool->isCompatible(*aTemp, *m_priVtx)) {
	      TLorentzVector tmpTrack(0,0,0,0);
	      tmpTrack.SetPtEtaPhiM(  aTemp->pt(), aTemp->eta(), aTemp->phi(),0);
	      pseudoTrackJet+=tmpTrack; ntrk++;
	    }
	  }
	}

	if (m_trackSelectorTool->selectTrack(aTemp)) sumTrkpT += aTemp->pt();	
      }
      
      for( trkIter = associationLinks.begin(); trkIter != associationLinks.end() ; ++trkIter ) {
        const xAOD::TrackParticle* aTemp = **trkIter;
        nbTrak++;
        if( m_trackSelectorTool->selectTrack(aTemp, sumTrkpT) ) {
          TrackGrade* theGrade = m_trackGradeFactory->getGrade(*aTemp, jetToTag->p4() );
          ATH_MSG_VERBOSE("#BTAG#  result of selectTrack is OK, grade= " << theGrade->gradeString() );
	  bool tobeUsed = false;
          for(int i=0;i<nbPart;i++) {
            if( std::find( (m_trackGradePartitions[i]->grades()).begin(), 
			   (m_trackGradePartitions[i]->grades()).end(), 
			   *theGrade ) 
		!= (m_trackGradePartitions[i]->grades()).end() ) tobeUsed = true;
          }
	  // is it a bad track ?
          if( std::find(TrkFromV0.begin(),TrkFromV0.end(),aTemp) != TrkFromV0.end() ) {
            ATH_MSG_VERBOSE("#BTAG# Bad track in jet, pt = " << aTemp->pt() << " eta = " 
	 		    << aTemp->eta() << " phi = " << aTemp->phi() ); 
            if (m_RejectBadTracks) tobeUsed = false;
          }
          // check required IP sign:
          if (tobeUsed) m_tracksInJet.push_back(GradedTrack(*trkIter, *theGrade));
          delete theGrade;
          theGrade=0;
        }
      }
    }
    // temporary: storing these variables (which aren't IP3D-specific) ought to be moved to a separate Tool
    if (m_xAODBaseName == "IP3D") { 
      BTag->auxdata<unsigned>("trkSum_ntrk") = ntrk;
      BTag->auxdata<float>("trkSum_SPt") = sumTrkpT;
      BTag->auxdata<float>("trkSum_VPt") = pseudoTrackJet.Pt();
      BTag->auxdata<float>("trkSum_VEta") = (pseudoTrackJet.Pt() != 0) ? pseudoTrackJet.Eta() : 1000;
    }


    ATH_MSG_VERBOSE("#BTAG# #tracks = " << nbTrak);
    ATH_MSG_VERBOSE("#BTAG# the z of the primary = " << m_priVtx->position().z());

    /** jet direction: */
    Amg::Vector3D jetDirection(jetToTag->px(),jetToTag->py(),jetToTag->pz());
    Amg::Vector3D unit = jetDirection.unit();
    if (m_SignWithSvx && canUseSvxDirection) {
      unit = SvxDirection.unit();
      ATH_MSG_DEBUG("#BTAG# Using direction from sec vertex finder: " << 
		    " phi: " << unit.phi() << " theta: " << unit.theta() << 
		    " instead of jet direction phi: " << jetDirection.phi() << 
		    " theta: " << jetDirection.theta() );
    }

    /** prepare vectors with all track information: TP links, i.p. significances, track grade, etc */
    std::vector<ElementLink<xAOD::TrackParticleContainer> > IPTracks;
    std::vector<double> vectD0;
    std::vector<double> vectD0Signi;
    std::vector<double> vectZ0;
    std::vector<double> vectZ0Signi;
    std::vector<TrackGrade> vectGrades;
    ///////////std::vector<const xAOD::TrackParticle*> vectTP;
    std::vector<bool> vectFromV0;
    std::vector<float> vectWeightB;
    std::vector<float> vectWeightU;
    std::vector<float> vectWeightC;
    std::vector<myIPxDinfo> vectObj;

    // reserve approximate space (optimization):
    const int nbTrackMean = 5;
    vectD0.reserve(nbTrackMean);
    vectD0Signi.reserve(nbTrackMean);
    vectZ0.reserve(nbTrackMean);
    vectZ0Signi.reserve(nbTrackMean);
    vectGrades.reserve(nbTrackMean);
    vectFromV0.reserve(nbTrackMean);
    vectWeightB.reserve(nbTrackMean);
    vectWeightU.reserve(nbTrackMean);
    vectWeightC.reserve(nbTrackMean);
    vectObj.reserve(nbTrackMean);

    for (std::vector<GradedTrack>::iterator trkItr = m_tracksInJet.begin(); 
         trkItr != m_tracksInJet.end(); ++trkItr) {

      const xAOD::TrackParticle* trk = *(trkItr->track);
      bool isFromV0 = (std::find(TrkFromV0.begin(),TrkFromV0.end(),trk) != TrkFromV0.end());

      /** track parameters defined at the primary vertex: */
      double d0wrtPriVtx(0.);
      double z0wrtPriVtx(0.);
      double d0ErrwrtPriVtx(1.);
      double z0ErrwrtPriVtx(1.);

      /** use new Tool for "unbiased" IP estimation */
      const Trk::ImpactParametersAndSigma* myIPandSigma(0);
      if (m_trackToVertexIPEstimator) { 
        myIPandSigma = m_trackToVertexIPEstimator->estimate(trk, m_priVtx, m_unbiasIPEstimation);
      }
      if(0==myIPandSigma) {
        ATH_MSG_WARNING("#BTAG# IPTAG: trackToVertexIPEstimator failed !");
      } else {
        d0wrtPriVtx=myIPandSigma->IPd0;
        d0ErrwrtPriVtx=myIPandSigma->sigmad0;
        z0wrtPriVtx=myIPandSigma->IPz0SinTheta;
        z0ErrwrtPriVtx=myIPandSigma->sigmaz0SinTheta;
        delete myIPandSigma;
        myIPandSigma=0;
      }

      /** sign of the impact parameter */
      double signOfIP(1.);
      if (m_impactParameterView=="2D" || m_impactParameterView=="1D" ||
         ( m_impactParameterView=="3D" && m_use2DSignForIP3D)  ) {
        signOfIP=m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack(trk->perigeeParameters(),
                                                                      unit, *m_priVtx);
      }
      if (m_impactParameterView=="3D" && !m_use2DSignForIP3D) {
        signOfIP=m_trackToVertexIPEstimator->get3DLifetimeSignOfTrack(trk->perigeeParameters(),
                                                                      unit, *m_priVtx);
      }
      double signOfZIP = m_trackToVertexIPEstimator->getZLifetimeSignOfTrack(trk->perigeeParameters(),
                                                                             unit, *m_priVtx);

      if (m_useD0SignForZ0) signOfZIP = signOfIP;

      /** option to flip signs for negative tag method */
      if(m_flipIP  && m_runModus!="reference")  signOfIP = -signOfIP;
      if(m_flipZIP && m_runModus!="reference") signOfZIP = -signOfZIP;

      /** select according to sign: skip tracks if not ok */
      if( m_runModus != "reference" ) {
        if(signOfIP>0 && !m_usePosIP) continue;
        if(signOfIP<0 && !m_useNegIP) continue;
        if( m_useZIPSignForPosNeg ) {
          if(m_impactParameterView=="3D" && signOfZIP>0 && !m_usePosIP) continue;
          if(m_impactParameterView=="3D" && signOfZIP<0 && !m_useNegIP) continue;
        }
      }

      /** significances */
      double sIP         = signOfIP*fabs(d0wrtPriVtx);
      double significance= signOfIP*fabs(d0wrtPriVtx/d0ErrwrtPriVtx);
      double szIP        = signOfZIP*fabs(z0wrtPriVtx);
      double z0Sig       = signOfZIP*fabs(z0wrtPriVtx/z0ErrwrtPriVtx);

      msg(MSG::VERBOSE) << "#BTAG# IPTAG: Trk: grade= " << trkItr->grade.gradeString()
			<< " Eta= " << trk->eta() << " Phi= " << trk->phi() << " pT= " << trk->pt()
			<< " d0= " << sIP
			<< "+-" << d0ErrwrtPriVtx
			<< " sigd0= " << significance
			<< " z0= " << szIP
			<< "+-" << z0ErrwrtPriVtx
			<< " sigz0= " << z0Sig << endmsg;
      
      // VD: I know that this is ugly but I want to minimise the changes as much as I can
      myIPxDinfo tmpObj;
      tmpObj.trk=trkItr->track;
      tmpObj.d0   =sIP;
      tmpObj.d0sig=significance;
      tmpObj.z0   =szIP;
      tmpObj.z0sig=z0Sig;
      tmpObj.grade=trkItr->grade;
      tmpObj.trkP =trk;
      tmpObj.fromV0=isFromV0;
      // compute individual track contribution to likelihood:
      double tpb = 0., tpu = 0., tpc = 0.;
      this->trackWeight(author,trkItr->grade,significance,z0Sig,tpb,tpu,tpc);
      tmpObj.wB   =tpb;
      tmpObj.wU   =tpu;
      tmpObj.wC   =tpc;
      vectObj.push_back(tmpObj);
    }

    if (m_sortPt)      std::sort( vectObj.begin() , vectObj.end() , StructPTsorting );
    if (m_sortD0sig)   std::sort( vectObj.begin() , vectObj.end() , StructD0Sigsorting );
    if (m_sortZ0D0sig) std::sort( vectObj.begin() , vectObj.end() , StructZ0D0Sigsorting );
    
    int resizeVal=vectObj.size();
    if (m_NtrkMax!=1000) {
      if ( m_trkFract==1.0 ) {
	if ( m_NtrkMax<resizeVal ) resizeVal=m_NtrkMax;
      }	else {
	int resizeVal2=(int)( (float)resizeVal*m_trkFract);
	if (resizeVal<m_NtrkMin) resizeVal=m_NtrkMin;
	if ( resizeVal2<resizeVal ) resizeVal=resizeVal2;
      }
    }
    /////if ( resizeVal!=tmpSize ) std::cout << " Previous size: " << tmpSize << " ... reduced to: " << resizeVal << std::endl;
    //std::cout << "before resizing: " << vectObj.size() << std::endl;
    vectObj.resize(resizeVal);
    //std::cout << "after resizing: " << vectObj.size() << std::endl;
    
    for (unsigned int i=0; i<vectObj.size(); i++) {
      //if( m_impactParameterView=="3D" ) std::cout << i << " track pt: " << ((vectObj[i]).trkP)->pt() << "   d0sig: " << (vectObj[i]).d0sig << std::endl;
      /** fill reference histograms: */
      ///* ms
      if( m_runModus == "reference" ) {
	if( !pref.empty() ) { // current jet passes selection for Sig or Bkg
	  ATH_MSG_DEBUG("#BTAG# filling ref histo for " << pref);
          const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
          const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();
          std::vector<TrackGrade>::const_iterator listIter=gradeList.begin();
          std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();
          for ( ; listIter !=listEnd ; ++listIter ) {
            const TrackGrade & grd = (*listIter);
	    ATH_MSG_DEBUG("#BTAG#   filling ref histo for grade " << grd.gradeString() );
	    if( grd==(vectObj[i]).grade ) { // check the grade of current track
	      ATH_MSG_DEBUG("#BTAG#   track is of required grade ");
              const std::string suffix = "_" + grd.gradeString();
              if( m_impactParameterView=="1D" ) {
	        std::string hName = "/RefFile/IP1D/" + author + "/"
		  + pref + "/" + grd.gradeString() + "/SipZ0";
	        ATH_MSG_DEBUG("#BTAG#   histo 1D: " << hName);
		m_histoHelper->fillHisto(hName, (vectObj[i]).z0sig );
	      }
              if( m_impactParameterView=="2D" ) {
	        std::string hName = "/RefFile/IP2D/" + author + "/"
		  + pref + "/" + grd.gradeString() + "/SipA0";
	        ATH_MSG_DEBUG("#BTAG#   histo 2D: " << hName);
		m_histoHelper->fillHisto(hName, (vectObj[i]).d0sig );
	      }
              if( m_impactParameterView=="3D" ) {
	        std::string hName = "/RefFile/IP3D/" + author + "/"
		  + pref + "/" + grd.gradeString() + "/Sip3D";
	        ATH_MSG_DEBUG("#BTAG#   histo 3D: " << hName);
		m_histoHelper->fillHisto(hName,(vectObj[i]).d0sig, (vectObj[i]).z0sig);
	      }
	    }
	  }
	}
      }//*/ ////// END of reference mode ....

      IPTracks.push_back( (vectObj[i]).trk );
      vectD0.push_back( (vectObj[i]).d0 );
      vectD0Signi.push_back( (vectObj[i]).d0sig );
      vectZ0.push_back( (vectObj[i]).z0 );
      vectZ0Signi.push_back( (vectObj[i]).z0sig );
      vectGrades.push_back( (vectObj[i]).grade );
      //////vectTP.push_back( (vectObj[i]).trkP );
      vectFromV0.push_back( (vectObj[i]).fromV0 );
      vectWeightB.push_back( (vectObj[i]).wB );
      vectWeightU.push_back( (vectObj[i]).wU );
      vectWeightC.push_back( (vectObj[i]).wC );

    } // endloop on tracks

    /** fill information to xaod: */
    if(m_runModus=="analysis") {

      /** define and compute likelihood: */
      std::vector<Slice> slices;
      for(unsigned int i=0; i<vectD0Signi.size(); i++) {
        if(m_impactParameterView=="3D") {
          AtomicProperty atom1(vectD0Signi[i],"Significance of IP (rphi)");
          AtomicProperty atom2(vectZ0Signi[i],"Significance of Z0");
          std::string compoName(author+"#");
          compoName += vectGrades[i].gradeString();
          compoName += "/Sip3D";
          Composite compo1(compoName);
          compo1.atoms.push_back(atom1);
          compo1.atoms.push_back(atom2);
          Slice slice1("IP3D");
          slice1.composites.push_back(compo1);
          slices.push_back(slice1);
        }
        if(m_impactParameterView=="2D") {
          AtomicProperty atom1(vectD0Signi[i],"Significance of IP (rphi)");
          std::string compoName(author+"#");
          compoName += vectGrades[i].gradeString();
          compoName += "/SipA0";
          Composite compo1(compoName);
          compo1.atoms.push_back(atom1);
          Slice slice1("IP2D");
          slice1.composites.push_back(compo1);
          slices.push_back(slice1);
        }
        if(m_impactParameterView=="1D") {
          AtomicProperty atom1(vectZ0Signi[i],"Significance of IP (z)");
          std::string compoName(author+"#");
          compoName += vectGrades[i].gradeString();
          compoName += "/SipZ0";
          Composite compo1(compoName);
          compo1.atoms.push_back(atom1);
          Slice slice1("IP1D");
          slice1.composites.push_back(compo1);
          slices.push_back(slice1);
        }
      }
      m_likelihoodTool->setLhVariableValue(slices);
      std::vector<double> lkl;
      lkl.reserve(3);
      if(vectD0Signi.size()>0) {
        lkl = m_likelihoodTool->calculateLikelihood();
      } else {
        lkl.push_back(1.);
        lkl.push_back(1.e9);
        if(m_useCHypo) lkl.push_back(1.e9);
      }
      if(lkl.size()<1) lkl[0] = 0.;
      if(lkl.size()<2) lkl[1] = 0.;
      if(lkl.size()<3) lkl[2] = 0.;
	  ATH_MSG_DEBUG("#BTAG# likelihood : number of values = " << lkl.size() );
	  ATH_MSG_DEBUG("#BTAG# likelihood : " 
        << "pb= " << lkl[0] << " pu= " << lkl[1] << " pc= " << lkl[2] );

      /** fill likelihood information in xaod: */
      BTag->setVariable<double>(m_xAODBaseName, "pb", lkl[0]);
      BTag->setVariable<double>(m_xAODBaseName, "pu", lkl[1]);
      if (m_useCHypo) BTag->setVariable<double>(m_xAODBaseName, "pc", lkl[2]);

      /** fill trackParticle links and other track informations in xaod: */
      
      if (IPTracks.size() == 0) {
        ATH_MSG_VERBOSE("#BTAG# IPTracks is empty.");
      }
      else
      {
	ATH_MSG_VERBOSE("#BTAG# IPTracks is not empty: "<<IPTracks.size());
      }
      // NB: all vector<double> are converted as vector<float> for persistency.
      std::vector<float> f_vectD0( vectD0.begin(), vectD0.end() );
      std::vector<float> f_vectD0Signi( vectD0Signi.begin(), vectD0Signi.end() );
      std::vector<float> f_vectZ0( vectZ0.begin(), vectZ0.end() );
      std::vector<float> f_vectZ0Signi( vectZ0Signi.begin(), vectZ0Signi.end() );
      std::vector<int> i_vectGrades( vectGrades.begin(), vectGrades.end() );
      // specific fast accessors for mainstream instances of IPTag: IP3D, IP2D
      if( "IP3D"==m_xAODBaseName ) {
        if (m_storeTrackParticles) BTag->setIP3D_TrackParticleLinks(IPTracks);
        if (m_storeIpValues) {
          BTag->setTaggerInfo(f_vectD0,      xAOD::BTagInfo::IP3D_valD0wrtPVofTracks);
          BTag->setTaggerInfo(f_vectZ0,      xAOD::BTagInfo::IP3D_valZ0wrtPVofTracks);
        }
        if (m_storeTrackParameters) {
          BTag->setTaggerInfo(f_vectD0Signi, xAOD::BTagInfo::IP3D_sigD0wrtPVofTracks);
          BTag->setTaggerInfo(f_vectZ0Signi, xAOD::BTagInfo::IP3D_sigZ0wrtPVofTracks);
          BTag->setTaggerInfo(vectWeightB,   xAOD::BTagInfo::IP3D_weightBofTracks);
          BTag->setTaggerInfo(vectWeightU,   xAOD::BTagInfo::IP3D_weightUofTracks);
          BTag->setTaggerInfo(vectWeightC,   xAOD::BTagInfo::IP3D_weightCofTracks);
          BTag->setTaggerInfo(vectFromV0,    xAOD::BTagInfo::IP3D_flagFromV0ofTracks);
          BTag->setTaggerInfo(i_vectGrades,  xAOD::BTagInfo::IP3D_gradeOfTracks);
        }
      } else {
        if( "IP2D"==m_xAODBaseName ) {
          if (m_storeTrackParticles) BTag->setIP2D_TrackParticleLinks(IPTracks);
          if (m_storeIpValues) BTag->setTaggerInfo(f_vectD0,      xAOD::BTagInfo::IP2D_valD0wrtPVofTracks);
          if (m_storeTrackParameters) {
            BTag->setTaggerInfo(f_vectD0Signi, xAOD::BTagInfo::IP2D_sigD0wrtPVofTracks);
            BTag->setTaggerInfo(vectWeightB,   xAOD::BTagInfo::IP2D_weightBofTracks);
            BTag->setTaggerInfo(vectWeightU,   xAOD::BTagInfo::IP2D_weightUofTracks);
            BTag->setTaggerInfo(vectWeightC,   xAOD::BTagInfo::IP2D_weightCofTracks);
            BTag->setTaggerInfo(vectFromV0,    xAOD::BTagInfo::IP2D_flagFromV0ofTracks);
            BTag->setTaggerInfo(i_vectGrades,  xAOD::BTagInfo::IP2D_gradeOfTracks);
          }
        } else { // generic accessors
          //// need to handle to persistify for dynamic values before adding this
          if (m_storeTrackParticles) {
            BTag->setVariable<std::vector<ElementLink<xAOD::TrackParticleContainer> > > (
              m_xAODBaseName, "TrackParticleLinks", IPTracks );
            BTag->setDynTPELName( m_xAODBaseName, "TrackParticleLinks");
          }
          if (m_storeIpValues) {
            BTag->setVariable< std::vector<float> > (m_xAODBaseName, "valD0wrtPVofTracks", f_vectD0);
            if (m_impactParameterView=="3D") {
              BTag->setVariable< std::vector<float> > (m_xAODBaseName, "valZ0wrtPVofTracks", f_vectZ0);
            }
          }
          if (m_storeTrackParameters) {
            BTag->setVariable< std::vector<float> > (m_xAODBaseName, "sigD0wrtPVofTracks", f_vectD0Signi);
            if(m_impactParameterView=="3D") {
              BTag->setVariable< std::vector<float> > (m_xAODBaseName, "sigZ0wrtPVofTracks", f_vectZ0Signi);
            }
            BTag->setVariable< std::vector<float> > (m_xAODBaseName, "weightBofTracks",    vectWeightB);
            BTag->setVariable< std::vector<float> > (m_xAODBaseName, "weightUofTracks",    vectWeightU);
            BTag->setVariable< std::vector<float> > (m_xAODBaseName, "weightCofTracks",    vectWeightC);
            BTag->setVariable< std::vector<bool> >  (m_xAODBaseName, "flagFromV0ofTracks", vectFromV0);
            BTag->setVariable< std::vector<int> >   (m_xAODBaseName, "gradeOfTracks",      i_vectGrades);
          }
        }
      }
    }

    IPTracks.clear();
    
    m_likelihoodTool->clear();

    m_tracksInJet.clear();

    return StatusCode::SUCCESS;
  }

  /** compute individual track contribution to the likelihoods: */
  void IPTag::trackWeight(std::string author, TrackGrade grade, double sa0, double sz0, 
     double & twb, double & twu, double & twc) { // output parameters
    /** define and compute likelihood: */
    std::vector<Slice> slices;
    if(m_impactParameterView=="3D") {
      AtomicProperty atom1(sa0,"Significance of IP (rphi)");
      AtomicProperty atom2(sz0,"Significance of Z0");
      std::string compoName(author+"#");
      compoName += grade.gradeString();
      compoName += "/Sip3D";
      Composite compo1(compoName);
      compo1.atoms.push_back(atom1);
      compo1.atoms.push_back(atom2);
      Slice slice1("IP3D");
      slice1.composites.push_back(compo1);
      slices.push_back(slice1);
    }
    if(m_impactParameterView=="2D") {
      AtomicProperty atom1(sa0,"Significance of IP (rphi)");
      std::string compoName(author+"#");
      compoName += grade.gradeString();
      compoName += "/SipA0";
      Composite compo1(compoName);
      compo1.atoms.push_back(atom1);
      Slice slice1("IP2D");
      slice1.composites.push_back(compo1);
      slices.push_back(slice1);
    }
    if(m_impactParameterView=="1D") {
      AtomicProperty atom1(sz0,"Significance of IP (z)");
      std::string compoName(author+"#");
      compoName += grade.gradeString();
      compoName += "/SipZ0";
      Composite compo1(compoName);
      compo1.atoms.push_back(atom1);
      Slice slice1("IP1D");
      slice1.composites.push_back(compo1);
      slices.push_back(slice1);
    }
    m_likelihoodTool->setLhVariableValue(slices);
    std::vector<double> tmp = m_likelihoodTool->calculateLikelihood();
    m_likelihoodTool->clear();
    twb = tmp[0];
    twu = tmp[1];
    twc = 0.;
    if(m_useCHypo) twc = tmp[2];
  }


  
}//end namespace



