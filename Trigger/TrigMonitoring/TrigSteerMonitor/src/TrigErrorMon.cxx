/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/TrigSteer.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <vector>
#include <algorithm>
#include <TH1I.h>
#include <TH2I.h>
#include "TrigErrorMon.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace HLT;

TrigErrorMon::TrigErrorMon(const std::string & type, const std::string & name,
			 const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent)
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("expertMode", m_expertMode=false, "If set to 'true' includes also errors when HLT::Action == CONTINUE and errors eta/phi");
}


/* ******************************** initialize ************************************** */
StatusCode TrigErrorMon::initialize()
{
  ATH_CHECK(TrigMonitorToolBase::initialize());

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if ( !m_parentAlg ) {
    ATH_MSG_ERROR(" Unable to cast the parent algorithm to HLT::TrigSteer !");
    return StatusCode::FAILURE;
  }

  m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;
  
  return StatusCode::SUCCESS;
}


/* ******************************** bookHists  ************************************** */

StatusCode TrigErrorMon::bookHists()
{
  ATH_CHECK(bookHistograms( false, false, true ));
  return StatusCode::SUCCESS;
}




/* ******************************** book Histograms ************************************* */
StatusCode TrigErrorMon::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{
  TrigMonGroup expertHistograms( this, boost::replace_all_copy(name(), ".", "/" ), expert );
  
  //retrieve chain names from configuredChains
  std::vector<const HLT::SteeringChain*> cc = m_parentAlg->getConfiguredChains();
  unsigned int totalNChains = cc.size();

  // store chainnames and binnumber in maps
  std::vector<std::string> chainname(totalNChains);
  int binnr=0;
  for (const HLT::SteeringChain* chain : cc) {
    unsigned int chaincounter = chain->getChainCounter();
    // fill a map for consecutive numbering, this avoids empty bins, m_binmap starts at 0 !
    // fill array with ChainNames  - chainname starts at 0 !
    chainname[binnr] = chain->getChainName();
    m_binmap[chaincounter] = binnr++;
  }
  
  //resete private members 
  m_histo_reason=0;
  m_histo_action=0;
  m_histo_steeringInternalReason=0;
  m_histo2d_reason=0;
  m_histo2d_action=0;
  
  //get number of error codes
  ErrorCode eclast=HLT::LAST_ERROR;
  int nmaxcode_reason=eclast.reason();
  int nmaxcode_action=eclast.action();
  int nmaxcode_sir=eclast.steeringInternalReason();
  
  // reason and ateeringInternalReason ec=0 => "", start from ec=1
  // action "CONTINUE" start from 0 
  
  //book 3 1D histos  for the 3  types of ErrorCode namely for action, reason and steeringinternalreason


  TString htitle = TString("Errors reason total count ")+TString(m_trigLvl); 

  m_histo_reason =  new TrigLBNHist<TH1I>(TH1I( "ErrorsReasonTotalCount",htitle.Data(),nmaxcode_reason+1, -0.5, nmaxcode_reason+0.5));
  

  htitle=TString("Errors action total count ")+TString(m_trigLvl);
  m_histo_action = new TrigLBNHist<TH1I>(TH1I( "ErrorsActionTotalCount",htitle.Data(),nmaxcode_action+1, -0.5, nmaxcode_action+0.5));

  htitle=TString("Errors steering internal reason  total count ")+TString(m_trigLvl);
  m_histo_steeringInternalReason = new TrigLBNHist<TH1I>(TH1I( "ErrorsSteeringInternalReasonTotalCount",htitle.Data(),nmaxcode_sir+1, -0.5, nmaxcode_sir+0.5));
  
  //book 2 2d histos for  ErrorCode vs chain for action and reason
 
  
  htitle=TString("Errors reason per chain ")+TString(m_trigLvl);
  m_histo2d_reason =  new TH2I( "ErrorsReasonPerChain",htitle.Data(),
				 totalNChains, -0.5, totalNChains-0.5,
				 nmaxcode_reason+1, -0.5, nmaxcode_reason+0.5);
  
  htitle=TString("Errors action per chain ")+TString(m_trigLvl);
  m_histo2d_action =  new TH2I( "ErrorsActionPerChain",htitle.Data(),
				 totalNChains, -0.5, totalNChains-0.5,
				 nmaxcode_action+1, -0.5, nmaxcode_action+0.5);
  

  
  //add axis labels
  m_histo2d_reason->GetYaxis()->SetTitle("ErrorCode");
  m_histo2d_reason->GetXaxis()->SetTitle("chains");
  m_histo2d_action->GetYaxis()->SetTitle("ErrorCode");
  m_histo2d_action->GetXaxis()->SetTitle("chains");
  //make histos nice 
  m_histo2d_reason->GetYaxis()->SetTitleOffset(3);
  m_histo2d_action->GetYaxis()->SetTitleOffset(3);

  
  // labeling the ErrorCode bins
  // loop errorcodes 
  for(int i=0;i<= max(max(nmaxcode_reason,nmaxcode_sir),nmaxcode_action);i++){
    //retrieve ErrorCodes
    ErrorCode ec=HLT::ErrorCode((HLT::Action::Code) min(i,nmaxcode_action),
				(HLT::Reason::Code) min(i,nmaxcode_reason),
				(HLT::SteeringInternalReason::Code) min(i,nmaxcode_sir));
    //get strings
    std::string sirName=strErrorCode(ec);
    std::string actionName,reasonName;
    string::size_type loc1=strErrorCode(ec).find(" ");
    string::size_type loc2=strErrorCode(ec).find_last_of(" ");
    //retrieve action, reason and steeringinternalreason code 
    if (loc1 ==std::string::npos  || loc1==loc2){
      ATH_MSG_WARNING ("  can not identify Action, Reason and SteeringInternalReason from strErrorCode ");
    }else{
      actionName=sirName.substr(0,loc1);
      reasonName=sirName.substr(loc2);
      sirName=sirName.substr(loc1,loc2-loc1);
    }

    // set bin labels, the first bin is UNKNOWN. (the zeroth bin is the underflow bin)
    // and from the second bin on, the knowns
    if(i>=1 && i<=nmaxcode_reason){
      if ( i==1 ){
	m_histo_reason->  GetXaxis()->SetBinLabel(i, "UNKNOWN");
	m_histo2d_reason->GetYaxis()->SetBinLabel(i, "UNKNOWN");
      }
      m_histo_reason->  GetXaxis()->SetBinLabel(i+1, reasonName.c_str());
      m_histo2d_reason->GetYaxis()->SetBinLabel(i+1, reasonName.c_str());
    }
    if(i>=1 && i<=nmaxcode_action){
      if ( i==1 ){
	m_histo_action->GetXaxis()->SetBinLabel(i, "CONTINUE");
	m_histo2d_action->GetYaxis()->SetBinLabel(i, "CONTINUE");
      }
      m_histo_action->GetXaxis()->SetBinLabel(i+1, actionName.c_str());
      m_histo2d_action->GetYaxis()->SetBinLabel(i+1, actionName.c_str());
    }
    if(i>=1 && i<=nmaxcode_sir){
      if ( i==1 ){
	m_histo_steeringInternalReason->GetXaxis()->SetBinLabel(i, "UNKNOWN");
      }
      m_histo_steeringInternalReason->GetXaxis()->SetBinLabel(i+1, sirName.c_str());
    }
  }
  
  // naming the chain bins of 2D histos  using map chain name
  for (unsigned int tmpbin=1;tmpbin<totalNChains+1;tmpbin++) {
    m_histo2d_action->GetXaxis()->SetBinLabel(tmpbin  , chainname[tmpbin-1].c_str());
    m_histo2d_reason->GetXaxis()->SetBinLabel(tmpbin  , chainname[tmpbin-1].c_str());
  }

  
  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_reason).isFailure())
    msg() << MSG::WARNING << "Can't book "
	     <<  m_histo_reason->GetName() << endmsg;

  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_action).isFailure())
    msg() << MSG::WARNING << "Can't book "
	     << m_histo_action->GetName() << endmsg;

  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_steeringInternalReason).isFailure())
    msg() << MSG::WARNING << "Can't book "
	     << m_histo_steeringInternalReason->GetName() << endmsg;
  
  if ( expertHistograms.regHist(m_histo2d_reason).isFailure())
    msg() << MSG::WARNING << "Can't book "
	     <<  m_histo2d_reason->GetName() << endmsg;
  
  if ( expertHistograms.regHist(m_histo2d_action).isFailure())
    msg() << MSG::WARNING << "Can't book "
	     << m_histo2d_action->GetName() << endmsg;


  if ( ! m_expertMode )
    return StatusCode::SUCCESS;

  // book histo with eta-phi map of TEs errors
  m_histo_te_errors_etaphi = new TH2I("TE_Errors_All",
      "#eta vs #varphi of TEs errors", 64, -4.0, 4.0, 64,  -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_te_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_te_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  if ( expertHistograms.regHist(m_histo_te_errors_etaphi).isFailure()) {
    msg() << MSG::WARNING << "Can't book "
	     <<  m_histo_te_errors_etaphi->GetName() << endmsg;
  }

  return StatusCode::SUCCESS;
}


