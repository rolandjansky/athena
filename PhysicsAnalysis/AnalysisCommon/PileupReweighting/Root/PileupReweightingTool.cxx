/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "PileupReweighting/PileupReweightingTool.h"


#include "PathResolver/PathResolver.h"

#include "PATInterfaces/SystematicRegistry.h"

#ifndef XAOD_STANDALONE
#include "GaudiKernel/ITHistSvc.h"
#endif

#ifdef XAOD_STANDALONE
#include "ReweightUtils/McEventWeight.h"
#endif

#include "TH1.h"
#include "TTree.h"


namespace CP {

PileupReweightingTool::PileupReweightingTool( const std::string& name ) :CP::TPileupReweighting(name.c_str()), asg::AsgTool( name ), 
									 m_inConfigMode(false), 
									 m_upTool(0), m_downTool(0), m_systUp("PRW_DATASF", 1 ), m_systDown("PRW_DATASF", -1), 
									 m_activeTool(this), 
									 m_noWeightsMode(false), 
									 m_weightTool("McEventWeight/myWeightTool") {

#ifndef XAOD_STANDALONE
   declareProperty("ConfigOutputStream", m_configStream="", "When creating PRW config files, this is the THistSvc stream it goes into. If blank, it wont write this way");
#endif

   declareProperty("ConfigFiles", m_prwFiles, "List of prw config files"); //array of files
   declareProperty("ConfigFilesPathPrefix", m_prwFilesPathPrefix="", "Path of additional folder structure in prw config files"); //string prefix
   declareProperty("LumiCalcFiles", m_lumicalcFiles, "List of lumicalc files, in the format '<filename>:<trigger>' .. if no trigger given, 'None' is assumed"); //array of files
   declareProperty("Prefix",m_prefix="","Prefix to attach to all decorations ... only used in the 'apply' method");
   declareProperty("UnrepresentedDataAction",m_unrepresentedDataAction=3,"1 = remove unrepresented data, 2 = leave it there, 3 = reassign it to nearest represented bin");
   declareProperty("DataScaleFactor",m_dataScaleFactorX=1.);
   declareProperty("DefaultChannel",m_defaultChannel=0); //when channel info not present in config file, use this channel instead
   declareProperty("UsePeriodConfig",m_usePeriodConfig="auto","Use this period configuration when in config generating mode. Set to 'auto' to auto-detect");

   declareProperty("DataScaleFactorUP",m_upVariation=0.,"Set to a value representing the 'up' fluctuation - will report a PRW_DATASF uncertainty to Systematic Registry");
   declareProperty("DataScaleFactorDOWN",m_downVariation=0.,"Set to a value representing the 'down' fluctuation - will report a PRW_DATASF uncertainty to Systematic Registry");

   declareProperty("LumiCalcRunNumberOffset",m_lumicalcRunNumberOffset=0,"Use to 'fake' a Run2 lumicalc file. Suggest using a value of 22000 to do this from an 8TeV lumicalc file");

#ifdef XAOD_STANDALONE
   declareProperty( "WeightTool", m_weightTool = new McEventWeight("myWeightTool"),"The tool to compute the weight in the sumOfWeights");
#else
   declareProperty( "WeightTool", m_weightTool,"The tool to compute the weight in the sumOfWeights");
#endif

#ifndef XAOD_STANDALONE
   //attached update handler to the outputlevel property, so we can pass changes on to the underlying tool 
   auto props = getProperties();
 	for( Property* prop : props ) {
      if( prop->name() != "OutputLevel" ) {
         continue;
      }
      prop->declareUpdateHandler( &PileupReweightingTool::updateHandler, this );
      break;
   }
#endif

}

#ifndef XAOD_STANDALONE
//rootcore can't do this yet!
void PileupReweightingTool::updateHandler(Property& p) {
   //call the original update handler 
   this->msg_update_handler(p);
   EnableDebugging(this->msgLvl(MSG::DEBUG));
}
#endif

bool PileupReweightingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
   CP::SystematicSet sys = affectingSystematics(); return sys.find( systematic ) != sys.end();
}

CP::SystematicSet PileupReweightingTool::affectingSystematics() const {
   CP::SystematicSet result;
   if(m_upVariation) result.insert( m_systUp  );
   if(m_downVariation) result.insert( m_systDown );
   return result;
}

