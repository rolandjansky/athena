/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "PileupReweighting/PileupReweightingTool.h"


#include "PathResolver/PathResolver.h"

#include "xAODEventInfo/EventInfoAuxContainer.h"

namespace CP {

PileupReweightingTool::PileupReweightingTool( const std::string& name ) : asg::AsgTool( name ), Root::TPileupReweighting(name.c_str()), m_inConfigMode(false) {

   declareProperty("ConfigFiles", m_prwFiles, "List of prw config files"); //array of files
   declareProperty("LumiCalcFiles", m_lumicalcFiles, "List of lumicalc files"); //array of files
   declareProperty("LumiCalcTriggers",m_lumicalcTriggers, "List of triggers associated to each lumicalc file. 'None' is a keyword. If none specified, will assume all are 'None'"); //alias names - if not same length of LumiCalcFiles, will just use "None" (the default)
   declareProperty("UnrepresentedDataAction",m_unrepdataAction=2);
   declareProperty("DataScaleFactor",m_dataScaleFactor=1.);
   declareProperty("DefaultChannel",m_defaultChannel=0); //when channel info not present in config file, use this channel instead
   declareProperty("UsePeriodConfig",m_usePeriodConfig="","Use this period configuration when in config generating mode. Leave blank to try auto-detect");
   declareProperty("DataWeightTriggers",m_dataWeightTriggers,"List of triggers used to calculate dataweights and attach to eventinfo during apply method, if running on data");
   declareProperty("MuDependentDataWeights",m_useMuDependent=true,"Mu dependent data weights means calculating dataweights from luminosity in the bins of mu - more accurate than doing a global weight with all the luminosity");
   declareProperty("Prefix",m_prefix="","Prefix to attach to all decorations");
   declareProperty("Input",m_inputKey="","Specify a specific EventInfo object");
   declareProperty("Output",m_outputKey="","Specify an output EventInfo object. If differs from input, will create a clone of EventInfo and decorate that");
}

StatusCode PileupReweightingTool::initialize() {

   //should we set the period config (file maker mode)
   if(m_prwFiles.size()+m_lumicalcFiles.size()==0) {
      m_inConfigMode=true;
      ATH_MSG_INFO("In Config file making mode."); 
      if(m_usePeriodConfig!="") {
         /*m_tool->*/UsePeriodConfig(m_usePeriodConfig);
      } 
   } else {
      //have we any prw to load
      for(unsigned int j=0;j<m_prwFiles.size();j++) {
            ATH_MSG_INFO("Locating File: " << m_prwFiles[j]);
            std::string file = PathResolverFindCalibFile(m_prwFiles[j]);
            if(file=="") { ATH_MSG_ERROR("Unable to find the PRW Config file: " << m_prwFiles[j]); return StatusCode::FAILURE; }
            /*m_tool->*/AddConfigFile(file.c_str());
      }

      //have we any lumicalc files to load
      bool useNoneLabel = (m_lumicalcFiles.size() != m_lumicalcTriggers.size());
      for(unsigned int j=0;j<m_lumicalcFiles.size();j++) {
            ATH_MSG_INFO("Locating File: " << m_lumicalcFiles[j]);
            std::string file = PathResolverFindCalibFile(m_lumicalcFiles[j]);
            if(file=="") { ATH_MSG_ERROR("Unable to find the Lumicalc file: " << m_lumicalcFiles[j]); return StatusCode::FAILURE; }
            /*m_tool->*/SetDataScaleFactors(m_dataScaleFactor);
            /*m_tool->*/AddLumiCalcFile(file.c_str(),(useNoneLabel)? "None" : m_lumicalcTriggers[j]);
      }
      /*m_tool->*/SetUnrepresentedDataAction(m_unrepdataAction);
      /*m_tool->*/SetDefaultChannel(m_defaultChannel);
   }

   //delay initializing underlying tool until first usage, just in case user wants to do any advanced initialization options

   return StatusCode::SUCCESS;
}

StatusCode PileupReweightingTool::finalize() {
   if(m_inConfigMode) {
      //write the prw config files
      std::string nameWithoutParent = this->name().substr(this->name().find(".")+1);
      TString fileName = TString::Format("%s.prw.root", nameWithoutParent.c_str());
      /*m_tool->*/WriteToFile(fileName);
   }
   return StatusCode::SUCCESS;
}

void PileupReweightingTool::checkInit(const xAOD::EventInfo* eventInfo) {
   if(!/*m_tool->*/IsInitialized()) {
      if(m_usePeriodConfig=="") { //try autodetect based on runnum of first event 
         switch(eventInfo->runNumber()) {
            case 180164: case 183003: case 186169: case 189751:  m_usePeriodConfig="MC12c";/*m_tool->*/UsePeriodConfig("MC12c");break;
            case 195847: case 195848: m_usePeriodConfig="MC12ab";/*m_tool->*/UsePeriodConfig("MC12ab");break;
            case 212272: m_usePeriodConfig="MC14";/*m_tool->*/UsePeriodConfig("MC14");break;
         }
      }
      if(/*m_tool->*/Initialize()!=0) throw std::runtime_error("PileupReweighting: Failure in init");
   }
   return;
}

void PileupReweightingTool::apply(const xAOD::EventInfo* eventInfo) {
   checkInit(eventInfo);

   if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      //allowed to do data weights 
      for(auto& trigger : m_dataWeightTriggers) {
         eventInfo->auxdecor<double>(m_prefix+trigger) = dataWeight(eventInfo, trigger, m_useMuDependent);
      }
      return;
   }

