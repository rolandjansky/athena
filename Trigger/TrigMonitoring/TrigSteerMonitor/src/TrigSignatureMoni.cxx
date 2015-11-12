/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"

#include "TrigSteerMonitor/TrigSignatureMoni.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/ResultBuilder.h"
#include "TrigSteering/Signature.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>
#include <sstream>
#include <limits>
#include <vector> 
#include <algorithm>
#include <TH1I.h>
#include <TH2I.h>
#include <map>
#include <typeinfo>


TrigSignatureMoni::TrigSignatureMoni(const std::string & type, const std::string & name,
				     const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),   
     m_trigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
     m_histoPathexpert(""),
     m_parentAlg(0),
     m_generalBlock(0),
     m_streamBlock(0),
     m_groupBlock(0),
     m_chainBlock(0),
     m_streamCorrel(0),
     m_chainlengthHist(0),
     m_lumiBlockNumDiffHist(0),
     m_stepForEBHist(0),
     m_signatureAcceptanceHist(0),
     m_eventsPassingStepHist(0),
     m_totaleventsPassingStepHist(0),
     m_trigLvl(""),
     m_highestLBN(0)
  
{
  declareProperty("HistoPathexpert", m_histoPathexpert = "/EXPERT/TrigSteering");
  declareProperty("EventsPerLBHack",m_eventsPerLBHack=10000);
}

StatusCode TrigSignatureMoni::finalize()
{
  return StatusCode::SUCCESS;
}

// Define helper classes for the signature acceptance histogram (and stream correlation plot)

class BinBlock;
template class std::vector<BinBlock*>;

// This is needed since static variables get mixed between L2 and EF in athena
// So define one for each level.
struct staticVariables {
  int firstSigHistBin;
  std::string unknown;
  std::vector<BinBlock *> binBlocks;
  std::vector<std::string> stepnames;
  unsigned int maxSteps; 
  unsigned int totalBins;
  unsigned int inputBin;
  unsigned int rawBin;
  unsigned int psBin;
  unsigned int firstStepBin;
  unsigned int rerunBin;
  unsigned int algoInBin;
  unsigned int errorBin;
  unsigned int ptBin;
};

enum bb_triglevel {tl_L2, tl_EF, tl_HLT};


class BinBlock {
  
public:
  virtual ~BinBlock();
  int GetFirstBin() {return m_firstBin;} // first bin of a block
  const std::string &GetXLab(int relBin){return m_XLabels[relBin];}

  static int GetFirstSigHistBin(){ return m_sv[m_tl].firstSigHistBin;} // first bin of the signature hist
  static int TotalBinCount() {return m_sv[m_tl].totalBins;}
  static int OverflowBin() { return m_sv[m_tl].totalBins + 1;}
  static const std::string &GetSigXAxisLabel(int bin); 
  static void FinalizeInitialization();
  //  static void PrintBlocks();
  static void PrepareToBook(enum bb_triglevel tl);
  static void SetLevel(enum bb_triglevel tl){ m_tl = tl; }

  static const std::vector<std::string> &GetStepNames(){return m_sv[m_tl].stepnames;}
  static int GetNStepBins() {return m_sv[m_tl].stepnames.size();}
  static unsigned int GetMaxSteps(){return m_sv[m_tl].maxSteps;} // max num of algorithm steps
  static unsigned int GetInputBin(){return m_sv[m_tl].inputBin;}
  static unsigned int GetRawBin(){return m_sv[m_tl].rawBin;}
  static unsigned int GetPSBin(){ return m_sv[m_tl].psBin;}
  static unsigned int GetPTBin(){ return m_sv[m_tl].ptBin;}
  static unsigned int GetFirstAlgBin(){return m_sv[m_tl].firstStepBin;}
  static unsigned int GetRerunBin(){return m_sv[m_tl].rerunBin;}
  static unsigned int GetAlgoInBin(){return m_sv[m_tl].algoInBin;}
  static unsigned int GetErrorBin(){return m_sv[m_tl].errorBin;}
  unsigned int GetNBins() {return m_NBins;}
  virtual void PrintBlock() = 0;

protected:


  BinBlock(BinBlock *, MsgStream *log);


  static void SetMaxSteps(unsigned int max) {m_sv[m_tl].maxSteps = max;}

  std::map<int, std::string> m_XLabels;
  unsigned int m_NBins;
  int m_firstBin;

  MsgStream *m_log;
  
  static enum bb_triglevel m_tl; // the trigger level
  static struct staticVariables m_sv[3]; // one for each trigger level 
  
private:
};
// storage for statics;
enum bb_triglevel BinBlock::m_tl;
struct staticVariables BinBlock::m_sv[3]; 

class GeneralBlock : public BinBlock { // General event columns
  
public:
  GeneralBlock(MsgStream *log) : BinBlock(this, log)
  {
    m_XLabels[0] = "total rate";
    m_XLabels[1] = "OR of active chains";
    m_allBin =0;
    m_orBin = 1;
    m_NBins = 2;
  }
  ~GeneralBlock(){;}
  int GetOrBin(){return GetFirstSigHistBin() + m_firstBin + m_orBin;}
  int GetAllEvsBin(){return GetFirstSigHistBin() + m_firstBin + m_allBin;}

  void PrintBlock() {
    (*m_log) << MSG::DEBUG << "**** General Block ****" << endreq;
    (*m_log) << MSG::DEBUG << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endreq;
    (*m_log) << MSG::DEBUG << ", allBin: "  << m_allBin   << ", orBin: " << m_orBin << endreq;
  }
  
private:

  int m_allBin;
  int m_orBin;
};



class GroupBlock: public BinBlock { // columns for groups
  
public:
  GroupBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log);
  ~GroupBlock(){;}
  const std::set<int> &GetBinSet(unsigned int chainCounter);
  void SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap);

  virtual void PrintBlock();
  
private:

  std::map<unsigned int, std::set<int> > m_ch_bin_map; // mapping from chain counter to bin
  std::set<int> m_emptyList;
};

class ChainBlock: public BinBlock { // columns for chains
  
public:
  ChainBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log);
  ~ChainBlock(){;}
  int GetSigHistValue(unsigned int chainCounter); // to be used by Fill for example
  int GetSigHistBin(unsigned int chainCounter); // to be used by SetBinContent for example

  virtual void PrintBlock();

private:
  std::map<unsigned int, int> m_ch_bin;
};

class StreamBlock : public BinBlock { // columns for streams
  
public:

  StreamBlock(const TrigConf::HLTChainList* allChains,
              std::vector<std::string> &errorStreamNames, std::string trigLvl,
              MsgStream *log);

