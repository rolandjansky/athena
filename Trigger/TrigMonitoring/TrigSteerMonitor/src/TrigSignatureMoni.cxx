/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"

#include "TrigSignatureMoni.h"

#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/Signature.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include <vector> 
#include <set> 
#include <algorithm>
#include <TH1I.h>
#include <TH2I.h>
#include <map>

TrigSignatureMoni::TrigSignatureMoni(const std::string & type, const std::string & name,
				     const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),   
     m_trigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name)
{
  declareProperty("HistoPathexpert", m_histoPathexpert = "/EXPERT/TrigSteering");
}

// Define helper classes for the signature acceptance histogram (and stream correlation plot)

class BinBlock;
template class std::vector<BinBlock*>;

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


class BinBlock {
  
public:
  virtual ~BinBlock() {}
  int GetFirstBin() const {return m_firstBin;} // first bin of a block
  const std::string& GetXLabel(int relBin){return m_XLabels[relBin];}

  static int GetFirstSigHistBin(){ return m_sv.firstSigHistBin;} // first bin of the signature hist
  static int TotalBinCount() {return m_sv.totalBins;}
  static int OverflowBin() { return m_sv.totalBins + 1;}
  static const std::string& GetSigXAxisLabel(int bin); 
  static void FinalizeInitialization();
  static void PrepareToBook();

  static const std::vector<std::string> &GetStepNames(){return m_sv.stepnames;}
  static int GetNStepBins() {return m_sv.stepnames.size();}
  static unsigned int GetMaxSteps(){return m_sv.maxSteps;} // max num of algorithm steps
  static unsigned int GetInputBin(){return m_sv.inputBin;}
  static unsigned int GetRawBin(){return m_sv.rawBin;}
  static unsigned int GetPSBin(){ return m_sv.psBin;}
  static unsigned int GetPTBin(){ return m_sv.ptBin;}
  static unsigned int GetFirstAlgBin(){return m_sv.firstStepBin;}
  static unsigned int GetRerunBin(){return m_sv.rerunBin;}
  static unsigned int GetAlgoInBin(){return m_sv.algoInBin;}
  static unsigned int GetErrorBin(){return m_sv.errorBin;}
  unsigned int GetNBins() const {return m_NBins;}
  virtual void PrintBlock() = 0;

protected:


  BinBlock(BinBlock *, MsgStream *log);

  static void SetMaxSteps(unsigned int max) {m_sv.maxSteps = max;}

  std::map<int, std::string> m_XLabels;
  unsigned int m_NBins;
  int m_firstBin;

  MsgStream *m_log;
  
  static struct staticVariables m_sv;
  
private:
};
// storage for statics;
struct staticVariables BinBlock::m_sv; 

class GeneralBlock : public BinBlock { // General event columns
  
public:
  GeneralBlock(MsgStream *log) 
    : BinBlock(this, log)
  {
    m_XLabels[0] = "total rate";
    m_XLabels[1] = "OR of active chains";
    m_NBins = 2;
  }
  int GetOrBin() const {return GetFirstSigHistBin() + m_firstBin + BINS::OR;}
  int GetAllEvsBin() const {return GetFirstSigHistBin() + m_firstBin + BINS::ALL;}

  void PrintBlock() {
    (*m_log) << MSG::DEBUG << "**** General Block ****" << endmsg;
    (*m_log) << MSG::DEBUG << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endmsg;
    (*m_log) << MSG::DEBUG << ", allBin: "  << BINS::ALL  << ", orBin: " << BINS::OR << endmsg;
  }
  
private:
  enum BINS {ALL=0,OR=1};
};



class GroupBlock: public BinBlock { // columns for groups
  
public:
  GroupBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log);
  const std::set<int>& GetBinSet(unsigned int chainCounter) const;
  void SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap);

  virtual void PrintBlock();
  
private:

  std::map<unsigned int, std::set<int> > m_ch_bin_map; // mapping from chain counter to bin
  std::set<int> m_emptyList;
};

class ChainBlock: public BinBlock { // columns for chains
  
public:
  ChainBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log);
  int GetSigHistValue(unsigned int chainCounter) const; // to be used by Fill for example
  int GetSigHistBin(unsigned int chainCounter) const; // to be used by SetBinContent for example

  virtual void PrintBlock();

private:
  std::map<unsigned int, int> m_ch_bin;
};

class StreamBlock : public BinBlock { // columns for streams
  
public:

  StreamBlock(const TrigConf::HLTChainList* allChains,
              std::set<std::string> &errorStreamNames, std::string trigLvl,
              MsgStream *log);

