/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// NAME:     LArCellMonAlg.cxx
//           based on LArCellMonTool:
//                W.Lampl  - Spring 2017: Major re-design
//                M. Spalla, P.Strizenec - Migration to AthenaMT (2020)
// ********************************************************************
#include "LArCellMonAlg.h"

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

#include <cassert>
#include <algorithm>

////////////////////////////////////////////
LArCellMonAlg::LArCellMonAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  :CaloMonAlgBase(name, pSvcLocator),
   m_badChannelMask("BadLArRawChannelMask",this),
   m_LArOnlineIDHelper(nullptr),
   m_calo_id(nullptr)
{    

  // Trigger Awareness:
  declareProperty("rndmTriggerNames", m_triggerNames[RNDM]);
  declareProperty("caloTriggerNames",m_triggerNames[CALO]);
  declareProperty("minBiasTriggerNames",m_triggerNames[MINBIAS]);
  declareProperty("metTriggerNames",m_triggerNames[MET]);
  declareProperty("miscTriggerNames",m_triggerNames[MISC]);
  
  // Bad channel masking options 
  declareProperty("LArBadChannelMask",m_badChannelMask,"Tool handle for LArBadChanelMasker");


  // LAr Thresholdsd
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



LArCellMonAlg::~LArCellMonAlg() {

}

////////////////////////////////////////////
StatusCode LArCellMonAlg::initialize() {

  ATH_MSG_DEBUG("LArCellMonAlg::initialize() start");

  // Initialize superclass
  ATH_CHECK( CaloMonAlgBase::initialize() );

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

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_noiseCDOKey.initialize() );

  // Bad channels key
  //(used for building eta phi map of known bad channels)
  if( m_doKnownBadChannelsVsEtaPhi) {
    ATH_CHECK(m_BCKey.initialize());
  }


  //JobO consistency check:
  if (m_useTrigger && std::all_of(m_triggerNames.begin(),m_triggerNames.end(),[](const std::string& trigName){return trigName.empty();})) {
      ATH_MSG_WARNING("UseTrigger set to true but no trigger names given! Forcing useTrigger to false");
      m_useTrigger=false;
  }

  // FIXME check consistency between layer and partitions from jO to enums
 
  // Check that sizes of layer names and ncells is the same
  if (m_layerNames.size() != m_layerNcells.size()) {
        ATH_MSG_ERROR("LayerNames and LayerNcells not of the same length, aborting.....");
        return StatusCode::FAILURE;
  }

  //retrieve trigger decision tool and chain groups
  if( m_useTrigger && !getTrigDecisionTool().empty() ) {
    const ToolHandle<Trig::TrigDecisionTool> trigTool=getTrigDecisionTool();
    ATH_MSG_INFO("TrigDecisionTool retrieved");
    for (size_t i=0;i<NOTA;++i) {
      const std::string& trigName=m_triggerNames[i];
      if (!trigName.empty()) m_chainGroups[i]=trigTool->getChainGroup(trigName.c_str());
    }//end loop over TriggerType enum
  }//end if m_useTrigger

  // Sets the threshold value arrays
  ATH_CHECK(initThresh());

  //Fill the LArCellBinning for each layer
  //setLArCellBinning();

  m_toolmapAll = Monitored::buildToolMap<std::map<std::string,int>>(m_tools, "allMon", m_layerNames, m_thresholdNameProp);

  ATH_MSG_DEBUG("LArCellMonAlg::initialize() is done!");

  return StatusCode::SUCCESS;
}