  ~StreamBlock(){;}
  const std::set<int> &GetSigHistValueSet(unsigned int chainCounter);
  int GetSigHistValue(std::string streamTag);
  int GetCorrelHistBin(std::string streamTag);
  int GetCatchAllBin(){return m_catchAll;}

  void SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap);
  static void extractErrorStreamNames( std::string errStreamString, 
				       std::vector<std::string> &errorStreamNames );

  virtual void PrintBlock();  
private:

  void InsertStreams(bool isPhysType, std::string trgLvl,
                     const TrigConf::HLTChainList* allChains);

  std::map<std::string, int> m_str_tag_map; // mapping from tag name (name_type) to bin
  std::map<unsigned int, std::set<int> > m_ch_str_map; // mapping from chain counter to bin
  std::set<int> notFound;
  int m_catchAll;
};


TrigSignatureMoni::~TrigSignatureMoni()
{
  enum bb_triglevel tl =  m_trigLvl == "L2" ? tl_L2 :  m_trigLvl == "EF" ? tl_EF : tl_HLT;
  BinBlock::PrepareToBook(tl);

}

bool TrigSignatureMoni::getDebugStreams(std::vector<std::string> &errorStreamNames)
{
  /* 
     If this function fails for any reason, any debug streams which have been defined
     in the ResultBuilder's job options will not be added to the histograms. That's
     the only consequence. So I remove most warnings.
  */

  // Set up the job service
  IJobOptionsSvc* jobOptionsSvc;
  StatusCode sc;
  
  sc = service("JobOptionsSvc", jobOptionsSvc);

  if(sc.isFailure()) {
    ATH_MSG_WARNING("Could not find JobOptionsSvc");
    return false;
  }      
  const std::vector<const Property*> *rb_properties;
  
  try {
    if(m_trigLvl == "L2")
      rb_properties = jobOptionsSvc->getProperties("TrigSteer_L2.ResultBuilder");
    else if (m_trigLvl == "EF")
      rb_properties = jobOptionsSvc->getProperties("TrigSteer_EF.ResultBuilder");
    else 
      rb_properties = jobOptionsSvc->getProperties("TrigSteer_HLT.ResultBuilder");
  }
  catch(...) {
    ATH_MSG_WARNING("Attempt to retrieve ResultBuilder properties provoked an exception.");
    return false;
  }
  std::vector<const Property*>::const_iterator rb_it;
  
  if(rb_properties == 0) {
    ATH_MSG_ERROR("Unable to get the job properties of the ResultBuilder");
    return false;
  }
  
  for(rb_it = rb_properties->begin(); rb_it != rb_properties->end(); rb_it++) {
    
    if( (*rb_it)->name() == "DefaultStreamTagForErrors") {
      
      const StringProperty *sprop = dynamic_cast<const StringProperty*>( (*rb_it) );
      
      if(sprop == 0) {
        ATH_MSG_DEBUG("dynamic_cast of DefaultStreamTagForErrors property to StringProperty failed");	
        continue;
      }		
      
      errorStreamNames.push_back( sprop->value() + "_debug");
      
    } else if( (*rb_it)->name() == "ErrorStreamTags") {
      const StringProperty *sprop = dynamic_cast<const StringProperty*>( (*rb_it) );
      // on lxplus tests and RTT, the cast works but online it is a StringArrayProperty
      if(sprop != 0) {
        StreamBlock::extractErrorStreamNames( sprop->value(), errorStreamNames );
      } else {
        ATH_MSG_DEBUG("dynamic_cast of ErrorStreamTags property to StringProperty failed");	
	
        const StringArrayProperty *saprop = dynamic_cast<const StringArrayProperty*>( (*rb_it) );
        
        if ( saprop != 0 ) {
          const std::vector<std::string>& theProps = saprop->value( );
          for ( std::vector<std::string>::const_iterator ip = theProps.begin();
                ip != theProps.end();  ip++ )  {
                        
            StreamBlock::extractErrorStreamNames( *ip, errorStreamNames );
          }
        } else {
          ATH_MSG_DEBUG("dynamic_cast of ErrorStreamTags property to StringArrayProperty failed");	
        }
      }
    }    
  }
  
  for(std::vector<std::string>::const_iterator it_name = errorStreamNames.begin(); 
      it_name != errorStreamNames.end(); it_name++) {
    ATH_MSG_INFO("adding error stream name: " << *it_name);
  }
  
  return true;
}


void findChainsInStreams(std::map<std::string, TH1I*>& histograms,   const std::vector<const HLT::SteeringChain*>& config, const std::string& level) {
  std::map<std::string, std::vector<std::string> > stream_to_chains; 

  
  for ( const HLT::SteeringChain* chain : config ) {
     for ( const HLT::StreamTag& stream :  chain->getStreamTags()) {
      stream_to_chains[stream.getStream()].push_back(chain->getChainName());
    }
  }  
  std::map<std::string, std::vector<std::string> >::const_iterator p;
  for (  p = stream_to_chains.begin();   p != stream_to_chains.end(); ++p ) {

    TH1I* h = histograms[p->first] = new TH1I(("ChainsInStream_"+p->first).c_str(), ("Chains in " +level + "stream "+p->first).c_str(), p->second.size(), 0, p->second.size());
    int bin = 1;
    for ( const std::string& label : p->second) {
      h->GetXaxis()->SetBinLabel(bin, label.c_str());
      ++bin;
    }
  }

}

void fillChainsInStreams(std::map<std::string, TH1I*>& histograms, const std::vector<const HLT::SteeringChain*>& result) {
   for( const HLT::SteeringChain* chain : result ) {
      if (chain->chainPassed()){
         for( const HLT::StreamTag& stream :  chain->getStreamTags() ){
            if ( ! stream.isPrescaled() ) {//fill
               histograms[stream.getStream()]->Fill(chain->getChainName().c_str(), 1.);
            }
         }
      }
   }
}
  