   if(m_inConfigMode)  {
      Fill(eventInfo->runNumber(), eventInfo->mcChannelNumber(), eventInfo->mcEventWeight(), eventInfo->averageInteractionsPerCrossing());
      return;
   }

   //decorate with standard PileupWeight 
   eventInfo->auxdecor<double>(m_prefix+"PileupWeight") = /*m_tool->*/GetCombinedWeight(   eventInfo->runNumber(), eventInfo->mcChannelNumber(), eventInfo->averageInteractionsPerCrossing() );
   //decorate with random run number etc
   //using mu-dependent randomization by default ... jetetmiss seem to like it muchly
   /*m_tool->*/SetRandomSeed(314159+eventInfo->mcChannelNumber()*2718+eventInfo->eventNumber()); //to make randomization repeatable
   eventInfo->auxdecor<unsigned int>(m_prefix+"RandomRunNumber") = /*m_tool->*/GetRandomRunNumber(   eventInfo->runNumber(), eventInfo->averageInteractionsPerCrossing()  );
   eventInfo->auxdecor<unsigned int>(m_prefix+"RandomLumiBlockNumber") = (eventInfo->auxdecor<unsigned int>(m_prefix+"RandomRunNumber")==0) ? 0 : /*m_tool->*/GetRandomLumiBlockNumber(  eventInfo->auxdecor<unsigned int>(m_prefix+"RandomRunNumber")  );

   return; //FIXME return statuscode instead maybe...
}

double PileupReweightingTool::dataWeight(const xAOD::EventInfo* eventInfo, TString trigger, bool mu_dependent) {
   checkInit(eventInfo);

   if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      ATH_MSG_WARNING("Requesting Data Weight is not intended for simulated events. Returning 1.");
      return 1; //no data weights for simulated events
   }

   if(!mu_dependent) return /*m_tool->*/GetDataWeight(eventInfo->runNumber(), trigger);
   return /*m_tool->*/GetDataWeight( eventInfo->runNumber(), trigger, eventInfo->averageInteractionsPerCrossing() );

}

StatusCode PileupReweightingTool::execute() {
   const xAOD::EventInfo* evtInfo =0;

   if(m_inputKey.length()>0) ATH_CHECK(evtStore()->retrieve(evtInfo,m_inputKey));
   else {
      #ifdef XAOD_STANDALONE
         ATH_CHECK( evtStore()->retrieve(evtInfo,"") ); //apparently TEvent can't do keyless retrieves!!??
      #else
         ATH_CHECK(evtStore()->retrieve(evtInfo));
      #endif
   }

   //do we need to make a copy??
   if(m_inputKey!=m_outputKey && m_outputKey!="") {
      xAOD::EventInfo* evtInfoCopy = new xAOD::EventInfo( *evtInfo );
      xAOD::EventInfoAuxContainer* aux = new xAOD::EventInfoAuxContainer;
      evtInfoCopy->setStore(aux);
      ATH_CHECK( evtStore()->record( evtInfoCopy , m_outputKey ) );
      ATH_CHECK( evtStore()->record( aux , m_outputKey+"Aux." ) );
      evtInfo = evtInfoCopy;
   }

   apply(evtInfo);

   return StatusCode::SUCCESS;

}


} // namespace CP