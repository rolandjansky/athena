#include "NSWCalibSmearingTool.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "PathResolver/PathResolver.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace Muon;

Muon::NSWCalibSmearingTool::NSWCalibSmearingTool(const std::string& t,
						 const std::string& n, 
						 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
  declareInterface<INSWCalibSmearingTool>(this);
  
  declareProperty("TimeSmear",   m_timeSmear   = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChargeSmear", m_chargeSmear = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChannelEfficiency",  m_channelEfficiency  = {1.,1.,1.,1.,1.,1.,1.,1.});
  declareProperty("ClusterEfficiency",  m_clusterEfficiency  = {1.,1.,1.,1.,1.,1.,1.,1.});

  declareProperty("GainFraction",  m_gainFraction  = {1.,1.,1.,1.,1.,1.,1.,1.});
  
  declareProperty("PhiSectors", m_phiSectors = {true,true,true,true,true,true,true,true} );
  // first two eta sectors are side-C, second two are side-A
  declareProperty("EtaSectors", m_etaSectors = {true,true,true,true} );

  //initialize the efficiency values reading from a file
  declareProperty("ReadEfficiencyFromFile",m_readEfficiencyFromFile=false);
  declareProperty("ReadGainFractionFromFile",m_readGainFractionFromFile=false);
  declareProperty("FileName",m_fileName);

}


Muon::NSWCalibSmearingTool::~NSWCalibSmearingTool()
{ }


StatusCode Muon::NSWCalibSmearingTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");

  // initialize the MuonIdHelperTool and check the configuration
  ATH_CHECK(m_idHelperTool.retrieve());

  if ( !(m_idHelperTool->HasMM() && m_idHelperTool->HasSTgc() ) ) {
    ATH_MSG_ERROR("MuonIdHelperTool not properly configured, missing MM or STGC");
    return StatusCode::FAILURE;
  }
 
  m_random = TRandom3();

  if (m_readEfficiencyFromFile || m_readGainFractionFromFile) {
    ATH_CHECK(readHighVoltagesStatus());
  }

  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibSmearingTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");

  return StatusCode::SUCCESS;
}

//
// check if a hit is acceppted
//
StatusCode Muon::NSWCalibSmearingTool::isAccepted(const Identifier id, bool& accepted)
{
  accepted = true;

  int etaSector = 0;
  int phiSector = 0;
  int gasGap = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  /// either efficiency per layer set via configuration, or read from file
  float efficiencyCut = 0.0;
  if ( !m_readEfficiencyFromFile ) {
    efficiencyCut = m_clusterEfficiency.value()[gasGap-1];
  }
  else {
    float hv = getHighVoltage(id);

    if(hv == -2.0) { // could not convert id to pcb id
      return StatusCode::FAILURE;
    }
    else if(hv == -1.0) { // hv not found in hv map
      accepted = true;
      return StatusCode::SUCCESS;
    }
    else {
      efficiencyCut = getMMEfficiencyFromHV(hv);
    }
  }
  /// check if a full hit can be accepted
  if ( m_random.Rndm() > efficiencyCut ) {
    accepted = false;
  }
  return StatusCode::SUCCESS;
}




//
// smear only the charge
//
StatusCode Muon::NSWCalibSmearingTool::smearCharge(Identifier id, float& charge, bool& accepted)
{

  ATH_MSG_DEBUG("Smearing the strips charge");

  int etaSector = 0;
  int phiSector = 0;
  int gasGap = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {
    // smear charge
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    charge = charge+m_random.Gaus(0.0,chargeSmear);
    
    // check if the single strip can be accepted
    accepted = true;
    if ( m_random.Rndm() > m_channelEfficiency.value()[gasGap-1] ) {
      accepted = true;
    }
  }

  return StatusCode::SUCCESS;
}

//
// smear time and charge
//
StatusCode Muon::NSWCalibSmearingTool::smearTimeAndCharge(Identifier id, float& time, float& charge, bool& accepted)
{
  
  if ( m_idHelperTool->issTgc(id) ) {
    ATH_MSG_ERROR("Can't smear time for the STGC's");
    return StatusCode::FAILURE;
  } 

  int etaSector = 0;
  int phiSector = 0;
  int gasGap    = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {

    // smear time and charge
    double timeSmear   = m_timeSmear.value()[gasGap-1];
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    
    time = time+m_random.Gaus(0.0,timeSmear);
    charge = charge+m_random.Gaus(0.0,chargeSmear);

    // check if the RDO can be accepted
    accepted = true;
    if ( m_random.Rndm() > m_channelEfficiency.value()[gasGap-1] ) {
      accepted = false;
    }
  }

  return StatusCode::SUCCESS;
}