CP::SystematicSet PileupReweightingTool::recommendedSystematics() const {
   return affectingSystematics();
}

CP::SystematicCode PileupReweightingTool::applySystematicVariation( const CP::SystematicSet& systConfig ) { 
   if(systConfig.find( m_systUp ) != systConfig.end() && systConfig.find( m_systDown ) != systConfig.end()) {
      ATH_MSG_ERROR("Errr... what the!? You're trying to do both PRW_DATASF directions at the same time!!!??"); 
      return SystematicCode::Unsupported; 
   }
   if(systConfig.find( m_systUp ) != systConfig.end()) {
      if(!m_upTool) { ATH_MSG_ERROR("Requested up variation of PRW_DATASF, but not configured to do this :-("); return SystematicCode::Unsupported; }
       m_activeTool = m_upTool;
   }
   else if(systConfig.find( m_systDown ) != systConfig.end() ) {
      if(!m_downTool) { ATH_MSG_ERROR("Requested down variation of PRW_DATASF, but not configured to do this :-("); return SystematicCode::Unsupported; }
      m_activeTool = m_downTool;
   }
   else m_activeTool = this;
   return SystematicCode::Ok;
}


float PileupReweightingTool::getLumiBlockMu( const xAOD::EventInfo& eventInfo ) { return CP::TPileupReweighting::GetLumiBlockMu(eventInfo.runNumber(),eventInfo.lumiBlock()); }

double PileupReweightingTool::getLumiBlockIntegratedLumi(const xAOD::EventInfo& eventInfo) { return CP::TPileupReweighting::GetLumiBlockIntegratedLumi(eventInfo.runNumber(),eventInfo.lumiBlock()); }

