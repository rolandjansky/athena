/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ElectronChargeEfficiencyCorrectionTool
   @brief Apply the correction for the electrons charge mis-ID different rates in MC/data

   @author Giulia Gonella <giulia.gonella@cern.ch>
   @date   September 2015
*/
// Include this class's header
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
// xAOD includes
#include "PathResolver/PathResolver.h"
// #include "xAODTruth/TruthEventContainer.h"
// #include "xAODEgamma/EgammaTruthxAODHelpers.h"
// #include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Electron.h"

// ROOT includes
#include "TFile.h"
// #include "TRandom.h"

// STL includes
#include <stdlib.h>     /* atoi */


// =============================================================================
// Standard constructor
// =============================================================================
CP::ElectronChargeEfficiencyCorrectionTool::ElectronChargeEfficiencyCorrectionTool(std::string name) :
  AsgTool(name),
  m_dataTypeOverwrite(-1),
  m_eventInfoCollectionName("EventInfo"),
  m_SF_SS(),
  m_SF_OS(),
  m_RunNumbers(),
  m_useRandomRunNumber(true),
  m_defaultRandomRunNumber(999999),
  m_filename(""),
  m_workingPoint(""),
  m_eta_lowlimit(0.0),
  m_eta_uplimit(0.0),
  m_pt_lowlimit(0.0),
  m_pt_uplimit(0.0),
  m_gevmev(0.0),
  m_truthCharge(0),
  m_filtered_sys_sets(),
  m_mySysConf(),
  m_affectingSys(),
  m_appliedSystematics(0),
  m_sf_decoration_name("chargeIDEffiSF"),
  m_sfDec(0)
{
  // Declare the needed properties
  declareProperty("CorrectionFileName",        m_filename,     "Name of the file with charge flipping rates");
  declareProperty("WorkingPoint",              m_workingPoint, "Name of working point folder in the file");
  declareProperty("ScaleFactorDecorationName", m_sf_decoration_name);
  declareProperty("ForceDataType",             m_dataTypeOverwrite,
                  "Force the DataType of the electron to specified value (to circumvent problem of incorrect DataType for forward electrons in some old releases)");
  declareProperty("EventInfoCollectionName",   m_eventInfoCollectionName, "The EventInfo Collection Name");
  declareProperty("UseRandomRunNumber",        m_useRandomRunNumber);
  declareProperty("DefaultRandomRunNumber",    m_defaultRandomRunNumber);

}

// =============================================================================
// Standard destructor
// =============================================================================
CP::ElectronChargeEfficiencyCorrectionTool::~ElectronChargeEfficiencyCorrectionTool()
{
  if(m_sfDec) delete m_sfDec;
}


// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode CP::ElectronChargeEfficiencyCorrectionTool::initialize()
{
  ATH_MSG_DEBUG("initializing");

  // initialize the random number generator (used in case of charge flip approach)
  //m_Rndm = new TRandom3(1);

  if(m_sfDec) delete m_sfDec;
  m_sfDec = new SG::AuxElement::Decorator< float>(m_sf_decoration_name);//xxxx

  //Resolve the path to the input file for the charge flip rates
  const std::string rootfilename = PathResolverFindCalibFile(m_filename);
  if (m_filename.empty()) {
    ATH_MSG_ERROR ( " PathResolver was not able to find the file ... aborting" );
    return StatusCode::FAILURE;
  }

  // Getting the root file and histograms
  TFile* rootFile = TFile::Open( rootfilename.c_str() );

  // protection against bad file
  if ( rootFile==0 ) {
    ATH_MSG_ERROR ( " Was not able to open file: " <<  rootfilename << " ...... aborting" );
    return StatusCode::FAILURE;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // explanation: attempt to loop generally over a file
  // -- if certain SINGALWORD is present -- then this is taken as a signal,
  // that this is another dimension... can be dynamically added.
  // e.g.
  // SFSyst<number>_RunNumber<minRN>-<maxRN>_Nvtx<minNvtx>-<maxNvtx>
  // SFStat_RunNumber<minRN>-<maxRN>_Nvtx<minNvtx>-<maxNvtx>
  // SFCentral_RunNumber<minRN>-<maxRN>_Nvtx<minNvtx>-<maxNvtx>

  //     Then can create a key that will dynamically give us access to a map:
  //    std::map<std::string key, std::vector<TH2D *>> m_SF_SS;     // keys (e.g. RunNumber223333_319200_Nvtx0_10_Phi1.5_1.6) mapping to vector of SF histograms --> vector m_SF: 0=nominal, 1=stat, 2,3,4...n=syst
  //     std::map<std::string key, std::vector<TH2D *>> m_SF_OS;     // keys (e.g. RunNumber223333_319200_Nvtx0_10_Phi1.5_1.6) mapping to vector of SF histograms --> vector m_SF: 0=nominal, 1=stat, 2,3,4...n=syst
  // TFile*         data/ChMisIDSF_TightLL_FixedCutTight.root
  //  KEY: TH2F     SFCentral_RunNumber296939_311481_SS;1   SFCentral_RunNumber296939_311481_SS
  //  KEY: TH2F     SFCentral_RunNumber296939_311481_OS;1   SFCentral_RunNumber296939_311481_OS
  //  KEY: TH2F     STAT_RunNumber296939_311481_SS;1        STAT_RunNumber296939_311481_SS
  //  KEY: TH2F     STAT_RunNumber296939_311481_OS;1        STAT_RunNumber296939_311481_OS
  //  KEY: TH2F     SYST_RunNumber296939_311481_total_SS;1  SYST_RunNumber296939_311481_SS: total
  //  KEY: TH2F     SYST_RunNumber296939_311481_total_OS;1  SYST_RunNumber296939_311481_OS: total


  m_SF_SS.clear();
  m_SF_OS.clear();
  TList* keyListfolder = rootFile->GetListOfKeys();
  std::vector<std::string> names;

  for ( int j=0; j<keyListfolder->GetEntries(); j++ ){
    names.push_back(( keyListfolder->At(j)->GetName() ));
  }
  std::sort(names.begin(), names.end());

  for ( unsigned int j=0; j<names.size(); j++ ){

    std::string name = names.at(j);
    ATH_MSG_DEBUG("Got ROOT object with name: " << name);
    if ( name.find(Form("SFCentral_") ) != std::string::npos){
      ATH_MSG_VERBOSE("Found name 'SFCentral_' in ROOT object name");
      // Check for opposite-sign (=opposite-charge)
      bool isOS = false;
      if ( name.find(Form("_OS") ) != std::string::npos ){
        isOS = true;
        ATH_MSG_VERBOSE("Found name '_OS' in ROOT object name");
      }
      if (isOS){
        std::string histid = ( names.at(j) );
        histid.erase(0,10);
        histid.erase(histid.size()-3,3);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using histid: " << histid);

        if ( histid.find("RunNumber") != std::string::npos ){
          ATH_MSG_VERBOSE("Found name 'RunNumber' in histid");
          std::string runlow = histid;
          runlow.erase(histid.find(Form("RunNumber") ),9);
          runlow.erase(runlow.find("_"),runlow.size() );
          m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runlow.c_str())) );
          std::string runhigh = histid;
          runhigh.erase(histid.find(Form("RunNumber") ),9);
          runhigh.erase(0,runhigh.find("_")+1);
          m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runhigh.c_str())) );
        }
        ATH_MSG_VERBOSE("Using histid (OS hid): " << histid);
        m_SF_OS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }
      else {
        std::string histid = ( names.at(j) );
        histid.erase(0,10);
        histid.erase(histid.size()-3,3);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using histid (do we this in ? SS): " << histid);
        m_SF_SS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }
    }///// if ( name.find(Form("SFCentral_") ) != std::string::npos)


    /// STAT ERROR
    if ( name.find(Form("STAT_") ) != std::string::npos ){
      ATH_MSG_VERBOSE("Found name 'STAT_' in ROOT object name");
      bool isOS = false;
      if ( name.find(Form("_OS") ) != std::string::npos ){
        isOS = true;
        ATH_MSG_VERBOSE("Found name '_OS' in ROOT object name");
      }
      if ( isOS ){
        std::string histid = ( names.at(j) );
        histid.erase(0,5);
        histid.erase(histid.size()-3,3);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using histid: " << histid);

        if ( histid.find("RunNumber") != std::string::npos ){
          ATH_MSG_VERBOSE("Found name 'RunNumber' in histid");
          std::string runlow = histid;
          runlow.erase(histid.find(Form("RunNumber") ),9);
          runlow.erase(runlow.find("_"),runlow.size() );
	  //          m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runlow.c_str())) );
          std::string runhigh = histid;
          runhigh.erase(histid.find(Form("RunNumber") ),9);
          runhigh.erase(0,runhigh.find("_")+1);
	  //          m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runhigh.c_str())) );
        }
        ATH_MSG_VERBOSE("Using histid (OS hid): " << histid);
        m_SF_OS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }
      else {
        std::string histid = ( names.at(j) );
        ATH_MSG_VERBOSE("Found  histid: " << histid);
        histid.erase(0,5);
        histid.erase(histid.size()-3,3);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using histid (do we this in ? SS): " << histid);
        m_SF_SS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }

    }///// if ( name.find(Form("SYST") ) != std::string::npos)


    /// STAT ERROR
    if ( name.find(Form("SYST") ) != std::string::npos ){
      ATH_MSG_VERBOSE("Found name 'SYST' in ROOT object name");
      bool isOS = false;
      if ( name.find(Form("_OS") ) != std::string::npos ){
        isOS = true;
        ATH_MSG_VERBOSE("Found name '_OS' in ROOT object name");
      }
      if ( isOS ){
        std::string histid = ( names.at(j) );
        histid.erase(0,4);
        histid.erase(histid.size()-3,3);// remove _SS, _OS

        std::string sysname = histid;
        sysname.erase(sysname.find("_"),sysname.size());
        m_systematics.push_back(sysname);

        histid.erase(0,histid.find("_")+1);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using syst histid: " << histid);

        if (histid.find("RunNumber") != std::string::npos ) {
          std::string runlow = histid;
          runlow.erase(histid.find(Form("RunNumber") ),9);
          runlow.erase(runlow.find("_"),runlow.size() );
	  //        m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runlow.c_str())) );
          std::string runhigh = histid;
          runhigh.erase(histid.find(Form("RunNumber") ),9);
          runhigh.erase(0,runhigh.find("_")+1);
	  //      m_RunNumbers.push_back( static_cast<unsigned int>(atoi(runhigh.c_str())) );
        }
        ATH_MSG_VERBOSE("Using histid (OS hid): " << histid);
        m_SF_OS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }
      else {
        std::string histid = ( names.at(j) );
        histid.erase(0,4);
        histid.erase(histid.size()-3,3);// remove _SS, _OS
        histid.erase(0,histid.find("_")+1);// remove _SS, _OS
        ATH_MSG_VERBOSE("Using histid (sys ? SS): " << histid);
        m_SF_SS[histid].push_back( (TH2D*)rootFile->Get( names.at(j).c_str() ));
      }

    }///end // if ( name.find(Form("SYST") ) != std::string::npos)

  }

  /////////// checks ... --> same vector length... all files there?

  if ( m_SF_OS.size() <1 || m_SF_SS.size() <1 || m_SF_SS.size()!=m_SF_OS.size() ) {
    ATH_MSG_ERROR("OS/SS SF vectors not filled or of different size. -- Problem with files. -- Report to <hn-atlas-EGammaWG@cern.ch>");
    return CP::CorrectionCode::Error;
  }


  std::sort(m_RunNumbers.begin(),m_RunNumbers.end());
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //  rootFile->Close();

  // Determine the limits of validity

  /// here: need to use iterator over map!!!
  ATH_MSG_DEBUG("Having m_SF_OS.size() = " << m_SF_OS.size());
  std::map<std::string,std::vector<TH2D*> >::iterator it = m_SF_OS.begin();

  // Get the kinematic limits
  m_eta_lowlimit   = (*it).second.at(0)->GetYaxis()->GetXmin();
  m_eta_uplimit    = (*it).second.at(0)->GetYaxis()->GetXmax();
  ATH_MSG_VERBOSE("|eta| limits " << m_eta_lowlimit << ", " << m_eta_uplimit);

  m_pt_lowlimit =  (*it).second.at(0)->GetXaxis()->GetXmin();
  m_pt_uplimit  = (*it).second.at(0)->GetXaxis()->GetXmax();
  ATH_MSG_VERBOSE("pt limits " << m_pt_lowlimit << ", " << m_pt_uplimit);

  // Check if the input file is in GeV or MeV
  if ( m_pt_uplimit > 1500 ) {
    ATH_MSG_VERBOSE("Rates in input file are in MeV");
    m_gevmev = 1.;
  }
  else {
    ATH_MSG_VERBOSE("Rates in input file are in GeV");
    m_gevmev = 0.001;
  }

  // Systematics // dynamic too?
  m_affectingSys = affectingSystematics();


  return StatusCode::SUCCESS;
}


// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode CP::ElectronChargeEfficiencyCorrectionTool::finalize()
{
  return StatusCode::SUCCESS ;
}



//---------------------------------------------------------------------------------------
// Get the scale factor for the electron
//---------------------------------------------------------------------------------------

//

CP::CorrectionCode
CP::ElectronChargeEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::IParticle& part, double& sf) const {
  ATH_MSG_DEBUG("In CP::ElectronChargeEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::IParticle& part, double& sf) const");
  if ( part.type() != xAOD::Type::Electron ){
    ATH_MSG_ERROR("This function requires an electron to be passed. Failing!");
    return CP::CorrectionCode::Error;
  }
  const xAOD::Electron& ele = static_cast<const xAOD::Electron&>(part);

  // initialize the SF at 1
  sf = 1.0;

  // checking on the truth electron: up to now if this is not a good ele it's returning
  bool goodEle = false;
  CP::CorrectionCode goodEle_result = this->isGoodEle( ele, goodEle); //// ## Giulia: This is to change
  if ( goodEle_result != CP::CorrectionCode::Ok ) {
    sf = -999.0;
    ATH_MSG_DEBUG("This is the check of goodeleCC in getscalefactor. Scale factor set to -999");
    return goodEle_result;
  }

  if ( goodEle == false ) {
    // electron is background electron and should not be corrected
    return CP::CorrectionCode::Ok;
    ATH_MSG_DEBUG("Here goodele is false but CC ok");
  }

  // taking reconstructed variables
  int reco_ele_charge  = ele.charge();
  const double ele_pt  = ele.pt()*m_gevmev;
  const double ele_eta = std::abs( ele.caloCluster()->etaBE(2)   );

  // getting the truth charge
  int truth_ele_charge = 9999;
  CP::CorrectionCode charge_result = this->getEleTruthCharge( ele, truth_ele_charge); //// ## Giulia: second function to change
  if ( charge_result != CP::CorrectionCode::Ok ) {
    sf = -9999.0;
    ATH_MSG_VERBOSE("This is check of geteletruthchargeCC in getscalefactor. Scale factor set to -9999");
    return charge_result;
  }

  if ( truth_ele_charge == 0 ) {
    ATH_MSG_DEBUG("Here truth charge is =0!!");
    return CP::CorrectionCode::Ok;
  }

  ATH_MSG_DEBUG("Reco charge = " << reco_ele_charge << "; Truth charge = " << truth_ele_charge);

  // getting the rates from file....
  float retVal(0.0);

  //////////////////////////////////////
  // here determine, WHICH of the [histid] to choose (after cuuts on runnumber etc....)
  std::string cutRunNumber = "all";

  if (m_RunNumbers.size()>0) {
    unsigned int runnumber = m_defaultRandomRunNumber;
    ATH_MSG_DEBUG("RandomRunNumber: " << runnumber << " " << m_useRandomRunNumber);
    if (m_useRandomRunNumber) {
      const xAOD::EventInfo *eventInfo = evtStore()->retrieve< const xAOD::EventInfo> (m_eventInfoCollectionName);
      if (!eventInfo) {
        ATH_MSG_ERROR("Could not retrieve EventInfo object!");
        sf = 1.0;
        return CP::CorrectionCode::Error;
      }
      static const SG::AuxElement::Accessor<unsigned int> randomrunnumber("RandomRunNumber");
      if (!randomrunnumber.isAvailable(*eventInfo)) {
        sf = 1.0;
        ATH_MSG_WARNING("Pileup tool not run before using ElectronEfficiencyTool! SFs do not reflect PU distribution in data");
        return CP::CorrectionCode::Error;
      }
      runnumber = randomrunnumber(*(eventInfo));
    }
    ATH_MSG_DEBUG("Number of RunNumbers: " << m_RunNumbers.size() );
    for ( std::size_t r=0; r<m_RunNumbers.size(); r++ ){
      ATH_MSG_DEBUG(  m_RunNumbers.at(r) );
    }
    ATH_MSG_VERBOSE("DONE");

    for ( std::size_t r=0; r<m_RunNumbers.size()-1; r++ ){
      ATH_MSG_VERBOSE(m_RunNumbers.size()-1 << "  " << m_RunNumbers.at(r) << "  " << m_RunNumbers.at(r+1) << "  " << runnumber);
      if ( runnumber > (unsigned int)m_RunNumbers.at(r) && runnumber <= (unsigned int)m_RunNumbers.at(r+1) ) {
        cutRunNumber.clear();
        cutRunNumber = Form("RunNumber%d_%d",m_RunNumbers.at(r) ,m_RunNumbers.at(r+1));
        ATH_MSG_DEBUG(m_RunNumbers.at(r));
      }
    }

    if (runnumber<m_RunNumbers.at(0) || (runnumber>m_RunNumbers.at(m_RunNumbers.size()-1) )) {
      ATH_MSG_DEBUG("RunNumber not in valid RunNumber Range ");
      sf = -9999.0;
      return CP::CorrectionCode::OutOfValidityRange;
    }

  }

  // Determine WHICH histograms to use here
  const std::vector<TH2D*>& SShistograms = m_SF_SS.at(cutRunNumber.c_str());
  const std::vector<TH2D*>& OShistograms = m_SF_OS.at(cutRunNumber.c_str());


  // here check OS or SS
  bool isOS=false;

  if (truth_ele_charge * reco_ele_charge > 0) isOS=true;

  if (isOS) {
    retVal = this->getChargeFlipRate( ele_eta, ele_pt,OShistograms.at(0), sf);
    if ( retVal != 0 ) {
      sf = -9999.0;
      return CP::CorrectionCode::OutOfValidityRange;
    }
  }
  else {
    ATH_MSG_DEBUG("Get SS his");
    retVal = this->getChargeFlipRate( ele_eta, ele_pt, SShistograms.at(0), sf);
    if ( retVal != 0 ) {
      sf = -9999.0;
      return CP::CorrectionCode::OutOfValidityRange;
    }
  }

  ATH_MSG_DEBUG("eta: " << ele_eta << "  pt: "<< ele_pt );
  ATH_MSG_DEBUG("SF Rates---- . SF: " << sf );


  // Systematics ------------------------------------------------------------------------------------------------------
  double val_stat;

  /// STAT
  if (isOS) {
    retVal = this->getChargeFlipRate( ele_eta, ele_pt,OShistograms.at(1), val_stat);
    if ( retVal != 0 ) {
      sf = -9999.0;
      return CP::CorrectionCode::OutOfValidityRange;
    }
  }
  else {
    ATH_MSG_DEBUG("Get SS his");
    retVal = this->getChargeFlipRate( ele_eta, ele_pt, SShistograms.at(1), val_stat);
    if ( retVal != 0 ) {
      sf = -9999.0;
      return CP::CorrectionCode::OutOfValidityRange;
    }
  }

  std::vector<float> systs;
  double val_sys{0.0};
  /// STAT
  for (unsigned int s=2;s<OShistograms.size();s++){
    if (isOS) {
      retVal = this->getChargeFlipRate( ele_eta, ele_pt,OShistograms.at(s), val_sys);
      if ( retVal != 0 ) {
        val_sys = -9999.0;
        return CP::CorrectionCode::OutOfValidityRange;
      }
    }
    else {
      ATH_MSG_DEBUG("Get SS his");
      retVal = this->getChargeFlipRate( ele_eta, ele_pt, SShistograms.at(s), val_sys);
      if ( retVal != 0 ) {
        val_sys = -9999.0;
        return CP::CorrectionCode::OutOfValidityRange;
      }
    }
    systs.push_back(static_cast<float>(val_sys));
  }

  ATH_MSG_DEBUG(" ... nominal SF: "   << sf);


  if ( m_mySysConf.size()==0 ) {
    ATH_MSG_DEBUG(" ... nominal SF: "   << sf);
  }
  else if (*(m_mySysConf.begin()) == SystematicVariation ("EL_CHARGEID_STAT",  1)) { sf=(sf+(val_stat)); ATH_MSG_DEBUG("SF after STATup = "   << sf); }
  else if (*(m_mySysConf.begin()) == SystematicVariation ("EL_CHARGEID_STAT", -1)) { sf=(sf-(val_stat)); ATH_MSG_DEBUG("SF after STATdown = " << sf); }
  else {

    for (unsigned int i=0;i<m_systematics.size();i++){
      if (*(m_mySysConf.begin()) == SystematicVariation (Form("EL_CHARGEID_SYS%s",m_systematics.at(i).c_str()), 1)) { sf=(sf+(val_sys));  ATH_MSG_DEBUG("SF after SYSup = "    << sf); }

      if (*(m_mySysConf.begin()) == SystematicVariation (Form("EL_CHARGEID_SYS%s",m_systematics.at(i).c_str()), -1)) { sf=(sf-(val_sys));  ATH_MSG_DEBUG("SF after SYSdown = "    << sf); }

    }

    // else if (*(m_mySysConf.begin()) == SystematicVariation ("EL_CHARGEID_SYS" , -1)) { val_sf*=(1-(val_sf_sys*0.01));  ATH_MSG_DEBUG("SF after SYSdown = "  << val_sf); }
    // else ATH_MSG_ERROR("No systematic string found");
  }

  return CP::CorrectionCode::Ok;
}



