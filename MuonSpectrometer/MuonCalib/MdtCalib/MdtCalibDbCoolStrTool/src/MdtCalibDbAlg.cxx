/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibDbCoolStrTool/MdtCalibDbAlg.h"

//includes from MdtCalibDbCoolStrTool.cxx

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
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

//#include "MdtCalibDbCoolStrTool/MdtCalibDbCoolStrTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TSpline.h"
#include "TFile.h"

//like MdtCalibrationDbSvc
//for corData in loadRt
#include "MdtCalibData/MdtCorFuncSetCollection.h"
#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/BFieldCorFunc.h"
#include "MdtCalibData/WireSagCorFunc.h"
#include "MdtCalibData/MdtSlewCorFuncHardcoded.h"
#include "MdtCalibData/CalibFunc.h"


MdtCalibDbAlg::MdtCalibDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_condSvc{"CondSvc", name},
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),
  m_regionSvc("MdtCalibrationRegionSvc", name),
  m_rtFolder("/MDT/RTBLOB"),
  m_tubeFolder("/MDT/T0BLOB"),
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
  m_decompression_buffer(NULL),
  m_readKeyRt("/MDT/RTBLOB"),
  m_readKeyTube("/MDT/T0BLOB"),
  m_writeKeyRt("MdtRtRelationCollection"),
  m_writeKeyTube("MdtTubeCalibContainerCollection"),
  m_writeKeyCor("MdtCorFuncSetCollection")
{

  //declareInterface< IMdtCalibDBTool >(this);

  //Db Folders
  declareProperty("TubeFolder",m_tubeFolder,"DB folder containing the tube constants");
  declareProperty("RtFolder",m_rtFolder,"DB folder containing the RT calibrations");
  
  //Properties to deform the t0 and rt relationship
  declareProperty("T0Shift",m_t0Shift,"for simulation: common shift of all T0s, in ns");
  declareProperty("T0Spread",m_t0Spread,"for simulation: sigma for random smeraing of T0s, in ns");
  declareProperty("RTShift",m_rtShift,"for simulations: maximum RT distortion, in mm");
  declareProperty("RTScale",m_rtScale,"for simulations: a muliplicitive scale to the drift r");

  //is this the simplest way to initialize a list?
  std::ostringstream myse;
  std::vector<std::string> myord;
  myse << "DC2_rt_default.dat";
  myord.push_back(myse.str());

  declareProperty("RT_InputFiles",m_RTfileNames=myord,"single input ascii file for default RT to be applied in absence of DB information");

  //defaultT0, used for tubes not found in DB
  declareProperty("defaultT0",m_defaultT0,"default T0 value to be used in absence of DB information");
  declareProperty("TimeSlewingCorrection", m_TimeSlewingCorrection);
  declareProperty("MeanCorrectionVsR", m_MeanCorrectionVsR);
  declareProperty("UseMLRt", m_UseMLRt,"Enable use of ML-RTs from COOL");
  declareProperty("PropagationSpeedBeta", m_prop_beta);

  //like MdtCalibrationDbSvc
  //for corData in loadRt
  declareProperty("CreateBFieldFunctions", m_create_b_field_function = false,
		  "If set to true, the B-field correction functions are initialized for each rt-relation that is loaded.");
  declareProperty("CreateWireSagFunctions", m_createWireSagFunction = false,
		  "If set to true, the wire sag correction functions are initialized for each rt-relation that is loaded.");
  declareProperty("CreateSlewingFunctions", m_createSlewingFunction = false,
		  "If set to true, the slewing correction functions are initialized for each rt-relation that is loaded.");

}

