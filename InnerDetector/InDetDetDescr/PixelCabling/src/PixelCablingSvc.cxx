/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelCablingSvc.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Classes
#include "Identifier/Identifier.h"
#include "PixelCablingData.h"
#include "InDetIdentifier/PixelID.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/StatusCode.h"
#include "PixelGeoModel/IBLParameterSvc.h"

// Tools to fill the cabling
#include "GaudiKernel/IToolSvc.h"
#include "PixelFillCablingData_Final.h"
#include "PixelFillCablingData_CoralDB.h"

// for callback
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// COOL includes
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"


//#define PIXEL_DEBUG


using namespace std;
using eformat::helper::SourceIdentifier; 


static unsigned int defaultColumnsPerFE_pix     = 18;   // number of columns per FE
static unsigned int defaultRowsPerFE_pix        = 164;  // number of rows per FE
static unsigned int defaultFEsPerHalfModule_pix = 8;    // number of FEs in a row
//static unsigned int defaultColumnsPerFE_IBL     = 80;
//static unsigned int defaultRowsPerFE_IBL        = 336;
//static unsigned int defaultFEsPerHalfModule_IBL = 2;


static const unsigned int defaultNumLayers = 4;      // number of barrel layers, incl IBL
static const unsigned int defaultNumDisks = 3;       // number of disk layers

////////////////////////
// constructor
////////////////////////

PixelCablingSvc::PixelCablingSvc(const std::string& name, ISvcLocator*svc) :
    AthService(name,svc),
    m_cablingTool("PixelFillCablingData_Final"),
    m_detStore("DetectorStore", name),
    m_IBLParameterSvc("IBLParameterSvc",name),
    m_dataString(""),
    m_key("/PIXEL/ReadoutSpeed"),
    m_keyFEI4("/PIXEL/HitDiscCnfg"),
    m_callback_calls(0),
    m_idHelper(0),
    m_cabling(0),
    m_useIBLParameterSvc(true),
    m_IBLpresent(false),
    m_isHybrid(false),
    m_DBMpresent(false),
    m_dbm_columnsPerFE(0),
    m_dbm_rowsPerFE(0),
    m_dbm_FEsPerHalfModule(0),
    m_eta_module_offset(0)
{
    declareProperty("MappingType", m_mappingType = "Final");
    declareProperty("MappingFile", m_final_mapping_file = "");
    declareProperty("Bandwidth", m_bandwidth = 0);
    declareProperty("Coral_Connectionstring", m_coraldbconnstring = "oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL"); // used to configure CORAL based tool
    declareProperty("DictionaryTag", m_dictTag="PIXEL");
    declareProperty("ConnectivityTag", m_connTag="PIT-ALL-V39");
    declareProperty("KeyFEI4", m_keyFEI4 = "/PIXEL/HitDiscCnfg");
    declareProperty("Key", m_key, "Key=/PIXEL/ReadoutSpeed");
}


////////////////////////
// Copy constructor
////////////////////////
PixelCablingSvc::PixelCablingSvc(const PixelCablingSvc &other, const std::string& name, ISvcLocator*svc) :
    AthService(name,svc),
    m_cablingTool("PixelFillCablingData_Final"),
    m_detStore("DetectorStore", name),
    m_IBLParameterSvc("IBLParameterSvc",name),
    m_dataString(""),m_key("/PIXEL/ReadoutSpeed"),
    m_callback_calls(0),
    m_idHelper(0),
    m_cabling(0),
    m_useIBLParameterSvc(true),
    m_IBLpresent(false),
    m_isHybrid(false),
    m_DBMpresent(false),
    m_dbm_columnsPerFE(0),
    m_dbm_rowsPerFE(0),
    m_dbm_FEsPerHalfModule(0),
    m_eta_module_offset(0)
{
    // Copy everyzing
    m_mappingType = other.m_mappingType;
    m_final_mapping_file = other.m_final_mapping_file;
    m_bandwidth = other.m_bandwidth;
    m_coraldbconnstring = other.m_coraldbconnstring;
    m_dictTag = other.m_dictTag;
    m_connTag = other.m_connTag;
    m_key = other.m_key;
    m_keyFEI4 = other.m_keyFEI4;
}

////////////////////////
// Assignment operator
////////////////////////
PixelCablingSvc& PixelCablingSvc::operator= (const PixelCablingSvc &other) {

    if (&other != this) {
        m_mappingType = other.m_mappingType;
        m_final_mapping_file = other.m_final_mapping_file;
        m_bandwidth = other.m_bandwidth;
        m_coraldbconnstring = other.m_coraldbconnstring;
        m_dictTag = other.m_dictTag;
        m_connTag = other.m_connTag;
        m_key = other.m_key;
        m_keyFEI4 = other.m_keyFEI4;
    }
    return *this;
}


////////////////////////
// destructor
////////////////////////
PixelCablingSvc::~PixelCablingSvc()
{
}