bool need_fill(const HLT::ErrorCode& ec, bool includeall) {
  if (includeall)
    return true;
  else 
    return ec.action() > HLT::Action::CONTINUE;
}

/* ******************************** fill Histograms ************************************** */
StatusCode TrigErrorMon::fillHists()
{
  
  ErrorCode ec ;
  ec = m_parentAlg->getAlgoConfig()->getHltStatus() ;
  
  if( !m_histo_action || !m_histo_reason || !m_histo_steeringInternalReason 
      || !m_histo2d_reason || !m_histo2d_action ){
    msg()<<MSG::WARNING<<" pointers to histograms not ok, dont Fill ! "<<endmsg;
    return StatusCode::FAILURE;
  }


  if ( ec == HLT::OK ) return StatusCode::SUCCESS;

  if ( need_fill(ec, m_expertMode) ) { 
    m_histo_action->Fill((int) ec.action() );
    m_histo_reason->Fill((int) ec.reason() );
    m_histo_steeringInternalReason->Fill((int) ec.steeringInternalReason() );
  } else {
    return StatusCode::SUCCESS;
  }

  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();
 
  for (std::vector<const HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
       chain != activeChains.end(); ++chain) {
    Int_t chaincounter = (Int_t)(*chain)->getChainCounter();
    ErrorCode ecchain=(ErrorCode) (*chain)->getErrorCode();
    if ( ecchain != HLT::OK ) {
      if ( need_fill(ecchain, m_expertMode) ){
	     m_histo2d_reason->Fill(m_binmap[chaincounter],(int) ecchain.reason());
	     m_histo2d_action->Fill(m_binmap[chaincounter],(int) ecchain.action());	
      }
    }
  }

  if ( ! m_expertMode )
    return StatusCode::SUCCESS;

  // Get TEs
  std::vector<HLT::TriggerElement*> TEs;
  m_parentAlg->getAlgoConfig()->getNavigation()->getAll(TEs, true);

  std::vector<HLT::TriggerElement*>::const_iterator te;

  // Loop over TEs
  for (te = TEs.begin(); te != TEs.end(); ++te) {

    // If TE contains errros fill histogram with eta and phi of this TE 
    if ( (*te)->getErrorState() ) {

      // Get trigger ROI descriptor
      std::vector<const TrigRoiDescriptor*> roiDescriptor;
      if (  m_parentAlg->getAlgoConfig()->getNavigation()->getRecentFeatures( 
            (*te), roiDescriptor, "initialRoI")  == false ) {
        continue;
      }

      std::vector<const TrigRoiDescriptor*>::const_iterator roiDescriptorIt;
      for ( roiDescriptorIt = roiDescriptor.begin(); roiDescriptorIt != roiDescriptor.end(); ++roiDescriptorIt){
        if (*roiDescriptorIt == 0)  {
          continue;
        }

        float eta = (*roiDescriptorIt)->eta();
        float phi = (*roiDescriptorIt)->phi();

        m_histo_te_errors_etaphi->Fill( eta, phi );

      }
    }
  }

  return StatusCode::SUCCESS;
}