//---------------------------------------------------------------------------------------
// Decorate the electron with the scale factor
//---------------------------------------------------------------------------------------


CP::CorrectionCode
CP::ElectronChargeEfficiencyCorrectionTool::applyEfficiencyScaleFactor(const xAOD::IParticle& part) const {
  ATH_MSG_DEBUG("In CP::ElectronChargeEfficiencyCorrectionTool::applyEfficiencyScaleFactor(const xAOD::IParticle& part) const");
  double sf = 0.0;
  CP::CorrectionCode result = this->getEfficiencyScaleFactor(part,sf);
  //Decorate the electron
  (*m_sfDec)(part) = static_cast<float>(sf);
  return result;

}


//// Giulia: This one won't exist!! ################################################ Kristin: But ok to just comment out for the moment

// //---------------------------------------------------------------------------------------
// // Get the efficiency for data only
// //---------------------------------------------------------------------------------------

// CP::CorrectionCode CP::ElectronChargeEfficiencyCorrectionTool::getDataEfficiency(const xAOD::Electron& ele, float& rate) {

//   const double ele_pt  = ele.pt()*m_gevmev;
//   const double ele_eta = fabs(ele.eta());
//   rate = 999.;

//   float syst_err(-1.0);
//   float stat_err(-1.0);
//   float retVal(0.0);