//
// get the fraction of the actual gain for a given gap
//
StatusCode Muon::NSWCalibSmearingTool::getGainFraction(Identifier id, float& gainFraction)
{
  int etaSector = 0;
  int phiSector = 0;
  int gasGap    = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  gainFraction = 1.0;

  if(!m_readGainFractionFromFile) {
    if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {
      gainFraction = m_gainFraction.value()[gasGap-1];
    }
  }
  else {
    float hv = getHighVoltage(id);
    if(hv == -2.0) { // could not convert id to PCB id
      return StatusCode::FAILURE;
    }
    else if(hv == -1.0) { // could not find PCB in HV map
      gainFraction = 1;
    }
    else {
      gainFraction=getMMGainFractionFromHV(hv);
      ATH_MSG_DEBUG("Got gain fraction: "<< gainFraction << " for id " << m_idHelperTool->toString(id));     
    }
  }
  return StatusCode::SUCCESS;
}


//
// get id fields for both STGC and MM
//
bool NSWCalibSmearingTool::getIdFields(const Identifier id, int& etaSector, int& phiSector,
				       int& gasGap)
{

  if ( m_idHelperTool->isMM(id) ) {
    int multilayer = m_idHelperTool->mmIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->mmIdHelper().gasGap(id);
    etaSector = m_idHelperTool->mmIdHelper().stationEta(id);
    phiSector = m_idHelperTool->mmIdHelper().stationPhi(id);
  } 
  else if ( m_idHelperTool->issTgc(id) ) {
    int multilayer = m_idHelperTool->stgcIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->stgcIdHelper().gasGap(id);
    etaSector = m_idHelperTool->stgcIdHelper().stationEta(id);
    phiSector = m_idHelperTool->stgcIdHelper().stationPhi(id);
  } 
  else {
    ATH_MSG_WARNING("Wrong identifier: should be MM or STGC");
    return false;
  }

  // transform the eta sector
  etaSector < 0 ? etaSector = etaSector + 3 : etaSector = etaSector + 2;


  if ( phiSector < 1 || phiSector> (int) m_phiSectors.value().size() 
       || etaSector < (int) (-m_etaSectors.value().size()) || etaSector> (int) m_etaSectors.value().size() || etaSector==0
       || gasGap < 1 || gasGap> (int) m_timeSmear.value().size() || gasGap>(int) m_chargeSmear.value().size() ) {
    ATH_MSG_WARNING("Wrong phi, eta sector, or gasGap number: " << phiSector << " " 
		  << etaSector << " " << gasGap);
    return false;
  }

  return true;
}

//
// get the high voltage from a strip identifier
double NSWCalibSmearingTool::getHighVoltage(Identifier stripId) const
{
  Identifier pcbId;
  bool foundPCB = getPCBIdentifier(stripId,pcbId);
  if ( !foundPCB ) {
    ATH_MSG_ERROR("Identifier " << m_idHelperTool->toString(stripId) << " not converted" );
    return -2.0;
  } 

  double hv = -1.0;
  std::map<Identifier,float>::const_iterator it = m_hvMap.find(pcbId);
  if (it == m_hvMap.end() ) {
    ATH_MSG_DEBUG("Identifier " << m_idHelperTool->toString(pcbId) << " not found in the map" );
    return -1.0;
  }

  hv = (*it).second;
  return hv;
}


//
// get the efficiency from the parametrization vs HV for the MM
double NSWCalibSmearingTool::getMMEfficiencyFromHV(double hv) const
{

  // sigmoid to paramtrize efficiency  (initial values from BB5 measurements)
  double eff = 1.0/(1+exp(-0.0551*(hv-510.54)));

  return eff;
}

//
// get the gain fraction from the parametrization vs HV for the MM
double NSWCalibSmearingTool::getMMGainFractionFromHV(double hv) const
{

  // initial values from BB5 measurements. Scale cluster charge with respect to 570 V
  return  std::exp(-8.87971 + 0.0224561 * hv) / std::exp(-8.87971 + 0.0224561 * 570);

}



///
// get the PCB identifier as the identifier of the central strip ( 512 ) of each PCB (MM only)
///
bool NSWCalibSmearingTool::getPCBIdentifier(const Identifier id, Identifier& pcb_id) const
{
  
  if ( m_idHelperTool->isMM(id) ) {
    // get the channel number
    int channel = m_idHelperTool->mmIdHelper().channel(id);
    int pcb_strip = channel/1024;
    pcb_strip = pcb_strip * 1024 + 512;
    
    int stationName = m_idHelperTool->mmIdHelper().stationName(id);
    int stationEta  = m_idHelperTool->mmIdHelper().stationEta(id);
    int stationPhi = m_idHelperTool->mmIdHelper().stationPhi(id);

    int multilayer = m_idHelperTool->mmIdHelper().multilayer(id);
    int gasGap = m_idHelperTool->mmIdHelper().gasGap(id);

    pcb_id = m_idHelperTool->mmIdHelper().channelID(stationName,stationEta,stationPhi,multilayer,gasGap,pcb_strip);

  }
  else {
    ATH_MSG_WARNING("Requesting PCB id for STGC");
    return false;
  }
  
  return true;
}

