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
  
  //print contents of a histo or profile
  void printContents( TH1 * aHisto){
    const int nbins=aHisto->GetNbinsX()+1;
    for(int i(0);i!=nbins;++i){
      double contents=aHisto->GetBinContent(i);
      std::cout<<i<<") "<<contents<<std::endl;
    }
  }
  
  void printContents( TH2 * aHisto){
     const int nbinsx=aHisto->GetNbinsX()+1;
     const int nbinsy=aHisto->GetNbinsY()+1;
     for(int i(0);i!=nbinsx;++i){
       for(int j(0);j!=nbinsy;++j){
         double contents=aHisto->GetBinContent(i,j);
         std::cout<<i<<","<<j<<") "<<contents<<std::endl;
       }
     }
   }
 
  
  int numberOfInefficientSides( TH2* pHistogramArray[],const int xbin, const int ybin, const float threshold){
    float histogramBinContent0=pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1=pHistogramArray[1]->GetBinContent(xbin, ybin);
    //std::cout<<pHistogramArray[0]->GetTitle()<<": "<<xbin<<","<<ybin<<" "<<histogramBinContent0<<", "<<histogramBinContent1<<std::endl;
    int side0Inefficient=(histogramBinContent0 > threshold);
    int side1Inefficient=(histogramBinContent1 > threshold);
    return side0Inefficient + side1Inefficient;
  }
  
  bool eitherSideIsOverThreshold( TH2* pHistogramArray[],const int xbin, const int ybin, const float threshold){
    float histogramBinContent0=pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1=pHistogramArray[1]->GetBinContent(xbin, ybin);
    return ((histogramBinContent0 > threshold) or (histogramBinContent1 > threshold));
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
    m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name),
    m_flaggedSvc("InDetSCT_FlaggedConditionSvc", name),
    m_thistSvc("THistSvc",name),
    m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name)
{
  declareProperty("histoPathBase", m_stream = "/stat");
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

 
  FirstHit= nullptr;
  FirstHit_ECp= nullptr;
  FirstHit_ECm= nullptr;
  SecondHit= nullptr;
  SecondHit_ECp= nullptr;
  SecondHit_ECm= nullptr;
  m_nErrors = nullptr;
  nErrors_buf = nullptr;
  nErrors_pos = 0;
  
  m_MaskedLinks= nullptr;
  numberOfEvents=0;
  numberOfEventsLumi=0;
  m_initialize=false;
  
  m_ConfEffOnline = 0;
  m_ConfNoiseOnline = 0;
  m_ConfNoiseOnlineRecent = 0;
  m_current_lb=0;
  m_last_reset_lb=0;
  
  m_pSCTHelper = nullptr;
  m_DetailedConfiguration = nullptr;

  //c-style arrays are not default initialized, so we initialize here
  clear3D(m_allErrs);
  clear3D(m_allErrsPerLumi);
  clear3D(m_pallErrs);
  clear3D(m_pallErrsPerLumi);
  
  clear(m_summaryErrsRecent);

  clear1D(m_numErrorsPerLumi);
  clear1D(m_rateErrorsPerLumi);
  clear1D(m_Conf);
  clear1D(m_ConfOnline);

  clear1D(m_MaskedLinksVsLB);
  clear1D(m_ROBFragmentVsLB);
  clear1D(m_NumberOfErrorsVsLB);
  clear1D(m_ModulesWithErrorsVsLB); 
}

//====================================================================================================
//====================================================================================================
SCTErrMonTool::~SCTErrMonTool(){
  free(nErrors_buf);
}

