/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// NAME:     LArCellMonTool.cxx

//                W.Lampl  - Spring 2017: Major re-design
// ********************************************************************
#include "LArCellMonTool.h"

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadChanBitPacking.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/Units.h"

#include "TProfile2D.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TH1F.h"

#include <cassert>
#include <algorithm>

using namespace CaloMonitoring;
using Athena::Units::GeV;

////////////////////////////////////////////
LArCellMonTool::LArCellMonTool(const std::string& type, const std::string& name,const IInterface* parent) 
  :CaloMonToolBase(type, name, parent),
   m_useElectronicNoiseOnly(false),
   m_noiseTool("CaloNoiseTool"),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
   m_badChannelMask("BadLArRawChannelMask",this),
   m_LArOnlineIDHelper(nullptr),
   m_calo_id(nullptr),
   m_counter_sporadic_protc(0),
   m_h_sporadicHists()
{    
  declareInterface<IMonitorToolBase>(this);

  declareProperty("DoSaveTempHists",m_doSaveTempHists=false,"Store temporary, intermediate histograms in a /Temp/ directory (for debugging");

  // tools 
  declareProperty("useElectronicNoiseOnly",m_useElectronicNoiseOnly=false,"Consider only electronic noise and ignore pile-up contributiuon)");
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise Tool");

  // Trigger Awareness:
  declareProperty("useTrigger",m_useTrigger=true);
  declareProperty("rndmTriggerNames", m_triggerNames[RNDM]);
  declareProperty("caloTriggerNames",m_triggerNames[CALO]);
  declareProperty("minBiasTriggerNames",m_triggerNames[MINBIAS]);
  declareProperty("metTriggerNames",m_triggerNames[MET]);
  declareProperty("miscTriggerNames",m_triggerNames[MISC]);
  
  // Bad channel masking options 
  declareProperty("LArBadChannelMask",m_badChannelMask,"Tool handle for LArBadChanelMasker AlgTool");
  declareProperty("MaskBadChannels",m_maskKnownBadChannels=false,"Do not fill histograms with values from known bad channels"); 
  declareProperty("MaskNoCondChannels", m_maskNoCondChannels=false,"Do not fill histograms with values from cells reco'ed w/o conditions database");

  // Switch on/off additional (non thresholded) histograms:
  declareProperty("doDatabaseNoiseVsEtaPhi",m_doDatabaseNoiseVsEtaPhi=true);
  declareProperty("doKnownBadChannelsVsEtaPhi",m_doKnownBadChannelsVsEtaPhi=true);
  declareProperty("doEnergyVsTime", m_doEnergyVsTime=true);
  declareProperty("doUnnormalized1DEnergy", m_doUnnormalized1DEnergy=false);
  declareProperty("useLogarithmicEnergyBinning", m_useLogarithmicEnergyBinning=false);

  // Switch to use WeightedEff rather than WeightedAvg merging where appropriate:
  declareProperty("useWeightedEffMerging",m_useWeightedEffMerging=true,"set the merging strategy for occupancy plots");

  // sporadic threshold 
  declareProperty("Sporadic_switch",m_sporadic_switch=false);
  declareProperty("minSporadicEvents",m_minSporadicNoiseEventsPerCell=10); //Attention: 0 means never!
  declareProperty("Threshold_EM_S0S1",m_threshold_em_S0S1 = 5000);
  declareProperty("Threshold_HECFCALEMS2S3",m_threshold_HECFCALEMS2S3=15000);
  declareProperty("NsporadicPlotLimit",m_sporadicPlotLimit=300);
  declareProperty("Sporadic_protection",m_sporadic_protc = 4000);

  // Switch on/off thresholded histograms for each threshold:
  declareProperty("ThresholdType", m_thresholdNameProp,"Vector of threshold names");
  declareProperty("ThresholdDirection", m_thresholdDirectionProp,"Vector of threshold directions");
  declareProperty("TriggersToInclude", m_triggersToIncludeProp);
  declareProperty("TriggersToExclude", m_triggersToExcludeProp);

  declareProperty("DoPercentageOccupancy",m_doPercentageOccupancyProp,"Turns both the 1D and 2D occupancy from absolute counts to percentage of events tha passed the threshold");
  declareProperty("DoEtaPhiOccupancy",m_doEtaPhiOccupancyProp,"Activate 2D occupancy plots (per threshold-type)");
  declareProperty("DoEtaOccupancy",m_doEtaOccupancyProp,"Activate occupancy-vs eta plots (per threshold-type)");
  declareProperty("DoPhiOccupancy",m_doPhiOccupancyProp,"Activate occupancy-vs phi plots (per threshold-type)");

  declareProperty("DoEtaPhiAverageEnergy",m_doEtaPhiAverageEnergyProp);
  declareProperty("DoEtaPhiTotalEnergy",m_doEtaPhiTotalEnergyProp);
 
  declareProperty("DoEtaPhiAverageQuality",m_doEtaPhiAverageQualityProp);
  declareProperty("DoEtaPhiFractionOverQth", m_doEtaPhiFractionOverQthProp);
  declareProperty("QualityFactorThreshold",m_qualityFactorThresholdProp);

  declareProperty("DoEtaPhiAverageTime",m_doEtaPhiAverageTimeProp);
  declareProperty("DoEtaPhiFractionPastTth", m_doEtaPhiFractionPastTthProp);
  declareProperty("TimeThreshold",m_timeThresholdProp);  

  declareProperty("DoBeamBackgroundRemoval",m_doBeamBackgroundRemovalProp);

  declareProperty("ThresholdTitleTemplates", m_thresholdTitleTemplates);
  declareProperty("DefaultThresholds", m_defaultThresholds);
  declareProperty("ThresholdinSigNoise", m_inSigNoise);

  declareProperty("ThresholdColumnType", m_thresholdColumnType);

  // Lar Thresholds
  // Em Barrel
  declareProperty("EMBP_Thresh",m_thresholdsProp[EMBPNS]);
  declareProperty("EMB1_Thresh",m_thresholdsProp[EMB1NS]);
  declareProperty("EMB2_Thresh",m_thresholdsProp[EMB2NS]);
  declareProperty("EMB3_Thresh",m_thresholdsProp[EMB3NS]);
  // EM Endcap
  declareProperty("EMECP_Thresh",m_thresholdsProp[EMECPNS]);
  declareProperty("EMEC1_Thresh",m_thresholdsProp[EMEC1NS]);
  declareProperty("EMEC2_Thresh",m_thresholdsProp[EMEC2NS]);
  declareProperty("EMEC3_Thresh",m_thresholdsProp[EMEC3NS]);
  // Hadronic Endcap
  declareProperty("HEC0_Thresh",m_thresholdsProp[HEC0NS]);
  declareProperty("HEC1_Thresh",m_thresholdsProp[HEC1NS]);
  declareProperty("HEC2_Thresh",m_thresholdsProp[HEC2NS]);
  declareProperty("HEC3_Thresh",m_thresholdsProp[HEC3NS]);
  // Forward Calorimeters
  declareProperty("FCAL1_Thresh",m_thresholdsProp[FCAL1NS]);
  declareProperty("FCAL2_Thresh",m_thresholdsProp[FCAL2NS]);
  declareProperty("FCAL3_Thresh",m_thresholdsProp[FCAL3NS]);
}



LArCellMonTool::~LArCellMonTool() {

}

////////////////////////////////////////////
StatusCode LArCellMonTool::initialize() {

  ATH_MSG_DEBUG("LArCellMonTool::initialize() start");

  //Identfier-helpers 
  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_calo_id) );

  // Bad channel masker tool 
  if(m_maskKnownBadChannels){
    ATH_CHECK(m_badChannelMask.retrieve());
  }
  else {
    m_badChannelMask.disable();
  }

  ATH_CHECK( m_cablingKey.initialize() );

  // Bad channels key
  //(used for building eta phi map of known bad channels)
  if( m_doKnownBadChannelsVsEtaPhi) {
    ATH_CHECK(m_BCKey.initialize());
  }

  //Calonoise tool
  ATH_CHECK(m_noiseTool.retrieve());


  //JobO consistency check:
  if (m_useTrigger && std::all_of(m_triggerNames.begin(),m_triggerNames.end(),[](const std::string& trigName){return trigName.empty();})) {
      ATH_MSG_WARNING("UseTrigger set to true but no trigger names given! Forcing useTrigger to false");
      m_useTrigger=false;
  }
    
  //retrieve trigger decision tool and chain groups
  if( m_useTrigger) {
    ATH_CHECK(m_trigDec.retrieve());
    ATH_MSG_INFO("TrigDecisionTool retrieved");
    for (size_t i=0;i<NOTA;++i) {
      const std::string& trigName=m_triggerNames[i];
      if (!trigName.empty()) m_chainGroups[i]=m_trigDec->getChainGroup(trigName.c_str());
    }//end loop over TriggerType enum
  }//end if m_useTrigger
  else {
    m_trigDec.disable();
  }

  //Choose noise type
  if (m_useElectronicNoiseOnly) 
    m_noiseType = ICalorimeterNoiseTool::ELECTRONICNOISE;
  else
    m_noiseType=ICalorimeterNoiseTool::TOTALNOISE; 

  // Sets the threshold value arrays
  ATH_CHECK(initThreshHists());

  //Fill the LArCellBinning for each layer
  setLArCellBinning();

  //Initialize read handle key
  ATH_CHECK( m_cellContainerName.initialize() );

  //Call base-call initialize methods
  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  ATH_CHECK( CaloMonToolBase::initialize() );

  ATH_MSG_DEBUG("LArCellMonTool::initialize() is done!");
  return StatusCode::SUCCESS;
}



