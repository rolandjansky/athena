/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/TransientAddress.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MdtCalibData/CalibFunc.h"
#include "MuonCalibStl/ToString.h"
#include "MdtCalibUtils/RtDataFromFile.h"
#include "PathResolver/PathResolver.h"
#include "MdtCalibData/MdtCalibrationFactory.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include <fstream>
#include <string>
#include <dirent.h>

#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"

#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"

//this
#include "MdtCalibDbAsciiTool/MdtCalibDbAsciiTool.h"
#include "MdtCalibDbAsciiTool/StationT0FromFile.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

namespace MuonCalib {

MdtCalibDbAsciiTool::MdtCalibDbAsciiTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_tubeData(NULL),
    m_rtData(NULL),
    m_doCommissioning(false),
    m_bilt0(215.),
    m_bist0(150.),
    m_bmlt0(190.),
    m_bmst0(90.),
    m_bolt0(150.),
    m_bost0(180.),
    m_eilt0(850.),
    m_eist0(850.),
    m_emlt0(850.),
    m_emst0(850.),
    m_eolt0(850.),
    m_eost0(850.),
    m_rtDataLocation("RtKey"),
    m_tubeDataLocation("TubeKey"),
    m_calib_dir("NONE"), 
    m_t0Shift(0),
    m_t0Spread(0),
    m_rtShift(0)
{

  declareInterface< IMdtCalibDBTool >(this) ;

  // the keys are hardcoded in order to be sure to match the ones
  // used in regFcn by the Svc
  //declareProperty("TubeKey",m_tubeDataLocation);
  //declareProperty("RtKey",m_rtDataLocation);

  declareProperty("doCommissioning",m_doCommissioning,"allow setting T0s by chamber type for commissioning and sets default global T0 to 150 ns");
  declareProperty("BILT0",m_bilt0,"for commissioning (doCommissioning True) sets all BIL chambers T0s to a common value");
  declareProperty("BIST0",m_bist0,"for commissioning (doCommissioning True) sets all BIS chambers T0s to a common value");
  declareProperty("BMLT0",m_bmlt0,"for commissioning (doCommissioning True) sets all BML chambers T0s to a common value");
  declareProperty("BMST0",m_bmst0,"for commissioning (doCommissioning True) sets all BMS chambers T0s to a common value");
  declareProperty("BOLT0",m_bolt0,"for commissioning (doCommissioning True) sets all BOL chambers T0s to a common value");
  declareProperty("BOST0",m_bost0,"for commissioning (doCommissioning True) sets all BOS chambers T0s to a common value");
  declareProperty("EILT0",m_eilt0,"for commissioning (doCommissioning True) sets all EIL chambers T0s to a common value");
  declareProperty("EIST0",m_eist0,"for commissioning (doCommissioning True) sets all EIS chambers T0s to a common value");
  declareProperty("EMLT0",m_emlt0,"for commissioning (doCommissioning True) sets all EML chambers T0s to a common value");
  declareProperty("EMST0",m_emst0,"for commissioning (doCommissioning True) sets all EMS chambers T0s to a common value");
  declareProperty("EOLT0",m_eolt0,"for commissioning (doCommissioning True) sets all EOL chambers T0s to a common value");
  declareProperty("EOST0",m_eost0,"for commissioning (doCommissioning True) sets all EOS chambers T0s to a common value");

  
  declareProperty("T0Shift",m_t0Shift,"for simulation: common shift of all T0s, in ns");
  declareProperty("T0Spread",m_t0Spread,"for simulation: sigma for random smeraing of T0s, in ns");
  declareProperty("RTShift",m_rtShift,"for simulations: maximum RT distortion, in mm");
  declareProperty("CalibDir", m_calib_dir, "directory containg calibration files as written from standalone_tarball. If not NONE all other settings are overridden");
  
  // is this the simplest way to initialize a list?
  std::ostringstream myse;
  std::vector<std::string> myord;
  myse<< "DC2_rt_asciiDb.dat";
  myord.push_back(myse.str());
  declareProperty("RT_InputFiles",m_RTfileNames=myord,"list of RT input files, see doxygen main page for the case with multiple input files");

}

StatusCode MdtCalibDbAsciiTool::updateAddress(StoreID::type /*storeID*/,
                                              SG::TransientAddress* tad,
                                              const EventContext& /*ctx*/) {
  CLID clid        = tad->clID();
  std::string key  = tad->name();
  if ( 1221928754== clid && m_tubeDataLocation == key) {
    ATH_MSG_DEBUG("OK Tube");
    return StatusCode::SUCCESS;
  }
  if ( 1270996316== clid && m_rtDataLocation == key) {
    ATH_MSG_DEBUG("OK Rt");
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode MdtCalibDbAsciiTool::initialize() {
  ATH_MSG_DEBUG("Initializing");

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_DetectorManagerKey.initialize());

  ATH_CHECK(serviceLocator()->service("MdtCalibrationRegionSvc", m_regionSvc));
  ATH_MSG_DEBUG("Retrieved MdtCalibrationRegionSvc");

  if( m_t0Spread != 0. ) {
    static const bool CREATEIFNOTTHERE(true);
    ATH_CHECK(service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE));
    if (0 == p_AtRndmGenSvc) {
      ATH_MSG_ERROR(" Could not initialize Random Number Service");
      return StatusCode::FAILURE;
    } else{
      ATH_MSG_DEBUG(" initialize Random Number Service: running with t0 shift "
			   << m_t0Shift << " spread " << m_t0Spread << " rt shift " << m_rtShift);
    }
    // getting our random numbers stream
    p_engine  =       p_AtRndmGenSvc->GetEngine("MDTCALIBDBASCIITOOL");
  }

  // initialize MdtTubeCalibContainers 
  ATH_CHECK(defaultT0s());
  ATH_CHECK(detStore()->record( m_tubeData, m_tubeDataLocation, true ));

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MdtTubeCalibContainerCollection>::ID(), m_tubeDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR("Unable to get the proxy for class MdtTubeCalibContainerCollection");
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG("set address provider for MdtTubeCalibContainerCollection");

  ATH_CHECK(defaultRt());
  ATH_CHECK(detStore()->record( m_rtData, m_rtDataLocation, true ));

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  proxy = detStore()->proxy(ClassID_traits<MdtRtRelationCollection>::ID(), m_rtDataLocation);
  if (!proxy) {
    ATH_MSG_ERROR("Unable to get the proxy for class MdtRtRelationCollection");
    return StatusCode::FAILURE;
  }

  addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_DEBUG("set address provider for MdtRtRelationCollection");

  return StatusCode::SUCCESS;
}

StatusCode MdtCalibDbAsciiTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  ATH_MSG_DEBUG("LoadCalibration has been triggered for the following keys ");
 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_DEBUG(*itr << " I="<<I<<" ");
  }

  return StatusCode::SUCCESS;

}


