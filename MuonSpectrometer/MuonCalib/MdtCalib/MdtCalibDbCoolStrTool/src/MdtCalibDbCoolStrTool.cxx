/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

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
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

#include "TSpline.h"
#include "TFile.h"

namespace MuonCalib 
{

MdtCalibDbCoolStrTool::MdtCalibDbCoolStrTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AlgTool(type, name, parent), m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),m_rtFolder("/MDT/RT"), m_tubeFolder("/MDT/T0"),
    m_rtDataLocation("RtKey"), m_tubeDataLocation("TubeKey"), m_defaultT0(800.),
    
    m_t0Shift(0.),
    m_t0Spread(0.),
    m_rtShift(0.),
    m_rtScale(1.),
    m_log(0),
    m_debug(false),
    m_verbose(false),
    m_buffer_length(0),
    m_decompression_buffer(NULL)
{

  declareInterface< IMdtCalibDBTool >(this) ;

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
  myse<< "DC2_rt_default.dat";
  myord.push_back(myse.str());

  declareProperty("RT_InputFiles",m_RTfileNames=myord,"single input ascii file for default RT to be applied in absence of DB information");

  // defaultT0, used for tubes not found in DB
  declareProperty("defaultT0",m_defaultT0,"default T0 value to be used in absence of DB information");
  m_TimeSlewingCorrection=false;
  declareProperty("TimeSlewingCorrection", m_TimeSlewingCorrection);
  declareProperty("MeanCorrectionVsR", m_MeanCorrectionVsR);
  m_TsCorrectionT0=0.;
  m_prop_beta=1.0;
  declareProperty("PropagationSpeedBeta", m_prop_beta);
}


MdtCalibDbCoolStrTool::~MdtCalibDbCoolStrTool()
	{
	if (m_decompression_buffer)
		delete [] m_decompression_buffer;
	}


StatusCode MdtCalibDbCoolStrTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad)
{
    CLID clid        = tad->clID();
    std::string key  = tad->name();
    if ( 1221928754== clid && m_tubeDataLocation == key)
    {
        if( m_debug ) *m_log << MSG::DEBUG << "OK Tube" << endreq;
        return StatusCode::SUCCESS;
    }
    if ( 1270996316== clid && m_rtDataLocation == key)
    {
        if( m_debug )*m_log << MSG::DEBUG << "OK Rt" << endreq;
        return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
}

StatusCode MdtCalibDbCoolStrTool::initialize()
{ 
  m_log = new MsgStream(msgSvc(), name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  if( m_debug ) *m_log << MSG::DEBUG << "Initializing " << endreq;
  if(m_MeanCorrectionVsR.size())
  	{
	m_TsCorrectionT0=m_MeanCorrectionVsR[0];
	for(std::vector<float>::iterator it=m_MeanCorrectionVsR.begin(); it!=m_MeanCorrectionVsR.end(); it++)
		{
		(*it)-=m_TsCorrectionT0;
		}
	
	}
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug ) *m_log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    *m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }

  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (!sc.isSuccess()) {
    *m_log << MSG::ERROR << "Can't retrieve MdtIdHelper" << endreq;
    return sc;
  }
  
  sc = m_detStore->retrieve( m_detMgr );
  if (!sc.isSuccess()) {
    *m_log << MSG::ERROR << "Can't retrieve MuonDetectorManager" << endreq;
    return sc;
  }


  // Get interface to IOVSvc
  m_IOVDbSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVDbSvc", m_IOVDbSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       *m_log << MSG::ERROR << "Unable to get the IOVDbSvc" << endreq;
       return StatusCode::FAILURE;
  }

  sc = serviceLocator()->service("MdtCalibrationRegionSvc", m_regionSvc);
  if ( sc.isSuccess() ) {
    if( m_debug ) *m_log << MSG::DEBUG << "Retrieved MdtCalibrationRegionSvc" << endreq;
  }else{
    *m_log << MSG::ERROR << "Failed to retrieve MdtCalibrationRegionSvc" << endreq;    return sc;
  }

  // register callbacks
  const DataHandle<CondAttrListCollection> tubeData;
  sc=m_detStore->regFcn(&IMdtCalibDBTool::loadTube,
                        dynamic_cast<IMdtCalibDBTool*>(this),
                        tubeData,m_tubeFolder);
  if(sc.isFailure()) return StatusCode::FAILURE;
  const DataHandle<CondAttrListCollection> rtData;
  sc=m_detStore->regFcn(&IMdtCalibDBTool::loadRt,
                        dynamic_cast<IMdtCalibDBTool*>(this),
                        rtData,m_rtFolder);
  if(sc.isFailure()) return StatusCode::FAILURE;

  sc = m_idToFixedIdTool.retrieve();
  if (sc.isFailure()) {
     *m_log << MSG::FATAL << "Could not find " << m_idToFixedIdTool << endreq;
      return sc;
  } else {
     if( m_debug )*m_log << MSG::INFO << "Retrieved " << m_idToFixedIdTool << endreq;
  }


  MdtCalibrationDbSvc * dbSvc;

  sc = serviceLocator()->service("MdtCalibrationDbSvc", dbSvc);
  if ( sc.isSuccess() ) {
    if( m_debug ) *m_log << MSG::DEBUG << "Retrieved MdtCalibrationDbSvc" << endreq;
  }else{
    *m_log << MSG::ERROR << "Failed to retrieve MdtCalibrationDbSvc" << endreq;    return sc;
  }
  sc= m_detStore->regFcn(&IMdtCalibDBTool::loadTube,dynamic_cast<IMdtCalibDBTool*>(this),&MdtCalibrationDbSvc::loadTube,dynamic_cast<MdtCalibrationDbSvc*>(dbSvc));
  if(sc.isFailure()) return StatusCode::FAILURE;
  sc=m_detStore->regFcn(&IMdtCalibDBTool::loadRt,dynamic_cast<IMdtCalibDBTool*>(this),&MdtCalibrationDbSvc::loadRt,dynamic_cast<MdtCalibrationDbSvc*>(dbSvc));
  if(sc.isFailure()) return StatusCode::FAILURE;

  // initialize MdtTubeCalibContainers 
  sc=defaultT0s();
  if(sc.isFailure()) return StatusCode::FAILURE;
  sc=m_detStore->record( m_tubeData, m_tubeDataLocation, true );
  if(sc.isFailure()) return StatusCode::FAILURE;

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtTubeCalibContainerCollection>::ID(), m_tubeDataLocation);
  if (!proxy) {
    *m_log << MSG::ERROR << "Unable to get the proxy for class MdtTubeCalibContainerCollection" << endreq;
    return StatusCode::FAILURE;
  }

  SG::TransientAddress* tad =  proxy->transientAddress();
  if (!tad) {
    *m_log << MSG::ERROR << "Unable to get the tad" << endreq;
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  if( m_debug ) *m_log << MSG::DEBUG << "set address provider for MdtTubeCalibContainerCollection" << endreq;

  // WARNING: the region mapping is currently hardcoded in 
  //          MdtCalibrationRegionSvc and we use the folder names to select
  //	      Rt data organized in a single region (default /MDT/RTUNIQUE folder)
  //          or organized in one region per chamber 
  if(m_rtFolder != "/MDT/RTUNIQUE") {
    m_regionSvc->remapRtRegions("OnePerChamber");
  }
  sc=defaultRt();
  if(sc.isFailure()) return StatusCode::FAILURE;
  sc=m_detStore->record( m_rtData, m_rtDataLocation, true );
  if(sc.isFailure()) return StatusCode::FAILURE;
                                                                                
  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  proxy = m_detStore->proxy(ClassID_traits<MdtRtRelationCollection>::ID(), m_rtDataLocation);
  if (!proxy) {
    *m_log << MSG::ERROR << "Unable to get the proxy for class MdtRtRelationCollection" << endreq;
    return StatusCode::FAILURE;
  }
                                                                                
  tad =  proxy->transientAddress();
  if (!tad) {
    *m_log << MSG::ERROR << "Unable to get the tad" << endreq;
    return StatusCode::FAILURE;
  }
                                                                                
  addp = this;
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  if( m_debug ) *m_log << MSG::DEBUG << "set address provider for MdtRtRelationCollection" << endreq;
   

  //initiallize random number generator if doing t0 smearing (for robustness studies)
  if( m_t0Spread != 0. ) {
    static const bool CREATEIFNOTTHERE(true);
    StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
    if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc)
      {
	*m_log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
	return RndmStatus;
      }      
    else{
      if( m_debug ) *m_log << MSG::DEBUG << " initialize Random Number Service: running with t0 shift "
			   << m_t0Shift << " spread " << m_t0Spread << " rt shift " << m_rtShift << endreq;
      
    }
    // getting our random numbers stream
    p_engine  =       p_AtRndmGenSvc->GetEngine("MDTCALIBDBASCIITOOL");
  }

  if ( m_rtShift != 0. || m_rtScale != 1. || m_t0Shift != 0. || m_t0Spread != 0.) {
    *m_log << MSG::INFO << "************************************" << std::endl
	   << " Running with Calibration Deformations! " << std::endl
	   << " For performance studies only!" << std::endl
	   << " **************************************" << endreq;
    *m_log << MSG::DEBUG << " rt scale " << m_rtScale << " t0 shift " 
	   << m_t0Shift << " spread " << m_t0Spread << " rt shift " << m_rtShift << endreq;
  }
  
  return sc;
}



StatusCode MdtCalibDbCoolStrTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  StatusCode sc;
  if( m_debug ) *m_log << MSG::DEBUG << "LoadCalibration has been triggered for the following keys " << endreq;
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    if( m_debug ) *m_log << MSG::DEBUG << *itr << " I="<<I<<" ";
  }
  if( m_debug ) *m_log << MSG::DEBUG << endreq;

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

}


StatusCode MdtCalibDbCoolStrTool::loadTube(IOVSVC_CALLBACK_ARGS) 
{

   StatusCode sc;
   if( m_debug ) *m_log << MSG::DEBUG << "Load Tube Calibration from DB" << endreq;
   // retreive the and remove the old collection 
   sc = m_detStore->retrieve( m_tubeData, m_tubeDataLocation );
   if(sc.isSuccess())  {
    if( m_debug ) *m_log << MSG::DEBUG << "Tube Collection found " << m_tubeData << endreq;
    sc = m_detStore->removeDataAndProxy( m_tubeData );
    if(sc.isSuccess())  {
      if( m_debug ) *m_log << MSG::DEBUG << "Tube Collection at " << m_tubeData << " removed "<<endreq;
    }
   }

   // reinitialize the MdtTubeCalibContainerCollection
   sc=defaultT0s();
   if(sc.isFailure()) return StatusCode::FAILURE;

   // retreive the collection of strings read out from the DB
   const CondAttrListCollection * atrc;
   sc=m_detStore->retrieve(atrc,m_tubeFolder);
   if(sc.isFailure())  {
     *m_log << MSG::ERROR 
         << "could not retreive the CondAttrListCollection from DB folder " 
         << m_tubeFolder << endreq;
     return sc;
   }

   // unpack the strings in the collection and update the 
   // MdtTubeCalibContainers in TDS
   CondAttrListCollection::const_iterator itr;
   for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    std::string a,b,c;

    bool t0_ts_applied=(atr["tech"].data<int>() & TIME_SLEWING_CORRECTION_APPLIED);
    if (atr["data"].specification().type() == typeid(coral::Blob))
    	{
	if( m_verbose ) *m_log << MSG::VERBOSE << "Data load is a blob. Uncompressing"<< endreq;
	if(!uncompressInMyBuffer(atr["data"].data<coral::Blob>()))
		{
		(*m_log) << MSG::FATAL << "Cannot uncompress buffer" << endreq;
		return StatusCode::FAILURE;
		}
	std::istringstream istr(reinterpret_cast<char*>(m_decompression_buffer));
	istr >> a >> b >> c;
	}
    else
    	{
	std::string data;
	data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
	//data=atr("data").data<std::string>();
	//atr["data"].getValue(data);
	if( m_verbose ) *m_log << MSG::VERBOSE << "Data load is " << data << endreq;

	// interpret as string stream
	std::istringstream istr(data.c_str());
	// a: header, b: payload, c: trailer
	istr >> a >> b >> c;
	}
    if( m_verbose) *m_log << MSG::VERBOSE << "Read string1:" << a << " string2:" << b 
			  << " string3:" << c << endreq;
                                                                                
    char* eta; 
    char* phi;
    int ieta, iphi, region, ntubes;
    std::string seperator("_");
    std::string rName;
    // parameters for the MdtTubeContainer
    // header filename,version,region,tubes
    char * parameters= new char [a.size() +1];
    strncpy(parameters, a.c_str(), a.size() +1);
    parameters[a.size()] = '\0';
    char * pch;
    int i=0;
    pch = strtok (parameters," _,");
    std::string name(pch,2,3);
    if( m_verbose ) *m_log << MSG::VERBOSE  << "name of chamber is " << pch << " station name is " << name << endreq;
 
    pch = strtok (NULL, "_,");
    while (pch != NULL)
    {
     i++;
     std::istringstream is(pch);
     if (i==1) {
       phi=pch; 
       is >> iphi; 
       if( m_verbose ) *m_log << MSG::VERBOSE  << "phi value is " << phi << endreq; 
     }
     if (i==2) {
       eta=pch; 
       is >> ieta; 
       if( m_verbose ) *m_log << MSG::VERBOSE  << "eta value is " << eta << endreq; 
     }
     if (i==4) {
       is >> region; 
       if( m_verbose ) *m_log << MSG::VERBOSE  << "region value is " << region << endreq; 
     }
     if (i==5) {
       is >> ntubes; 
       if( m_verbose ) *m_log << MSG::VERBOSE  << "ntubes value is " << ntubes << endreq; 
     }
     pch = strtok (NULL, "_,");
    }
    delete [] parameters;
    // find chamber ID
    Identifier chId=m_mdtIdHelper->elementID(name,ieta,iphi);
    if( m_verbose ) *m_log << MSG::VERBOSE  << "station name is " << name << " chamber ID  is " << chId <<endreq;
 
    MuonCalib::MdtTubeCalibContainer* tubes = 0;

    // get chamber hash
    MdtBasicRegionHash hash;
    IdContext idCont = m_mdtIdHelper->module_context();
    m_mdtIdHelper->get_hash( chId , hash, &idCont );
    if( m_verbose ) *m_log << MSG::VERBOSE  << "corresponding hash is " << hash <<endreq;
    
   //scip illegal stations. 
    
    if (hash>=m_tubeData->size())
    	{
	//std::cout<<pch<<std::endl;
	*m_log << MSG::INFO << "Illegal station (1)! (" << name << "," << iphi << "," << ieta << ")" << endreq;
	continue;
	}
                                                                                
    // or retreive the existing one?
    tubes = (*m_tubeData)[hash];

    if(tubes==NULL)
    	{
	//std::cout<<pch<<std::endl;
	*m_log << MSG::INFO << "Illegal station (2)! (" << name << "," << iphi << "," << ieta << ")" <<endreq;
	continue;
	}
    

    int nml=tubes->numMultilayers();
    int nlayers=tubes->numLayers();
    int ntubesLay=tubes->numTubes();
    int size = nml*nlayers*ntubesLay;
    if(size!=ntubes) {
     *m_log << MSG::ERROR  << "Pre-existing MdtTubeCalibContainer for chamber ID " <<chId<< " size does not match the one found in DB "<<endreq; 
     return StatusCode::FAILURE;
    }

    int ml=1; int l= 1; int t=1;
    int k=0;
    MuonCalib::MdtTubeCalibContainer::SingleTubeCalib datatube;
    char * TubePar= new char [b.size() +1];
    strncpy(TubePar, b.c_str(), b.size() +1);
    TubePar[b.size()]='\0';
    
    char * pch1;
    pch1 = strtok (TubePar,",");
    while(pch1 != NULL) {
       ++k;
       if (k==1) {
	 double tzero = atof(pch1);
	 if( m_t0Shift != 0. ){
	   tzero += m_t0Shift;
	   if( m_verbose ) *m_log << MSG::VERBOSE << "T0 shift " << m_t0Shift << " t0 " << tzero 
				  << " id " << ml << " " << l << " " << t << endreq;
	 }
	 if(m_t0Spread != 0. ){
	   double sh = CLHEP::RandGaussZiggurat::shoot(p_engine,0.,m_t0Spread);
	   tzero += sh;
	   if( m_verbose ) *m_log << MSG::VERBOSE << "T0 spread " << sh << " t0 " << tzero 
				  << " id " << ml << " " << l << " " << t << endreq;
	 }
	 if(!t0_ts_applied && m_TimeSlewingCorrection)
	 	{
		tzero += m_TsCorrectionT0;
		}
	if(t0_ts_applied && !m_TimeSlewingCorrection)
		{
		tzero -= m_TsCorrectionT0;
		}
         datatube.t0=tzero;
       }
                                                                                
       if (k==2) datatube.statusCode = atoi(pch1);
       if (k==3) {
        datatube.adcCal = atof(pch1);
        datatube.inversePropSpeed = (1./(300. * m_prop_beta));
        tubes->setCalib( ml-1,l-1,t-1,datatube);
        if( m_verbose ) *m_log<<MSG::VERBOSE<< "Loading T0s "<<ml << " " << l << " " << t << " " << datatube.t0 <<endreq; 
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
       pch1 = strtok (NULL, ", ");
    }
    delete [] TubePar;
   }

   // update the IOV for the MdtTubeCalibContainerCollection 
   // using the IOV from the string class
   // for some reason the clid appears to be 1 instead of 1238547719 
   if( m_verbose ) *m_log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
                     << atrc->clID() << endreq;
   // use 1238547719 to get the IOVRange from DB (
/*   IOVRange range;
   sc = m_IOVSvc->getRange(1238547719, m_tubeFolder, range);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange failed" << endreq;}
   if( m_debug ) *m_log << MSG::DEBUG <<"CondAttrListCollection IOVRange "<<range<<endreq;

   IOVRange range2;
   sc = m_IOVSvc->setRange(1221928754, m_tubeDataLocation, range);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::setRange failed" << endreq;}
   sc = m_IOVSvc->getRange(1221928754, m_tubeDataLocation, range2);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange2 failed" << endreq;}
   if( m_debug ) *m_log << MSG::DEBUG <<"MdtTubeCalibContainerCollection new IOVRange "<<range2<<endreq;

*/

   // finally record MdtTubeCalibContainerCollection
   sc = m_detStore->record( m_tubeData, m_tubeDataLocation );
   if (sc.isFailure()){*m_log << MSG::WARNING << "m_tubeData not recorded" << endreq;}

  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtTubeCalibContainerCollection>::ID(), m_tubeDataLocation);
  if (!proxy) {
    *m_log << MSG::ERROR << "Unable to get the proxy for class MdtTubeCalibContainerCollection" << endreq;
    return StatusCode::FAILURE;
  }

  SG::TransientAddress* tad =  proxy->transientAddress();
  if (!tad) {
    *m_log << MSG::ERROR << "Unable to get the tad" << endreq;
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  if( m_debug ) *m_log << MSG::DEBUG<< "set address provider for MdtTubeCalibContainerCollection" << endreq;

   m_IOVDbSvc->dropObject(m_tubeFolder, true);
   return  sc; 
}

