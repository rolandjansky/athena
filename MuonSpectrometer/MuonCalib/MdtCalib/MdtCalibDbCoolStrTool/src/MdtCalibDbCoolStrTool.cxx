/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MdtCalibData/CalibFunc.h"
#include "MuonCalibStl/ToString.h"
#include "MdtCalibUtils/RtDataFromFile.h"
#include "PathResolver/PathResolver.h"
#include "MdtCalibData/MdtCalibrationFactory.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MuonCalibMath/SamplePoint.h"
#include "MuonCalibTools/IdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibIdentifier/MdtCalibCreationFlags.h"
#include <fstream>
#include <string>
#include <vector>

#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"

#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

#include "MdtCalibDbCoolStrTool/MdtCalibDbCoolStrTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TSpline.h"
#include "TFile.h"

namespace MuonCalib {

MdtCalibDbCoolStrTool::MdtCalibDbCoolStrTool(const std::string &type,
					     const std::string &name,
					     const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),
    m_IOVDbSvc("IOVDbSvc", name),
    m_regionSvc("MdtCalibrationRegionSvc", name),
    m_rtFolder("/MDT/RTBLOB"), m_tubeFolder("/MDT/T0BLOB"),
    m_rtDataLocation("RtKey"), m_tubeDataLocation("TubeKey"),
    m_TimeSlewingCorrection(false),
    m_UseMLRt(true),
    m_TsCorrectionT0(0.),
    m_defaultT0(40.),
    m_t0Shift(0.),
    m_t0Spread(0.),
    m_rtShift(0.),
    m_rtScale(1.),
    m_prop_beta(1.0),
    m_AtRndmGenSvc ("AtRndmGenSvc", name),
    m_buffer_length(0),
    m_decompression_buffer(NULL){

  declareInterface< IMdtCalibDBTool >(this);

  // the keys are hardcoded in order to be sure to match the ones
  // used in regFcn by the Svc
  //declareProperty("TubeKey",m_tubeDataLocation);
  //declareProperty("RtKey",m_rtDataLocation);

  // Db Folders
  declareProperty("TubeFolder",m_tubeFolder,"DB folder containing the tube constants");
  declareProperty("RtFolder",m_rtFolder,"DB folder containing the RT calibrations");
  
  // Properties to deform the t0 and rt relationship
  declareProperty("T0Shift",m_t0Shift,"for simulation: common shift of all T0s, in ns");
  declareProperty("T0Spread",m_t0Spread,"for simulation: sigma for random smeraing of T0s, in ns");
  declareProperty("RTShift",m_rtShift,"for simulations: maximum RT distortion, in mm");
  declareProperty("RTScale",m_rtScale,"for simulations: a muliplicitive scale to the drift r");

  // is this the simplest way to initialize a list?
  std::ostringstream myse;
  std::vector<std::string> myord;
  myse << "DC2_rt_default.dat";
  myord.push_back(myse.str());

  declareProperty("RT_InputFiles",m_RTfileNames=myord,"single input ascii file for default RT to be applied in absence of DB information");

  // defaultT0, used for tubes not found in DB
  declareProperty("defaultT0",m_defaultT0,"default T0 value to be used in absence of DB information");
  declareProperty("TimeSlewingCorrection", m_TimeSlewingCorrection);
  declareProperty("MeanCorrectionVsR", m_MeanCorrectionVsR);
  declareProperty("UseMLRt", m_UseMLRt,"Enable use of ML-RTs from COOL");
  declareProperty("PropagationSpeedBeta", m_prop_beta);
}  //end MdtCalibDbCoolStrTool::MdtCalibDbCoolStrTool

MdtCalibDbCoolStrTool::~MdtCalibDbCoolStrTool() {
  if( m_decompression_buffer != NULL )
    delete [] m_decompression_buffer;
}

// Check StoreGate ClassID (clid) and key for tube and RT calib data (required by the IAddressProvider interface)
// (poor name choice, not related to updating address)
StatusCode MdtCalibDbCoolStrTool::updateAddress(StoreID::type /*storeID*/,
                                                SG::TransientAddress *tad,
                                                const EventContext& /*ctx*/) {
  CLID clid        = tad->clID();
  std::string key  = tad->name();
  if( 1221928754 == clid && m_tubeDataLocation == key) {
    ATH_MSG_DEBUG( "OK Tube" );
    return StatusCode::SUCCESS;
  }
  if( 1270996316 == clid && m_rtDataLocation == key) {
    ATH_MSG_DEBUG( "OK Rt" );
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode MdtCalibDbCoolStrTool::initialize() { 
  ATH_MSG_DEBUG( "MdtCalibDbCoolStrTool::initialize START " );

// if timeslew correction vector m_MeanCorrectionVsR has non-zero size then set
// m_TsCorrectionT0=m_MeanCorrectionVsR[0] and subtract this each value in the vector.
  if(m_MeanCorrectionVsR.size()) {
    m_TsCorrectionT0 = m_MeanCorrectionVsR[0];
    for(std::vector<float>::iterator it=m_MeanCorrectionVsR.begin(); it!=m_MeanCorrectionVsR.end(); it++) {
      (*it) -= m_TsCorrectionT0;
    }
  }

  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );
  ATH_CHECK( detStore()->retrieve( m_detMgr ) );

  ATH_CHECK( m_IOVDbSvc.retrieve() );
  ATH_CHECK( m_regionSvc.retrieve() );

  // register callbacks
  const DataHandle<CondAttrListCollection> tubeData;
  ATH_CHECK( detStore()->regFcn(&IMdtCalibDBTool::loadTube,
                                dynamic_cast<IMdtCalibDBTool*>(this),
                                tubeData,
				m_tubeFolder) );
  const DataHandle<CondAttrListCollection> rtData;
  ATH_CHECK( detStore()->regFcn(&IMdtCalibDBTool::loadRt,
                                dynamic_cast<IMdtCalibDBTool*>(this),
                                rtData,
				m_rtFolder) );

  ATH_CHECK( m_idToFixedIdTool.retrieve() );

  ServiceHandle<MdtCalibrationDbSvc> dbSvc("MdtCalibrationDbSvc", name());
  ATH_CHECK( dbSvc.retrieve() );
  ATH_MSG_DEBUG( "Retrieved MdtCalibrationDbSvc" );

  ATH_CHECK( detStore()->regFcn(&IMdtCalibDBTool::loadTube,
				dynamic_cast<IMdtCalibDBTool*>(this),
				&MdtCalibrationDbSvc::loadTube,
				dynamic_cast<MdtCalibrationDbSvc*>(&*dbSvc)) );
  ATH_CHECK( detStore()->regFcn(&IMdtCalibDBTool::loadRt,
				dynamic_cast<IMdtCalibDBTool*>(this),
				&MdtCalibrationDbSvc::loadRt,
				dynamic_cast<MdtCalibrationDbSvc*>(&*dbSvc)) );

  // initialize MdtTubeCalibContainers 
  ATH_CHECK( defaultT0s() );
  ATH_CHECK( detStore()->record( m_tubeData, m_tubeDataLocation, true ) );

  // Get the TransientAddress from DetectorStore and set "this" as the AddressProvider
  // First get the proxy from StoreGate using the CLID and key for MdtTubeCalibContainerCollection
  SG::DataProxy *proxy = detStore()->proxy(ClassID_traits<MdtTubeCalibContainerCollection>::ID(), m_tubeDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class MdtTubeCalibContainerCollection" );
    return StatusCode::FAILURE;
  }

  // Address provider is the pointer to this tool (MdtCalibDbCoolStrTool)
  // That is, the calib data is stored in MdtCalibDbCoolStrTool, but a pointer to that
  // is kept in StoreGate (I think...)
  IAddressProvider *addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG( "set address provider for MdtTubeCalibContainerCollection" );

  // initialize MdtRtRelationCollection 
  // if COOL RT folder is called /MDT/RTUNIQUE then only read one RT from COOL and use for all chambers
  // Not sure this option has ever been used, perhaps could be used for simulated data.
  // Job option RtFolder would need to be set to "/MDT/RTUNIQUE" to make this work.
  if(m_rtFolder == "/MDT/RTUNIQUE") {
    m_regionSvc->remapRtRegions("OneRt"); 
  } else if( m_UseMLRt ) {
    m_regionSvc->remapRtRegions("OnePerMultilayer");
  } else {
    m_regionSvc->remapRtRegions("OnePerChamber");
  }
  ATH_CHECK( defaultRt() );
  ATH_CHECK( detStore()->record( m_rtData, m_rtDataLocation, true ) );
                                                                                
  // Get the TransientAddress from DetectorStore and set "this" as the AddressProvider
  proxy = detStore()->proxy(ClassID_traits<MdtRtRelationCollection>::ID(), m_rtDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class MdtRtRelationCollection" );
    return StatusCode::FAILURE;
  }
                                                                                
  addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG( "set address provider for MdtRtRelationCollection" );
   
  //initiallize random number generator if doing t0 smearing (for robustness studies)
  if( m_t0Spread != 0. ) {
    ATH_CHECK( m_AtRndmGenSvc.retrieve() );
    ATH_MSG_DEBUG( " initialize Random Number Service: running with t0 shift "
                    << m_t0Shift << " spread " << m_t0Spread << " rt shift " << m_rtShift );
      
    // getting our random numbers stream
    p_engine = m_AtRndmGenSvc->GetEngine("MDTCALIBDBASCIITOOL");
  }

  if ( m_rtShift != 0. || m_rtScale != 1. || m_t0Shift != 0. || m_t0Spread != 0.) {
    ATH_MSG_INFO( "************************************" << std::endl
                   << " Running with Calibration Deformations! " << std::endl
                   << " For performance studies only!" << std::endl
                   << " **************************************" );
    ATH_MSG_DEBUG( " rt scale " << m_rtScale << " t0 shift " 
                    << m_t0Shift << " spread " << m_t0Spread << " rt shift " << m_rtShift );
  }
  
  return StatusCode::SUCCESS;
}  //end MdtCalibDbCoolStrTool::initialize

// retrieve constants from database, calls loadRt or loadTube according to the IOVSVC_CALLBACK_ARGS
StatusCode MdtCalibDbCoolStrTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  StatusCode sc;
  ATH_MSG_DEBUG( "LoadCalibration has been triggered for the following keys " );
  
  std::list<std::string>::const_iterator itr;
  if( msgLvl(MSG::DEBUG) ) {
    for (itr=keys.begin(); itr!=keys.end(); ++itr) {
      msg() << MSG::DEBUG << *itr << " I="<<I<<" ";
    }
    msg() << MSG::DEBUG << endmsg;
  }

  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    if(*itr==m_tubeFolder) {
      sc=loadTube(I,keys);    
      if(sc!=StatusCode::SUCCESS) return sc;
    }
    if(*itr==m_rtFolder) {
      sc=loadRt(I,keys);    
      if(sc!=StatusCode::SUCCESS) return sc;
    }
  }

