/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 *   */

#include "JetTagCalibration/JetTagCalibCondAlg.h"

//#include "JetTagTools/HistoHelperRoot.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PoolSvc/IPoolSvc.h"
#include "FileCatalog/IFileCatalog.h"
#include "TH1.h"
#include "TH2.h"
#include <TObjString.h>
#include "TObject.h"
#include <TString.h>
#include "TTree.h"

namespace Analysis {

  /**
     @ class JetTagCalibCondAlg

     @author emmanuel.le.guirriec@cern.ch
  */


  JetTagCalibCondAlg::JetTagCalibCondAlg (const std::string& name, ISvcLocator* pSvcLocator)
    : ::AthAlgorithm( name, pSvcLocator )
    , m_condSvc("CondSvc", name) ,
    m_Likelihood_smoothNTimes(1) ,
    m_JetFitterNN_calibrationDirectory("JetFitter"),
    m_JetFitterNN_calibrationSubDirectory("NeuralNetwork"),
    m_JetFitterNN_useCombinedIPNN(false),
    m_JetFitterNN_maximumRegisteredLayers(4)
    {
      declareProperty("taggers", m_taggers);
      declareProperty("channels", m_originalChannels);
      declareProperty("channelAliases", m_channelAliases);
      declareProperty("IPTag_UseCHypo"            , m_IPTag_useCHypo = true);
      declareProperty("IP2D_TrackGradePartitions", m_IP2D_trackGradePartitions);
      declareProperty("SV_useDRJPV", m_useDRJPVSV=true);
      declareProperty("SV2_usePt", m_usePtSV2=false);
      declareProperty("Likelihood_smoothNTimes",m_Likelihood_smoothNTimes);
      declareProperty("Likelihood_vetoSmoothingOf",  m_Likelihood_vetoSmoothingOf);
      declareProperty("JetFitterNN_CalibrationDirectory",m_JetFitterNN_calibrationDirectory);
      declareProperty("JetFitterNN_CalibrationSubDirectory",m_JetFitterNN_calibrationSubDirectory);
      declareProperty("JetFitterNN_useCombinedIPNN",m_JetFitterNN_useCombinedIPNN);
      declareProperty("JetFitterNN_maximumRegisteredLayers",m_JetFitterNN_maximumRegisteredLayers);
      declareProperty("RNNIP_NetworkConfig", m_RNNIP_network_cfg);
      m_Likelihood_vetoSmoothingOf.push_back("/N2T");
      m_Likelihood_vetoSmoothingOf.push_back("/N2TEffSV2");
      m_Likelihood_vetoSmoothingOf.push_back("/Sip3D");
    }
     
  JetTagCalibCondAlg::~JetTagCalibCondAlg()
    {
    }