//   retVal= this->getChargeFlipRate(ele_eta, ele_pt, m_correctionRates_data[0], rate);
//   if ( retVal != 0 ) return CP::CorrectionCode::OutOfValidityRange;

//   //.......sys.......
//   retVal = this->getChargeFlipRate( ele_eta, ele_pt, m_correctionRates_data[1], syst_err);
//   if ( retVal != 0 )  return CP::CorrectionCode::OutOfValidityRange;

//   //.......stat.......
//   retVal = this->getChargeFlipRate( ele_eta, ele_pt, m_correctionRates_data[2], stat_err);
//   if ( retVal != 0 )  return CP::CorrectionCode::OutOfValidityRange;

//   float total=sqrt( syst_err*syst_err + stat_err*stat_err );

//   ATH_MSG_VERBOSE("Rates---- data: " << rate);


//   // Systematics ------------------------------------------------------------------------------------------------------
//   if ( m_mySysConf.size() >1 )  ATH_MSG_ERROR("m_mySysConf.size() >1 !! By now just one systematic implemented!!");

//   if ( m_mySysConf.empty() ) ATH_MSG_VERBOSE("mySysConf is empty. NOMINAL value for rate!!!--->" << rate);

//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_STAT",  1)) {rate*=(1+(stat_err*0.01)); ATH_MSG_VERBOSE("Rate data after STATup = "   << rate); }
//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_STAT", -1)) {rate*=(1-(stat_err*0.01)); ATH_MSG_VERBOSE("Rate data after STATdown = " << rate); }