  return StatusCode::SUCCESS;
}  //end MdtCalibDbCoolStrTool::LoadCalibration

// loads the tube constants (T0, ADCcal) from COOL DB
StatusCode MdtCalibDbCoolStrTool::loadTube(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_DEBUG( "Load Tube Calibration from DB" );
  // retrieve the and remove the old collection 
  StatusCode sc = detStore()->retrieve( m_tubeData, m_tubeDataLocation );
  if(sc.isSuccess()) {
    sc = detStore()->removeDataAndProxy( m_tubeData );
    if( msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG( "Tube Collection found " << m_tubeData );
      if(sc.isSuccess()) {
	ATH_MSG_DEBUG( "Tube Collection at " << m_tubeData << " removed ");
      }
    }
  }

  // reinitialize the MdtTubeCalibContainerCollection
  ATH_CHECK( defaultT0s() );

  //retrieve the collection of strings (calibration data [T0s]) read from DB
  const CondAttrListCollection *atrc = nullptr;
  ATH_CHECK( detStore()->retrieve(atrc,m_tubeFolder) );

  // Inverse of wire propagation speed
  float inversePropSpeed = 1./(299.792458*m_prop_beta);

  // unpack the strings in the collection and update the 
  // MdtTubeCalibContainers in TDS
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList &atr = itr->second;
    std::string header="",payload="",trailer="";

    bool t0_ts_applied = (atr["tech"].data<int>() & TIME_SLEWING_CORRECTION_APPLIED);
    // If BLOB data then uncompress
    if (atr["data"].specification().type() == typeid(coral::Blob)) {
      ATH_MSG_VERBOSE( "Data load is a blob. Uncompressing");
      if(!uncompressInMyBuffer(atr["data"].data<coral::Blob>())) {
	ATH_MSG_FATAL( "Cannot uncompress buffer" );
	return StatusCode::FAILURE;
      }
      std::string istr(reinterpret_cast<char*>(m_decompression_buffer));
      ATH_CHECK( extractString(istr, header, "\n") );
      ATH_CHECK( extractString(istr, payload, "\n") );
      if( istr.size() ) ATH_CHECK( extractString(istr, trailer, "\n") );
    } else {        //else is uncompressed CLOB (no longer used)
      std::string data;
      data = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
      ATH_MSG_VERBOSE( "Data load is " << data );

      // interpret as string stream
      std::string istr(data.c_str());
      ATH_CHECK( extractString(istr, header, "\n") );
      ATH_CHECK( extractString(istr, payload, "\n") );
      if( istr.size() ) ATH_CHECK( extractString(istr, trailer, "\n") );
    }
    ATH_MSG_VERBOSE( "Read header:" << header << " payload:" << payload << " trailer:" << trailer );

    // Extract info from the header line, chamber name, number of tubes.
    int ieta=-99, iphi=-99, region=-99, ntubes=-99;
    //    std::string rName;

    // parameters for the MdtTubeContainer
    // header filename,version,region,tubes
    char *parameters = new char [header.size()+1];
    strncpy(parameters, header.c_str(), header.size()+1);
    parameters[header.size()] = '\0';      //terminate string
    char *pch = strtok(parameters," _,");  //split using delimiters "_" and ","
    std::string name(pch,2,3);             //extract 3-character station to "name" (e.g. BIL) 

    // Split header line and extract phi, eta, region, ntubes
    pch = strtok (NULL, "_,");
    for( int i=1; pch!=NULL; pch=strtok(NULL,"_,"), i++ ) {
      std::istringstream is(pch);
      if(i==1) {
	is >> iphi; 
      } else if(i==2) {
	is >> ieta; 
      } else if(i==4) {
	is >> region; 
      } else if(i==5) {
	is >> ntubes; 
      }
    }
    delete [] parameters;

    // find chamber ID
    Identifier chId = m_mdtIdHelper->elementID(name,ieta,iphi);
 
    MuonCalib::MdtTubeCalibContainer *tubes = NULL;

    // get chamber hash
    IdentifierHash hash;
    IdContext idCont = m_mdtIdHelper->module_context();
    m_mdtIdHelper->get_hash( chId , hash, &idCont );

    if( msgLvl(MSG::VERBOSE) ) {
      ATH_MSG_VERBOSE( "name of chamber is " << pch << " station name is " << name );
      ATH_MSG_VERBOSE( "phi value is " << iphi );
      ATH_MSG_VERBOSE( "eta value is " << ieta );
      ATH_MSG_VERBOSE( "region value is " << region );
      ATH_MSG_VERBOSE( "ntubes value is " << ntubes );
      ATH_MSG_VERBOSE( "station name is " << name << " chamber ID  is " << chId );
      ATH_MSG_VERBOSE( "corresponding hash is " << hash );
    }
    
   //skip illegal stations.     
    if (hash>=m_tubeData->size()) {
      ATH_MSG_INFO( "Illegal station (1)! (" << name << "," << iphi << "," << ieta << ")" );
      continue;
    }
                                                                                
    // retrieve the existing one (created by defaultt0() )
    tubes = (*m_tubeData)[hash];

    if(tubes==NULL) {
      ATH_MSG_INFO( "Illegal station (2)! (" << name << "," << iphi << "," << ieta << ")" );
      continue;
    }
    
    int nml       = tubes->numMultilayers();
    int nlayers   = tubes->numLayers();
    int ntubesLay = tubes->numTubes();
    int size      = nml*nlayers*ntubesLay;
    if(size!=ntubes) {
      ATH_MSG_ERROR( "Pre-existing MdtTubeCalibContainer for chamber ID " <<chId<< " size does not match the one found in DB ");
      return StatusCode::FAILURE;
    }

    //Extract T0, ADCcal, valid flag for each tube from payload.
    MuonCalib::MdtTubeCalibContainer::SingleTubeCalib datatube;
    char *TubePar= new char [payload.size()+1];
    strncpy(TubePar, payload.c_str(), payload.size()+1);
    TubePar[payload.size()]='\0';
    
    //Loop over payload 
    char *pch1=strtok(TubePar,",");
    int ml=1, l=1, t=1;
    for( int k=1; pch1!=NULL; pch1=strtok(NULL,", "), k++) { 
      if(k==1) {
	double tzero = atof(pch1);
	if( m_t0Shift != 0. ){
	  tzero += m_t0Shift;
	  ATH_MSG_VERBOSE( "T0 shift " << m_t0Shift << " t0 " << tzero 
			   << " id " << ml << " " << l << " " << t );
	}
	if(m_t0Spread != 0. ){
	  double sh = CLHEP::RandGaussZiggurat::shoot(p_engine,0.,m_t0Spread);
	  tzero += sh;
	  ATH_MSG_VERBOSE( "T0 spread " << sh << " t0 " << tzero 
			   << " id " << ml << " " << l << " " << t );
	}
	if(!t0_ts_applied && m_TimeSlewingCorrection) {
	  tzero += m_TsCorrectionT0;
	}
	if(t0_ts_applied && !m_TimeSlewingCorrection) {
	  tzero -= m_TsCorrectionT0;
	}
	datatube.t0=tzero;
      } else if(k==2) {
	datatube.statusCode = atoi(pch1);
      } else if(k==3) {
        datatube.adcCal = atof(pch1);
        datatube.inversePropSpeed = inversePropSpeed;
        tubes->setCalib( ml-1,l-1,t-1,datatube);
        ATH_MSG_VERBOSE( "Loading T0s "<<ml << " " << l << " " << t << " " << datatube.t0 );
        t++; k=0;
        if (t>ntubesLay) {
          l++; 
          t=1;
        }
        if (l>nlayers) {
          ml++;
          l=1;
        }
      }
    }
    delete [] TubePar;
  }  //end loop over atrc
  
  // update the IOV for the MdtTubeCalibContainerCollection 
  // using the IOV from the string class
  // for some reason the clid appears to be 1 instead of 1238547719 
  ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID "<< atrc->clID() );