StatusCode LArCellMonTool::initThreshHists() {

  //Interpret the jobO defining the per-threshold histograms 
  //and initialize vector<thresholdHist_t> m_thresholdHists;

  const size_t nThr=m_thresholdNameProp.value().size();

  //Check uniqueness of threshold-type by filling a set
  std::set<std::string> uniqunesCheck;
  for (const std::string& s : m_thresholdNameProp.value()) {
    auto r=uniqunesCheck.insert(s);
    if (!r.second) {
      ATH_MSG_ERROR( "Configuration error: Threshold type " << s << " appears more than once" );
      return StatusCode::FAILURE;
    }
  }
  uniqunesCheck.clear();    


  StatusCode sc=StatusCode::SUCCESS;
  //Check consistency:
#define CONSISTENCY(ARRAYPROP) if (ARRAYPROP.value().size()!= nThr) {\
    ATH_MSG_ERROR( "Configuration problem: Property 'ThresholdType' has size " <<  nThr \
		    <<  " but '" << ARRAYPROP.name() << "' has size " << ARRAYPROP.value().size() );\
    sc=StatusCode::FAILURE;    \
  }
  
  CONSISTENCY(m_thresholdDirectionProp);
  CONSISTENCY(m_defaultThresholds);
  CONSISTENCY(m_inSigNoise);
  CONSISTENCY(m_thresholdTitleTemplates);
  CONSISTENCY(m_triggersToIncludeProp);
  CONSISTENCY(m_triggersToExcludeProp);
  CONSISTENCY(m_doPercentageOccupancyProp);
  CONSISTENCY(m_doEtaPhiOccupancyProp); 
  CONSISTENCY(m_doEtaOccupancyProp);
  CONSISTENCY(m_doPhiOccupancyProp);
  CONSISTENCY(m_doEtaPhiTotalEnergyProp);
  CONSISTENCY(m_doEtaPhiAverageEnergyProp);
  CONSISTENCY(m_doEtaPhiAverageQualityProp);
  CONSISTENCY(m_doEtaPhiFractionOverQthProp);
  CONSISTENCY(m_qualityFactorThresholdProp);
  CONSISTENCY(m_doEtaPhiAverageTimeProp);
  CONSISTENCY(m_doEtaPhiFractionPastTthProp);
  CONSISTENCY(m_timeThresholdProp);
  CONSISTENCY(m_doBeamBackgroundRemovalProp); 
#undef CONSISTENCY  

  if (sc.isFailure()) return sc;

  const std::map<const std::string, const Direction> stringToDirection {{"over",OVER},{"under",UNDER},{"both",BOTH},{"none",NONE}};
  const std::map<const std::string, const TriggerType> stringToTrigType {{"nota",NOTA},{"rndm",RNDM},{"calo",CALO},{"minbias",MINBIAS},
									{"met",MET},{"misc",MISC},{"none",NOTA},{"all",NOTA}};

  m_thresholdHists.resize(nThr);
  for (size_t iThr=0;iThr<nThr;++iThr) {
    thresholdHist_t& threshold=m_thresholdHists[iThr];
    threshold.m_threshName=m_thresholdNameProp.value()[iThr];
    threshold.m_threshTitleTemplate= m_thresholdTitleTemplates.value()[iThr];
    std::fill(threshold.m_threshValue.begin(),threshold.m_threshValue.end(),m_defaultThresholds.value()[iThr]);
    threshold.m_inSigNoise=m_inSigNoise.value()[iThr];

    auto itD=stringToDirection.find(strToLower(m_thresholdDirectionProp.value()[iThr]));
    if (itD!=stringToDirection.end()) {
      threshold.m_threshDirection=itD->second;
    }
    else {
      ATH_MSG_ERROR( "Configuration problem. Unknown threshold direction '" 
		      << m_thresholdDirectionProp.value()[iThr] 
		      << "'given" );
      return StatusCode::FAILURE;
    }

    auto itT=stringToTrigType.find(strToLower(m_triggersToExcludeProp.value()[iThr]));
    if (itT!=stringToTrigType.end()) {
      threshold.m_triggersToExclude.set(itT->second);
      if (itT->first=="all") {ATH_MSG_WARNING( "Setting TriggersToExclude to 'all' has no effect!" );}
    }
    else {
      ATH_MSG_ERROR( "Configuration problem. Unknown trigger type '" 
		      << m_triggersToExcludeProp.value()[iThr] 
		      << "' given in propety 'TriggersToExlude'" );
      return StatusCode::FAILURE;
    }

    itT=stringToTrigType.find(strToLower(m_triggersToIncludeProp.value()[iThr]));
    if (itT!=stringToTrigType.end()) {
      threshold.m_triggersToInclude.set(itT->second);
      if (itT->first=="none") {ATH_MSG_WARNING( "Setting TriggersToInclude to 'none' has no effect!" );}
    }
    else {
      ATH_MSG_ERROR( "Configuration problem. Unknown trigger type '" 
		      << m_triggersToIncludeProp.value()[iThr] 
		      << "' given in propety 'TriggersToInclude'" );
      return StatusCode::FAILURE;
    }

    threshold.m_doPercentageOccupancy=m_doPercentageOccupancyProp.value()[iThr];
    threshold.m_doEtaPhiOccupancy=m_doEtaPhiOccupancyProp.value()[iThr];
    threshold.m_doEtaOccupancy=m_doEtaOccupancyProp.value()[iThr];
    threshold.m_doPhiOccupancy=m_doPhiOccupancyProp.value()[iThr];
    threshold.m_doEtaPhiTotalEnergy=m_doEtaPhiTotalEnergyProp.value()[iThr];	  
    threshold.m_doEtaPhiAverageEnergy=m_doEtaPhiAverageEnergyProp.value()[iThr];	  
    threshold.m_doEtaPhiAverageQuality=m_doEtaPhiAverageQualityProp.value()[iThr];	  
    threshold.m_doEtaPhiFractionOverQth=m_doEtaPhiFractionOverQthProp.value()[iThr];	  
    threshold.m_qualityFactorThreshold=m_qualityFactorThresholdProp.value()[iThr];	  
    threshold.m_doEtaPhiAverageTime=m_doEtaPhiAverageTimeProp.value()[iThr];	  
    threshold.m_doEtaPhiFractionPastTth=m_doEtaPhiFractionPastTthProp.value()[iThr];	  
    threshold.m_timeThreshold=m_timeThresholdProp.value()[iThr];		  
    threshold.m_doBeamBackgroundRemoval=m_doBeamBackgroundRemovalProp.value()[iThr];
  }//end loop over threshold names

 
  //Overwrite per-layer thresholds:
  for (size_t iThrOvr=0;iThrOvr!= m_thresholdColumnType.value().size();++iThrOvr) {
    const std::string& nameToOverwrite=m_thresholdColumnType.value()[iThrOvr];
    auto it=std::find_if(m_thresholdHists.begin(),m_thresholdHists.end(),
			 [&](const thresholdHist_t& x) {return (x.m_threshName==nameToOverwrite);}
			 );
  
    if (it==m_thresholdHists.end()) {
      ATH_MSG_ERROR( "Configuration error reading 'ThresholdColumnType': Threshold type '" << nameToOverwrite << "' is not defined in 'ThresholdType'" );
      return StatusCode::FAILURE;
    }
     
     for (unsigned iLyr=0;iLyr<MAXLYRNS;++iLyr) {
       if (m_thresholdsProp[iLyr].value().size()<iThrOvr) {
	 ATH_MSG_ERROR( "Configuration error: Not enough values in threshold vector for layer " << iLyr );
	 return StatusCode::FAILURE;
       }  

      it->m_threshValue[iLyr]=m_thresholdsProp[iLyr].value()[iThrOvr];
    }
  }//end loop over threshold types with per-layer thresholds


  //Clean out thresholds with no histogram requested:
  auto thrIt=m_thresholdHists.begin();
  while (thrIt!=m_thresholdHists.end()) {
    const thresholdHist_t& thr=*thrIt;
    if (!(thr.m_doPercentageOccupancy || thr.m_doEtaPhiOccupancy || thr.m_doEtaOccupancy || thr.m_doPhiOccupancy || thr.m_doEtaPhiTotalEnergy ||
	  thr.m_doEtaPhiAverageEnergy || thr.m_doEtaPhiAverageQuality || 
	  thr.m_doEtaPhiFractionOverQth || thr.m_doEtaPhiAverageTime || thr.m_doEtaPhiFractionPastTth)) {
      ATH_MSG_INFO( "Config issue: Threshold type '" << thr.m_threshName << "' defined but no histograms requested. Deleting." );
      thrIt=m_thresholdHists.erase(thrIt);
    }
    else {
      ATH_MSG_INFO("Threshold histograms requested for threshold '" << thrIt->m_threshName << "'");
      thrIt++;
    }
  }


  //Fix up histogram titles
  for (thresholdHist_t& thr : m_thresholdHists) {
    if (thr.m_threshDirection!=NONE && 
	std::count(thr.m_threshTitleTemplate.begin(),thr.m_threshTitleTemplate.end(),'%')==1) {
      const size_t maxTitleLenght=thr.m_threshTitleTemplate.size()+32;
      std::unique_ptr<char[]> toBeFilled(new char[maxTitleLenght]);
	
      for (unsigned iLyrns=0;iLyrns<MAXLYRNS;++iLyrns) {
	//Attempt to fill in theshold value
	snprintf(toBeFilled.get(),maxTitleLenght,thr.m_threshTitleTemplate.c_str(),thr.m_threshValue[iLyrns]);
	toBeFilled[maxTitleLenght-1]='\0'; //To be absolutely sure...
	thr.m_threshTitles[iLyrns]=toBeFilled.get();
	//std::cout << "Fixup result:" << thr.m_threshTitles[iLyrns] << std::endl;
      } //end loop over thresholds
    } //and if somthing to fix up
    else {      
      //No threshold value to be set, keep as is 
      for (unsigned iLyrns=0;iLyrns<MAXLYRNS;++iLyrns) {
	  thr.m_threshTitles[iLyrns]=thr.m_threshTitleTemplate;
      }
    }
  }

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////
StatusCode LArCellMonTool::bookHistograms() {

  ATH_MSG_INFO("BookHistogram called");

  resetInternals();


  ATH_CHECK(bookLarMultThreHists());

  if(m_sporadic_switch) 
    ATH_CHECK(bookSporHists());

  ATH_CHECK(bookLarNonThreHists());
  
  return StatusCode::SUCCESS; 
}


