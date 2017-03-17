/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/JetProbTag.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/GenericFunctions/Erf.hh"

#include "GaudiKernel/IToolSvc.h"

//#include "JetEvent/Jet.h"
#include "JetTagEvent/TrackAssociation.h"
#include "xAODTracking/TrackParticle.h"
#include "JetTagInfo/JetProbInfoBase.h"
#include "JetTagInfo/IPInfoPlus.h"
#include "Navigation/NavigationToken.h"
#include "JetTagInfo/TruthInfo.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/GradedTrack.h"
#include "JetTagInfo/TrackGrade.h"
#include "JetTagCalibration/CalibrationBroker.h"

#include "JetTagInfo/TrackGradesDefinition.h"
#include "JetTagTools/ITrackGradeFactory.h"

#include "JetTagInfo/SvxSummary.h"

#include "GaudiKernel/ITHistSvc.h"
#include "JetTagTools/HistoHelperRoot.h"

#include "JetTagTools/SVForIPTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include <TH1.h>
#include <TF1.h>

#include <map>
#include <algorithm>
#include <cmath>

namespace Analysis {

  typedef std::vector<double> FloatVec;
  typedef std::vector<double>::iterator FloatVecIter;

  JetProbTag::JetProbTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_runModus("analysis"),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_trackSelectorTool("Analysis::TrackSelector"),
      m_calibrationTool("BTagCalibrationBroker"),
      m_secVxFinderNameForV0Removal("InDetVKalVxInJetTool"),
      m_secVxFinderNameForIPSign("InDetVKalVxInJetTool") ,
      m_SVForIPTool("Analysis::SVForIPTool"),
      m_trackGradeFactory("Analysis::BasicTrackGradeFactory"),
      m_unbiasIPEstimation(true)
  {
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus",       m_runModus);

    declareProperty("InputType", m_inputType = "Undefined");

    declareProperty("trackSelectorTool", m_trackSelectorTool);
    declareProperty("trackToVertexTool", m_trackToVertexTool);
    declareProperty("trackGradeFactory",m_trackGradeFactory);
 
    declareProperty("impactParameterView", m_impactParameterView = "2D");

    // track categories:
    declareProperty("trackGradePartitions", m_trackGradePartitionsDefinition);
    m_trackGradePartitionsDefinition.push_back("Good+Shared");

    declareProperty("UseNegIP",      m_negIP = true);
    declareProperty("UsePosIP",      m_posIP = true);
    declareProperty("flipIPSign",    m_flipIP = false);

    declareProperty("RejectBadTracks", m_RejectBadTracks = false);
    declareProperty("SignWithSvx",     m_SignWithSvx     = false);
    declareProperty("SecVxFinderNameForV0Removal",m_secVxFinderNameForV0Removal);
    declareProperty("SecVxFinderNameForIPSign",m_secVxFinderNameForIPSign);
    declareProperty("SVForIPTool",m_SVForIPTool);

    declareProperty("calibrationTool",  m_calibrationTool);

    // information to persistify:
    declareProperty("originalTPCollectionName", m_originalTPCollectionName = "TrackParticleCandidate");
    declareProperty("writeInfoBase", m_writeInfoBase = true);
    declareProperty("infoPlusName", m_infoPlusName = "IPInfoPlus");
    declareProperty("jetCollectionList", m_jetCollectionList);
    declareProperty("jetWithInfoPlus", m_jetWithInfoPlus);
    m_jetWithInfoPlus.push_back("Cone4H1Tower");

    // for making reference histograms:
    declareProperty("checkOverflows", m_checkOverflows = false); 
    declareProperty("jetPtMinRef", m_jetPtMinRef = 15.*Gaudi::Units::GeV);
    declareProperty("isolationDeltaR", m_isolationDeltaR = 0.8);
    declareProperty("useTrueFlavour", m_useTrueFlavour = false); 
    declareProperty("referenceType", m_referenceType = "ALL"); // B, UDSG, ALL
    declareProperty("truthMatchingName", m_truthMatchingName = "TruthInfo");
    declareProperty("purificationDeltaR", m_purificationDeltaR = 0.8);

    declareProperty("TrackToVertexIPEstimator",m_trackToVertexIPEstimator);
    declareProperty("unbiasIPEstimation",m_unbiasIPEstimation);

    // for using histograms vs fit
    declareProperty("UseHistograms", m_useHistograms = false);
    declareProperty("FitExpression", m_fitExpression);

    //// integral accuracy - speed up integral
    declareProperty("IAccuracy", m_iAccuracy=1e-8);
    
    m_fullIntegral = 1.;
    m_printWarning=true;

//     m_fitExpression ="[0]*TMath::Exp(-0.5*(x*x)/([1]*[1]))+[2]*TMath::Exp(-0.5*(x*x)/([3]*[3]))+TMath::Exp([4]+x*[5])+TMath::Exp([6]+x*[7])";
//     m_fitParams.clear(); 
 
//     //data
//     m_fitParams.push_back(0.26);  
//     m_fitParams.push_back(0.61);  
//     m_fitParams.push_back(0.19);  
//     m_fitParams.push_back(0.92);  
//     m_fitParams.push_back(2.94);  
//     m_fitParams.push_back(0.79);  
//     m_fitParams.push_back(6.08);  
//     m_fitParams.push_back(0.21);  
    
// mc 
//     m_fitParams.push_back(0.32);  
//     m_fitParams.push_back(0.56);  
//     m_fitParams.push_back(0.19);  
//     m_fitParams.push_back(0.89);  
//     m_fitParams.push_back(3.09);  
//     m_fitParams.push_back(0.81);  
//     m_fitParams.push_back(6.07);  
//     m_fitParams.push_back(0.23); 
 
    //fit of resolution function extracted from Data @ 7TeV using tracks in jets with d0<0, symmetrized
  }