StatusCode MdtCalibDbAsciiTool::defaultT0s() {
  // create collection
  if(m_tubeData) {
    delete m_tubeData; m_tubeData=0;
  }
  m_tubeData = new MdtTubeCalibContainerCollection();
  m_tubeData->resize( m_idHelperSvc->mdtIdHelper().module_hash_max() );
  ATH_MSG_DEBUG(" Created new MdtTubeCalibContainerCollection size " << m_tubeData->size());

  MdtIdHelper::const_id_iterator it     = m_idHelperSvc->mdtIdHelper().module_begin();
  MdtIdHelper::const_id_iterator it_end = m_idHelperSvc->mdtIdHelper().module_end();
  for( ; it!=it_end;++it ) {
    
    MuonCalib::MdtTubeCalibContainer* tubes=0;
    // create a MdtTubeContainer
    tubes = buildMdtTubeCalibContainer(*it);

    if(tubes) {
      std::string rName=tubes->regionKey();
      std::string stationName(rName,0,3);
      double t0 = 800.;

      // special treatment of the t0's for commissioning
      if( m_doCommissioning ){
        t0 = 150.;
	if( stationName == "BIL" ) {
	  t0 = m_bilt0;
	}else if( stationName == "BIS" ){
	  t0 = m_bist0;
	}else if( stationName == "BML" ){
	  t0 = m_bmlt0;
	}else if( stationName == "BMS" ){
	  t0 = m_bmst0;
	}else if( stationName == "BOL" ){
	  t0 = m_bolt0;
	}else if( stationName == "BOS" ){
	  t0 = m_bost0;
	}else if( stationName == "EIL" ){
	  t0 = m_eilt0;
	}else if( stationName == "EIS" ){
	  t0 = m_eist0;
	}else if( stationName == "EML" ){
	  t0 = m_emlt0;
	}else if( stationName == "EMS" ){
	  t0 = m_emst0;
	}else if( stationName == "EOL" ){
	  t0 = m_eolt0;
	}else if( stationName == "EOS" ){
	  t0 = m_eost0;
	}	
      }
      int nml=tubes->numMultilayers();
      int nlayers=tubes->numLayers();
      int ntubes=tubes->numTubes();
      int size = nml*nlayers*ntubes;
      //look for calibration file
      bool from_file(false);
      StationT0FromFile station_t0;
      if(m_calib_dir != "NONE") {
	std::ifstream t0_file((m_calib_dir + "/t0s/T0" + rName + ".dat").c_str());
	if(t0_file.fail()) {
	    ATH_MSG_VERBOSE("File '" << (m_calib_dir + "/t0s/" + rName + ".dat") << "' not found!");
	    ATH_MSG_VERBOSE("Using default t0 for chamber '" << rName << "'!");
	} else {
	  ATH_MSG_DEBUG("Reading calibraation for '" << rName << "' from calibration directory.");
	  from_file=true;
	  station_t0.readT0File(t0_file);
	}
      }
	ATH_MSG_VERBOSE("Adding chamber " << m_idHelperSvc->mdtIdHelper().print_to_string(*it));
	ATH_MSG_VERBOSE(" size " << size
	       << " ml " << nml << " l " << nlayers << " t " 
	       << ntubes << " address " << tubes);
      for( int ml=0;ml<nml;++ml ){
	for( int l=0;l<nlayers;++l ){
	  for( int t=0;t<ntubes;++t ){
	    MuonCalib::MdtTubeCalibContainer::SingleTubeCalib data;
	    double tt = t0;
	    if(from_file) {
	      tt = station_t0.t0(ml + 1, l + 1, t + 1);
	      ATH_MSG_VERBOSE("tt="<<tt);
	    }
	    if( m_t0Shift != 0 ) {
	      tt += m_t0Shift;
	      ATH_MSG_VERBOSE("T0 shift " << m_t0Shift << " t0 " << tt 
				     << " id " << ml << " " << l << " " << t);
	    }
	    if(m_t0Spread != 0 ) {
	      double sh = CLHEP::RandGaussZiggurat::shoot(p_engine,0.,m_t0Spread);
	      tt += sh;
	      ATH_MSG_VERBOSE("T0 spread " << sh << " t0 " << tt 
				     << " id " << ml << " " << l << " " << t);
	    }
	    data.t0 = tt;
	    data.adcCal = 1.;
	    data.inversePropSpeed = 1./299.792458;	   
	    tubes->setCalib( ml,l,t,data );
	  }
	}
      }
    }
    ATH_MSG_VERBOSE(" set t0's done ");
    //MdtBasicRegionHash hash;
    IdentifierHash hash;
    IdContext idCont = m_idHelperSvc->mdtIdHelper().module_context();
    m_idHelperSvc->mdtIdHelper().get_hash( *it, hash, &idCont );

    if( hash < m_tubeData->size() ){
      (*m_tubeData)[hash] = tubes;
      ATH_MSG_VERBOSE(" adding tubes at " << hash << " current size " << m_tubeData->size());
    }else{
      ATH_MSG_WARNING(" HashId out of range " << hash << " max " << m_tubeData->size());
    delete tubes; tubes=0;
    }
   
  }
  ATH_MSG_DEBUG(" Done defaultT0s " << m_tubeData->size());

  return StatusCode::SUCCESS;
}