StatusCode LArCellMonTool::bookSporHists(){

  std::stringstream dir;
  dir.precision(0);
  dir.setf(std::ios_base::fixed, std::ios_base::floatfield);
  dir << "/CaloMonitoring/LArCellMon_NoTrigSel/Sporadic_" <<  m_threshold_em_S0S1*1e-3 << "_"  << m_threshold_HECFCALEMS2S3*1e-3 << "GeV";
  m_sporadicDir=dir.str();
 
  for (unsigned i=0;i<MAXPARTITIONS;++i) {
    const std::string& partitionName=m_partitionNames[i];
    MonGroup mg(this,dir.str()+"/"+partitionName,run);
    std::string hName="NSporadic_"+partitionName;
    std::string hTitle="Number of sporadic-cell plots in "+partitionName+ " with CSC veto";
    m_h_sporadicHists[i]=new TH1F(hName.c_str(),hTitle.c_str(),10,0,10);
    ATH_CHECK(mg.regHist(m_h_sporadicHists[i]));
  }

  return StatusCode::SUCCESS;
}



StatusCode LArCellMonTool::bookLarNonThreHists(){

  //Summary histograms, mostly from the base-class  
  MonGroup summaryGroup(this,m_lArPath+"Summary",run);
  m_h_n_trigEvent = new TH1F("nEvtsByTrigger","Total Events: bin 0, RNDM Trigger: 1, Calo Trigger: 2, MinBias Trigger: 3, MET Trigger: 4, Misc Trigger: 5, Events Selected for Noise Plots: 6 ",7,1.,8.);
  summaryGroup.regHist( m_h_n_trigEvent ).ignore();
  ATH_CHECK(bookBaseHists(&summaryGroup)); //from base class


  //Book non-threshold energy histograms:
  std::array<double,55> logEnergyBins;
  if (m_useLogarithmicEnergyBinning) {
    const static double step = std::pow(10,0.1);
    logEnergyBins[0]=10;
    for(size_t i = 1; i<logEnergyBins.size(); i++){  //Cover full range of energy specification, 10 MeV to 3 TeV
      logEnergyBins[i] = step * logEnergyBins[i-1];
    }
  }//end if logarithimcEnergyBinning
    
  //Energy bining in case for cosmics:
  std::array<std::pair<float,float>,MAXPARTITIONS > cosmicEneBinning;
  cosmicEneBinning[EMBA]=cosmicEneBinning[EMBC]={-318.5,1088.5};   //bin size 7 MeV
  cosmicEneBinning[EMECA]=cosmicEneBinning[EMECC]={-591.5,2021.5}; //bin size 13 MeV
  cosmicEneBinning[HECA]=cosmicEneBinning[HECC]={-2047.5,6997.5};  //bin size 45 MeV
  cosmicEneBinning[FCALA]=cosmicEneBinning[FCALC]={-1865.5,6375.5}; //bin size 41 MeV
  

  if (m_doUnnormalized1DEnergy) {
    MonGroup mg(this,m_lArPath+"/Energy", run);
    
    for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
      const std::string histName=std::string("CellEnergy_")+m_layerNames[ilyr];
      const std::string histTitle=std::string("Cell Energy in ") +m_layerNames[ilyr]+" with CSC veto";
      if (m_useLogarithmicEnergyBinning) {
	m_h_energy[ilyr] = new TH1F(histName.c_str(),histTitle.c_str(),54,logEnergyBins.data());
	m_h_energy[ilyr]->GetYaxis()->SetTitle("Cell Events");
	m_h_energy[ilyr]->GetYaxis()->SetTitleOffset(2);
      }
      else {//no logarithmic energy binning 
	if(m_dataType == AthenaMonManager::cosmics) {
	  m_h_energy[ilyr] = new TH1F(histName.c_str(),histTitle.c_str(),201,cosmicEneBinning[ilyr].first,cosmicEneBinning[ilyr].second);
	}
	else { //collisions, no cosmics	  
	  m_h_energy[ilyr] = new TH1F(histName.c_str(),histTitle.c_str(),401,-1912.5,28162.5); // bin size 75 MeV 
	}
	
	std::stringstream ytitle;
	ytitle.precision(0);
	ytitle << "Cell Events/" << m_h_energy[ilyr]->GetYaxis()->GetBinWidth(1) << "MeV";
	m_h_energy[ilyr]->GetYaxis()->SetTitle(ytitle.str().c_str());
	m_h_energy[ilyr]->GetYaxis()->SetTitleOffset(2);

      }
      m_h_energy[ilyr]->GetXaxis()->SetTitle("Cell Energy (MeV)");
      m_h_energy[ilyr]->GetXaxis()->SetLabelSize(0.02);
      m_h_energy[ilyr]->GetXaxis()->SetLabelOffset(0.01);
      m_h_energy[ilyr]->GetXaxis()->SetTitleSize(0.03);
      m_h_energy[ilyr]->GetXaxis()->SetTitleOffset(1.5);
      mg.regHist(m_h_energy[ilyr]).ignore();
    } //end loop over layers
  } // end if  m_doUnnormalized1DEnergy

  if (m_doEnergyVsTime){
    MonGroup mg(this,m_lArPath+"/EnergyVsTime", run);
    const int nbintime = 92;
    const double timescale[93] = {-200,-195,-190,-185,-180,-175,-170,-165,-160,-155,-150,-145,-140,-135,-130,-125,-120,-115,-110,-105,-100,-95,-90,-85,-80,-75,-70,-65,-60,-55,-50,-45,-40,-35,-30,-25,-20,-15,-10,-8,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200};
    const int nbinenergy = 11;
    const double energyscale[12] = {250,500,1000,2500,5000,10000,25000,50000,100000,250000,500000,1000000};
    for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
      const std::string histName=std::string("CellEnergyVsTime_")+m_layerNames[ilyr];
      const std::string histTitle=std::string("Cell Energy vs Cell Time in ") +m_layerNames[ilyr]+" with CSC veto";
      m_h_energyVsTime[ilyr] = new TH2F(histName.c_str(),histTitle.c_str(),nbintime,timescale,nbinenergy,energyscale);
      m_h_energyVsTime[ilyr]->GetXaxis()->SetTitle("Cell Time (ns)");
      m_h_energyVsTime[ilyr]->GetYaxis()->SetTitle("Energy (MeV)");
      mg.regHist(m_h_energyVsTime[ilyr]).ignore();

      const int eThr=m_eCutForTiming[ilyr/2];
      std::stringstream shistName;
      shistName << "CellEnergyVsTime_" << m_layerNames[ilyr] << "_" << eThr;
      std::stringstream shistTitle;
      shistTitle << m_layerNames[ilyr] << " - Cell Time (E>" << eThr << " MeV) with CSC veto";
      m_h_energyVsTime_DifferThresholds[ilyr] = new TH1F(shistName.str().c_str(),shistTitle.str().c_str(),nbintime,timescale);
      m_h_energyVsTime_DifferThresholds[ilyr]->GetXaxis()->SetTitle("Cell Time (ns)");
      m_h_energyVsTime_DifferThresholds[ilyr]->GetYaxis()->SetTitle("Energy (MeV)");
      mg.regHist(m_h_energyVsTime_DifferThresholds[ilyr]).ignore();
    } //end loop over layers
  }//end if (m_doEnergyVsTime

  return StatusCode::SUCCESS;
}
 