// use 1238547719 to get the IOVRange from DB (
//    IOVRange range;
//    sc = m_IOVSvc->getRange(1238547719, m_tubeFolder, range);
//    if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange failed" << endmsg;}
//    if( m_debug ) *m_log << MSG::DEBUG <<"CondAttrListCollection IOVRange "<<range<<endmsg;

//    IOVRange range2;
//    sc = m_IOVSvc->setRange(1221928754, m_tubeDataLocation, range);
//    if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::setRange failed" << endmsg;}
//    sc = m_IOVSvc->getRange(1221928754, m_tubeDataLocation, range2);
//    if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange2 failed" << endmsg;}
//    if( m_debug ) *m_log << MSG::DEBUG <<"MdtTubeCalibContainerCollection new IOVRange "<<range2<<endmsg;

  // finally record MdtTubeCalibContainerCollection
  sc = detStore()->record( m_tubeData, m_tubeDataLocation );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "m_tubeData not recorded" );
  }

  SG::DataProxy *proxy = detStore()->proxy(ClassID_traits<MdtTubeCalibContainerCollection>::ID(), m_tubeDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class MdtTubeCalibContainerCollection" );
    return StatusCode::FAILURE;
  }
  
  IAddressProvider *addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG( "set address provider for MdtTubeCalibContainerCollection" );
  
  m_IOVDbSvc->dropObject(m_tubeFolder, true);
  return  sc; 
}  //end MdtCalibDbCoolStrTool::loadTube