StatusCode MdtCalibDbCoolStrTool::loadRt(IOVSVC_CALLBACK_ARGS) 
{
  if( m_debug ) *m_log << MSG::DEBUG << "Load Rt Calibration from DB" << endreq;

//tr-relation creators
   MuonCalib::RtResolutionFromPoints res_from_points;
   MuonCalib::RtFromPoints rt_fromPoints;

  // retreive the and remove the old collection
  StatusCode sc = m_detStore->retrieve( m_rtData, m_rtDataLocation );
  if(sc.isSuccess())  {
   if( m_debug ) *m_log << MSG::DEBUG << "Rt Collection found " << m_rtData << endreq;   
   sc = m_detStore->removeDataAndProxy( m_rtData );
   if(sc.isSuccess())  {
     if( m_debug ) *m_log << MSG::DEBUG << "Rt Collection at " << m_rtData << " removed "<<endreq;
   }
  }

  // reinitialize the MdtRtRelationCollection
  sc=defaultRt();
  if(sc.isFailure()) return StatusCode::FAILURE;

                                                                                
  // retreive the collection of strings read out from the DB
  const CondAttrListCollection * atrc;
  sc=m_detStore->retrieve(atrc,m_rtFolder);
  if(sc.isFailure())  {
     *m_log << MSG::ERROR 
         << "could not retreive the CondAttrListCollection from DB folder " 
         << m_rtFolder << endreq;
     return sc;
  }

  // unpack the strings in the collection and update the
  // MdtTubeCalibContainers in TDS
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    bool rt_ts_applied=(atr["tech"].data<int>() & TIME_SLEWING_CORRECTION_APPLIED);
    std::string a,b,c;
     if (atr["data"].specification().type() == typeid(coral::Blob))
    	{
	if( m_verbose ) *m_log << MSG::VERBOSE << "Data load is a blob. Uncompressing"<< endreq;
	if(!uncompressInMyBuffer(atr["data"].data<coral::Blob>()))
		{
		(*m_log) << MSG::FATAL << "Cannot uncompress buffer" << endreq;
		return StatusCode::FAILURE;
		}
	std::istringstream istr(reinterpret_cast<char*>(m_decompression_buffer));
	istr >> a >> b >> c;
	if( m_verbose ) *m_log << MSG::VERBOSE << "Read string1:" << a << " string2:" << b
			   << " string3:" << c << endreq;
	}
   else
        {
	std::string data;
	data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    //data=atr("data").data<string>();
    //atr["data"].getValue(data);
	if( m_verbose ) *m_log << MSG::VERBOSE << "Data load is " << data << endreq;
    // interpret as string stream
	std::istringstream istr(data.c_str());
	istr >> a >> b >> c;
	if( m_verbose ) *m_log << MSG::VERBOSE << "Read string1:" << a << " string2:" << b
			   << " string3:" << c << endreq;
     	}                                                                       
    char * parameters = new char [a.size() +1];
    strncpy(parameters, a.c_str(), a.size() +1);
    parameters[a.size()]='\0';
    unsigned int regionId, npoints(0);
    char * pch;
    int i=0;
    pch = strtok (parameters," _,");
    regionId=atoi(pch);
    if(regionId>2500){
    // the clob contains the fixed identifier rather than the hash
      MuonFixedId id(regionId);
      Identifier athenaId=m_idToFixedIdTool->fixedIdToId(id);
      IdContext idCont = m_mdtIdHelper->module_context();
      MdtBasicRegionHash hash;
      m_mdtIdHelper->get_hash( athenaId , hash, &idCont );
      if(m_verbose) *m_log << MSG::VERBOSE << "Fixed region Id "<<regionId
                    <<" converted into athena Id "<<athenaId
                    <<" and then into hash "<<hash<<endreq;
      regionId=hash;
    }
    while (pch != NULL)
    {
      pch = strtok (NULL, "_,");
      i++;
      if (i==1) {
        npoints= atoi(pch);
      }
    }
    delete  []  parameters;                                                                          
    int k=0;
    unsigned int n=0;

    char * RTPar= new char [b.size() +1];
    strncpy(RTPar, b.c_str(), b.size() +1);
    RTPar[b.size()]='\0';
    char * pch1;
    pch1 = strtok (RTPar,",");
                                                                                
    MuonCalib::CalibFunc::ParVec rtPars;
    MuonCalib::CalibFunc::ParVec resoPars;
//    double t_min=0;
//    double bin_size=0;

    std::vector<SamplePoint> tr_points(0), ts_points(0);
    SamplePoint tr_point, ts_point;
    float multilayer_tmax_diff(-9e9);
    while(pch1 != NULL && n<=npoints) {
     ++k;
     if (k==1) {
       float rad=atof(pch1);
       
       if( m_rtShift != 0. ){
	 float oldrad = rad;
	 float rshift = m_rtShift*1.87652e-2*rad*(rad-14.6);
	 rad = oldrad + rshift;
	 if( m_debug ) *m_log << MSG::DEBUG  << "DEFORM RT: old radius " << oldrad << " new radius " 
			      << rad << " shift " << rshift << " max shift " << m_rtShift << std::endl;	   
       }
       
       if( m_rtScale !=1.){
//	 float oldrad = rad;
	 rad = rad * m_rtScale;
	 if( m_debug ) *m_log << MSG::DEBUG  << "DEFORM RT: old radius " << rad << " new radius " 
			      << rad << " scale factor " << m_rtScale << std::endl;	   
       }
       
       tr_point.set_x2(rad);
     }
     if (k==2) {
       float tim= atof(pch1);
        tr_point.set_x1(tim);
	ts_point.set_x1(tim);
     }
     if (k==3) {
       float sigma= atof(pch1);
       ts_point.set_x2(sigma);
       ts_point.set_error(1.0);
       tr_point.set_error(1.0);
       if(tr_point.x2()<-99)
       	        {
//		std::cout<<"XXxxXX Multilayer t_max diff found!"<<tr_point.x1() <<std::endl;
		multilayer_tmax_diff=tr_point.x1();
		}
       else if(n==0 || (tr_points[n-1].x1()<tr_point.x1() && tr_points[n-1].x2()<tr_point.x2()))
       		{
		tr_points.push_back(tr_point);	       
		ts_points.push_back(ts_point);
	       n++;
		}
       k=0;
     }
     pch1 = strtok (NULL, ", ");
   }
   delete [] RTPar;
   if(ts_points.size()<3)
	{
	*m_log<<MSG::FATAL<<"Rt relation broken!"<<endreq;
	*m_log<<MSG::FATAL <<"file='"<<atr["file"].data<std::string>()<<"'"<<endreq;
	*m_log<<MSG::FATAL<<"a='"<<a<<"'"<<endreq;
	
	return StatusCode::FAILURE;
	}
   if  (rt_ts_applied != m_TimeSlewingCorrection)
   	{
	float sign(rt_ts_applied ? -1.0 : 1.0);
	float slice_width=14.6/static_cast<float>(m_MeanCorrectionVsR.size());
	for(std::vector<SamplePoint>::iterator it=tr_points.begin(); it!=tr_points.end(); it++)
		{
		int slice_number=static_cast<int>(std::floor(it->x2()/slice_width));		
		if (slice_number<0)
			slice_number=0;
		if (slice_number >= static_cast<int>(m_MeanCorrectionVsR.size()))
			slice_number = static_cast<int>(m_MeanCorrectionVsR.size()) - 1;
		it->set_x1(it->x1() + sign * m_MeanCorrectionVsR[slice_number]);	
		}
	}

   MuonCalib::IRtResolution* reso = getRtResolutionInterpolation(ts_points);
   if (m_debug)
   	{
   *m_log << MSG::DEBUG  << "Resolution pints :"<<endreq;
   for(std::vector<SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++)
   	{
	*m_log << MSG::DEBUG  <<it->x1()<<"|"<<it->x2()<<"|"<<it->error()<<endreq;
	}
	
	
   *m_log << MSG::DEBUG  << "Resolution parameters :"<<endreq;
   for(unsigned int i=0; i<reso->nPar(); i++)
   	{
	 *m_log << MSG::DEBUG  << i<<" "<<reso->par(i) << endreq;
	}
	}
      try {
      MuonCalib::IRtRelation* rt =new RtRelationLookUp(rt_fromPoints.getRtRelationLookUp(tr_points));;
   if( reso && rt ){
     //if((*m_rtData)[regionId]) delete (*m_rtData)[regionId];
     if(regionId>=m_rtData->size())
     	{
	delete reso; delete rt;
	*m_log << MSG::WARNING << "Illegal regionId "<< regionId <<endreq;
	}
	else 
	{ 
	if (rt->par(1)==0.0)
		{
		*m_log << MSG::WARNING << "Bin size is 0"<< endreq;
   		for(std::vector<SamplePoint>::const_iterator it=tr_points.begin(); it!=tr_points.end(); it++)
			*m_log << MSG::WARNING << it->x1() << " " <<it->x2() <<" "<< it->error() << endreq;
		}
     if(multilayer_tmax_diff>-8e8)
     	{
	rt->SetTmaxDiff(multilayer_tmax_diff);
	}
     (*m_rtData)[regionId] = new MuonCalib::MdtRtRelation( rt, reso, 0.);
     if( m_debug ) *m_log << MSG::DEBUG << "Inserting new Rt for region "<<regionId<< endreq;
     }
   }
   }   
   catch (int i)
   	{
	 *m_log << MSG::FATAL << "Error in creating rt-relation!" <<endreq;
	 *m_log << MSG::FATAL << "npoints="<<tr_points.size()<<endreq;
	 *m_log << MSG::FATAL << "Offending input: a=" << a <<endreq;
	 *m_log << MSG::FATAL << "Offending input: b=" << b <<endreq;
	 return StatusCode::FAILURE;
	}

  }