StatusCode PileupReweightingTool::initialize() {

   //set debugging if debugging is on:
   EnableDebugging(this->msgLvl(MSG::DEBUG));

   //see if we need variations 
   if(m_upVariation) {
      m_upTool = new TPileupReweighting((name()+"_upVariation").c_str());
      m_upTool->SetUnrepresentedDataAction(m_unrepresentedDataAction);
      m_upTool->SetDataScaleFactors(m_upVariation);
   }
   if(m_downVariation) {
      m_downTool = new TPileupReweighting((name()+"_downVariation").c_str());
      m_downTool->SetUnrepresentedDataAction(m_unrepresentedDataAction);
      m_downTool->SetDataScaleFactors(m_downVariation);
   }

   SetDefaultChannel(m_defaultChannel); if(m_upTool) m_upTool->SetDefaultChannel(m_defaultChannel); if(m_downTool) m_downTool->SetDefaultChannel(m_defaultChannel);

   //should we set the period config (file maker mode)
   if(m_prwFiles.size()+m_lumicalcFiles.size()==0) {
      m_inConfigMode=true;
      ATH_MSG_INFO("In Config file making mode."); 
      if(m_usePeriodConfig!="auto" && m_usePeriodConfig!="") {
         /*m_tool->*/UsePeriodConfig(m_usePeriodConfig);
      }
#ifndef XAOD_STANDALONE
      //retrieve histsvc now, otherwise it wont get correctly configured by the time it's used in the finalize ... so it seems
      ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
      CHECK( histSvc.retrieve() );
#endif
   } else {
      //have we any prw to load
      for(unsigned int j=0;j<m_prwFiles.size();j++) {
            ATH_MSG_INFO("Locating File: " << m_prwFiles[j]);
            std::string file = PathResolverFindCalibFile(m_prwFiles[j]);
            if(file=="") { ATH_MSG_ERROR("Unable to find the PRW Config file: " << m_prwFiles[j]); return StatusCode::FAILURE; }
            /*m_tool->*/AddConfigFile(file.c_str());
            if(m_upTool) m_upTool->AddConfigFile(file.c_str()); if(m_downTool) m_downTool->AddConfigFile(file.c_str());
      }

      //have we any lumicalc files to load? .. if we do and had no prwFiles then the user must specify the period configuration
      if(m_lumicalcFiles.size()>0 && m_prwFiles.size()==0) {
         if(m_usePeriodConfig!="auto" && m_usePeriodConfig!="") {
            ATH_MSG_INFO("Setting up without a PRW config file, but with period config " << m_usePeriodConfig << ". You will only be able to use random run number and data weight functionality... no reweighting!");
            if(UsePeriodConfig(m_usePeriodConfig)!=0) {
               ATH_MSG_ERROR("Unrecognised PeriodConfig: " << m_usePeriodConfig); return StatusCode::FAILURE;
            }
            m_noWeightsMode=true; //will stop the prw weight being decorated in apply method
            if(m_upTool) m_upTool->UsePeriodConfig(m_usePeriodConfig); if(m_downTool) m_downTool->UsePeriodConfig(m_usePeriodConfig);
         } else {
            ATH_MSG_INFO("No config files provided, but " << m_lumicalcFiles.size() << " lumicalc file provided. Please specify a UsePeriodConfig if you want to use the tool without a config file (e.g. do 'MC15') ");
            return StatusCode::FAILURE;
         }
      }
   
      for(unsigned int j=0;j<m_lumicalcFiles.size();j++) {
            //see if there's a trigger at the end of the filename .. format is "file:trigger" 
            TString myFile = m_lumicalcFiles[j];
            TString myTrigger = (myFile.Contains(':')) ? TString(myFile(myFile.Last(':')+1,myFile.Length()-myFile.Last(':'))) : TString("None");
            myFile = (myFile.Contains(':')) ? TString(myFile(0,myFile.Last(':'))) : myFile;
            ATH_MSG_INFO("Locating File: " << myFile);
            std::string file = PathResolverFindCalibFile(myFile.Data());
            if(file=="") { ATH_MSG_ERROR("Unable to find the Lumicalc file: " << myFile); return StatusCode::FAILURE; }
            /*m_tool->*/AddLumiCalcFile(file.c_str(),myTrigger);
            if(m_upTool) m_upTool->AddLumiCalcFile(file.c_str(),myTrigger); if(m_downTool) m_downTool->AddLumiCalcFile(file.c_str(),myTrigger);
      }
   }

   //register ourselves with the systematic registry! 
   CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
   if( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;


   //delay initializing underlying tool until first usage, just in case user wants to do any advanced initialization options

   ATH_MSG_DEBUG("Retrieving weight tool...");
   StatusCode sc = m_weightTool.retrieve();
   if (sc != StatusCode::SUCCESS) {
     ATH_MSG_ERROR("    " << m_weightTool->name() << " could not be retrieved.");
     return sc;
   }
   else {
     ATH_MSG_DEBUG("    " << m_weightTool->name() << " retrieved.");
     m_weightTool->print();
   }

   return StatusCode::SUCCESS;
}

StatusCode PileupReweightingTool::finalize() {
   if(m_inConfigMode) {
#ifndef XAOD_STANDALONE
   if(m_configStream=="") {
      //write the prw config files
      std::string nameWithoutParent = this->name().substr(this->name().find(".")+1);
      TString fileName = TString::Format("%s.prw.root", nameWithoutParent.c_str());
      /*m_tool->*/WriteToFile(fileName);
   } else {
      //write to the histsvc stream instead ...
      
      ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
      CHECK( histSvc.retrieve() );

      TTree *outTreeMC=0;
      TTree *outTreeData=0;
      Int_t channel = 0;UInt_t runNumber = 0; 
      std::vector<UInt_t>* pStarts = 0;std::vector<UInt_t>* pEnds = 0;
      Char_t histName[150];


      //loop over periods ... periods only get entry in table if they have an input histogram 
      for(auto period : m_periods) {
         if(!period.second) continue; //should never happen, but just in case!
         if(period.first != period.second->id) continue; //skips redirects 
         runNumber = period.first;
         pStarts = new std::vector<UInt_t>;pEnds = new std::vector<UInt_t>;
         if(period.second->subPeriods.size()==0) { 
            pStarts->push_back(period.second->start); pEnds->push_back(period.second->end); 
         }
         else {
            for(auto subp : period.second->subPeriods) {
               pStarts->push_back(subp->start); pEnds->push_back(subp->end);
            }
         }
         for(auto inHist : period.second->inputHists) {
            channel = inHist.first;
            TH1* hist = inHist.second;
            strcpy(histName,hist->GetName());
            CHECK( histSvc->regHist(TString::Format("/%s/PileupReweighting/%s",m_configStream.c_str(),hist->GetName()).Data(),hist) );
            if(!outTreeMC) {
               outTreeMC = new TTree("MCPileupReweighting","MCPileupReweighting");
               outTreeMC->Branch("Channel",&channel);
               outTreeMC->Branch("RunNumber",&runNumber);
               outTreeMC->Branch("PeriodStarts",&pStarts);
               outTreeMC->Branch("PeriodEnds",&pEnds);
               outTreeMC->Branch("HistName",&histName,"HistName/C");
               CHECK( histSvc->regTree(TString::Format("/%s/PileupReweighting/%s",m_configStream.c_str(),outTreeMC->GetName()).Data(),outTreeMC) );
            }
            outTreeMC->Fill();
         }
         delete pStarts; delete pEnds;
      }

      //loop over data 
      for(auto& run : m_runs) {
         runNumber = run.first;
         if(run.second.inputHists.find("None")==run.second.inputHists.end()) continue;
   
         TH1* hist = run.second.inputHists["None"];
         strcpy(histName,hist->GetName());
         CHECK( histSvc->regHist(TString::Format("/%s/PileupReweighting/%s",m_configStream.c_str(),hist->GetName()).Data(),hist) );
         if(!outTreeData) {
            outTreeData = new TTree("DataPileupReweighting","DataPileupReweighting");
            outTreeData->Branch("RunNumber",&runNumber);
            outTreeData->Branch("HistName",&histName,"HistName/C");
            CHECK( histSvc->regTree(TString::Format("/%s/PileupReweighting/%s",m_configStream.c_str(),outTreeData->GetName()).Data(),outTreeData) );
         }
         outTreeData->Fill();
      }

      Info("WriteToFile", "Successfully generated config file to stream: %s",m_configStream.c_str());
      Info("WriteToFile", "Happy Reweighting :-)");

   }

#else
      //write the prw config files
      std::string nameWithoutParent = this->name().substr(this->name().find(".")+1);
      TString fileName = TString::Format("%s.prw.root", nameWithoutParent.c_str());
      /*m_tool->*/WriteToFile(fileName);
#endif
   }

   if(m_upTool) {delete m_upTool;m_upTool=0;} if(m_downTool)  {delete m_downTool;m_downTool=0;} 

   return StatusCode::SUCCESS;
}

ULong64_t PileupReweightingTool::getPRWHash( const xAOD::EventInfo& eventInfo ) {
   return m_activeTool->GetPRWHash(eventInfo.runNumber(), eventInfo.mcChannelNumber(), eventInfo.averageInteractionsPerCrossing() );
}

/// Return combined pileup weight
float PileupReweightingTool::getCombinedWeight( const xAOD::EventInfo& eventInfo ) {
   return m_activeTool->GetCombinedWeight(   eventInfo.runNumber(), eventInfo.mcChannelNumber(), eventInfo.averageInteractionsPerCrossing() );
}

float PileupReweightingTool::getCombinedWeight( const xAOD::EventInfo& eventInfo, Double_t x, Double_t y ) {
   return m_activeTool->GetCombinedWeight(   eventInfo.runNumber(), eventInfo.mcChannelNumber(), x, y );
}

/// When using UnrepresentedDataAction=2, you may want to apply this additional weight to ensure sum of weights are preserved
float PileupReweightingTool::getUnrepresentedDataWeight( const xAOD::EventInfo& eventInfo ) {
    return m_activeTool->GetUnrepresentedDataWeight( eventInfo.runNumber(), eventInfo.mcChannelNumber() );
}


/// Get a random run number for this MC event, mu_dependency is not recommended for this
int PileupReweightingTool::getRandomRunNumber( const xAOD::EventInfo& eventInfo , bool mu_dependent) {
   m_activeTool->SetRandomSeed(314159+eventInfo.mcChannelNumber()*2718+eventInfo.eventNumber()); //to make randomization repeatable
   if(mu_dependent) return m_activeTool->GetRandomRunNumber(   eventInfo.runNumber(), eventInfo.averageInteractionsPerCrossing()  );
   else return m_activeTool->GetRandomRunNumber( eventInfo.runNumber() );
}

int PileupReweightingTool::fill( const xAOD::EventInfo& eventInfo ) {
   return fill(eventInfo,eventInfo.averageInteractionsPerCrossing());
}

int PileupReweightingTool::fill( const xAOD::EventInfo& eventInfo, Double_t x, Double_t y) {
   
   //auto-detect the period config if necessary 
   if(m_usePeriodConfig=="auto" && !m_doneConfigs[eventInfo.runNumber()]) { //try autodetect based on runnum of first event ... 
      //if data, we only need to ensure a binning is done ... for now, we assume the MC15 binning
      if(!eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION)) {
         if(!m_emptyHistogram) { SetUniformBinning(100,0,100.); } //use a default binning
      } else {
         switch(eventInfo.runNumber()) {
            //case 180164: case 183003: case 186169: case 189751:  m_usePeriodConfig="MC12c";/*m_tool->*/UsePeriodConfig("MC12c");break;
            //case 195847: case 195848: m_usePeriodConfig="MC12ab";/*m_tool->*/UsePeriodConfig("MC12ab");break;
            case 212272: UsePeriodConfig("MC14_8TeV");break;
            case 222222: UsePeriodConfig("MC14_13TeV");break;
            case 222510: case 222525: UsePeriodConfig("MC15"); break;
         }
      }
      m_doneConfigs[eventInfo.runNumber()] = true;
   }

   return TPileupReweighting::Fill(eventInfo.runNumber(), eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION) ? eventInfo.mcChannelNumber() : -1 /*data*/, eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION) ? m_weightTool->getWeight() : 1., x, y);

}