// Build the transient structure in StoreGate and load default RT function read from a text file
// In principle, a list of text files can be specified in the job options, and each text file
// may potentially contain multiple RT functions.  However, only the first valid RT function in 
// the first text file is used as the default for all chambers.
StatusCode MdtCalibDbCoolStrTool::defaultRt() {
  ATH_MSG_DEBUG( "MdtCalibDbCoolStrTool::defaultRt START");
  
  m_rtData = NULL;
  // Try to retrieve MdtTubeCalibContainerCollection from StoreGate
  if (detStore()->contains<MdtRtRelationCollection>(m_rtDataLocation)) {
    StatusCode sc = detStore()->retrieve(m_rtData, m_rtDataLocation);
    if(!sc.isSuccess()) m_rtData=NULL;
  }
  // If no MdtTubeCalibContainerCollection in StoreGate then create it.
  if(m_rtData==NULL) {
    ATH_MSG_DEBUG( "Creating new MdtRtRelationCollection");
    m_rtData = new MdtRtRelationCollection(); 
  }
 
  //Resize Rt collection according to the number of regions
  m_rtData->resize(m_regionSvc->numberOfRegions());
  ATH_MSG_DEBUG( "Created new MdtRtRelationCollection size " << m_rtData->size() );
  
  // Check that an RT text file has been specified in job options
  std::vector<std::string>::const_iterator it     = m_RTfileNames.value().begin();
  std::vector<std::string>::const_iterator it_end = m_RTfileNames.value().end();  
  if (it == it_end ) {
    ATH_MSG_FATAL( "No input RT file declared in jobOptions");
    return StatusCode::FAILURE;
  } else if (it_end-it>1) {
    ATH_MSG_WARNING( "Only first RT file declared in jobOptions will be used");
  }

  // Open the Ascii file with the RT relations
  std::string fileName = PathResolver::find_file(it->c_str(),"DATAPATH");
  if(fileName.length() == 0) {
    ATH_MSG_ERROR( "RT Ascii file \"" <<  it->c_str() << "\" not found" );
  }
  std::ifstream inputFile( fileName.c_str() );
  if( !inputFile ) {
    ATH_MSG_ERROR( "Unable to open RT Ascii file: " << fileName.c_str() );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Opened RT Ascii file: " <<  fileName.c_str() );
  }
 
  // Read the RTs from the text file
  MuonCalib::RtDataFromFile rts;
  rts.read(inputFile);
  ATH_MSG_VERBOSE( "File contains " << rts.nRts() << " RT relations " );

  // Loop over all RTs in the file (but the default file only has 1 RT)
  // Use the first valid RT found in the file as the default for all chambers.
  for(unsigned int n=0;n<rts.nRts();++n) {
    MuonCalib::RtDataFromFile::RtRelation *rt = rts.getRt( n );

    // This check for regionId is obselete because we don't use the regionId from the text file anymore
    //    unsigned int regionId = rt->regionId();  
    //    if( regionId >= m_rtData->size() ){
    //      ATH_MSG_WARNING( "regionHash out of range: " << regionId << " max " << m_rtData->size() );
    //      delete rt;
    //      continue;
    //    }
      
    const MuonCalib::RtDataFromFile::RtRelation::DataVec &times = rt->times();
    const MuonCalib::RtDataFromFile::RtRelation::DataVec &radii = rt->radii();
    const MuonCalib::RtDataFromFile::RtRelation::DataVec &reso  = rt->resolution();
       
    // check if rt contains data, at least two points on the rt are required
    if( times.size() < 2 ) {
      ATH_MSG_ERROR( " defaultRt rt table has too few entries" );
      continue;
    }

    // check if all tables have same size
    if( times.size() != radii.size() || times.size() != reso.size() ) {
      ATH_MSG_ERROR( "defaultRt rt table size mismatch " );
      continue;
    }

    // check for negative time bins, i.e. decreasing time value with radius 
    double t_min    = times[0];
    double bin_size = times[1]-t_min;
    if( bin_size <= 0 ) {
      ATH_MSG_ERROR( "defaultRt rt table negative binsize " );
      continue;
    }

    // create a vector to hold the r values, 
    // we need two extra fields to store t_min and bin_size
    MuonCalib::CalibFunc::ParVec rtPars;
    rtPars.push_back( t_min );
    rtPars.push_back( bin_size );

    // copy r values into vector
    rtPars.insert( rtPars.end(), radii.begin(), radii.end() );

    ATH_MSG_DEBUG( "defaultRt new MuonCalib::IRtRelation" );

    MuonCalib::CalibFunc::ParVec resoPars;
    resoPars.push_back( t_min );
    resoPars.push_back( bin_size );

    // copy r values into vector
    resoPars.insert( resoPars.end(), reso.begin(), reso.end() );

    ATH_MSG_DEBUG( "defaultRt new MuonCalib::IRtResolution" );

    // create RT and resolution "I" objects
    MuonCalib::IRtRelation *rtRel = 
      MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );
    if( !rtRel ) ATH_MSG_WARNING( "ERROR creating RtRelationLookUp " );

    MuonCalib::IRtResolution *resoRel = 
      MuonCalib::MdtCalibrationFactory::createRtResolution( "RtResolutionLookUp", resoPars );
    if( !resoRel ) ATH_MSG_WARNING( "ERROR creating RtResolutionLookUp " );

    // if either RT and resolution are not OK then delete both and try next RT in file
    if( !resoRel || !rtRel ) {
      if(resoRel) delete resoRel;
      if(rtRel) delete rtRel;
      delete rt;
      continue;
    }

    //Since the same RT is loaded for all chambers you might be tempted to create it once
    //and simply store the same pointer in m_rtData for all regions.
    //However it seems that when StoreGate clears m_rtData (which will happen in LoadRt
    //by detStore()->removeDataAndProxy) it will crash unless there are unique pointers/objects 
    //for rtRel, resoRel, and MdtRtRelation

    //Loop over RT regions and store the default RT in each
    for(unsigned int iregion=0; iregion<m_rtData->size(); iregion++) {
      ATH_MSG_DEBUG( "Inserting default Rt for region "<<iregion);
      // create RT and resolution "I" objects, again, so they can all be cleanly deleted later.
      rtRel   = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );
      resoRel = MuonCalib::MdtCalibrationFactory::createRtResolution( "RtResolutionLookUp", resoPars );
      (*m_rtData)[iregion] = new MuonCalib::MdtRtRelation( rtRel, resoRel, 0. );
    }   //end loop over RT regions

    //if VERBOSE enabled print out RT function
    if( msgLvl(MSG::VERBOSE) ) {
      int npoints = rtRel->nPar()-2;
      ATH_MSG_VERBOSE( "defaultRt npoints from rtRel="<< npoints );
      
      for( int ipt=0;ipt<npoints;++ipt ){
	double t = t_min + ipt*bin_size;
	ATH_MSG_VERBOSE(" "<<ipt<<" "<<t<<" "<< rtRel->radius(t)<<" "<< resoRel->resolution(t));
      }
    }
    delete rt; 
    break;            //only need the first good RT from the text file
  }  //end loop over RTs in file

  return StatusCode::SUCCESS;
}  //end MdtCalibDbCoolStrTool::defaultRt

