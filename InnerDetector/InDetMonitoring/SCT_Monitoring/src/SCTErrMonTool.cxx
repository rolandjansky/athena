/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTErrMonTool.cxx
 *
 *
 *
 *
 *    @author Martin White, based on code by Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson
 *    Major tidying/restructuring by Martin Flechl
 */


#include "SCT_Monitoring/SCTErrMonTool.h"
#include "SCT_Monitoring/SCT_ClusterStruct.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"

//conditions stuff
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TProfile2D.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "cArrayUtilities.h"
#include <vector>
#include <set>
#include <iostream>
#include <map>


using namespace std;
using namespace SCT_Monitoring;

namespace{ //anon. namespace for file-scoped functions
  
  //test offline the online code
  const bool testOffline(false);
 
  enum Pattern{ IXX=0, XIX, XXI };
  
  bool timeBinInPattern(const int tbin, const Pattern xxx){
    switch (xxx) {
    case IXX:
      return (tbin>3);
      break;
    case XIX:
      return (tbin==2||tbin==3||tbin==6||tbin==7);
      break;
    case XXI:
      return (tbin==1||tbin==3||tbin==5||tbin==7);
      break;
    default:
      return false;
      break;
    }
  }
  const std::string names[]={"Endcap C","Barrel","Endcap A"};
  const std::string abbreviations[]={"ECC", "Bar","ECA"};
  
  int numberOfInefficientSides( TH2* pHistogramArray[],const int xbin, const int ybin, const float threshold){
    float histogramBinContent0=pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1=pHistogramArray[1]->GetBinContent(xbin, ybin);
    int side0Inefficient=(histogramBinContent0 > threshold);
    int side1Inefficient=(histogramBinContent1 > threshold);
    return side0Inefficient + side1Inefficient;
  }
  
  bool eitherSideIsOverThreshold( TH2* pHistogramArray[],const int xbin, const int ybin, const float threshold){
    float histogramBinContent0=pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1=pHistogramArray[1]->GetBinContent(xbin, ybin);
    return ((histogramBinContent0 > threshold) or (histogramBinContent1 > threshold));
  }

  bool checkOverThreshold( TProfile2D* pHistogram,const int xbin, const int ybin, const float threshold){
    float histogramBinContent=pHistogram->GetBinContent(xbin, ybin);
    return (histogramBinContent > threshold);
  }
  
  bool thereAreEnoughEntries( TH2* pHistogramArray[], const float threshold){
    float histogramEntries0=pHistogramArray[0]->GetEntries();
    float histogramEntries1=pHistogramArray[1]->GetEntries();
    return ((histogramEntries0 > threshold) and (histogramEntries1 > threshold));
  }
  
  void fillRateHistogram(const TH2F* sourceHisto, TProfile2D* targetHisto, const int xbin, const int ybin, const float centreX, const float centreY, const int nEvent){
     const float content = sourceHisto->GetBinContent(xbin,ybin); 
      targetHisto->Fill(centreX,centreY,1,content); 
      targetHisto->Fill(centreX,centreY,0,nEvent - content);
  }
  
  void countNoisyModules(const int regionIndex, const int generalIndex, TH2* pHistogramArray[], const float threshold, int countArray[]){
     const int xbins(pHistogramArray[0]->GetNbinsX()+1);
      const int ybins(pHistogramArray[0]->GetNbinsY()+1);
      for (int xb(1); xb !=xbins; ++xb){
        for (int yb(1); yb !=ybins; ++yb){
          if (eitherSideIsOverThreshold(pHistogramArray,xb,yb,threshold)){
            countArray[generalIndex]++; 
            countArray[regionIndex]++;
          }
        }
      }
  }

  void countNoisyModulesRN(const int regionIndex, const int generalIndex, TProfile2D* pHistogram, const float threshold, int countArray[]){
    const int xbins(pHistogram->GetNbinsX()+1);
    const int ybins(pHistogram->GetNbinsY()+1);
    for(int xb(1); xb!=xbins; ++xb){
      for(int yb(1); yb!=ybins; ++yb){
	if(checkOverThreshold(pHistogram,xb,yb,threshold)){
	  countArray[generalIndex]++;
	  countArray[regionIndex]++;
	}
      }
    }
  }
}



  


//====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection of the filepath for histograms, 
 *  the first and second plane numbers to be used, and the timebin.
 */
//====================================================================================================
SCTErrMonTool::SCTErrMonTool(const std::string & type,const std::string & name,const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
  	m_firstHit{},
  	m_firstHit_ECp{},
  	m_firstHit_ECm{},
  	m_secondHit{},
  	m_secondHit_ECp{},
  	m_secondHit_ECm{},
  	m_allErrs{},
  	m_allErrsPerLumi{},
  	m_pallErrs{},
  	m_pallErrsPerLumi{},
  	m_summaryErrsRecent{},
  	m_numErrorsPerLumi{},
  	m_rateErrorsPerLumi{},
  	m_nErrors{},
  	m_nLinksWithErrors{},
  	nErrors_buf{},
  	nLinksWithErrors_buf{},
  	nErrors_pos{},
  	m_MaskedLinks{},
  	m_numberOfEventsLumi{},
  	m_numberOfEvents{},
  	m_initialize{},
  	m_previous_lb{},
  	m_maskedLinkErrsMax{},
  	m_robFragmentErrsMax{},
  	m_abcdErrsMax{},
  	m_rawErrsMax{},
  	m_timeoutErrsMax{},
  	m_lvl1idErrsMax{},
  	m_bcidErrsMax{},
  	m_preambleErrsMax{},
  	m_formatterErrsMax{},
  	m_rodClockErrsMax{},
  	m_truncRodErrsMax{},
  	m_bsParseErrsMax{},
  m_misslinkErrsMax{},
  	m_totErrsMax{},
  	m_totModErrsMax{},
    m_nLink0{},
    m_nLink1{},
    m_goodModules{},
    m_pnoiseoccupancymapHistoVectorECC{},
    m_pnoiseoccupancymapHistoVectorECCSide0{},
    m_pnoiseoccupancymapHistoVectorECCSide1{},
    m_pnoiseoccupancymapHistoVectorBar{},
    m_pnoiseoccupancymapHistoVectorBarSide0{},
    m_pnoiseoccupancymapHistoVectorBarSide1{},
    m_pnoiseoccupancymapHistoVectorECA{},
    m_pnoiseoccupancymapHistoVectorECASide0{},
    m_pnoiseoccupancymapHistoVectorECASide1{},
    m_noSidesHit{},
    m_oneSideHit{},
    nZero{},
    nOne{},
    nOneSide0{},
    nOneSide1{},
    nLayer{},
    nEta{},
    nPhi{},
    nNonGoodModule{},  
    tbin(-1),
    modNum(0),
    ratio(-1.0),
    ratioside0(-1.0),
    ratioside1(-1.0),
    m_p2DmapHistoVectorAll{},
    //m_stream{}, declared property
    m_path{},
    //m_checkrate{}, declared property
    //m_checkrecent{}, declared property
    m_current_lb{},
    m_last_reset_lb{},
    //m_runOnline{}, declared property
    //m_evtsbins{}, declared property
    //m_doPositiveEndcap{}, declared property
    //m_doNegativeEndcap{}, declared property
    //m_makeConfHisto{}, property
    //m_doPerLumiErrors{}, property
    //m_doErr2DPerLumiHists{}, property
    //m_min_stat_ineff_mod{}, property
    m_dataObjectName{},
    m_pSCTHelper{},
    m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name),
    m_flaggedSvc("InDetSCT_FlaggedConditionSvc", name),
    m_Conf{},
    m_ConfRN{},
    m_ConfOnline{},
    m_MaskedLinksVsLB{},
    m_ROBFragmentVsLB{},
    m_ABCDVsLB{},
    m_RawErrsVsLB{},
    m_TimeOutVsLB{},
    m_LVL1IDVsLB{},
    m_BCIDVsLB{},
    m_PreambleVsLB{},
    m_FormatterVsLB{},
    m_RODClockVsLB{},
    m_TruncRODVsLB{},
    m_BSParseVsLB{},
    m_MissingLinkHeaderVsLB{},
    m_MaxMaskedLinksVsLB{},
    m_MaxROBFragmentVsLB{},
    m_MaxABCDVsLB{},
    m_MaxRawErrsVsLB{},
    m_MaxTimeOutVsLB{},
    m_MaxLVL1IDVsLB{},
    m_MaxBCIDVsLB{},
    m_MaxPreambleVsLB{},
    m_MaxFormatterVsLB{},
    m_MaxRODClockVsLB{},
    m_MaxTruncRODVsLB{},
    m_MaxBSParseVsLB{},
    m_MaxMissingLinkHeaderVsLB{},
    // m_2d_MaxMaskedLinksVsLB{},
    // m_2d_MaxROBFragmentVsLB{},
    // m_2d_MaxABCDVsLB{},
    // m_2d_MaxRawErrsVsLB{},
    // m_2d_MaxTimeOutVsLB{},
    // m_2d_MaxLVL1IDVsLB{},
    // m_2d_MaxBCIDVsLB{},
    // m_2d_MaxPreambleVsLB{},
    // m_2d_MaxFormatterVsLB{},
    // m_2d_MaxRODClockVsLB{},
    // m_2d_MaxTruncRODVsLB{},
    // m_2d_MaxBSParseVsLB{},
    // m_2d_MaxMissingLinkHeaderVsLB{},
    m_NumberOfErrorsVsLB{},
    m_ModulesWithErrorsVsLB{},
    m_LinksWithErrorsVsLBBarrel{},
    m_LinksWithErrorsVsLBEndcapA{},
    m_LinksWithErrorsVsLBEndcapC{},
    m_MaxNumberOfErrorsVsLB{},
    m_MaxModulesWithErrorsVsLB{},
    // m_2d_MaxNumberOfErrorsVsLB{},
    // m_2d_MaxModulesWithErrorsVsLB{},
    m_ConfEffOnline{},
    m_ConfNoiseOnline{},
    m_ConfNoiseOnlineRecent{},
    m_DetailedConfiguration{},
    m_thistSvc("THistSvc",name),
    m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
    m_checkBadModules(true),
    m_ignore_RDO_cut_online(true)
    //m_errThreshold{}, property
    //m_effThreshold{}, property
    //m_noiseThreshold{}, property
{
/** sroe 3 Sept 2015:
  histoPathBase is declared as a property in the base class, assigned to m_path
  with default as empty string.
	Declaring it here as well gives rise to compilation warning
	WARNING duplicated property name 'histoPathBase', see https://its.cern.ch/jira/browse/GAUDI-1023

  declareProperty("histoPathBase", m_stream = "/stat"); **/
  
  m_stream="/stat";
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("runOnline",m_runOnline=false);
  declareProperty("CheckRecent",m_checkrecent=20);
  declareProperty("doPositiveEndcap",m_doPositiveEndcap=true);
  declareProperty("doNegativeEndcap",m_doNegativeEndcap=true);
  declareProperty("EvtsBins",m_evtsbins=5000);
  declareProperty("MakeConfHisto",    m_makeConfHisto=true);
  declareProperty("conditionsService" , m_ConfigurationSvc);
  declareProperty("flaggedService", m_flaggedSvc);

  //Thresholds for the SCTConf histogram
  declareProperty("error_threshold", m_errThreshold=0.7);
  declareProperty("efficiency_threshold", m_effThreshold=0.9);
  declareProperty("noise_threshold", m_noiseThreshold=150);
  // Min stats per layer to use for number of inefficient modules
  declareProperty("MinStatsForInEffModules",m_min_stat_ineff_mod=500.0);

  declareProperty("DoPerLumiErrors",m_doPerLumiErrors=true);
  declareProperty("DoErr2DPerLumiHists",m_doErr2DPerLumiHists=false);
  declareProperty("checkBadModules", m_checkBadModules);
  declareProperty("IgnoreRDOCutOnline",m_ignore_RDO_cut_online);
}

//====================================================================================================
//====================================================================================================
SCTErrMonTool::~SCTErrMonTool(){
  free(nErrors_buf);
  free(nLinksWithErrors_buf);
}

//====================================================================================================
//                            SCTErrMonTool :: bookHistograms
//====================================================================================================
StatusCode SCTErrMonTool::bookHistograms()
{  
  ATH_MSG_DEBUG( " initialize being called " );
  if(newRun){
    m_numberOfEvents=0;
    if(AthenaMonManager::dataType()==AthenaMonManager::cosmics)m_checkrate=100;
  }
  if(ManagedMonitorToolBase::newLumiBlock) m_numberOfEventsLumi=0;
  m_dataObjectName = "SCT_RDOs";
  const InDetDD::SCT_DetectorManager* mgr; //confusingly this is in a dedicated namespace
  ATH_CHECK(detStore()->retrieve(mgr, "SCT"));
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper,"SCT_ID"));

  // Services for Summary Histograms: SCT_ModuleConditionsTool from CondDB and flagged conditions service
  ATH_MSG_INFO( "Checking for CondDB" );
  ATH_CHECK(m_ConfigurationSvc.retrieve()); 
  ATH_MSG_INFO( "Checking for Flagged Service" );
  ATH_CHECK(m_flaggedSvc.retrieve());

  // get a handle on the histogramming service //
  ATH_CHECK(m_thistSvc.retrieve());

  /// SUMMARY HISTOGRAM
  if ( bookConfMaps().isFailure() )                  msg(MSG::WARNING) << "Error in bookConfMaps()" << endreq;
  if ( bookPositiveEndCapConfMaps().isFailure() )    msg(MSG::WARNING) << "Error in bookPositiveEndCapConfMaps()" << endreq;
  if ( bookNegativeEndCapConfMaps().isFailure() )    msg(MSG::WARNING) << "Error in bookNegativeEndCapConfMaps()" << endreq;

  /// BARREL
  if( bookErrHistos().isFailure() )   msg(MSG::WARNING) << "Error in bookErrHistos()" << endreq;

  /// POSITIVE ENDCAP
  if(m_doPositiveEndcap){
    if( bookPositiveEndCapErrHistos().isFailure() )  msg(MSG::WARNING) << "Error in bookPositiveEndCapErrHistos()" << endreq;
  }
  /// NEGATIVE ENDCAP
  if(m_doNegativeEndcap){
    if( bookNegativeEndCapErrHistos().isFailure() )  msg(MSG::WARNING) << "Error in bookNegativeEndCapErrHistos()" << endreq;
  }
  ATH_CHECK(m_byteStreamErrSvc.retrieve());

  m_initialize=true;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                            SCTErrMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time 