/*   IOVRange range;
   sc = m_IOVSvc->getRange(1238547719, m_rtFolder, range);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange failed" << endreq;}
   if( m_debug ) *m_log << MSG::DEBUG <<"CondAttrListCollection IOVRange "<<range<<endreq;
                                                                                
   IOVRange range2;
   sc = m_IOVSvc->setRange(1270996316, m_rtDataLocation, range);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::setRange failed" << endreq;}
   sc = m_IOVSvc->getRange(1270996316, m_rtDataLocation, range2);
   if (sc.isFailure()){*m_log << MSG::WARNING << "IIOVSvc::getRange2 failed" << endreq;}
   if( m_debug ) *m_log << MSG::DEBUG <<"MdtRtRelationCollection new IOVRange "<<range2<<endreq;*/
                                                                                
   // finally record MdtRtRelationCollection
   sc = m_detStore->record( m_rtData, m_rtDataLocation );
   if (sc.isFailure()){*m_log << MSG::WARNING << "m_tubeData not recorded" << endreq;}

  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtRtRelationCollection>::ID(), m_rtDataLocation);
  if (!proxy) {
    *m_log << MSG::ERROR << "Unable to get the proxy for class MdtRtRelationCollection" << endreq;
    return StatusCode::FAILURE;
  }
                                                                                
  SG::TransientAddress* tad =  proxy->transientAddress();
  if (!tad) {
    *m_log << MSG::ERROR << "Unable to get the tad" << endreq;
    return StatusCode::FAILURE;
  }
                                                                                
  IAddressProvider* addp = this;
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  if( m_debug ) *m_log << MSG::DEBUG<< "set address provider for MdtRtRelationCollection" << endreq;
                                                                                
   m_IOVDbSvc->dropObject(m_rtFolder, true);
                                                                                

  return  StatusCode::SUCCESS; 
}