////////////////////////
// initialize
////////////////////////
StatusCode PixelCablingSvc::initialize( )
{
    StatusCode sc;
    msg(MSG::INFO) << "PixelCablingSvc::initialize" << endreq;

    // Check that Identifiers are built as 64-bit
#ifndef __IDENTIFIER_64BIT__
    msg(MSG::ERROR) << "Not using 64-bit Identifiers - likely to cause undefined behaviour for IBL/DBM entries" << endreq;
#else
    msg(MSG::DEBUG) << "Using 64-bit Identifiers" << endreq;
#endif


    // Get an Identifier helper object
    sc = m_detStore.retrieve();
    if(!sc.isSuccess()){
        ATH_MSG_FATAL("Unable to retrieve detector store");
        return StatusCode::FAILURE;
    }
    else {
        msg(MSG::DEBUG) << "DetectorStore service found" << endreq;
    }



    // Get the PixelID Helper
    if (m_detStore->retrieve(m_idHelper, "PixelID").isFailure()) {
        msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
        return StatusCode::FAILURE;
    }

    // Get ToolSvc
    IToolSvc* toolSvc;
    if(StatusCode::SUCCESS != service("ToolSvc", toolSvc)) {
        msg(MSG::ERROR) << "Cannot get ToolSvc!" << endreq;
        return StatusCode::FAILURE;
    }


    // Get IBLParameterSvc
    if (!m_useIBLParameterSvc) {
        msg(MSG::INFO) << "IBLParameterSvc not used" << endreq;
    }
    else if (m_IBLParameterSvc.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve IBLParameterSvc" << endreq;
        return StatusCode::FAILURE;
    }


    // Check if jobOptions set a specific mapping file
    bool useMapFromOptions = false;
    if (m_final_mapping_file != "") {
        msg(MSG::INFO) << "Using mapping file from jobOptions" << endreq;
        useMapFromOptions = true;
    }



    // Get IBL and DBM properties from IBLParameterSvc

    // Set layout
    m_IBLpresent = m_IBLParameterSvc->containsIBL();
    m_isHybrid = m_IBLParameterSvc->contains3D();
    m_DBMpresent = m_IBLParameterSvc->containsDBM();


    // Get the values
    m_IBLParameterSvc->setCablingParameters(m_layer_columnsPerFE, m_layer_rowsPerFE, m_layer_FEsPerHalfModule,
                                            &m_dbm_columnsPerFE, &m_dbm_rowsPerFE, &m_dbm_FEsPerHalfModule);


    // Set mapping file according to geometry
    if (!useMapFromOptions) {

        // Hybrid IBL with DBM
        if (m_IBLpresent && m_isHybrid && m_DBMpresent) {
            //m_final_mapping_file = "Pixels_Atlas_IdMapping_inclIBL3D_DBM.dat";
            //m_final_mapping_file = "Pixels_Atlas_IdMapping_M7.dat";
            //m_final_mapping_file = "Pixels_Atlas_IdMapping_Run2.dat";
            m_final_mapping_file = "Pixels_Atlas_IdMapping_2016.dat";
        }
        // Homogeneous IBL with DBM
        else if (m_IBLpresent && !m_isHybrid && m_DBMpresent) {
            m_final_mapping_file = "Pixels_Atlas_IdMapping_inclIBL_DBM.dat";
        }
        // IBL but no DBM
        else if (m_IBLpresent && !m_DBMpresent) {
            m_final_mapping_file = "Pixels_Atlas_IdMapping_inclIBL.dat";
        }
        // No IBL, no DBM
        else {
            //m_final_mapping_file = "Pixels_Atlas_IdMapping.dat";
            m_final_mapping_file = "Pixels_Atlas_IdMapping_May08.dat";
        }
    }


    // Fill the columns/rows/FEsPerHalfModule vectors with remaining pixel values
    // m_layer_columns(rows)PerFE should be [IBL, PixLayer1, PixLayer2, PixLayer3]

    unsigned int numLayers = m_IBLpresent ? defaultNumLayers : (defaultNumLayers - 1);

    msg(MSG::DEBUG) << "numLayers = " << numLayers << endreq;

    while (numLayers > m_layer_columnsPerFE.size()) {
        m_layer_columnsPerFE.push_back(defaultColumnsPerFE_pix);
    }
    while (numLayers > m_layer_rowsPerFE.size()) {
        m_layer_rowsPerFE.push_back(defaultRowsPerFE_pix);
    }
    std::vector<int> numFEs;
    numFEs.push_back(defaultFEsPerHalfModule_pix);
    while (numLayers > m_layer_FEsPerHalfModule.size()) {
        m_layer_FEsPerHalfModule.push_back(numFEs);
    }

    // Do the same for disks
    while (defaultNumDisks > m_disk_columnsPerFE.size()) {
        m_disk_columnsPerFE.push_back(defaultColumnsPerFE_pix);
    }
    while (defaultNumDisks > m_disk_rowsPerFE.size()) {
        m_disk_rowsPerFE.push_back(defaultRowsPerFE_pix);
    }
    while (defaultNumDisks > m_disk_FEsPerHalfModule.size()) {
        m_disk_FEsPerHalfModule.push_back(defaultFEsPerHalfModule_pix);
    }


    // The Eta_module value returned by PixelID::eta_module is a signed integer,
    // symmetric around z = 0. We need to offset is so that it starts at 0 (is unsigned)
    m_eta_module_offset = m_layer_FEsPerHalfModule[0].size()/2;


    // Print out all values
#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "-- PixelCablingSvc ------------------------------" << endreq;
    msg(MSG::DEBUG) << "m_IBLpresent = " << m_IBLpresent << endreq;
    msg(MSG::DEBUG) << "m_DBMpresent = " << m_DBMpresent << endreq;
    msg(MSG::DEBUG) << "m_isHybrid = " << m_isHybrid << endreq;
    msg(MSG::DEBUG) << "m_layer_columnsPerFE = " << m_layer_columnsPerFE << endreq;
    msg(MSG::DEBUG) << "m_layer_rowsPerFE = " << m_layer_rowsPerFE << endreq;
    msg(MSG::DEBUG) << "m_layer_FEsPerHalfModule = " << m_layer_FEsPerHalfModule << endreq;
    msg(MSG::DEBUG) << "m_disk_columnsPerFE = " << m_disk_columnsPerFE << endreq;
    msg(MSG::DEBUG) << "m_disk_rowsPerFE = " << m_disk_rowsPerFE << endreq;
    msg(MSG::DEBUG) << "m_disk_FEsPerHalfModule = " << m_disk_FEsPerHalfModule << endreq;
    msg(MSG::DEBUG) << "m_dbm_columnsPerFE = " << m_dbm_columnsPerFE << endreq;
    msg(MSG::DEBUG) << "m_dbm_rowsPerFE = " << m_dbm_rowsPerFE << endreq;
    msg(MSG::DEBUG) << "m_dbm_FEsPerHalfModule = " << m_dbm_FEsPerHalfModule << endreq;
    msg(MSG::DEBUG) << "m_final_mapping_file = " << m_final_mapping_file << endreq;
    msg(MSG::DEBUG) << "useMapFromOptions = " << useMapFromOptions << endreq;
    msg(MSG::DEBUG) << "-------------------------------------------------" << endreq;
#endif



    // Fill the cabling maps

    std::string toolType;
    msg(MSG::INFO) << "Mapping Type: " << m_mappingType << endreq;

    if (m_mappingType == "Final") {
        toolType = "PixelFillCablingData_Final";
        msg(MSG::INFO) << "Using cabling from text file: " << m_final_mapping_file << endreq;

        if (m_cablingTool.retrieve().isFailure()) {
            msg(MSG::ERROR) << "Cannot get PixelFillCablingData_Final tool" << endreq;
            return StatusCode::FAILURE;
        }
        else msg(MSG::INFO) << "PixelFillCablingData_Final tool retrieved" << endreq;

        m_cablingTool->setMappingFile(m_final_mapping_file);
        m_cabling = m_cablingTool->FillMaps();
        if (m_cabling == NULL) {
            msg(MSG::ERROR) << "PixelCabling FillMaps returned NULL" << endreq;
            return StatusCode::FAILURE;
        }
    }
    else {
        msg(MSG::ERROR) << "Cabling tool type not specified" << endreq;
        return StatusCode::FAILURE;
    }
    m_context = m_idHelper->wafer_context();



    // Cross check - test if number of modules read into the cabling
    // map is equal to number of modules reported by IBLParameterSvc
    // (to prevent running with incorrect mapping file)

/*  Temporarily removed for M5, since not all IBL modules will be included
    int mapSize = m_cabling->get_size_onoff();
    int nModules = 1744;                                                    // number of pixel modules
    if (m_IBLpresent) nModules += 14 * m_layer_FEsPerHalfModule[0].size();  // add number of IBL modules
    if (m_DBMpresent) nModules += 24;                                       // add number of DBM modules

    if (mapSize != nModules) {
        msg(MSG::FATAL) << "Bad configuration - number of modules in cabling map "
                        << "does not match the number reported by IBLParameterSvc" << endreq;
        msg(MSG::INFO) << "Cabling map contains " << abs(mapSize-nModules) << " modules "
                       << (mapSize > nModules ? "more" : "less") << " than expected, "
                       << "indicating wrong cabling map w.r.t geometry" << endreq;
        return StatusCode::FAILURE;
    }
*/

    // Register callback to ReadoutSpeed
    const DataHandle<AthenaAttributeList> attrlist_rs;
    if (m_detStore->contains<AthenaAttributeList>(m_key)) {

        sc = m_detStore->regFcn(&IPixelCablingSvc::IOVCallBack,
                                dynamic_cast<IPixelCablingSvc*>(this),
                                attrlist_rs, m_key);
        // If regFcn fails even when folder is present -> abort
        if (!sc.isSuccess()) {
            ATH_MSG_FATAL("Unable to register readoutspeed callback");
            return StatusCode::FAILURE;
        }
    }
    else {
        ATH_MSG_INFO("Folder " << m_key << " not found, using default readoutspeed"
                     << " values (all modules at SINGLE_40)");
    }

    // Register callback to HitDiscCnfg
    if (m_IBLpresent) {
        const DataHandle<AthenaAttributeList> attrlist_hdc;
        if (m_detStore->contains<AthenaAttributeList>(m_keyFEI4)) {

            sc = m_detStore->regFcn(&IPixelCablingSvc::IOVCallBack_HitDiscCnfg,
                                    dynamic_cast<IPixelCablingSvc*>(this),
                                    attrlist_hdc, m_keyFEI4);

            // If regFcn fails even when folder is present -> abort
            if (!sc.isSuccess()) {
                ATH_MSG_FATAL("Unable to register HitDiscCnfg callback");
                return StatusCode::FAILURE;
            }
        }
        else {
            ATH_MSG_INFO("Folder " << m_keyFEI4 << " not found, using default HitDiscCnfg"
                         << " values (all FEs at HitDiscCnfg = 3)");
        }
    }



    return sc;
}