StatusCode TrigSignatureMoni::initialize()
{
  if ( TrigMonitorToolBase::initialize().isFailure() ) {
    ATH_MSG_ERROR(" Unable to initialize base class !");
    return StatusCode::FAILURE;
  }

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if ( !m_parentAlg ) {
    ATH_MSG_ERROR("Unable to cast the parent algorithm to HLT::TrigSteer");
    return StatusCode::FAILURE;
  }

  m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : (m_parentAlg->getAlgoConfig()->getHLTLevel()==HLT::EF ? "EF" : "HLT");

  //get access to ConfigService 
  if (m_trigConfigSvc.empty()) {
    ATH_MSG_ERROR("No TrigConfigSvc set in the jobOptions");
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_trigConfigSvc.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoni::bookHists()
{
  ATH_CHECK( bookHistograms( false, false, true ) );
  return StatusCode::SUCCESS;
}

		
StatusCode TrigSignatureMoni::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{
  TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );
  //TrigMonGroup shiftHistograms( this, m_parentAlg->name(), shift );
  std::vector<std::string> errorStreamNames;

  bool sc = getDebugStreams(errorStreamNames);
  if(!sc) ATH_MSG_WARNING("Any python-configured error streams will not appear in histograms");

  // Some preliminaries to setting up m_signatureAcceptanceHist...

  const TrigConf::HLTChainList *chainList = m_trigConfigSvc->chainList();
  enum bb_triglevel tl = (m_trigLvl == "L2")? tl_L2 :  ((m_trigLvl == "EF")? tl_EF : tl_HLT);

  BinBlock::PrepareToBook(tl);

  std::vector<const HLT::SteeringChain*> configuredChains = m_parentAlg->getConfiguredChains();

  m_generalBlock = new GeneralBlock(&msg());
  m_streamBlock  = new StreamBlock(chainList, errorStreamNames, m_trigLvl, &msg());
  m_groupBlock   = new GroupBlock(configuredChains, &msg());
  m_chainBlock   = new ChainBlock(configuredChains, &msg());
  BinBlock::FinalizeInitialization();

  // Needed only for checking:
  /*
  if(m_logLvl <= MSG::DEBUG){
    m_generalBlock->PrintBlock();
    m_streamBlock->PrintBlock();
    m_groupBlock->PrintBlock();
    m_chainBlock->PrintBlock();
  }
  */

  // Reset private members
  
  //histograms 
  m_signatureAcceptanceHist=0;
  m_chainlengthHist=0;
  m_eventsPassingStepHist=0;
  m_totaleventsPassingStepHist=0;
  m_stepForEBHist=0;
 
    
  // HISTOGRAM DEFINITONS
  
  // Step For EventBuilding histogram  
 if(m_trigLvl == "HLT") {
   m_stepForEBHist = new TH1I("StepForEB", "Step at which EB is called  ", 60, -30., 30.);
   if ( expertHistograms.regHist(m_stepForEBHist).isFailure()){
     ATH_MSG_WARNING("Can't book " << m_histoPathexpert+ m_stepForEBHist->GetName());
   }
   m_stepForEBHist->GetYaxis()->SetTitle("Events");
   m_stepForEBHist->GetXaxis()->SetTitle("Step of Event Building");
 
   // histogram for: Step separating L2EF 
   // m_chainstepL2EFHist = new TH1I("ChainL2EFstep","Step at which L2/EF separation is set for chains",
   // 				  BinBlock::TotalBinCount(), 
   // 				  BinBlock::GetFirstSigHistBin()-0.5, 
   // 				  BinBlock::GetFirstSigHistBin() + BinBlock::TotalBinCount()-0.5);
  
   // if ( expertHistograms.regHist(m_chainstepL2EFHist).isFailure()){
   //   if (m_logLvl <= MSG::WARNING) (*m_log) << MSG::WARNING << "Can't book "
   // 					    << m_histoPathexpert+ m_chainstepL2EFHist->GetName() << endreq;
   // }
   // m_chainstepL2EFHist->GetYaxis()->SetTitle("L2/EF separation step (first step configured after EB)");
   // m_chainstepL2EFHist->GetXaxis()->SetTitle("chains");

 }
 

 
  // Chain length histogram
  TString htit;
  std::string tmpstring  = "Length of Chains in  ";
  tmpstring += m_trigLvl;
  htit       = Form(tmpstring.c_str());
  //tmpstring  = "chainLength_"+m_trigLvl;
  
  m_chainlengthHist = new TH1I("ChainLength",htit.Data(),
			       BinBlock::TotalBinCount(), 
			       BinBlock::GetFirstSigHistBin()-0.5, 
			       BinBlock::GetFirstSigHistBin() + BinBlock::TotalBinCount()-0.5);
  
  if ( expertHistograms.regHist(m_chainlengthHist).isFailure()){
    ATH_MSG_WARNING("Can't book "<< m_histoPathexpert+ m_chainlengthHist->GetName());
  }
  m_chainlengthHist->GetYaxis()->SetTitle("Step (Step 0 is the Input!)");
  m_chainlengthHist->GetXaxis()->SetTitle("chains");
  
  // Stream correlation histogram
  if(m_trigLvl == "EF" || m_trigLvl == "HLT") {
    m_streamCorrel = new TH2I("StreamCorrel", "Stream vs. Stream (n.b: non-diags are filled on both sides of diag.)", 
			      m_streamBlock->GetNBins(), 0, m_streamBlock->GetNBins(), 
			      m_streamBlock->GetNBins(), 0, m_streamBlock->GetNBins());
    if(m_streamCorrel == 0) {
      ATH_MSG_WARNING("Can't book "<< m_streamCorrel->GetName());
    }
    
    for(unsigned int i = 0; i <  m_streamBlock->GetNBins(); i++) {
      m_streamCorrel->GetXaxis()->SetBinLabel(i+1, m_streamBlock->GetXLab(i).data());
      m_streamCorrel->GetYaxis()->SetBinLabel(i+1, m_streamBlock->GetXLab(i).data());
    }
    if ( expertHistograms.regHist(m_streamCorrel).isFailure()){
      ATH_MSG_WARNING("Can't register "<< m_streamCorrel->GetName() << ", deleting");
      delete m_streamCorrel;
      m_streamCorrel = 0;
    }

  }

  // Signature acceptance histogram
  std::string tmpstring_title  = "Raw acceptance of signatures in "+ m_trigLvl;
  m_signatureAcceptanceHist = new
    TrigLBNHist<TH2I>(TH2I("SignatureAcceptance",tmpstring_title.c_str(), 
			   BinBlock::TotalBinCount(), 
			   BinBlock::GetFirstSigHistBin()-0.5, 
			   BinBlock::GetFirstSigHistBin() + BinBlock::TotalBinCount()-0.5,
			   BinBlock::GetNStepBins(), 
			   -0.5, 
			   BinBlock::GetNStepBins()-0.5 )
		      );

  for(int bin = 0; bin < BinBlock::TotalBinCount(); bin++)
    m_signatureAcceptanceHist->GetXaxis()->SetBinLabel(bin+1, BinBlock::GetSigXAxisLabel(bin).data());
  for(int bin = 0; bin < BinBlock::GetNStepBins(); bin++)
    m_signatureAcceptanceHist->GetYaxis()->SetBinLabel(bin+1, BinBlock::GetStepNames()[bin].data());
    

  if ( expertHistograms.regHist((ITrigLBNHist*)m_signatureAcceptanceHist).isFailure()) {
    ATH_MSG_WARNING("Can't book "<< m_signatureAcceptanceHist->GetName());
  }
  
  /*
  // Events passing step histogram
  tmpstring_title  = "Number of events passing a step in "+ m_trigLvl;
  
  
  m_eventsPassingStepHist=new TrigLBNHist<TH1I>(TH1I("EventsPassingStep",tmpstring_title.c_str(),
  BinBlock::GetNStepBins()+1,
  -0.5,
  BinBlock::GetNStepBins() +0.5)
  );
  
  m_eventsPassingStepHist->GetXaxis()->SetTitle("step");
  m_eventsPassingStepHist->GetYaxis()->SetTitle("Number of successful events");
  

  if ( expertHistograms.regHist((ITrigLBNHist*)m_eventsPassingStepHist).isFailure()) {
  if (m_logLvl <= MSG::WARNING)(*m_log) << MSG::WARNING << "Can't book "
					  << m_eventsPassingStepHist->GetName() << endreq;
					  }

  
  // Total events passing step histogram
  tmpstring_title = "Number of total events passing a step in "+ m_trigLvl;

  m_totaleventsPassingStepHist=new TrigLBNHist<TH1I>(TH1I("TotalEventsPassingStep",tmpstring_title.c_str(),
							  BinBlock::GetNStepBins()+1,
							  -0.5,
							  BinBlock::GetNStepBins()+0.5 )
						     );

  m_totaleventsPassingStepHist->GetXaxis()->SetTitle("step");
  m_totaleventsPassingStepHist->GetYaxis()->SetTitle("Number of total successful events");
  
  if ( expertHistograms.regHist((ITrigLBNHist*)m_totaleventsPassingStepHist).isFailure()) {
    if (m_logLvl <= MSG::WARNING)(*m_log) << MSG::WARNING << "Can't book "
					  << m_totaleventsPassingStepHist->GetName() << endreq;
  }
  */

#ifdef _IF_LUMI_BLOCK_NUMBER_DIFF_HISTO_IS_WANTED_GET_RID_OF_THIS_IFDEF
  m_lumiBlockNumDiffHist = new TH1I("LBNDiff", "Diff between current LumiBlockNum and highest seen(if nonzero)",
				    20, 1, 21);
			       
  if ( expertHistograms.regHist(m_lumiBlockNumDiffHist).isFailure()){
    ATH_MSG_WARNING("Can't book "<< m_histoPathexpert+ m_lumiBlockNumDiffHist->GetName());
  }
#endif

  // chain length histo: fill 1d histogram with the length of each chain
  for ( const HLT::SteeringChain* chain : configuredChains ) {
    unsigned int chainCounter = chain->getChainCounter();
    int length = chain->getSignatures().size();
    m_chainlengthHist->Fill( m_chainBlock->GetSigHistValue(chainCounter), length);

    //set unused bins in signatureAcceptance to -1 
    int binNum = 0;
    int stepNum = 0;
    std::vector<std::string> stepNames = BinBlock::GetStepNames();
    for ( std::string step : stepNames) {
      if( step.substr(0,4) == "step") {

        if(stepNum >= length) {
          m_signatureAcceptanceHist->SetBinContent(m_chainBlock->GetSigHistBin(chainCounter)+1, binNum+1, -1);
        }
        stepNum++;
      }
      binNum++;
    }
  }

  
  //set streamTag-xbins  and group-xbins to -1 at step1 
  int step1 = BinBlock::GetFirstAlgBin();
  for(unsigned int bin = 0; bin < m_streamBlock->GetNBins(); bin++)
    m_signatureAcceptanceHist->SetBinContent(m_streamBlock->GetFirstBin() + bin+1, step1+1, -1);
  
  for(unsigned int bin = 0; bin < m_groupBlock->GetNBins(); bin++)
    m_signatureAcceptanceHist->SetBinContent(m_groupBlock->GetFirstBin() + bin+1, step1+1, -1);
  
  findChainsInStreams(m_chainsInStream, configuredChains, m_trigLvl);
  std::map<std::string,TH1I*>::const_iterator s;
  for ( s = m_chainsInStream.begin(); s != m_chainsInStream.end(); ++s ) {
    s->second->GetXaxis()->LabelsOption("a");
    if ( expertHistograms.regHist(s->second).isFailure()) {
      ATH_MSG_WARNING("Failed to book stream histogram");
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode TrigSignatureMoni::fillHists()
{
  if( ! m_signatureAcceptanceHist  ){
    ATH_MSG_WARNING("Pointers to histograms not ok, dont Fill ! ");
    return StatusCode::FAILURE;  
  }

  enum bb_triglevel tl =(m_trigLvl == "L2")? tl_L2 :  ((m_trigLvl == "EF")? tl_EF : tl_HLT);

  BinBlock::SetLevel(tl);

  const EventInfo* constEventInfo(0);
  StatusCode sc_cei =  evtStore()->retrieve(constEventInfo);

  //TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );

  if(sc_cei.isFailure()){
    ATH_MSG_WARNING("Can't get EventInfo object (for stream tags & Lumiblock number)");
  }

#ifdef _IF_LUMI_BLOCK_NUMBER_DIFF_HISTO_IS_WANTED_GET_RID_OF_THIS_IFDEF
  //get lumiBlockNumber

  unsigned int lumiBlockNumber=0;
  unsigned int runNumber=0;

  
  if ( sc_cei.isSuccess && constEventInfo->event_ID()) {
    lumiBlockNumber =  constEventInfo->event_ID()->lumi_block();
    runNumber =  constEventInfo->event_ID()->run_number();
  }else{
    ATH_MSG_DEBUG("LBN not possible because missing event_ID");
  }

  m_highestLBN = (lumiBlockNumber > m_highestLBN)? lumiBlockNumber : m_highestLBN;
  
  int diff =  ((int) m_highestLBN) - ((int) lumiBlockNumber);
  
  if( diff > 0)
    if(m_lumiBlockNumDiffHist != 0) m_lumiBlockNumDiffHist->Fill(diff);

#endif

  
  
  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();
  unsigned int maxsuccessfulSteps=0;
  bool eventPassedRaw = false;
  bool eventPassedPS = false;
  bool eventPassedPT = false;
  bool eventHasError = false;
  
  enum accMask {hitInput = 1, hitRaw = 2, hitPS = 4, hitAlgoIn = 8, hitPT = 16, hitRerun=32, hitError=64};
  std::map<int, unsigned int> streamHit; // Hold a bit mask for each stream hit
  std::map<int, unsigned int> groupHit;  // Hold a bit mask for each group hit

  //loop chains and fill x-bins for chains
  for (std::vector<const HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
       chain != activeChains.end(); ++chain) {
    const HLT::SteeringChain *ch = *chain;
    
    unsigned int chainCounter = (unsigned int)ch->getChainCounter();
    unsigned int bin = m_chainBlock->GetSigHistValue(chainCounter);
    eventPassedRaw = eventPassedRaw  ||  ch->chainPassedRaw();
    eventPassedPS  = eventPassedPS   ||  ch->chainPassedRaw(); // same as above
    eventPassedPT  = eventPassedPT   ||  ch->chainPassed();


    if ( !ch->isResurrected() ) {
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetInputBin());
      m_streamBlock->SetMask(chainCounter, hitInput, streamHit);
      m_groupBlock->SetMask(chainCounter, hitInput, groupHit);
    }

    if ( ch->isResurrected() ) {
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetRerunBin());
      m_streamBlock->SetMask(chainCounter, hitRerun, streamHit);
      m_groupBlock->SetMask(chainCounter, hitRerun, groupHit);
      continue;
    }

    if ( !ch->isResurrected() && !ch->isPrescaled() ) {
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetPSBin());
      m_streamBlock->SetMask(chainCounter, hitPS, streamHit);
      m_groupBlock->SetMask(chainCounter, hitPS, groupHit);
    }

    if ( !ch->isResurrected()  && ( !ch->isPrescaled() || ch->isPassedThrough() ) ) {
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetAlgoInBin());
      m_streamBlock->SetMask(chainCounter, hitAlgoIn, streamHit);
      m_groupBlock->SetMask(chainCounter, hitAlgoIn, groupHit);
    }

    if ( !ch->isResurrected() && ch->chainPassedRaw() ){
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetRawBin());
      m_streamBlock->SetMask(chainCounter, hitRaw, streamHit);
      m_groupBlock->SetMask(chainCounter, hitRaw, groupHit);
    }

    if ( !ch->isResurrected()  && ( ch->chainPassedRaw() || ch->isPassedThrough() )){
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetPTBin());
      m_streamBlock->SetMask(chainCounter, hitPT, streamHit);
      m_groupBlock->SetMask(chainCounter, hitPT, groupHit);
    }

    HLT::ErrorCode ecchain = ch->getErrorCode();
    if ( ecchain.action() > HLT::Action::CONTINUE ) {
      eventHasError = true;
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetErrorBin());
      m_streamBlock->SetMask(chainCounter, hitError, streamHit);
      m_groupBlock->SetMask(chainCounter, hitError, groupHit);
    }

    // fill chain vs. step with the number of successful events per signature
    unsigned int nsteps = ch->getChainStep();
    for (unsigned int step = 0; step < nsteps; ++step){
      m_signatureAcceptanceHist->Fill(bin, BinBlock::GetFirstAlgBin() + step);
    }
    
    maxsuccessfulSteps = nsteps > maxsuccessfulSteps ? nsteps : maxsuccessfulSteps;
    
  }//end of chain loop
  
  // fill hit streams
  for(std::map<int, unsigned int>::const_iterator sh = streamHit.begin(); sh != streamHit.end(); sh++) {
    unsigned int mask = sh->second;
    if(mask & hitInput)
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetInputBin());

    if(mask & hitRaw)
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetRawBin());
    
    if(mask & hitPS) 
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetPSBin());
    
    if(mask & hitAlgoIn) 
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetAlgoInBin());
    
    if(mask & hitPT)// the PT bin is filled from the streamtag info below.
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetFirstAlgBin() + BinBlock::GetMaxSteps() - 1);

    if(mask & hitRerun)
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetRerunBin());

    if(mask & hitError)
      m_signatureAcceptanceHist->Fill(sh->first, BinBlock::GetErrorBin());
  }	

  // fill hit groups
  for(std::map<int, unsigned int>::iterator gh = groupHit.begin(); gh != groupHit.end(); gh++) {
    unsigned int mask = gh->second;
    if(mask & hitInput)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetInputBin());

    if(mask & hitRaw)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetRawBin());
    
    if(mask & hitPS)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetPSBin());
    
    if(mask & hitAlgoIn)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetAlgoInBin());
    
    if(mask & hitPT)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetPTBin());

    if(mask & hitRerun)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetRerunBin());

    if(mask & hitError)
      m_signatureAcceptanceHist->Fill(gh->first, BinBlock::GetErrorBin());

  }	

  // total level input  ALL EVENTS (chain 0, step=0)  change to x=-1
  m_signatureAcceptanceHist->Fill( m_generalBlock->GetAllEvsBin(), BinBlock::GetInputBin() );
  
  if(activeChains.size() > 0){
    m_signatureAcceptanceHist->Fill( m_generalBlock->GetOrBin(),     BinBlock::GetInputBin());
  }
  
  if(eventPassedPT) {
    m_signatureAcceptanceHist->Fill( m_generalBlock->GetOrBin(),     BinBlock::GetPTBin());
  }
  
  if(eventPassedPS) {
    m_signatureAcceptanceHist->Fill( m_generalBlock->GetOrBin(),     BinBlock::GetPSBin());
  }
  
  if(eventPassedRaw) {
    m_signatureAcceptanceHist->Fill( m_generalBlock->GetOrBin(),     BinBlock::GetRawBin());
  }
  
  if(eventHasError) {
    m_signatureAcceptanceHist->Fill( m_generalBlock->GetOrBin(),     BinBlock::GetErrorBin());
  }
  for (unsigned int step=0; step <= BinBlock::GetMaxSteps() ;step++)
    if(step < maxsuccessfulSteps)
      m_signatureAcceptanceHist->Fill(m_generalBlock->GetOrBin(), BinBlock::GetFirstAlgBin() + step);
  

  
  if(maxsuccessfulSteps > BinBlock::GetMaxSteps()){
    ATH_MSG_ERROR("Something wrong? maxsuccessfulSteps "<<maxsuccessfulSteps<<" > m_sv[m_tl].maxSteps "
                  <<  BinBlock::GetMaxSteps() <<" ?");
  }

  /*
  // fill the histogram with events passing each step, be careful, the chains have different length!
  for (unsigned int step=0; step <= BinBlock::GetMaxSteps() ;step++) {
  if(step <= maxsuccessfulSteps){
  m_eventsPassingStepHist->Fill(step,1);
  
  m_totaleventsPassingStepHist->Fill(step,1);
  
  }else if(eventPassedRaw)
  m_totaleventsPassingStepHist->Fill(step, 1);
  }
  */


  if(sc_cei) {
    //use EventInfo -> TriggerInfo-> Stream Tag  to get streams this event contributes to
    std::vector<TriggerInfo::StreamTag> streamTags = constEventInfo->trigger_info()->streamTags();
    
    if(streamTags.size() > 0)
      m_signatureAcceptanceHist->Fill(m_generalBlock->GetAllEvsBin(), BinBlock::GetPTBin());
    
    // fill total out per stream including additional streamTag prescales
    
    for(std::vector<TriggerInfo::StreamTag>::iterator iter=streamTags.begin(); iter != streamTags.end(); iter++){    
      std::string streamKey = iter->name()+"_"+iter->type();
      
      //streamKey = "nobodys_business"; // FOR TESTING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      int binNo = m_streamBlock->GetSigHistValue(streamKey);
      if(binNo != m_streamBlock->GetCatchAllBin())
	m_signatureAcceptanceHist->Fill( binNo, BinBlock::GetPTBin());
    }
    
    if(m_streamCorrel != 0) { // Fill the correlation histogram, EF only
      for(std::vector<TriggerInfo::StreamTag>::iterator iter=streamTags.begin(); iter != streamTags.end(); iter++){
	std::string streamKey = iter->name()+"_"+iter->type();
	//streamKey = "nobodys_business"; // FOR TESTING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int bin1 = m_streamBlock->GetCorrelHistBin(streamKey);
	for(std::vector<TriggerInfo::StreamTag>::iterator it2=iter; it2 != streamTags.end(); it2++) {
	  int bin2 = m_streamBlock->GetCorrelHistBin(it2->name()+"_"+it2->type());
	  if(bin1 != m_streamBlock->GetCatchAllBin() && bin1 != m_streamBlock->GetCatchAllBin() ) {
	    m_streamCorrel->Fill(bin1,bin2);
	    if(bin1 != bin2)
	      m_streamCorrel->Fill(bin2,bin1);
	  }
	}
      }
    }
  }
  
  fillChainsInStreams(m_chainsInStream, activeChains);

  if(m_trigLvl == "HLT") 
    m_stepForEBHist->Fill(m_parentAlg->stepForEB());

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoni::finalHists() { 
  m_chainsInStream.clear();
  return StatusCode::SUCCESS;
}