StatusCode MdtCalibDbCoolStrTool::defaultT0s()
{
  m_tubeData=NULL;
  // create collection
  if (m_detStore->contains<MdtTubeCalibContainerCollection>(m_tubeDataLocation))
  	{
	StatusCode sc=m_detStore->retrieve(m_tubeData, m_tubeDataLocation);
	if(!sc.isSuccess()) m_tubeData=NULL;
	}
  if(m_tubeData==NULL)
  	{
	*m_log<<MSG::INFO<<"Creating new tube data vector"<<endreq;
	m_tubeData = new MdtTubeCalibContainerCollection();
	}
  m_tubeData->resize( m_mdtIdHelper->module_hash_max() );
  if( m_debug ) *m_log << MSG::DEBUG << " Created new MdtTubeCalibContainerCollection size " << m_tubeData->size() << endreq;

  MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
  MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
  for( ; it!=it_end;++it ){
    
    MuonCalib::MdtTubeCalibContainer* tubes=0;
    // create a MdtTubeContainer
    tubes = buildMdtTubeCalibContainer(*it);

    if(tubes) {
      std::string rName=tubes->regionKey();
      double t0 = m_defaultT0;

      int nml=tubes->numMultilayers();
      int nlayers=tubes->numLayers();
      int ntubes=tubes->numTubes();
      int size = nml*nlayers*ntubes;
      if( m_verbose ) {
	*m_log << MSG::VERBOSE << "Adding chamber " << m_mdtIdHelper->print_to_string(*it) << endreq;
	*m_log << MSG::VERBOSE << " size " << size
	       << " ml " << nml << " l " << nlayers << " t " 
	       << ntubes << " address " << tubes << endreq;
      }
      for( int ml=0;ml<nml;++ml ){
	for( int l=0;l<nlayers;++l ){
	  for( int t=0;t<ntubes;++t ){
	    MuonCalib::MdtTubeCalibContainer::SingleTubeCalib data;
	    data.t0 = t0;
	    data.adcCal = 1.;
	    data.inversePropSpeed = 1./(299.792458*m_prop_beta);
	    tubes->setCalib( ml,l,t,data );
	  }
	}
      }
    }
    if( m_verbose ) *m_log << MSG::VERBOSE << " set t0's done " << endreq;
    MdtBasicRegionHash hash;
    IdContext idCont = m_mdtIdHelper->module_context();
    m_mdtIdHelper->get_hash( *it, hash, &idCont );

    if( hash < m_tubeData->size() ){
      (*m_tubeData)[hash] = tubes;
      if( m_verbose ) *m_log << MSG::VERBOSE << " adding tubes at " << hash << " current size " << m_tubeData->size() << endreq;
//write out string for chamberlist
      if(tubes)
    	{
      int nml=tubes->numMultilayers();
      int nlayers=tubes->numLayers();
      int ntubes=tubes->numTubes();	
      if( m_verbose ) *m_log << MSG::VERBOSE << "CHAMBERLIST: " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(*it)) << " " << m_mdtIdHelper->stationEta(*it) << " " << m_mdtIdHelper->stationPhi(*it) << " " << nml*nlayers*ntubes << " " << nml << " " << nlayers << " " << ntubes << " dummy " << hash << endreq;      
      }
    }else{
      if(tubes) delete tubes;
      *m_log << MSG::WARNING << " HashId out of range " << hash << " max " << m_tubeData->size() << endreq;
    }
  }
  if( m_debug )*m_log << MSG::DEBUG << " Done defaultT0s " << m_tubeData->size() << endreq;



  return StatusCode::SUCCESS;
}