// Read the RTs from the DB and loads them in StoreGate (in m_rtData)
StatusCode MdtCalibDbCoolStrTool::loadRt(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_DEBUG( "Load Rt Calibration from COOL" );

  //tr-relation creators
  //  MuonCalib::RtResolutionFromPoints res_from_points;  //never used
  MuonCalib::RtFromPoints rt_fromPoints;

  // retrieve and remove the old RT collection
  StatusCode sc = detStore()->retrieve( m_rtData, m_rtDataLocation );
  if(sc.isSuccess())  {
    ATH_MSG_DEBUG( "Rt Collection found m_rtData=" << m_rtData );
    sc = detStore()->removeDataAndProxy( m_rtData );
    if(sc.isSuccess())  {
      ATH_MSG_DEBUG( "Rt Collection at " << m_rtData << " removed ");
    }
  }

  // reinitialize the MdtRtRelationCollection
  ATH_CHECK( defaultRt() );
                                                                                
  // retrieve the collection of strings read out from the DB
  const CondAttrListCollection *atrc = nullptr;
  ATH_CHECK( detStore()->retrieve(atrc,m_rtFolder) );

  // unpack the strings in the collection and update the rtData
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList &atr=itr->second;
    bool rt_ts_applied = (atr["tech"].data<int>() & TIME_SLEWING_CORRECTION_APPLIED);
    std::string header="",payload="",trailer="";
    // if BLOB data
    if (atr["data"].specification().type() == typeid(coral::Blob)){
      ATH_MSG_VERBOSE( "Data load is a blob. Uncompressing");
      if(!uncompressInMyBuffer(atr["data"].data<coral::Blob>())) {
	ATH_MSG_FATAL( "Cannot uncompress buffer" );
	return StatusCode::FAILURE;
      }
      std::string istr(reinterpret_cast<char*>(m_decompression_buffer));
      ATH_CHECK( extractString(istr, header, "\n") );
      ATH_CHECK( extractString(istr, payload, "\n") );
      if( istr.size() ) ATH_CHECK( extractString(istr, trailer, "\n") );
    } else {      // else CLOB data
      std::string data;
      data = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
      ATH_MSG_VERBOSE( "Data load is " << data );
      // interpret as string stream
      std::string istr(data.c_str());
      ATH_CHECK( extractString(istr, header, " ") );
      ATH_CHECK( extractString(istr, payload, " ") );
      if( istr.size() ) ATH_CHECK( extractString(istr, trailer, " ") );
    }                                                                       
    ATH_MSG_VERBOSE( "Read header:" << header << " payload:" << payload << " trailer:" << trailer );

    // the header contains the muonfixedid rather than the hash
    char *parameters = new char [header.size()+1];
    strncpy(parameters, header.c_str(), header.size()+1);
    parameters[header.size()]='\0';
    unsigned int regionId, npoints(0);
    Identifier  athenaId; 
    char *pch = strtok(parameters," _,");
    regionId = atoi(pch);
    //Long ago the hash was put in the RT file header, but now (2016) 
    //the muonfixedid of the chamber is in the header.  Hence the "if" below will always be true.    
    if(regionId>2500) {
      MuonFixedId id(regionId);
      athenaId = m_idToFixedIdTool->fixedIdToId(id);
      // If using chamber RTs skip RTs for ML2 -- use ML1 RT for entire chamber
      if( m_regionSvc->RegionType()==ONEPERCHAMBER && m_mdtIdHelper->multilayer(athenaId)==2 ) {
        ATH_MSG_VERBOSE("MdtCalibDbCoolStrTool::loadRt Ignore ML2 RT for region "<<regionId<<" "<<
			m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(athenaId))<<"_"<<
			m_mdtIdHelper->stationPhi(athenaId)<<"_"<<m_mdtIdHelper->stationEta(athenaId)<<
			" ML"<<m_mdtIdHelper->multilayer(athenaId));  //TEMP
	continue;
      }
      IdentifierHash hash;  //chamber hash
      IdContext idCont = m_mdtIdHelper->module_context();
      idCont = m_mdtIdHelper->module_context();
      m_mdtIdHelper->get_hash( athenaId, hash, &idCont );
      ATH_MSG_VERBOSE( "Fixed region Id "<<regionId<<" converted into athena Id "<<athenaId <<" and then into hash "<<hash);
      regionId = hash;      //reset regionId to chamber hash
    }
    // extract npoints in RT function
    pch = strtok (NULL, "_,");
    npoints = atoi(pch);
    delete [] parameters;                                                                          
                                                                                
    MuonCalib::CalibFunc::ParVec rtPars;
    MuonCalib::CalibFunc::ParVec resoPars;

    SamplePoint tr_point, ts_point;  //pairs of numbers; tr = (time,radius); ts = (time,sigma)  [sigma=resolution]
    std::vector<SamplePoint> tr_points(0), ts_points(0);  //all the points in time,radius [RT] and time,sigma [resolution func]
    float multilayer_tmax_diff(-9e9);

    char *RTPar= new char [payload.size()+1];
    strncpy(RTPar, payload.c_str(), payload.size()+1);
    RTPar[payload.size()]='\0';   //terminate string (not sure this is really needed because payload.c_str() should be terminated in \0)
    char *pch1 = strtok (RTPar,",");
    unsigned int n=0;
    //loop over RT function payload (triplets of radius,time,sigma(=resolution) )
    for( int k=1; pch1!=NULL && n<=npoints; pch1=strtok(NULL,", "), k++ ) {
      if(k==1) {    //radius point
	float radius = atof(pch1);       
	if( m_rtShift != 0. ) {
	  float oldradius = radius;
	  float rshift = m_rtShift*1.87652e-2*radius*(radius-14.6);
	  radius = oldradius + rshift;
	  ATH_MSG_DEBUG( "DEFORM RT: old radius " << oldradius << " new radius " 
			  << radius << " shift " << rshift << " max shift " << m_rtShift );
	}
       
	if( m_rtScale !=1. ) {
	  radius = radius*m_rtScale;
	  ATH_MSG_DEBUG( "DEFORM RT: old radius " << radius << " new radius " 
			  << radius << " scale factor " << m_rtScale );
	}
       
	tr_point.set_x2(radius);
      } else if(k==2) {     //time
	float time= atof(pch1);
	tr_point.set_x1(time);
	ts_point.set_x1(time);
      } else if(k==3) {     //sigma or resolution
	float sigma= atof(pch1);
	ts_point.set_x2(sigma);
	ts_point.set_error(1.0);
	tr_point.set_error(1.0);
	if(tr_point.x2()<-99) {    //if radius is < -99 then treat time as ML Tmax difference
	  //		std::cout<<"XXxxXX Multilayer t_max diff found!"<<tr_point.x1() <<std::endl;
	  multilayer_tmax_diff = tr_point.x1();
	} else if(n==0 || (tr_points[n-1].x1()<tr_point.x1() && tr_points[n-1].x2()<tr_point.x2())) {
	  tr_points.push_back(tr_point);	       
	  ts_points.push_back(ts_point);
	  n++;  //count points in RT
	}
	k=0;
      }
    }   //end loop over RT function payload (triplets of radius,time,resolution)
    delete [] RTPar;

    //Must have at least 3 points to have a valid RT
    if(ts_points.size()<3) {
      ATH_MSG_FATAL( "Rt relation broken!");
      ATH_MSG_FATAL( "file='"<<atr["file"].data<std::string>()<<"'");
      ATH_MSG_FATAL( "header='"<<header<<"'");
      return StatusCode::FAILURE;
    }

    if(rt_ts_applied != m_TimeSlewingCorrection) {
      float sign(rt_ts_applied ? -1.0 : 1.0);
      float slice_width = 14.6/static_cast<float>(m_MeanCorrectionVsR.size());
      for(std::vector<SamplePoint>::iterator it=tr_points.begin(); it!=tr_points.end(); it++) {
	int slice_number=static_cast<int>(std::floor(it->x2()/slice_width));		
	if (slice_number<0)
	  slice_number=0;
	if (slice_number >= static_cast<int>(m_MeanCorrectionVsR.size()))
	  slice_number = static_cast<int>(m_MeanCorrectionVsR.size()) - 1;
	it->set_x1(it->x1() + sign * m_MeanCorrectionVsR[slice_number]);	
      }
    }
    
    //Create resolution function from ts_points
    MuonCalib::IRtResolution *reso = getRtResolutionInterpolation(ts_points);
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Resolution points :");
      for(std::vector<SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++) {
	ATH_MSG_DEBUG( it->x1()<<"|"<<it->x2()<<"|"<<it->error());
      }
	
      ATH_MSG_DEBUG( "Resolution parameters :");
      for(unsigned int i=0; i<reso->nPar(); i++) {
	ATH_MSG_DEBUG(  i<<" "<<reso->par(i) );
      }
    }

    //Create RT function from tr_points and load RT and resolution functions 
    try {
      MuonCalib::IRtRelation *rt = new RtRelationLookUp(rt_fromPoints.getRtRelationLookUp(tr_points));
      if( reso && rt ){
	if(regionId>=m_rtData->size()) {
	  delete reso; delete rt;
	  ATH_MSG_WARNING( "Illegal regionId "<< regionId );
	} else { 
	  if (rt->par(1)==0.) {
	    ATH_MSG_WARNING( "Bin size is 0");
	    for(std::vector<SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++)
	      ATH_MSG_WARNING( it->x1() << " " <<it->x2() <<" "<< it->error() );
	  }
	  //Save ML difference if it is available
	  if(multilayer_tmax_diff>-8e8) {
	    rt->SetTmaxDiff(multilayer_tmax_diff);
	  }
	  //Store RT and resolution functions for this region
	  if( m_regionSvc->RegionType() == ONERT ) {
	    (*m_rtData)[0] = new MuonCalib::MdtRtRelation( rt, reso, 0.);
	    break;   // only read one RT from COOL for ONERT option.
	  // If doing ML2 RTs, and this is a ML2 RT function then add it to the end of m_rtData
	  } else if( m_regionSvc->RegionType()==ONEPERMULTILAYER && m_mdtIdHelper->multilayer(athenaId)==2 ) {
	    ATH_MSG_VERBOSE("MdtCalibDbCoolStrTool::loadRt Load ML2 RT for region "<<regionId<<" "<<
			 m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(athenaId))<<"_"<<
			 m_mdtIdHelper->stationPhi(athenaId)<<"_"<<m_mdtIdHelper->stationEta(athenaId)<<
			 " ML"<<m_mdtIdHelper->multilayer(athenaId));
	    (*m_rtData).push_back(new MuonCalib::MdtRtRelation( rt, reso, 0.));
	    IdentifierHash mlHash;
	    m_mdtIdHelper->get_detectorElement_hash( athenaId, mlHash ); 
	    m_regionSvc->setRegionHash(mlHash);
	  } else {   //store RT for chamber or ML1 if doing ONEPERMULTILAYER
	    (*m_rtData)[regionId] = new MuonCalib::MdtRtRelation( rt, reso, 0.);
	  }
	}      //end else regionId is OK
      }        //end if reso && rt
    }          //end try
    catch (int i) {
      ATH_MSG_FATAL( "Error in creating rt-relation!" );
      ATH_MSG_FATAL( "npoints="<<tr_points.size());
      ATH_MSG_FATAL( "Offending input: header=" << header );
      ATH_MSG_FATAL( "Offending input: payload=" << payload );
      return StatusCode::FAILURE;
    }
    
  }  //end loop over itr (strings read from COOL)
  ATH_MSG_INFO("MdtCalibDbCoolStrTool::loadRt Read "<<m_regionSvc->numberOfRegions()<<"RTs from COOL");