/**


   Implementation of BinBlock and inheriting classes


**/

BinBlock::BinBlock(BinBlock *binBlock, MsgStream *log): m_NBins(0), m_log(log)
{

  m_sv[m_tl].firstSigHistBin = -1; // for historical reasons!
  m_sv[m_tl].unknown = "unknown";

  std::vector<BinBlock*>::const_iterator bbit;
  m_sv[m_tl].totalBins = 0;
  
  for(bbit = m_sv[m_tl].binBlocks.begin(); bbit != m_sv[m_tl].binBlocks.end(); bbit++) {
    m_sv[m_tl].totalBins += (*bbit)->GetNBins();
  }
  
  m_firstBin = m_sv[m_tl].totalBins;
  
  m_sv[m_tl].binBlocks.push_back(binBlock);

}

BinBlock::~BinBlock()
{
}

// This function is badly named. All it does is delete the binBlocks if they exist and clean up.
void BinBlock::PrepareToBook(enum bb_triglevel tl){

  m_tl = tl;

  std::vector<BinBlock*>::iterator bbit;
  for(bbit = m_sv[m_tl].binBlocks.begin(); bbit != m_sv[m_tl].binBlocks.end(); bbit++){
    delete *bbit;
  }
  m_sv[m_tl].binBlocks.clear();
  m_sv[m_tl].stepnames.clear();

}