void LArCellMonTool::checkTriggerAndBeamBackground() {

  m_h_n_trigEvent->Fill(0.5);

  if (m_useTrigger) {
    std::bitset<MAXTRIGTYPE> triggersPassed(0x1<<NOTA); //Last bit: NOTA, always passes
    constexpr std::bitset<MAXTRIGTYPE> NOTAmask=~(0x1<<NOTA);
    for (unsigned i=0;i<m_chainGroups.size();++i) {
      if (m_chainGroups[i] && m_chainGroups[i]->isPassed()) triggersPassed.set(i);
    }
  
    for (unsigned i=0;i<NOTA;++i) {
      if (triggersPassed.test(i)) m_h_n_trigEvent->Fill(0.5+i);  
    }

    for (thresholdHist_t& thr : m_thresholdHists) { //Loop over thresholds
      thr.m_threshTriggerDecision=(thr.m_triggersToInclude & triggersPassed).any() && (thr.m_triggersToExclude & triggersPassed & NOTAmask).none();
    }// end loop over thresholds
  } //end if trigger used
  else {
    m_h_n_trigEvent->Fill(6.5); 
  }
  //Note that thr.m_threshTriggerDecision remains in it's default state 'true' if trigger wasn't used

  //Check beam-background removal 
  for (thresholdHist_t& thr : m_thresholdHists) { //Loop over thresholds     
    if (thr.m_threshTriggerDecision && (m_passBeamBackgroundRemoval || !thr.m_doBeamBackgroundRemoval)) {
      //The counter of events passing. Will be incremented even if neither trigger nor background removal is requested for this threshold
      thr.m_eventsPassed++; 
    }
  }//end loop over thresholds
  return;
} 


void LArCellMonTool::sporadicNoiseCandidate(const CaloCell* cell, const LArCellMonTool::LayerEnum iLyr, const float threshold, const LArOnOffIdMapping* cabling) {
  
  const Identifier id=cell->ID();
  const PartitionEnum part=m_layerEnumtoPartitionEnum[iLyr];  
  ATH_MSG_INFO( "Found sporadic noise candidate cell with id 0x" << std::hex << id.get_identifier32().get_compact() << std::dec << " in " << m_partitionNames[part] );
 
  SporadicNoiseCell_t& snc=m_sporadicNoiseCells[id];
  snc.m_counter++;
  //Note wrt AthenaMP: Counting noisy event is obviously flawed, even for 
  //serial processing as events are scattered over many files
  //The sporadic-noise histograms here are only an indication, not reproducible

  if (snc.m_counter==m_minSporadicNoiseEventsPerCell && m_counter_sporadic_protc<m_sporadic_protc &&  m_sporadicPerPartCounter[part] < m_sporadicPlotLimit) {
    ++m_sporadicPerPartCounter[part];
    ++m_counter_sporadic_protc;
    m_h_sporadicHists[part]->Fill(1.0); //FIXME Sounds like nonsense but used in webdisplay config      
    bookNoisyCellHistos(snc,cell->caloDDE(),part,threshold, cabling);
  }// end if reached m_minSporadicNoiseEventsPerCell and  < m_sporadic_protection

  if (snc.m_h_energy) {
    const float energy=cell->energy();
    const float quality=cell->quality();
    snc.m_h_energy->Fill(energy);
    snc.m_h_quality->Fill(quality);
    snc.m_h_energyVsLB->Fill(m_lb,energy);
  }

  return;
}

void LArCellMonTool::bookNoisyCellHistos(SporadicNoiseCell_t& result, const CaloDetDescrElement* dde, 
					 const PartitionEnum part, const float threshold,
                                         const LArOnOffIdMapping* cabling) {

  const HWIdentifier onlID=cabling->createSignalChannelID(dde->identify());
  const int ft = m_LArOnlineIDHelper->feedthrough(onlID);
  const int slot = m_LArOnlineIDHelper->slot(onlID);
  const int channel = m_LArOnlineIDHelper->channel(onlID);


  std::stringstream hName;
  hName.precision(2);
  hName <<  m_partitionNames[part] << "_FT"<<ft<<"Sl"<<slot<<"Ch"<<channel<<"_(Phi"<<dde->phi()<<"_Eta"<<dde->eta()<<"_sampling" << dde->getLayer() << ")";

  std::stringstream titleSuffix;
  titleSuffix.precision(0);
  titleSuffix << "::" << threshold*1e-3 <<"GeV with CSC veto";
  
  const std::string dir=m_sporadicDir+"/"+m_partitionNames[part];

  //Energy Histogram
  result.m_h_energy=new TH1F((hName.str()+"_EN").c_str(),
			     (hName.str()+"_EN"+titleSuffix.str()).c_str(),
			     75,-10000,140000); //bins of 2GeV
  result.m_h_energy->GetXaxis()->SetTitle("MeV");
  result.m_h_energy->GetYaxis()->SetTitle("Number of Events");

  regHist(result.m_h_energy,dir,run).ignore();
  

  //Quality Histogram (Q: is this actually needed?
   result.m_h_quality=new TH1F((hName.str()+"_Quality").c_str(),
			     (hName.str()+"_Quality"+titleSuffix.str()).c_str(),
			     75,-10000,140000); //bins of 2GeV
  result.m_h_quality->GetXaxis()->SetTitle("Quality Factor");
  result.m_h_quality->GetYaxis()->SetTitle("Number of Events");
  
  regHist(result.m_h_quality,dir,run).ignore();

  //E vs LB
  result.m_h_energyVsLB=new TProfile((hName.str()+"_ENLB").c_str(),
				     (hName.str()+"_LB"+titleSuffix.str()).c_str(),
				     1400,0.5,1400.5);
 result.m_h_energyVsLB->GetXaxis()->SetTitle("LumiBlocks");
 result.m_h_energyVsLB->GetYaxis()->SetTitle("MeV");

 regHist(result.m_h_energyVsLB,dir,run,ATTRIB_X_VS_LB,"","merge").ignore();

 return;
}



////////////////////////////////////////////////////////////////////////////
StatusCode LArCellMonTool::fillHistograms(){  

  ATH_MSG_DEBUG("LArCellMonTool::fillHistograms() starts");

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling");
     return StatusCode::FAILURE;
  }

  SG::ReadHandle<CaloCellContainer> cellContHandle{m_cellContainerName};
  if (! cellContHandle.isValid()) { return StatusCode::FAILURE; }
  const CaloCellContainer* cellCont = cellContHandle.get();
	    
  if (!m_oncePerJobHistosDone) {
    ATH_CHECK(createPerJobHistograms(cellCont));
    m_oncePerJobHistosDone=true;
  }

  bool ifPass = true;
  ATH_CHECK(checkFilters(ifPass)); //Check ATLAS-Ready, beam-background, etc from base class
  if(!ifPass) return StatusCode::SUCCESS;

  checkTriggerAndBeamBackground();

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 
  // loop over cells -------------
  for ( ; it!=it_e;++it) {
    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    bool is_lar=m_calo_id->is_lar(id);
    
    if(!is_lar) continue;

    const CaloDetDescrElement* caloDDEl=cell->caloDDE();
    const IdentifierHash cellHash=caloDDEl->calo_hash();

    const CaloGain::CaloGain  gain= cell->gain();
    const float cellen = cell->energy();
    const float celltime = cell->time();
    const uint16_t cellquality = cell->quality();
    const uint16_t cellprovenance = cell->provenance();
    const bool celltqavailable = ( cellprovenance & 0x2000 );
    
    // No more filling if we encounter a bad channel ....
    if (m_maskKnownBadChannels && m_badChannelMask->cellShouldBeMasked(id,gain)) continue;

    // ...or a channel w/o conditions
    if (m_maskNoCondChannels && (cellprovenance & 0x00FF) != 0x00A5) continue; //FIXME, I think that cut is wrong

    float celleta, cellphi;
    unsigned iLyr, iLyrNS;
    getHistoCoordinates(caloDDEl, celleta, cellphi, iLyr, iLyrNS);
 
    //Start filling per-threshold histograms:
    for (auto& thr :  m_thresholdHists) {
      //std::cout << "Threshold name " << thr.m_threshName << std::endl;
      //Any of the conditons below means we do not fill the histogram:
    
      //Trigger passed?
      if (m_useTrigger && !thr.m_threshTriggerDecision) continue;
      //std::cout << " Trigger passed" << std::endl;

      //Beam background event?
      if (thr.m_doBeamBackgroundRemoval && !m_passBeamBackgroundRemoval) continue;

      //std::cout << " Beam background passed" << std::endl;

      float thresholdVal=thr.m_threshValue[iLyrNS];
      if (thresholdVal==0) {
	ATH_MSG_WARNING("Got threshold 0 for type '" << thr.m_threshName  << "'for cell in layer " << m_layerNames[iLyr]);
      }

      if (thr.m_inSigNoise) thresholdVal*=m_noiseTool->getNoise(cell,m_noiseType);
        
      if (thr.m_threshDirection==OVER && cellen <= thresholdVal) continue;
      if (thr.m_threshDirection==UNDER && cellen > thresholdVal) continue;
      if (thr.m_threshDirection==BOTH && (cellen > -thresholdVal && cellen <= thresholdVal)) continue;   
      //std::cout << "Threshold '"<< thr.m_threshName << "' passed" << std::endl;

      (thr.m_occupancyCounter[cellHash])++; 

      if (thr.m_h_totalEnergy_etaphi[iLyr]) thr.m_h_totalEnergy_etaphi[iLyr]->Fill(celleta,cellphi,cellen); 

      if(thr.m_h_poorQualityOccupancy_etaphi[iLyr] && (cellquality > thr.m_qualityFactorThreshold)) {
	thr.m_h_poorQualityOccupancy_etaphi[iLyr]->Fill(celleta,cellphi);
      }
      if (thr.m_h_totalQuality_etaphi[iLyr]) thr.m_h_totalQuality_etaphi[iLyr]->Fill(celleta,cellphi,cellquality);
      

      if(thr.m_h_totalTime_etaphi[iLyr]) {
	thr.m_h_totalTime_etaphi[iLyr]->Fill(celleta,cellphi,celltime);
      }
    
      if (thr.m_h_poorTimeOccupancy_etaphi[iLyr] && (fabs(celltime) > thr.m_timeThreshold)) {
	thr.m_h_poorTimeOccupancy_etaphi[iLyr]->Fill(celleta,cellphi);
      }
    
    }//end loop over thresholds

    if(m_passBeamBackgroundRemoval) {
      // 1D Energy distribution:
      if (m_h_energy[iLyr]) m_h_energy[iLyr]->Fill(cellen); 

      // Time vs Energy:
      if (m_doEnergyVsTime && celltqavailable && cellquality<4000) {
	m_h_energyVsTime[iLyr]->Fill(celltime,cellen); 
	if (cellen>m_eCutForTiming[iLyrNS]) {
	  m_h_energyVsTime_DifferThresholds[iLyr]->Fill(celltime,cellen);
	}
      }
     
      //Checking for signs of sporadic noise 
      if(m_sporadic_switch) {
	float energyThreshold;
	//We have two energy thresholds for sporadic noise, one for EM samling 1 and 2 and one for the rest
	if(iLyrNS==EMB1NS || iLyrNS==EMB2NS || iLyrNS==EMEC1NS || iLyrNS==EMEC2NS) {
	  energyThreshold=m_threshold_em_S0S1;
	}
	else {
	  energyThreshold = m_threshold_HECFCALEMS2S3;
	}
	if (cell->energy()> energyThreshold) {
	  sporadicNoiseCandidate(cell,(LayerEnum)iLyr,energyThreshold, cabling);
	}  
      }//end if m_sporadic_switch
    } // end if m_passBeamBackgroundRemoval
  }//end loop over cells

  // ATH_MSG_INFO("LArCellMonTool::fillLarHists() is done");
  return StatusCode::SUCCESS;
}