//====================================================================================================
StatusCode SCTErrMonTool::fillHistograms(){
  typedef SCT_RDORawData SCTRawDataType;
  const EventInfo* pEvent(0);
  if( evtStore()->retrieve(pEvent).isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve event info!" );
    return StatusCode::RECOVERABLE; 
  }
  m_current_lb = pEvent->event_ID()->lumi_block();
  if (fillByteStreamErrors().isFailure() ) {
    ATH_MSG_ERROR("Could not fill ByteStreamErrors ");
    return StatusCode::FAILURE;
  }  
  const SCT_RDO_Container* p_rdocontainer;
  ATH_CHECK(evtStore()->retrieve(p_rdocontainer,m_dataObjectName));
  Identifier SCT_Identifier;

  //Define variables for error histograms
  int numFirstHit=0,     numSecondHit=0,      numSCTRDOs=0;
  int numFirstHit_ECp=0, numSecondHit_ECp=0,  numSCTRDOs_ECp=0;
  int numFirstHit_ECm=0, numSecondHit_ECm=0,  numSCTRDOs_ECm=0;

  for(int i= 0; i < n_mod[GENERAL_INDEX]; i++){
    m_nLink0[i]=0;
    m_nLink1[i]=0;
    m_goodModules[i] = true;
  }

  tbin = -1;

  int count_SCT_RDO = 0;
  //Use new IDC
  SCT_RDO_Container::const_iterator lastCol  = p_rdocontainer->end();
  for(SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin(); col_it!= lastCol ; ++col_it){
    const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
    if (!SCT_Collection) continue;  // select only SCT RDOs
    DataVector<SCTRawDataType>::const_iterator p_rdo_end= SCT_Collection->end();
    const InDetRawDataCollection<SCT_RDORawData>* rd(*col_it);

    Identifier SCT_Identifier = SCT_Collection->identify();
    int thisLayerDisk = m_pSCTHelper->layer_disk(SCT_Identifier);
    int thisPhi       = m_pSCTHelper->phi_module(SCT_Identifier);
    int thisEta       = m_pSCTHelper->eta_module(SCT_Identifier);
    int thisSide      = m_pSCTHelper->side(SCT_Identifier);
       
    modNum = (rd->identifyHash())/2;
    
    bool goodModule = (m_checkBadModules and m_ConfigurationSvc) ? m_ConfigurationSvc->isGood(rd->identifyHash()) : true;

    m_goodModules[modNum] = goodModule;
    nLayer[modNum]= thisLayerDisk;
    nEta[modNum]= thisEta;  
    nPhi[modNum]= thisPhi;

    if(!goodModule){
      nNonGoodModule[modNum] = 1;
    }
    for(DataVector<SCTRawDataType>::const_iterator p_rdo=SCT_Collection->begin() ; p_rdo!=p_rdo_end; ++p_rdo){
      count_SCT_RDO++;
      SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*p_rdo);
      if (rdo3!=0) tbin = (rdo3)->getTimeBin();
      if(timeBinInPattern(tbin,XIX) && goodModule){
	if(thisSide==1){ 
	  m_nLink1[modNum]+=1;
	}else{
	  m_nLink0[modNum]+=1;
	}
      }
      SCT_Identifier = (*p_rdo)->identify();
      int barrel_ec = m_pSCTHelper->barrel_ec(SCT_Identifier);
      if(barrel_ec==BARREL) {++numSCTRDOs;}
      if(barrel_ec==ENDCAP_A && m_doPositiveEndcap) {++numSCTRDOs_ECp;}
      if(barrel_ec==ENDCAP_C && m_doNegativeEndcap) {++numSCTRDOs_ECm;}

      if (not SCT_Identifier.is_valid()) return msg(MSG::ERROR) <<"The SCT identifier is not valid."<<endreq,StatusCode::FAILURE;

      //Start filling histograms, whilst counting the number of each type of errors. A map of vectors contains 
      //histograms for each type of error. The vectors contain one histogram for each plane of the SCT
      if(rdo3!=0){
        if((rdo3)->FirstHitError()){
          if(barrel_ec==BARREL) numFirstHit++;
          else if(barrel_ec==ENDCAP_A && m_doPositiveEndcap) numFirstHit_ECp++;
          else if(barrel_ec==ENDCAP_C && m_doNegativeEndcap) numFirstHit_ECm++;
        }
        if((rdo3)->SecondHitError()){
          if(barrel_ec==BARREL) numSecondHit++;
          else if(barrel_ec==ENDCAP_A && m_doPositiveEndcap) numSecondHit_ECp++;
          else if(barrel_ec==ENDCAP_C && m_doNegativeEndcap) numSecondHit_ECm++;
        }
      }
    }
  }//end for(SCT_RDO_Container...

  if(count_SCT_RDO<1E6 || (m_ignore_RDO_cut_online && m_environment==AthenaMonManager::online)){
    for(int j=0;j<n_mod[GENERAL_INDEX]; j++){
      m_noSidesHit=false;
      m_oneSideHit=false;
      if(m_nLink0[j] !=0 && m_nLink1[j] !=0) {

      } else {
	if(m_nLink0[j] !=0 || m_nLink1[j] !=0 ){
	  m_oneSideHit=true;
	}
      }
      if(m_nLink0[j] ==0 && m_nLink1[j] ==0 && m_goodModules[j]/*no-sides-fix*/) {
	m_noSidesHit=true;
      }
      if(m_noSidesHit && !nNonGoodModule[j]){
	nZero[j]+=1;
      }
      if(m_oneSideHit){
	nOne[j]+=1;
	if(m_nLink0[j] != 0)
	  nOneSide0[j]+=1;
	if(m_nLink1[j] != 0)
	  nOneSide1[j]+=1;
      }   // end if(oneSideHit)
    }
  }
  if(numSCTRDOs!=0){
    double scale = 100./double(numSCTRDOs);
    m_firstHit->Fill( double(numFirstHit)*scale ,1.); m_secondHit->Fill( double(numSecondHit)*scale ,1.);
  }
  if(numSCTRDOs_ECp!=0 && m_doPositiveEndcap){
    double scale = 100./double(numSCTRDOs_ECp);
    m_firstHit_ECp->Fill( double(numFirstHit_ECp)*scale ,1.); m_secondHit_ECp->Fill( double(numSecondHit_ECp)*scale ,1.);
  }
  if(numSCTRDOs_ECm!=0 && m_doNegativeEndcap){
    double scale = 100./double(numSCTRDOs_ECm);
    m_firstHit_ECm->Fill( double(numFirstHit_ECm)*scale ,1.); m_secondHit_ECm->Fill( double(numSecondHit_ECm)*scale ,1.);
  }
  if(m_environment==AthenaMonManager::online){
    if ( m_numberOfEvents==1 or (m_numberOfEvents > 1 && (m_numberOfEvents % m_checkrate) == 0 )){
      if ( resetCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in resetCondDBMaps()" << endreq;
      if ( fillCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in fillCondDBMaps()" << endreq;
      if ( (m_numberOfEvents % (m_checkrate*10)) == 0 ) { 
        if ( resetConfigurationDetails().isFailure() )   msg(MSG::WARNING) << "Error in resetConfigurationDetails()" << endreq;
        if ( fillConfigurationDetails().isFailure() )   msg(MSG::WARNING) << "Error in fillConfigurationDetails()" << endreq;
      }
    }
  }
  if(m_environment==AthenaMonManager::online){
    if ( (m_current_lb % m_checkrecent == 0) && (m_current_lb>m_last_reset_lb) ) m_last_reset_lb=m_current_lb;
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                           SCTErrMonTool :: checkRateHists
//DD 17/08/2010
//Here we have the check of the histograms and we can build rate
//error rate histograms from the basic ones
//====================================================================================================
StatusCode  SCTErrMonTool::checkRateHists(){
	/** TO BE REPLACED WHEN NEW VERSION OF BASE CLASS IS AVAILABLE: **/
  bool isEndOfEventsBlock(endOfLumiBlock);
  const int nRegions(3);
  const int barrelRegion(0);
  if(m_environment!=AthenaMonManager::online){
    if(m_initialize){
      float content = 0; float cxb = 0; float cyb = 0; int evt = m_numberOfEvents; int evt_lumi = m_numberOfEventsLumi;
      for (int reg = barrelRegion ; reg !=nRegions; ++reg) {
        if( m_doPerLumiErrors && endOfLumiBlock ) {
          const int xbins=m_numErrorsPerLumi[reg]->GetNbinsX()+1;
          const int ybins=m_numErrorsPerLumi[reg]->GetNbinsY()+1;
          for (int xb(1); xb != xbins; ++xb) {
						cxb = m_numErrorsPerLumi[reg]->GetXaxis()->GetBinCenter(xb);
            for (int yb(1); yb !=ybins ; ++yb) {
              cyb = m_numErrorsPerLumi[reg]->GetYaxis()->GetBinCenter(yb);
              int num_modules = getNumModules(reg,yb-1);
              content = m_numErrorsPerLumi[reg]->GetBinContent(xb,yb);
              if(num_modules>0) {
                m_rateErrorsPerLumi[reg]->Fill(cxb,cyb,1,content);
                m_rateErrorsPerLumi[reg]->Fill(cxb,cyb,0,(evt_lumi*((double) num_modules) - content));
              }
            }
          }
        }
        int nlyr=(reg==barrelRegion)?(N_BARRELSx2):(N_ENDCAPSx2);
        for (int lyr = 0 ; lyr < nlyr; ++lyr) {
		  		const int xbins = m_pallErrs[SUMMARY][reg][lyr]->GetNbinsX()+1;
		  		const int ybins = m_pallErrs[SUMMARY][reg][lyr]->GetNbinsY()+1;
          for (int xb = 1; xb !=xbins ; ++xb) {
						cxb = m_pallErrs[SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb);
            for (int yb = 1; yb !=ybins ; ++yb) {
              cyb = m_pallErrs[SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb);
              if(isEndOfEventsBlock) {
								for (int errType(0);errType!=SUMMARY;++errType){
								  fillRateHistogram(m_pallErrs[errType][reg][lyr], m_allErrs[errType][reg][lyr], xb, yb, cxb, cyb, evt);
								}
              }
        
        // per lumi hists
              if( m_doPerLumiErrors and endOfLumiBlock and m_doErr2DPerLumiHists) {
								for (int errType(0);errType!=SUMMARY;++errType){
								  fillRateHistogram(m_pallErrsPerLumi[errType][reg][lyr], m_allErrsPerLumi[errType][reg][lyr], xb, yb, cxb, cyb, evt);
								}
                
              }
            }
          }
        }
      }

      if(isEndOfEventsBlock) {
        if ( resetCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in resetCondDBMaps()" << endreq;
        if ( fillCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in fillCondDBMaps()" << endreq;
        if ( resetConfigurationDetails().isFailure() )   msg(MSG::WARNING) << "Error in resetConfigurationDetails()" << endreq;
        if ( fillConfigurationDetails().isFailure() )  msg(MSG::WARNING) << "Error in fillConfigurationDetails()" << endreq;
      }
    } //mf moved brackets to allow online compilation
  }
  msg(MSG::INFO)<<"SCTErrMonTool, package version "<<PACKAGE_VERSION<<endreq;
  return StatusCode::SUCCESS;
}

// //====================================================================================================
//                    SCTErrMonTool :: procHistograms
//====================================================================================================
//StatusCode  SCTErrMonTool::procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool /*isEndOfRun*/){
StatusCode  SCTErrMonTool::procHistograms(){
/** TO BE REPLACED **/
bool endOfEventsBlock(endOfLumiBlock);
  if(endOfEventsBlock || endOfLumiBlock){
    ATH_MSG_DEBUG( "finalHists()" );
    ATH_MSG_DEBUG( "Total Rec Event Number: " << m_numberOfEvents );
    ATH_MSG_DEBUG( "Calling checkHists(true); true := end of run" );
    if (checkRateHists().isFailure())
      msg(MSG::WARNING) << "Error in checkRateHists()" << endreq ;
    ATH_MSG_DEBUG( "Exiting finalHists" );
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrorsHelper, Martin Flechl 10/09/2009                                                     
//====================================================================================================
int SCTErrMonTool::fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F* histo[NREGIONS_INC_GENERAL][N_DISKSx2], bool lumi2DHist, int err_type, bool b_MaskedLinks=false){
  int nerrors = 0;
  std::set<IdentifierHash>::iterator fit = errors->begin();
  std::set<IdentifierHash>::iterator fitEnd = errors->end();
  for (; fit != fitEnd; ++fit) {
    nerrors++;
    if (b_MaskedLinks) m_MaskedLinks->Fill(0);
    if(fit->is_valid()){
      Identifier fitId = m_pSCTHelper->wafer_id(*fit);
      int layer = m_pSCTHelper->layer_disk(fitId);
      int side = m_pSCTHelper->side(fitId);
      int barrel_ec = m_pSCTHelper->barrel_ec(fitId);
      int ieta(m_pSCTHelper->eta_module(fitId));
      int iphi(m_pSCTHelper->phi_module(fitId));
      layer = layer*2 + side;
      //why these different indexing systems?
      int regionIndex=iGEN;
      int maskIndex = 0;
      if (barrel_ec == BARREL && layer>=0 && layer < N_BARRELSx2) {
        regionIndex = iBARREL; 
        maskIndex = 2;
      }else if (barrel_ec == ENDCAP_A){
        regionIndex = iECp;
        maskIndex = 1;
      } else if (barrel_ec == ENDCAP_C) {
        regionIndex = iECm;
        maskIndex=3;
      }
      //
      if (b_MaskedLinks) m_MaskedLinks->Fill(maskIndex);
      histo[regionIndex][layer]->Fill(ieta,iphi);
      if(lumi2DHist) {
        if(m_doPerLumiErrors && m_doErr2DPerLumiHists) m_pallErrsPerLumi[SUMMARY][regionIndex][layer]->Fill(ieta,iphi);
      }else {
        if(m_doPerLumiErrors) m_numErrorsPerLumi[regionIndex]->Fill(err_type,layer);
        m_pallErrs[SUMMARY][regionIndex][layer]->Fill(ieta,iphi);
      }
    }
    
  }
  return nerrors;
}


//====================================================================================================
//          SCTErrMonTool :: numByteStreamErrors, Daniel Damiani 04/07/2011                                                     
//====================================================================================================
void SCTErrMonTool::numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc){
  for (auto fit: *errors) {
    if(fit.is_valid()){
      Identifier fitId = m_pSCTHelper->wafer_id(fit);
      int layer = m_pSCTHelper->layer_disk(fitId);
      int side = m_pSCTHelper->side(fitId);
      int barrel_ec = m_pSCTHelper->barrel_ec(fitId);
      layer = layer*2 + side;
      ntot++;
      if (barrel_ec == BARREL && layer>=0 && layer < N_BARRELSx2) {
        nbar++;
      } else if (barrel_ec == ENDCAP_A){
        neca++;
      }else if (barrel_ec == ENDCAP_C) {
        necc++;
      }
    }
  }
}

//====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrors
//====================================================================================================
StatusCode SCTErrMonTool::fillByteStreamErrors() {

  // Masked and ROB Fragment vs. lb
  const EventInfo* pEvent(0);
  if( evtStore()->retrieve(pEvent).isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve event info!" <<endreq;
    return StatusCode::FAILURE; 
  }
  unsigned int current_lb = pEvent->event_ID()->lumi_block();
  int maskedlink_errs[4]={0,0,0,0};
  int robfragment_errs[4]={0,0,0,0};
  int abcd_errs[4]={0,0,0,0};
  int raw_errs[4]={0,0,0,0};
  int timeout_errs[4]={0,0,0,0};
  int lvl1id_errs[4]={0,0,0,0};
  int bcid_errs[4]={0,0,0,0};
  int preamble_errs[4]={0,0,0,0};
  int formatter_errs[4]={0,0,0,0};
  int rodclock_errs[4]={0,0,0,0};
  int truncrod_errs[4]={0,0,0,0};
  int bsparse_errs[4]={0,0,0,0};
  int misslink_errs[4]={0,0,0,0};
  int tot_err[4]={0,0,0,0};
  int tot_mod_err[4]={0,0,0,0};
  int tot_mod_err_bar[4]={0,0,0,0};
  int tot_mod_err_eca[9]={0,0,0,0,0,0,0,0,0};
  int tot_mod_err_ecc[9]={0,0,0,0,0,0,0,0,0};

  if(current_lb!=m_previous_lb){
    for (int reg=0; reg<4;++reg){
      m_maskedLinkErrsMax[reg]=0;
      m_robFragmentErrsMax[reg]=0;
      m_abcdErrsMax[reg]=0;
      m_rawErrsMax[reg]=0;
      m_timeoutErrsMax[reg]=0;
      m_lvl1idErrsMax[reg]=0;
      m_bcidErrsMax[reg]=0;
      m_preambleErrsMax[reg]=0;
      m_formatterErrsMax[reg]=0;
      m_rodClockErrsMax[reg]=0;
      m_truncRodErrsMax[reg]=0;
      m_bsParseErrsMax[reg]=0;
      m_misslinkErrsMax[reg]=0;
      m_totErrsMax[reg]=0;
      m_totModErrsMax[reg]=0;
    }
  }

  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::MaskedLink), maskedlink_errs[3],maskedlink_errs[0],maskedlink_errs[1],maskedlink_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError), robfragment_errs[3],robfragment_errs[0],robfragment_errs[1],robfragment_errs[2]);
  
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::ABCDError), abcd_errs[3],abcd_errs[0],abcd_errs[1],abcd_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::RawError), raw_errs[3],raw_errs[0],raw_errs[1],raw_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::TimeOutError), timeout_errs[3],timeout_errs[0],timeout_errs[1],timeout_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::LVL1IDError), lvl1id_errs[3],lvl1id_errs[0],lvl1id_errs[1],lvl1id_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::BCIDError), bcid_errs[3],bcid_errs[0],bcid_errs[1],bcid_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::PreambleError), preamble_errs[3],preamble_errs[0],preamble_errs[1],preamble_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::FormatterError), formatter_errs[3],formatter_errs[0],formatter_errs[1],formatter_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::RODClockError), rodclock_errs[3],rodclock_errs[0],rodclock_errs[1],rodclock_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::TruncatedROD), truncrod_errs[3],truncrod_errs[0],truncrod_errs[1],truncrod_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::ByteStreamParseError), bsparse_errs[3],bsparse_errs[0],bsparse_errs[1],bsparse_errs[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::MissingLinkHeaderError), misslink_errs[3], misslink_errs[0], misslink_errs[1], misslink_errs[2]);
  for (int reg=0; reg<4;++reg) {
    m_MaskedLinksVsLB[reg]->Fill(current_lb,double (maskedlink_errs[reg]));
    m_ROBFragmentVsLB[reg]->Fill(current_lb,double (robfragment_errs[reg]));
    m_ABCDVsLB[reg]->Fill(current_lb,double (abcd_errs[reg]));
    m_RawErrsVsLB[reg]->Fill(current_lb,double (raw_errs[reg]));
    m_TimeOutVsLB[reg]->Fill(current_lb,double (timeout_errs[reg]));
    m_LVL1IDVsLB[reg]->Fill(current_lb,double (lvl1id_errs[reg]));
    m_BCIDVsLB[reg]->Fill(current_lb,double (bcid_errs[reg]));
    m_PreambleVsLB[reg]->Fill(current_lb,double (preamble_errs[reg]));
    m_FormatterVsLB[reg]->Fill(current_lb,double (formatter_errs[reg]));
    m_RODClockVsLB[reg]->Fill(current_lb,double (rodclock_errs[reg]));
    m_TruncRODVsLB[reg]->Fill(current_lb,double (truncrod_errs[reg]));
    m_BSParseVsLB[reg]->Fill(current_lb,double (bsparse_errs[reg]));
    m_MissingLinkHeaderVsLB[reg]->Fill(current_lb,double (misslink_errs[reg]));
    
    if(m_maskedLinkErrsMax[reg]<maskedlink_errs[reg]){
      m_maskedLinkErrsMax[reg]=maskedlink_errs[reg];
      m_MaxMaskedLinksVsLB[reg]->SetBinContent((int)current_lb,maskedlink_errs[reg]);
      // m_2d_MaxMaskedLinksVsLB[reg]->SetBinContent((int)current_lb,m_maskedLinkErrsMax[reg]+1,1);
    }
    if(m_robFragmentErrsMax[reg]<robfragment_errs[reg]){
      m_robFragmentErrsMax[reg]=robfragment_errs[reg];
      m_MaxROBFragmentVsLB[reg]->SetBinContent((int)current_lb,robfragment_errs[reg]);
      // m_2d_MaxROBFragmentVsLB[reg]->SetBinContent((int)current_lb,m_robFragmentErrsMax[reg]+1,1); 
    }
    if(m_abcdErrsMax[reg]<abcd_errs[reg]){
      m_abcdErrsMax[reg]=abcd_errs[reg];
      m_MaxABCDVsLB[reg]->SetBinContent((int)current_lb,m_abcdErrsMax[reg]);
      // m_2d_MaxABCDVsLB[reg]->SetBinContent((int)current_lb,m_abcdErrsMax[reg]+1,1); 
    }
    if(m_rawErrsMax[reg]<raw_errs[reg]){
      m_rawErrsMax[reg]=raw_errs[reg];
      m_MaxRawErrsVsLB[reg]->SetBinContent((int)current_lb,m_rawErrsMax[reg]);
      // m_2d_MaxRawErrsVsLB[reg]->SetBinContent((int)current_lb,m_rawErrsMax[reg]+1,1); 
    }
    if(m_timeoutErrsMax[reg]<timeout_errs[reg]){
      m_timeoutErrsMax[reg]=timeout_errs[reg];
      m_MaxTimeOutVsLB[reg]->SetBinContent((int)current_lb,m_timeoutErrsMax[reg]);
      // m_2d_MaxTimeOutVsLB[reg]->SetBinContent((int)current_lb,m_timeoutErrsMax[reg]+1,1); 
    }
    if(m_lvl1idErrsMax[reg]<lvl1id_errs[reg]){
      m_lvl1idErrsMax[reg]=lvl1id_errs[reg];
      m_MaxLVL1IDVsLB[reg]->SetBinContent((int)current_lb,m_lvl1idErrsMax[reg]);
      // m_2d_MaxLVL1IDVsLB[reg]->SetBinContent((int)current_lb,m_lvl1idErrsMax[reg]+1,1); 
    }
    if(m_bcidErrsMax[reg]<bcid_errs[reg]){
      m_bcidErrsMax[reg]=bcid_errs[reg];
      m_MaxBCIDVsLB[reg]->SetBinContent((int)current_lb,m_bcidErrsMax[reg]);
      // m_2d_MaxBCIDVsLB[reg]->SetBinContent((int)current_lb,m_bcidErrsMax[reg]+1,1); 
    }
    if(m_preambleErrsMax[reg]<preamble_errs[reg]){
      m_preambleErrsMax[reg]=preamble_errs[reg];
      m_MaxPreambleVsLB[reg]->SetBinContent((int)current_lb,m_preambleErrsMax[reg]);
      // m_2d_MaxPreambleVsLB[reg]->SetBinContent((int)current_lb,m_preambleErrsMax[reg]+1,1); 
    }
    if(m_formatterErrsMax[reg]<formatter_errs[reg]){
      m_formatterErrsMax[reg]=formatter_errs[reg];
      m_MaxFormatterVsLB[reg]->SetBinContent((int)current_lb,m_formatterErrsMax[reg]);
      // m_2d_MaxFormatterVsLB[reg]->SetBinContent((int)current_lb,m_formatterErrsMax[reg]+1,1); 
    }
    if(m_rodClockErrsMax[reg]<rodclock_errs[reg]){
      m_rodClockErrsMax[reg]=rodclock_errs[reg];
      m_MaxRODClockVsLB[reg]->SetBinContent((int)current_lb,m_rodClockErrsMax[reg]);
      // m_2d_MaxRODClockVsLB[reg]->SetBinContent((int)current_lb,m_rodClockErrsMax[reg]+1,1); 
    }
    if(m_truncRodErrsMax[reg]<truncrod_errs[reg]){
      m_truncRodErrsMax[reg]=truncrod_errs[reg];
      m_MaxTruncRODVsLB[reg]->SetBinContent((int)current_lb,m_truncRodErrsMax[reg]);
      // m_2d_MaxTruncRODVsLB[reg]->SetBinContent((int)current_lb,m_truncRodErrsMax[reg]+1,1); 
    }
    if(m_bsParseErrsMax[reg]<bsparse_errs[reg]){
      m_bsParseErrsMax[reg]=bsparse_errs[reg];
      m_MaxBSParseVsLB[reg]->SetBinContent((int)current_lb,m_bsParseErrsMax[reg]);
      // m_2d_MaxBSParseVsLB[reg]->SetBinContent((int)current_lb,m_bsParseErrsMax[reg]+1,1); 
    }
    if(m_misslinkErrsMax[reg]<misslink_errs[reg]){
      m_misslinkErrsMax[reg]=misslink_errs[reg];
      m_MaxMissingLinkHeaderVsLB[reg]->SetBinContent((int)current_lb,m_misslinkErrsMax[reg]);
      // m_2d_MaxMissingLinkHeaderVsLB[reg]->SetBinContent((int)current_lb,m_misslinkErrsMax[reg]+1,1); 
    }
  }

  m_MaskedLinks->Reset();
  int total_errors = 0; float cxb = 0; float cyb = 0; 
    
  int evt = m_numberOfEvents; int content = 0;

    
  if(m_environment==AthenaMonManager::online){
    if( (m_current_lb % m_checkrecent == 0) && (m_current_lb>m_last_reset_lb) ) {
      for (int reg=0; reg!=NREGIONS_INC_GENERAL;++reg) for (int i=0; i!=N_DISKSx2;++i) if (m_summaryErrsRecent[reg][i]) m_summaryErrsRecent[reg][i]->Reset();
    }
  }
  //reset histos
  for (int reg=0; reg!=NREGIONS_INC_GENERAL;++reg) for (int i=0; i!=N_DISKSx2;++i) if (m_pallErrs[SUMMARY][reg][i]) m_pallErrs[SUMMARY][reg][i]->Reset();
  //set up which errors we get; the SCT_ByteStreamErrors indexing does not correspond to the numbering in this class, so we translate
  const int errorsToGet[N_ERRTYPES]={SCT_ByteStreamErrors::ABCDError, SCT_ByteStreamErrors::RawError, SCT_ByteStreamErrors::TimeOutError,
    SCT_ByteStreamErrors::LVL1IDError, SCT_ByteStreamErrors::BCIDError, SCT_ByteStreamErrors::FormatterError,
    SCT_ByteStreamErrors::PreambleError, SCT_ByteStreamErrors::MaskedLink,SCT_ByteStreamErrors::RODClockError,
				     SCT_ByteStreamErrors::TruncatedROD,SCT_ByteStreamErrors::ROBFragmentError,SCT_ByteStreamErrors::ByteStreamParseError,
				     SCT_ByteStreamErrors::MissingLinkHeaderError
  };
  for (int errType(0);errType!=SUMMARY;++errType){
    const bool mask=(errorsToGet[errType]==SCT_ByteStreamErrors::MaskedLink);
    total_errors += fillByteStreamErrorsHelper(m_byteStreamErrSvc->getErrorSet(errorsToGet[errType]), m_pallErrs[errType], false,errType,mask);
  }
 
   
  if(m_doPerLumiErrors && m_doErr2DPerLumiHists) {
    for (int reg=0; reg!=NREGIONS_INC_GENERAL;++reg) for (int i=0; i!=N_DISKSx2;++i) if (m_pallErrsPerLumi[SUMMARY][reg][i]) m_pallErrsPerLumi[SUMMARY][reg][i]->Reset();
    for (int errType(0);errType!=SUMMARY;++errType){
      const bool mask=(errorsToGet[errType]==SCT_ByteStreamErrors::MaskedLink);
      total_errors += fillByteStreamErrorsHelper(m_byteStreamErrSvc->getErrorSet(errorsToGet[errType]), m_pallErrsPerLumi[errType], false,errType,mask);
    }
  }

  for (int reg = 0 ; reg != 3; ++reg) {
    int nlyr=(reg==BARREL)?(N_BARRELSx2):(N_ENDCAPSx2);
    for (int lyr = 0 ; lyr != nlyr; ++lyr) {
      if(m_environment==AthenaMonManager::online){
        if (m_numberOfEvents%m_checkrate == 0 && m_runOnline == true) {
          for (int errType(0);errType!=SUMMARY;++errType){
            m_allErrs[errType][reg][lyr]->Reset();
          }
        }
      }
      const int nBinsX(m_pallErrs[SUMMARY][reg][lyr]->GetNbinsX()+1);
      const int nBinsY(m_pallErrs[SUMMARY][reg][lyr]->GetNbinsY()+1);
      for (int xb = 1; xb !=nBinsX; ++xb) {
	cxb = m_pallErrs[SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb);
        for (int yb = 1; yb !=nBinsY; ++yb) {
          cyb = m_pallErrs[SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb);
          // Fill values for vs. lb plots
          tot_err[reg]+=m_pallErrs[SUMMARY][reg][lyr]->GetBinContent(xb,yb);
          if (m_pallErrs[SUMMARY][reg][lyr]->GetBinContent(xb,yb) > 0) {
            m_allErrs[SUMMARY][reg][lyr]->Fill(cxb,cyb,1);
            if(m_environment==AthenaMonManager::online) m_summaryErrsRecent[reg][lyr]->Fill(cxb,cyb,1);
            tot_mod_err[reg]++;
            if(reg==0) tot_mod_err_bar[lyr]++;
            if(reg==1) tot_mod_err_eca[lyr]++;
            if(reg==2) tot_mod_err_ecc[lyr]++;
          }else {
            m_allErrs[SUMMARY][reg][lyr]->Fill(cxb,cyb,0);
            if(m_environment==AthenaMonManager::online) m_summaryErrsRecent[reg][lyr]->Fill(cxb,cyb,0);
          }
          if(m_doPerLumiErrors && m_doErr2DPerLumiHists) {
            if (m_pallErrsPerLumi[SUMMARY][reg][lyr]->GetBinContent(xb,yb) > 0) m_allErrsPerLumi[SUMMARY][reg][lyr]->Fill(cxb,cyb,1);
            else m_allErrsPerLumi[SUMMARY][reg][lyr]->Fill(cxb,cyb,0);
          }
	  
          if(m_environment==AthenaMonManager::online){
        //mf cast to (int) below to avoid compiler warnings... we do want int, right? Some code duplication below...
            if (m_numberOfEvents%m_checkrate == 0 && m_runOnline == true) {
              for (int errType(0);errType!=SUMMARY;++errType){
                content = (int)m_pallErrs[errType][reg][lyr]->GetBinContent(xb,yb);
                m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 1, content);
                m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 0, evt - content);
              }
            }
          }
        }
      }
    }
  }
  tot_err[3] = tot_err[0]+tot_err[1]+tot_err[2];
  tot_mod_err[3] = tot_mod_err[0]+tot_mod_err[1]+tot_mod_err[2];
  for (int reg=0; reg<4;++reg) {
    if(m_totErrsMax[reg]<tot_err[reg]){
      m_totErrsMax[reg]=tot_err[reg];
      m_MaxNumberOfErrorsVsLB[reg]->SetBinContent((int)current_lb,(tot_err[reg]));
      // m_2d_MaxNumberOfErrorsVsLB[reg]->SetBinContent((int)current_lb,m_totErrsMax[reg]+1,1); 
    }
    m_NumberOfErrorsVsLB[reg]->Fill((int)current_lb,(tot_err[reg]));
    if(m_totModErrsMax[reg]<tot_mod_err[reg]){
      m_totModErrsMax[reg]=tot_mod_err[reg];
      m_MaxModulesWithErrorsVsLB[reg]->SetBinContent((int)current_lb,(tot_mod_err[reg]));
      // m_2d_MaxModulesWithErrorsVsLB[reg]->SetBinContent((int)current_lb,m_totModErrsMax[reg]+1,1); 
    }
    m_ModulesWithErrorsVsLB[reg]->Fill((int)current_lb,(tot_mod_err[reg]));
    int nlyr = (reg==0)? 4:9;
    for (int lyr=0; lyr<nlyr; lyr++) {
      if(reg==0) m_LinksWithErrorsVsLBBarrel[lyr]->Fill((int)current_lb,(tot_mod_err_bar[lyr]));
      if(reg==1) m_LinksWithErrorsVsLBEndcapA[lyr]->Fill((int)current_lb,(tot_mod_err_eca[lyr]));
      if(reg==2) m_LinksWithErrorsVsLBEndcapC[lyr]->Fill((int)current_lb,(tot_mod_err_ecc[lyr]));
    }
  }

  //  if(m_environment==AthenaMonManager::online){
  // Time Dependent SP plots only online
  nErrors_buf[nErrors_pos] = total_errors;
  nLinksWithErrors_buf[nErrors_pos] = tot_mod_err[3];
  nErrors_pos++;
  if (nErrors_pos == m_evtsbins) nErrors_pos = 0;
  if (m_numberOfEvents % m_checkrate == 0) {
    m_nErrors->Reset(); Int_t latest_nErrors_pos = nErrors_pos;
    m_nLinksWithErrors->Reset();
    for (Int_t i=1; i < m_evtsbins; i++) {
      if (latest_nErrors_pos == m_evtsbins) latest_nErrors_pos = 0;
      if (m_numberOfEvents < m_evtsbins){
	if (i < nErrors_pos) {
	  m_nErrors->SetBinContent(i,nErrors_buf[i]);
	  m_nLinksWithErrors->SetBinContent(i,nLinksWithErrors_buf[i]);
	}else {
	  m_nErrors->SetBinContent(i,0);
	  m_nLinksWithErrors->SetBinContent(i,0);
	}
      } else {
	m_nErrors->SetBinContent(i,nErrors_buf[latest_nErrors_pos]);
	m_nLinksWithErrors->SetBinContent(i,nLinksWithErrors_buf[latest_nErrors_pos]);
	m_nErrors->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
	m_nLinksWithErrors->GetXaxis()->Set(m_evtsbins,m_numberOfEvents-m_evtsbins,m_numberOfEvents);
      }
      latest_nErrors_pos++; 
      if (latest_nErrors_pos == m_evtsbins) latest_nErrors_pos = 0;
    }
  }
  //}5.12.2014
  m_previous_lb = current_lb;
  m_numberOfEvents++;
  m_numberOfEventsLumi++;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper                                               