void BinBlock::FinalizeInitialization()
{

  std::vector<BinBlock*>::const_iterator bbit;
  m_sv[m_tl].totalBins = 0;

  for(bbit = m_sv[m_tl].binBlocks.begin(); bbit != m_sv[m_tl].binBlocks.end(); bbit++) {
    m_sv[m_tl].totalBins += (*bbit)->GetNBins();
  }

  // set the step name
  m_sv[m_tl].stepnames.push_back("errors");
  m_sv[m_tl].stepnames.push_back("input");
  m_sv[m_tl].stepnames.push_back("!PS rate");
  m_sv[m_tl].stepnames.push_back("algoIn");
  

  // give numbers to the steps
  for (unsigned int i_step = 1; i_step <= m_sv[m_tl].maxSteps; i_step++){
    std::string tmpstr(Form("step %i",i_step));
    m_sv[m_tl].stepnames.push_back(tmpstr);
  }

  m_sv[m_tl].stepnames.push_back("raw rate");
  m_sv[m_tl].stepnames.push_back("total rate");
  m_sv[m_tl].stepnames.push_back("rerun rate");

  m_sv[m_tl].errorBin     = 0;
  m_sv[m_tl].inputBin     = 1;
  m_sv[m_tl].psBin        = 2;
  m_sv[m_tl].algoInBin    = 3;
  m_sv[m_tl].firstStepBin = 4;
  m_sv[m_tl].rawBin   = m_sv[m_tl].maxSteps + 4;
  m_sv[m_tl].ptBin    = m_sv[m_tl].maxSteps + 5;
  m_sv[m_tl].rerunBin = m_sv[m_tl].maxSteps + 6; 

}