//
// read the MM HV map from a set of ascii files
//
StatusCode NSWCalibSmearingTool::readHighVoltagesStatus()
{

  std::string fileNamesA[16] = {"A01.txt","A02.txt","A03.txt","A04.txt",
				"A05.txt","A06.txt","A07.txt","A08.txt",
				"A09.txt","A10.txt","A11.txt","A12.txt",
				"A13.txt","A14.txt","A15.txt","A16.txt" };
  
  for (int ifile = 0 ; ifile<16 ; ++ifile) {

    std::string fileName = PathResolverFindCalibFile(Form("NSWCalibTools/210216_initial/%s", fileNamesA[ifile].c_str()));

    std::ifstream file(fileName,std::ios::in);
    if ( !file.is_open() ) {
      ATH_MSG_DEBUG("HV File " << fileNamesA[ifile] << " not available " );
      continue;
    } 
    ATH_MSG_INFO("Reading HV from configuration file: " << fileName);

    //LM sector
    //Layer PCB HV_left HV_right HV_min
    //LM1 - IP

    std::string line;
    bool isLM,isSM,isIP,isHO;
    int stationName,stationEta,stationPhi,multilayer,gasGap,HVval;
    int side = 0;
    std::string layerId[4] = {"L1","L2","L3","L4"};

    int endPCB=0;

    getline(file,line);
    ATH_MSG_VERBOSE(line);
    getline(file,line);
    ATH_MSG_VERBOSE(line);
    
    while ( getline(file,line) ) {
      ATH_MSG_VERBOSE(line);
      
      isIP=false;
      isHO=false;
      
      std::string secName = fileNamesA[ifile].substr(0,2);
	
      if ( secName.substr(0,1)=="A" ) side = +1;
      else if ( secName.substr(0,1)=="C" ) side = -1;
      else {
	ATH_MSG_ERROR("ERROR side not defined");
	return StatusCode::FAILURE;
      }
      int phiSec = std::stoi(fileNamesA[ifile].substr(1,2));
      stationPhi = (phiSec-1)/2+1;
      
      size_t fLM = line.find("LM");
      size_t fSM = line.find("SM");
      isLM = (fLM!=std::string::npos);
      isSM = (fSM!=std::string::npos);
      
      // get layer 1 from the line with the module name
      if ( isLM || isSM ) { 
	if ( isSM ) {
	  stationEta = side*std::stoi(line.substr(fSM+2,1));
	}
	else if ( isLM ) {
	  stationEta = side*std::stoi(line.substr(fLM+2,1));
	}
	
	/// PCB range is 1 to 5 for stations 1, 1 to 3 for stations 2
	if (stationEta==1) endPCB=5;
	else if (stationEta==2) endPCB=3;
	else {
	  ATH_MSG_ERROR("wrong stationEta value = " << stationEta);
	}
	
	isSM ? stationName=55 : stationName=56;
	
	std::size_t fIP = line.find("IP");
	isIP = (fIP!=std::string::npos);
	std::size_t fHO = line.find("HO");
	isHO = (fHO!=std::string::npos);
	if ( !isIP && !isHO ) {
	  ATH_MSG_ERROR("ERROR multilayer id not found (IP/HO) "); 
	  return StatusCode::FAILURE;
	}
	else if ( isIP && isHO ) {
	  ATH_MSG_ERROR("ERROR multilayer id duplicated (IP and HO) ");
	  return StatusCode::FAILURE;
	}
	else if ( isIP ) {
	  multilayer = 1;
	}
	else if ( isHO ) {
	  multilayer = 2;
	}
	
	// now read the various layers
	//int ilayer = 1;
	//int ipcb = 1;
	for (int ilayer = 1; ilayer <= 4; ilayer++) {
	  for (int ipcb = 1; ipcb <= endPCB; ipcb++) { 	  
	    getline(file,line);
	    ATH_MSG_VERBOSE(line);
	    
	    std::istringstream elem(line);
	    std::vector<std::string> elements;
	    
	    while(elem) {
	      std::string subs_elem;
	      elem >> subs_elem;
	      if (subs_elem != "") elements.push_back(subs_elem);
	    }
	    
	    gasGap = std::stoi(elements[0]);
	    int readed_pcb = std::stoi(elements[1]);
	    if (gasGap != ilayer || readed_pcb != ipcb) {
	      ATH_MSG_ERROR("Layer or pcb wrong!");
	      return StatusCode::FAILURE;
	    }	    

	    HVval=std::stoi(elements[4]);
	    
	    ATH_MSG_DEBUG("PCB done, stationName, stationEta, stationPhi, ml, layer, pcb, hv: "
			  << stationName<< " " << stationEta << " " << stationPhi << " " << multilayer << " "
			  << ilayer << " " << ipcb << " " << HVval );
	    
	    int chanNum = (ipcb-1)*1024+512;
	    /// add the PCB to the map
	    Identifier pcbId = m_idHelperTool->mmIdHelper().channelID(stationName,stationEta,stationPhi,
									multilayer,ilayer,chanNum);
	    float hv = (float)HVval;
	    m_hvMap.insert(std::pair<Identifier,float>(pcbId,hv));
	    
	  }
	  if ( ilayer == 4 ) { 
	    getline(file,line);
	    ATH_MSG_VERBOSE(line);
	  }
	} // loop on the layers
      }
    }
  }   // loop on the files
  return StatusCode::SUCCESS;
}

