/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

//#include "StoreGate/StoreGate.h"
//#include "StoreGate/StoreGateSvc.h"


#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MDTcabling/MDTcablingSvc.h"

#include "AmdcAth/AmdcsimrecAthenaSvc.h"

#include "PathResolver/PathResolver.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonCondInterface/IMDTCablingDbTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <TH1F.h>

using namespace std;

MDTcablingSvc::MDTcablingSvc(const std::string& name,ISvcLocator* sl) :
    Service(name,sl),
    m_pMdtIdHelper(NULL),
    m_cabling_updated(false),
    m_alignment_updated(false),
    m_histProp_barrel_alignCorrOnRadius(Gaudi::Histo1DDef("Barrel_AlignCorr_Radius",-10.,10.,100)),
    m_histProp_barrel_alignCorrOnZeta(Gaudi::Histo1DDef("Barrel_AlignCorr_Zeta",-10.,10.,100)),
    m_histProp_endcap_alignCorrOnRadius(Gaudi::Histo1DDef("Endcap_AlignCorr_Radius",-10.,10.,100)),
    m_histProp_endcap_alignCorrOnZeta(Gaudi::Histo1DDef("Endcap_AlignCorr_Zeta",-10.,10.,100)),
    m_pDetStore("DetectorStore",name),
    m_AmdcsimrecAthenaSvc ("AmdcsimrecAthenaSvc",name),
    m_hist_barrel_alignCorrOnRadius(0),
    m_hist_barrel_alignCorrOnZeta(0),
    m_hist_endcap_alignCorrOnRadius(0),
    m_hist_endcap_alignCorrOnZeta(0),
    m_dbTool("MDTCablingDbTool/MDTCablingDbTool")
{
    declareProperty("DetStore",m_pDetStore,"Detector Store");
    declareProperty("AMDC", m_AmdcsimrecAthenaSvc, "AMDC geometry service");
    
    declareProperty("DisableTagInfoMatch", m_hasTagInfoMatched = false,
       "Disable the callback for TagInfo");
    
    declareProperty("DisableDBcablingLoad", m_do_not_loadFromDB = true,
       "Disable the callback for loading the DB cabling map");

    declareProperty( "DoNotUseAlignment", m_doNotUseAlignment=false,
          "Switch for turning off the use of alignment corrections");

    declareProperty( "UseOldGeoAccess", m_useOldGeoAccess=true,
          "Switch for using the old geometry access");

    declareProperty( "MapFlag", m_map_flag=false,
       "Switch on the debug printout of the complete chamber mapping" );
    declareProperty( "GeoFlag", m_geo_flag=false,
       "Switch on the debug printout of the geometry parameters used" );
    declareProperty( "CheFlag", m_che_flag=false,
       "Switch on the debug printouts of the TDC chamber map" );
    declareProperty( "RemFlag", m_rem_flag=false,
       "Switch on the debug printouts of the TDC remapping" );

    declareProperty( "RgsOut", m_rgs_out=false,
       "Switch on the output of the RegionSelector file" );
    declareProperty( "GeoOut", m_geo_out=false,
       "Dump the MDT geometry into a separate file" );
    declareProperty( "ReadFlag", m_read_flag=false,
       "Read the MDT geometry from a separate file" );

    declareProperty( "RGSfile", m_rgs_filename="RegionSelector.data",
       "Name of the RegionSelector file to be output" );
    declareProperty( "GEOfile", m_geo_filename="GEOmap.data",
       "Name of the geometry file to be output" );
    declareProperty( "RODfile", m_rod_filename="RODmap.data",
       "Name of the ROD file to be read" );
    declareProperty( "CABfile", m_rem_filename="",
       "Name of the cabling file to be read" );
    declareProperty( "MEZfile", m_mez_filename="",
       "Name of the cabling map file to be read" );
    declareProperty( "ShiftLinks", m_shift_LinkId=true,
       "Switch on the shift of the MRDO Links" );
       
    declareProperty( "Use_cm", m_unit_in_cm=true,
       "Switch for using the cm unit measurement" );
       
    declareProperty( "Barrel_AlignCorr_Radius",m_histProp_barrel_alignCorrOnRadius);
    declareProperty( "Barrel_AlignCorr_Zeta",m_histProp_barrel_alignCorrOnZeta);
    declareProperty( "Endcap_AlignCorr_Radius",m_histProp_endcap_alignCorrOnRadius);
    declareProperty( "Endcap_AlignCorr_Zeta" ,m_histProp_endcap_alignCorrOnZeta);
       
}