const std::string &BinBlock::GetSigXAxisLabel(int bin)
{
  std::vector<BinBlock*>::const_iterator bbit;

  for(bbit = m_sv[m_tl].binBlocks.begin(); bbit != m_sv[m_tl].binBlocks.end(); bbit++) {
    int firstBin = (*bbit)->GetFirstBin();
    if(bin >= firstBin && bin < firstBin + int((*bbit)->GetNBins())) {
      return (*bbit)->GetXLab(bin - firstBin);
    }
  }
  return m_sv[m_tl].unknown;
}

GroupBlock::GroupBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log) : BinBlock(this, log)
{

 
  std::map<std::string, int> groupBinMap;
  m_NBins = 0;

  std::vector<const HLT::SteeringChain*>::const_iterator cit;
  
  // First populate groupBinMap with the group names
  // Done this way to ensure that groups are in alphabetical order in histo

  for ( const HLT::SteeringChain * chain : configuredChains ) {
     for ( std::string group : chain->getConfigChain()->groups() )
        groupBinMap[group] = 0;
  }

  // Assign bins, set label
  //  for(std::map<std::string, int>::iterator grit = groupBinMap.begin();
  //      grit != groupBinMap.end(); grit++) {
  typedef std::pair<std::string, int> groupBin_t;
  for ( groupBin_t group_bin : groupBinMap) {
      m_XLabels[m_NBins] = "grp_" + group_bin.first;
      group_bin.second = GetFirstSigHistBin() + m_firstBin + m_NBins++;
  }

  // Now set the bin maps for each chain
  for ( const HLT::SteeringChain * chain : configuredChains ) {
    std::set<int> binNums;
    for ( std::string group : chain->getConfigChain()->groups() )
       binNums.insert(groupBinMap[group]);
    
    m_ch_bin_map[ chain->getChainCounter() ] = binNums;      
  }
}