StatusCode LArCellMonTool::procHistograms() {

  ATH_MSG_DEBUG("procHistogram called newEB/newLB/newRun=" <<  newEventsBlockFlag() << "/" << newLumiBlockFlag() << "/" << newRunFlag());

  //Loop over thresholds
  for (thresholdHist_t& thr : m_thresholdHists) {
    ATH_MSG_INFO("processHist for threshold '" << thr.m_threshName << "', nb of events passed Trigger and Background removal: " << thr.m_eventsPassed);
    if (thr.m_eventsPassed>0 && m_oncePerJobHistosDone) {
      //fill the 1D and 2D occupancy histograms based on the counter of cells above threshold
      ATH_CHECK(fillOccupancyHist(thr));

      for (size_t iLyr=0;iLyr<MAXLAYER;++iLyr) {

	//Re-scale 1D occupancy histogram for the number of cells in each bin.
	//For 1D histograms, we are working on the same histogram, no temporary histogram used
	if (thr.m_h_occupancy_eta[iLyr]) {
	  assert(m_h_nActiveCellsFirstEvent_eta[iLyr]);
	  thr.m_h_occupancy_eta[iLyr]->Divide(m_h_nActiveCellsFirstEvent_eta[iLyr]); 
	  ATH_MSG_DEBUG("Adjusting number of entries in eta occupancy histogram from " << thr.m_h_occupancy_eta[iLyr]->GetEntries() << " to " << thr.m_eventsPassed);
	  thr.m_h_occupancy_eta[iLyr]->SetEntries(thr.m_eventsPassed);
	  if (thr.m_doPercentageOccupancy) thr.m_h_occupancy_eta[iLyr]->Scale(100.0/thr.m_eventsPassed); //Convert to percentage if required
	}

	if (thr.m_h_occupancy_phi[iLyr]) {
	  assert(m_h_nActiveCellsFirstEvent_phi[iLyr]);
	  thr.m_h_occupancy_phi[iLyr]->Divide(m_h_nActiveCellsFirstEvent_phi[iLyr]);
	  ATH_MSG_DEBUG("Adjusting number of entries from in phi occupancy histogram" << thr.m_h_occupancy_phi[iLyr]->GetEntries() << " to " << thr.m_eventsPassed);
	  if (thr.m_doPercentageOccupancy) thr.m_h_occupancy_phi[iLyr]->Scale(100.0/thr.m_eventsPassed); //Convert to percentage if required
	  thr.m_h_occupancy_phi[iLyr]->SetEntries(thr.m_eventsPassed);
	}
	
	
	if (thr.m_h_averageEnergy_etaphi[iLyr]) {
	  assert(thr.m_h_totalEnergy_etaphi[iLyr]);
	  assert(thr.m_h_occupancy_etaphi[iLyr]);
	  divideByOccupancy(thr.m_h_averageEnergy_etaphi[iLyr],thr.m_h_totalEnergy_etaphi[iLyr],thr.m_h_occupancy_etaphi[iLyr]);
	  thr.m_h_averageEnergy_etaphi[iLyr]->SetEntries(thr.m_h_totalEnergy_etaphi[iLyr]->GetEntries());
	}//end if m_doAverageEnergy

	if (thr.m_h_averageTime_etaphi[iLyr]) {
	  assert(thr.m_h_totalTime_etaphi[iLyr]);
	  assert(thr.m_h_occupancy_etaphi[iLyr]);
	  divideByOccupancy(thr.m_h_averageTime_etaphi[iLyr],thr.m_h_totalTime_etaphi[iLyr],thr.m_h_occupancy_etaphi[iLyr]);
	  thr.m_h_averageTime_etaphi[iLyr]->SetEntries(thr.m_h_totalTime_etaphi[iLyr]->GetEntries());
	}//end if m_doEtaPhiAverageTime

	if (thr.m_doEtaPhiFractionPastTth) {
	  assert(thr.m_h_poorTimeOccupancy_etaphi[iLyr]);
	  assert(thr.m_h_fractionPastTth_etaphi[iLyr]);
	  divideByOccupancy(thr.m_h_fractionPastTth_etaphi[iLyr],thr.m_h_poorTimeOccupancy_etaphi[iLyr],thr.m_h_occupancy_etaphi[iLyr]);
	  thr.m_h_fractionPastTth_etaphi[iLyr]->SetEntries(thr.m_h_poorTimeOccupancy_etaphi[iLyr]->GetEntries());
	}//end if m_h_fractionPastTth_etaphi

	if (thr.m_h_averageQuality_etaphi[iLyr]) {
	  assert(thr.m_h_totalQuality_etaphi[iLyr]);
	  assert(thr.m_h_occupancy_etaphi[iLyr]);
	  divideByOccupancy(thr.m_h_averageQuality_etaphi[iLyr],thr.m_h_totalQuality_etaphi[iLyr],thr.m_h_occupancy_etaphi[iLyr]);
	  thr.m_h_averageQuality_etaphi[iLyr]->SetEntries(thr.m_h_totalQuality_etaphi[iLyr]->GetEntries());
	}//end if m_doEtaPhiAverageQuality

	if (thr.m_doEtaPhiFractionOverQth) {
	  assert(thr.m_h_poorQualityOccupancy_etaphi[iLyr]);
	  assert(thr.m_h_fractionOverQth_etaphi[iLyr]);
	  divideByOccupancy(thr.m_h_fractionOverQth_etaphi[iLyr],thr.m_h_poorQualityOccupancy_etaphi[iLyr],thr.m_h_occupancy_etaphi[iLyr]);
	  thr.m_h_fractionOverQth_etaphi[iLyr]->SetEntries(thr.m_h_poorQualityOccupancy_etaphi[iLyr]->GetEntries());
	}//end if m_h_fractionPastQth_etaphi


	//Convert 2D occupancy histogram to percentage if required.
	if (thr.m_h_occupancy_etaphi[iLyr] && thr.m_doPercentageOccupancy) {
	  thr.m_h_occupancy_phi[iLyr]->Scale(100.0/thr.m_eventsPassed); //Convert to percentage if required
	}

      }//end loop over layers
    }//end if at least one event seen
  }//end loop over thresholds
  

  if (newLumiBlockFlag()) {
    ATH_MSG_INFO("New Lumi-Block. Resetting internal counters");
    resetInternals();
  }

  return StatusCode::SUCCESS;
 }



void  LArCellMonTool::regTempHist(TH1* h, MonGroup& mg) {
  if (m_doSaveTempHists) {
    mg.regHist(h).ignore();
  }
  else {
    h->SetDirectory(nullptr); //That means we manage the memory ourselves
  }
}