MuonCalib::MdtTubeCalibContainer * MdtCalibDbCoolStrTool::buildMdtTubeCalibContainer(const Identifier& id)    
{    
    MuonCalib::MdtTubeCalibContainer* tubes = 0;

    const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID(id,1,1,1) );
    const MuonGM::MdtReadoutElement* detEl2 = 0;
    if (m_mdtIdHelper->numberOfMultilayers(id) == 2){
      detEl2 = m_detMgr->getMdtReadoutElement(m_mdtIdHelper->channelID(id,2,1,1) );
    }else{
      if( m_verbose ) *m_log << MSG::VERBOSE << "A single multilayer for this station " << m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id))<<","<< m_mdtIdHelper->stationPhi(id) <<","<< m_mdtIdHelper->stationEta(id) <<endreq;
    }

    if( m_verbose ) *m_log << MSG::VERBOSE << " new det el " << detEl << endreq;
    
    if( !detEl ){ 
      *m_log << MSG::INFO << "Ignoring non existing station in calibration db: " << m_mdtIdHelper->print_to_string(id) << endreq;
    }else{
      int nml = 2;
      if( !detEl2 ) {
	nml = 1;
      }

      int nlayers = detEl->getNLayers();
      if( detEl2 && detEl2->getNLayers() > nlayers ){
	if( m_debug ) *m_log << MSG::DEBUG << "Second multilayer has more layers " << detEl2->getNLayers() << " then first " << nlayers << endreq;
	nlayers = detEl2->getNLayers();
      }

      int ntubes = detEl->getNtubesperlayer();
      if( detEl2 && detEl2->getNtubesperlayer() > ntubes ){
	if( m_debug ) *m_log << MSG::DEBUG << "Second multilayer has more tubes " << detEl2->getNtubesperlayer() << " then first " << ntubes << endreq;
	ntubes = detEl2->getNtubesperlayer();
      }

      //      int size = nml*nlayers*ntubes;
      // build the region name in the format STATION_ETA_PHI
      std::string rName;

      int stName =  m_mdtIdHelper->stationName(id);
      int stPhi = m_mdtIdHelper->stationPhi(id);
      int stEta = m_mdtIdHelper->stationEta(id);
  
      std::string seperator("_");
      MuonCalib::ToString ts;
      rName = m_mdtIdHelper->stationNameString(stName);
      rName += seperator + ts( stPhi ) + seperator + ts( stEta );
      tubes=new MuonCalib::MdtTubeCalibContainer( rName,nml, nlayers, ntubes );
    }

    return tubes;
}