// queryInterface 
StatusCode MDTcablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if(IID_IMDTcablingSvc.versionMatch(riid) ) 
    { 
        *ppvIF = (IMDTcablingSvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode MDTcablingSvc::initialize() 
{

    MsgStream log(messageService(), name());
    log << MSG::INFO << "Skipping initialization of old MDTcablingSvc" << endreq;
    
    return StatusCode::SUCCESS;

    StatusCode sc;


    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;

    log << MSG::DEBUG << name() << ": Start of initialisation" << endreq;
    log << MSG::INFO << "==== Global  configuration ====" << endreq;
    log << MSG::INFO << m_hasTagInfoMatched << endreq;
    log << MSG::INFO << m_do_not_loadFromDB << endreq;
    log << MSG::INFO << m_doNotUseAlignment << endreq;
    log << MSG::INFO << m_useOldGeoAccess   << endreq;
    log << MSG::INFO << m_rod_filename << endreq;
    log << MSG::INFO << m_mez_filename << endreq;
    log << MSG::INFO << m_rem_filename << endreq;
    log << MSG::INFO << m_shift_LinkId << endreq;
    log << MSG::INFO << m_unit_in_cm << endreq;
    log << MSG::INFO << "===== Debug configuration =====" << endreq;
    log << MSG::INFO << m_map_flag << endreq;
    log << MSG::INFO << m_geo_flag << endreq;
    log << MSG::INFO << m_che_flag << endreq;
    log << MSG::INFO << m_rem_flag << endreq;
    log << MSG::INFO << "==== Output  configuration ====" << endreq;
    log << MSG::INFO << m_rgs_out << endreq;
    log << MSG::INFO << m_geo_out << endreq;
    log << MSG::INFO << m_read_flag << endreq;
    log << MSG::INFO << m_rgs_filename << endreq;
    log << MSG::INFO << m_geo_filename << endreq;


    
    // retrieve the AMDC geometry
    if (m_AmdcsimrecAthenaSvc.retrieve().isFailure()) {
        log << MSG::FATAL << "Failed to retrieve service " 
                          << m_AmdcsimrecAthenaSvc << endreq;
        return StatusCode::FAILURE;
    }
    
    //retrieve the detector store
    if (m_pDetStore.retrieve().isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found!" << endreq;
        return StatusCode::FAILURE;
    }
    
    m_cablingMap = 0;
    
    if(m_do_not_loadFromDB){ 
        log << MSG::WARNING 
	    << "MDTcabling is forced not to use callbacks for the cabling."
	    << "Callback to MDT cabling folders not registered."
	    << endreq;
        m_cabling_updated = true;
    } else {
    
        if ( m_dbTool.retrieve().isFailure()) {
            log << MSG::FATAL << "Could not find tool " << m_dbTool 
	                      << ". Exiting." << endreq;
            return StatusCode::FAILURE;
        } else {
            log << MSG::DEBUG << "Database tool \"" << m_dbTool 
	                      << "\" retrieved." << endreq;
        }
        
	log << MSG::INFO 
	    << "Registering for callback the loadCablingMap function" << endreq;

        std::string mapFolderName = m_dbTool->mapFolderName();
        log << MSG::INFO << "Map folder name: " << m_dbTool->mapFolderName() 
	    << endreq;

        const DataHandle<CondAttrListCollection> MapData;

        sc = m_pDetStore->regFcn(&MDTcablingSvc::loadCablingMap,
                                 this, MapData, mapFolderName);
   
        if (sc.isFailure()) {

            log << MSG::ERROR
                << "Could not register loadCablingMap function for callback against: " 
		<< mapFolderName << endreq;
            return sc;
        }

        // register the mezzanine folder for callback
        std::string mezzanineFolderName = m_dbTool->mezzanineFolderName();
        log << MSG::INFO << "Mezzanine folder name: " 
                         << mezzanineFolderName << endreq;
       
        const DataHandle<CondAttrListCollection> MapData_mez;
       
        sc = m_pDetStore->regFcn(&MDTcablingSvc::loadCablingMap,
                                 this,MapData_mez, mezzanineFolderName);
       
        if (sc.isFailure()) {
         
            log << MSG::ERROR
                << "Could not register loadCablingMap function for callback against: "
	        << mezzanineFolderName
                << endreq;
            return sc;
        }
	
    }
    
    if (!m_AmdcsimrecAthenaSvc->InitializedSvc()) {
        if (m_doNotUseAlignment) {
            log << MSG::WARNING 
	        << "AMDC needs callback but MDTcabling is forced not to use it."
	        << "Callback to updateAMDC not registered."
	        << endreq;
	    m_alignment_updated = true;
	} else {
            sc = m_pDetStore->regFcn(
                  &AmdcsimrecAthenaSvc::UpdatedSvc,(&*m_AmdcsimrecAthenaSvc),
                  &MDTcablingSvc::updateAMDC,this,true);
            if (sc.isFailure()) {
                log << MSG::FATAL 
		    << "Unable to register callback on updateAMDC "
                    << "from AmdcsimrecAthenaSvc::UpdatedSvc " << endreq;
                return StatusCode::FAILURE;
            }
            log << MSG::INFO << "Registered callback on updateAMDC "
                << "from AmdcsimrecAthenaSvc::UpdatedSvc " << endreq;
        }
    }
 
    // retrieve the MuonIdHelpers
    sc = m_pDetStore->retrieve(m_pMdtIdHelper, "MDTIDHELPER");
    if (sc.isFailure() || m_pMdtIdHelper == NULL) {
        log << MSG::FATAL << "Could not get MdtIdHelper!" << endreq;
        return sc;
    }
    

    // Register callbalcks if needed
    
    // ===================  TAG INFO =====================
    const DataHandle<TagInfo> tagInfoH;
    std::string tagInfoKey = "";

    ITagInfoMgr*  pTagInfoMgr = NULL;
    sc = service("TagInfoMgr", pTagInfoMgr);
    if(sc.isFailure() || pTagInfoMgr==0) {
        log << MSG::WARNING 
	    << " Unable to locate TagInfoMgr service" << endreq;
    }
    else {
        tagInfoKey = pTagInfoMgr->tagInfoKey();
    }

    //register the call-back to matchTagInfo
    if(!m_hasTagInfoMatched) {
        sc = m_pDetStore->regFcn(&MDTcablingSvc::matchTagInfo,
                                 this,tagInfoH,tagInfoKey);

        if( sc.isFailure() ) {
            log << MSG::WARNING
            << "Cannot register matchTagInfo function for key "
            << tagInfoKey << endreq;
        }
        else {
            log << MSG::DEBUG
            << "Registered matchTagInfo callback for key: "
            << tagInfoKey << endreq;
        }
    }

    
    // printout the histogram property
    log << MSG::INFO << "Hist:Barrel_AlignCorr_Radius = " 
                     << m_hist_barrel_alignCorrOnRadius << endreq;
    log << MSG::INFO << "Barrel_AlignCorr_Zeta        = "
                     << m_hist_barrel_alignCorrOnZeta << endreq;
    log << MSG::INFO << "Endcap_AlignCorr_Radius      = "
                     << m_hist_endcap_alignCorrOnRadius << endreq;
    log << MSG::INFO << "Endcap_AlignCorr_Zeta        = "
                     << m_hist_endcap_alignCorrOnZeta << endreq; 


    // build geomettry map if everything is correctly configured
    sc = buildMdtGeometry();
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
            log << MSG::INFO << "Postpone MDTcabing creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }

    return StatusCode::SUCCESS;
}

StatusCode MDTcablingSvc::buildMdtGeometry() 
{ 
    MsgStream log(messageService(), name());
    
    log << MSG::DEBUG << "AMDC initialization is "
                      << m_AmdcsimrecAthenaSvc->InitializedSvc() << endreq;
    log << MSG::DEBUG << m_alignment_updated << endreq;
    log << MSG::DEBUG << m_cabling_updated << endreq;
    log << MSG::DEBUG << m_hasTagInfoMatched << endreq;
    
    if(!m_hasTagInfoMatched) {
        log << MSG::INFO << "Cannot build " << name()
                         << " now, becasue a TagInfo match is required" 
                         << endreq;
        return StatusCode::RECOVERABLE;
    }

    // configure MDTGeometry
    MDTGeometry::s_pMdtIdHelper = m_pMdtIdHelper;
    
    if (!m_AmdcsimrecAthenaSvc->InitializedSvc() && !m_alignment_updated) {
        log << MSG::INFO << "Cannot build " << name()
                         << " now, becasue AMDC is not yet initialized" 
                         << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    if (!m_cabling_updated && m_cablingMap==0) {
        log << MSG::INFO << "Cannot build " << name()
                         << " now, becasue the cabling map is not loaded from database" 
                         << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    
    
    // find histogramming service
    ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
      if ((rootHistSvc.retrieve()).isFailure()) {
        log << MSG::ERROR << "Unable to locate THistSvc" << endreq;
        rootHistSvc.release().ignore();
        return StatusCode::FAILURE;
    }
    
    
    log << MSG::INFO << "Building MDTgeometry LUT" << endreq;
    
    std::string histopath = std::string("/EXPERT/MDTgeometry/");
    
    // reset, create and register the histogram monitoring the alignment
    if(m_hist_barrel_alignCorrOnRadius==0) {
        m_hist_barrel_alignCorrOnRadius = new TH1F 
         (m_histProp_barrel_alignCorrOnRadius.value().title().c_str(),
         (m_histProp_barrel_alignCorrOnRadius.value().title()+";diff").c_str(),
          m_histProp_barrel_alignCorrOnRadius.value().bins(),
          m_histProp_barrel_alignCorrOnRadius.value().lowEdge(),
          m_histProp_barrel_alignCorrOnRadius.value().highEdge());

        if (m_hist_barrel_alignCorrOnRadius) {
	    // 2012.12.20 kn shut off kCanRebin
	    // m_hist_barrel_alignCorrOnRadius->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_barrel_alignCorrOnRadius->GetName(), 
	       m_hist_barrel_alignCorrOnRadius).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_barrel_alignCorrOnRadius->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_barrel_alignCorrOnZeta==0) {
        m_hist_barrel_alignCorrOnZeta = new TH1F 
         (m_histProp_barrel_alignCorrOnZeta.value().title().c_str(),
         (m_histProp_barrel_alignCorrOnZeta.value().title()+";diff").c_str(),
          m_histProp_barrel_alignCorrOnZeta.value().bins(),
          m_histProp_barrel_alignCorrOnZeta.value().lowEdge(),
          m_histProp_barrel_alignCorrOnZeta.value().highEdge());

        if (m_hist_barrel_alignCorrOnZeta) {
	    // 2012.12.20 kn shut off kCanRebin
	    // m_hist_barrel_alignCorrOnZeta->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_barrel_alignCorrOnZeta->GetName(), 
	       m_hist_barrel_alignCorrOnZeta).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_barrel_alignCorrOnZeta->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_endcap_alignCorrOnRadius==0) {
        m_hist_endcap_alignCorrOnRadius = new TH1F 
         (m_histProp_endcap_alignCorrOnRadius.value().title().c_str(),
         (m_histProp_endcap_alignCorrOnRadius.value().title()+";diff").c_str(),
          m_histProp_endcap_alignCorrOnRadius.value().bins(),
          m_histProp_endcap_alignCorrOnRadius.value().lowEdge(),
          m_histProp_endcap_alignCorrOnRadius.value().highEdge());

        if (m_hist_endcap_alignCorrOnRadius) {
	    // 2012.12.20 kn shut off kCanRebin
	    // m_hist_endcap_alignCorrOnRadius->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_endcap_alignCorrOnRadius->GetName(), 
	       m_hist_endcap_alignCorrOnRadius).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_endcap_alignCorrOnRadius->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_endcap_alignCorrOnZeta==0) {
        m_hist_endcap_alignCorrOnZeta = new TH1F 
         (m_histProp_endcap_alignCorrOnZeta.value().title().c_str(),
         (m_histProp_endcap_alignCorrOnZeta.value().title()+";diff").c_str(),
          m_histProp_endcap_alignCorrOnZeta.value().bins(),
          m_histProp_endcap_alignCorrOnZeta.value().lowEdge(),
          m_histProp_endcap_alignCorrOnZeta.value().highEdge());

        if (m_hist_endcap_alignCorrOnZeta) {
	    // 2012.12.20 kn shut off kCanRebin
            // m_hist_endcap_alignCorrOnZeta->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_endcap_alignCorrOnZeta->GetName(), 
	       m_hist_endcap_alignCorrOnZeta).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_endcap_alignCorrOnZeta->GetName() << endreq;
            }
        }
    }
    
    rootHistSvc.release().ignore();

    if( m_rod_filename.value()=="") {
      if( m_pDetStore->retrieve(m_cablingMap).isFailure() ) {
        log << MSG::ERROR << "loadCablingMap: can't retrieve the cabling map from the detector store" << endreq;
        return StatusCode::FAILURE;
      }
      MDTGeometry::s_cabling_map = m_cablingMap;
    }

    MDTGeometry::s_useOldGeoAccess = m_useOldGeoAccess;
    MDTGeometry::s_amdc            = m_AmdcsimrecAthenaSvc->GetAmdcsimrec();

    MDTGeometry::MapFlag = m_map_flag;
    MDTGeometry::GeoFlag = m_geo_flag;
    MDTGeometry::CheFlag = m_che_flag;
    MDTGeometry::RemFlag = m_rem_flag;

    MDTGeometry::RgsOut = m_rgs_out;
    MDTGeometry::GeoOut = m_geo_out;
    MDTGeometry::ReadFlag = m_read_flag;

    MDTGeometry::RgsFile = m_rgs_filename;
    MDTGeometry::GeoFile = m_geo_filename;
    
    #ifndef LVL1_STANDALONE
    MDTGeometry::s_hist_barrel_alignCorrOnRadius = m_hist_barrel_alignCorrOnRadius;
    MDTGeometry::s_hist_barrel_alignCorrOnZeta   = m_hist_barrel_alignCorrOnZeta;
    MDTGeometry::s_hist_endcap_alignCorrOnRadius = m_hist_endcap_alignCorrOnRadius;
    MDTGeometry::s_hist_endcap_alignCorrOnZeta   = m_hist_endcap_alignCorrOnZeta;
    #endif
    
    log << MSG::INFO << "after loading data to MDTGeometry" << endreq;

    // implement the search of Remapping file trought the pathresolver utility.
    std::string remappingFile = m_rem_filename;
    if (remappingFile.length() != 0) {

	std::string rem_filename;
        rem_filename = PathResolver::find_file (remappingFile, "DATAPATH");

	if (rem_filename == "") {
            log << MSG::ERROR 
                << "Cannot locate " << m_rem_filename 
	        << " from ${DATAPATH}" << endreq;
            return StatusCode::FAILURE;
        }

	MDTGeometry::CABFile = rem_filename;

        log << MSG::INFO << "Configuring mezanine map from file "
	    << MDTGeometry::CABFile << endreq;
	    
    } else MDTGeometry::CABFile = m_rem_filename;
    
    std::string mezanineFile = m_mez_filename;
    if (mezanineFile.length() != 0) {
        
	std::string mez_filename;
        mez_filename = PathResolver::find_file (mezanineFile, "DATAPATH");

	if (mez_filename == "") {
            log << MSG::ERROR 
                << "Cannot locate " << m_mez_filename 
	        << " from ${DATAPATH}" << endreq;
            return StatusCode::FAILURE;
        }

	MDTGeometry::MEZFile = mez_filename;
	
	log << MSG::INFO << "Reading mezanine map from file "
	    << MDTGeometry::MEZFile << endreq;
	    
    } else MDTGeometry::MEZFile = m_mez_filename;
    
    
    MDTGeometry::InstallAreaDir = PathResolver::find_directory("", "DATAPATH");

    MDTGeometry::s_testbeam = false;
    MDTGeometry::s_shift_LinkId = m_shift_LinkId;
    
    
    // implement the search of RODFile trought the pathresolver utility.
    std::string rodFile = m_rod_filename;
    if (rodFile.length() != 0) {
    
        std::string rod_filename = PathResolver::find_file (rodFile, "DATAPATH");

        if (rod_filename == "")
        {
            log << MSG::ERROR 
                << "Cannot locate " << m_rod_filename 
	        << " from ${DATAPATH}" << endreq;
            return StatusCode::FAILURE;
	}
	
	MDTGeometry::RODFile = rod_filename;
    
        log << MSG::INFO << "Reading ROD map from file "
	                 << MDTGeometry::RODFile << endreq;
	
    } else MDTGeometry::RODFile = m_rod_filename;


	    
    // set unit to be used
    if(m_unit_in_cm) MDTGeometry::s_ScaleFactor = CLHEP::centimeter;
    else             MDTGeometry::s_ScaleFactor = CLHEP::millimeter;
    
    MDTGeometry::s_pi = CLHEP::pi;
    
    // Instanciate the Muon geometry
    MDTGeometry::instance();
    
    if(! MDTGeometry::s_status)
    {
        log << MSG::ERROR <<   "MDT geometry model is not loaded!"  << endl 
            << endreq;
        delete MDTGeometry::s_instance;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode MDTcablingSvc::finalize() 
{
    delete MDTGeometry::s_instance;
    return StatusCode::SUCCESS;
} 

// Implement the specifics ... 

bool 
MDTcablingSvc::getOnlineIDfromOfflineID(int name, int StationEta,
                                  int StationPhi,int MultiLayer, int TubeLayer,
				  int Tube, 
                                  unsigned short int& SubsystemId,
				  unsigned short int& MrodId,
				  unsigned short int& LinkId,
				  unsigned short int& TdcId,
				  unsigned short int& ChannelId) const
{
    const MDTGeometry* geo = MDTGeometry::s_instance;
    if (geo) {
        return geo->getOnlineIDfromOfflineID(name,
	                                 StationEta,StationPhi,MultiLayer,
                                         TubeLayer,Tube,SubsystemId,MrodId,
                                         LinkId,TdcId,ChannelId);
    } else {
        SubsystemId = 0xffff;
	MrodId      = 0xffff;
	LinkId      = 0xffff;
	TdcId       = 0xffff; 
	ChannelId   = 0xffff;
	return false;
    }
}

bool 
MDTcablingSvc::getOfflineIDfromOnlineID(unsigned short int SubsystemId,
				  unsigned short int MrodId,
				  unsigned short int LinkId,
				  unsigned short int TdcId,
				  unsigned short int ChannelId,
                                  int& name, int& StationEta,
                                  int& StationPhi,int& MultiLayer, 
                                  int& TubeLayer,int& Tube) const
{
    const MDTGeometry* geo = MDTGeometry::s_instance;
    if(geo) {
        return geo->getOfflineIDfromOnlineID(SubsystemId,MrodId,LinkId,TdcId,
				         ChannelId,name,StationEta,StationPhi,
                                         MultiLayer,TubeLayer,Tube);
    } else {
        name       = 9999;
	StationEta = 9999;
        StationPhi = 9999;
	MultiLayer = 9999;
	TubeLayer  = 9999;
	Tube       = 9999;
        return false;
    }
}

const MdtIdHelper *
MDTcablingSvc::idHelper() const
{
    return m_pMdtIdHelper;
}


const MDTGeometry::ROBid 
MDTcablingSvc::getROBid(float phiMin,float PhiMax,
                        float etaMin,float etaMax) const
{
    const MDTGeometry* geo = MDTGeometry::s_instance;
    if (geo) {
        return geo->getROBid(phiMin,PhiMax,etaMin,etaMax);
    } else {
        return std::set< uint32_t >();
    }
}


const MDTGeometry::HASHid 
MDTcablingSvc::getHASHid(float phiMin,float PhiMax,
                         float etaMin,float etaMax) const
{
    const MDTGeometry* geo = MDTGeometry::s_instance;
    if(geo) {   
        return geo->getHASHid(phiMin,PhiMax,etaMin,etaMax);
    } else {
        return std::set< uint32_t >();
    }
}



const MDTGeometry*
MDTcablingSvc::mdtGeometry() const
{
    return MDTGeometry::s_instance;
}

bool
MDTcablingSvc::isTestBeamSetup() const
{
    return false;
}


StatusCode 
MDTcablingSvc::matchTagInfo(IOVSVC_CALLBACK_ARGS){
    
    MsgStream log(msgSvc(), name());
    bool tagMatch = true;

    log << MSG::DEBUG << "matchTagInfo() callback triggered" << endreq;

    std::string confTag;
    if ((std::string)m_mez_filename=="AtlasMEZmap.data" && 
        (std::string)m_rod_filename=="AtlasRODmap.data") confTag = "Atlas";
    else confTag = "old Montecarlo";

    log << MSG::DEBUG << "MDT cabling configuration from python: " 
        << confTag << endreq;

    // Get TagInfo and retrieve tags
    const TagInfo* tagInfo = 0;
    StatusCode sc = m_pDetStore->retrieve(tagInfo);

    std::string cablingType;

    if (sc.isFailure() || tagInfo==0) {
        log << MSG::DEBUG
        << "No TagInfo in DetectorStore while attempting to compare tags"
        << endreq;
    }
    else {
        tagInfo->findInputTag("MDT_CablingType", cablingType);

        log << MSG::DEBUG
            << "MDT_CablingType from TagInfo: " << cablingType << endreq;

        if(cablingType=="") {
            // assume old Montecarlo configuration by default
            if (confTag!="old Montecarlo") {
	        log << MSG::INFO
		 << "TagInfo not present, but MDTcabling configured for ATLAS!"
	            << endreq;
                log << MSG::INFO
		    << "Setting configuration for old Montecarlo."
	            << endreq;
		m_rod_filename = "RODmap.data";
	        m_mez_filename = "";
	        m_shift_LinkId = true;
            }
	}
        if (confTag=="Atlas") {
            tagMatch = (cablingType != "OldMDT_Cabling");
        }
        else {
            tagMatch = (cablingType != "NewMDT_Cabling");
        }
    }

    if (!tagMatch) {
        if (confTag=="Atlas") {
            log << MSG::INFO
                << "TagInfo configured for Montecarlo cabling, but MDTcabling configured for Atlas"
                << endreq;
            log << MSG::INFO << "Setting configuration for old Montecarlo" 
	        << endreq;
	    m_rod_filename = "RODmap.data";
	    m_mez_filename = "";
	    m_shift_LinkId = true;
        }
        else {
            log << MSG::INFO
                << "TagInfo configured for Atlas cabling, but MDTcabling configured for old Montecarlo"
                << endreq;
            log << MSG::INFO << "Setting configuration for Atlas cabling" 
	        << endreq;
	    m_rod_filename = "AtlasRODmap.data";
	    m_mez_filename = "AtlasMEZmap.data";
	    m_shift_LinkId = false;
        }
    }
   
    m_hasTagInfoMatched = true;

    sc = buildMdtGeometry();
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
            log << MSG::INFO << "Postpone MDTcabling creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }
    //else return StatusCode::SUCCESS;

    return StatusCode::SUCCESS;
}


StatusCode
MDTcablingSvc::updateAMDC(IOVSVC_CALLBACK_ARGS){
    m_alignment_updated = true;
    StatusCode sc = buildMdtGeometry();
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
	    MsgStream log(messageService(), name());
            log << MSG::INFO << "Postpone MDTcabing creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }
    return StatusCode::SUCCESS;
}


StatusCode 
MDTcablingSvc::loadCablingMap(IOVSVC_CALLBACK_ARGS_P(I,keys)){
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "loadCablingMap() callback triggered" << endreq;
  log << MSG::DEBUG << "ToolHandle in initMappingModel - <"  << m_dbTool 
                    << ">" << endreq;

  m_cabling_updated = true;

  if(!m_do_not_loadFromDB) {
    StatusCode sc = m_dbTool->loadParameters(I, keys);
    if (sc.isFailure()) {
      log << MSG::ERROR <<"loadCablingMap: reading Cabling maps from COOL failed"<<endreq;
      return StatusCode::FAILURE;
    }
 
    // retrieve the map from the detector store
   
    sc = m_pDetStore->retrieve(m_cablingMap);
    if (sc!=StatusCode::SUCCESS) {
      log << MSG::ERROR << "loadCablingMap: can't retrieve the cabling map from the detector store" << endreq;
      return false;
    }
    else {
      log << MSG::DEBUG << "Retrieved map from the detector store" << endreq;
      MDTGeometry::s_cabling_map = m_cablingMap;
      sc = buildMdtGeometry();
      if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
            log << MSG::INFO << "Postpone MDTcabing creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
      }
    }
 
  }
  else {
    log << MSG::INFO << "Do not load cabling from database" << endreq;
  }

  return StatusCode::SUCCESS;
}