StatusCode MdtCalibDbAlg::initialize(){

  ATH_MSG_DEBUG( "initialize " << name() );
  ATH_CHECK(m_condSvc.retrieve());

  //if timeslew correction vector m_MeanCorrectionVsR has non-zero size then set
  //m_TsCorrectionT0=m_MeanCorrectionVsR[0] and subtract this each value in the vector.
  if(m_MeanCorrectionVsR.size()) {
    m_TsCorrectionT0 = m_MeanCorrectionVsR[0];
    for(std::vector<float>::iterator it=m_MeanCorrectionVsR.begin(); it!=m_MeanCorrectionVsR.end(); it++) {
      (*it) -= m_TsCorrectionT0;
    }
  }

  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );
  ATH_CHECK( detStore()->retrieve( m_detMgr ) );
  ATH_CHECK( m_regionSvc.retrieve() );
  ATH_CHECK( m_idToFixedIdTool.retrieve() );

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

  ATH_CHECK(m_readKeyRt.initialize());
  ATH_CHECK(m_readKeyTube.initialize());
  ATH_CHECK(m_writeKeyRt.initialize());
  ATH_CHECK(m_writeKeyTube.initialize());
  ATH_CHECK(m_writeKeyCor.initialize());

  if(m_condSvc->regHandle(this, m_writeKeyRt).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyRt.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  if(m_condSvc->regHandle(this, m_writeKeyTube).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyTube.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  if(m_condSvc->regHandle(this, m_writeKeyCor).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyCor.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MdtCalibDbAlg::execute(){
  
  ATH_MSG_DEBUG( "execute " << name() );  

  if ( loadRt().isFailure() ) {
    ATH_MSG_FATAL("loadRt().isFailure()");
    return StatusCode::FAILURE;
  }

  if ( loadTube().isFailure() ) {
    ATH_MSG_FATAL("loadTube().isFailure()");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode MdtCalibDbAlg::defaultRt(std::unique_ptr<MdtRtRelationCollection>& writeCdoRt){
  ATH_MSG_DEBUG( "defaultRt " << name() );

  if ( writeCdoRt == nullptr ) {
    ATH_MSG_ERROR("writeCdoRt == nullptr");
    return StatusCode::FAILURE; 
  }   

  //Build the transient structure in StoreGate and load default RT function read from a text file
  //In principle, a list of text files can be specified in the job options, and each text file
  //may potentially contain multiple RT functions.  However, only the first valid RT function in
  //the first text file is used as the default for all chambers.

  writeCdoRt->resize(m_regionSvc->numberOfRegions());
  ATH_MSG_DEBUG( "Created new MdtRtRelationCollection size " << writeCdoRt->size() );

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
    //and simply store the same pointer in writeCdoRt for all regions.
    //However it seems that when StoreGate clears writeCdoRt (which will happen in LoadRt
    //by detStore()->removeDataAndProxy) it will crash unless there are unique pointers/objects
    //for rtRel, resoRel, and MdtRtRelation

    //Loop over RT regions and store the default RT in each
    for(unsigned int iregion=0; iregion<writeCdoRt->size(); iregion++) {
      ATH_MSG_DEBUG( "Inserting default Rt for region "<<iregion);
      // create RT and resolution "I" objects, again, so they can all be cleanly deleted later.
      rtRel   = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );
      resoRel = MuonCalib::MdtCalibrationFactory::createRtResolution( "RtResolutionLookUp", resoPars );
      (*writeCdoRt)[iregion] = new MuonCalib::MdtRtRelation( rtRel, resoRel, 0. );
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
}


StatusCode MdtCalibDbAlg::loadRt(){
  ATH_MSG_DEBUG( "loadRt " << name() );

  SG::WriteCondHandle<MdtRtRelationCollection> writeHandleRt{m_writeKeyRt};
  if ( writeHandleRt.isValid() ) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleRt.fullKey() << " is already valid.");
    return StatusCode::SUCCESS; 
  }
  std::unique_ptr<MdtRtRelationCollection> writeCdoRt{std::make_unique<MdtRtRelationCollection>()};

  SG::WriteCondHandle<MdtCorFuncSetCollection> writeHandleCor{m_writeKeyCor};
  if ( writeHandleCor.isValid() ) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleCor.fullKey() << " is already valid.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<MdtCorFuncSetCollection> writeCdoCor{std::make_unique<MdtCorFuncSetCollection>()};

  //like MdtCalibDbCoolStrTool::loadRt()
  //m_rtData is writeCdoRt here
  //atrc is readCdoRt here

  //tr-relation creators
  //MuonCalib::RtResolutionFromPoints res_from_points;//never used
  MuonCalib::RtFromPoints rt_fromPoints;

  if ( defaultRt(writeCdoRt).isFailure() ){
    ATH_MSG_FATAL("defaultRt(writeCdoRt).isFailure()");
    return StatusCode::FAILURE;
  }
  
  //Read Cond Handle  
  SG::ReadCondHandle<CondAttrListCollection> readHandleRt{ m_readKeyRt };
  const CondAttrListCollection* readCdoRt{*readHandleRt}; 
  if ( readCdoRt==nullptr ) {
    ATH_MSG_ERROR("readCdoRt==nullptr");
    return StatusCode::FAILURE; 
  }   
  EventIDRange rangeRt;
  if ( !readHandleRt.range(rangeRt) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleRt.key());
    return StatusCode::FAILURE;
  }  
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleRt.fullKey() << " readCdoRt->size()= " << readCdoRt->size());
  ATH_MSG_INFO("Range of input is " << rangeRt);

  // unpack the strings in the collection and update the writeCdoRt
  CondAttrListCollection::const_iterator itr;
  for (itr = readCdoRt->begin(); itr != readCdoRt->end(); ++itr) {
    const coral::AttributeList &atr=itr->second;
    bool rt_ts_applied = (atr["tech"].data<int>() & MuonCalib::TIME_SLEWING_CORRECTION_APPLIED);
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
      MuonCalib::MuonFixedId id(regionId);
      athenaId = m_idToFixedIdTool->fixedIdToId(id);
      // If using chamber RTs skip RTs for ML2 -- use ML1 RT for entire chamber
      if( m_regionSvc->RegionType()==ONEPERCHAMBER && m_mdtIdHelper->multilayer(athenaId)==2 ) {
        ATH_MSG_VERBOSE("MdtCalibDbAlg::loadRt Ignore ML2 RT for region "<<regionId<<" "<<
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

    MuonCalib::SamplePoint tr_point, ts_point;  //pairs of numbers; tr = (time,radius); ts = (time,sigma)  [sigma=resolution]
    std::vector<MuonCalib::SamplePoint> tr_points(0), ts_points(0);  //all the points in time,radius [RT] and time,sigma [resolution func]
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
      for(std::vector<MuonCalib::SamplePoint>::iterator it=tr_points.begin(); it!=tr_points.end(); it++) {
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
      for(std::vector<MuonCalib::SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++) {
	ATH_MSG_DEBUG( it->x1()<<"|"<<it->x2()<<"|"<<it->error());
      }
	
      ATH_MSG_DEBUG( "Resolution parameters :");
      for(unsigned int i=0; i<reso->nPar(); i++) {
	ATH_MSG_DEBUG(  i<<" "<<reso->par(i) );
      }
    }

    //Create RT function from tr_points and load RT and resolution functions 
    try {
      MuonCalib::IRtRelation *rt = new MuonCalib::RtRelationLookUp(rt_fromPoints.getRtRelationLookUp(tr_points));
      if( reso && rt ){
	if(regionId>=writeCdoRt->size()) {
	  delete reso; delete rt;
	  ATH_MSG_WARNING( "Illegal regionId "<< regionId );
	} else { 
	  if (rt->par(1)==0.) {
	    ATH_MSG_WARNING( "Bin size is 0");
	    for(std::vector<MuonCalib::SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++)
	      ATH_MSG_WARNING( it->x1() << " " <<it->x2() <<" "<< it->error() );
	  }
	  //Save ML difference if it is available
	  if(multilayer_tmax_diff>-8e8) {
	    rt->SetTmaxDiff(multilayer_tmax_diff);
	  }
	  //Store RT and resolution functions for this region
	  if( m_regionSvc->RegionType() == ONERT ) {
	    (*writeCdoRt)[0] = new MuonCalib::MdtRtRelation( rt, reso, 0.);
	    break;   // only read one RT from COOL for ONERT option.
	  // If doing ML2 RTs, and this is a ML2 RT function then add it to the end of writeCdoRt
	  } else if( m_regionSvc->RegionType()==ONEPERMULTILAYER && m_mdtIdHelper->multilayer(athenaId)==2 ) {
	    ATH_MSG_VERBOSE("MdtCalibDbAlg::loadRt Load ML2 RT for region "<<regionId<<" "<<
			 m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(athenaId))<<"_"<<
			 m_mdtIdHelper->stationPhi(athenaId)<<"_"<<m_mdtIdHelper->stationEta(athenaId)<<
			 " ML"<<m_mdtIdHelper->multilayer(athenaId));
	    (*writeCdoRt).push_back(new MuonCalib::MdtRtRelation( rt, reso, 0.));
	    IdentifierHash mlHash;
	    m_mdtIdHelper->get_detectorElement_hash( athenaId, mlHash ); 
	    m_regionSvc->setRegionHash(mlHash);
	  } else {   //store RT for chamber or ML1 if doing ONEPERMULTILAYER
	    (*writeCdoRt)[regionId] = new MuonCalib::MdtRtRelation( rt, reso, 0.);
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
    
  }//end loop over itr (strings read from COOL)
  ATH_MSG_INFO("MdtCalibDbAlg::loadRt Read "<<m_regionSvc->numberOfRegions()<<"RTs from COOL");

  //like MdtCalibrationDbSvc
  //for corData in loadRt

  //Check if doing corrections; return if not
  //if( !m_createSlewingFunction && !m_createWireSagFunction && !m_create_b_field_function ) return StatusCode::SUCCESS;
  //here at least record default as an Alg
  //and then check flags again in the DbSvc

  writeCdoCor->resize(writeCdoRt->size());
  ATH_MSG_DEBUG( "Initializing " << writeCdoCor->size() << " b-field functions" );
  for (unsigned int i=0; i < writeCdoCor->size(); i++) {
    (*writeCdoCor)[i] = new MuonCalib::MdtCorFuncSet();
    if(m_create_b_field_function) initialize_B_correction((*writeCdoCor)[i], (*writeCdoRt)[i]);
    if(m_createWireSagFunction)   initializeSagCorrection((*writeCdoCor)[i]);
    if(m_createSlewingFunction)   (*writeCdoCor)[i]->setSlewing(new MuonCalib::MdtSlewCorFuncHardcoded(MuonCalib::CalibFunc::ParVec()));
  }

  //finally record writeCdo

  if ( writeCdoRt->size()==0 ) {
    ATH_MSG_WARNING("writeCdoRt->size()==0"); 
    return StatusCode::FAILURE;
  }
  if (writeHandleRt.record(rangeRt, std::move(writeCdoRt)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandleRt.key()
		  << " with EventRange " << rangeRt
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandleRt.key()
	       << " with range " << rangeRt
	       << " into Conditions Store");

  if ( writeCdoCor->size()==0 ) {
    ATH_MSG_WARNING("writeCdoCor->size()==0");
    return StatusCode::FAILURE;
  }
  if (writeHandleCor.record(rangeRt, std::move(writeCdoCor)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandleCor.key()
		  << " with EventRange " << rangeRt
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandleCor.key()
	       << " with range " << rangeRt
	       << " into Conditions Store");
  
  return StatusCode::SUCCESS;
}


// build the transient structure and load some defaults for T0s
StatusCode MdtCalibDbAlg::defaultT0s(std::unique_ptr<MdtTubeCalibContainerCollection>& writeCdoTube) {

  if ( writeCdoTube == nullptr ) {
    ATH_MSG_ERROR("writeCdoTube == nullptr");
    return StatusCode::FAILURE; 
  }
  
  //like MdtCalibDbCoolStrTool::defaultT0s()
  //m_tubeData is writeCdoTube here

  writeCdoTube->resize( m_mdtIdHelper->module_hash_max() );
  ATH_MSG_DEBUG( " Created new MdtTubeCalibContainerCollection size " << writeCdoTube->size() );

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

    if( hash < writeCdoTube->size() ){
      (*writeCdoTube)[hash] = tubes;
      ATH_MSG_VERBOSE( " adding tubes at " << hash << " current size " << writeCdoTube->size() );
//write out string for chamberlist
      if(tubes) {
	int nml     = tubes->numMultilayers();
	int nlayers = tubes->numLayers();
	int ntubes  = tubes->numTubes();	
	ATH_MSG_VERBOSE( "CHAMBERLIST: " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(*it)) << " " << m_mdtIdHelper->stationEta(*it) << " " << m_mdtIdHelper->stationPhi(*it) << " " << nml*nlayers*ntubes << " " << nml << " " << nlayers << " " << ntubes << " dummy " << hash );
      }
    } else {
      if(tubes) delete tubes;
      ATH_MSG_WARNING( " HashId out of range " << hash << " max " << writeCdoTube->size() );
    }
  }
  ATH_MSG_DEBUG( " Done defaultT0s " << writeCdoTube->size() );

  return StatusCode::SUCCESS;
}  //end MdtCalibDbAlg::defaultT0s


StatusCode MdtCalibDbAlg::loadTube(){
  ATH_MSG_DEBUG( "loadTube " << name() );

  SG::WriteCondHandle<MdtTubeCalibContainerCollection> writeHandleTube{m_writeKeyTube};
  if ( writeHandleTube.isValid() ) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleTube.fullKey() << " is already valid.");
    return StatusCode::SUCCESS; 
  }
  std::unique_ptr<MdtTubeCalibContainerCollection> writeCdoTube{std::make_unique<MdtTubeCalibContainerCollection>()};

  //like MdtCalibDbCoolStrTool::loadTube()
  //m_tubeData is writeCdoTube here
  //atrc is readCdoTube here

  if ( defaultT0s(writeCdoTube).isFailure() ) {
    ATH_MSG_FATAL("defaultT0s().isFailure()");
    return StatusCode::FAILURE;
  }
  
  //Read Cond Handle  
  SG::ReadCondHandle<CondAttrListCollection> readHandleTube{ m_readKeyTube };
  const CondAttrListCollection* readCdoTube{*readHandleTube}; 
  if ( readCdoTube==nullptr ) {
    ATH_MSG_ERROR("readCdoTube==nullptr");
    return StatusCode::FAILURE; 
  }   
  EventIDRange rangeTube;
  if ( !readHandleTube.range(rangeTube) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleTube.key());
    return StatusCode::FAILURE;
  }  
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleTube.fullKey() << " readCdoTube->size()= " << readCdoTube->size());
  ATH_MSG_INFO("Range of input is " << rangeTube);
  
  // Inverse of wire propagation speed
  float inversePropSpeed = 1./(299.792458*m_prop_beta);

  // unpack the strings in the collection and update the 
  // MdtTubeCalibContainers in TDS
  CondAttrListCollection::const_iterator itr;
  for (itr = readCdoTube->begin(); itr != readCdoTube->end(); ++itr) {
    const coral::AttributeList &atr = itr->second;
    std::string header="",payload="",trailer="";

    bool t0_ts_applied = (atr["tech"].data<int>() & MuonCalib::TIME_SLEWING_CORRECTION_APPLIED);
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
    if (hash>=writeCdoTube->size()) {
      ATH_MSG_INFO( "Illegal station (1)! (" << name << "," << iphi << "," << ieta << ")" );
      continue;
    }
                                                                                
    // retrieve the existing one (created by defaultt0() )
    tubes = (*writeCdoTube)[hash];

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
  }//end loop over readCdoTube

  //finally record writeCdo

  if ( writeCdoTube->size()==0 ) {
    ATH_MSG_WARNING("writeCdoTube->size()==0"); 
    return StatusCode::FAILURE;
  }
  if (writeHandleTube.record(rangeTube, std::move(writeCdoTube)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandleTube.key()
		  << " with EventRange " << rangeTube
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandleTube.key()
	       << " with range " << rangeTube
	       << " into Conditions Store");

  return StatusCode::SUCCESS;
}


StatusCode MdtCalibDbAlg::finalize(){  
  ATH_MSG_DEBUG( "finalize " << name() );
  return StatusCode::SUCCESS;
}


// Build a MuonCalib::MdtTubeCalibContainer for a given Identifier
MuonCalib::MdtTubeCalibContainer* MdtCalibDbAlg::buildMdtTubeCalibContainer(const Identifier &id) {    
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
}  //end MdtCalibDbAlg::buildMdtTubeCalibContainer

inline bool MdtCalibDbAlg::uncompressInMyBuffer(const coral::Blob &blob) {
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
}

inline MuonCalib::RtResolutionLookUp* MdtCalibDbAlg::getRtResolutionInterpolation( const std::vector<MuonCalib::SamplePoint> &sample_points) {

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
  return new MuonCalib::RtResolutionLookUp(res_param);
}

inline StatusCode MdtCalibDbAlg::extractString(std::string &input, std::string &output, std::string separator) {
  unsigned long int pos = 0;
  std::string::size_type start = input.find_first_not_of(separator.c_str(),pos);
  if(start == std::string::npos) {
    ATH_MSG_ERROR("MdtCalibDbAlg::extractString: Cannot extract string in a proper way!");
    return StatusCode::FAILURE;
  }
  std::string::size_type stop = input.find_first_of(separator.c_str(),start+1);
  if (stop == std::string::npos) stop = input.size();
  output = input.substr(start,stop-start);
  input.erase(pos,stop-pos);

  return StatusCode::SUCCESS;
}


//like MdtCalibrationDbSvc
//for corData in loadRt
void MdtCalibDbAlg::initialize_B_correction(MuonCalib::MdtCorFuncSet *funcSet,
                                                  const MuonCalib::MdtRtRelation *rt_rel) {
  if (rt_rel==NULL) {
    funcSet->setBField(NULL);
    return;
  }
  ATH_MSG_VERBOSE( "initialize_B_correction..." );
  std::vector<double> corr_params(2);
  corr_params[0] = 3080.0; // high voltage (not correct for sMDT which use 2730V!)
  corr_params[1] = 0.11;   // epsilon parameter
  funcSet->setBField(new MuonCalib::BFieldCorFunc(std::string("medium"), corr_params, rt_rel->rt()));
}

void MdtCalibDbAlg::initializeSagCorrection(MuonCalib::MdtCorFuncSet *funcSet) {
  ATH_MSG_VERBOSE( "initializeSagCorrection..." );
  std::vector<double> corr_params(0);
  funcSet->wireSag(new MuonCalib::WireSagCorFunc(corr_params));
}