StatusCode MdtCalibDbCoolStrTool::defaultRt()
{
  m_rtData=NULL;
  // create collection
  if (m_detStore->contains<MdtRtRelationCollection>(m_rtDataLocation))
  	{
	StatusCode sc=m_detStore->retrieve(m_rtData, m_rtDataLocation);
	if(!sc.isSuccess())
		m_rtData=NULL;
	}
  if(m_rtData==NULL)
  	{
	*m_log<<MSG::INFO<<"Creating new tube data vector"<<endreq;
  	m_rtData = new MdtRtRelationCollection(); 
	}
 
  // create collection 
  m_rtData->resize(m_regionSvc->numberOfRtRegions());
  if( m_debug ) *m_log << MSG::DEBUG << " Created new MdtRtRelationCollection size " << m_rtData->size() << endreq;
  
  std::vector<std::string>::const_iterator it     = m_RTfileNames.value().begin();
  std::vector<std::string>::const_iterator it_end = m_RTfileNames.value().end();

  if (it == it_end ) {
     *m_log << MSG::FATAL << "No input RT file declared in jobOptions"<<endreq;
     return StatusCode::FAILURE;
  } else if (it_end-it>1) {
     *m_log << MSG::WARNING << "Only first RT file declared in jobOptions will be used"<<endreq;
  }

  //for ( ; it != it_end ; ++it) {
    
    std::string fileName = 
      PathResolver::find_file(it->c_str(),"DATAPATH");
    if(fileName.length() == 0){
      *m_log << MSG::ERROR << "RT Ascii file \"" <<  it->c_str() << "\" not found" << endreq;
    }
    // Open the Ascii file with the RT relations
    std::ifstream inputFile( fileName.c_str() );
    if( !inputFile ) {
      *m_log << MSG::ERROR << "Unable to open RT Ascii file: " << fileName.c_str() << endreq;
      return StatusCode::FAILURE;
    }else{
      if( m_debug ) *m_log << MSG::DEBUG << "Opened RT Ascii file: " <<  fileName.c_str() << endreq;
    }
    MuonCalib::RtDataFromFile rts;
    rts.read(inputFile);
    if( m_verbose ) *m_log << MSG::VERBOSE << "File contains " << rts.nRts() << " RT relations " << endreq;

    for(unsigned int n=0;n<rts.nRts();++n){
      MuonCalib::RtDataFromFile::RtRelation* rt = rts.getRt( n );
      unsigned int regionId = rt->regionId();

      if( regionId >= m_rtData->size() ){
	*m_log << MSG::WARNING << " regionHash out of range: " << regionId << " max " << m_rtData->size() << endreq;
	delete rt;
	continue;
      }
      
      const MuonCalib::RtDataFromFile::RtRelation::DataVec& times  = rt->times();
      const MuonCalib::RtDataFromFile::RtRelation::DataVec& radii  = rt->radii();
      const MuonCalib::RtDataFromFile::RtRelation::DataVec& reso   = rt->resolution();
	
       
      // check if rt contains data, at least two point on the rt are required
      if( times.size() < 2 ) {
	*m_log << MSG::ERROR << " defaultRt rt table has too few entries"
	    << endreq;
	continue;
      }

      // check if all tables have same size
      if( times.size() != radii.size() || times.size() != reso.size() ) {
	*m_log << MSG::ERROR << "defaultRt rt table size mismatch "
	    << endreq;
	continue;
      }

      // check if
      double t_min = times[0];
      double bin_size = times[1]-t_min;

      // additional consistency check 
      if( bin_size <= 0 ) {
	*m_log << MSG::ERROR<< "RtCalibrationClassic::defaultRt rt table negative binsize "
	    << endreq;
	continue;
      }

      // create a vector to hold the r values, 
      // we need two extra fields to store t_min and bin_size
      MuonCalib::CalibFunc::ParVec rtPars;
      rtPars.push_back( t_min );
      rtPars.push_back( bin_size );

      // copy r values into vector
      rtPars.insert( rtPars.end(), radii.begin(), radii.end() );

      if( m_debug ) *m_log << MSG::DEBUG << "defaultRt new  MuonCalib::IRtRelation" 
			   << endreq;

      MuonCalib::CalibFunc::ParVec resoPars;
      resoPars.push_back( t_min );
      resoPars.push_back( bin_size );

      // copy r values into vector
      resoPars.insert( resoPars.end(), reso.begin(), reso.end() );

      if( m_debug ) *m_log << MSG::DEBUG << "defaultRt new  MuonCalib::IRtResolution" 
			   << endreq;

      for(unsigned int j=0; j<m_rtData->size();j++) {
        MuonCalib::IRtRelation* rtRel = 
        MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );
        if( !rtRel ){
     	  *m_log << MSG::WARNING << "ERROR creating RtRelationLookUp " 
              << std::endl;
        }
        MuonCalib::IRtResolution* resoRel = 
        MuonCalib::MdtCalibrationFactory::createRtResolution( "RtResolutionLookUp", resoPars );
        if( !rtRel ){
	  *m_log << MSG::WARNING << "ERROR creating RtResolutionLookUp " 
              << endreq;
        }
        if( resoRel && rtRel ){
	  (*m_rtData)[j] = new MuonCalib::MdtRtRelation( rtRel, resoRel, 0. );
	  if( m_debug ) *m_log << MSG::DEBUG << "Inserting default Rt for region "<<j<< endreq;
        }

        if(j==0 && rtRel && resoRel){
          int npoints= rtRel->nPar()-2;
          if( m_verbose ) *m_log << MSG::VERBOSE 
				 << "defaultRt npoints from rtRel="<< npoints<< endreq;

          for( int j=0;j<npoints;++j ){
	    double t = t_min + j*bin_size;
	    if( m_verbose ) *m_log<<MSG::VERBOSE << "  " << j << " " << t << "  " 
				  << rtRel->radius(t) << " " << resoRel->resolution(t) << endreq;
          }
        }
    if(!resoRel || !rtRel) {
	  if(resoRel) delete resoRel;
	  if(rtRel) delete rtRel;
	  }
      }
      delete rt;
    }
  //}
  return StatusCode::SUCCESS;

}