//////////////////////////////////////////////////////////////////
StatusCode LArCellMonTool::finalize() {

  if (m_sporadic_switch && msgLvl(MSG::INFO)) {
    ATH_MSG_INFO( "Total number of single-cell sporadic-noise histograms produced: " 
		   <<  m_counter_sporadic_protc  << " (max " << m_sporadic_protc << ")" );
    for (size_t part=0;part<MAXPARTITIONS;++part) {
      ATH_MSG_INFO( "Number of single-cell sporadic-noise histograms produced for " <<  m_layerNames[part] 
		     << ": " <<  m_sporadicPerPartCounter[part] << " (max " << m_sporadicPlotLimit << ")" );
    }
  }

  //Delete temporary histograms (if needed)
  if (!m_doSaveTempHists) { //We mange the temporary histograms ourselves
    for (size_t iLyr=0;iLyr<MAXLAYER;++iLyr) {
      delete m_h_nActiveCellsFirstEvent_eta[iLyr]; m_h_nActiveCellsFirstEvent_eta[iLyr]=nullptr; 
      delete m_h_nActiveCellsFirstEvent_phi[iLyr]; m_h_nActiveCellsFirstEvent_phi[iLyr]=nullptr; 
    }

    for (auto& thr : m_thresholdHists) {
      for (size_t iLyr=0;iLyr<MAXLAYER;++iLyr) {
	for (TH1* h : {thr.m_h_totalTime_etaphi[iLyr], thr.m_h_poorTimeOccupancy_etaphi[iLyr],
	      thr.m_h_poorQualityOccupancy_etaphi[iLyr],thr.m_h_totalQuality_etaphi[iLyr]}) {
	  delete h;h=nullptr;
	}//end loop over histograms
	if (!thr.m_doEtaPhiOccupancy && thr.m_h_occupancy_etaphi[iLyr]) {  //this one may be managed by underlying root
	  delete thr.m_h_occupancy_etaphi[iLyr]; thr.m_h_occupancy_etaphi[iLyr]=nullptr;
	}
      }//end loop over layers
    }//end loop over thresholds
  }//end if not saving temp hists
    
  return StatusCode::SUCCESS;
}



TH2F* LArCellMonTool::newEtaPhiHist(const std::string& hName, const std::string& hTitle, const CaloMonitoring::LArCellBinning& binning) const {
 
  TH2F* result=new TH2F(hName.c_str(),hTitle.c_str(),
			binning.getNTotEtaBins(),binning.getEtaBinArray(),
			binning.getNTotPhiBins(),binning.getPhiBinArray());

  result->GetXaxis()->SetTitle("#eta");
  result->GetYaxis()->SetTitle("#phi");
  return result;
}



StatusCode LArCellMonTool::createPerJobHistograms(const CaloCellContainer* cellCont) {

  ATH_MSG_INFO("Creating the once-per-job histograms");
  //The following histograms can be considered constants for one job
  //(in fact, they are constant for an entire run or even run-periode)
  //ActiveCells in eta/phi (to normalize 1D occupancy plots)
  //BadChannel word
  //Database noise
  
  
  //1. Bad Channel word
  if (m_doKnownBadChannelsVsEtaPhi) {
    MonGroup knownBadChannelGroup(this,m_lArPath+"KnownBadChannels", run, ATTRIB_MANAGED,"","weightedAverage"); //Merging this info makes no sense at all! 
    for (unsigned iLyr=0;iLyr<MAXLAYER;++iLyr) {
      const CaloMonitoring::LArCellBinning& binning=m_binning[iLyr];
      std::string histName=std::string("KnownBadChannelsVsEtaPhi_") + m_layerNames[iLyr];
      std::string histTitle=std::string("Map of known bad channels in (#eta,#phi) for ") + m_layerNames[iLyr];
      m_h_badChannels_etaphi[iLyr]=new TH2F(histName.c_str(),histTitle.c_str(),
					    binning.getNTotEtaBins(),binning.getEtaBinArray(),
					    binning.getNTotPhiBins(),binning.getPhiBinArray());
				
      m_h_badChannels_etaphi[iLyr]->GetXaxis()->SetTitle("#eta");
      m_h_badChannels_etaphi[iLyr]->GetYaxis()->SetTitle("#phi");	
      ATH_CHECK(knownBadChannelGroup.regHist(m_h_badChannels_etaphi[iLyr]));    
    }//end loop over layers
  }// end if m_doKnownBadChannelsVsEtaPhi

  //2. Database noise
  if (m_doDatabaseNoiseVsEtaPhi) {
    MonGroup  mongroup(this,m_lArPath+"DatabaseNoise",run, ATTRIB_MANAGED,"","weightedAverage"); 
    for (unsigned iLyr=0;iLyr<MAXLAYER;++iLyr) {
      const CaloMonitoring::LArCellBinning& binning=m_binning[iLyr];
      std::string histName=std::string("DatabaseNoiseVsEtaPhi_") + m_layerNames[iLyr];
      std::string histTitle=std::string("Map of Noise Values from the Database vs (#eta,#phi) for ") + m_layerNames[iLyr];
      m_h_dbnoise_etaphi[iLyr]=new TH2F(histName.c_str(),histTitle.c_str(),
					binning.getNTotEtaBins(),binning.getEtaBinArray(),
					binning.getNTotPhiBins(),binning.getPhiBinArray());
				
      m_h_dbnoise_etaphi[iLyr]->GetXaxis()->SetTitle("#eta");
      m_h_dbnoise_etaphi[iLyr]->GetYaxis()->SetTitle("#phi");	
      ATH_CHECK(mongroup.regHist(m_h_dbnoise_etaphi[iLyr]));
    }//end loop over layers
  }//end if m_doDatabaseNoiseVsEtaPhi
    
    
  //3.1 Number of active cells in eta to normalize 1D eta occupancy plot
  MonGroup  mongroup(this,m_lArPath+"ActiveCells",run, ATTRIB_MANAGED); 
  for (const auto& thr :  m_thresholdHists) {  
    if (thr.m_doEtaOccupancy) {
      ATH_MSG_DEBUG("Booking NCellsActiveVsEta temp histogram");
      for (unsigned iLyr=0;iLyr<MAXLAYER;++iLyr) {
	const CaloMonitoring::LArCellBinning& binning=m_binning[iLyr];
	const std::string histName=std::string("NCellsActiveVsEta_") +  m_layerNames[iLyr];
	const std::string histTitle=std::string("No. of Active Cells in #eta for ") +  m_layerNames[iLyr];
	m_h_nActiveCellsFirstEvent_eta[iLyr]=new TH1F(histName.c_str(),histTitle.c_str(),binning.getNTotEtaBins(),binning.getEtaBinArray());
	regTempHist(m_h_nActiveCellsFirstEvent_eta[iLyr], mongroup);
      }//end loop over layers
      break; //break loop over thresholds, one such histo is enough
    } //end if thr.m_doEtaOccupancy
  }//end loop over thresholds

  //3.2 Number of active cells in phi to normalize 1D phi occupancy plot
  for (const auto& thr :  m_thresholdHists) {  
    if (thr.m_doPhiOccupancy) {
      ATH_MSG_DEBUG("Booking NCellsActiveVsPhi temp histogram");
      for (unsigned iLyr=0;iLyr<MAXLAYER;++iLyr) {
	const CaloMonitoring::LArCellBinning& binning=m_binning[iLyr];
	const std::string histName=std::string("NCellsActiveVsPhi_") +  m_layerNames[iLyr];
	const std::string histTitle=std::string("No. of Active Cells in #phi for ") +  m_layerNames[iLyr];
	m_h_nActiveCellsFirstEvent_phi[iLyr]=new TH1F(histName.c_str(),histTitle.c_str(),binning.getNTotPhiBins(),binning.getPhiBinArray());
	regTempHist(m_h_nActiveCellsFirstEvent_phi[iLyr], mongroup);
      }//end loop over layers
      break; //break loop over thresholds, one such histo is enough
    } //end if thr.m_doPhiOccupancy
  }//end loop over thresholds
  

  SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
  const LArBadChannelCont *bcCont {*readHandle};
  if(m_doKnownBadChannelsVsEtaPhi && !bcCont) {
     ATH_MSG_WARNING( "Do not have Bad chan container !!!" );
     return StatusCode::FAILURE;
  }
  
  //filling:
  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 
  for ( ; it!=it_e;++it) { 
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    bool is_lar=m_calo_id->is_lar(id);
    if(!is_lar) continue;
    const CaloDetDescrElement* caloDDEl=cell->caloDDE();
    float celleta, cellphi;
    unsigned iLyr, iLyrNS;
    getHistoCoordinates(caloDDEl, celleta, cellphi, iLyr, iLyrNS);

    if (m_doKnownBadChannelsVsEtaPhi) {
      const LArBadChannel larBadChannel = bcCont->offlineStatus(id);
      if (!larBadChannel.good()) {
	const uint32_t badCellWord = larBadChannel.packedData();
	m_h_badChannels_etaphi[iLyr]->Fill(celleta,cellphi,badCellWord);
      }
    } 
  
    if ( m_doDatabaseNoiseVsEtaPhi ) {
      const float cellnoisedb = m_noiseTool->getNoise(cell,m_noiseType);
      m_h_dbnoise_etaphi[iLyr]->Fill(celleta,cellphi,cellnoisedb);
    }      
    
    // 1D reference plots of active cells versus eta and phi: (needed for nomalization)
    if ( m_h_nActiveCellsFirstEvent_eta[iLyr] ) {
      m_h_nActiveCellsFirstEvent_eta[iLyr]->Fill(celleta);
    }
    if (m_h_nActiveCellsFirstEvent_phi[iLyr] ) {
      m_h_nActiveCellsFirstEvent_phi[iLyr]->Fill(cellphi);
    }
    
  }//end loop over cells

  return StatusCode::SUCCESS;
}