////////////////////////
// finalize
////////////////////////
StatusCode PixelCablingSvc::finalize()
{
    return StatusCode::SUCCESS;
}

////////////////////////
// query interface
////////////////////////
StatusCode
PixelCablingSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
    if ( IPixelCablingSvc::interfaceID().versionMatch(riid) ) {
        *ppvInterface = dynamic_cast<IPixelCablingSvc*>(this);
    }
    else {
        // Interface is not directly available : try out a base class
        return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

////////////////////////
// getOfflineList - get list of offlineIds from the RobId
////////////////////////
void PixelCablingSvc::getOfflineList(std::vector<IdentifierHash>& offlineIdHashList, int robid)
{
    deque<Identifier> offlineIdList = m_cabling->find_entry_offlineList(robid);
    std::deque<Identifier>::iterator it1 = offlineIdList.begin();
    std::deque<Identifier>::iterator it2 = offlineIdList.end();
    for(; it1!=it2;++it1)
        offlineIdHashList.push_back(getOfflineIdHashFromOfflineId(*it1));
}
////////////////////////
// getOfflineId - get the offlineId from the onlineId
////////////////////////
Identifier PixelCablingSvc::getOfflineId(uint64_t onlineId)
{
    return m_cabling->find_entry_onoff(onlineId);
}

////////////////////////
// getOfflineIdHash - get the offlineIdHash from the onlineId
////////////////////////
IdentifierHash 
PixelCablingSvc::getOfflineIdHash(uint64_t onlineId)
{

    Identifier offlineId = m_cabling->find_entry_onoff(onlineId);
    return getOfflineIdHashFromOfflineId(offlineId);
}

////////////////////////
// getOfflineIdHashFromOfflineId - get the offlineIdHash from the offlineId
////////////////////////
IdentifierHash 
PixelCablingSvc::getOfflineIdHashFromOfflineId(Identifier offlineId)
{
    if (offlineId == 0)
        return 0xffffffff; //was -1, but Identifier hash is unsigned, so I return the unsigned equivalent (2^32 -1, assuming int is 32 bit) (shaun, 15 may 2008)

    return m_idHelper->wafer_hash(offlineId);   // ->InDetDetDescr/InDetIdentifier:PixelID - get hash identifier
}

////////////////////////
// getOfflineIdFromDCS - get the offlineId from the DCS name
////////////////////////
Identifier PixelCablingSvc::getOfflineIdFromDCS(std::string DCSname)
{
    return m_cabling->find_entry_DCSoffline(DCSname);
}

////////////////////////
// getRobId - get RobId from offlineId
////////////////////////
uint32_t PixelCablingSvc::getRobId(Identifier offlineId)
{
    return m_cabling->find_entry_offrob(offlineId);
}

// For backward compatibility
uint32_t PixelCablingSvc::getRobID(Identifier offlineId)
{  
    return m_cabling->find_entry_offrob(offlineId);
}

////////////////////////
// getRobId - get RobId from RodId
////////////////////////
int PixelCablingSvc::getRobId(const int rodid)
{
    return m_cabling->find_entry_rodrob(rodid);
}


////////////////////////
// getRobId - get RODId from ROBId
////////////////////////
int PixelCablingSvc::getRodId(const int robid)
{
    return m_cabling->find_entry_robrod(robid);
}

////////////////////////
// getOnlineId - get onlineId from offlineId
////////////////////////
uint64_t PixelCablingSvc::getOnlineId(Identifier offlineId)
{
    return m_cabling->find_entry_offon(offlineId);

}

////////////////////////
// getPixelIdfromHash - get the pixelId from the offlineIdHash, FE, row and column
////////////////////////
Identifier PixelCablingSvc::getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column) 
{
    return PixelCablingSvc::getPixelId(m_idHelper->wafer_id(offlineIdHash), FE, row, column);
}