MuonCalib::MdtTubeCalibContainer * MdtCalibDbAsciiTool::buildMdtTubeCalibContainer(const Identifier& id) {    
    MuonCalib::MdtTubeCalibContainer* tubes = 0;
    
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
    if(MuonDetMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
      return tubes; 
    } 

    const MuonGM::MdtReadoutElement* detEl = MuonDetMgr->getMdtReadoutElement( m_idHelperSvc->mdtIdHelper().channelID(id,1,1,1) );
    const MuonGM::MdtReadoutElement* detEl2 = 0;
    if (m_idHelperSvc->mdtIdHelper().numberOfMultilayers(id) == 2){
      detEl2 = MuonDetMgr->getMdtReadoutElement(m_idHelperSvc->mdtIdHelper().channelID(id,2,1,1) );
    }else{
      ATH_MSG_ERROR("A single multilayer for this station " << m_idHelperSvc->mdtIdHelper().show_to_string(id));
    }

    ATH_MSG_VERBOSE(" new det el " << detEl);
    
    if( !detEl ){ 
      ATH_MSG_WARNING("No detEl found for " << m_idHelperSvc->mdtIdHelper().print_to_string(id));
    }else{
      int nml = 2;
      if( !detEl2 ) {
	nml = 1;
      }

      int nlayers = detEl->getNLayers();
      if( detEl2 && detEl2->getNLayers() > nlayers ){
	ATH_MSG_DEBUG("Second multilayer has more layers " << detEl2->getNLayers() << " then first " << nlayers);
	nlayers = detEl2->getNLayers();
      }

      int ntubes = detEl->getNtubesperlayer();
      if( detEl2 && detEl2->getNtubesperlayer() > ntubes ){
	ATH_MSG_DEBUG("Second multilayer has more tubes " << detEl2->getNtubesperlayer() << " then first " << ntubes);
	ntubes = detEl2->getNtubesperlayer();
      }

      // build the region name in the format STATION_ETA_PHI
      std::string rName;

      int stName = m_idHelperSvc->mdtIdHelper().stationName(id);
      int stPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
      int stEta = m_idHelperSvc->mdtIdHelper().stationEta(id);
  
      std::string seperator("_");
      MuonCalib::ToString ts;
      rName = m_idHelperSvc->mdtIdHelper().stationNameString(stName);
      rName += seperator + ts( stPhi ) + seperator + ts( stEta );
      tubes=new MuonCalib::MdtTubeCalibContainer( rName,nml, nlayers, ntubes );
    }

    return tubes;
}