StatusCode LArCellMonTool::bookLarMultThreHists() {

  const std::string percentMerging(m_useWeightedEffMerging ? "weightedEff" : "weightedAverage");

  MonGroup monGroup2DOcc(this,m_lArPath+"2d_Occupancy", run, ATTRIB_MANAGED); 
  MonGroup monGroup1DOcc(this,m_lArPath+"1d_Occupancy", run, ATTRIB_MANAGED,"",percentMerging); 
  MonGroup monGroupTotalE(this,m_lArPath+"2d_TotalEnergy", run, ATTRIB_MANAGED); 
  MonGroup monGroupAvgE(this,m_lArPath+"2d_AvgEnergy", run, ATTRIB_MANAGED,"","weightedAverage"); 
  MonGroup monGroupAvgT(this,m_lArPath+"2d_AvgTime", run, ATTRIB_MANAGED,"","weightedAverage"); 
  MonGroup monGroupOutOfTime(this,m_lArPath+"2d_FractionOutOfTime", run, ATTRIB_MANAGED,"","weightedAverage");
  MonGroup monGroupAvgQ(this,m_lArPath+"2d_AvgQuality", run, ATTRIB_MANAGED,"","weightedAverage"); 
  MonGroup monGroupPoorQ(this,m_lArPath+"2d_PoorQualityFraction", run, ATTRIB_MANAGED,"","weightedAverage"); 

  MonGroup monGroup2DPercentOcc(this,m_lArPath+"2d_Occupancy",run,ATTRIB_MANAGED,"",percentMerging);

  MonGroup monGroupTemp(this,m_lArPath+"Temp",run);

  for (auto& thr :  m_thresholdHists) {

    const std::string cscVetoTitle(thr.m_doBeamBackgroundRemoval ? " with CSC veto" : " w/o CSC veto");
    const std::string cscVetoName(thr.m_doBeamBackgroundRemoval ? "_CSCveto" : "");

    for (unsigned iLyr=0;iLyr<MAXLAYER;++iLyr) {
      const unsigned iLyrNS=iLyr/2;
      const CaloMonitoring::LArCellBinning& binning=m_binning[iLyr];
   
      //The 2D occupancy plot may be only necessary to normalize the average plots, in this case it will be temp hist
      if (thr.m_doEtaPhiOccupancy || thr.m_doEtaPhiAverageEnergy || thr.m_doEtaPhiAverageTime || thr.m_doEtaPhiFractionPastTth) {
	ATH_MSG_DEBUG("Booking 2D occupancy histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	if (thr.m_doPercentageOccupancy) {
	  //Name it 'percentage' if required (will be converted at the end of procHist)
	  thr.m_h_occupancy_etaphi[iLyr]=newEtaPhiHist(std::string("PercentCellOccupancyVsEtaPhi_")+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
						       "Percentage of events in (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
						       binning);
	}
	else {//no percentage requested
	  thr.m_h_occupancy_etaphi[iLyr]=newEtaPhiHist(std::string("CellOccupancyVsEtaPhi_")+m_layerNames[iLyr]+"_"+thr.m_threshName,
						       "No. of events in (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
						       binning);
	}
	if (thr.m_doEtaPhiOccupancy) //Asked to store this histogram?
	  ATH_CHECK(monGroup2DOcc.regHist(thr.m_h_occupancy_etaphi[iLyr])); //->register histogram
	else
	  regTempHist(thr.m_h_occupancy_etaphi[iLyr],monGroupTemp); 
       
      }//end if need 2D occupancy histogram
    
    
      //1D eta or phi occupacies
      if (thr.m_doEtaOccupancy) {
	ATH_MSG_DEBUG("Booking eta occupancy histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	std::string histName=std::string("CellOccupancyVsEta_") +  m_layerNames[iLyr] + "_" + thr.m_threshName;//+cscVetoName;
	std::string histTitle;
	if (thr.m_doPercentageOccupancy)
	  histTitle="Percentage of events in (#eta) for ";
	else {
	  histTitle="No. of events in (#eta) for "; //+  m_layerNames[iLyr] + " - " + thr.m_threshTitles[iLyrNS] + cscVetoTitle;
	}
	histTitle+=(m_layerNames[iLyr] + " - " + thr.m_threshTitles[iLyrNS] + cscVetoTitle);

	thr.m_h_occupancy_eta[iLyr]=new TH1F(histName.c_str(),histTitle.c_str(),binning.getNTotEtaBins(),binning.getEtaBinArray());
	thr.m_h_occupancy_eta[iLyr]->GetXaxis()->SetTitle("#eta");
	ATH_CHECK(monGroup1DOcc.regHist(thr.m_h_occupancy_eta[iLyr]));

      }//end doEtaOccupancy

      if (thr.m_doPhiOccupancy) {
	ATH_MSG_DEBUG("Booking phi occupancy histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	std::string histName=std::string("CellOccupancyVsPhi_") +  m_layerNames[iLyr] + "_" + thr.m_threshName;//; + cscVetoName;
	std::string histTitle;
	if (thr.m_doPercentageOccupancy)
	  histTitle="Percentage of events in (#phi) for ";
	else {
	  histTitle="No. of events in (#phi) for "; //+  m_layerNames[iLyr] + " - " + thr.m_threshTitles[iLyrNS] + cscVetoTitle;
	}
	histTitle+=(m_layerNames[iLyr] + " - " + thr.m_threshTitles[iLyrNS] + cscVetoTitle);

	thr.m_h_occupancy_phi[iLyr]=new TH1F(histName.c_str(),histTitle.c_str(),binning.getNTotPhiBins(),binning.getPhiBinArray());
	thr.m_h_occupancy_phi[iLyr]->GetXaxis()->SetTitle("#phi");
	ATH_CHECK(monGroup1DOcc.regHist(thr.m_h_occupancy_phi[iLyr]));

      }//end doPhiOccupancy

    
      if (thr.m_doEtaPhiTotalEnergy || thr.m_doEtaPhiAverageEnergy) {
	ATH_MSG_DEBUG("Booking 2D total energy histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	thr.m_h_totalEnergy_etaphi[iLyr]=newEtaPhiHist("TotalEnergyVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
						       "Total Cell Energy vs (#eta,#phi) in "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
						       binning);
	ATH_CHECK(monGroupTotalE.regHist(thr.m_h_totalEnergy_etaphi[iLyr]));
      }

      if (thr.m_doEtaPhiAverageEnergy) {
	ATH_MSG_DEBUG("Booking 2D average energy histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	thr.m_h_averageEnergy_etaphi[iLyr]=newEtaPhiHist("CellAvgEnergyVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName,
							 "Average Cell Energy vs (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
							 binning);
	ATH_CHECK(monGroupAvgE.regHist(thr.m_h_averageEnergy_etaphi[iLyr]));
      }


      if (thr.m_doEtaPhiAverageTime) {
	ATH_MSG_DEBUG("Booking 2D average time histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);

	thr.m_h_totalTime_etaphi[iLyr]=newEtaPhiHist("TempTotalTimeVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
						   "Total Cell Time vs (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
						   binning);
	//thr.m_h_totalTime_etaphi[iLyr]->Sumw2();
	regTempHist(thr.m_h_totalTime_etaphi[iLyr],monGroupTemp);


	thr.m_h_averageTime_etaphi[iLyr]=newEtaPhiHist("AverageTimeVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName,
						     "Average Cell Time vs (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
						     binning);
	ATH_CHECK(monGroupAvgT.regHist(thr.m_h_averageTime_etaphi[iLyr]));
      }// end if doEtaPhiAverageTime


      if (thr.m_doEtaPhiFractionPastTth) {
	ATH_MSG_DEBUG("Booking 2D poor time histograms for threshold " << thr.m_threshName << ", layer " << m_layerNames[iLyr]);
	thr.m_h_poorTimeOccupancy_etaphi[iLyr]=newEtaPhiHist("TempPoorTimeOccupancy_"+m_layerNames[iLyr]+"_"+thr.m_threshName,
							     "Occupancy of Cells beyond quality threshold vs (#eta,#phi) for "+m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle, 
							     binning);
	//thr.m_h_poorTimeOccupancy_etaphi[iLyr]->Sumw2();
	regTempHist(thr.m_h_poorTimeOccupancy_etaphi[iLyr],monGroupTemp);
     
	std::stringstream sHistTitle;
	sHistTitle.precision(0);
	sHistTitle <<  "Fraction of Events in " << m_layerNames[iLyr] << "  with " << thr.m_threshName 
		   <<  " for which the Time is further than " << thr.m_timeThreshold << " from Zero";
	thr.m_h_fractionPastTth_etaphi[iLyr]=newEtaPhiHist("fractionPastTthVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName,
							   sHistTitle.str().c_str(),binning);
      
	ATH_CHECK(monGroupOutOfTime.regHist(thr.m_h_fractionPastTth_etaphi[iLyr]));
      }//end if doEtaPhiFractionPastTth

      if (thr.m_doEtaPhiFractionOverQth) {
	thr.m_h_poorQualityOccupancy_etaphi[iLyr]=newEtaPhiHist("TempPoorQualityOccupancy"+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
								"Occupancy of Cells beyond quality threshold vs (#eta,#phi) in" +m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
								binning);
	regTempHist(thr.m_h_poorQualityOccupancy_etaphi[iLyr],monGroupTemp);


	
	std::stringstream sHistTitle;
	sHistTitle.precision(0);
	sHistTitle <<  "Fraction of Events in " << m_layerNames[iLyr] << "  with " << thr.m_threshName 
		   << " for which the Quality Factor Exceeds " << thr.m_qualityFactorThreshold;

	thr.m_h_fractionOverQth_etaphi[iLyr]=newEtaPhiHist("fractionOverQthVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName,
							   sHistTitle.str().c_str(),binning);

	ATH_CHECK(monGroupPoorQ.regHist(thr.m_h_fractionOverQth_etaphi[iLyr]));
      }
    
      if (thr.m_doEtaPhiAverageQuality) {
	thr.m_h_totalQuality_etaphi[iLyr]=newEtaPhiHist("TempTotalQuality_"+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
							"Total quality factor of Cells vs (#eta,#phi) in " + m_layerNames[iLyr]+" - "+thr.m_threshTitles[iLyrNS]+cscVetoTitle,
							binning);
	regTempHist(thr.m_h_totalQuality_etaphi[iLyr],monGroupTemp);

		
	thr.m_h_averageQuality_etaphi[iLyr]=newEtaPhiHist("AverageQualityVsEtaPhi_"+m_layerNames[iLyr]+"_"+thr.m_threshName+cscVetoName,
							  "Average Cell Quality in " + m_layerNames[iLyr] + "  with " + thr.m_threshTitles[iLyrNS]+cscVetoTitle,
							  binning);
	
	ATH_CHECK(monGroupAvgQ.regHist(thr.m_h_averageQuality_etaphi[iLyr]));

      }
    }//end loop over layers
  }//end loop over thresholds
  return StatusCode::SUCCESS;
}


std::string  LArCellMonTool::strToLower(const std::string& input) const {
  std::string output;
  for (const auto& c : input) {
    output.push_back(std::tolower(c));
  }
  return output;
}

void LArCellMonTool::resetInternals() {

  ATH_MSG_DEBUG("Resetting internal counters and temporary histograms");

  for (unsigned& i :  m_sporadicPerPartCounter) {
    i=0;
  }
  m_counter_sporadic_protc=0;

  
  for (auto& thr : m_thresholdHists) {
    //Reset event counters used for normalization
    thr.m_eventsPassed=0;
    for (auto& ctr : thr.m_occupancyCounter) { ctr=0;}
    //The following histograms (may) be temporary, so are not reset by THistSvc
    //Note that they are deleted by the underlying root infrastructure (gDirectory)
    //as long as TH1::AddDirectoryStatus is true. 

    if (!m_doSaveTempHists) { //We the mange temporary histograms ourselves
      for (size_t iLyr=0;iLyr<MAXLAYER;++iLyr) {
	for (TH1* h : {thr.m_h_totalTime_etaphi[iLyr], thr.m_h_poorTimeOccupancy_etaphi[iLyr],
	      thr.m_h_poorQualityOccupancy_etaphi[iLyr],thr.m_h_totalQuality_etaphi[iLyr]}) {
	  if(h) h->Reset(); 
	}//end loop over histograms

	if (!thr.m_doEtaPhiOccupancy && thr.m_h_occupancy_etaphi[iLyr]) {  //this one may be managed by underlying root
	  thr.m_h_occupancy_etaphi[iLyr]->Reset();
	}

      }//end loop over layers
    }//end if not saving temp hists

  }//end loop over thresholds

  return;
}


StatusCode LArCellMonTool::fillOccupancyHist(LArCellMonTool::thresholdHist_t& thr) {
  
  for (size_t iLyr=0;iLyr<MAXLAYER;++iLyr) {
    if (thr.m_h_occupancy_etaphi[iLyr]) thr.m_h_occupancy_etaphi[iLyr]->Reset();
    if (thr.m_h_occupancy_eta[iLyr]) thr.m_h_occupancy_eta[iLyr]->Reset();
    if (thr.m_h_occupancy_phi[iLyr]) thr.m_h_occupancy_phi[iLyr]->Reset();
  }


  const CaloDetDescrManager* detDescMgr;
  ATH_CHECK(detStore()->retrieve(detDescMgr));
  
  auto it=detDescMgr->element_begin();
  auto it_e=detDescMgr->element_end();

  for (;it!=it_e;++it) {
    const CaloDetDescrElement* dde=(*it);
    if (dde->is_tile()) continue;

    const IdentifierHash hash=dde->calo_hash();
    assert(hash<thr.m_occupancyCounter.size());

    const unsigned aboveThrCntr=thr.m_occupancyCounter[hash];

    if (aboveThrCntr) { //any hit?
      float celleta, cellphi;
      unsigned iLyr, iLyrNS;
      getHistoCoordinates(dde, celleta, cellphi, iLyr, iLyrNS);

      if (thr.m_h_occupancy_etaphi[iLyr]) thr.m_h_occupancy_etaphi[iLyr]->Fill(celleta,cellphi,aboveThrCntr);
      if (thr.m_h_occupancy_eta[iLyr]) thr.m_h_occupancy_eta[iLyr]->Fill(celleta,aboveThrCntr);
      if (thr.m_h_occupancy_phi[iLyr]) thr.m_h_occupancy_phi[iLyr]->Fill(cellphi,aboveThrCntr);
      
      
    }//end if have at least one hit

  }//end loop over calo DDEs
  return StatusCode::SUCCESS;
}


void LArCellMonTool::getHistoCoordinates(const CaloDetDescrElement* dde, float& celleta, float& cellphi, unsigned& iLyr, unsigned& iLyrNS) const {
  
  celleta=dde->eta_raw();
  cellphi=dde->phi_raw();
  
  int calosample=dde->getSampling();
  if (dde->is_lar_em_endcap_inner()) calosample-=1; //Here, we consider the two layers of the EMEC IW as EME1 and EME2 instad of layer 2 and 3
  iLyrNS=m_caloSamplingToLyrNS.at(calosample); //will throw if out of bounds
  if ((iLyrNS==EMB1NS || iLyrNS==EMB2NS) && m_calo_id->region(dde->identify())==1) {
    //We are in the awkward region 1 of the EM Barrel
    //Looking at the image at http://atlas.web.cern.ch/Atlas/GROUPS/LIQARGEXT/TDR/figures6/figure6-17.eps
    //may be useful to understand what's going on here. 

    //In brief: Region 1, layer 1 is closer related ot the middle compartment (aka layer 2)
    //          and region 1, layer 2 closer related to the back compartment (aka layer 3)
    iLyrNS+=1;

    //Layer 2: 0<eta<1.4 + 1.4<eta<1.475, deta = 0.025. 3 rows of cells from region 1
    //Layer 3: 0<eta<1.35 (deta=0.050) +  1.4<eta<1.475 (deta = 0.075).  1 row of cell from region 1 with different dEta
    //The following couts were used to get the binning right:
    //if (iLyrNS==EMB2NS) std::cout << "Eta of EM layer 1->2 region 1 cell=" << celleta << std::endl;
    //if (iLyrNS==EMB3NS) std::cout << "Eta of EM layer 2->3 region 1 cell=" << celleta << std::endl;
  }
  
  const unsigned side=(celleta>0) ? 0 : 1; //Value >0 means A-side
  iLyr=iLyrNS*2+side;  //Getting LayerEnum value. This logic works because of the way the enums LayerEnum and LayerEnumNoSides are set up. 
  return;
}


void LArCellMonTool::divideByOccupancy(TH2F* fraction, const TH2F* total, const TH2* occupancy) {
  //This method divides total/occupancy and sets the error to sqrt(1/occupancy)
  //this way the weightedAverage merging method does the right thing (modulo rounding errors)

  //Assumes that the histograms have identical binning

  fraction->Reset();
  const size_t nCells=total->GetNcells();
  assert(static_cast<int>(nCells)==fraction->GetNcells());
  assert(static_cast<int>(nCells)==occupancy->GetNcells());
 
  for (size_t i=0;i<nCells;++i) {
    const double t = total->GetBinContent(i);
    const double o = occupancy->GetBinContent(i);
  
    if (o>0) {
      fraction->SetBinContent(i,t/o);
      fraction->SetBinError(i,std::sqrt(1./o));
    }
  }
  return;

}