////////////////////////
// getPixelId - get the pixelId from the offlineId, FE, row and column
////////////////////////
Identifier PixelCablingSvc::getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column) 
{
    // Identify the module type
    moduletype thisModule = getModuleType(offlineId);

    unsigned int columnsPerFE, rowsPerFE, FEsPerHalfModule;
    unsigned int phi_index, eta_index;

    unsigned int layer_disk = abs(m_idHelper->layer_disk(offlineId));
    unsigned int eta_module = 0;    // Only IBL modules need this

    // Declare row/column offset for FE-I4 vs FE-I3
    int column_row_offset = 0;


    // ---------------------
    // Set module properties
    // ---------------------

    switch (thisModule) {

    case NONE:
        msg(MSG::ERROR) << "Module type not identified -- cannot build pixelId" << endreq;
        return Identifier();

    case DBM:
        columnsPerFE = m_dbm_columnsPerFE;
        rowsPerFE = m_dbm_rowsPerFE;
        FEsPerHalfModule = m_dbm_FEsPerHalfModule;
        column_row_offset = -1;
        break;

    case PIX_ENDCAP:
        FEsPerHalfModule = m_disk_FEsPerHalfModule[layer_disk];
        columnsPerFE =  m_disk_columnsPerFE[layer_disk];
        rowsPerFE = m_disk_rowsPerFE[layer_disk];
        break;

    case IBL:
        column_row_offset = -1;
        eta_module = m_idHelper->eta_module(offlineId) + m_eta_module_offset;
        if (eta_module >= m_layer_FEsPerHalfModule[0].size()) {
            msg(MSG::WARNING) << "getPixelId: Eta_module out of range (eta_module = " << eta_module
                              << ", expected number of modules per stave = " << m_layer_FEsPerHalfModule[0].size() << ")" << endreq;
            return Identifier();
        }
        // fall through

    default:        // PIX_BARREL + IBL
        columnsPerFE = m_layer_columnsPerFE[layer_disk];
        rowsPerFE = m_layer_rowsPerFE[layer_disk];
        FEsPerHalfModule = m_layer_FEsPerHalfModule[layer_disk][eta_module];
        break;
    }


    // ---------------------
    // Check input sanity
    // ---------------------

    // Correct row, column
    row = row + column_row_offset;
    column = column + column_row_offset;

    if (row >= rowsPerFE || column >= columnsPerFE ||
            FE >= ((thisModule == IBL || thisModule == DBM) ? FEsPerHalfModule : 2*FEsPerHalfModule)) {
        msg(MSG::WARNING) << "Illegal pixel requested OfflineID: " << std::hex << offlineId << std::dec
                        << " FE: " << FE << " row: " << row << " column: " << column << endreq;
        msg(MSG::DEBUG) << "Limits are: FE < " << ((thisModule == IBL || thisModule == DBM) ? FEsPerHalfModule : 2*FEsPerHalfModule)
                       << ", row < " << rowsPerFE << ", column < " << columnsPerFE << endreq;
        return Identifier(); // illegal Identifier, standardized for PixelRodDecoder
    }

    // ---------------------
    // Convert row/column to eta/phi indices
    // ---------------------

    switch (thisModule) {

    case DBM:
        eta_index = rowsPerFE - 1 - row;
        if (m_idHelper->barrel_ec(offlineId) > 0) {     // A side (pos. eta)
            phi_index = column;
        } else {                                        // C side
            phi_index = columnsPerFE - 1 - column;
        }
        break;

    case IBL:
        phi_index = rowsPerFE - 1 - row;
        eta_index = FE * columnsPerFE + column;
        break;

    default:    // pixels
        if (FE < FEsPerHalfModule) {
            phi_index = ((2*rowsPerFE)-1)-row;
            eta_index = ((columnsPerFE*FEsPerHalfModule)-1)-(column+(FE*columnsPerFE));
        } else {
            phi_index = row;
            eta_index = ((FE-FEsPerHalfModule)*columnsPerFE)+column;
        }
        if (thisModule == PIX_ENDCAP) {
            // Swap phi_index for even endcap modules
            if ((m_idHelper->phi_module(offlineId))%2 == 0) {
                phi_index = 2*rowsPerFE-phi_index-1;
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Even disk module found, phi module: " << m_idHelper->phi_module(offlineId)
                                << " swapped phi index to : " << phi_index << endreq;
            }
        }
        break;
    }


    Identifier pixelId = m_idHelper->pixel_id(offlineId,phi_index,eta_index);


#ifdef PIXEL_DEBUG
    unsigned int eta_index_max =  m_idHelper->eta_index_max(offlineId);
    unsigned int phi_index_max =  m_idHelper->phi_index_max(offlineId);
    if (eta_index > eta_index_max) msg(MSG::WARNING) << "Error! eta_index: " << eta_index << " > eta_index_max: " << eta_index_max << endreq;
    if (phi_index > phi_index_max) msg(MSG::WARNING) << "Error! phi_index: " << phi_index << " > phi_index_max: " << phi_index_max << endreq;
    //consistency check - to be removed to speed up
    uint32_t check_FE = getFE(&pixelId,offlineId);
    uint32_t check_row = getRow(&pixelId,offlineId) + column_row_offset;
    uint32_t check_column = getColumn(&pixelId,offlineId) + column_row_offset;
    if (check_FE != FE || check_row != row || check_column != column) {
        msg(MSG::WARNING) << "identify OfflineID: 0x" << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column << " unequal to:" << endreq;
        msg(MSG::WARNING) << "identify PixelID: 0x" << std::hex << pixelId << std::dec << " FE: " << check_FE << " row: " << check_row << " column: " << check_column << endreq;
    }
#endif

    return pixelId;
}