//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_SYS" ,  1)) {rate*=(1+(syst_err*0.01)); ATH_MSG_VERBOSE("Rate data after SYSup = "    << rate); }
//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_SYS" , -1)) {rate*=(1-(syst_err*0.01)); ATH_MSG_VERBOSE("Rate data after SYSdown = "  << rate); }

//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_TOT" ,  1)) {rate*=(1+(total*0.01)); ATH_MSG_VERBOSE("Rate data after TOTup = "    << rate); }
//   else if (*(m_mySysConf.begin()) == SystematicVariation ("ELE_ChargeMisID_TOT" , -1)) {rate*=(1-(total*0.01)); ATH_MSG_VERBOSE("Rate data after TOTdown = "  << rate); }

//   else ATH_MSG_ERROR("No systematic string found");


//   return CP::CorrectionCode::Ok;
// }




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get the charge of the original electron
// Giulia: not needed anymore!

CP::CorrectionCode CP::ElectronChargeEfficiencyCorrectionTool::getEleTruthCharge( const xAOD::Electron& ele,  int& truthcharge) const
{
  // if ( !(ele.isAvailable<int>("firstEgMotherPdgId")) || !(ele.isAvailable<int>("truthPdgId")) ) {
  if ( !(ele.isAvailable<int>("firstEgMotherPdgId")) ) {
    ATH_MSG_ERROR("Link not available for firstEgMotherPdgId...BAD!!!");
    ATH_MSG_ERROR("Need to have present: ( !(ele.isAvailable<int>('firstEgMotherPdgId')) )" );
    return CP::CorrectionCode::OutOfValidityRange;
  }

  //don't you need a - sign? like electron pdg is +11? or am I wrong?
  truthcharge = (-1)*ele.auxdata<int>("firstEgMotherPdgId");
  // Make truthcharge -1, 0, +1
  truthcharge = (0 < truthcharge) - (truthcharge < 0);

  return CP::CorrectionCode::Ok;

}