//   IOVRange range;
   // sc = m_IOVSvc->getRange(1238547719, m_rtFolder, range);
   // if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange failed" << endmsg;}
   // if( m_debug ) *m_log << MSG::DEBUG <<"CondAttrListCollection IOVRange "<<range<<endmsg;
                                                                                
   // IOVRange range2;
   // sc = m_IOVSvc->setRange(1270996316, m_rtDataLocation, range);
   // if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::setRange failed" << endmsg;}
   // sc = m_IOVSvc->getRange(1270996316, m_rtDataLocation, range2);
   // if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange2 failed" << endmsg;}
   // if( m_debug ) *m_log << MSG::DEBUG <<"MdtRtRelationCollection new IOVRange "<<range2<<endmsg;
                                                                                
   // finally record MdtRtRelationCollection in StoreGate
  sc = detStore()->record( m_rtData, m_rtDataLocation );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "m_tubeData not recorded" );
  }
  
  SG::DataProxy *proxy = detStore()->proxy(ClassID_traits<MdtRtRelationCollection>::ID(), m_rtDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class MdtRtRelationCollection" );
    return StatusCode::FAILURE;
  }
                                                                                
  IAddressProvider *addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG( "set address provider for MdtRtRelationCollection" );
                                                                                
  m_IOVDbSvc->dropObject(m_rtFolder, true);

  return  StatusCode::SUCCESS; 
}  //end MdtCalibDbCoolStrTool::loadRt