StatusCode PileupReweightingTool::apply(const xAOD::EventInfo& eventInfo, bool mu_dependent) {

   if(m_inConfigMode)  {
      fill( eventInfo );
      return StatusCode::SUCCESS;
   }

   if(!eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION)) {
      eventInfo.auxdecor<float>(m_prefix+"corrected_averageInteractionsPerCrossing") = getLumiBlockMu(eventInfo);
      return StatusCode::SUCCESS;
   }

   //just copy the value over for MC
   eventInfo.auxdecor<float>(m_prefix+"corrected_averageInteractionsPerCrossing") = eventInfo.averageInteractionsPerCrossing();

   //decorate with standard PileupWeight 
   if(!m_noWeightsMode) {
      double weight =  /*m_tool->*/getCombinedWeight( eventInfo );
      if(m_unrepresentedDataAction==2) weight *= getUnrepresentedDataWeight( eventInfo );
      eventInfo.auxdecor<double>(m_prefix+"PileupWeight") = weight;
   }
   //decorate with random run number etc
   eventInfo.auxdecor<unsigned int>(m_prefix+"RandomRunNumber") = /*m_tool->*/getRandomRunNumber( eventInfo, mu_dependent );
   eventInfo.auxdecor<unsigned int>(m_prefix+"RandomLumiBlockNumber") = (eventInfo.auxdecor<unsigned int>(m_prefix+"RandomRunNumber")==0) ? 0 : /*m_tool->*/GetRandomLumiBlockNumber(  eventInfo.auxdecor<unsigned int>(m_prefix+"RandomRunNumber")  );

   eventInfo.auxdecor<ULong64_t>(m_prefix+"PRWHash") = getPRWHash( eventInfo );

   ATH_MSG_VERBOSE("PileupWeight = " << eventInfo.auxdecor<double>(m_prefix+"PileupWeight") << " RandomRunNumber = " << eventInfo.auxdecor<unsigned int>(m_prefix+"RandomRunNumber") << " RandomLumiBlockNumber = " << eventInfo.auxdecor<unsigned int>(m_prefix+"RandomLumiBlockNumber"));

   return StatusCode::SUCCESS;
}

float PileupReweightingTool::getDataWeight(const xAOD::EventInfo& eventInfo, const TString& trigger, bool mu_dependent) {

   if(eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION)) {
      ATH_MSG_WARNING("Requesting Data Weight is not intended for simulated events. Returning 1.");
      return 1; //no data weights for simulated events
   }

   if(!mu_dependent) return /*m_tool->*/m_activeTool->GetDataWeight(eventInfo.runNumber(), trigger);
   return /*m_tool->*/m_activeTool->GetDataWeight( eventInfo.runNumber(), trigger, getLumiBlockMu(eventInfo) /*use the 'correct' mu instead of the one from the file!!*/ );

}


} // namespace CP