const std::set<int> &GroupBlock::GetBinSet(unsigned int chainCounter)
{
  if(m_ch_bin_map.find(chainCounter) == m_ch_bin_map.end())
    return m_emptyList;

  return m_ch_bin_map[chainCounter];
}

void GroupBlock::PrintBlock()
{
  (*m_log) << MSG::DEBUG  << "Group Block " 
	   << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endreq;

  std::map<unsigned int, std::set<int> >::const_iterator mcbit;
  for(mcbit = m_ch_bin_map.begin(); mcbit != m_ch_bin_map.end(); mcbit++) {
    (*m_log) << MSG::DEBUG << "chain counter: " << mcbit->first << ", bins: ";
    for( std::set<int>::const_iterator si = (mcbit->second).begin(); si != (mcbit->second).end(); si++) 
      (*m_log) << MSG::DEBUG << (*si) << ", ";
    (*m_log) << MSG::DEBUG << endreq;
  }
}

void GroupBlock::SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap) {

  const std::set<int> groups = GetBinSet(chainCounter);

  for(std::set<int>::iterator hgit = groups.begin(); hgit != groups.end(); hgit++) {

    if(hitMap.find(*hgit) == hitMap.end())
      hitMap[*hgit] = 0;

    hitMap[*hgit] |= mask;
  }
}

ChainBlock::ChainBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log) : BinBlock(this, log)
{
  m_NBins = 0;
  unsigned int maxSteps = 0;

  // This is here to produce an alphabetically sorted list of chains (to address Savannah bug 79938
  std::map<std::string, unsigned int> chainname;
  for (std::vector<const HLT::SteeringChain*>::const_iterator it = configuredChains.begin();
       it != configuredChains.end(); ++it) {
    chainname[(*it)->getChainName()]= (*it)->getChainCounter();

    const std::vector<const HLT::Signature*>& signatures = (*it)->getSignatures();
    // searching for the longest (max. steps) configured chain
    maxSteps = signatures.size() > maxSteps ? signatures.size() : maxSteps;
  }
  

  //std::vector<const HLT::SteeringChain*>::const_iterator cit;
  
  //for(cit = configuredChains.begin(); cit != configuredChains.end(); cit++) {
  for( std::map<std::string, unsigned int>::const_iterator it = chainname.begin(); it != chainname.end();
       it++) {
    unsigned int chainCounter = it->second;
    
    std::string namestring = it->first + "_ChCo_%i";
    m_XLabels[m_NBins] = Form(namestring.c_str(),chainCounter);
    m_ch_bin[chainCounter] = GetFirstSigHistBin() + m_firstBin + m_NBins++;

    //const std::vector<const HLT::Signature*>& signatures = (*cit)->getSignatures();
    // searching for the longest (max. steps) configured chain
    //maxSteps = signatures.size() > maxSteps ? signatures.size() : maxSteps;
  }
  BinBlock::SetMaxSteps(maxSteps);
}


void ChainBlock::PrintBlock()
{
  (*m_log) << MSG::DEBUG << "Chain Block " 
	   << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endreq;

  std::map<unsigned int, int>::const_iterator mcbit;

  for( mcbit = m_ch_bin.begin(); mcbit != m_ch_bin.end(); mcbit++) {
    (*m_log) << MSG::DEBUG << mcbit->first << ": " << mcbit->second << "   " << endreq;
  }

}

int ChainBlock::GetSigHistValue(unsigned int chainCounter)
{
  if(m_ch_bin.find(chainCounter) != m_ch_bin.end())
    return m_ch_bin[chainCounter];

  return OverflowBin();

}

int ChainBlock::GetSigHistBin(unsigned int chainCounter)
{
  if(m_ch_bin.find(chainCounter) != m_ch_bin.end())
    return m_ch_bin[chainCounter] - GetFirstSigHistBin();

  return OverflowBin();
  
}


StreamBlock::StreamBlock(const TrigConf::HLTChainList* allChains,
			 std::vector<std::string> &errorStreamNames, std::string trigLvl,
			 MsgStream *log) :
  BinBlock(this, log) {
  
  // first pick out the EF physics streams, then any other EF streams, then any nonphysics L2 streams
  if(trigLvl == "EF") {
    InsertStreams(true, "EF", allChains);
    InsertStreams(false, "EF", allChains);
    InsertStreams(false, "L2", allChains);
  } else if(trigLvl == "L2") {
    InsertStreams(true, "L2", allChains);
    InsertStreams(false, "L2", allChains);
  }  else {
    InsertStreams(true, "HLT", allChains);
    InsertStreams(false, "EF", allChains);
    InsertStreams(false, "L2", allChains);
  }

  // now the debug streams
  std::vector<std::string>::const_iterator esnit;
  for(esnit = errorStreamNames.begin(); esnit != errorStreamNames.end(); esnit++) {
    m_XLabels[m_NBins] = "str_" + (*esnit);
    m_str_tag_map[(*esnit)] = m_NBins++;
  }

  // now a catch-all         --- suppressed on request from Brian 7.4.2011
  //m_XLabels[m_NBins] = "str_other";
  //m_str_tag_map["other"] = m_NBins;
  //m_catchAll = m_NBins++;
  m_catchAll = -1;


  if(m_log->level() <= MSG::DEBUG) {
    (*log) << MSG::DEBUG << "************ stream tag to bin map *****************" << endreq;

    std::map<std::string, int>::const_iterator stmit;
    for(stmit = m_str_tag_map.begin(); stmit != m_str_tag_map.end(); stmit++)
      (*log) << MSG::DEBUG << (*stmit).first << ": " << (*stmit).second << endreq;
    
    (*log) << MSG::DEBUG << "*********** chain counter to bin map for streams ***************" << endreq;
    std::map<unsigned int, std::set<int> >::const_iterator csmit;
    for(csmit = m_ch_str_map.begin(); csmit != m_ch_str_map.end(); csmit++) {
      (*log) << MSG::DEBUG << (*csmit).first << ": ";
      std::set<int>::iterator si;
      for(si = (*csmit).second.begin(); si != (*csmit).second.end(); si++)
	(*log) << (*si) << "  ";
      (*log) << endreq;
    }
  }

}