////////////////////////
// getFE - get local front-end from pixelId
////////////////////////
uint32_t PixelCablingSvc::getFE(Identifier *pixelId, Identifier offlineId)
{
    moduletype thisModule = getModuleType(offlineId);

    unsigned int columnsPerFE, rowsPerFE, FEsPerHalfModule;
    unsigned int layer_disk = abs(m_idHelper->layer_disk(offlineId));
    unsigned int eta_module = 0;

    unsigned int phi_index = m_idHelper->phi_index(*pixelId);
    int eta_index = m_idHelper->eta_index(*pixelId);

    unsigned int FE;

    // ---------------------
    // Set module properties 
    // ---------------------

    switch (thisModule) {

    case DBM:
        FE = 0;     // simple as that
        return FE;
        break;

    case PIX_ENDCAP:
        FEsPerHalfModule = m_disk_FEsPerHalfModule[layer_disk];
        columnsPerFE =  m_disk_columnsPerFE[layer_disk];
        rowsPerFE = m_disk_rowsPerFE[layer_disk];

        // Swap phi_index for even endcap modules
        if ((m_idHelper->phi_module(offlineId))%2 == 0) {
            phi_index = 2*rowsPerFE-phi_index-1;
        }

        break;

    case IBL:
        eta_module = m_idHelper->eta_module(offlineId) + m_eta_module_offset; //offset by 10 to start counting from 0
        if (eta_module >= m_layer_FEsPerHalfModule[0].size()) {
            msg(MSG::WARNING) << "getFE: Eta_module out of range (eta_module = " << eta_module
                              << ", expected number of modules per stave = " << m_layer_FEsPerHalfModule[0].size() << ")" << endreq;
            return 0xffffffff;
        }
        // fall through

    default:        // PIX_BARREL + IBL
        columnsPerFE = m_layer_columnsPerFE[layer_disk];
        rowsPerFE = m_layer_rowsPerFE[layer_disk];
        FEsPerHalfModule = m_layer_FEsPerHalfModule[layer_disk][eta_module];
        break;
    }
    
    // ---------------------
    // Compute FE number 
    // ---------------------
    
    if (phi_index >= rowsPerFE) {
        FE = (int)((FEsPerHalfModule-1)-(eta_index/columnsPerFE));
    } else {
        FE = (int)(eta_index/columnsPerFE)+FEsPerHalfModule;
    }

    // For IBL, the above returns FE number in range [2,3] (planar), or [1] (3D sensors).
    // Change that to be [0,1] (planar) and [0] (3D) for consistency
    if (thisModule == IBL) FE = FE - FEsPerHalfModule;

    return FE;
}