StatusCode MdtCalibDbAsciiTool::defaultRt() {
  std::vector<std::string>::const_iterator it     = m_RTfileNames.value().begin();
  std::vector<std::string>::const_iterator it_end = m_RTfileNames.value().end();

//resolve filenames
  std::vector<std::string> resolved_filenames;
  for ( ; it != it_end ; ++it) {
    std::string fileName = PathResolver::find_file(it->c_str(),"DATAPATH");
    if(fileName.length() == 0) {
      ATH_MSG_ERROR("RT Ascii file \"" <<  it->c_str() << "\" not found");
      continue;
    }
    resolved_filenames.push_back(fileName);
  }
		
  AppendRtFiles(resolved_filenames);
  it = resolved_filenames.begin();
  it_end = resolved_filenames.end();
 
  if (it == it_end ) {
     ATH_MSG_FATAL("No input RT files found");
     return StatusCode::FAILURE;
  } else if (it_end-it>1) {
     // many files listed in jobO: assume there is one for each chamber
     // people are interested in 
     m_regionSvc->remapRtRegions("OnePerChamber");
  }

  // create collection 
  if(m_rtData) {
    delete m_rtData; m_rtData=0;
  }
  m_rtData = new MdtRtRelationCollection(); 
  m_rtData->resize(m_regionSvc->numberOfRegions());
  ATH_MSG_DEBUG(" Created new MdtRtRelationCollection size " << m_rtData->size());

  // Loop over RT files
  for ( ; it != it_end ; ++it) {    
    const std::string & fileName(*it);
    // Open the Ascii file with the RT relations
    std::ifstream inputFile( fileName.c_str() );
    if( !inputFile ) {
      ATH_MSG_ERROR("Unable to open RT Ascii file: " << fileName.c_str());
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_DEBUG("Opened RT Ascii file: " <<  fileName.c_str());
    }
    RtDataFromFile rts;
    rts.read(inputFile);
    ATH_MSG_VERBOSE("File contains " << rts.nRts() << " RT relations ");

    int nmax = static_cast <int> (rts.nRts());
    if( nmax > 10000 ) nmax = 10000;   //prevent arbirarily large loop range to appease Coverity
    for( int n=0; n<nmax; ++n ) {
      RtDataFromFile::RtRelation* rt = rts.getRt( n );
      unsigned int regionId = rt->regionId();

      if( regionId >= m_rtData->size() ){
	ATH_MSG_WARNING(" regionHash out of range: " << regionId << " max " << m_rtData->size());
	delete rt; rt=0;
	continue;
      }
      
      const RtDataFromFile::RtRelation::DataVec& times  = rt->times();
      const RtDataFromFile::RtRelation::DataVec& radii  = rt->radii();
      const RtDataFromFile::RtRelation::DataVec& reso   = rt->resolution();
      // check if rt contains data, at least two point on the rt are required
      if( times.size() < 2 ) {
	ATH_MSG_ERROR(" defaultRt rt table has too few entries");
	continue;
      }

      // check if all tables have same size
      if( times.size() != radii.size() || times.size() != reso.size() ) {
	ATH_MSG_ERROR("defaultRt rt table size mismatch ");
	continue;
      }

      // check if
      double t_min = times[0];
      double bin_size = times[1]-t_min;

      // additional consistency check 
      if( bin_size <= 0 ) {
	ATH_MSG_ERROR("RtCalibrationClassic::defaultRt rt table negative binsize ");
	continue;
      }

      // create a vector to hold the r values, 
      // we need two extra fields to store t_min and bin_size
      MuonCalib::CalibFunc::ParVec rtPars;
      rtPars.push_back( t_min );
      rtPars.push_back( bin_size );
      
      if( m_rtShift != 0. ){

	RtDataFromFile::RtRelation::DataVec::const_iterator rtIt = radii.begin();
	RtDataFromFile::RtRelation::DataVec::const_iterator rtIt_end = radii.end();
	for( ;rtIt!=rtIt_end;++rtIt ){
	  double rold = *rtIt;
	  double rshift = m_rtShift*1.87652e-2*rold*(rold-14.6);
	  double rnew = rold + rshift;
	  ATH_MSG_DEBUG("DEFORM RT: old radius " << rold << " new radius " << rnew << " shift " << rshift
			    << " max shift " << m_rtShift);
	  rtPars.push_back(rnew);
	} 
      }else{
	// copy r values into vector
	rtPars.insert( rtPars.end(), radii.begin(), radii.end() );
      }

      ATH_MSG_DEBUG("defaultRt new  MuonCalib::IRtRelation");
      MuonCalib::IRtRelation* rtRel = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );
      if( !rtRel ){
	ATH_MSG_WARNING("ERROR creating RtRelationLookUp ");
      }

      MuonCalib::CalibFunc::ParVec resoPars;
      resoPars.push_back( t_min );
      resoPars.push_back( bin_size );

      // copy r values into vector
      resoPars.insert( resoPars.end(), reso.begin(), reso.end() );

      ATH_MSG_DEBUG("defaultRt new  MuonCalib::IRtResolution");
      MuonCalib::IRtResolution* resoRel = MuonCalib::MdtCalibrationFactory::createRtResolution( "RtResolutionLookUp", resoPars );
      if( !rtRel ){
	ATH_MSG_WARNING("ERROR creating RtResolutionLookUp ");
      }

      if( resoRel && rtRel ){
	int npoints= rtRel->nPar()-2;
	(*m_rtData)[regionId] = new MuonCalib::MdtRtRelation( rtRel, resoRel, 0. );
	ATH_MSG_VERBOSE("defaultRt npoints from rtRel="<< npoints);

	for( int j=0;j<npoints;++j ){
	  double t = t_min + j*bin_size;
	  ATH_MSG_VERBOSE("  " << j << " " << t << "  " << rtRel->radius(t) << " " << resoRel->resolution(t));
	}
      }
      delete rt; rt=0;
    
    }
  }
  return StatusCode::SUCCESS;
}