void StreamBlock::InsertStreams(bool isPhysType, std::string trgLvl,
                                const TrigConf::HLTChainList* allChains)
{
  
  TrigConf::HLTChainList::const_iterator cit;
  
  // MM, 16.8.11  First get the names of the streams into a local set (for ordering).
  //              Then use it to set up the m_str_tag_map, then set up the chain to stream map

  std::set<std::string> streamNames;
  for(cit = allChains->begin(); cit != allChains->end(); cit++) {

    if( (*cit)->level() != trgLvl)
      continue;

    std::vector<TrigConf::HLTStreamTag*>::const_iterator stit;

    for(stit = (*cit)->streams().begin(); stit !=(*cit)->streams().end();stit++ ) {
       if( (isPhysType && (*stit)->type() == "physics") || (!isPhysType && (*stit)->type() != "physics")) {
          const std::string namkey = (*stit)->stream()+"_"+(*stit)->type();
          streamNames.insert(namkey);
       }
    }
  }

  // Set up m_ch_str_map and the labels vector
  for(std::set<std::string>::const_iterator snit = streamNames.begin(); snit != streamNames.end(); snit++) {
    m_str_tag_map[ *snit ] = m_NBins;
    m_XLabels[m_NBins] = "str_" + *snit;
    m_NBins++;
  }

  // Set up the chain to streams map
  for(cit = allChains->begin(); cit != allChains->end(); cit++) {

     if( (*cit)->level() != trgLvl)
        continue;

     std::vector<TrigConf::HLTStreamTag*>::const_iterator stit;

     for(stit = (*cit)->streams().begin(); stit !=(*cit)->streams().end();stit++ ){
        if( (isPhysType && (*stit)->type() == "physics") || (!isPhysType && (*stit)->type() != "physics")) {
           const std::string namkey = (*stit)->stream()+"_"+(*stit)->type();
           if(m_str_tag_map.find(namkey) == m_str_tag_map.end()) {
              m_ch_str_map.insert(std::pair<unsigned int, std::set<int> >((*cit)->chain_counter(), 
                                                                          std::set<int>()));
           }
           m_ch_str_map[(*cit)->chain_counter()].insert( GetFirstSigHistBin() + 
                                                         m_firstBin + 
                                                         m_str_tag_map[namkey] );
        }
     }
  }
}

void StreamBlock::PrintBlock()
{
  if(m_log->level() <= MSG::DEBUG) { 
    (*m_log) << MSG::DEBUG  << "**** Stream Block ****" << endreq
	     << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endreq;
    (*m_log) << MSG::DEBUG  << "Stream tag to bin map " << endreq;
    std::map<std::string, int>::iterator mstit;
    for(mstit = m_str_tag_map.begin(); mstit != m_str_tag_map.end(); mstit++) {
      (*m_log) << MSG::DEBUG  << mstit->first << ": " << mstit->second << endreq;
    }
    (*m_log) << MSG::DEBUG  << "Chain counter to bin map" << endreq;
    std::map<unsigned int, std::set<int> >::iterator mcsit;
    for(mcsit = m_ch_str_map.begin(); mcsit != m_ch_str_map.end(); mcsit++) {
      (*m_log) << MSG::DEBUG  << "chain counter " << mcsit->first << ": ";
      for(std::set<int>::iterator si = mcsit->second.begin(); si != mcsit->second.end(); si++)
	(*m_log) << MSG::DEBUG  << *si << ", ";
      (*m_log) << MSG::DEBUG  << endreq;
    }
  }
}

void StreamBlock::extractErrorStreamNames( std::string errStreamString, 
					   std::vector<std::string> &errorStreamNames )
{
  /*
    Parse the string returned by getProperties.  I don't understand
    why but getProperties returns a string containing all error
    streams rather than a vector of streams.
  */

  std::stringstream ss;
  ss.str(errStreamString);
  char c;
  
  // assuming errStreamString looks something like:
  //['ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION: hltexceptions physics', 'ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION: hltex2 physics']
  
  /* there are no brackets when the tag comes from a StringArrayProperty
  if( (c = ss.get()) != '[') {
    //(*m_log) << MSG::DEBUG << "MMMM I'm lost already" << std::endl;
    return;
  }
  */
  
  while( ss.good() ) {
    ss.get(c);
    if( c == '\'') {
      const int bufSize = 100;
      char charBuf[bufSize];

      ss.getline(charBuf, bufSize, ':');    
      ss.getline(charBuf, bufSize, '\'');

      char *pBuf = charBuf;
      for(int i = 0; i<bufSize-1 && charBuf[i] == ' '; i++) pBuf++;

      for(int i = 0; i<bufSize - (pBuf-charBuf) && pBuf[i] != '\0'; i++) { // change space to underscore
        pBuf[i] = (pBuf[i] == ' ')? '_' : pBuf[i];
      }

      errorStreamNames.push_back(pBuf);
    }
  }
}


const std::set<int> &StreamBlock::GetSigHistValueSet(unsigned int chainCounter){

  if(m_ch_str_map.find(chainCounter) != m_ch_str_map.end())
    return m_ch_str_map[chainCounter];
  
  return notFound;
     
}

int StreamBlock::GetSigHistValue(std::string streamTag) {

  if(m_str_tag_map.find(streamTag) != m_str_tag_map.end())
    return GetFirstSigHistBin() + m_firstBin + m_str_tag_map[streamTag];

  //return GetFirstSigHistBin() + m_firstBin + m_catchAll;
  return m_catchAll;
}

int StreamBlock::GetCorrelHistBin(std::string streamTag) {
  if(m_str_tag_map.find(streamTag) != m_str_tag_map.end())
    return m_str_tag_map[streamTag];

  return m_catchAll;
}


void StreamBlock::SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap) {

  const std::set<int> streams = GetSigHistValueSet(chainCounter);

  for(std::set<int>::iterator hsit = streams.begin(); hsit != streams.end(); hsit++) {

    if(hitMap.find(*hsit) == hitMap.end())
      hitMap[*hsit] = 0;

    hitMap[*hsit] |= mask;
  }
}
    