////////////////////////
// getColumn - get the column number from the PixelId
////////////////////////
uint32_t PixelCablingSvc::getColumn(Identifier *pixelId, Identifier offlineId)
{
    moduletype thisModule = getModuleType(offlineId);

    unsigned int columnsPerFE, rowsPerFE;
    unsigned int layer_disk = abs(m_idHelper->layer_disk(offlineId));
    unsigned int eta_module = 0;
    int column_offset = 0;

    unsigned int phi_index = m_idHelper->phi_index(*pixelId);
    int eta_index = m_idHelper->eta_index(*pixelId);


    // ---------------------
    // Set module properties
    // ---------------------

    switch (thisModule) {

    case DBM:
        columnsPerFE = m_dbm_columnsPerFE;
        rowsPerFE = m_dbm_rowsPerFE;
        column_offset = 1;
        break;

    case PIX_ENDCAP:
        columnsPerFE =  m_disk_columnsPerFE[layer_disk];
        rowsPerFE = m_disk_rowsPerFE[layer_disk];

        // Swap phi_index for even endcap modules
        if ((m_idHelper->phi_module(offlineId))%2 == 0) {
            phi_index = 2*rowsPerFE-phi_index-1;
        }

        break;

    case IBL:
        column_offset = 1;
        eta_module = m_idHelper->eta_module(offlineId) + m_eta_module_offset; //offset by 10 to start counting from 0
        if (eta_module >= m_layer_FEsPerHalfModule[0].size()) {
            msg(MSG::WARNING) << "getColumn: Eta_module out of range (eta_module = " << eta_module
                              << ", expected number of modules per stave = " << m_layer_FEsPerHalfModule[0].size() << ")" << endreq;
            return 0xffffffff;
        }
        // fall through

    default:        // PIX_BARREL + IBL
        columnsPerFE = m_layer_columnsPerFE[layer_disk];
        rowsPerFE = m_layer_rowsPerFE[layer_disk];
        break;
    }


    // ---------------------
    // Convert eta index to column number
    // ---------------------

    int column;

    // DBM (column <-> phi_index)
    if (thisModule == DBM) {
        if (m_idHelper->barrel_ec(offlineId) > 0) column = m_idHelper->phi_index(*pixelId);     // A side
        else column = columnsPerFE - m_idHelper->phi_index(*pixelId) - 1;                       // C side
    }
    // Pixels, IBL
    else {
        if ((phi_index >= rowsPerFE)) {
            column = (columnsPerFE-1)-(eta_index%columnsPerFE);
        } else {
            column = eta_index%columnsPerFE;
        }
    }


    // ---------------------
    // Check output sanity
    // ---------------------
    if (column >= (int)columnsPerFE) {
        msg(MSG::WARNING) << "Computed column number exceeds maximum value: col = " << column + column_offset
                          << " (max = " << columnsPerFE << ")" <<  endreq;
        return 0xffffffff;
    }

    return column + column_offset;

}

////////////////////////
// getRow - get the row number from the pixelId
////////////////////////
uint32_t PixelCablingSvc::getRow(Identifier *pixelId, Identifier offlineId)
{
    moduletype thisModule = getModuleType(offlineId);

    unsigned int rowsPerFE;
    unsigned int layer_disk = abs(m_idHelper->layer_disk(offlineId));
    unsigned int eta_module = 0;
    int row_offset = 0;

    unsigned int phi_index = m_idHelper->phi_index(*pixelId);


    // ---------------------
    // Set module properties
    // ---------------------
    switch (thisModule) {

    case DBM:
        rowsPerFE = m_dbm_rowsPerFE;
        row_offset = 1;
        break;

    case PIX_ENDCAP:
        rowsPerFE = m_disk_rowsPerFE[layer_disk];

        // Swap phi_index for even endcap modules
        if ((m_idHelper->phi_module(offlineId))%2 == 0) {
            phi_index = 2*rowsPerFE-phi_index-1;
        }

        break;

    case IBL:
        row_offset = 1;
        eta_module = m_idHelper->eta_module(offlineId) + m_eta_module_offset; //offset by 10 to start counting from 0
        if (eta_module >= m_layer_FEsPerHalfModule[0].size()) {
            msg(MSG::WARNING) << "getRow: Eta_module out of range (eta_module = " << eta_module
                              << ", expected number of modules per stave = " << m_layer_FEsPerHalfModule[0].size() << ")" << endreq;
            return 0xffffffff;
        }
        // fall through

    default:        // PIX_BARREL + IBL
        rowsPerFE = m_layer_rowsPerFE[layer_disk];
        break;
    }


    // ---------------------
    // Convert phi index to row number
    // ---------------------

    int row;

    switch (thisModule) {

    case DBM:
        // Row <-> eta_index
        row = rowsPerFE - m_idHelper->eta_index(*pixelId) - 1;
        break;

    case IBL:
        row = rowsPerFE - 1 - phi_index;
        break;

    default:    // Pixels
        if (phi_index >= rowsPerFE) {
            row = ((2*rowsPerFE)-1)-phi_index;
        } else {
            row = phi_index;
        }
        break;
    }

    // ---------------------
    // Check output sanity
    // ---------------------
    if (row >= (int)rowsPerFE) {
        msg(MSG::WARNING) << "Computed row number exceeds maximum value: row = " << row + row_offset
                          << "(max = " << rowsPerFE << ")" << endreq;
        return 0xffffffff;
    }


    return row + row_offset;

}