  StatusCode JetTagCalibCondAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());

    // CondSvc   
    ATH_CHECK( m_condSvc.retrieve() ); 

    // PoolSvc
    ATH_CHECK(service("PoolSvc",m_poolsvc));
  
    // Condition Handles
    ATH_CHECK( m_readKey.initialize() );
    ATH_CHECK( m_writeKey.initialize() );

    // Register write handle
    if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_ERROR("#BTAG# Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    // Prepare histo maps:
    if (m_taggers.size() > 0) {
      ATH_MSG_DEBUG( "#BTAG# List of calibrated taggers");
      for(uint i=0;i<m_taggers.size();i++) {
        ATH_MSG_DEBUG("#BTAG# Tagger " << m_taggers[i] );
        std::vector<std::string> hvect;
        m_taggersHists.push_back(hvect);
      }
    }
    else {
      ATH_MSG_ERROR( "#BTAG# No taggers defined");
    }

    // List of channels:
    ATH_MSG_DEBUG( "#BTAG# Original channels " );
    
    for(uint i=0;i<m_originalChannels.size();i++) {
      ATH_MSG_DEBUG("#BTAG# Channel " << m_originalChannels[i] );
    }

    // Decode channel aliases
    if (m_channelAliases.size() > 0) {
      for(std::vector<std::string>::const_iterator aliasI = m_channelAliases.value().begin(),
          aliasE = m_channelAliases.value().end();
          aliasI != aliasE; aliasI++) {
        const std::string::size_type delim = aliasI->find("->");
        if(delim == std::string::npos) {
          ATH_MSG_ERROR( "#BTAG# Unexpected format in channelAliases: " << (*aliasI));
        } else {
          ATH_MSG_DEBUG( "#BTAG# Calibration channel alias: " << aliasI->substr(0, delim) << " -> "
                      << aliasI->substr(delim+2) );
          std::string jetc= aliasI->substr(0, delim);
          std::vector<std::string> jeta = tokenize(aliasI->substr(delim+2), ",");
          m_channelAliasesMultiMap.insert(std::make_pair(jetc, jeta) );
          // Add to list of channels to which aliases will be attached
          // (necessary because getJetAuthor used in taggers does not use
          //  jet collection name but standardised info)
          if (std::find(m_originalChannels.begin(), m_originalChannels.end(),jetc)
                       == m_originalChannels.end()) m_originalChannels.push_back(jetc);
        }
      }
    }
    else {
      ATH_MSG_ERROR( "#BTAG# No channel aliases defined");
    }

    m_directoryMap.clear();

    //IP2D tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "IP2D") != m_taggers.end()) {
      initializeIP2D();
    }

    //IP3D tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "IP3D") != m_taggers.end()) {
      initializeIP3D();
    }

    //SV1 tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "SV1") != m_taggers.end()) {
      initializeSV1();
    }

    //SV2 tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "SV2") != m_taggers.end()) {
      initializeSV2();
    }

    //JetFitterNN tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "JetFitterNN") != m_taggers.end()) {
      initializeJetFitterNN();
    }


    //SoftMu tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "SoftMu") != m_taggers.end()) {
      initializeSoftMu();
    }

    //MV2c10 tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MV2c10") != m_taggers.end()) {
      initializeMV2("MV2c10");
    }

    //MV2cl100 tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MV2cl100") != m_taggers.end()) {
      initializeMV2("MV2cl100");
    }

    //MV2c100 tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MV2c100") != m_taggers.end()) {
      initializeMV2("MV2c100");
    }

    //MV2c10mu tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MV2c10mu") != m_taggers.end()) {
      initializeMV2("MV2c10mu");
    }

    //MV2c10rnn tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MV2c10rnn") != m_taggers.end()) {
      initializeMV2("MV2c10rnn");
    }
   
    //RNNIP tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "RNNIP") != m_taggers.end()) {
      initializeRNNIP();
    }

    //JetVertexCharge tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "JetVertexCharge") != m_taggers.end()) {
      initializeJetVertexCharge();
    }

    //MultiSV tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MultiSVbb1") != m_taggers.end()) {
      initializeMultiSV("MultiSVbb1");
    }

    //MultiSV tagger
    if (std::find(m_taggers.begin(), m_taggers.end(), "MultiSVbb2") != m_taggers.end()) {
      initializeMultiSV("MultiSVbb2");
    }

    //Dl1 taggers
    if (std::find(m_taggers.begin(), m_taggers.end(), "DL1") != m_taggers.end()) {
      initializeDL1("DL1");
      initializeDL1("DL1mu");
      initializeDL1("DL1rnn");
    }


    return StatusCode::SUCCESS;
  }

 
  void JetTagCalibCondAlg::initializeIPTag() {
    ATH_MSG_DEBUG("initialize IPTag paths of the calibration file");
     m_IPTag_hypotheses.push_back("B");
     m_IPTag_hypotheses.push_back("U");
     if(m_IPTag_useCHypo){
       m_IPTag_hypotheses.push_back("C");
     }
  }
 
  void JetTagCalibCondAlg::initializeIP2D() {
    ATH_MSG_DEBUG("initialize IP2D paths of the calibration file");

    this->initializeIPTag();

    //check that grades are defined
       
    //check that hypotheses for likelihood tool are defined
    std::string hName;
    for(unsigned int i=0;i<m_IP2D_trackGradePartitions.size();i++) {
      for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
        hName = m_IPTag_hypotheses[ih]+"/"+m_IP2D_trackGradePartitions[i]+"/SipA0";
        registerHistogram("IP2D", hName);          
      }
    }
  }
 
 
  void JetTagCalibCondAlg::initializeIP3D() {
    ATH_MSG_DEBUG("initialize IP3D paths of the calibration file");

    this->initializeIPTag();

    
    //check that hypotheses for likelihood tool are defined
    std::string hName;
    for(unsigned int i=0;i<m_IP2D_trackGradePartitions.size();i++) {
      for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
        hName = m_IPTag_hypotheses[ih]+"/"+m_IP2D_trackGradePartitions[i]+"/Sip3D";
        registerHistogram("IP3D", hName);          
      }
    }
  }

  void JetTagCalibCondAlg::initializeSVEff(std::string SVmode) {
    // for SV efficiencies, add a few histograms:
    std::string hName;
    for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
	hName = m_IPTag_hypotheses[ih]+"/N2TEff"+SVmode;
        registerHistogram(SVmode, hName);
	hName = m_IPTag_hypotheses[ih]+"/N2TNorm"+SVmode;
        registerHistogram(SVmode, hName);
    }
  }

  
  void JetTagCalibCondAlg::initializeSV1() {
    ATH_MSG_DEBUG("initialize SV1 paths of the calibration file");

    this->initializeIPTag();

    std::string hName;
    for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
      hName = m_IPTag_hypotheses[ih]+"/N2T";
      registerHistogram("SV1", hName);
      hName = m_IPTag_hypotheses[ih]+"/BidimME";
      registerHistogram("SV1", hName);
        if(m_useDRJPVSV) {
          hName = m_IPTag_hypotheses[ih]+"/DRJPVSV";
          registerHistogram("SV1", hName);
      }
    }

    this->initializeSVEff("SV1");
  }

  void JetTagCalibCondAlg::initializeSV2() {
    ATH_MSG_DEBUG("initialize SV2 paths of the calibration file");

    this->initializeIPTag();

    std::string hName;
    if(m_usePtSV2){
      for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
        hName = m_IPTag_hypotheses[ih]+"/TridimMENPt";
        registerHistogram("SV2", hName);
        hName = m_IPTag_hypotheses[ih]+"/N2TEffSV2";
        registerHistogram("SV2", hName);
       }
    }else{
      for(unsigned int ih=0;ih<m_IPTag_hypotheses.size();ih++) {
        hName = m_IPTag_hypotheses[ih]+"/TridimMEN2T";
        registerHistogram("SV2", hName);
      }
    }

    this->initializeSVEff("SV2");
  }

  
  void JetTagCalibCondAlg::initializeJetFitterNN()
  {
    
    TString directory(m_JetFitterNN_calibrationSubDirectory);
    directory+="/";
    if (m_JetFitterNN_useCombinedIPNN)
    {
      directory+="comb";
    }
    else
    {
      directory+="standalone";
    }
    directory+="/";
    
    this->registerHistogram("JetFitterNN", m_JetFitterNN_calibrationDirectory,
                                         std::string((const char*)(directory+"LayersInfo")));
    
    Int_t nHidden=m_JetFitterNN_maximumRegisteredLayers-2;
    
    for (Int_t i=0;i<nHidden+1;++i)
    {
      
      TString weightName("Layer");
      weightName+=i;
      weightName+="_weights";
      
      TString thresholdName("Layer");
      thresholdName+=i;
      thresholdName+="_thresholds";
      
      this->registerHistogram("JetFitterNN", m_JetFitterNN_calibrationDirectory,
                                           std::string((const char*)(directory+weightName)));
      
      this->registerHistogram("JetFitterNN", m_JetFitterNN_calibrationDirectory,
                                           std::string((const char*)(directory+thresholdName)));
    }
    ATH_MSG_DEBUG(" Registered NN histograms with directory: " << m_JetFitterNN_calibrationDirectory << " and subdirectory " << directory);

  }


  void JetTagCalibCondAlg::initializeSoftMu()
  {
    std::string taggerNameBase("SMT");
    std::string treeName("BDT");
    std::string varStrName("variables");
    ATH_MSG_DEBUG("#BTAG# Folder I look into: "<< taggerNameBase+"Calib/"+treeName);
    this->registerHistogram("SoftMu", taggerNameBase, taggerNameBase+"Calib/"+treeName);
    this->registerHistogram("SoftMu", taggerNameBase, taggerNameBase+"Calib/"+varStrName);
  } 


  void JetTagCalibCondAlg::initializeMV2(std::string taggerNameBase)
  {
    std::string treeName("BDT");
    std::string varStrName("variables");
    ATH_MSG_DEBUG("#BTAG# Folder I look into: "<< taggerNameBase+"Calib/"+treeName);

    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib/"+treeName);
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib/"+varStrName);
  }

  void JetTagCalibCondAlg::initializeRNNIP()
  {
    for (const auto& rnn_name_pair: m_RNNIP_network_cfg) {
      if (rnn_name_pair.second.size() == 0) {
        ATH_MSG_VERBOSE("registering RNN " << rnn_name_pair.first);
        this->registerHistogram("RNNIP",rnn_name_pair.first);
        ATH_MSG_DEBUG(" #BTAG# Registered NN histograms with directory: " <<
		  "RNNIP");
      }
    }
  }

  
  void JetTagCalibCondAlg::initializeJetVertexCharge()
  {
    std::string taggerNameBase = "JetVertexCharge";
    //MVA xml files
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib_cat_JC_SVC_noMu");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib_cat_JC_SVC_incMu");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib_cat_JC_SVC_TVC_noMu");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib_cat_JC_SVC_TVC_incMu");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib_cat_JC_incMu");

    //reference histos
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_noMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_noMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_incMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_incMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_TVC_noMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_TVC_noMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_TVC_incMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_SVC_TVC_incMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_incMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_incMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_noMu_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_noMu_bbar");
    this->registerHistogram(taggerNameBase, "jvc_SVC_b");
    this->registerHistogram(taggerNameBase, "jvc_SVC_bbar");
    this->registerHistogram(taggerNameBase, "jvc_JC_all_b");
    this->registerHistogram(taggerNameBase, "jvc_JC_all_bbar");
  }

  void JetTagCalibCondAlg::initializeMultiSV(std::string taggerNameBase)
  {
    ATH_MSG_DEBUG("#BTAG# taggerNameBase " << taggerNameBase);
    std::string treeName = "BDT";
    std::string varStrName = "variables";
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib");
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib/"+treeName);
    this->registerHistogram(taggerNameBase, taggerNameBase+"Calib/"+varStrName);
  }

  void JetTagCalibCondAlg::initializeDL1(std::string taggerNameBase)
  {
    m_DL1_file_name = "net_configuration"; // directory of NN calibration (starting from specific jet collection directory) in COOL db
    this->registerHistogram(taggerNameBase, m_DL1_file_name);  //register the calibration file for later access

    ATH_MSG_DEBUG(" #BTAG# Registered NN histograms with directory: " << taggerNameBase);
  }

  void JetTagCalibCondAlg::registerHistogram(const std::string& tagger, const std::string& hname) {
    std::string dir(tagger);
    this->registerHistogram(tagger,dir,hname);
  }

  void JetTagCalibCondAlg::registerHistogram(const std::string& tagger, const std::string& directory, const std::string& hname) {
    ATH_MSG_DEBUG(  "#BTAG# registering histogram " << hname << " in tagger " << tagger );
    m_directoryMap[tagger] = directory;
    bool registered = false;
    for(uint i=0;i<m_taggers.size();i++) {
      if(tagger==m_taggers[i]) {
        ATH_MSG_DEBUG( "#BTAG# tagger " << tagger << " found in pos " << i
                       << " , registrating " << hname );

        m_taggersHists[i].push_back(hname);
        registered = true;
      }
    }
    if(!registered) {
      ATH_MSG_DEBUG( "#BTAG# tagger " << tagger << " not found."
                     << " Registrating of " << hname << " not possible.");
    }
  }


  StatusCode JetTagCalibCondAlg::execute() {
    ATH_MSG_DEBUG("execute " << name());

    // Write Cond Handle
    SG::WriteCondHandle<JetTagCalibCondData > histoWriteHandle{m_writeKey};
    //For serial Athena. Execute() should not be called in AthenaMT
    if (histoWriteHandle.isValid()) {
      ATH_MSG_DEBUG("#BTAG# Write CondHandle "<< histoWriteHandle.fullKey() << " is already valid");
      return StatusCode::SUCCESS;
    }

    m_mappedAlias.clear();

    // Read Cond Handle - GUID
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
    const CondAttrListCollection* atrcol{*readHandle};
    if(atrcol==nullptr) {
      ATH_MSG_ERROR("#BTAG# Cannot retrieve CondAttrListCollection for " << m_readKey.key());
      return StatusCode::FAILURE;
    }


    // Construct the output Cond Object and fill it in
    std::unique_ptr<JetTagCalibCondData> writeCdo{std::make_unique<JetTagCalibCondData>()};
    writeCdo->resize(m_taggers);

    // Define validity of the output cond object and record it
    EventIDRange rangeW;
    if(!readHandle.range(rangeW)) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }


    unsigned int channel=1; //Always 1 in old version with CoolHistSvc
    CondAttrListCollection::const_iterator citr = atrcol->chanAttrListPair(channel);
    if (citr==atrcol->end()) {
      ATH_MSG_WARNING("#BTAG# Cannot find valid reference for " << readHandle.key() << " channel " << channel);
      return StatusCode::FAILURE;
    }

    const std::string coolguid=citr->second["fileGUID"].data<std::string>();
    ATH_MSG_DEBUG("#BTAG# Folder key "+ readHandle.key()+" has current file GUID "+coolguid);

    // Open the file
    std::string pfname, tech;
    m_poolsvc->catalog()->getFirstPFN(coolguid, pfname, tech );
    TFile* pfile = TFile::Open(pfname.c_str(),"READ");
    if (pfile==nullptr || !pfile->IsOpen()) {
      delete pfile;
      ATH_MSG_WARNING("Problems opening input file "+pfname);
      return StatusCode::FAILURE;
    }

    StatusCode sc = createHistoMap(pfile, writeCdo.get());
    if(sc != StatusCode::SUCCESS){
    // do nothing for the moment
    }

    for(uint i=0;i<m_taggers.size();i++) {
      std::string tagger = m_taggers[i];

      ATH_MSG_DEBUG( "#BTAG# registrating tagger "<< m_taggers[i]);
      std::vector<std::string> histnames = m_taggersHists[i];
      for(unsigned int h=0; h<histnames.size(); ++h){
	std::string hname = histnames[h];
	for(uint j=0;j<m_mappedAlias.size();j++) {
          std::string fname = writeCdo->fullHistoName(m_mappedAlias[j],hname);
          ATH_MSG_DEBUG( "#BTAG# Retrieving " << tagger <<":"<< fname );
          std::string channel = writeCdo->channelName(fname);
          std::string hname = writeCdo->histoName(fname);
          std::string hFullName(m_directoryMap[tagger]);
          hFullName+="/"; hFullName+=channel; 
          hFullName+="/"; hFullName+=hname;
          ATH_MSG_DEBUG( "#BTAG#     histo name in physical file= " << hFullName );
          TObject* hPointer = nullptr;
          if (getTObject(hFullName, pfile, hPointer)) {
            if(hPointer) {
              ATH_MSG_DEBUG( "#BTAG# Cached pointer to histogram or string: " << hPointer);
              if (tagger.find("DL1")!=std::string::npos ) {
                ATH_MSG_DEBUG("#BTAG# Build DL1 NN config for tagger " << tagger << " and jet collection " << channel << " and write it in condition data");
                TObjString* cal_string = dynamic_cast<TObjString*>(hPointer);
                std::istringstream nn_config_sstream(cal_string->GetString().Data());
                lwt::JSONConfig nn_config = lwt::parse_json(nn_config_sstream);
                ATH_MSG_DEBUG("#BTAG# Layers size << " << nn_config.layers.size());

                writeCdo->addDL1NN(tagger, channel, nn_config);
              }
              else {
                if (tagger == "IP2D" || tagger == "IP3D" || tagger == "SV1") {
                  ATH_MSG_VERBOSE("#BTAG# Smoothing histogram " << hname << " ...");
                  smoothAndNormalizeHistogram(hPointer, hname);
                }

                writeCdo->addHisto(i,fname,hPointer);
              }

              const TString rootClassName=hPointer->ClassName();
              if (rootClassName=="TTree") {
                ((TTree*)hPointer)->LoadBaskets();
                ((TTree*)hPointer)->SetDirectory(0);
              } 
            
            } else {
              ATH_MSG_ERROR( "#BTAG# Could not cache pointer to histogram " << fname );
            }
          } else {
            ATH_MSG_WARNING("#BTAG# error: histogram "<<hFullName
                          <<" does not exist - you are probably using an old database tag");
          }

        } //end loop mapped alias
      } //end loop histograms
    } //end loop tagger

    if(histoWriteHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
      ATH_MSG_ERROR("#BTAG# Could not record vector of histograms maps " << histoWriteHandle.key()
         		  << " with EventRange " << rangeW
	 		  << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << histoWriteHandle.key() << " with range " << rangeW << " into Conditions Store");
              
    // close the file
    pfile->Close();
    delete pfile;

    return StatusCode::SUCCESS;
  }
 
  StatusCode JetTagCalibCondAlg::createHistoMap(TFile* pfile, JetTagCalibCondData * histosCdo){

    ATH_MSG_DEBUG("#BTAG# in createHistoMap" );
    std::vector< std::string > channels;

    for(unsigned int j=0; j<m_originalChannels.size(); ++j){
      channels.push_back(m_originalChannels[j]);
    }

    std::string folder(m_readKey.key());
    

    for(uint i=0;i<m_taggers.size();++i) {
      std::string tagger = m_taggers[i];

      for(unsigned int j=0; j<m_originalChannels.size(); ++j){
        /// get all aliases
        std::map<std::string, std::vector<std::string> >::iterator ialiaslist 
          = m_channelAliasesMultiMap.find(m_originalChannels[j]);
	if(ialiaslist == m_channelAliasesMultiMap.end()){
	  ATH_MSG_DEBUG( "#BTAG#  no alias for original channel" << m_originalChannels[j] );
	  if(!objectTDirExists(tagger+"/"+m_originalChannels[j], pfile)){
	    ATH_MSG_WARNING( "#BTAG# no calibration for jet collection " << m_originalChannels[j]
			   << " consider using aliases " );
	  }
	  continue;
	}
	std::vector<std::string> aliaslist = ialiaslist->second;
	if(aliaslist.size() == 1){
	  if("none" == aliaslist[0]){
	    ATH_MSG_DEBUG("#BTAG#  no alias for original channel" << m_originalChannels[j]);
				
            if(objectTDirExists(tagger+"/"+m_originalChannels[j], pfile)){
	      ATH_MSG_WARNING( "#BTAG# no calibration for jet collection " << m_originalChannels[j]
				     << " consider using aliases " );
            }
            continue;
	  }
	}

	bool foundalias=false;

	for(unsigned int k=0; k<aliaslist.size(); ++k){

	  std::string aliasentry = aliaslist[k];
	  if("none" == aliasentry){
	    ATH_MSG_DEBUG("#BTAG# first alias entry is none - replace with original channel" 
				<< m_originalChannels[j] );
            aliasentry= m_originalChannels[j];
	  }
	  /// now see if the jet collection exists in db
	  std::string hFullName(tagger); 
	  hFullName+="/"; hFullName+=aliasentry; 
          // Check if jet collection already in channel alias map
          if (std::count(m_mappedAlias.begin(), m_mappedAlias.end(), aliasentry) > 0) {
	    ATH_MSG_DEBUG( "#BTAG# found alias entry in Map " << aliasentry );
            histosCdo->addChannelAlias(m_originalChannels[j],aliasentry);
            foundalias=true;
            break;
          }
          else {
            if (objectTDirExists(hFullName, pfile)) {
	      ATH_MSG_DEBUG( "#BTAG# found alias entry in DB " << aliasentry );
              if("none"!=aliaslist[k]){
		std::vector<std::string>::const_iterator pos = find(channels.begin(), 
								channels.end(), aliasentry);
		if(pos==channels.end()) {
		  ATH_MSG_DEBUG("#BTAG# Alias is pointing to undefined channel: " <<  aliasentry
				    << ". Adding it to channel list.");
		  channels.push_back(aliasentry);
	        }
                histosCdo->addChannelAlias(m_originalChannels[j],aliasentry);
                m_mappedAlias.push_back(aliasentry);
	      }
	      foundalias=true;
	      break;
	    }
	    else{
	      ATH_MSG_DEBUG( "#BTAG# no alias entry " << aliasentry 
		       << " trying next alias ");
	    }
	  }
	}
	if(!foundalias){
	  ATH_MSG_WARNING( "#BTAG# none of the aliases exist for jet collection " 
			 << m_originalChannels[j]);
	}

      }
      break ; /// check alias for the first tagger. same jet collections for all taggers for now

    }

    ATH_MSG_DEBUG( "#BTAG# final registered channels " );
    for(uint i=0;i<channels.size();++i) {
      ATH_MSG_DEBUG( "#BTAG# Channel " << channels[i] );
    }

    //print alias map
    histosCdo->printAliasesStatus();

    return StatusCode::SUCCESS;

  }



  std::vector<std::string> JetTagCalibCondAlg::tokenize(std::string str, std::string delim){
    std::vector<std::string> tokens;
    std::string::size_type sPos, sEnd, sLen;
    // if str starts with a character in delim, do you want an empty string in tokens?
    // sPos = 0; // if answer is yes
    sPos = str.find_first_not_of(delim);  // if answer is no
    while(sPos != std::string::npos){
      sEnd = str.find_first_of(delim, sPos);
      if(sEnd == std::string::npos) sEnd = str.length();
      sLen = sEnd - sPos;
      std::string token = str.substr(sPos, sLen);
      tokens.push_back(token);
      sPos = str.find_first_not_of(delim, sEnd);
    }
    return tokens;
  }

  StatusCode JetTagCalibCondAlg::finalize() {
    ATH_MSG_DEBUG("finalize " << name());
    return StatusCode::SUCCESS;
  }

  StatusCode JetTagCalibCondAlg::objectTDirExists(const std::string& histname, TFile * pfile) const {
    
    ATH_MSG_DEBUG("#BTAG# in objectTDirExists" );

     // now read the histogram into memory
     ATH_MSG_DEBUG("Getting object "+histname+" from file");
     TObject* hist = nullptr;
     pfile->GetObject(histname.c_str(),hist);
     if (hist==nullptr) {
       ATH_MSG_DEBUG("#BTAG# Could not load TObject " << histname);
       return StatusCode::FAILURE;
     }

     return StatusCode::SUCCESS;
  }

  StatusCode JetTagCalibCondAlg::getTObject(const std::string& histname, TFile * pfile, TObject*& hist) const {
     // now read the histogram into memor
     ATH_MSG_DEBUG("Getting object "+histname+" from file");
     std::unique_ptr<TObject> hist_raw(pfile->Get(histname.c_str()));
     hist = dynamic_cast<TObject *>(hist_raw.get());
     hist_raw.release();
     if (hist==nullptr) {
       ATH_MSG_DEBUG("#BTAG# Could not load TObject " << histname);
       return StatusCode::FAILURE;
     }
     else {
       // make this histogram unassociated with the TFile, so file can be closed
       // only for histogram objects, others do not get associated
       // TTrees have special treatment 
       TH1* ihist=dynamic_cast<TH1*>(hist);
       if (ihist!=nullptr) ihist->SetDirectory(0);
       // if it is a TDirectory, also need special treatment to unassociate parent
       TDirectory* idir=dynamic_cast<TDirectory*>(hist);
       if (idir!=nullptr) {
         TDirectory* mdir=idir->GetMotherDir();
         if (mdir!=nullptr) {
           ATH_MSG_DEBUG("Disconnecting TDirectory "+histname+" from parent");
           mdir->GetList()->Remove(idir);
           idir->SetMother(0);
         } else {
           ATH_MSG_WARNING("Could not get MotherDir for TDirectory "+histname);
         }
       }
     }

     return StatusCode::SUCCESS;
   }
  
  void JetTagCalibCondAlg::smoothAndNormalizeHistogram(TObject*& hist, const std::string& hname) {
    //Select small part of NewLikelihoodTool to reproduce CalibrationBroker behaviour (smooth and normalize histogram)
    TH1* h = dynamic_cast<TH1*>(hist);
    if(h) {
      double norm = h->Integral();
      if(norm) {
        // check if smoothing of histogram is not vetoed:
        bool veto = false;
        for(unsigned int iv=0; iv<m_Likelihood_vetoSmoothingOf.size(); iv++) {
          if(hname.find(m_Likelihood_vetoSmoothingOf[iv])!=std::string::npos) {
            veto = true;
            ATH_MSG_VERBOSE("#BTAG# Smoothing of " << hname << " is vetoed !");
            break;
          }
        }
        if(1==h->GetDimension() && m_Likelihood_smoothNTimes) {
          if(!veto) {
            if(norm>10000)h->Smooth(m_Likelihood_smoothNTimes);
            else h->Smooth((int)(m_Likelihood_smoothNTimes*100./sqrt(norm)));
          }
        }
        if(2==h->GetDimension()) {
          int m2d=3;
          if(!veto) {
	    TH2 * dc_tmp = dynamic_cast<TH2*>(h);
	    if (dc_tmp) {
              this->smoothASH2D(dc_tmp, m2d, m2d);
            }
          }
        }
        if(3==h->GetDimension()) {
          ATH_MSG_WARNING("#BTAG# Code needs to be migrated from NewLikelihoodTool");
        }
        // normalize:
        norm = h->Integral();
        h->Scale(1./norm);
      } else {
        ATH_MSG_DEBUG("#BTAG# Histo "<<h<<" is empty!");
      }
    }
  }