StatusCode LArCellMonAlg::initThresh() {

  //Interpret the jobO defining the per-threshold histograms 
  //and initialize vector<threshold_t> m_thresholds;

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


  const std::map<const std::string, const Direction> stringToDirection {{"over",OVER},{"under",UNDER},{"both",BOTH},{"none",NONE}};
  const std::map<const std::string, const TriggerType> stringToTrigType {{"nota",NOTA},{"rndm",RNDM},{"calo",CALO},{"minbias",MINBIAS},
									{"met",MET},{"misc",MISC},{"none",NOTA},{"all",NOTA}};

  m_thresholds.resize(nThr);
  for (size_t iThr=0;iThr<nThr;++iThr) {
    threshold_t& threshold=m_thresholds[iThr];
    threshold.m_threshName=m_thresholdNameProp.value()[iThr];
    threshold.m_thrIndex=iThr;
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

    threshold.m_doPercentageOccupancy=isThrListed(m_doEtaPhiPercentageOccupancyNames.value(),threshold.m_threshName);
    threshold.m_doEtaPhiOccupancy=isThrListed(m_doEtaPhiTotalOccupancyNames.value(),threshold.m_threshName);
    threshold.m_doEtaOccupancy=isThrListed(m_doEtaOccupancyNames.value(),threshold.m_threshName);
    threshold.m_doPhiOccupancy=isThrListed(m_doPhiOccupancyNames.value(),threshold.m_threshName);
    threshold.m_doEtaPhiTotalEnergy=isThrListed(m_doEtaPhiTotEnergyNames.value(),threshold.m_threshName);
    threshold.m_doEtaPhiAverageQuality=isThrListed(m_doEtaPhiAvgQualityNames.value(),threshold.m_threshName);
    threshold.m_doEtaPhiFractionOverQth=isThrListed(m_doEtaPhiFractionOverQthNames.value(),threshold.m_threshName);
    threshold.m_qualityFactorThreshold=m_qualityFactorThresholdProp.value()[iThr];	  
    threshold.m_doEtaPhiAverageTime=isThrListed(m_doEtaPhiAvgTimeNames.value(),threshold.m_threshName);
    threshold.m_doEtaPhiFractionPastTth=isThrListed(m_doEtaPhiFractionPastTthNames.value(),threshold.m_threshName);
    threshold.m_timeThreshold=m_timeThresholdProp.value()[iThr];		  
    threshold.m_doBeamBackgroundRemoval=m_doBeamBackgroundRemovalProp.value()[iThr];
  }//end loop over threshold names

 
  //Overwrite per-layer thresholds:
  for (size_t iThrOvr=0;iThrOvr!= m_thresholdColumnType.value().size();++iThrOvr) {
    const std::string& nameToOverwrite=m_thresholdColumnType.value()[iThrOvr];
    auto it=std::find_if(m_thresholds.begin(),m_thresholds.end(),
			 [&](const threshold_t& x) {return (x.m_threshName==nameToOverwrite);}
			 );
  
    if (it==m_thresholds.end()) {
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
  auto thrIt=m_thresholds.begin();
  while (thrIt!=m_thresholds.end()) {
    const threshold_t& thr=*thrIt;
    if (!(thr.m_doEtaPhiOccupancy || thr.m_doPercentageOccupancy || thr.m_doEtaOccupancy || thr.m_doPhiOccupancy || thr.m_doEtaPhiTotalEnergy ||
	  thr.m_doEtaPhiAverageQuality || thr.m_doEtaPhiFractionOverQth || thr.m_doEtaPhiAverageTime || thr.m_doEtaPhiFractionPastTth)) {
      ATH_MSG_INFO( "Config issue: Threshold type '" << thr.m_threshName << "' defined but no histograms requested. Deleting." );
      thrIt=m_thresholds.erase(thrIt);
    }
    else {
      ATH_MSG_INFO("Threshold histograms requested for threshold '" << thrIt->m_threshName << "'");
      thrIt++;
    }
  }


  //Fix up histogram titles
  for (threshold_t& thr : m_thresholds) {
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
/*
StatusCode LArCellMonAlg::bookHistograms() {

  ATH_MSG_INFO("BookHistogram called");

  resetInternals();


  ATH_CHECK(bookLarMultThreHists());


  ATH_CHECK(bookLarNonThreHists());
  
  return StatusCode::SUCCESS; 
}




*/ 

void LArCellMonAlg::checkTriggerAndBeamBackground(bool passBeamBackgroundRemoval, std::vector<threshold_t> &thresholds) const {

  auto mon_trig = Monitored::Scalar<float>("trigType",-1);

  mon_trig=0.5;
  fill(m_MonGroupName,mon_trig);
  //m_h_n_trigEvent->Fill(0.5);

  if (m_useTrigger) {
    std::bitset<MAXTRIGTYPE> triggersPassed(0x1<<NOTA); //Last bit: NOTA, always passes
    constexpr std::bitset<MAXTRIGTYPE> NOTAmask=~(0x1<<NOTA);
    for (unsigned i=0;i<m_chainGroups.size();++i) {
      if (m_chainGroups[i] && m_chainGroups[i]->isPassed()) triggersPassed.set(i);
    }
  
    for (unsigned i=0;i<NOTA;++i) {
      mon_trig=0.5+i;
      fill(m_MonGroupName,mon_trig);
      //if (triggersPassed.test(i)) m_h_n_trigEvent->Fill(0.5+i);  
    }

    for (threshold_t& thr : thresholds) { //Loop over thresholds
      thr.m_threshTriggerDecision=(thr.m_triggersToInclude & triggersPassed).any() && (thr.m_triggersToExclude & triggersPassed & NOTAmask).none();
    }// end loop over thresholds
  } //end if trigger used
  else {
    mon_trig=6.5;
    fill(m_MonGroupName,mon_trig);
    //m_h_n_trigEvent->Fill(6.5); 
  }
  //Note that thr.m_threshTriggerDecision remains in it's default state 'true' if trigger wasn't used

  //Check beam-background removal 
    for (const threshold_t& thr : thresholds) { //Loop over thresholds     
      if (thr.m_threshTriggerDecision && (passBeamBackgroundRemoval || !thr.m_doBeamBackgroundRemoval)) {
	//The counter of events passing. Will be incremented even if neither trigger nor background removal is requested for this threshold
	auto eventCounter = Monitored::Scalar<size_t>("eventCounter",thr.m_thrIndex);
	fill(m_MonGroupName,eventCounter);
      }
    }//end loop over thresholds
  return;
}

/*
void LArCellMonAlg::sporadicNoiseCandidate(const CaloCell* cell, const LArCellMonAlg::LayerEnum iLyr, const float threshold, const LArOnOffIdMapping* cabling) const {
  
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
*/

/*
void LArCellMonAlg::bookNoisyCellHistos(SporadicNoiseCell_t& result, const CaloDetDescrElement* dde, 
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
*/

struct LArMonValues {
    float mon_eta;
    float mon_phi;
    float en;
    float time;
    uint16_t mon_qual; 
    bool pass_qual;
    bool pass_time;
    bool passThrCut;
};

////////////////////////////////////////////////////////////////////////////
StatusCode LArCellMonAlg::fillHistograms(const EventContext& ctx) const{  

  ATH_MSG_DEBUG("LArCellMonAlg::fillHistograms() starts");

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling");
     return StatusCode::FAILURE;
  }

  SG::ReadHandle<CaloCellContainer> cellHdl{m_cellContainerKey, ctx};
  const CaloCellContainer* cellCont = cellHdl.cptr();
	    
  SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey, ctx};
  const CaloNoise *noisep = *noiseHdl;

  if (ctx.evt()==0) {
    ATH_CHECK(createPerJobHistograms(cellCont, noisep));
  }

  bool ifPass = true;
  bool passBeamBackgroundRemoval = true;
  ATH_CHECK(checkFilters(ifPass,passBeamBackgroundRemoval,m_MonGroupName,ctx)); //Check ATLAS-Ready, beam-background, etc from base class
  if(!ifPass) return StatusCode::SUCCESS;

  std::vector<threshold_t> thresholds = m_thresholds;

  checkTriggerAndBeamBackground(passBeamBackgroundRemoval, thresholds);

  //get LB
  auto lumiBlock = Monitored::Scalar<unsigned int>("lumiBlock",0);
  lumiBlock = GetEventInfo(ctx)->lumiBlock();

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 
  // loop over cells -------------

  std::vector<std::vector<std::vector<LArMonValues>>> monValueVec;
  monValueVec.reserve(m_layerNames.size());
  for (size_t ilayer = 0; ilayer < m_layerNames.size(); ++ilayer) {
    monValueVec.emplace_back();
    monValueVec[ilayer].reserve(thresholds.size());
    for (size_t ithreshold = 0; ithreshold < thresholds.size(); ++ithreshold) {
      monValueVec[ilayer].emplace_back();
      // this could be more intelligent (this is the worst case for #cells in a layer, most are much less)
      monValueVec[ilayer][ithreshold].reserve(m_layerNcells[ilayer]);
    }
  }

  for ( ; it!=it_e;++it) {
    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    bool is_lar=m_calo_id->is_lar(id);
    const ULong64_t monCellID = id.get_compact();

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

    float celleta,cellphi;
    unsigned iLyr, iLyrNS;
    getHistoCoordinates(caloDDEl, celleta, cellphi, iLyr, iLyrNS);

    //Start filling per-threshold histograms:
    auto& lvaluemap = monValueVec[iLyr];
    for (size_t ithr = 0; ithr < thresholds.size(); ++ithr) {
      const auto& thr = thresholds[ithr];
    // for (auto& thr :  thresholds) {
      //std::cout << "Threshold name " << thr.m_threshName << std::endl;
      //Any of the conditons below means we do not fill the histogram:
    
      //Trigger passed?
      if (m_useTrigger && !thr.m_threshTriggerDecision) continue;
      //std::cout << " Trigger passed" << std::endl;

      //Beam background event?
      if (thr.m_doBeamBackgroundRemoval && !passBeamBackgroundRemoval) continue;

      //std::cout << " Beam background passed" << std::endl;

      float thresholdVal=thr.m_threshValue[iLyrNS];
      if (thresholdVal==0) {
	ATH_MSG_WARNING("Got threshold 0 for type '" << thr.m_threshName  << "'for cell in layer " << m_layerNames[iLyr]);
      }

      if (thr.m_inSigNoise) thresholdVal*=noisep->getNoise(cellHash, gain);
        

      bool passThrCut(true);
      if (thr.m_threshDirection==OVER && cellen <= thresholdVal) passThrCut=false;
      if (thr.m_threshDirection==UNDER && cellen > thresholdVal) passThrCut=false;
      if (thr.m_threshDirection==BOTH && (cellen > -thresholdVal && cellen <= thresholdVal)) passThrCut=false;   


    	bool pass_qual = (cellquality > thr.m_qualityFactorThreshold);
    	bool pass_time = (fabs(celltime) > thr.m_timeThreshold);

      lvaluemap[ithr].push_back({celleta, cellphi, cellen, celltime, cellquality, pass_qual, pass_time, passThrCut});

      if(!passThrCut) continue;
    
    }//end loop over thresholds

    //some additional monitored objects
    auto en = Monitored::Scalar<float>("cellEnergy_"+m_layerNames[iLyr],cellen);
    auto tim = Monitored::Scalar<float>("cellTime_"+m_layerNames[iLyr],celltime);
    if(passBeamBackgroundRemoval) {
      // 1D Energy distribution:
      //if (m_h_energy[iLyr]) m_h_energy[iLyr]->Fill(cellen); 

      // Time vs Energy:
      if (m_doEnergyVsTime && celltqavailable && cellquality<4000) {
	bool passecut = (cellen>m_eCutForTiming[iLyrNS]);
	auto eGTcut = Monitored::Scalar<float>("enGreaterThanCut_"+m_layerNames[iLyr],passecut);
	fill(m_MonGroupName,en,tim,eGTcut);

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
	if (cellen > energyThreshold) {
	  ATH_MSG_INFO( "Found sporadic noise candidate cell with id 0x" << std::hex << id.get_identifier32().get_compact() << std::dec << " in " << m_partitionNames[m_layerEnumtoPartitionEnum[iLyr]] );
	  auto sporadicCellE = Monitored::Scalar<float>("sporadicCellE",cellen);
	  auto sporadicCellTime = Monitored::Scalar<float>("sporadicCellTime",celltime);
	  auto sporadicCellQuality = Monitored::Scalar<uint16_t>("sporadicCellQuality",cellquality);
	  auto sporadicCellID = Monitored::Scalar<ULong64_t>("sporadicCellID",monCellID);
	  fill(m_MonGroupName,sporadicCellE,sporadicCellTime,sporadicCellQuality,sporadicCellID,lumiBlock);
	}  
      }//end if m_sporadic_switch
    } // end if m_passBeamBackgroundRemoval
  }//end loop over cells

  // fill, for every layer/threshold
  for (size_t ilayer = 0; ilayer < monValueVec.size(); ++ilayer) {
    for (size_t ithreshold = 0; ithreshold < monValueVec[ilayer].size(); ++ithreshold) {
      const auto& tool = monValueVec[ilayer][ithreshold];
      auto mon_eta = Monitored::Collection("celleta",tool,[](const auto& v){return v.mon_eta;});
      auto mon_phi = Monitored::Collection("cellphi",tool,[](const auto& v){return v.mon_phi;});
      auto en = Monitored::Collection("cellEnergy",tool,[](const auto& v){return v.en;});
      auto mon_qual = Monitored::Collection("cellQuality",tool,[](const auto& v){return v.mon_qual;}); 
      auto pass_qual = Monitored::Collection("isPoorQuality",tool,[](const auto& v){return v.pass_qual;});
      auto tim = Monitored::Collection("cellTime",tool,[](const auto& v){return v.time;});
      auto pass_time = Monitored::Collection("isLateTime",tool,[](const auto& v){return v.pass_time;});
      auto passThrCut = Monitored::Collection("passThrCut",tool,[](const auto& v){return v.passThrCut;});
      fill(m_tools[m_toolmapAll.at(m_layerNames[ilayer]).at(thresholds[ithreshold].m_threshName)], 
              passThrCut, mon_eta, mon_phi, en, mon_qual,
              pass_qual, tim, pass_time);
    }
  }

  ATH_MSG_DEBUG("LArCellMonAlg::fillLarHists() is done");
  return StatusCode::SUCCESS;
}


StatusCode LArCellMonAlg::createPerJobHistograms(const CaloCellContainer* cellCont, const CaloNoise *noisep ) const {

  ATH_MSG_INFO("Creating the once-per-job histograms");
  //The following histograms can be considered constants for one job
  //(in fact, they are constant for an entire run or even run-periode)
  //ActiveCells in eta/phi (to normalize 1D occupancy plots)
  //BadChannel word
  //Database noise
   
  auto doDatabaseNoisePlot = Monitored::Scalar<bool>("doDatabaseNoisePlot",m_doDatabaseNoiseVsEtaPhi);
  auto doCellsActiveEtaPlot = Monitored::Scalar<bool>("doCellsActiveEtaPlot",m_doEtaOccupancyNames.size()>0);
  auto doCellsActivePhiPlot = Monitored::Scalar<bool>("doCellsActivePhiPlot",m_doPhiOccupancyNames.size()>0);

  if(!m_doKnownBadChannelsVsEtaPhi && !doDatabaseNoisePlot && !doCellsActiveEtaPlot && !doCellsActivePhiPlot) {
    ATH_MSG_INFO("No once-per-job histogram requested");
    return StatusCode::SUCCESS;
  }

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
    
    auto mon_eta = Monitored::Scalar<float>("celleta_"+m_layerNames[iLyr],celleta);
    auto mon_phi = Monitored::Scalar<float>("cellphi_"+m_layerNames[iLyr],cellphi);
    
    const LArBadChannel larBadChannel = bcCont->offlineStatus(id);
    auto fillBadChannelPlot = Monitored::Scalar<bool>("fillBadChannelPlot",(m_doKnownBadChannelsVsEtaPhi && (!larBadChannel.good())));
    auto badCellWord = Monitored::Scalar<uint32_t>("badCellWord_"+m_layerNames[iLyr],larBadChannel.packedData());
    
    auto cellnoisedb = Monitored::Scalar<float>("cellnoisedb_"+m_layerNames[iLyr],noisep->getNoise(id,cell->gain()));
    
    fill(m_MonGroupNamePerJob,badCellWord,cellnoisedb,mon_eta,mon_phi,fillBadChannelPlot,doDatabaseNoisePlot,doCellsActiveEtaPlot,doCellsActivePhiPlot);	
  }//end loop over cells

  return StatusCode::SUCCESS;
}



std::string  LArCellMonAlg::strToLower(const std::string& input) const {
  std::string output;
  for (const auto& c : input) {
    output.push_back(std::tolower(c));
  }
  return output;
}

 bool LArCellMonAlg::isThrListed(std::vector<std::string> vec, std::string s) const {
   return (std::find(vec.begin(),vec.end(),s)!=vec.end());
 }



void LArCellMonAlg::getHistoCoordinates(const CaloDetDescrElement* dde, float& celleta, float& cellphi, unsigned& iLyr, unsigned& iLyrNS) const {
  
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
  }
  
  const unsigned side=(celleta>0) ? 0 : 1; //Value >0 means A-side
  iLyr=iLyrNS*2+side;  //Getting LayerEnum value. This logic works because of the way the enums LayerEnum and LayerEnumNoSides are set up. 
  return;
}