// build the transient structure and load some defaults for T0s
StatusCode MdtCalibDbCoolStrTool::defaultT0s() {
  m_tubeData = NULL;
  // Try to retrieve MdtTubeCalibContainerCollection from StoreGate
  if (detStore()->contains<MdtTubeCalibContainerCollection>(m_tubeDataLocation)) {
    StatusCode sc = detStore()->retrieve(m_tubeData, m_tubeDataLocation);
    if(!sc.isSuccess()) m_tubeData=NULL;
  }
  // If no MdtTubeCalibContainerCollection in StoreGate then create it.
  if(m_tubeData==NULL) {
    ATH_MSG_INFO( "Creating new MdtTubeCalibContainerCollection");
    m_tubeData = new MdtTubeCalibContainerCollection();
  }
  //Resize tube container according to the number of chambers
  m_tubeData->resize( m_mdtIdHelper->module_hash_max() );
  ATH_MSG_DEBUG( " Created new MdtTubeCalibContainerCollection size " << m_tubeData->size() );

  // Inverse of wire propagation speed
  float inversePropSpeed = 1./(299.792458*m_prop_beta);

  //loop over modules (MDT chambers) and create an MdtTubeContainer for each
  MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
  MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
  for(; it!=it_end;++it ) {
    MuonCalib::MdtTubeCalibContainer *tubes=0;
    //create an MdtTubeContainer
    tubes = buildMdtTubeCalibContainer(*it);

    // is tubes ever 0?  how could that happen?
    if(tubes) {
      std::string rName=tubes->regionKey();
      double t0 = m_defaultT0;

      int nml=tubes->numMultilayers();
      int nlayers=tubes->numLayers();
      int ntubes=tubes->numTubes();
      int size = nml*nlayers*ntubes;
      ATH_MSG_VERBOSE( "Adding chamber " << m_mdtIdHelper->print_to_string(*it) );
      ATH_MSG_VERBOSE( " size " << size << " ml " << nml << " l " << nlayers << " t " 
		       << ntubes << " address " << tubes );
      for( int ml=0;ml<nml;++ml ){
	for( int l=0;l<nlayers;++l ){
	  for( int t=0;t<ntubes;++t ){
	    MuonCalib::MdtTubeCalibContainer::SingleTubeCalib data;
	    data.t0 = t0;
	    data.adcCal = 1.;
	    data.inversePropSpeed = inversePropSpeed;
	    tubes->setCalib( ml,l,t,data );
	  }
	}
      }
    }  //end loop over chambers (modules)
    ATH_MSG_VERBOSE( " set t0's done " );
    IdentifierHash hash;
    IdContext idCont = m_mdtIdHelper->module_context();
    m_mdtIdHelper->get_hash( *it, hash, &idCont );

    if( hash < m_tubeData->size() ){
      (*m_tubeData)[hash] = tubes;
      ATH_MSG_VERBOSE( " adding tubes at " << hash << " current size " << m_tubeData->size() );
//write out string for chamberlist
      if(tubes) {
	int nml     = tubes->numMultilayers();
	int nlayers = tubes->numLayers();
	int ntubes  = tubes->numTubes();	
	ATH_MSG_VERBOSE( "CHAMBERLIST: " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(*it)) << " " << m_mdtIdHelper->stationEta(*it) << " " << m_mdtIdHelper->stationPhi(*it) << " " << nml*nlayers*ntubes << " " << nml << " " << nlayers << " " << ntubes << " dummy " << hash );
      }
    } else {
      if(tubes) delete tubes;
      ATH_MSG_WARNING( " HashId out of range " << hash << " max " << m_tubeData->size() );
    }
  }
  ATH_MSG_DEBUG( " Done defaultT0s " << m_tubeData->size() );

  return StatusCode::SUCCESS;
}  //end MdtCalibDbCoolStrTool::defaultT0s