  const std::set<int> &GetSigHistValueSet(unsigned int chainCounter) const;
  int GetSigHistValue(const std::string& streamTag) const;
  int GetCorrelHistBin(const std::string& streamTag) const;
  int GetCatchAllBin() const {return m_catchAll;}

  void SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap);

  virtual void PrintBlock();  
private:

  void InsertStreams(bool isPhysType, const std::string& trgLvl,
                     const TrigConf::HLTChainList* allChains);

  std::map<std::string, int> m_str_tag_map; // mapping from tag name (name_type) to bin
  std::map<unsigned int, std::set<int> > m_ch_str_map; // mapping from chain counter to bin
  std::set<int> m_notFound;
  int m_catchAll;
};


TrigSignatureMoni::~TrigSignatureMoni()
{
  BinBlock::PrepareToBook();
}

void findChainsInStreams(std::map<std::string, TH1I*>& histograms, 
                         const std::vector<const HLT::SteeringChain*>& config, const std::string& level) 
{
  std::map<std::string, std::vector<std::string> > stream_to_chains; 

  for ( const HLT::SteeringChain* chain : config ) {
     for ( const HLT::StreamTag& stream : chain->getStreamTags()) {
      stream_to_chains[stream.getStream()].push_back(chain->getChainName());
    }
  }  
  for ( auto& p : stream_to_chains ) {

    TH1I* h = histograms[p.first] = new TH1I(("ChainsInStream_"+p.first).c_str(), ("Chains in " +level + "stream "+p.first).c_str(), p.second.size(), 0, p.second.size());
    int bin = 1;
    std::sort(p.second.begin(), p.second.end());   // sort alphabetically
    for ( const std::string& label : p.second) {
      h->GetXaxis()->SetBinLabel(bin, label.c_str());
      ++bin;
    }
  }

}