inline bool MdtCalibDbCoolStrTool :: uncompressInMyBuffer(const coral::Blob & blob)
	{
	if (!m_decompression_buffer)
		{
		m_buffer_length= 50000;
		m_decompression_buffer = new Bytef[m_buffer_length];
		}
	uLongf actual_length;	
	while(1)
		{
		actual_length=m_buffer_length;
		int res(uncompress(m_decompression_buffer, &actual_length, reinterpret_cast<const Bytef *>(blob.startingAddress()), static_cast<uLongf>(blob.size())));
		if (res == Z_OK) break;
	//double buffer if it was not big enough
		if( res == Z_BUF_ERROR)
			{
			m_buffer_length*=2;
			 *m_log<<MSG::VERBOSE << "Increasing buffer to " << m_buffer_length<<endreq;
			delete [] m_decompression_buffer;
			m_decompression_buffer = new Bytef[m_buffer_length];
			continue;
			}
	//somethinh else is wrong
		return false;
		}
	//append 0 to terminate string, increase buffer if it is not big enough
	if (actual_length >= m_buffer_length)
		{
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




//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::
//:: METHOD getRtResolutionInterpolation ::
//:::::::::::::::::::::::::::::::::::::::::

 inline RtResolutionLookUp * MdtCalibDbCoolStrTool::getRtResolutionInterpolation(
				const std::vector<SamplePoint> & sample_points) {

///////////////
// VARIABLES //
///////////////

	Double_t *x = new Double_t[sample_points.size()];
	Double_t *y = new Double_t[sample_points.size()];
	
	for (unsigned int i=0; i<sample_points.size(); i++)
		{
		x[i]=sample_points[i].x1();
		y[i]=sample_points[i].x2();
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
		Double_t xx=x[0]+k*bin_width;
		res_param[k+2] = sp.Eval(xx);
		if (std::isnan(res_param[k+2]))
			{
			TFile outf("kacke.root", "RECREATE");
			sp.Write("kacke");
			exit(99);
			}
	}
	delete [] x;
	delete [] y;
	return new RtResolutionLookUp(res_param);
	
}

}