void JetTagCalibCondAlg::smoothASH2D(TH2* input2D, int m1, int m2) {

  ATH_MSG_DEBUG("Smoothing a two dimensional histogram "<< input2D->GetName()
              << " " << m1 << " " << m2);
  ATH_MSG_DEBUG("First (1-3, 1-3) 3x3 bins before smoothing: ");
  for(int i=1;i<4;i++) {
    for(int j=1;j<4;j++) {
      ATH_MSG_DEBUG(i<<" "<<j<<" : "<<input2D->GetBinContent(i,j)<< " / ");
    }
  }
  int ioffset = input2D->GetNbinsX() / 2 ;
  int joffset = input2D->GetNbinsY() / 2 ;
  ATH_MSG_DEBUG("Middle (" << ioffset+1 << "-" << ioffset+4 << ", ("
              << joffset+1 << "-" << joffset+4 << ") 3x3 bins before smoothing: ");
  for(int i=1;i<4;i++) {
    for(int j=1;j<4;j++) {
      ATH_MSG_DEBUG(i<<" "<<j<<" : "<<input2D->GetBinContent(i+ioffset,j+joffset)<< " / ");
    }
  }

  //
  const int lsup = 20;
  if (m1 > lsup || m2 > lsup) {
    ATH_MSG_DEBUG("HistoHelperRoot::smoothASH2D: m1 or m2 too big !");
    return;
  } else {
    int nx = input2D->GetNbinsX()+1;
    int ny = input2D->GetNbinsY()+1;
    float **h, **res;
    h   = new float*[nx-1];
    res = new float*[nx-1];
    for (int i = 0;i < nx-1;i++) {
      h[i]   = new float[ny-1];
      res[i] = new float[ny-1];
    }
    for (int iy = 1;iy<ny;iy++) {
      for (int ix = 1;ix<nx;ix++) {
        h[ix-1][iy-1] = (float) input2D->GetBinContent(ix,iy);
      }
    }
    //
    int i,j,k,l;
    float wk1[41],wk2[41],wgt[100][100];
    double wk[41][41],wks = 0.;
    float ai,am1 = float(m1), am2 = float(m2);
    const float am12 = am1*am1, am22 = am2*am2;
    const float inv_am1_am2 = 1. / (am1 * am2);
    const float inv_am12 = 1. / am12;
    const float inv_am22 = 1. / am22;
    // Initialisation
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        res[k][l] = 0.; wgt[k][l] = 0.;
      }
    }
    // Weights
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk1[i] = 15./16.*(1.-ai*inv_am12)*(1.-ai*inv_am12);
      wks = wks + wk1[i];
    }
    const double fac1 = am1 / wks;
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      wk1[i] =  wk1[i]*fac1;
    }
    wks = 0.;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk2[i] = 15./16.*(1.-ai*inv_am22)*(1.-ai*inv_am22);
      wks = wks + wk2[i];
    }
    const double fac2 = am2 / wks;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      wk2[i] =  wk2[i]*fac2;
    }
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      for (j = lsup+1-m2;j<lsup+m2;j++) {
        wk[i][j] = wk1[i]*wk2[j];
      }
    }
    //
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        for (i = std::max(0,k-m1+1);i<std::min(nx-1,k+m1);i++) {
          for (j = std::max(0,l-m2+1);j<std::min(ny-1,l+m2);j++) {
            res[i][j] = res[i][j] + wk[lsup+i-k][lsup+j-l]*h[k][l];
            wgt[i][j] = wgt[i][j] + wk[lsup+i-k][lsup+j-l];
          }
        }
      }
    }
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        res[k][l] = res[k][l]*inv_am1_am2;
        if (wgt[k][l] != 0.) {res[k][l] = res[k][l]/wgt[k][l];}
      }
    }
    // replace the histo content with the result of smoothing:
    input2D->Reset();
    for (int iy = 1;iy<ny;iy++) {
      for (int ix = 1;ix<nx;ix++) {
        input2D->SetBinContent(ix,iy,res[ix-1][iy-1]);
      }
    }
    for (i = 0; i < nx-1; i++){
      delete[] h[i];
      delete[] res[i];
    }
    delete[] h;
    delete[] res;
  }
  ATH_MSG_DEBUG("First (1-3, 1-3) 3x3 bins after smoothing: ");
  for(int i=1;i<4;i++) {
    for(int j=1;j<4;j++) {
      ATH_MSG_DEBUG(i<<" "<<j<<" : "<<input2D->GetBinContent(i,j)<< " / ");
    }
  }
  ioffset = input2D->GetNbinsX() / 2 ;
  joffset = input2D->GetNbinsY() / 2 ;
  ATH_MSG_DEBUG("Middle (" << ioffset+1 << "-" << ioffset+4 << ", ("
              << joffset+1 << "-" << joffset+4 << ") 3x3 bins after smoothing: ");
  for(int i=1;i<4;i++) {
    for(int j=1;j<4;j++) {
      ATH_MSG_DEBUG(i<<" "<<j<<" : "<<input2D->GetBinContent(i+ioffset,j+joffset)<< " / ");
    }
  }
}

}