//====================================================================================================
//                            SCTErrMonTool :: bookHistograms
//====================================================================================================
StatusCode SCTErrMonTool::bookHistograms()
{  
  msg(MSG::INFO) << " initialize being called " << endreq;
  if(newRun) numberOfEvents=0;
  if(ManagedMonitorToolBase::newLumiBlock) numberOfEventsLumi=0;
  m_dataObjectName = "SCT_RDOs";
  const InDetDD::SCT_DetectorManager* mgr; //confusingly this is in a dedicated namespace
  if (detStore()->retrieve(mgr, "SCT").isFailure()){
    msg(MSG::ERROR) << "Could not retrieve Detector Manager in SCTErrMonTool::bookHistograms" << endreq;
    return StatusCode::FAILURE;
  }
  detStore()->retrieve(m_pSCTHelper,"SCT_ID");

  // Services for Summary Histograms: SCT_ModuleConditionsTool from CondDB and flagged conditions service
  msg(MSG::INFO) << "Checking for CondDB" << endreq;
  if (m_ConfigurationSvc.retrieve().isFailure()) msg(MSG::ERROR) << "Conditions summary service NOT found!" << endreq; 
  msg(MSG::INFO) << "Checking for Flagged Service" << endreq;
  if (m_flaggedSvc.retrieve().isFailure()) msg(MSG::ERROR) << "Flagged service NOT found!" << endreq;

  // get a handle on the histogramming service //
  if (m_thistSvc.retrieve().isFailure())  msg(MSG::ERROR) << "Unable to retrieve pointer to THistSvc"  << endreq;

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
  if (m_byteStreamErrSvc.retrieve().isFailure()) return msg(MSG::FATAL) << "Failed to retrieve service " << m_byteStreamErrSvc << endreq, StatusCode::FAILURE;
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved service " << m_byteStreamErrSvc << endreq;

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
    ATH_MSG_ERROR( "Could not retrieve event info!" );
    return StatusCode::RECOVERABLE; 
  }
  m_current_lb = pEvent->event_ID()->lumi_block();
  if (fillByteStreamErrors().isFailure() ) {
    ATH_MSG_ERROR("Could not fill ByteStreamErrors ");
    return StatusCode::FAILURE;
  }  
  const SCT_RDO_Container* p_rdocontainer;
  if (evtStore()->retrieve(p_rdocontainer,m_dataObjectName).isFailure()){
    ATH_MSG_ERROR("Could not get RDO collection in SCTErrMonTool :: fillHistograms " << numberOfEvents);
    return StatusCode::FAILURE; 
  }
  Identifier SCT_Identifier;

  //Define variables for error histograms
  int numFirstHit=0,     numSecondHit=0,      numSCTRDOs=0;
  int numFirstHit_ECp=0, numSecondHit_ECp=0,  numSCTRDOs_ECp=0;
  int numFirstHit_ECm=0, numSecondHit_ECm=0,  numSCTRDOs_ECm=0;

  //Use new IDC
  SCT_RDO_Container::const_iterator lastCol  = p_rdocontainer->end();
  for(SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin(); col_it!= lastCol ; ++col_it){
    const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
    if (!SCT_Collection) continue;  // select only SCT RDOs
    DataVector<SCTRawDataType>::const_iterator p_rdo_end= SCT_Collection->end();
    for(DataVector<SCTRawDataType>::const_iterator p_rdo=SCT_Collection->begin() ; p_rdo!=p_rdo_end; ++p_rdo){
      SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*p_rdo);
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

  if(numSCTRDOs!=0){
    double scale = 100./double(numSCTRDOs);
    FirstHit->Fill( double(numFirstHit)*scale ,1.); SecondHit->Fill( double(numSecondHit)*scale ,1.);
  }
  if(numSCTRDOs_ECp!=0 && m_doPositiveEndcap){
    double scale = 100./double(numSCTRDOs_ECp);
    FirstHit_ECp->Fill( double(numFirstHit_ECp)*scale ,1.); SecondHit_ECp->Fill( double(numSecondHit_ECp)*scale ,1.);
  }
  if(numSCTRDOs_ECm!=0 && m_doNegativeEndcap){
    double scale = 100./double(numSCTRDOs_ECm);
    FirstHit_ECm->Fill( double(numFirstHit_ECm)*scale ,1.); SecondHit_ECm->Fill( double(numSecondHit_ECm)*scale ,1.);
  }
  if(m_environment==AthenaMonManager::online){
    if ( numberOfEvents==1 or (numberOfEvents > 1 && (numberOfEvents % m_checkrate) == 0 )){
      if ( resetCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in resetCondDBMaps()" << endreq;
      if ( fillCondDBMaps().isFailure() )  msg(MSG::WARNING) << "Error in fillCondDBMaps()" << endreq;
      if ( (numberOfEvents % (m_checkrate*10)) == 0 ) { 
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
      float content = 0; float cxb = 0; float cyb = 0; int evt = numberOfEvents; int evt_lumi = numberOfEventsLumi;
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
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "finalHists()" << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Total Rec Event Number: " << numberOfEvents << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling checkHists(true); true := end of run" << endreq;
    if (checkRateHists().isFailure())
      msg(MSG::WARNING) << "Error in checkRateHists()" << endreq ;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Exiting finalHists" << endreq;
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
  int maskedlink_err[4]={0,0,0,0};
  int robfragment_err[4]={0,0,0,0};
  int tot_err[4]={0,0,0,0};
  int tot_mod_err[4]={0,0,0,0};

  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::MaskedLink), maskedlink_err[3],maskedlink_err[0],maskedlink_err[1],maskedlink_err[2]);
  numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError), robfragment_err[3],robfragment_err[0],robfragment_err[1],robfragment_err[2]);
  for (int reg=0; reg<4;++reg) {
    m_MaskedLinksVsLB[reg]->Fill(current_lb,double (maskedlink_err[reg]));
    m_ROBFragmentVsLB[reg]->Fill(current_lb,double (robfragment_err[reg]));
  }

  m_MaskedLinks->Reset();
  int total_errors = 0; float cxb = 0; float cyb = 0; 
    
  int evt = numberOfEvents; int content = 0;

    
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
    SCT_ByteStreamErrors::TruncatedROD,SCT_ByteStreamErrors::ROBFragmentError,SCT_ByteStreamErrors::ByteStreamParseError 
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
        if (numberOfEvents%m_checkrate == 0 && m_runOnline == true) {
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
            if (numberOfEvents%m_checkrate == 0 && m_runOnline == true) {
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
    m_NumberOfErrorsVsLB[reg]->Fill(current_lb,double (tot_err[reg]));
    m_ModulesWithErrorsVsLB[reg]->Fill(current_lb,double (tot_mod_err[reg]));
  }

  if(m_environment==AthenaMonManager::online){
    // Time Dependent SP plots only online
    nErrors_buf[nErrors_pos] = total_errors;
    nErrors_pos++;
    if (nErrors_pos == m_evtsbins) nErrors_pos = 0;
    if (numberOfEvents % m_checkrate == 0) {
      m_nErrors->Reset(); Int_t latest_nErrors_pos = nErrors_pos;
      for (Int_t i=1; i < m_evtsbins; i++) {
        if (latest_nErrors_pos == m_evtsbins) latest_nErrors_pos = 0;
        if (numberOfEvents < m_evtsbins){
          if (i < nErrors_pos) m_nErrors->SetBinContent(i,nErrors_buf[i]);
          else m_nErrors->SetBinContent(i,0);
        } else {
          m_nErrors->SetBinContent(i,nErrors_buf[latest_nErrors_pos]);
          m_nErrors->GetXaxis()->Set(m_evtsbins,numberOfEvents-m_evtsbins,numberOfEvents);
        }
        latest_nErrors_pos++; 
        if (latest_nErrors_pos == m_evtsbins) latest_nErrors_pos = 0;
      }
    }
  }
  numberOfEvents++;
  numberOfEventsLumi++;
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
  const std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Premable","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse","summary"};
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
            std::string name1 = m_errorsNames[errType] + "ErrsPerLumi";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsPerLumi_";
            somethingFailed |=bookErrHistosHelper(lumiErr,name1,title,name2,m_allErrsPerLumi[errType][iBARREL][layer],m_pallErrsPerLumi[errType][iBARREL][layer],layer).isFailure();
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
      size_t nErrors_buf_size;
      nErrors_buf_size = m_evtsbins * sizeof (int);
      nErrors_buf = (int *) malloc (nErrors_buf_size);
      nErrors_pos = 0;
      if(Errors.regHist(m_nErrors).isFailure())    msg(MSG::WARNING) << "Couldn't book nErrors vs event number hist" << endreq;
      //Book percentage error histograms      
      FirstHit = new TH1F("FirstHit","Percentage of FirstHit errors", nbins,0.,100.);
      FirstHit->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
      SecondHit = new TH1F("SecondHit","Percentage of SecondHit errors", nbins,0.,100.);
      SecondHit->GetXaxis()->SetTitle("Percentage of SecondHit errors");
      if( err.regHist(FirstHit).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
      if( err.regHist(SecondHit).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;
      bool somethingFailed(false);
      for (int layer(0); layer!=N_BARRELSx2;++layer) {
        for (int errType(0);errType!=N_ERRTYPES;++errType){
          std::string name1 = m_errorsNames[errType] + "Errs_";
          std::string title = m_errorsNames[errType] + " errors layer ";
          std::string name2 = std::string("T") + m_errorsNames[errType] + "Errs_";
          somethingFailed |=bookErrHistosHelper(err,name1,title,name2,m_allErrs[errType][iBARREL][layer],m_pallErrs[errType][iBARREL][layer],layer).isFailure();
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
  std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse", "summary"};
  if(m_doPerLumiErrors) {
    MonGroup lumiErr(this,"SCT/SCTEA/errors",lumiBlock,ATTRIB_UNMANAGED );
    if(ManagedMonitorToolBase::newLumiBlock) {
      std::string m_layerNames[N_DISKSx2] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
      m_numErrorsPerLumi[iECp] = new TH2F("NumErrsPerLumi","Total Number of Error Types for Layer per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_numErrorsPerLumi[iECp]).isFailure()) msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endreq;
      m_rateErrorsPerLumi[iECp] = new TProfile2D("RateErrorsPerLumi","Rate of Error Types for Layers per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
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
            std::string name1 = m_errorsNames[errType] + "ErrsECpPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECpPerLumi_";
            failedBooking |=bookErrHistosHelper(lumiErr,name1,title,name2,m_allErrsPerLumi[errType][iECp][layer],m_pallErrsPerLumi[errType][iECp][layer],layer).isFailure();
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
    FirstHit_ECp = new TH1F("FirstHit_ECp","Percentage of FirstHit errors in positive endcap", nbins,0.,100.);
    FirstHit_ECp->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
    SecondHit_ECp = new TH1F("SecondHit_ECp","Percentage of SecondHit errors in positive endcap", nbins,0.,100.);
    SecondHit_ECp->GetXaxis()->SetTitle("Percentage of SecondHit errors"); 
    if ( err.regHist(FirstHit_ECp).isFailure() )
      msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
    if ( err.regHist(SecondHit_ECp).isFailure() )
      msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;

    bool failedBooking(false); 
    for (int layer(0); layer!=N_DISKSx2;++layer) {
      for (int errType(0);errType!=N_ERRTYPES;++errType){
        std::string name1 = m_errorsNames[errType] + "ErrsECp_";
        std::string title = m_errorsNames[errType] + " errors layer ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECp_";
        failedBooking |=bookErrHistosHelper(err,name1,title,name2,m_allErrs[errType][iECp][layer],m_pallErrs[errType][iECp][layer],layer).isFailure();
        m_allErrs[errType][iECp][layer]->GetXaxis()->SetTitle("Index in the direction of #eta"); 
        m_allErrs[errType][iECp][layer]->GetYaxis()->SetTitle("Index in the direction of #phi"); 
      }

      if(m_environment==AthenaMonManager::online) failedBooking |= bookErrHistosHelper(err,"summaryErrsRecentECp_","summary recent Layer ",m_summaryErrsRecent[iECp][layer],layer,false).isFailure();
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
  std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Preamble","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse", "summary"};
  if(m_doPerLumiErrors) {
    MonGroup lumiErr(this,"SCT/SCTEC/errors",lumiBlock,ATTRIB_UNMANAGED );
    if(ManagedMonitorToolBase::newLumiBlock) {
      std::string m_errorsNames[] = {"ABCD","Raw","TimeOut","LVL1ID","BCID","Premable","Formatter","MaskedLink","RODClock","TruncROD","ROBFrag","BSParse"};
      std::string m_layerNames[N_DISKSx2] = {"0_0","0_1","1_0","1_1","2_0","2_1","3_0","3_1","4_0","4_1","5_0","5_1","6_0","6_1","7_0","7_1","8_0","8_1"};
      m_numErrorsPerLumi[iECm] = new TH2F("NumErrsPerLumi","Total Number of Error Types for Layer per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
      if(lumiErr.regHist(m_numErrorsPerLumi[iECm]).isFailure()) msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endreq;
      m_rateErrorsPerLumi[iECm] = new TProfile2D("RateErrorsPerLumi","Rate of Error Types for Layers per Lumi-Block",n_lumiErrBins,-0.5,n_lumiErrBins-0.5,N_ENDCAPSx2,-0.5,N_ENDCAPSx2-0.5);
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
            std::string name1 = m_errorsNames[errType] + "ErrsECmPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECmPerLumi_";
            failedBooking |=bookErrHistosHelper(lumiErr,name1,title,name2,m_allErrsPerLumi[errType][iECm][layer],m_pallErrsPerLumi[errType][iECm][layer],layer).isFailure();
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
    FirstHit_ECm = new TH1F("FirstHit_ECm","Percentage of FirstHit errors in negative endcap", nbins,0.,100.);
    FirstHit_ECm->GetXaxis()->SetTitle("Percentage of FirstHit errors"); 
    SecondHit_ECm = new TH1F("SecondHit_ECm","Percentage of SecondHit errors in negative endcap", nbins,0.,100.);
    SecondHit_ECm->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if ( err.regHist(FirstHit_ECm).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endreq;
    if ( err.regHist(SecondHit_ECm).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endreq;

    //int limit = 2*n_disks;
    bool failedBooking(false);
    for (int layer(0); layer!=N_DISKSx2;++layer) {
      for (int errType(0);errType!=N_ERRTYPES;++errType){
        std::string name1 = m_errorsNames[errType] + "ErrsECm_";
        std::string title = m_errorsNames[errType] + " errors layer ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECm_";
        failedBooking |=bookErrHistosHelper(err,name1,title,name2,m_allErrs[errType][iECm][layer],m_pallErrs[errType][iECm][layer],layer).isFailure();
        m_allErrs[errType][iECm][layer]->GetXaxis()->SetTitle("Index in the direction of #eta"); 
        m_allErrs[errType][iECm][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
      }
      if(m_environment==AthenaMonManager::online) failedBooking |= bookErrHistosHelper(err,"summaryErrsRecentECm_","summary recent Layer ",m_summaryErrsRecent[iECm][layer],layer,false).isFailure();
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
    //    std::string streamName = streamNameFunction()->getStreamName(ConfHist,"");
    std::string streamName = streamNameFunction()->getStreamName(this,ConfHist,"",false); // Hidetoshi Otono 14.03.05
    m_path = streamName.substr(0, streamName.rfind("SCT/GENERAL/Conf"));
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Global Path :" << m_path << endreq;

    //std::string m_SummaryBinNames[] = {"Mod Out","Flagged Links","Masked Links","Errors","Inefficient","Noisy"}; 
    std::string m_ConfigurationBinNames[] = {"Modules","Link 0","Link 1","Chips","Strips (10^{2})"};
    std::string m_ConfigurationOnlineBinNames[] = {"Mod Out","Flagged Links","Masked Links","Errors"};
    std::string m_ConfigurationEffBinNames[] = {"Ineff B","Ineff EA","Ineff EC","ALL"};

    std::string m_ConfigurationNoiseBinNames[] = {"Noisy B","Noisy EA","Noisy EC","ALL"};

    m_p2DmapHistoVectorAll[iBARREL].clear();
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
        TString conflabel[4]={"Num of Problematic Module in Barrel","Num of Problematic Module in EndcapA", 
                                      "Num of Problematic Module in EndcapC","Num of Problematic Module in All Region"};//30.11.2014 
        TString confonlinetitle[4]={"SCTOnlineConfBarrel","SCTOnlineConfEndcapA","SCTOnlineConfEndcapC","SCTOnlineConf"};
        TString confefftitle="SCTEffConf";

        TString confnoisetitle="SCTNoiseConf";
        TString confnoisetitle_recent="SCTNoiseConfRecent";
        TString maskedlinktitle[4]={"SCTMaskedLinkConfBarrel","SCTMaskedLinkConfEndcapA","SCTMaskedLinkConfEndcapC","SCTMaskedLinkConf"};
        TString robfragtitle[4]={"SCTROBFragmentConfBarrel","SCTROBFragmentConfEndcapA","SCTROBFragmentConfEndcapC","SCTROBFragmentConf"};
        TString numerrors[4]={"SCTNumberOfErrorsBarrel","SCTNumberOfErrorsEndcapA","SCTNumberOfErrorsEndcapC","SCTNumberOfErrors"};
        TString moderrors[4]={"SCTModulesWithErrorsBarrel","SCTModulesWithErrorsEndcapA","SCTModulesWithErrorsEndcapC","SCTModulesWithErrors"};
        for(int reg=0; reg<4; reg++){
          m_Conf[reg] = new TProfile(conftitle[reg],conflabel[reg],Confbins,-0.5,Confbins-0.5);//30.11.2014           for (int bin = 0; bin<Confbins; bin++) m_Conf[reg]->GetXaxis()->SetBinLabel(bin+1,m_SummaryBinNames[bin].c_str());
          const int conf_online_bins = 4;
          if(m_environment==AthenaMonManager::online or testOffline){
            m_ConfOnline[reg]=new TH1F(confonlinetitle[reg],conflabel[reg]+" Online",conf_online_bins,-0.5,conf_online_bins-0.5);//30.11.2014 
            for (int bin = 0; bin<conf_online_bins; bin++) m_ConfOnline[reg]->GetXaxis()->SetBinLabel(bin+1,m_ConfigurationOnlineBinNames[bin].c_str());
          }
          m_MaskedLinksVsLB[reg] = new TProfile(maskedlinktitle[reg],"Average number of masked link errors per event vs. lumiblock",n_lumiBins,0.5,n_lumiBins+0.5);
          m_MaskedLinksVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_ROBFragmentVsLB[reg] = new TProfile(robfragtitle[reg],"Average number of ROB fragment errors per event vs. lumiblock",n_lumiBins,0.5,n_lumiBins+0.5);
          m_ROBFragmentVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_NumberOfErrorsVsLB[reg] = new TProfile(numerrors[reg],"Average number of errors per event vs. lumiblock",n_lumiBins,0.5,n_lumiBins+0.5);
          m_NumberOfErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
          m_ModulesWithErrorsVsLB[reg] = new TProfile(moderrors[reg],"Average number of modules with errors per event vs. lumiblock",n_lumiBins,0.5,n_lumiBins+0.5);
          m_ModulesWithErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock"); 
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
      if ( ConfHist.regHist(m_NumberOfErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfMaps.regHist(m_NumberOfErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECA.regHist(m_NumberOfErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHistECC.regHist(m_NumberOfErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTNumberOfErrors" << endreq;
      if ( ConfHist.regHist(m_ModulesWithErrorsVsLB[3]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfMaps.regHist(m_ModulesWithErrorsVsLB[0]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECA.regHist(m_ModulesWithErrorsVsLB[1]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
      if ( ConfHistECC.regHist(m_ModulesWithErrorsVsLB[2]).isFailure() ) msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endreq;
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
             Form("Module Out of Configuration : Disk %d Side +%d",i/2,i%2),  // title
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
             Form("Module Out of Configuration : Disk %d Side -%d",i/2,i%2),  // title
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
    //endcap A
    failedbooking=getHisto(lyr, iECp, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iECp, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }

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
      m_Conf[reg]->Fill(1.,double (Flagged[reg]));
      m_Conf[reg]->Fill(2.,double (MaskedLinks[reg]));
      m_Conf[reg]->Fill(3.,double (ModErr[reg]));
      m_Conf[reg]->Fill(4.,double (InEffModules[reg]));
      m_Conf[reg]->Fill(5.,double (NoisyModules[reg]));
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