void fillChainsInStreams(std::map<std::string, TH1I*>& histograms, const std::vector<const HLT::SteeringChain*>& result) 
{
  for( const HLT::SteeringChain* chain : result ) {
    if (chain->chainPassed()){
      for( const HLT::StreamTag& stream :  chain->getStreamTags() ){
        if ( ! stream.isPrescaled() ) {
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

  std::set<std::string> errorStreamNames;
  for (const auto& s : m_parentAlg->getErrorStreamTags()) {
    errorStreamNames.insert(s.name()+"_"+s.type());
    ATH_MSG_INFO("adding error stream name: " << s.name()+"_"+s.type());
  }

  // Some preliminaries to setting up m_signatureAcceptanceHist...

  const TrigConf::HLTChainList *chainList = m_trigConfigSvc->chainList();

  BinBlock::PrepareToBook();

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
    
  // Step For EventBuilding histogram  
 if(m_trigLvl == "HLT") {
   m_stepForEBHist = new TH1I("StepForEB", "Step at which EB is called  ", 60, -30., 30.);
   if ( expertHistograms.regHist(m_stepForEBHist).isFailure()){
     ATH_MSG_WARNING("Can't book " << m_histoPathexpert+ m_stepForEBHist->GetName());
   }
   m_stepForEBHist->GetYaxis()->SetTitle("Events");
   m_stepForEBHist->GetXaxis()->SetTitle("Step of Event Building");
 }
 
  // Chain length histogram
  std::string title = "Length of Chains in  " + m_trigLvl;
  m_chainlengthHist = new TH1I("ChainLength",title.c_str(),
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
      m_streamCorrel->GetXaxis()->SetBinLabel(i+1, m_streamBlock->GetXLabel(i).data());
      m_streamCorrel->GetYaxis()->SetBinLabel(i+1, m_streamBlock->GetXLabel(i).data());
    }
    if ( expertHistograms.regHist(m_streamCorrel).isFailure()){
      ATH_MSG_WARNING("Can't register "<< m_streamCorrel->GetName() << ", deleting");
      delete m_streamCorrel;
      m_streamCorrel = 0;
    }

  }

  // Signature acceptance histogram
  title = "Raw acceptance of signatures in "+ m_trigLvl;
  m_signatureAcceptanceHist = new
    TrigLBNHist<TH2I>(TH2I("SignatureAcceptance",title.c_str(), 
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
  for ( const auto& s : m_chainsInStream ) { // (string,TH1I*)
    if ( expertHistograms.regHist(s.second).isFailure()) {
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

  const EventInfo* constEventInfo(0);
  StatusCode sc_cei =  evtStore()->retrieve(constEventInfo);

  if(sc_cei.isFailure()){
    ATH_MSG_WARNING("Can't get EventInfo object (for stream tags & Lumiblock number)");
  }

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
  for (const HLT::SteeringChain* ch : activeChains) {
    
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
  for (unsigned int step=0; step <= BinBlock::GetMaxSteps() ;step++) {
    if(step < maxsuccessfulSteps)
      m_signatureAcceptanceHist->Fill(m_generalBlock->GetOrBin(), BinBlock::GetFirstAlgBin() + step);
  }

  
  if(maxsuccessfulSteps > BinBlock::GetMaxSteps()){
    ATH_MSG_ERROR("Something wrong? maxsuccessfulSteps "<<maxsuccessfulSteps<<" > m_sv.maxSteps "
                  <<  BinBlock::GetMaxSteps() <<" ?");
  }

 
  if(sc_cei) {
    //use EventInfo -> TriggerInfo-> Stream Tag  to get streams this event contributes to
    std::vector<TriggerInfo::StreamTag> streamTags = constEventInfo->trigger_info()->streamTags();
    
    if(streamTags.size() > 0)
      m_signatureAcceptanceHist->Fill(m_generalBlock->GetAllEvsBin(), BinBlock::GetPTBin());
    
    // fill total out per stream including additional streamTag prescales
    
    for(const TriggerInfo::StreamTag& st : streamTags) {
      std::string streamKey = st.name()+"_"+st.type();
      
      int binNo = m_streamBlock->GetSigHistValue(streamKey);
      if(binNo != m_streamBlock->GetCatchAllBin())
        m_signatureAcceptanceHist->Fill( binNo, BinBlock::GetPTBin());
    }
    
    if(m_streamCorrel != 0) { // Fill the correlation histogram, EF only
      for(std::vector<TriggerInfo::StreamTag>::iterator iter=streamTags.begin(); iter != streamTags.end(); iter++){
        std::string streamKey = iter->name()+"_"+iter->type();

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

BinBlock::BinBlock(BinBlock *binBlock, MsgStream *log): 
  m_NBins(0), m_log(log)
{

  m_sv.firstSigHistBin = -1; // for historical reasons!
  m_sv.unknown = "unknown";
  m_sv.totalBins = 0;
  
  for(const BinBlock* bb : m_sv.binBlocks) {
    m_sv.totalBins += bb->GetNBins();
  }
  
  m_firstBin = m_sv.totalBins;
  
  m_sv.binBlocks.push_back(binBlock);

}

// This function is badly named. All it does is delete the binBlocks if they exist and clean up.
void BinBlock::PrepareToBook()
{
  for(BinBlock* bb : m_sv.binBlocks) {
    delete bb;
  }
  m_sv.binBlocks.clear();
  m_sv.stepnames.clear();
}

void BinBlock::FinalizeInitialization()
{
  m_sv.totalBins = 0;

  for(const BinBlock* bb : m_sv.binBlocks) {
    m_sv.totalBins += bb->GetNBins();
  }

  // set the step name
  m_sv.stepnames.push_back("errors");
  m_sv.stepnames.push_back("input");
  m_sv.stepnames.push_back("!PS rate");
  m_sv.stepnames.push_back("algoIn");
  

  // give numbers to the steps
  for (unsigned int i_step = 1; i_step <= m_sv.maxSteps; i_step++){
    m_sv.stepnames.push_back("step "+std::to_string(i_step));
  }

  m_sv.stepnames.push_back("raw rate");
  m_sv.stepnames.push_back("total rate");
  m_sv.stepnames.push_back("rerun rate");

  m_sv.errorBin     = 0;
  m_sv.inputBin     = 1;
  m_sv.psBin        = 2;
  m_sv.algoInBin    = 3;
  m_sv.firstStepBin = 4;
  m_sv.rawBin   = m_sv.maxSteps + 4;
  m_sv.ptBin    = m_sv.maxSteps + 5;
  m_sv.rerunBin = m_sv.maxSteps + 6; 

}


const std::string& BinBlock::GetSigXAxisLabel(int bin)
{
  for (BinBlock* bb : m_sv.binBlocks) {
    int firstBin = bb->GetFirstBin();
    if(bin >= firstBin && bin < firstBin + int(bb->GetNBins())) {
      return bb->GetXLabel(bin - firstBin);
    }
  }
  return m_sv.unknown;
}

GroupBlock::GroupBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log) 
  : BinBlock(this, log)
{

  std::map<std::string, int> groupBinMap;
  m_NBins = 0;

  // First populate groupBinMap with the group names
  // Done this way to ensure that groups are in alphabetical order in histo
  for ( const HLT::SteeringChain * chain : configuredChains ) {
     for ( const std::string& group : chain->getConfigChain()->groups() )
        groupBinMap[group] = 0;
  }

  // Assign bins, set label
  for ( auto group_bin : groupBinMap ) {
      m_XLabels[m_NBins] = "grp_" + group_bin.first;
      group_bin.second = GetFirstSigHistBin() + m_firstBin + m_NBins++;
  }

  // Now set the bin maps for each chain
  for ( const HLT::SteeringChain * chain : configuredChains ) {
    std::set<int> binNums;
    for ( const std::string& group : chain->getConfigChain()->groups() )
       binNums.insert(groupBinMap[group]);
    
    m_ch_bin_map[ chain->getChainCounter() ] = binNums;      
  }
}

const std::set<int> &GroupBlock::GetBinSet(unsigned int chainCounter) const
{
  const auto it = m_ch_bin_map.find(chainCounter);
  if(it == m_ch_bin_map.end())
    return m_emptyList;

  return it->second;
}

void GroupBlock::PrintBlock()
{
  (*m_log) << MSG::DEBUG  << "Group Block " 
	   << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endmsg;

  std::map<unsigned int, std::set<int> >::const_iterator mcbit;
  for(mcbit = m_ch_bin_map.begin(); mcbit != m_ch_bin_map.end(); mcbit++) {
    (*m_log) << MSG::DEBUG << "chain counter: " << mcbit->first << ", bins: ";
    for( std::set<int>::const_iterator si = (mcbit->second).begin(); si != (mcbit->second).end(); si++) 
      (*m_log) << MSG::DEBUG << (*si) << ", ";
    (*m_log) << MSG::DEBUG << endmsg;
  }
}

void GroupBlock::SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap) 
{
  const std::set<int>& groups = GetBinSet(chainCounter);

  for(int hg : groups) {
    auto it = hitMap.find(hg);
    if(it == hitMap.end())
      hitMap[hg] = 0;
    else
      it->second |= mask;
  }
}

ChainBlock::ChainBlock(const std::vector<const HLT::SteeringChain*>& configuredChains, MsgStream *log) 
  : BinBlock(this, log)
{
  m_NBins = 0;
  unsigned int maxSteps = 0;

  // This is here to produce an alphabetically sorted list of chains (to address Savannah bug 79938
  std::map<std::string, unsigned int> chainname;
  for (const HLT::SteeringChain* ch : configuredChains) {
    chainname[ch->getChainName()]= ch->getChainCounter();

    const std::vector<const HLT::Signature*>& signatures = ch->getSignatures();
    // searching for the longest (max. steps) configured chain
    maxSteps = signatures.size() > maxSteps ? signatures.size() : maxSteps;
  }
  
  for (const auto& kv : chainname) {  // (name,chainCounter)
    m_XLabels[m_NBins] = kv.first + "_ChCo_" + std::to_string(kv.second);
    m_ch_bin[kv.second] = GetFirstSigHistBin() + m_firstBin + m_NBins++;
  }
  BinBlock::SetMaxSteps(maxSteps);
}


void ChainBlock::PrintBlock()
{
  (*m_log) << MSG::DEBUG << "Chain Block " 
	   << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endmsg;

  std::map<unsigned int, int>::const_iterator mcbit;

  for( mcbit = m_ch_bin.begin(); mcbit != m_ch_bin.end(); mcbit++) {
    (*m_log) << MSG::DEBUG << mcbit->first << ": " << mcbit->second << "   " << endmsg;
  }

}

int ChainBlock::GetSigHistValue(unsigned int chainCounter) const
{
  const auto it = m_ch_bin.find(chainCounter);
  if(it != m_ch_bin.end())
    return it->second;

  return OverflowBin();

}

int ChainBlock::GetSigHistBin(unsigned int chainCounter) const
{
  const auto it = m_ch_bin.find(chainCounter);
  if(it != m_ch_bin.end())
    return it->second - GetFirstSigHistBin();

  return OverflowBin();
  
}


StreamBlock::StreamBlock(const TrigConf::HLTChainList* allChains,
			 std::set<std::string> &errorStreamNames, std::string trigLvl,
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
  for(const auto &s : errorStreamNames) {
    m_XLabels[m_NBins] = "str_" + s;
    m_str_tag_map[s] = m_NBins++;
  }

  // now a catch-all         --- suppressed on request from Brian 7.4.2011
  //m_XLabels[m_NBins] = "str_other";
  //m_str_tag_map["other"] = m_NBins;
  //m_catchAll = m_NBins++;
  m_catchAll = -1;


  if(m_log->level() <= MSG::DEBUG) {
    (*log) << MSG::DEBUG << "************ stream tag to bin map *****************" << endmsg;

    std::map<std::string, int>::const_iterator stmit;
    for(stmit = m_str_tag_map.begin(); stmit != m_str_tag_map.end(); stmit++)
      (*log) << MSG::DEBUG << (*stmit).first << ": " << (*stmit).second << endmsg;
    
    (*log) << MSG::DEBUG << "*********** chain counter to bin map for streams ***************" << endmsg;
    std::map<unsigned int, std::set<int> >::const_iterator csmit;
    for(csmit = m_ch_str_map.begin(); csmit != m_ch_str_map.end(); csmit++) {
      (*log) << MSG::DEBUG << (*csmit).first << ": ";
      std::set<int>::iterator si;
      for(si = (*csmit).second.begin(); si != (*csmit).second.end(); si++)
	(*log) << (*si) << "  ";
      (*log) << endmsg;
    }
  }

}

void StreamBlock::InsertStreams(bool isPhysType, const std::string& trgLvl,
                                const TrigConf::HLTChainList* allChains)
{
  
  TrigConf::HLTChainList::const_iterator cit;
  
  // First get the names of the streams into a local set (for ordering).
  // Then use it to set up the m_str_tag_map, then set up the chain to stream map

  std::set<std::string> streamNames;
  for(cit = allChains->begin(); cit != allChains->end(); cit++) {

    if( (*cit)->level() != trgLvl)
      continue;

    for(const TrigConf::HLTStreamTag* st : (*cit)->streams()) {
       if( (isPhysType && st->type() == "physics") || (!isPhysType && st->type() != "physics")) {
          const std::string namkey = st->stream()+"_"+st->type();
          streamNames.insert(namkey);
       }
    }
  }

  // Set up m_ch_str_map and the labels vector
  for(const std::string& sn : streamNames) {
    m_str_tag_map[ sn ] = m_NBins;
    m_XLabels[m_NBins] = "str_" + sn;
    m_NBins++;
  }

  // Set up the chain to streams map
  for(cit = allChains->begin(); cit != allChains->end(); cit++) {

     if( (*cit)->level() != trgLvl)
        continue;

      for(const TrigConf::HLTStreamTag* st : (*cit)->streams()) {
       if( (isPhysType && st->type() == "physics") || (!isPhysType && st->type() != "physics")) {
         const std::string namkey = st->stream()+"_"+st->type();
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
    (*m_log) << MSG::DEBUG  << "**** Stream Block ****" << endmsg
             << "First bin: " << m_firstBin << ", Number of bins: " << m_NBins << endmsg;
    (*m_log) << MSG::DEBUG  << "Stream tag to bin map " << endmsg;
    std::map<std::string, int>::iterator mstit;
    for(mstit = m_str_tag_map.begin(); mstit != m_str_tag_map.end(); mstit++) {
      (*m_log) << MSG::DEBUG  << mstit->first << ": " << mstit->second << endmsg;
    }
    (*m_log) << MSG::DEBUG  << "Chain counter to bin map" << endmsg;
    std::map<unsigned int, std::set<int> >::iterator mcsit;
    for(mcsit = m_ch_str_map.begin(); mcsit != m_ch_str_map.end(); mcsit++) {
      (*m_log) << MSG::DEBUG  << "chain counter " << mcsit->first << ": ";
      for(int si : mcsit->second)
        (*m_log) << MSG::DEBUG  << si << ", ";
      (*m_log) << MSG::DEBUG  << endmsg;
    }
  }
}

const std::set<int> &StreamBlock::GetSigHistValueSet(unsigned int chainCounter) const
{
  const auto it = m_ch_str_map.find(chainCounter);
  if(it != m_ch_str_map.end())
    return it->second;
  
  return m_notFound;
     
}

int StreamBlock::GetSigHistValue(const std::string& streamTag) const
{
  const auto it = m_str_tag_map.find(streamTag);
  if(it != m_str_tag_map.end())
    return GetFirstSigHistBin() + m_firstBin + it->second;

  return m_catchAll;
}

int StreamBlock::GetCorrelHistBin(const std::string& streamTag) const
{
  const auto it = m_str_tag_map.find(streamTag);
  if(it != m_str_tag_map.end())
    return it->second;

  return m_catchAll;
}


void StreamBlock::SetMask(unsigned int chainCounter, unsigned int mask, std::map<int, unsigned int> &hitMap) 
{
  const std::set<int>& streams = GetSigHistValueSet(chainCounter);
  for(int hs : streams) {
    hitMap[hs] |= mask;     // this works because default value of new int entry is 0
  }
}
    