  JetProbTag::~JetProbTag() {
  }


  StatusCode JetProbTag::initialize() {

    /** retrieving TrackToVertex: */
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexTool);
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_trackToVertexTool);
    }

    /** retrieving SVForIPTool: */
    if (m_SVForIPTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_SVForIPTool);
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_SVForIPTool);
    }

     /** retrieving the track grade factory */
    if ( m_trackGradeFactory.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackGradeFactory);
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_trackGradeFactory);
    }

    /** creation of TrackSelector: (private instance) */
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackSelectorTool);
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_trackSelectorTool);
    }

    if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexIPEstimator);
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
    }
    
    /** prepare the track partitions: */
    int nbPart = m_trackGradePartitionsDefinition.size();
    ATH_MSG_INFO("#BTAG#  Defining " << nbPart <<" track partitions: ");
    for(int i=0;i<nbPart;i++) {
      TrackGradePartition* part(0);
      try 
      {
        part = new TrackGradePartition(m_trackGradePartitionsDefinition[i],
                                       *m_trackGradeFactory);
      }
      catch (std::string error)
      {
        ATH_MSG_ERROR("#BTAG# Reported error " << error);

        ATH_MSG_ERROR("#BTAG#  List of categories provided to IPTag by jO : ");
        for (int l=0;l<nbPart;l++) {
          ATH_MSG_ERROR(" string " << m_trackGradePartitionsDefinition[i]);
        }
 
        ATH_MSG_ERROR("#BTAG#  List of categories provided by the TrackGradeFactory " << m_trackGradeFactory);

        const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();

        const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();

        std::vector<TrackGrade>::const_iterator listIter=gradeList.begin();
        std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();

        for ( ; listIter !=listEnd ; ++listIter )
        {
          ATH_MSG_ERROR("#BTAG# n. " << (*listIter).gradeNumber() << " string " << (*listIter).gradeString());
        }

        ATH_MSG_ERROR("#BTAG#  Terminating now... ");
        
        return StatusCode::FAILURE;
      }
      

      ATH_MSG_INFO((*part));
      m_trackGradePartitions.push_back(part);
    }

    /** retrieving calibrationTool: */
    if(m_runModus=="analysis") {
      if ( m_calibrationTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      } else {
	ATH_MSG_INFO("#BTAG# Retrieved tool " << m_calibrationTool);
      }
    }

    /** register calibration histograms: */
    if(m_runModus=="analysis") {
      ATH_MSG_DEBUG("#BTAG# of TrackGradePartitions: " << m_trackGradePartitions.size());
      for(unsigned int i=0; i<m_trackGradePartitions.size();i++) {
	ATH_MSG_DEBUG("#BTAG# TrackGradePartitions: " << i << " - # of Grades: "<< m_trackGradePartitions[i]->grades().size());
	for(unsigned int g=0; g<m_trackGradePartitions[i]->grades().size(); g++){
	  std::string hName;
	  hName = ((std::string)m_trackGradePartitions[i]->grades()[g])+"/Resol";
	  ATH_MSG_DEBUG("#BTAG# Registering histogram " << hName 
			<< " for track grade " << ((std::string)m_trackGradePartitions[i]->grades()[g])
			); 
	  m_calibrationTool->registerHistogram("JetProb", hName);
	}
      }
      if (!m_useHistograms){
	std::string hName = "";
	std::string m_fitOrigin = "";	
	if (m_inputType=="data"){
	  hName="Good/fit1_DATA";
	  m_fitOrigin="data";
	}
	else {
	  hName="Good/fit1_MC";  
	  m_fitOrigin="MC";
	}	
	ATH_MSG_INFO("#BTAG# Registering calibration fit from " << m_fitOrigin); 	
	m_calibrationTool->registerHistogram("JetProb",hName);
      }
      m_calibrationTool->printStatus();
    }

    /** book calibration histograms in reference mode: */ 
    if(m_runModus=="reference") {
      const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
      const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();
      std::vector<TrackGrade>::const_iterator listBegin=gradeList.begin();
      std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();
      // retrieve histohelper:
      ITHistSvc* myHistoSvc;
      if( service( "THistSvc", myHistoSvc ).isSuccess() ) {
        ATH_MSG_DEBUG("#BTAG# " << name() << ": HistoSvc loaded successfully.");
        m_histoHelper = new HistoHelperRoot(myHistoSvc);
        m_histoHelper->setCheckOverflows(m_checkOverflows);
	for(uint j=0;j<m_jetCollectionList.size();j++) {
          for(std::vector<TrackGrade>::const_iterator listIter=listBegin ; listIter !=listEnd ; ++listIter) {
            const TrackGrade & grd = (*listIter);
	    //if(grd==TrackGrade::Undefined) continue;
            std::string hName = "/RefFile/JetProb/" + m_jetCollectionList[j] + "/"
	                      + (std::string)grd + "/Resol"; 
	    ATH_MSG_VERBOSE("#BTAG# booking for JetProb: " << hName);
	    //use bins with variable size
	    const int nd0bins = 100;
	    double d0limits[nd0bins+1] = {0};
	    double d0min = -40;
	    double d0max = 40;
	    for(int i=0; i<=nd0bins; i++){
	      if(i<nd0bins/2) d0limits[i] = d0min * pow(10, -(4.*i)/nd0bins);
	      if(i>nd0bins/2) d0limits[i] = d0max * pow(10, -(4.*(nd0bins-i))/nd0bins);
	    }
	    m_histoHelper->bookHisto(hName,"d0 significance",nd0bins, d0limits);
	  }
	}
      } else {
        ATH_MSG_ERROR("#BTAG# " << name() << ": HistoSvc could NOT bo loaded.");
      }
    }

    return StatusCode::SUCCESS;
  }                                                    


  StatusCode JetProbTag::finalize() {
    for (size_t i = 0; i < m_trackGradePartitions.size(); i++)
      delete m_trackGradePartitions[i];
    return StatusCode::SUCCESS;
  }

  void JetProbTag::tagJet(xAOD::Jet& jetToTag) {

    /** author to know which jet algorithm: */
    std::string author = jetToTag.jetAuthor();

    /* Do not keep the InfoPlus for all jet collection */
    bool keepInfoPlus = false;
    if (std::find( m_jetWithInfoPlus.begin(), 
		   m_jetWithInfoPlus.end(), 
		   author ) != m_jetWithInfoPlus.end()) keepInfoPlus = true;

    /** retrieve the original TP collection for persistence: */
    /*if(m_runModus == "analysis" && keepInfoPlus) {
      if (evtStore()->retrieve(m_originalTPCollection, m_originalTPCollectionName).isFailure()) {
	ATH_MSG_ERROR("#BTAG# " << m_originalTPCollectionName << " not found in StoreGate.");
	return;
      } else {
	ATH_MSG_VERBOSE("#BTAG# TrackParticleContainer " << m_originalTPCollectionName << " found.");
      }
    }*/

    /** for reference mode: */
    bool jetIsOkForReference = false;
    if(m_runModus == "reference") {
      // here we require a jet selection:
      if( jetToTag.pt()>m_jetPtMinRef && fabs(jetToTag.eta())<2.5 ) {
	if(!m_useTrueFlavour) {
	  jetIsOkForReference = true;
	} else {
          // and also a truth match:
          const TruthInfo* mcinfo = jetToTag.tagInfo<TruthInfo>(m_truthMatchingName);
	  double deltaRmin(0.);
          if( mcinfo ) {
	    std::string label = mcinfo->jetTruthLabel();
	    // for purification: require no b or c quark closer than dR=m_purificationDeltaR
	    double deltaRtoClosestB = mcinfo->deltaRMinTo("B");
	    double deltaRtoClosestC = mcinfo->deltaRMinTo("C");
	    deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
            double deltaRtoClosestT = mcinfo->deltaRMinTo("T");
            deltaRmin = deltaRtoClosestT < deltaRmin ? deltaRtoClosestT : deltaRmin;
	    if ( (    "B"==m_referenceType &&   "B"==label ) ||  // b-jets    
	         ( "UDSG"==m_referenceType && "N/A"==label ) ||  // light jets
	         (  "ALL"==m_referenceType && // all jets: b + purified light jets
	  	    ( "B"==label || ( "N/A"==label && deltaRmin > m_purificationDeltaR ) ) )
	         ) jetIsOkForReference = true;
          } else {
            ATH_MSG_WARNING("#BTAG# No TruthInfo ! Cannot run on reference mode !");
          }
        }
      }
    }

    int nbPart = m_trackGradePartitionsDefinition.size();

    std::vector<const Trk::Track*> TrkFromV0;

    Amg::Vector3D SvxDirection;
    bool canUseSvxDirection=false;

    if (m_SVForIPTool) {
      if (m_SignWithSvx) {
	m_SVForIPTool->getDirectionFromSecondaryVertexInfo(SvxDirection,canUseSvxDirection,//output
							   jetToTag,m_secVxFinderNameForIPSign,m_priVtx->recVertex());//input
      }
      
      // bad tracks from V0s, conversions, interactions:
      m_SVForIPTool->getTrkFromV0FromSecondaryVertexInfo(TrkFromV0,//output
							 jetToTag,m_secVxFinderNameForV0Removal);//input
    }

    /** extract the TrackParticles from the jet and apply track selection: */
    int nbClus = 0;
    int nbTrak = 0;
    if (m_trackSelectorTool) {
      m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
      m_trackSelectorTool->prepare();
    }
    //JBdV does the jet have bad Trk ?
    int numberOfBadTracks = 0;

    std::vector<const xAOD::TrackParticle*>* trackVector = NULL;
    const Analysis::TrackAssociation *trkInJet = jetToTag.getAssociation<TrackAssociation>("Tracks");
    if(!trkInJet){
      ATH_MSG_WARNING("#BTAG#  Could not find tracks in jets");
    }
    else{
      trackVector = jetToTag.getAssociation<TrackAssociation>("Tracks")->tracks();
      
      for (std::vector<const xAOD::TrackParticle*>::iterator jetItr = trackVector->begin(); jetItr != trackVector->end() ; ++jetItr ) {
	const xAOD::TrackParticle * aTemp = *jetItr;
	nbTrak++;
	if( m_trackSelectorTool && m_trackSelectorTool->selectTrack(aTemp) ) {
	  
	  TrackGrade * theGrade = m_trackGradeFactory->getGrade(*aTemp,
								jetToTag.p4() );
	  
	ATH_MSG_VERBOSE("#BTAG#  result of selectTrack is OK, grade= "
			<< (std::string)(*theGrade));
	
	bool tobeUsed = false;
	for(int i=0;i<nbPart;i++) {
	  if (std::find((m_trackGradePartitions[i]->grades()).begin(), (m_trackGradePartitions[i]->grades()).end(), *theGrade) 
	      != (m_trackGradePartitions[i]->grades()).end()) tobeUsed = true;
	}
	// Is it a bad track ?
	std::string suffix = "g";
	if (std::find(TrkFromV0.begin(),TrkFromV0.end(),*aTemp->track()) != TrkFromV0.end()) {
	  suffix = "b";
	  numberOfBadTracks++;
	  ATH_MSG_VERBOSE("#BTAG# Bad track in jet, pt = " << aTemp->pt() << " eta = " << aTemp->eta() << " phi = " << aTemp->phi());
	  if (m_RejectBadTracks) tobeUsed = false;
	}
	if (tobeUsed) m_tracksInJet.push_back(GradedTrack(aTemp, *theGrade));
	delete theGrade;
	theGrade = 0;
	}
      }
      delete trackVector; trackVector=0;
    }

    ATH_MSG_VERBOSE("#BTAG#  #clusters = " << nbClus << " #tracks = " << nbTrak);
    ATH_MSG_VERBOSE("#BTAG# The z of the primary = "<<m_priVtx->recVertex().position().z());

    /** jet direction: */
    Amg::Vector3D jetDirection(jetToTag.px(),jetToTag.py(),jetToTag.pz());
    Amg::Vector3D unit = jetDirection.unit();
    //JBdV Try
    if (m_SignWithSvx && canUseSvxDirection) unit = SvxDirection.unit();

    FloatVec vectD0;
    FloatVec vectD0Signi;
    std::vector<TrackGrade> vectGrades;
    std::vector<const xAOD::TrackParticle*> vectTP;
    std::vector<bool> vectFromV0;
    FloatVec trackProbs;
    // reserve approximate space (optimization):
    const int nbTrackMean = 3;
    vectD0.reserve(nbTrackMean);
    vectD0Signi.reserve(nbTrackMean);
    vectGrades.reserve(nbTrackMean);
    vectTP.reserve(nbTrackMean);
    vectFromV0.reserve(nbTrackMean);
    trackProbs.reserve(nbTrackMean);

    for (std::vector<GradedTrack>::iterator trkItr = m_tracksInJet.begin(); trkItr != m_tracksInJet.end(); ++trkItr) {
      const xAOD::TrackParticle* trk = (*trkItr).track;

      bool isFromV0 = (std::find(TrkFromV0.begin(),TrkFromV0.end(),*trk->track()) != TrkFromV0.end());

      /** track parameters defined at the primary vertex:
	  temp: can use d0/z0 at perigee for comparison with CBNT */
      double d0wrtPriVtx(0.);
      double d0ErrwrtPriVtx(1.);
      /* use new Tool for "unbiased" IP estimation */
      const Trk::ImpactParametersAndSigma* myIPandSigma(0);
      if (m_trackToVertexIPEstimator) { 
	myIPandSigma = m_trackToVertexIPEstimator->estimate(*trk->track(),m_priVtx,m_unbiasIPEstimation);
      }
      if(0==myIPandSigma) {
	ATH_MSG_WARNING("#BTAG# JetProbTAG: trackToVertexIPEstimator failed !");
      } else {
	d0wrtPriVtx=myIPandSigma->IPd0;
	d0ErrwrtPriVtx=myIPandSigma->sigmad0;
	delete myIPandSigma;
	myIPandSigma=0;
      }

      /** sign of the impact parameter */
      double signOfIP(1.);
      if (m_trackToVertexIPEstimator) {
	if(m_impactParameterView=="2D" || m_impactParameterView=="1D") {
	  signOfIP = m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack(trk->perigeeParameters(),
									unit,m_priVtx->recVertex());
	}
	if(m_impactParameterView=="3D") {
	  signOfIP = m_trackToVertexIPEstimator->get3DLifetimeSignOfTrack(trk->perigeeParameters(),
									unit,m_priVtx->recVertex());
	}
      }
      
      /** fill reference histograms: */
      if( m_runModus == "reference" && jetIsOkForReference && signOfIP < 0 ) {
	const TrackGradesDefinition & trackFactoryGradesDefinition = m_trackGradeFactory->getTrackGradesDefinition();
        const std::vector<TrackGrade> & gradeList = trackFactoryGradesDefinition.getList();
        std::vector<TrackGrade>::const_iterator listIter=gradeList.begin();
        std::vector<TrackGrade>::const_iterator listEnd=gradeList.end();
        for ( ; listIter !=listEnd ; ++listIter ) {
          const TrackGrade & grd = (*listIter);
          //if(grd==TrackGrade::Undefined) continue;
	  ATH_MSG_DEBUG("#BTAG#   filling ref histo for grade " << (std::string)grd);
	  if( grd==(*trkItr).grade ) { // check the grade of current track
	    ATH_MSG_DEBUG("#BTAG#   track is of required grade ");
            const std::string suffix = "_" + (std::string)grd;
	    std::string hName = "/RefFile/JetProb/" + author + "/"
		              + (std::string)grd + "/Resol";
	    float val = fabs(d0wrtPriVtx/d0ErrwrtPriVtx);
	    ATH_MSG_DEBUG("#BTAG#   histo JetProb: " << hName << " " << val);
	    double binwidthminus = (m_histoHelper->getHisto1D(hName))->GetBinWidth(m_histoHelper->getHisto1D(hName)->FindBin(-val));
	    double binweightminus = 1./binwidthminus;
	    double binwidthplus = (m_histoHelper->getHisto1D(hName))->GetBinWidth(m_histoHelper->getHisto1D(hName)->FindBin(+val));
	    double binweightplus = 1./binwidthplus;
	    m_histoHelper->fillHistoWithWeight(hName,-val,binweightminus);
	    m_histoHelper->fillHistoWithWeight(hName,+val,binweightplus);
	  }
	}
      }

      if( m_runModus == "analysis"){
	/** select sign of IP, flip for negative tags */
	if(signOfIP > 0  && !m_posIP) continue;
	if(signOfIP <= 0 && !m_negIP) continue;
	if(m_flipIP) signOfIP *= -1;
	/** significances */
	double sIP = signOfIP*fabs(d0wrtPriVtx);
	double significance= signOfIP*fabs(d0wrtPriVtx/d0ErrwrtPriVtx);

	/** track grade and jet author*/
	ATH_MSG_VERBOSE("#BTAG# Track Grade...");
	TrackGrade grd = (*trkItr).grade;
	std::string trkName = author+":"+(std::string)grd;

	/** compute track probablity: */
	ATH_MSG_VERBOSE("#BTAG# Compute trackProb...");
	double trackProb = this->m_getTrackProb(significance, trkName);
        if(trackProb ==0){
	  trackProb=1e-10;
          ATH_MSG_DEBUG("#BTAG# Setting trackProb ==0 to 1e-10");
        }
	if(trackProb >= 100){
	  if(122 == trackProb){
	    if(m_printWarning){
	      ATH_MSG_WARNING("#BTAG# error: calibration fit for JetProb not available for track "<<trkName );
	      ATH_MSG_WARNING("#BTAG# -> dummy values will be filled - message level changed to DEBUG");
	      m_printWarning=false;
	    }
	    ATH_MSG_DEBUG("#BTAG# error: calibration fit for JetProb not available: dummy values will be filled");
	    continue;
	  }
	  ATH_MSG_WARNING("#BTAG# Error code " << trackProb << " returned when trying to compute track probability...");
	  continue;
	}
	if(trackProb>1){ // Should not occure now
	  ATH_MSG_DEBUG("#BTAG# Correcting floating point precision: " << trackProb << " returned when computing track probability...");
	  trackProb=1.;
	}

	vectD0.push_back(sIP);
	vectD0Signi.push_back(significance);
	vectGrades.push_back((*trkItr).grade);
	vectTP.push_back(trk);
	vectFromV0.push_back(isFromV0);
	trackProbs.push_back(trackProb);
	ATH_MSG_VERBOSE("#BTAG# trackProb = " << trackProb); 

	ATH_MSG_VERBOSE("#BTAG# JetProbTAG: Trk " 
			<< " d0= " << sIP 
			<< "+-" << d0ErrwrtPriVtx
			<< " sigd0= " << significance
			<< " trackProb= " << trackProb
			);
      }
    } // endloop on tracks

    JetProbInfoBase* infoBase(0);
    IPInfoPlus* infoPlus(0);
    bool isIPInfoPlusAlreadyThere = false;
    if(m_runModus=="analysis") {
      std::string instanceName("JetProb");
      if(m_flipIP) instanceName += "Neg";

      // -- fill new base info class ... 
      if(m_writeInfoBase) {
	infoBase = new JetProbInfoBase(instanceName);
	infoBase->nbTracks(vectTP.size());
      }
      // -- fill extended info class ...
      uint nbt = vectTP.size();
      if(keepInfoPlus && nbt > 0) {
	ATH_MSG_VERBOSE("#BTAG# Filling IPInfoPlus...");
        infoPlus = const_cast<IPInfoPlus*>(jetToTag.tagInfo<IPInfoPlus>(m_infoPlusName));
	if(infoPlus) {
	  ATH_MSG_VERBOSE("#BTAG# Previous IPInfoPlus " << m_infoPlusName << " found...");
	  isIPInfoPlusAlreadyThere = true;
	  // iterate on tracks and fill track weights
	  for(uint i=0;i<nbt;i++) {
	    if(m_flipIP) {
	      infoPlus->updateTrackWeight(vectTP[i],"JPneg",trackProbs[i]);
	    } else { 
	      infoPlus->updateTrackWeight(vectTP[i],"JP",trackProbs[i]);
	    }
          }
	} else {
	  infoPlus = new IPInfoPlus(m_infoPlusName);
	  if(infoPlus) {
	    ATH_MSG_VERBOSE("#BTAG# " << m_infoPlusName << " not found. New IPInfoPlus created...");
	    uint nbt = vectTP.size();
	    for(uint i=0;i<nbt;i++) {
	      IPTrackInfo tinfo(m_originalTPCollection, vectTP[i], 
				vectGrades[i], vectFromV0[i],
				vectD0[i], vectD0Signi[i],
				0., 0.);
	      tinfo.resetW();
	      infoPlus->addTrackInfo(tinfo);
	      if(m_flipIP) {
	        infoPlus->updateTrackWeight(vectTP[i],"JPneg",trackProbs[i]);
	      } else { 
	        infoPlus->updateTrackWeight(vectTP[i],"JP",trackProbs[i]);
	      }
	    }
	  }
	}
      }
    }

    /** compute jet prob: */
    ATH_MSG_VERBOSE("#BTAG# Compute JetProb...");
    double P0 = 1.;
    for(unsigned int it=0;it<trackProbs.size();it++) {
      P0*=trackProbs[it];
    }
    double PJet = 0.;
    for(unsigned int it=0;it<trackProbs.size();it++) {
      PJet += pow(-log(P0),it)/this->m_factorial(it);
    }
    PJet *= P0;
    if (trackProbs.size() == 0) PJet = 1;
    ATH_MSG_VERBOSE("#BTAG# JetProb: #tracks= " << trackProbs.size()
	 <<" P0= " << P0 << " Pj= " << PJet);

    std::vector<double> like;
    like.clear();
    like.push_back(PJet);
    if (infoBase) infoBase->setTagLikelihood(like);

    /** tagging done. Fill the JetTag and return ... */
    if (infoBase) jetToTag.addInfo(infoBase);
    if (infoPlus && !isIPInfoPlusAlreadyThere) jetToTag.addInfo(infoPlus);
    m_tracksInJet.clear();
  }

  void JetProbTag::finalizeHistos()
  {
  }

  double JetProbTag::m_getTrackProb(double significance, std::string trkName) {

    if(m_useHistograms){
      return m_getTrackProbFromHistograms(significance, trkName);
    }else{
      return m_getTrackProbFromFit(significance, trkName);
    }
  } 

  double JetProbTag::m_getTrackProbFromHistograms(double significance, std::string trkName){

    ATH_MSG_VERBOSE("#BTAG# Inside getTrackProbFromHistogram...");

    // find track grade and jet author by parsing trkName
    std::string grd = "";
    std::string author = "";
    if(trkName!=""){
      const std::string delim(":");
      std::string::size_type sPos, sEnd, sLen;
      sPos = trkName.find_first_not_of(delim);
      if(sPos != std::string::npos){
	sEnd = trkName.find_first_of(delim, sPos);
	if(sEnd!=std::string::npos){
	  sLen = sEnd - sPos;
	  author = trkName.substr(sPos,sLen);
	  sPos = trkName.find_first_not_of(delim, sEnd);
	  if(sPos != std::string::npos){
	    sEnd = trkName.find_first_of(delim, sPos);
	    if(sEnd==std::string::npos){
	      sEnd = trkName.length();
	      sLen = sEnd - sPos;
	      grd = trkName.substr(sPos,sLen);
	    }
	  }  
	}
      }
    }
    if("" == author || "" == grd){
      return 100.; // Parsing error
    }
    ATH_MSG_VERBOSE("#BTAG# Getting track probability: "
	 << " Name: " << trkName 
	 << " - grade: " << grd 
	 << " - Jet Author: " << author
	);

    // find track grade partition
    std::string part = "";
    for(unsigned int tgp=0; tgp<m_trackGradePartitions.size(); tgp++){      
      part = m_trackGradePartitions[tgp]->suffix();
      if(part.find(grd) != std::string::npos){
	break;
      }
      part = "";
    }
    if(""==part){
      return 101.; // Track grade not found in track grade partitions
    }
    ATH_MSG_VERBOSE("#BTAG# Using track grade partition: " << part);

    // get grade list
    std::vector<std::string> gradeList;
    {
      const std::string delimUds("_");
      std::string::size_type sPos, sEnd, sLen;
      sPos = part.find_first_not_of(delimUds);
      while ( sPos != std::string::npos ) {
	sEnd = part.find_first_of(delimUds, sPos);
	if(sEnd==std::string::npos) sEnd = part.length();
	sLen = sEnd - sPos;
	std::string word = part.substr(sPos,sLen);
	gradeList.push_back(word);
	sPos = part.find_first_not_of(delimUds, sEnd);
      }
    }
    if(gradeList.size() < 1){
      return 102.; // grade partition parsing error
    }
    ATH_MSG_VERBOSE("#BTAG# Grade list contains " << gradeList.size() << " grades");

    // get resolution histogram
    TH1* hresol = 0;
    for(unsigned int g=0; g<gradeList.size(); g++){
      std::string hName = gradeList[g]+"/Resol";
      std::pair<TH1*, bool> rth = m_calibrationTool->retrieveHistogram("JetProb",author,hName);
      if(!rth.first){
	ATH_MSG_WARNING("#BTAG# Histogram pointer is not valid for: " 
	     << " jet author: " << author 
	     << " - track grade: " << gradeList[g] 
	     << " - histo name: " << hName 
	     << " - histo pointer: " << rth.first
	    );
	return 103.; // histogram pointer is not valid! database error!
      }
      if(0 == hresol){
	hresol = (TH1*)rth.first->Clone();
	hresol->Scale(hresol->GetEntries()/hresol->Integral());
      }else{
	hresol->Add(rth.first, rth.first->GetEntries()/rth.first->Integral());
      }
      ATH_MSG_VERBOSE("#BTAG# Added histogram: "
	   << " jet author: " << author 
	   << " - track grade: " << gradeList[g] 
	   << " - histo name: " << hName 
	  );
    }
    if(0==hresol){
      return 104.; // histgram pointer is not valid... gradeList empty?!
    }

    // Smooth histogram
    hresol->Smooth();

    // normalize histogram
    if ((!m_negIP && m_posIP) || (m_negIP && !m_posIP)) hresol->Scale(1./hresol->Integral(0,hresol->GetNbinsX()+1,"width"));
    else     hresol->Scale(1./hresol->Integral(0,hresol->GetNbinsX()+1,"width"));

    // Get track probability
    double trkp = 1.;
    int nbins = hresol->GetNbinsX();
    double overflow = hresol->GetBinContent(nbins+1);
    int bin = hresol->FindBin(significance);
    if (bin > nbins) {
      if (0. == overflow){
	delete hresol;
	return 105.; // No overflow (e.g. if not enough stat. to calibrate) : do not use the track (conservative)
      }
      trkp = overflow;
    } else if (bin >= 1) {
      double sigdn  = hresol->GetBinLowEdge(bin);
      double sigup  = hresol->GetBinLowEdge(bin+1);
      double trkpdn = hresol->Integral(bin, nbins,"width") + overflow;
      double trkpup = overflow;
      if (bin+1 <= nbins) trkpup += hresol->Integral(bin+1, nbins,"width");
      trkp = trkpdn + (significance - sigdn) * (trkpup - trkpdn) / (sigup - sigdn);
    }
    delete hresol;
    return trkp;
  }

  double JetProbTag::m_getTrackProbFromFit(double significance, std::string trkName){

    double S = fabs(significance);

    std::string author = "";

    const std::string delim(":");
    std::string::size_type sPos, sEnd, sLen;
    sPos = trkName.find_first_not_of(delim);
    if(sPos != std::string::npos){
      sEnd = trkName.find_first_of(delim, sPos);
      if(sEnd!=std::string::npos){
	sLen = sEnd - sPos;
	author = trkName.substr(sPos,sLen);
      }
    }

    std::string hName = "";
    if (m_inputType=="data") hName="Good/fit1_DATA";
    else hName="Good/fit1_MC";   
    std::pair<TF1*, bool> rth = m_calibrationTool->retrieveTObject<TF1>("JetProb",author,hName);
    if(!rth.first){
      return 122.;
    }
    //TF1* F = (TF1*)rth.first->Clone();
    TF1* F = rth.first;



    double *opt=0;
    if(rth.second){
      ATH_MSG_INFO("#BTAG# calibration updated -> update cache ");
      //m_fullIntegral = F->Integral(-100.,100.,opt,m_iAccuracy);
      m_fullIntegral = F->Integral(-100.,100.);
      m_calibrationTool->updateHistogramStatus("JetProb",author,hName,false);
    }

    ATH_MSG_VERBOSE("#BTAG# Fit : hName : "<<hName <<" author : "<<author);
 

    for(int i=0; i<F->GetNumberFreeParameters(); i++){
      ATH_MSG_DEBUG("#BTAG# Fit parameter # "<<i<<" : "<< F->GetParameter(i));
    }
    double p = F->Integral(S,100.,opt,m_iAccuracy)/m_fullIntegral;

 //    F->Clear();
//     delete F;

    if(p<0) p = 0;
    if(p>1) p = 1;

    return p;
  }

  double JetProbTag::m_factorial(int n) {
    double fac = 1.;
    if (n > 1) {
      fac = n * this->m_factorial(n - 1);
    }
    return fac;
  }

}