// Avoids duplicate code in the bookErrHistosXXXX functions; added 8/9/09, Martin Flechl                                                                
//====================================================================================================
StatusCode SCTErrMonTool::bookErrHistosHelper(MonGroup & mg, TString name, TString title, TString titlehitmap, TProfile2D* &tprof, TH2F* &th, const int layer, const bool barrel){
  ostringstream streamhitmap; streamhitmap << layer/2 << "_" << layer%2;
  name+=streamhitmap.str();  title+=streamhitmap.str();  titlehitmap+=streamhitmap.str();

  if (barrel){
    const float xlo=FIRST_ETA_BIN-0.5;
    const float xhi=LAST_ETA_BIN+0.5;
    const float ylo=FIRST_PHI_BIN-0.5;
    const float yhi=LAST_PHI_BIN+0.5;
    tprof = new TProfile2D(name, title,N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
    th = new TH2F(titlehitmap, title, N_ETA_BINS, xlo, xhi,N_PHI_BINS, ylo, yhi);
  } else{
    const float xlo=FIRST_ETA_BIN_EC-0.5;
    const float xhi=LAST_ETA_BIN_EC+0.5;
    const float ylo=FIRST_PHI_BIN_EC-0.5;
    const float yhi=LAST_PHI_BIN_EC+0.5;
    tprof = new TProfile2D(name, title,N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
    th = new TH2F(titlehitmap, title,N_ETA_BINS_EC, xlo, xhi,N_PHI_BINS_EC, ylo, yhi);
  }
  StatusCode sc0 = mg.regHist(th);
  StatusCode sc1 = mg.regHist(tprof);
  if (sc0.isFailure() or sc1.isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper                                               
// Avoids duplicate code in the bookErrHistosXXXX functions; added 08/08/11, Daniel Damiani                                                                
//====================================================================================================
StatusCode SCTErrMonTool::bookErrHistosHelper(MonGroup & mg, TString name, TString title, TProfile2D* &tprof, const int layer, const bool barrel){
  ostringstream streamhitmap; streamhitmap << layer/2 << "_" << layer%2;
  name+=streamhitmap.str();  title+=streamhitmap.str();
  if (barrel){
    tprof = new TProfile2D(name, title,N_ETA_BINS, FIRST_ETA_BIN-0.5, LAST_ETA_BIN+0.5,N_PHI_BINS, FIRST_PHI_BIN-0.5, LAST_PHI_BIN+0.5);
  } else{
    tprof = new TProfile2D(name, title,N_ETA_BINS_EC, FIRST_ETA_BIN_EC-0.5, LAST_ETA_BIN_EC+0.5,N_PHI_BINS_EC, FIRST_PHI_BIN_EC-0.5, LAST_PHI_BIN_EC+0.5);
  }
  
  StatusCode sc = mg.regHist(tprof);
  if (sc.isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//====================================================================================================

//                       SCTErrMonTool :: bookErrHistos
// Book 1D and 2D Histograms of errors
//====================================================================================================
//StatusCode SCTErrMonTool::bookErrHistos(bool newRun, bool newLumiBlock){
StatusCode SCTErrMonTool::bookErrHistos(){
  const std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse","MissingLink","summary"};
  std::string m_errorsNamesMG[] = {"SCT/SCTB/errors","SCT/SCTB/errors","SCT/SCTB/errors","SCT/SCTB/errors/LVL1ID","SCT/SCTB/errors/BCID","SCT/SCTB/errors/Preamble","SCT/SCTB/errors/Formatter","SCT/SCTB/errors/MaskedLink","SCT/SCTB/errors/RODClock","SCT/SCTB/errors/TruncROD","SCT/SCTB/errors/ROBFrag","SCT/SCTB/errors","SCT/SCTB/errors","SCT/SCTB/errors"};
  if(m_doPerLumiErrors) {
    MonGroup lumiErr(this,"SCT/SCTB/errors",lumiBlock,ATTRIB_UNMANAGED );
    if(ManagedMonitorToolBase::newLumiBlock) {
      std::string m_layerNames[] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1"};
      m_numErrorsPerLumi[iBARREL] = new TH2F("NumErrsPerLumi","Total Number of Error Types for Layer per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_BARRELSx2,-0.5,N_BARRELSx2-0.5);
      if(lumiErr.regHist(m_numErrorsPerLumi[iBARREL]).isFailure()) msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endreq;
      m_rateErrorsPerLumi[iBARREL] = new TProfile2D("RateErrorsPerLumi","Rate of Error Types for Layers per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_BARRELSx2,-0.5,N_BARRELSx2-0.5);
      if(lumiErr.regHist(m_rateErrorsPerLumi[iBARREL]).isFailure()) msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endreq;
      for (unsigned int bin(0); bin<n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iBARREL]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iBARREL]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin<N_BARRELSx2; bin++) {
        m_numErrorsPerLumi[iBARREL]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iBARREL]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
      }
      //int limit = 2*n_barrels;
      bool somethingFailed(false);
      if(m_doErr2DPerLumiHists) {
        for (int layer(0); layer!=N_BARRELSx2;++layer) {
          for (int errType(0);errType!=N_ERRTYPES;++errType){
	    MonGroup lumiErr2(this,m_errorsNamesMG[errType],lumiBlock,ATTRIB_UNMANAGED );
            std::string name1 = m_errorsNames[errType] + "ErrsPerLumi";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsPerLumi_";
            somethingFailed |=bookErrHistosHelper(lumiErr2,name1,title,name2,m_allErrsPerLumi[errType][iBARREL][layer],m_pallErrsPerLumi[errType][iBARREL][layer],layer).isFailure();
          }
        }
      }
      if (somethingFailed) return StatusCode::FAILURE;
    }
  }
  MonGroup err(this,"SCT/SCTB/errors",run,ATTRIB_UNMANAGED );
  if(ManagedMonitorToolBase::newRun){
      MonGroup MaskErrs(this,"SCT/GENERAL/errors",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
      m_MaskedLinks = new TH1I("Masked Links","Number of Masked Links for SCT,ECA,B,ECC",4,-0.5,3.5); //should reorder to C,B,A, total
      m_MaskedLinks->GetXaxis()->SetBinLabel(1,"All"); 
      m_MaskedLinks->GetXaxis()->SetBinLabel(2,"EndCapA");
      m_MaskedLinks->GetXaxis()->SetBinLabel(3,"Barrel"); 
      m_MaskedLinks->GetXaxis()->SetBinLabel(4,"EndCapC");
      if(MaskErrs.regHist(m_MaskedLinks).isFailure())  msg(MSG::WARNING) << "Couldn't book MaskedLinks" << endreq;
      std::string stem=m_stream+"/SCT/SCTB/errors/" ;   
      int nbins=50;
      //Book errors vs event numbers
      MonGroup Errors(this,"SCT/GENERAL/errors",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
      m_nErrors = new TH1I("sct_errors_vs_en","Number of Errors vs Event Number",m_evtsbins,1,m_evtsbins+1);
      m_nErrors->GetXaxis()->SetTitle("Event Number"); 
      m_nErrors->GetYaxis()->SetTitle("Num of Errors"); 
      m_nLinksWithErrors = new TH1I("sct_links_with_errors_vs_en","Number of Links with Errors vs Event Number",m_evtsbins,1,m_evtsbins+1);
      m_nLinksWithErrors->GetXaxis()->SetTitle("Event Number"); 
      m_nLinksWithErrors->GetYaxis()->SetTitle("Num of Links with Errors"); 
      size_t nErrors_buf_size;
      nErrors_buf_size = m_evtsbins * sizeof (int);
      nErrors_buf = (int *) malloc (nErrors_buf_size);
      nLinksWithErrors_buf = (int *) malloc (nErrors_buf_size);
      nErrors_pos = 0;
      if(Errors.regHist(m_nErrors).isFailure())    msg(MSG::WARNING) << "Couldn't book nErrors vs event number hist" << endreq;
      if(Errors.regHist(m_nLinksWithErrors).isFailure())    msg(MSG::WARNING) << "Couldn't book nLinksWithErrors vs event number hist" << endreq;
      //Book percentage error histograms      
      m_firstHit = new TH1F("FirstHit","Percentage of FirstHit errors", nbins,0.,100.);
      m_firstHit->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
      m_secondHit = new TH1F("SecondHit","Percentage of SecondHit errors", nbins,0.,100.);
      m_secondHit->GetXaxis()->SetTitle("Percentage of SecondHit errors");
      if( err.regHist(m_firstHit).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
      if( err.regHist(m_secondHit).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;
      bool somethingFailed(false);
      for (int layer(0); layer!=N_BARRELSx2;++layer) {
        for (int errType(0);errType!=N_ERRTYPES;++errType){
	  MonGroup err2(this,m_errorsNamesMG[errType],run,ATTRIB_UNMANAGED );
          std::string name1 = m_errorsNames[errType] + "Errs_";
          std::string title = m_errorsNames[errType] + " errors layer ";
          std::string name2 = std::string("T") + m_errorsNames[errType] + "Errs_";
          somethingFailed |=bookErrHistosHelper(err2,name1,title,name2,m_allErrs[errType][iBARREL][layer],m_pallErrs[errType][iBARREL][layer],layer).isFailure();
          m_allErrs[errType][iBARREL][layer]->GetXaxis()->SetTitle("Index in the direction of #eta"); 
          m_allErrs[errType][iBARREL][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
        }
        if(m_environment==AthenaMonManager::online) somethingFailed |= bookErrHistosHelper(err,"summaryErrsRecent_","summary recent Layer ",m_summaryErrsRecent[iBARREL][layer],layer).isFailure();
      }
      if (somethingFailed) return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                 SCTErrMonTool :: bookPositiveEndCapErrHistos
// Book 1D and 2D Histograms of errors for positive endcap
//====================================================================================================
StatusCode SCTErrMonTool::bookPositiveEndCapErrHistos(){
  std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse","MissingLink", "summary"};
  std::string m_errorsNamesMG[] = {"SCT/SCTEA/errors","SCT/SCTEA/errors","SCT/SCTEA/errors","SCT/SCTEA/errors/LVL1ID","SCT/SCTEA/errors/BCID","SCT/SCTEA/errors/Preamble","SCT/SCTEA/errors/Formatter","SCT/SCTEA/errors/MaskedLink","SCT/SCTEA/errors/RODClock","SCT/SCTEA/errors/TruncROD","SCT/SCTEA/errors/ROBFrag","SCT/SCTEA/errors","SCT/SCTEA/errors","SCT/SCTEA/errors"};//07.01.2015
  if(m_doPerLumiErrors) {
    MonGroup lumiErr(this,"SCT/SCTEA/errors",lumiBlock,ATTRIB_UNMANAGED );
    if(ManagedMonitorToolBase::newLumiBlock) {
      std::string m_layerNames[N_DISKSx2] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
      m_numErrorsPerLumi[iECp] = new TH2F("NumErrsPerLumi","Total Number of Error Types for Disk per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_numErrorsPerLumi[iECp]).isFailure()) msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endreq;
      m_rateErrorsPerLumi[iECp] = new TProfile2D("RateErrorsPerLumi","Rate of Error Types for Disks per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_rateErrorsPerLumi[iECp]).isFailure())  msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endreq;
      for (unsigned int bin(0); bin<n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iECp]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iECp]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin!=N_ENDCAPSx2; ++bin) {
        m_numErrorsPerLumi[iECp]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iECp]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
      }
      bool failedBooking(false);
      if(m_doErr2DPerLumiHists) {
        for (int layer(0); layer!=N_DISKSx2;++layer) {
          for (int errType(0);errType!=N_ERRTYPES;++errType){
	    MonGroup lumiErr2(this,m_errorsNamesMG[errType],lumiBlock,ATTRIB_UNMANAGED );//07.01.2015
            std::string name1 = m_errorsNames[errType] + "ErrsECpPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock Disk ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECpPerLumi_";
            failedBooking |=bookErrHistosHelper(lumiErr2,name1,title,name2,m_allErrsPerLumi[errType][iECp][layer],m_pallErrsPerLumi[errType][iECp][layer],layer).isFailure();
          }
        }
      }
      if(failedBooking) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Something went wrong in bookPositiveEndCapErrHistos lumi part" << endreq;
    }
  }
  if(ManagedMonitorToolBase::newRun){
    MonGroup err(this,"SCT/SCTEA/errors",run,ATTRIB_UNMANAGED );
    std::string stem=m_stream+"/SCT/SCTEA/errors/" ;
    int nbins=50;
    m_firstHit_ECp = new TH1F("FirstHit_ECp","Percentage of FirstHit errors in positive endcap", nbins,0.,100.);
    m_firstHit_ECp->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
    m_secondHit_ECp = new TH1F("SecondHit_ECp","Percentage of SecondHit errors in positive endcap", nbins,0.,100.);
    m_secondHit_ECp->GetXaxis()->SetTitle("Percentage of SecondHit errors"); 
    if ( err.regHist(m_firstHit_ECp).isFailure() )
      msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
    if ( err.regHist(m_secondHit_ECp).isFailure() )
      msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;

    bool failedBooking(false); 
    for (int layer(0); layer!=N_DISKSx2;++layer) {
      for (int errType(0);errType!=N_ERRTYPES;++errType){
	MonGroup err2(this,m_errorsNamesMG[errType],run,ATTRIB_UNMANAGED ); 
        std::string name1 = m_errorsNames[errType] + "ErrsECp_";
        std::string title = m_errorsNames[errType] + " errors Disk ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECp_";
        failedBooking |=bookErrHistosHelper(err2,name1,title,name2,m_allErrs[errType][iECp][layer],m_pallErrs[errType][iECp][layer],layer,false).isFailure();
        m_allErrs[errType][iECp][layer]->GetXaxis()->SetTitle("Index in the direction of #eta"); 
        m_allErrs[errType][iECp][layer]->GetYaxis()->SetTitle("Index in the direction of #phi"); 
      }

      if(m_environment==AthenaMonManager::online) failedBooking |= bookErrHistosHelper(err,"summaryErrsRecentECp_","summary recent Disk ",m_summaryErrsRecent[iECp][layer],layer,false).isFailure();
    }
    if(failedBooking) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Something went wrong in bookPositiveEndCapErrHistos" << endreq;
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                 SCTErrMonTool :: bookNegativeEndCapErrHistos
// Book 1D and 2D Histograms of errors for negative endcap
//====================================================================================================
//StatusCode SCTErrMonTool::bookNegativeEndCapErrHistos(bool isNewRun, bool isNewLumiBlock){
StatusCode SCTErrMonTool::bookNegativeEndCapErrHistos(){
  std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse","MissingLink", "summary"};
  std::string m_errorsNamesMG[] = {"SCT/SCTEC/errors","SCT/SCTEC/errors","SCT/SCTEC/errors","SCT/SCTEC/errors/LVL1ID","SCT/SCTEC/errors/BCID","SCT/SCTEC/errors/Preamble","SCT/SCTEC/errors/Formatter","SCT/SCTEC/errors/MaskedLink","SCT/SCTEC/errors/RODClock","SCT/SCTEC/errors/TruncROD","SCT/SCTEC/errors/ROBFrag","SCT/SCTEC/errors","SCT/SCTEC/errors","SCT/SCTEC/errors"};//07.01.2015
  if(m_doPerLumiErrors) {
    MonGroup lumiErr(this,"SCT/SCTEC/errors",lumiBlock,ATTRIB_UNMANAGED );
    if(ManagedMonitorToolBase::newLumiBlock) {
      //std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse"};
      std::string m_layerNames[N_DISKSx2] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
      m_numErrorsPerLumi[iECm] = new TH2F("NumErrsPerLumi","Total Number of Error Types for Disk per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_numErrorsPerLumi[iECm]).isFailure()) msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endreq;
      m_rateErrorsPerLumi[iECm] = new TProfile2D("RateErrorsPerLumi","Rate of Error Types for Disks per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_rateErrorsPerLumi[iECm]).isFailure()) msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endreq;
      for (unsigned int bin(0); bin<n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iECm]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iECm]->GetXaxis()->SetBinLabel(bin+1,m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin<N_ENDCAPSx2; bin++) {
        m_numErrorsPerLumi[iECm]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iECm]->GetYaxis()->SetBinLabel(bin+1,m_layerNames[bin].c_str());
      }
      bool failedBooking(false);
      if(m_doErr2DPerLumiHists) {
        for (int layer(0); layer!=N_DISKSx2;++layer) {
          for (int errType(0);errType!=N_ERRTYPES;++errType){
	    MonGroup lumiErr2(this,m_errorsNamesMG[errType],lumiBlock,ATTRIB_UNMANAGED );//07.01.2015
            std::string name1 = m_errorsNames[errType] + "ErrsECmPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECmPerLumi_";
            failedBooking |=bookErrHistosHelper(lumiErr2,name1,title,name2,m_allErrsPerLumi[errType][iECm][layer],m_pallErrsPerLumi[errType][iECm][layer],layer).isFailure();
          }
        }
      }
      if(failedBooking and msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Something went wrong in bookNegativeEndCapErrHistos lumi part" << endreq;
    }
  }
  if(ManagedMonitorToolBase::newRun){
    MonGroup err(this,"SCT/SCTEC/errors",run,ATTRIB_UNMANAGED );
    std::string stem=m_stream+"/SCT/SCTEC/errors/" ;
    int nbins=50;
    m_firstHit_ECm = new TH1F("FirstHit_ECm","Percentage of FirstHit errors in negative endcap", nbins,0.,100.);
    m_firstHit_ECm->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
    m_secondHit_ECm = new TH1F("SecondHit_ECm","Percentage of SecondHit errors in negative endcap", nbins,0.,100.);
    m_secondHit_ECm->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if ( err.regHist(m_firstHit_ECm).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
    if ( err.regHist(m_secondHit_ECm).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;

    //int limit = 2*n_disks;
    bool failedBooking(false);
    for (int layer(0); layer!=N_DISKSx2;++layer) {
      for (int errType(0);errType!=N_ERRTYPES;++errType){
	MonGroup err2(this,m_errorsNamesMG[errType],run,ATTRIB_UNMANAGED );
        std::string name1 = m_errorsNames[errType] + "ErrsECm_";
        std::string title = m_errorsNames[errType] + " errors Disk ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECm_";
        failedBooking |=bookErrHistosHelper(err2,name1,title,name2,m_allErrs[errType][iECm][layer],m_pallErrs[errType][iECm][layer],layer,false).isFailure();
        m_allErrs[errType][iECm][layer]->GetXaxis()->SetTitle("Index in the direction of #eta"); 
        m_allErrs[errType][iECm][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
      }
      if(m_environment==AthenaMonManager::online) failedBooking |= bookErrHistosHelper(err,"summaryErrsRecentECm_","summary recent Disk ",m_summaryErrsRecent[iECm][layer],layer,false).isFailure();
    }
    if(failedBooking) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Something went wrong in bookNegativeEndCapErrHistos" << endreq;
     
  }
  return StatusCode::SUCCESS;
}
//====================================================================================================
//                         SCTErrMonTool :: bookConfMaps
//====================================================================================================
StatusCode  SCTErrMonTool::bookConfMaps(){
  if(ManagedMonitorToolBase::newRun){
    MonGroup ConfMapsExpert(this,"SCT/SCTB/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    MonGroup ConfHist(this,"SCT/GENERAL/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );

    MonGroup ConfMaps(this,"SCT/SCTB/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    MonGroup ConfHistECA(this,"SCT/SCTEA/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    MonGroup ConfHistECC(this,"SCT/SCTEC/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    MonGroup ConfHist2D(this,"SCT/GENERAL/errors/tmp",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED);

    //    std::string streamName = streamNameFunction()->getStreamName(ConfHist,"");
    std::string streamName = streamNameFunction()->getStreamName(this,ConfHist,"",false); // Hidetoshi Otono 14.03.05
    m_path = streamName.substr(0, streamName.rfind("SCT/GENERAL/Conf"));
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Global Path :" << m_path << endreq;

    std::string m_SummaryBinNames[] = {"Mod Out","Flagged Links","Masked Links","Errors","Inefficient","Noisy"}; 
    std::string m_ConfigurationBinNames[] = {"Modules","Link 0","Link 1","Chips","Strips (10^{2})"};
    std::string m_ConfigurationOnlineBinNames[] = {"Mod Out","Flagged Links","Masked Links","Errors"};
    std::string m_ConfigurationEffBinNames[] = {"Ineff B","Ineff EA","Ineff EC","ALL"};

    std::string m_ConfigurationNoiseBinNames[] = {"Noisy B","Noisy EA","Noisy EC","ALL"};
    string hNumEndcap[N_DISKS]={"0","1","2","3","4","5","6","7","8"};

    m_p2DmapHistoVectorAll[iBARREL].clear();
    VecProf2_t * storageVectors[]={&m_pnoiseoccupancymapHistoVectorECC, &m_pnoiseoccupancymapHistoVectorBar, &m_pnoiseoccupancymapHistoVectorECA};
    VecProf2_t * storageVectorsSide0[]={&m_pnoiseoccupancymapHistoVectorECCSide0, &m_pnoiseoccupancymapHistoVectorBarSide0, &m_pnoiseoccupancymapHistoVectorECASide0};
    VecProf2_t * storageVectorsSide1[]={&m_pnoiseoccupancymapHistoVectorECCSide1, &m_pnoiseoccupancymapHistoVectorBarSide1, &m_pnoiseoccupancymapHistoVectorECASide1};
    const int bec[] = {-2,0,2};
    const unsigned int limits[]={N_DISKS, N_BARRELS, N_DISKS};
    for(int systemIndex=0; systemIndex<3; systemIndex++){
      for(auto prof2: *storageVectors[systemIndex]) delete prof2;
      (storageVectors[systemIndex])->clear();
      for(auto prof2: *storageVectorsSide0[systemIndex]) delete prof2;
      (storageVectorsSide0[systemIndex])->clear();
      for(auto prof2: *storageVectorsSide1[systemIndex]) delete prof2;
      (storageVectorsSide1[systemIndex])->clear();
      
      for (unsigned int i=0; i!=limits[systemIndex];++i) {
	const string streamhitmap ="noiseoccupancymap"+abbreviations[systemIndex]+"_"+hNumEndcap[i];
	const string streamhitmapside0 ="noiseoccupancymap"+abbreviations[systemIndex]+"_"+hNumEndcap[i]+"_side0";
	const string streamhitmapside1 ="noiseoccupancymap"+abbreviations[systemIndex]+"_"+hNumEndcap[i]+"_side1";
	std::string histotitle="SCT Noise Occupancy map for "+names[systemIndex]+": "+hNumEndcap[i];
	std::string histotitleside0="SCT Noise Occupancy map for "+names[systemIndex]+": "+hNumEndcap[i]+" side0";
	std::string histotitleside1="SCT Noise Occupancy map for "+names[systemIndex]+": "+hNumEndcap[i]+" side1";
	prof2Factory(streamhitmap,histotitle, bec[systemIndex],*(storageVectors[systemIndex]));
	prof2Factory(streamhitmapside0,histotitleside0, bec[systemIndex],*(storageVectorsSide0[systemIndex]));
	prof2Factory(streamhitmapside1,histotitleside1, bec[systemIndex],*(storageVectorsSide1[systemIndex]));
      }   
    }
      //book 2D "noise" maps, containing hits that aren't associated to tracks
    for (int i=0; i<N_BARRELSx2;++i) {
      ostringstream stream2dmap;
      stream2dmap <<"modulemap_"<< i/2 << "_" << i%2 ;
      TH2F* hitsHisto_tmp = new TH2F(TString(stream2dmap.str()),          // path
				     Form("Module Out of Configuration : Layer %d Side %d",i/2,i%2),   // title
				     N_ETA_BINS, FIRST_ETA_BIN-0.5, LAST_ETA_BIN+0.5,                
				     N_PHI_BINS, FIRST_PHI_BIN-0.5, LAST_PHI_BIN+0.5);               
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta"); 
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");
      
      if ( ConfMapsExpert.regHist(hitsHisto_tmp).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endreq;
      m_p2DmapHistoVectorAll[iBARREL].push_back(hitsHisto_tmp); 
    }
    if (m_makeConfHisto or testOffline) {
      m_DetailedConfiguration = new TProfile("SCTConfDetails","Exclusion from the Configuration",ConfbinsDetailed,-0.5,ConfbinsDetailed - 0.5);
      if ( ConfHist.regHist(m_DetailedConfiguration).isFailure() )
        msg(MSG::WARNING) << "Cannot book Histogram:SCTConfDetails" << endreq; 

        TString conftitle[4]={"SCTConfBarrel","SCTConfEndcapA","SCTConfEndcapC","SCTConf"};
        TString conftitleRN[4]={"SCTConfBarrelRN","SCTConfEndcapARN","SCTConfEndcapCRN","SCTConfRN"};
        TString conflabel[4]={"Num of Problematic Module in Barrel","Num of Problematic Module in EndcapA", 
                                      "Num of Problematic Module in EndcapC","Num of Problematic Module in All Region"};//30.11.2014 
        TString confonlinetitle[4]={"SCTOnlineConfBarrel","SCTOnlineConfEndcapA","SCTOnlineConfEndcapC","SCTOnlineConf"};
        TString confefftitle="SCTEffConf";
	TString region[4]={"Barrel","EndcapA","EndcapC","All Region"};
	TString layer[9]={" layer0"," layer1"," layer2"," layer3"," layer4"," layer5"," layer6"," layer7"," layer8"};
        TString confnoisetitle="SCTNoiseConf";
        TString confnoisetitle_recent="SCTNoiseConfRecent";

        TString maskedlinktitle[4]={"SCTMaskedLinkVsLbsBarrel","SCTMaskedLinkVsLbsEndcapA","SCTMaskedLinkVsLbsEndcapC","SCTMaskedLinkVsLbs"};
        TString robfragtitle[4]={"SCTROBFragmentVsLbsBarrel","SCTROBFragmentVsLbsEndcapA","SCTROBFragmentVsLbsEndcapC","SCTROBFragmentVsLbs"};
        TString abcdtitle[4]={"SCTABCDerrsVsLbsBarrel","SCTABCDerrsVsLbsEndcapA","SCTABCDerrsVsLbsEndcapC","SCTABCDerrsVsLbs"};
        TString rawerrstitle[4]={"SCTRawerrsVsLbsBarrel","SCTRawerrsVsLbsEndcapA","SCTRawerrsVsLbsEndcapC","SCTRawerrsVsLbs"};
        TString timeouttitle[4]={"SCTTimeOutVsLbsBarrel","SCTTimeOutVsLbsEndcapA","SCTTimeOutVsLbsEndcapC","SCTTimeOutVsLbs"};
        TString lvl1idtitle[4]={"SCTLVL1IDerrsVsLbsBarrel","SCTLVL1IDerrsVsLbsEndcapA","SCTLVL1IDerrsVsLbsEndcapC","SCTLVL1IDerrsVsLbs"};
        TString bcidtitle[4]={"SCTBCIDerrsVsLbsBarrel","SCTBCIDerrsVsLbsEndcapA","SCTBCIDerrsVsLbsEndcapC","SCTBCIDerrsVsLbs"};
        TString preambletitle[4]={"SCTPreambleVsLbsBarrel","SCTPreambleVsLbsEndcapA","SCTPreambleVsLbsEndcapC","SCTPreambleVsLbs"};
        TString formattertitle[4]={"SCTFormattererrsVsLbsBarrel","SCTFormattererrsVsLbsEndcapA","SCTFormattererrsVsLbsEndcapC","SCTFormattererrsVsLbs"};
        TString rodclocktitle[4]={"SCTRODClockerrsVsLbsBarrel","SCTRODClockerrsVsLbsEndcapA","SCTRODClockerrsVsLbsEndcapC","SCTRODClockerrsVsLbs"};
        TString truncrodtitle[4]={"SCTTruncatedRODVsLbsBarrel","SCTTruncatedRODVsLbsEndcapA","SCTTruncatedRODVsLbsEndcapC","SCTTruncatedRODVsLbs"};
	TString bsparsetitle[4]={"SCTBSParseerrsVsLbsBarrel","SCTBSParseerrsVsLbsEndcapA","SCTBSParseerrsVsLbsEndcapC","SCTBSParseerrsVsLbs"};
	TString misslinktitle[4]={"SCTMissingLinkVsLbsBarrel","SCTMissingLinkVsLbsEndcapA","SCTMissingLinkVsLbsEndcapC","SCTMissingLinkVsLbs"};

        TString maximummaskedlinktitle[4]={"SCTMaskedLinkmaxVsLbsBarrel","SCTMaskedLinkmaxVsLbsEndcapA","SCTMaskedLinkmaxVsLbsEndcapC","SCTMaskedLinkmaxVsLbs"};
        TString maximumrobfragtitle[4]={"SCTROBFragmentmaxVsLbsBarrel","SCTROBFragmentmaxVsLbsEndcapA","SCTROBFragmentmaxVsLbsEndcapC","SCTROBFragmentmaxVsLbs"};
        TString maximumabcdtitle[4]={"SCTABCDerrsmaxVsLbsBarrel","SCTABCDerrsmaxVsLbsEndcapA","SCTABCDerrsmaxVsLbsEndcapC","SCTABCDerrsmaxVsLbs"};
        TString maximumrawerrstitle[4]={"SCTRawerrsmaxVsLbsBarrel","SCTRawerrsmaxVsLbsEndcapA","SCTRawerrsmaxVsLbsEndcapC","SCTRawerrsmaxVsLbs"};
        TString maximumtimeouttitle[4]={"SCTTimeOutmaxVsLbsBarrel","SCTTimeOutmaxVsLbsEndcapA","SCTTimeOutmaxVsLbsEndcapC","SCTTimeOutmaxVsLbs"};
        TString maximumlvl1idtitle[4]={"SCTLVL1IDerrsmaxVsLbsBarrel","SCTLVL1IDerrsmaxVsLbsEndcapA","SCTLVL1IDerrsmaxVsLbsEndcapC","SCTLVL1IDerrsmaxVsLbs"};
        TString maximumbcidtitle[4]={"SCTBCIDerrsmaxVsLbsBarrel","SCTBCIDerrsmaxVsLbsEndcapA","SCTBCIDerrsmaxVsLbsEndcapC","SCTBCIDerrsmaxVsLbs"};
        TString maximumpreambletitle[4]={"SCTPreamblemaxVsLbsBarrel","SCTPreamblemaxVsLbsEndcapA","SCTPreamblemaxVsLbsEndcapC","SCTPreamblemaxVsLbs"};
        TString maximumformattertitle[4]={"SCTFormattererrsmaxVsLbsBarrel","SCTFormattererrsmaxVsLbsEndcapA","SCTFormattererrsmaxVsLbsEndcapC","SCTFormattererrsmaxVsLbs"};
        TString maximumrodclocktitle[4]={"SCTRODClockerrsmaxVsLbsBarrel","SCTRODClockerrsmaxVsLbsEndcapA","SCTRODClockerrsmaxVsLbsEndcapC","SCTRODClockerrsmaxVsLbs"};
        TString maximumtruncrodtitle[4]={"SCTTruncatedRODmaxVsLbsBarrel","SCTTruncatedRODmaxVsLbsEndcapA","SCTTruncatedRODmaxVsLbsEndcapC","SCTTruncatedRODmaxVsLbs"};
	TString maximumbsparsetitle[4]={"SCTBSParseerrsmaxVsLbsBarrel","SCTBSParseerrsmaxVsLbsEndcapA","SCTBSParseerrsmaxVsLbsEndcapC","SCTBSParseerrsmaxVsLbs"};
	TString maximummissinglinktitle[4]={"SCTMissingLinkmaxVsLbsBarrel","SCTMissingLinkmaxVsLbsEndcapA","SCTMissingLinkmaxVsLbsEndcapC","SCTMissingLinkmaxVsLbs"};

        TString numerrors[4]={"SCTNumberOfErrorsBarrel","SCTNumberOfErrorsEndcapA","SCTNumberOfErrorsEndcapC","SCTNumberOfErrors"};
        TString moderrors[4]={"SCTModulesWithErrorsBarrel","SCTModulesWithErrorsEndcapA","SCTModulesWithErrorsEndcapC","SCTModulesWithErrors"};
	TString lyrerrors[9]={"lyr0","lyr1","lyr2","lyr3","lyr4","lyr5","lyr6","lyr7","lyr8"};
        TString maxnumerrors[4]={"SCTmaxNumberOfErrorsBarrel","SCTmaxNumberOfErrorsEndcapA","SCTmaxNumberOfErrorsEndcapC","SCTmaxNumberOfErrors"};
        TString maxmoderrors[4]={"SCTmaxModulesWithErrorsBarrel","SCTmaxModulesWithErrorsEndcapA","SCTmaxModulesWithErrorsEndcapC","SCTmaxModulesWithErrors"};

        for(int reg=0; reg<4; reg++){
          m_Conf[reg] = new TProfile(conftitle[reg],conflabel[reg],Confbins,-0.5,Confbins-0.5);//30.11.2014
	  for (int bin = 0; bin<Confbins; bin++) m_Conf[reg]->GetXaxis()->SetBinLabel(bin+1,m_SummaryBinNames[bin].c_str());
          m_ConfRN[reg] = new TProfile(conftitleRN[reg],conflabel[reg],Confbins,-0.5,Confbins-0.5);//30.11.2014
	  for (int bin = 0; bin<Confbins; bin++) m_ConfRN[reg]->GetXaxis()->SetBinLabel(bin+1,m_SummaryBinNames[bin].c_str());
          const int conf_online_bins = 4;
          if(m_environment==AthenaMonManager::online or testOffline){
            m_ConfOnline[reg]=new TH1F(confonlinetitle[reg],conflabel[reg]+" Online",conf_online_bins,-0.5,conf_online_bins-0.5);//30.11.2014 
            for (int bin = 0; bin<conf_online_bins; bin++) m_ConfOnline[reg]->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationOnlineBinNames[bin].c_str());
          }
          m_MaskedLinksVsLB[reg] = new TProfile(maskedlinktitle[reg],"Ave. masked link errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaskedLinksVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_MaskedLinksVsLB[reg]->GetYaxis()->SetTitle("Num of masked link errors"); 
          m_ROBFragmentVsLB[reg] = new TProfile(robfragtitle[reg],"Ave. ROB fragment errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_ROBFragmentVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ROBFragmentVsLB[reg]->GetYaxis()->SetTitle("Num of ROB fragment errors");
	  m_ABCDVsLB[reg] = new TProfile(abcdtitle[reg],"Ave. ABCD errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_ABCDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ABCDVsLB[reg]->GetYaxis()->SetTitle("Num of ABCD errors");
          m_RawErrsVsLB[reg] = new TProfile(rawerrstitle[reg],"Ave. Raw errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_RawErrsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_RawErrsVsLB[reg]->GetYaxis()->SetTitle("Num of raw errors");
          m_TimeOutVsLB[reg] = new TProfile(timeouttitle[reg],"Ave. Time Out errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_TimeOutVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_TimeOutVsLB[reg]->GetYaxis()->SetTitle("Num of time out errors");
          m_LVL1IDVsLB[reg] = new TProfile(lvl1idtitle[reg],"Ave. LVL1ID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_LVL1IDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_LVL1IDVsLB[reg]->GetYaxis()->SetTitle("Num of LVL1ID errors");
          m_BCIDVsLB[reg] = new TProfile(bcidtitle[reg],"Ave. BCID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_BCIDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_BCIDVsLB[reg]->GetYaxis()->SetTitle("Num of BCID errors");
          m_PreambleVsLB[reg] = new TProfile(preambletitle[reg],"Ave. Preamble errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_PreambleVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_PreambleVsLB[reg]->GetYaxis()->SetTitle("Num of preamble errors");
          m_FormatterVsLB[reg] = new TProfile(formattertitle[reg],"Ave. Formatter errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_FormatterVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_FormatterVsLB[reg]->GetYaxis()->SetTitle("Num of formatter errors");
          m_RODClockVsLB[reg] = new TProfile(rodclocktitle[reg],"Ave. ROD Clock errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_RODClockVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_RODClockVsLB[reg]->GetYaxis()->SetTitle("Num of ROD clock errors");
          m_TruncRODVsLB[reg] = new TProfile(truncrodtitle[reg],"Ave. Truncated RODs per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_TruncRODVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_TruncRODVsLB[reg]->GetYaxis()->SetTitle("Num of truncated RODs");
          m_BSParseVsLB[reg] = new TProfile(bsparsetitle[reg],"Ave. BS Parse errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_BSParseVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_BSParseVsLB[reg]->GetYaxis()->SetTitle("Num of BS parse errors");
          m_MissingLinkHeaderVsLB[reg] = new TProfile(misslinktitle[reg],"Ave. Missing Link Header errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_BSParseVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_BSParseVsLB[reg]->GetYaxis()->SetTitle("Num of Missing Link Header errors");

          m_MaxMaskedLinksVsLB[reg] = new TH1F(maximummaskedlinktitle[reg],"Max num of masked link errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxMaskedLinksVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_MaxMaskedLinksVsLB[reg]->GetYaxis()->SetTitle("Num of masked link errors"); 
          // m_2d_MaxMaskedLinksVsLB[reg] = new TH2I(maximummaskedlinktitle[reg]+"_2d","Max num of masked link errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxROBFragmentVsLB[reg] = new TH1F(maximumrobfragtitle[reg],"Max num of ROB fragment errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxROBFragmentVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxROBFragmentVsLB[reg]->GetYaxis()->SetTitle("Num of ROB fragment errors");
          // m_2d_MaxROBFragmentVsLB[reg] = new TH2I(maximumrobfragtitle[reg]+"_2d","Max num of ROB fragment errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);

          m_MaxABCDVsLB[reg] = new TH1F(maximumabcdtitle[reg],"Max num of ABCD errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxABCDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxABCDVsLB[reg]->GetYaxis()->SetTitle("Num of ABCD errors");
          // m_2d_MaxABCDVsLB[reg] = new TH2I(maximumabcdtitle[reg]+"_2d","Max num of ABCD errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxRawErrsVsLB[reg] = new TH1F(maximumrawerrstitle[reg],"Max num of Raw errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxRawErrsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxRawErrsVsLB[reg]->GetYaxis()->SetTitle("Num of raw errors");
          // m_2d_MaxRawErrsVsLB[reg] = new TH2I(maximumrawerrstitle[reg]+"_2d","Max num of Raw errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxTimeOutVsLB[reg] = new TH1F(maximumtimeouttitle[reg],"Max num of Time Out errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxTimeOutVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxTimeOutVsLB[reg]->GetYaxis()->SetTitle("Num of time out errors");
          // m_2d_MaxTimeOutVsLB[reg] = new TH2I(maximumtimeouttitle[reg]+"_2d","Max num of Time Out errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxLVL1IDVsLB[reg] = new TH1F(maximumlvl1idtitle[reg],"Max num of LVL1ID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxLVL1IDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxLVL1IDVsLB[reg]->GetYaxis()->SetTitle("Num of LVL1ID errors");
          // m_2d_MaxLVL1IDVsLB[reg] = new TH2I(maximumlvl1idtitle[reg]+"_2d","Max num of LVL1ID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxBCIDVsLB[reg] = new TH1F(maximumbcidtitle[reg],"Max num of BCID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxBCIDVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxBCIDVsLB[reg]->GetYaxis()->SetTitle("Num of BCID errors");
          // m_2d_MaxBCIDVsLB[reg] = new TH2I(maximumbcidtitle[reg]+"_2d","Max num of BCID errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxPreambleVsLB[reg] = new TH1F(maximumpreambletitle[reg],"Max num of Preamble errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxPreambleVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxPreambleVsLB[reg]->GetYaxis()->SetTitle("Num of preamble errors");
          // m_2d_MaxPreambleVsLB[reg] = new TH2I(maximumpreambletitle[reg]+"_2d","Max num of Preamble errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxFormatterVsLB[reg] = new TH1F(maximumformattertitle[reg],"Max num of Formatter errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxFormatterVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxFormatterVsLB[reg]->GetYaxis()->SetTitle("Num of formatter errors");
          // m_2d_MaxFormatterVsLB[reg] = new TH2I(maximumformattertitle[reg]+"_2d","Max num of Formatter errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxRODClockVsLB[reg] = new TH1F(maximumrodclocktitle[reg],"Max num of ROD Clock errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxRODClockVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxRODClockVsLB[reg]->GetYaxis()->SetTitle("Num of ROD clock errors");
          // m_2d_MaxRODClockVsLB[reg] = new TH2I(maximumrodclocktitle[reg]+"_2d","Max num of ROD Clock errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxTruncRODVsLB[reg] = new TH1F(maximumtruncrodtitle[reg],"Max num of Truncated RODs per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxTruncRODVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxTruncRODVsLB[reg]->GetYaxis()->SetTitle("Num of truncated RODs");
          // m_2d_MaxTruncRODVsLB[reg] = new TH2I(maximumtruncrodtitle[reg]+"_2d","Max num of Truncated RODs per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxBSParseVsLB[reg] = new TH1F(maximumbsparsetitle[reg],"Max num of BS Parse errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxBSParseVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxBSParseVsLB[reg]->GetYaxis()->SetTitle("Num of BS parse errors");
          // m_2d_MaxBSParseVsLB[reg] = new TH2I(maximumbsparsetitle[reg]+"_2d","Max num of BS Parse errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
          m_MaxMissingLinkHeaderVsLB[reg] = new TH1F(maximummissinglinktitle[reg],"Max num of Missing Link Header errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxMissingLinkHeaderVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
          m_MaxMissingLinkHeaderVsLB[reg]->GetYaxis()->SetTitle("Num of Missing Link Header errors");
          // m_2d_MaxMissingLinkHeaderVsLB[reg] = new TH2I(maximummissinglinktitle[reg]+"_2d","Max num of Missing Link Header errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);

          m_NumberOfErrorsVsLB[reg] = new TProfile(numerrors[reg],"Ave. errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_NumberOfErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_ModulesWithErrorsVsLB[reg] = new TProfile(moderrors[reg],"Ave. num of links with errors per event in "+region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_ModulesWithErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 

          m_MaxNumberOfErrorsVsLB[reg] = new TH1F(maxnumerrors[reg],"Max num of errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxNumberOfErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          // m_2d_MaxNumberOfErrorsVsLB[reg] = new TH2I(maxnumerrors[reg]+"_2d","Max num of errors per event in "+ region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176*5,0,8176*5);
          m_MaxModulesWithErrorsVsLB[reg] = new TH1F(maxmoderrors[reg],"Max num of links with errors per event in "+region[reg],n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaxModulesWithErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          // m_2d_MaxModulesWithErrorsVsLB[reg] = new TH2I(maxmoderrors[reg]+"_2d","Max num of links with errors per event in "+region[reg],n_lumiBins,0.5,n_lumiBins+0.5,8176,0,8176);
 
        }
	for(int lyr=0; lyr<4; lyr++){
	  m_LinksWithErrorsVsLBBarrel[lyr] = new TProfile("SCTLinksWithErrorsBarrel"+lyrerrors[lyr],"Ave. num of links with errors per event in Barrel "+layer[lyr],n_lumiBins,0.5,n_lumiBins+0.5);
	  m_LinksWithErrorsVsLBBarrel[lyr]->GetXaxis()->SetTitle("LumiBlock"); 
	}
	for(int lyr=0; lyr<9; lyr++){
	  m_LinksWithErrorsVsLBEndcapA[lyr] = new TProfile("SCTLinksWithErrorsEndcapA"+lyrerrors[lyr],"Ave. num of links with errors per event in EndcapA"+layer[lyr],n_lumiBins,0.5,n_lumiBins+0.5);
	  m_LinksWithErrorsVsLBEndcapA[lyr]->GetXaxis()->SetTitle("LumiBlock"); 
	}
	for(int lyr=0; lyr<9; lyr++){
	  m_LinksWithErrorsVsLBEndcapC[lyr] = new TProfile("SCTLinksWithErrorsEndcapC"+lyrerrors[lyr],"Ave. num of links with errors per event in EndcapC "+layer[lyr],n_lumiBins,0.5,n_lumiBins+0.5);
	  m_LinksWithErrorsVsLBEndcapC[lyr]->GetXaxis()->SetTitle("LumiBlock"); 
	}

        const int conf_noise_bins = 4;
        const int conf_eff_bins = 4;

        if(m_environment==AthenaMonManager::online or testOffline){
          m_ConfEffOnline = new TProfile(confefftitle,"Number of Inefficient Modules Online",conf_eff_bins,-0.5,conf_eff_bins-0.5);
          for (int bin = 0; bin<conf_eff_bins; bin++) m_ConfEffOnline->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationEffBinNames[bin].c_str());
          m_ConfNoiseOnline = new TProfile(confnoisetitle,"Number of Noisy Modules Online",conf_noise_bins,-0.5,conf_noise_bins-0.5);
          for (int bin = 0; bin<conf_noise_bins; bin++) m_ConfNoiseOnline->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationNoiseBinNames[bin].c_str());
          m_ConfNoiseOnlineRecent = new TProfile(confnoisetitle_recent,"Number of Noisy Modules Online Recent",conf_noise_bins,-0.5,conf_noise_bins-0.5); 

          for (int bin = 0; bin<conf_noise_bins; bin++) m_ConfNoiseOnlineRecent->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationNoiseBinNames[bin].c_str());
        }
        for (int bin = 0; bin<ConfbinsDetailed; bin++) m_DetailedConfiguration->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationBinNames[bin].c_str());
      if ( ConfHist.regHist(m_Conf[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfMaps.regHist(m_Conf[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfHistECA.regHist(m_Conf[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfHistECC.regHist(m_Conf[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfHist.regHist(m_ConfRN[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfMaps.regHist(m_ConfRN[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfHistECA.regHist(m_ConfRN[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if ( ConfHistECC.regHist(m_ConfRN[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endreq;
      if(m_environment==AthenaMonManager::online or testOffline){
        if ( ConfHist.regHist(m_ConfOnline[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfOnline" << endreq;
        if ( ConfMaps.regHist(m_ConfOnline[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfOnline" << endreq;
        if ( ConfHistECA.regHist(m_ConfOnline[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfOnline" << endreq;
        if ( ConfHistECC.regHist(m_ConfOnline[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfOnline" << endreq;
      }
      if ( ConfHist.regHist(m_MaskedLinksVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfMaps.regHist(m_MaskedLinksVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHistECA.regHist(m_MaskedLinksVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHistECC.regHist(m_MaskedLinksVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHist.regHist(m_ROBFragmentVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfMaps.regHist(m_ROBFragmentVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHistECA.regHist(m_ROBFragmentVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHistECC.regHist(m_ROBFragmentVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHist.regHist(m_ABCDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfMaps.regHist(m_ABCDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHistECA.regHist(m_ABCDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHistECC.regHist(m_ABCDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHist.regHist(m_RawErrsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfMaps.regHist(m_RawErrsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHistECA.regHist(m_RawErrsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHistECC.regHist(m_RawErrsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHist.regHist(m_TimeOutVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfMaps.regHist(m_TimeOutVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHistECA.regHist(m_TimeOutVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHistECC.regHist(m_TimeOutVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHist.regHist(m_LVL1IDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfMaps.regHist(m_LVL1IDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHistECA.regHist(m_LVL1IDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHistECC.regHist(m_LVL1IDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHist.regHist(m_BCIDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfMaps.regHist(m_BCIDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHistECA.regHist(m_BCIDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHistECC.regHist(m_BCIDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHist.regHist(m_PreambleVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfMaps.regHist(m_PreambleVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHistECA.regHist(m_PreambleVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHistECC.regHist(m_PreambleVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHist.regHist(m_FormatterVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfMaps.regHist(m_FormatterVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHistECA.regHist(m_FormatterVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHistECC.regHist(m_FormatterVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHist.regHist(m_RODClockVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfMaps.regHist(m_RODClockVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHistECA.regHist(m_RODClockVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHistECC.regHist(m_RODClockVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHist.regHist(m_TruncRODVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfMaps.regHist(m_TruncRODVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHistECA.regHist(m_TruncRODVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHistECC.regHist(m_TruncRODVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHist.regHist(m_BSParseVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfMaps.regHist(m_BSParseVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHistECA.regHist(m_BSParseVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHistECC.regHist(m_BSParseVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHist.regHist(m_MissingLinkHeaderVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfMaps.regHist(m_MissingLinkHeaderVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfHistECA.regHist(m_MissingLinkHeaderVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfHistECC.regHist(m_MissingLinkHeaderVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;

      if ( ConfHist.regHist(m_MaxMaskedLinksVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfMaps.regHist(m_MaxMaskedLinksVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxMaskedLinksVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxMaskedLinksVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
      if ( ConfHist.regHist(m_MaxROBFragmentVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfMaps.regHist(m_MaxROBFragmentVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxROBFragmentVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxROBFragmentVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
      if ( ConfHist.regHist(m_MaxABCDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfMaps.regHist(m_MaxABCDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxABCDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxABCDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
      if ( ConfHist.regHist(m_MaxRawErrsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfMaps.regHist(m_MaxRawErrsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxRawErrsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxRawErrsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
      if ( ConfHist.regHist(m_MaxTimeOutVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfMaps.regHist(m_MaxTimeOutVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxTimeOutVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxTimeOutVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
      if ( ConfHist.regHist(m_MaxLVL1IDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfMaps.regHist(m_MaxLVL1IDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxLVL1IDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxLVL1IDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
      if ( ConfHist.regHist(m_MaxBCIDVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfMaps.regHist(m_MaxBCIDVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxBCIDVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxBCIDVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
      if ( ConfHist.regHist(m_MaxPreambleVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfMaps.regHist(m_MaxPreambleVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxPreambleVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxPreambleVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
      if ( ConfHist.regHist(m_MaxFormatterVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfMaps.regHist(m_MaxFormatterVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxFormatterVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxFormatterVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
      if ( ConfHist.regHist(m_MaxRODClockVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfMaps.regHist(m_MaxRODClockVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxRODClockVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxRODClockVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
      if ( ConfHist.regHist(m_MaxTruncRODVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfMaps.regHist(m_MaxTruncRODVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxTruncRODVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxTruncRODVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
      if ( ConfHist.regHist(m_MaxBSParseVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfMaps.regHist(m_MaxBSParseVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxBSParseVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxBSParseVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
      if ( ConfHist.regHist(m_MaxMissingLinkHeaderVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfMaps.regHist(m_MaxMissingLinkHeaderVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfHistECA.regHist(m_MaxMissingLinkHeaderVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
      if ( ConfHistECC.regHist(m_MaxMissingLinkHeaderVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHEaderConf" << endreq;

      for(int reg=0;reg<4;reg++){
	// if ( ConfHist2D.regHist(m_2d_MaxMaskedLinksVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMaskedLinkConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxROBFragmentVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTROBFragmentConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxABCDVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTABCDConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxRawErrsVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRawErrsConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxTimeOutVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTimeOutConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxLVL1IDVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTLVL1IDConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxBCIDVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBCIDConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxPreambleVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTPreambleConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxFormatterVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTFormatterConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxRODClockVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTRODClockConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxTruncRODVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTTruncRODConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxBSParseVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTBSParseConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxMissingLinkHeaderVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTMissingLinkHeaderConf" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxNumberOfErrorsVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
	// if ( ConfHist2D.regHist(m_2d_MaxModulesWithErrorsVsLB[reg]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      }
      if ( ConfHist.regHist(m_NumberOfErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfMaps.regHist(m_NumberOfErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECA.regHist(m_NumberOfErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECC.regHist(m_NumberOfErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHist.regHist(m_ModulesWithErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_ModulesWithErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_ModulesWithErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_ModulesWithErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_LinksWithErrorsVsLBBarrel[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_LinksWithErrorsVsLBBarrel[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_LinksWithErrorsVsLBBarrel[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_LinksWithErrorsVsLBBarrel[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[4]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[5]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[6]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[7]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_LinksWithErrorsVsLBEndcapA[8]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[4]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[5]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[6]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[7]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_LinksWithErrorsVsLBEndcapC[8]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;

      if ( ConfHist.regHist(m_MaxNumberOfErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfMaps.regHist(m_MaxNumberOfErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECA.regHist(m_MaxNumberOfErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECC.regHist(m_MaxNumberOfErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHist.regHist(m_MaxModulesWithErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_MaxModulesWithErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_MaxModulesWithErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_MaxModulesWithErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;

      if(m_environment==AthenaMonManager::online or testOffline){
        if ( ConfHist.regHist(m_ConfEffOnline).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfEffOnline" << endreq;
        if ( ConfHist.regHist(m_ConfNoiseOnline).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfNoiseOnline" << endreq;
        if ( ConfHist.regHist(m_ConfNoiseOnlineRecent).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTConfNoiseOnlineRecent" << endreq;
      }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Finished registering Conf Histograms :" << m_path << endreq;
    }//end if
  }//end if isNewRun
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                   SCTErrMonTool :: bookPositiveEndCapConfMaps
//====================================================================================================
StatusCode  
SCTErrMonTool::bookPositiveEndCapConfMaps() {
  if(ManagedMonitorToolBase::newRun){
    MonGroup PlusEndCapConfMaps(this,"SCT/SCTEA/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    m_p2DmapHistoVectorAll[iECp].clear();
    //book 2D "noise" maps, containing hits that aren't associated to tracks, for positive endcap
    for (int i(0); i!=N_ENDCAPSx2;++i) {
      ostringstream stream2dmap;
      stream2dmap <<"modulemapECp_"<< i/2 << "_" << i%2 ;
      TH2F* hitsHisto_tmp = new TH2F(TString(stream2dmap.str()),         // path
             Form("Module Out of Configuration : Disk %d Side %d",i/2,i%2),  // title
             N_ETA_BINS_EC, FIRST_ETA_BIN_EC-0.5, LAST_ETA_BIN_EC+0.5,              // X num bins, X_lo, X_hi
             N_PHI_BINS_EC, FIRST_PHI_BIN_EC-0.5, LAST_PHI_BIN_EC+0.5);             // Y num bins, Y_lo, Y_hi  
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");   
      if ( PlusEndCapConfMaps.regHist(hitsHisto_tmp).isFailure() )msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endreq;
      m_p2DmapHistoVectorAll[iECp].push_back(hitsHisto_tmp); 
    } 
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                    SCTErrMonTool :: bookNegativeEndCapConfMaps
//====================================================================================================
StatusCode  
SCTErrMonTool::bookNegativeEndCapConfMaps(){
  if(ManagedMonitorToolBase::newRun){
    MonGroup MinusEndCapConfMaps(this,"SCT/SCTEC/Conf",ManagedMonitorToolBase::run,ATTRIB_UNMANAGED );
    m_p2DmapHistoVectorAll[iECm].clear();
    //book 2D "noise" maps, containing hits that aren't associated to tracks, for negative endcap
    for (int i=0; i!=N_ENDCAPSx2;++i) {
      ostringstream stream2dmap;
      stream2dmap <<"modulemapEcm_"<< i/2 << "_" << i%2 ;                
      TH2F* hitsHisto_tmp = new TH2F(TString(stream2dmap.str()),         // path
             Form("Module Out of Configuration : Disk %d Side %d",i/2,i%2),  // title
             N_ETA_BINS_EC, FIRST_ETA_BIN_EC-0.5, LAST_ETA_BIN_EC+0.5,               
             N_PHI_BINS_EC, FIRST_PHI_BIN_EC-0.5, LAST_PHI_BIN_EC+0.5);  
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta"); 
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");            

      if ( MinusEndCapConfMaps.regHist(hitsHisto_tmp).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endreq;
      m_p2DmapHistoVectorAll[iECm].push_back(hitsHisto_tmp); 
    }
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                          SCTErrMonTool :: fillCondDBMaps
//====================================================================================================
StatusCode 
SCTErrMonTool::fillCondDBMaps(){ 
  int Flagged[4] = {0};
  int MOut[4] = {0};
  int MaskedLinks[4] = {(int)m_MaskedLinks->GetBinContent(3),(int)m_MaskedLinks->GetBinContent(2),(int)m_MaskedLinks->GetBinContent(4),(int)m_MaskedLinks->GetBinContent(1)};
  int ModErr[4] = {0};
  int InEffModules[4] = {0};
  int NoisyModules[4] = {0};
  int NoisyModulesRecent[4] = {0};
  int RNoisyModules[4] = {0};

  bool failedbooking = false;
  TH2* hitsHisto_tmp[2];

  // Pointers are deleted by regHist() method
  std::string stem;
  SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
  SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();

  for(;planeIterator not_eq planeEnd;++planeIterator){
    Identifier  planeId(*planeIterator);
    int eta = m_pSCTHelper->eta_module(planeId);
    int phi = m_pSCTHelper->phi_module(planeId);
    int barrel_ec = m_pSCTHelper->barrel_ec(planeId);
    int element=2*m_pSCTHelper->layer_disk(planeId)+m_pSCTHelper->side(planeId);
    int IN = (m_ConfigurationSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1); 
    if (m_flaggedSvc->numBadIds() != 0){ 
      Flagged[iGEN] += (m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      if(barrel_ec==BARREL)Flagged[iBARREL]+=(m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      if(barrel_ec==ENDCAP_A)Flagged[iECp]+=(m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      if(barrel_ec==ENDCAP_C)Flagged[iECm]+=(m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
    }
    if (m_pSCTHelper->side(planeId) == 0) {
      if (IN == 1) {
        MOut[iGEN]++;
        if(barrel_ec==BARREL) MOut[iBARREL]++;    
        if(barrel_ec==ENDCAP_A) MOut[iECp]++;   
        if(barrel_ec==ENDCAP_C) MOut[iECm]++;
      }
    } 
    //danger: no check that barrel_ec is valid
    int y = phi + 1;
    int x = (barrel_ec==BARREL) ? (eta + 7):(eta + 1);
    int iRegion = iBARREL;
    if (barrel_ec==ENDCAP_A) iRegion = iECp;
    if (barrel_ec==ENDCAP_C) iRegion = iECm;
    m_p2DmapHistoVectorAll[iRegion][element]->SetBinContent(x,y,IN);
  }

  //calculate ratio noise
  for(int j=0; j<n_mod[GENERAL_INDEX]; j++){
    int layer = nLayer[j];
    if(nZero[j]!=0){
      ratio=0;
      ratioside0=0;
      ratioside1=0;
      ratio = 1E5*calculateNoiseOccupancyUsingRatioMethod(nOne[j], nZero[j]);
      ratioside0 = 1E5*calculateOneSideNoiseOccupancyUsingRatioMethod(nOneSide0[j], nZero[j]);
      ratioside1 = 1E5*calculateOneSideNoiseOccupancyUsingRatioMethod(nOneSide1[j], nZero[j]);
      if(isBarrel(j)){
	m_pnoiseoccupancymapHistoVectorBar[layer]->Fill(nEta[j],nPhi[j],ratio);
	m_pnoiseoccupancymapHistoVectorBarSide0[layer]->Fill(nEta[j],nPhi[j],ratioside0);
	m_pnoiseoccupancymapHistoVectorBarSide1[layer]->Fill(nEta[j],nPhi[j],ratioside1);
      }
      if(isEndcapA(j)){
	m_pnoiseoccupancymapHistoVectorECA[layer]->Fill(nEta[j],nPhi[j],ratio);
	m_pnoiseoccupancymapHistoVectorECASide0[layer]->Fill(nEta[j],nPhi[j],ratioside0);
	m_pnoiseoccupancymapHistoVectorECASide1[layer]->Fill(nEta[j],nPhi[j],ratioside1);
      }
      if(isEndcapC(j)){
	m_pnoiseoccupancymapHistoVectorECC[layer]->Fill(nEta[j],nPhi[j],ratio);
	m_pnoiseoccupancymapHistoVectorECCSide0[layer]->Fill(nEta[j],nPhi[j],ratioside0);
	m_pnoiseoccupancymapHistoVectorECCSide1[layer]->Fill(nEta[j],nPhi[j],ratioside1);
      }
    }
  }

  //barrel region
  for (int lyr = 0 ; lyr != N_BARRELS; ++lyr) {
    // Check for the number of Modules with more than m_errThreshold error rate
    const int xbins(m_allErrs[SUMMARY][iBARREL][lyr]->GetNbinsX()+1);
    const int ybins(m_allErrs[SUMMARY][iBARREL][lyr]->GetNbinsY()+1);
    for (int xb = 1; xb != xbins; ++xb) 
      for (int yb = 1; yb != ybins; ++yb) 
        if (m_allErrs[SUMMARY][iBARREL][2*lyr]->GetBinContent(xb,yb) > m_errThreshold || m_allErrs[SUMMARY][iBARREL][2*lyr+1]->GetBinContent(xb,yb) > m_errThreshold){
          ModErr[iGEN]++; 
          ModErr[iBARREL]++;
        }

    //noisy modules
    failedbooking=getHisto(lyr, iBARREL, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iBARREL, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iBARREL, iGEN, m_pnoiseoccupancymapHistoVectorBar[lyr], m_noiseThreshold, RNoisyModules);
    //noisy modules recent
    if(m_environment==AthenaMonManager::online){
      failedbooking=getHistoRecent(lyr, iBARREL, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iBARREL, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
    }

    //ineff modules
    failedbooking=getHisto(lyr, iBARREL, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if(m_environment!=AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX()+1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY()+1);
        //if (testOffline) printContents(hitsHisto_tmp[0]);
        for (int xb(1); xb != xbins; ++xb){
          for (int yb(1); yb != ybins; ++yb){
            float outOfConfig=m_p2DmapHistoVectorAll[iBARREL][2*lyr]->GetBinContent(xb,yb);
            if (outOfConfig < 1){ //i.e. its in the configuration
              int nSides=numberOfInefficientSides(hitsHisto_tmp,xb,yb,1-m_effThreshold);
              InEffModules[iGEN]+=nSides; 
              InEffModules[iBARREL]+=nSides;
            }
          }
        }
      }
    }
  }
  
  
  //endcaps
  for (int lyr = 0 ; lyr !=N_ENDCAPS; ++lyr) {
    //module errors
    const int xbins(m_allErrs[SUMMARY][iECp][lyr]->GetNbinsX()+1);
    const int ybins(m_allErrs[SUMMARY][iECp][lyr]->GetNbinsY()+1);
    for (int xb = 1; xb != xbins; xb++) 
      for (int yb = 1; yb != ybins; yb++) {
        if (m_allErrs[SUMMARY][iECp][2*lyr]->GetBinContent(xb,yb) > m_errThreshold || m_allErrs[SUMMARY][iECp][2*lyr+1]->GetBinContent(xb,yb) > m_errThreshold){
          ModErr[iGEN]++; ModErr[iECp]++;
        }
        if (m_allErrs[SUMMARY][iECm][2*lyr]->GetBinContent(xb,yb) > m_errThreshold || m_allErrs[SUMMARY][iECm][2*lyr+1]->GetBinContent(xb,yb) > m_errThreshold){
          ModErr[iGEN]++; ModErr[iECm]++;}
        }

    //noisy modules
    //endcap C
    failedbooking=getHisto(lyr, iECm, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iECm, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iECm, iGEN, m_pnoiseoccupancymapHistoVectorECC[lyr], m_noiseThreshold, RNoisyModules);
    //endcap A
    failedbooking=getHisto(lyr, iECp, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iECp, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iECp, iGEN, m_pnoiseoccupancymapHistoVectorECA[lyr], m_noiseThreshold, RNoisyModules);
    //noisy modules recent
    if(m_environment==AthenaMonManager::online){
      failedbooking=getHistoRecent(lyr, iECm, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iECm, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
      //
      failedbooking=getHistoRecent(lyr, iECp, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iECp, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
    }
    //ineff modules
    failedbooking=getHisto(lyr, iECm, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if(m_environment!=AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX()+1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY()+1);
        for (int xb = 1; xb != xbins; ++xb){
          for (int yb = 1; yb != ybins; ++yb){
            if (m_p2DmapHistoVectorAll[iECm][2*lyr]->GetBinContent(xb,yb) < 1){
              int nSides=numberOfInefficientSides(hitsHisto_tmp,xb,yb,1-m_effThreshold);
              InEffModules[iGEN]+=nSides; 
              InEffModules[iECm]+=nSides;
            }
          }
        }
      }
    }


    failedbooking=getHisto(lyr, iECp, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if(m_environment!=AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX()+1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY()+1);
        for (int xb = 1; xb != xbins; ++xb){
          for (int yb = 1; yb != ybins; ++yb){
            if (m_p2DmapHistoVectorAll[iECp][2*lyr]->GetBinContent(xb,yb) < 1){
              int nSides=numberOfInefficientSides(hitsHisto_tmp,xb,yb,1-m_effThreshold);
              InEffModules[iGEN]+=nSides; 
              InEffModules[iECp]+=nSides;
            }
          }
        }
      }
    }
  }

  
  if (m_makeConfHisto) {
    for (int reg=0; reg<=3;++reg){
      m_Conf[reg]->Fill(0.,double (MOut[reg]));
      m_ConfRN[reg]->Fill(0.,double (MOut[reg]));
      m_Conf[reg]->Fill(1.,double (Flagged[reg]));
      m_ConfRN[reg]->Fill(1.,double (Flagged[reg]));
      m_Conf[reg]->Fill(2.,double (MaskedLinks[reg]));
      m_ConfRN[reg]->Fill(2.,double (MaskedLinks[reg]));
      m_Conf[reg]->Fill(3.,double (ModErr[reg]));
      m_ConfRN[reg]->Fill(3.,double (ModErr[reg]));
      m_Conf[reg]->Fill(4.,double (InEffModules[reg]));
      m_ConfRN[reg]->Fill(4.,double (InEffModules[reg]));
      m_Conf[reg]->Fill(5.,double (NoisyModules[reg]));
      m_ConfRN[reg]->Fill(5.,double (RNoisyModules[reg]));
      if(m_environment==AthenaMonManager::online){
        m_ConfOnline[reg]->Fill(0.,double (MOut[reg]));
        m_ConfOnline[reg]->Fill(1.,double (Flagged[reg]));
        m_ConfOnline[reg]->Fill(2.,double (MaskedLinks[reg]));
        m_ConfOnline[reg]->Fill(3.,double (ModErr[reg]));
      }
    }
    if(m_environment==AthenaMonManager::online or testOffline){
      m_ConfEffOnline->Reset("ICE");
	    for (int i(0);i!=4;++i){
		    const float f(i);
	  	  m_ConfEffOnline->Fill(f,double (InEffModules[i]));
		    m_ConfNoiseOnline->Fill(f,double (NoisyModules[i]));
		    m_ConfNoiseOnlineRecent->Fill(f,double (NoisyModulesRecent[i]));
	    }
    }
  }
  return StatusCode::SUCCESS;
}

//==================================================================================================== 
//                                   SCTErrMonTool :: fillConfigurationDetails 
// Fills Number of Bad Modules / Links / Chips / Strips in to a histogram 
//==================================================================================================== 
StatusCode SCTErrMonTool::fillConfigurationDetails(){  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Inside fillConfigurationDetails()" << endreq; 
  unsigned int nBadMods   = m_ConfigurationSvc->badModules()->size(); //bad modules
  std::map<Identifier, std::pair<bool, bool> >* badLinks = m_ConfigurationSvc->badLinks(); //bad links
  unsigned int nBadLink0(0), nBadLink1(0), nBadLinkBoth(0); 
  for (auto link: *badLinks){
    std::pair<bool, bool> status = link.second; 
    if (status.first == false  && status.second == true ) ++nBadLink0; 
    if (status.first == true   && status.second == false) ++nBadLink1; 
    if (status.first == false  && status.second == false) ++nBadLinkBoth; 
  }
  
  std::map<Identifier, unsigned int>* badChips = m_ConfigurationSvc->badChips(); //bad chips
  unsigned int nBadChips(0); 
  for (auto chip : *badChips) { 
    unsigned int status = chip.second; 
    for (unsigned int i(0); i!=CHIPS_PER_MODULE; i++) nBadChips += ((status & (1<<i)) == 0 ? 0 : 1); 
  } 

  std::set<Identifier> badStripsAll; //bad strips
  m_ConfigurationSvc->badStrips(badStripsAll); 
  unsigned int nBadStrips = badStripsAll.size(); 

  std::set<Identifier> badStripsExclusive; //bad strips w/o bad modules and chips
  m_ConfigurationSvc->badStrips(badStripsExclusive, true, true); 
  int nBadStripsExclusive       = badStripsExclusive.size(); 
  int nBadStripsExclusiveBEC[]  = {0,0,0}; 
  for (auto strip: badStripsExclusive) { 
    int bec(m_pSCTHelper->barrel_ec(strip)); 
    nBadStripsExclusiveBEC[SCT_Monitoring::bec2Index(bec)] += 1; 
  } 

  m_DetailedConfiguration->Fill(0.,double (nBadMods)); 
  m_DetailedConfiguration->Fill(1.,double (nBadLink0)); 
  m_DetailedConfiguration->Fill(2.,double (nBadLink1)); 
  m_DetailedConfiguration->Fill(3.,double (nBadChips)); 
  m_DetailedConfiguration->Fill(4.,double (nBadStripsExclusive)/100.); 

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------" << endreq; 
    msg(MSG::DEBUG) << "Number of bad modules                          = " << nBadMods             << endreq; 
    msg(MSG::DEBUG) << "Number of bad link 0                           = " << nBadLink0            << endreq; 
    msg(MSG::DEBUG) << "Number of bad link 1                           = " << nBadLink1            << endreq; 
    msg(MSG::DEBUG) << "Number of bad link both                        = " << nBadLinkBoth         << endreq; 
    msg(MSG::DEBUG) << "Number of bad chips                            = " << nBadChips            << endreq;   
    msg(MSG::DEBUG) << "Number of bad strips                           = " << nBadStrips           << endreq; 
    msg(MSG::DEBUG) << "Number of bad strips exclusive                 = " << nBadStripsExclusive  << endreq; 
    msg(MSG::DEBUG) << "Number of bad strips exclusive (ECC, B, ECA)   = "  
        << nBadStripsExclusiveBEC[0] << ", " 
        << nBadStripsExclusiveBEC[1] << ", " 
        << nBadStripsExclusiveBEC[2] << ", " << endreq; 
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------" << endreq;
  } 
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                          SCTErrMonTool :: resetCondDBMaps, Daniel Damiani 25/07/2011
//====================================================================================================
StatusCode SCTErrMonTool::resetCondDBMaps(){
  if(m_environment!=AthenaMonManager::online) return StatusCode::SUCCESS;
  if(m_makeConfHisto) {
    for (int reg=0; reg<=3;++reg){
      m_Conf[reg]->Reset();
      m_ConfRN[reg]->Reset();
      m_ConfOnline[reg]->Reset();
    }
    m_ConfEffOnline->Reset();
    m_ConfNoiseOnline->Reset();
    m_ConfNoiseOnlineRecent->Reset();
  }
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                          SCTErrMonTool :: resetConfigurationDetails, Daniel Damiani 25/07/2011
//====================================================================================================
StatusCode SCTErrMonTool::resetConfigurationDetails(){
  if(m_environment!=AthenaMonManager::online) return StatusCode::SUCCESS;
  if(m_makeConfHisto) m_DetailedConfiguration->Reset();
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                          SCTErrMonTool :: getHisto, Martin Flechl 14/9/2009
//====================================================================================================
bool SCTErrMonTool::getHisto(const int layer, const int reg, const int type, TH2* histo[2]){
  
  const string trm[2][N_REGIONS]={ 
    {"SCT/SCTB/Noise/noiseoccupancymap_","SCT/SCTEA/Noise/noiseoccupancymapECp_","SCT/SCTEC/Noise/noiseoccupancymapECm_"},
    {"SCT/SCTB/eff/ineff_","SCT/SCTEA/eff/ineffp_","SCT/SCTEC/eff/ineffm_"} 
  };
  const string trm_trig[2][N_REGIONS]={ 
    {"SCT/SCTB/Noise/noiseoccupancymaptrigger_","SCT/SCTEA/Noise/noiseoccupancymaptriggerECp_","SCT/SCTEC/Noise/noiseoccupancymaptriggerECm_"},
    {"SCT/SCTB/eff/ineff_","SCT/SCTEA/eff/ineffp_","SCT/SCTEC/eff/ineffm_"} 
  };
  string shname1=m_path+trm[type][reg]+std::to_string(layer)+"_0";
  string shname2=m_path+trm[type][reg]+std::to_string(layer)+"_1";
  if( testOffline or ((m_environment!=AthenaMonManager::online) && (AthenaMonManager::dataType()!= AthenaMonManager::cosmics)) ) {
    shname1=m_path+trm_trig[type][reg]+std::to_string(layer)+"_0";
    shname2=m_path+trm_trig[type][reg]+std::to_string(layer)+"_1";
  }
  bool failedBooking(false);
  failedBooking |= m_thistSvc->getHist(shname1,histo[0]).isFailure();
  failedBooking |= m_thistSvc->getHist(shname2,histo[1]).isFailure();
  if ( failedBooking ) {
    msg(MSG::WARNING) << "Error getting Histogram from ITHistSvc : "<<shname1<<","<<shname2<<endreq;
    return true;
  }
  return false;
}

//====================================================================================================
//                          SCTErrMonTool :: getHistoRecent, Dan Damiani  21/7/2011
//====================================================================================================
bool SCTErrMonTool::getHistoRecent(const int layer, const int reg, const int type, TH2* histo[2]){
  const string trm[1][N_REGIONS]={ 
    {"SCT/SCTB/Noise/noiseoccupancymaprecent_","SCT/SCTEA/Noise/noiseoccupancymaprecentECp_","SCT/SCTEC/Noise/noiseoccupancymaprecentECm_"}
  };
  string shname1=m_path+trm[type][reg]+std::to_string(layer)+"_0";
  string shname2=m_path+trm[type][reg]+std::to_string(layer)+"_1";
  bool failedBooking(false);
  failedBooking |= m_thistSvc->getHist(shname1,histo[0]).isFailure();
  failedBooking |= m_thistSvc->getHist(shname2,histo[1]).isFailure();
  if ( failedBooking ) {
    msg(MSG::WARNING) << "Error getting Histogram from ITHistSvc : "<<shname1<<","<<shname2<<endreq;
    return true;
  }
  return false;
}


SCTErrMonTool::Prof2_t
SCTErrMonTool::prof2Factory(const std::string & name, const std::string & title, const unsigned int& bec, VecProf2_t & storageVector){
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS), nPhi(N_PHI_BINS);
  if (bec != BARREL){
    firstEta=FIRST_ETA_BIN_EC;
    lastEta=LAST_ETA_BIN_EC;
    firstPhi=FIRST_PHI_BIN_EC;
    lastPhi=LAST_PHI_BIN_EC;
    nEta=N_ETA_BINS_EC;
    nPhi=N_PHI_BINS_EC;
  }
  Prof2_t tmp = new TProfile2D(TString(name), TString(title), nEta, firstEta-0.5, lastEta+0.5, nPhi, firstPhi-0.5, lastPhi+0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  storageVector.push_back(tmp);
  return tmp;
}

  float SCTErrMonTool::calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide){
    float div, rat;
    constexpr int nstrips(768);
    if(numberZeroSide!=0){
      div = numberOneSide/numberZeroSide;
      rat = (div/(div+2))/nstrips;
      return rat;
    }
    else return -1;
  }
  
  float SCTErrMonTool::calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide){
    float div, rat;
    constexpr int nstrips(768);
    if(numberZeroSide!=0){
      div = numberOneSide/numberZeroSide;
      rat = (div/(div+1))/nstrips;
      return rat;
    }
    else return -1;
  }

  bool SCTErrMonTool::isEndcapC(const int moduleNumber){
    bool moduleinEndcapC = false;
    if(0<=moduleNumber    && moduleNumber<f_mod[BARREL_INDEX]){
      moduleinEndcapC = true;}
    return moduleinEndcapC;
  }

  
  bool SCTErrMonTool::isBarrel(const int moduleNumber){
    bool moduleinBarrel = false;
    if(f_mod[BARREL_INDEX]<=moduleNumber && moduleNumber<f_mod[ENDCAP_A_INDEX]){
      moduleinBarrel = true;}
    return moduleinBarrel;
  }
  
  bool SCTErrMonTool::isEndcapA(const int moduleNumber){
    bool moduleinEndcapA = false;
    if(f_mod[ENDCAP_A_INDEX]<=moduleNumber && moduleNumber<n_mod[GENERAL_INDEX]){
    moduleinEndcapA = true;}
    return moduleinEndcapA;
  }
  