// Build a MuonCalib::MdtTubeCalibContainer for a given Identifier
MuonCalib::MdtTubeCalibContainer* MdtCalibDbCoolStrTool::buildMdtTubeCalibContainer(const Identifier &id) {    
  MuonCalib::MdtTubeCalibContainer *tubes = 0;

  const MuonGM::MdtReadoutElement *detEl = m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID(id,1,1,1) );
  const MuonGM::MdtReadoutElement *detEl2 = 0;
  if (m_mdtIdHelper->numberOfMultilayers(id) == 2){
    detEl2 = m_detMgr->getMdtReadoutElement(m_mdtIdHelper->channelID(id,2,1,1) );
  } else {
    ATH_MSG_VERBOSE( "A single multilayer for this station " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id))<<","<< m_mdtIdHelper->stationPhi(id) <<","<< m_mdtIdHelper->stationEta(id) );
  }

  ATH_MSG_VERBOSE( " new det el " << detEl );
  
  if( !detEl ){ 
    ATH_MSG_INFO( "Ignoring nonexistant station in calibration DB: " << m_mdtIdHelper->print_to_string(id) );
  } else {
    int nml = 2;
    if( !detEl2 ) nml = 1;
    
    int nlayers = detEl->getNLayers();
    if( detEl2 && detEl2->getNLayers() > nlayers ){
      ATH_MSG_DEBUG( "Second multilayer has more layers " << detEl2->getNLayers() << " then first " << nlayers );
      nlayers = detEl2->getNLayers();
    }

    int ntubes = detEl->getNtubesperlayer();
    if( detEl2 && detEl2->getNtubesperlayer() > ntubes ){
      ATH_MSG_DEBUG( "Second multilayer has more tubes " << detEl2->getNtubesperlayer() << " then first " << ntubes );
      ntubes = detEl2->getNtubesperlayer();
    }

    // build the region name in the format STATION_ETA_PHI
    std::string rName;

    int stName = m_mdtIdHelper->stationName(id);
    int stPhi  = m_mdtIdHelper->stationPhi(id);
    int stEta  = m_mdtIdHelper->stationEta(id);
  
    std::string separator("_");
    MuonCalib::ToString ts;
    rName = m_mdtIdHelper->stationNameString(stName);
    rName += separator + ts( stPhi ) + separator + ts( stEta );
    tubes = new MuonCalib::MdtTubeCalibContainer( rName,nml, nlayers, ntubes );
  }

  return tubes;
}  //end MdtCalibDbCoolStrTool::buildMdtTubeCalibContainer


  /*
//wrapper function for the zlib uncompress, that automatically creates or increases the buffer if needed.    
inline bool MdtCalibDbCoolStrTool::uncompressInMyBuffer(const coral::Blob &blob) {
  //m_decompression_buffer holds the decompressed data from the BLOB
  if( m_decompression_buffer == NULL ) {
    m_buffer_length = 50000;
    m_decompression_buffer = new Bytef[m_buffer_length];
  }
  uLongf actual_length;	
  // Loop indefinately increasing buffer size until the uncompress succeeds or another error occurs
  while(1) {
    actual_length = m_buffer_length;
    int res(uncompress(m_decompression_buffer, &actual_length, reinterpret_cast<const Bytef *>(blob.startingAddress()), static_cast<uLongf>(blob.size())));
    if (res == Z_OK) break;
    //double buffer size if it was not big enough
    if( res == Z_BUF_ERROR) {
      m_buffer_length *= 2;
      ATH_MSG_VERBOSE( "Increasing buffer to " << m_buffer_length);
      delete [] m_decompression_buffer;
      m_decompression_buffer = new Bytef[m_buffer_length];
      continue;
    }
    //something else is wrong
    return false;
  }
  // increase buffer if it is not big enough
  if (actual_length >= m_buffer_length) {
    Bytef *old_buffer = m_decompression_buffer;
    size_t old_length = m_buffer_length;
    m_buffer_length *= 2;
    m_decompression_buffer = new Bytef[m_buffer_length];
    memcpy(m_decompression_buffer, old_buffer, old_length);
    delete [] old_buffer;
  }
  //append 0 to terminate string
  m_decompression_buffer[actual_length] = 0;

  return true;
}  //end MdtCalibDbCoolStrTool::uncompressInMyBuffer
  */
inline bool MdtCalibDbCoolStrTool::uncompressInMyBuffer(const coral::Blob &blob) {
  if (!m_decompression_buffer) {
    m_buffer_length= 50000;
    m_decompression_buffer = new Bytef[m_buffer_length];
  }
  uLongf actual_length;	
  while(1) {
    actual_length=m_buffer_length;
    int res(uncompress(m_decompression_buffer, &actual_length, reinterpret_cast<const Bytef *>(blob.startingAddress()), static_cast<uLongf>(blob.size())));
    if (res == Z_OK) break;
    //double buffer if it was not big enough
    if( res == Z_BUF_ERROR) {
      m_buffer_length*=2;
      ATH_MSG_VERBOSE(  "Increasing buffer to " << m_buffer_length);
      delete [] m_decompression_buffer;
      m_decompression_buffer = new Bytef[m_buffer_length];
      continue;
    }
    //something else is wrong
    return false;
  }
  //append 0 to terminate string, increase buffer if it is not big enough
  if (actual_length >= m_buffer_length)	{
    Bytef * old_buffer=m_decompression_buffer;
    size_t old_length=m_buffer_length;
    m_buffer_length*=2;
    m_decompression_buffer = new Bytef[m_buffer_length];
    memcpy(m_decompression_buffer, old_buffer, old_length);
    delete [] old_buffer;
  }
  m_decompression_buffer[actual_length]=0;
  return true;
}  //end MdtCalibDbCoolStrTool::uncompressInMyBuffer

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::
//:: METHOD getRtResolutionInterpolation ::
//:::::::::::::::::::::::::::::::::::::::::
inline RtResolutionLookUp* MdtCalibDbCoolStrTool::getRtResolutionInterpolation( const std::vector<SamplePoint> &sample_points) {

///////////////
// VARIABLES //
///////////////
  Double_t *x = new Double_t[sample_points.size()];
  Double_t *y = new Double_t[sample_points.size()];
	
  for (unsigned int i=0; i<sample_points.size(); i++) {
    x[i] = sample_points[i].x1();
    y[i] = sample_points[i].x2();
  }
  TSpline3 sp("Rt Res Tmp", x, y, sample_points.size());
///////////////////////////////////////////////////////////////////
// CREATE AN RtRelationLookUp OBJECT WITH THE CORRECT PARAMETERS //
///////////////////////////////////////////////////////////////////
  unsigned int nb_points(100);
  std::vector<double> res_param(nb_points+2); // r-t parameters
  Double_t bin_width=(x[sample_points.size() -1] - x[0]) / static_cast<Double_t>(nb_points);
	
  res_param[0] = x[0];
  res_param[1] = bin_width;
  for (unsigned int k=0; k<nb_points; k++) {
    Double_t xx = x[0] + k*bin_width;
    res_param[k+2] = sp.Eval(xx);
    if (std::isnan(res_param[k+2])) {
      TFile outf("kacke.root", "RECREATE");
      sp.Write("kacke");
      exit(99);
    }
  }
  delete [] x;
  delete [] y;
  return new RtResolutionLookUp(res_param);
}  //end MdtCalibDbCoolStrTool::getRtResolutionInterpolation

inline StatusCode MdtCalibDbCoolStrTool::extractString(std::string &input, std::string &output, std::string separator) {
  unsigned long int pos = 0;
  std::string::size_type start = input.find_first_not_of(separator.c_str(),pos);
  if(start == std::string::npos) {
    ATH_MSG_ERROR("MdtCalibDbCoolStrTool::extractString: Cannot extract string in a proper way!");
    return StatusCode::FAILURE;
  }
  std::string::size_type stop = input.find_first_of(separator.c_str(),start+1);
  if (stop == std::string::npos) stop = input.size();
  output = input.substr(start,stop-start);
  input.erase(pos,stop-pos);

  return StatusCode::SUCCESS;
}  //end MdtCalibDbCoolStrTool::extractString

}  //namespace MuonCalib