void MdtCalibDbAsciiTool::AppendRtFiles(std::vector<std::string> &rt_files) {
  DIR *directory(opendir((m_calib_dir + "/rts").c_str()));
  if(directory==NULL) return;
  struct dirent *dent;
  //loop on all files in directory
  while((dent=readdir(directory))!=NULL) {
    std::string nm(dent->d_name);
    std::string station;
    int eta, phi;
    if(!interpret_chamber_name(nm, "Rt_", station,eta, phi)) continue;
    rt_files.push_back(m_calib_dir + "/rts/" + nm);
    ATH_MSG_DEBUG("Appending rt file "<<m_calib_dir << "/rts/" << nm);
  }
  closedir(directory);
}

bool MdtCalibDbAsciiTool :: interpret_chamber_name(const std::string &nm, const char *prefix, std::string & station, int &eta, int & phi) const {
//check if name begins with the prefix
  std::string prefix_st(prefix);
  if(nm.find(prefix_st) !=0) return false;
  //check if filename ends in .dat
  if(static_cast<int>(nm.find(".dat")) <0 || nm.find(".dat")!=nm.size()-4) return false;
  //cut prefix and suffix from filename
  std::string cutout(nm, prefix_st.size(), nm.size()-4-prefix_st.size());
//extrect station name
  int uscore_pos(cutout.find('_'));		
  if(uscore_pos<=0) return false;	
  station=std::string(cutout, 0, uscore_pos);
//get eta and phi
  std::string cutout2(cutout, uscore_pos+1);
  if(sscanf(cutout2.c_str(), "%80d_%80d", &phi, &eta)!=2) return false;
  return true;	
}

}