////////////////////////
// getFEwrtSlink
// Function to get the number of an FE-I4 within an Slink, 
// i.e. pos. value in the range [0,7], corresponding to 
// the 'nnn' bits of a fragment header. To get the number 
// of an FE within a module, use getFE.
////////////////////////
uint32_t PixelCablingSvc::getFEwrtSlink(Identifier *pixelId) {

    unsigned int nnn = 99;
    Identifier offlineId = m_idHelper->wafer_id(*pixelId);
    uint64_t onlineId = getOnlineId(offlineId);
    uint32_t linkNum = (onlineId >> 24) & 0xFFFF;
    unsigned int localFE = getFE(pixelId, offlineId);   // FE number within module, [0,1]. Increases with increasing eta_index

    nnn = (linkNum >> (localFE * 8)) & 0xF;

    // Check for errors
    if (nnn > 7) msg(MSG::WARNING) << "Error in the identification of the FE-I4 w.r.t. Slink" << endreq;

    return nnn;
}


////////////////////////
// getAllRods - get all Rods from the mapping
////////////////////////

std::vector<uint32_t>& PixelCablingSvc::getAllRods()
{
    return m_cabling->get_allRods();
}

////////////////////////
// getAllRobs - get all ROBs from the mapping
////////////////////////
std::vector<uint32_t>& PixelCablingSvc::getAllRobs()
{
    return m_cabling->get_allRobs();
}

////////////////////////
// callback for changing the readout speed
////////////////////////
StatusCode PixelCablingSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

    m_callback_calls++;

    ATH_MSG_INFO("                     -----   in  Callback   -----");
    for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
        ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I<<"\n\t\t\t\t\t-----mtst-----\n");


    const AthenaAttributeList* attrlist = 0;
    StatusCode sc = m_detStore->retrieve(attrlist, m_key);
    if(!sc.isSuccess()){
        ATH_MSG_FATAL("Unable to retrieve AthenaAttributeList");
        return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("AthenaAttributeList for " << m_key << ":");
        attrlist->print(std::cout);
    }

    const coral::Blob& blob=(*attrlist)["readoutspeed_per_ROD"].data<coral::Blob>();
    const char* p = static_cast<const char*>(blob.startingAddress());
    unsigned int len = blob.size();
    m_dataString.resize(len);
    for (unsigned int i = 0; i!=len; ++i) m_dataString[i] = *p++;

    int pos=0;
    bool refill = false; // only refill the cabling if something changed
    while (m_dataString.find(",",pos) != string::npos){
        istringstream iss(m_dataString.substr(pos,m_dataString.find(",",pos)));
        uint32_t rod;
        iss >> std::hex >> rod;

        string speed = m_dataString.substr(m_dataString.find(",",pos)+1,m_dataString.find("\n",pos)-m_dataString.find(",",pos)-1);
        if (m_cablingTool->rodReadoutMap.find(rod) == m_cablingTool->rodReadoutMap.end()){
            if (speed != "SINGLE_40"){
                m_cablingTool->rodReadoutMap.insert(make_pair(rod,true));
                refill = true;
            }
        }
        else{
            if (speed != "SINGLE_40")
                m_cablingTool->rodReadoutMap[rod]=true;
            else
                m_cablingTool->rodReadoutMap[rod]=false;
            refill = true;
        }

        pos = m_dataString.find("\n",pos) + 1;
    }
    if (refill){
        delete m_cabling;
        m_cabling = m_cablingTool->FillMaps();
        ATH_MSG_INFO("                     -----   refilled Pixel cabling maps  -----");
    }
    ATH_MSG_INFO("                     -----   DONE  Callback "<<m_callback_calls<<"  -----");

    return StatusCode::SUCCESS;
}