//Giulia: already changed -> need to be checked --> Kristin: Looks good!

CP::CorrectionCode CP::ElectronChargeEfficiencyCorrectionTool::isGoodEle( const xAOD::Electron& ele, bool& goodele) const
{

  // good ele => (firstEgMotherPdgId) == 11 ## valid for both iso and conversion ele

  goodele = false;
  int firstEgPdgId = -9999;

  if ( !(ele.isAvailable<int>("firstEgMotherPdgId")) ) {
    ATH_MSG_ERROR( "firstEgMotherPdgId IS NOT AVAILABLE!!" );
    return CP::CorrectionCode::OutOfValidityRange;
  }
  else {

    firstEgPdgId = ele.auxdata<int>("firstEgMotherPdgId");

    if ( std::abs(firstEgPdgId) != 11) {

      goodele = false;
      ATH_MSG_VERBOSE( "  electron is not GOOD .... returning ...." );
      return CP::CorrectionCode::Ok;

    }
    else {

      goodele = true;
      return CP::CorrectionCode::Ok;

    }
  }

  return CP::CorrectionCode::OutOfValidityRange;
}


// Get the correction rate given pt (E), eta, histogram
float CP::ElectronChargeEfficiencyCorrectionTool::getChargeFlipRate( double eta, double pt, TH2D *hrates, double& flipRate) const
{
  ATH_MSG_VERBOSE(" -> in: getChargeFlipRate(" << pt <<", " << eta << " TH2D, double&)");

  if ( eta < m_eta_lowlimit || eta > m_eta_uplimit ) {

    ATH_MSG_ERROR("Got an electron outside of the range of ETA validity " << eta);
    return 1;
  }

  if ( pt < m_pt_lowlimit ) {

    ATH_MSG_ERROR("Got an electron outside of the range of pt validity: pt lower than lower limit");
    return 2;
  }

  if ( pt > m_pt_uplimit ) pt=m_pt_uplimit*0.999;

  int bin2D = hrates->FindBin(pt, eta);
  flipRate  = hrates->GetBinContent(bin2D);

  ATH_MSG_VERBOSE(" -> flipRate is " << flipRate <<", for histogram " << hrates->GetName() );


  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// returns whether this tool is affected by the given systematics
bool  CP::ElectronChargeEfficiencyCorrectionTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {

  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// returns the list of all systematics this tool can be affected by

CP::SystematicSet CP::ElectronChargeEfficiencyCorrectionTool::affectingSystematics() const{
  CP::SystematicSet result;
  result.insert (SystematicVariation ("EL_CHARGEID_STAT" , 1));
  result.insert (SystematicVariation ("EL_CHARGEID_STAT" , -1));

  for (unsigned int i=0;i< m_systematics.size();i++){
    result.insert (SystematicVariation (Form("EL_CHARGEID_SYS%s",m_systematics.at(i).c_str()), 1));
    result.insert (SystematicVariation (Form("EL_CHARGEID_SYS%s",m_systematics.at(i).c_str()), -1));
  }
  return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// returns the list of all systematics this tool recommends to use
CP::SystematicSet  CP::ElectronChargeEfficiencyCorrectionTool::recommendedSystematics() const {

  return affectingSystematics();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets a SystematicSet and filters it
CP::SystematicCode CP::ElectronChargeEfficiencyCorrectionTool::applySystematicVariation( const SystematicSet& systConfig ) {

  if (!SystematicSet::filterForAffectingSystematics(systConfig, m_affectingSys, m_mySysConf)){
    ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool! ");
    return SystematicCode::Unsupported;
  }



  return SystematicCode::Ok;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