////////////////////////
// callback for changing the FE-I4 HitDiscCnfg property
////////////////////////
StatusCode PixelCablingSvc::IOVCallBack_HitDiscCnfg(IOVSVC_CALLBACK_ARGS_P(I, keys)) {

    m_callback_calls++;

    ATH_MSG_INFO("                     -----   HitDiscCnfg callback   -----");
    for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
        ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I << "\n\t\t\t\t\t-----mtst-----\n");



    const AthenaAttributeList* attrlist = 0;
    StatusCode sc = m_detStore->retrieve(attrlist, m_keyFEI4);
    if(!sc.isSuccess()){
        ATH_MSG_FATAL("Unable to retrieve AthenaAttributeList");
        return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("AthenaAttributeList for " << m_key << ":");
        attrlist->print(std::cout);
    }

    const coral::Blob& blob=(*attrlist)["HitDiscCnfgData"].data<coral::Blob>();
    const uint32_t* p = static_cast<const uint32_t*>(blob.startingAddress());


    uint32_t cooldata;
    unsigned int len = blob.size()/sizeof(uint32_t);
    ATH_MSG_DEBUG("blob.size() = " << blob.size() << ", len = " << len);


    for (unsigned int i = 0; i < len; ++i) {

        cooldata = *p++;

        ATH_MSG_DEBUG("Got hitdisccnfgData[" << i << "] = 0x" << std::hex << cooldata << std::dec);

        // The implementation below uses one common value, one common 3D value,
        // and an exception list of individual FEs, in order to save DB space.
        // Here we convert this into only one common value and an exception list, i.e.
        // if the 3D FEs have a different common value they are all added to the exception list

        // Update the most common value, identified by 0xH0000000, where H = 00HH
        if ((cooldata & 0x8FFFFFFF) == 0x0) {
            ATH_MSG_DEBUG("Setting common HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));
            m_cabling->setCommonHitDiscCngf((cooldata&0x30000000) >> 28);
        }

        // Update all 3D sensors with a common value, identified by 0xZ0000000, where Z = 10hh
        else if ((cooldata & 0x8FFFFFFF) == 0x80000000) {

            ATH_MSG_DEBUG("Setting common 3D HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));

            // Loop over list of robs, find the 3D sensors and insert them in HitDiscCnfg map
            for (auto robid : m_cabling->get_allRobs()) {

                // Skip non-IBL ROBs (DBM is not affected by the common 3D value)
                if (!isIBL(robid)) continue;

                for (int link = 0 ; link < 8 ; ++link) {

                    if (((robid&0xF)==1 && link > 3) || (((robid&0xF)==2) && link < 4))  {
                        m_cabling->add_entry_HitDiscCnfg((robid | (link >> 24)), (cooldata & 0x30000000) >> 28);
                    }
                }
            }
        }


        // Update a single link, 0xHLDDRRRR
        else {
            ATH_MSG_DEBUG("Setting HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28)
                          << " for ROB 0x" << std::hex << (cooldata & 0xFFFFFF)
                          << ", link " << std::dec << ((cooldata & 0xF000000) >> 24));
            m_cabling->add_entry_HitDiscCnfg((cooldata & 0xFFFFFFF), (cooldata & 0x30000000) >> 28);
        }

    }

    ATH_MSG_INFO("                     -----   DONE  Callback " << m_callback_calls << "  -----");

    // Print map contents
    if (msgLvl(MSG::DEBUG)) m_cabling->printHitDiscCnfg();


    return StatusCode::SUCCESS;
}



////////////////////////
// getOnlineIdFromRobId
// Wrapper function to PixelCablingData::getOnlineIdFromRobId
////////////////////////
uint64_t PixelCablingSvc::getOnlineIdFromRobId(const uint32_t robId, const uint32_t link_module) {
    return m_cabling->getOnlineIdFromRobId(robId, link_module);
}


////////////////////////
// getLocalFEI4
// returns local FE number on IBL / DBM module
////////////////////////
unsigned int PixelCablingSvc::getLocalFEI4(const uint32_t fe, const uint64_t onlineId) {

    unsigned int linknum40 = (onlineId >> 24) & 0xFF;
    unsigned int linknum80 = (onlineId >> 32) & 0xFF;

    if (fe == linknum40) return 0;
    else if (fe == linknum80) return 1;
    else msg(MSG::WARNING) << "Error in retrieving local FE-I4 number: linknumber " << fe
                           << " not found in onlineID " << std::hex << onlineId << endreq;
    return 0xF;
}

////////////////////////
// isIBL - determine if a given robId or pixelId corresponds to an IBL module or not
////////////////////////
bool PixelCablingSvc::isIBL(const uint32_t robId) {
    // IBL subdetector ID is 0x14
    if (((robId >> 16) & 0xFF) == 0x14) return true;
    else return false;
}

bool PixelCablingSvc::isIBL(const Identifier& id) {
    // If IBL is present, an IBL pixelId will have barrel_ec = layer_disk = 0
    if (m_IBLpresent && m_idHelper->barrel_ec(id) == 0 && m_idHelper->layer_disk(id) == 0) return true;
    else return false;
}

////////////////////////
// isDBM - determine if a given robId or pixelId corresponds to a DBM module or not
////////////////////////
bool PixelCablingSvc::isDBM(const uint32_t robId) {
    // DBM subdetector ID is 0x15
    if (((robId >> 16) & 0xFF) == 0x15) return true;
    else return false;
}

bool PixelCablingSvc::isDBM(const Identifier& id) {
    return m_idHelper->is_dbm(id);
}

////////////////////////
// getModuleType
////////////////////////
PixelCablingSvc::moduletype PixelCablingSvc::getModuleType(const Identifier& id) {

    moduletype isType = NONE;

    if (isIBL(id)) isType = IBL;
    else if (isDBM(id)) isType = DBM;
    else {
        if (abs(m_idHelper->barrel_ec(id)) == 2) isType = PIX_ENDCAP;
        else isType = PIX_BARREL;
    }

    return isType;
}



////////////////////////
// getHitDiscCnfg - wrapper function to PixelCablingData::getHitDiscCnfg
////////////////////////
int PixelCablingSvc::getHitDiscCnfg(const uint32_t robId, const int link) {
    return m_cabling->getHitDiscCnfg((link << 24) | robId);
}


////////////////////////
// getHitDiscCnfg - overloaded function to be used by offline
////////////////////////
int PixelCablingSvc::getHitDiscCnfg(Identifier* pixelId) {

    uint32_t robId = getRobId(m_idHelper->wafer_id(*pixelId));
    int link = getFEwrtSlink(pixelId);
    return getHitDiscCnfg(robId, link);
}




