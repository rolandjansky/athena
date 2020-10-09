/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "DetDescrCnvSvc/DetDescrCnvSvc.h"
#include "DetDescrCnvSvc/IDetDescrCnvSvc.h"
//#include "AthenaServices/AthenaConversionSvc.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "StoreGate/StoreGateSvc.h"
//  #include "SGTools/TransientAddress.h"

//External definitions
const long DetDescr_StorageType=0x44;

//-------------------------------------------------------------------------

/// Standard constructor
DetDescrCnvSvc::DetDescrCnvSvc(const std::string& name, ISvcLocator* svc)
        :
        ConversionSvc( name, svc, DetDescr_StorageType),
        m_detStore(0),
        m_decodeIdDict(true),
        m_idDictFromRDB(false),
        m_fromRoot(false),
        m_fromNova(false),
        m_detElemsfromDetNodes(false),
        m_compact_ids_only(false),
        m_do_checks(false),
        m_do_neighbours(true),
        m_hasCSC(true),
        m_hasSTgc(true),
        m_hasMM(true)
{
    declareProperty("DetectorManagers",            m_detMgrs);
    declareProperty("DetectorNodes",   	           m_detNodes );
    declareProperty("DecodeIdDict",	           m_decodeIdDict );
    declareProperty("IdDictName",   	           m_idDictName );
    declareProperty("IdDictFromRDB",   	           m_idDictFromRDB );
    declareProperty("IdDictGlobalTag",             m_idDictGlobalTag );
    declareProperty("ReadFromROOT",   	           m_fromRoot );
    declareProperty("ReadFromNova",   	           m_fromNova );
    declareProperty("InitDetElemsFromGeoModel",    m_detElemsfromDetNodes);
    declareProperty("CompactIDsOnly",              m_compact_ids_only);
    declareProperty("DoIdChecks",                  m_do_checks);
    declareProperty("DoInitNeighbours",            m_do_neighbours);
    declareProperty("HasCSC",                      m_hasCSC);
    declareProperty("HasSTgc",                     m_hasSTgc);
    declareProperty("HasMM",                       m_hasMM);

    declareProperty("AtlasIDFileName",             m_idDictATLASName);
    declareProperty("InDetIDFileName",             m_idDictInDetName);
    declareProperty("LArIDFileName",               m_idDictLArName);
    declareProperty("TileIDFileName",              m_idDictTileName);
    declareProperty("CaloIDFileName",              m_idDictLVL1Name);
    declareProperty("MuonIDFileName",              m_idDictMuonName);
    declareProperty("HighVoltageIDFileName",       m_idDictLArHighVoltageName);
    declareProperty("LArElectrodeIDFileName",      m_idDictLArElectrodeName);
    declareProperty("ForwardIDFileName",           m_idDictForwardName);
    declareProperty("FullAtlasNeighborsFileName",  m_fullAtlasNeighborsName);
    declareProperty("FCAL2DNeighborsFileName",     m_fcal2dNeighborsName);
    declareProperty("FCAL3DNeighborsNextFileName", m_fcal3dNeighborsNextName);
    declareProperty("FCAL3DNeighborsPrevFileName", m_fcal3dNeighborsPrevName);
    declareProperty("TileNeighborsFileName",       m_tileNeighborsName);

}

//-------------------------------------------------------------------------

/// Standard Destructor
DetDescrCnvSvc::~DetDescrCnvSvc()   {
}

//-------------------------------------------------------------------------

/// Identify interfaces to which this service is responsive
StatusCode
DetDescrCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
    if ( riid == IConversionSvc::interfaceID() )  {
        *ppvInterface = (IConversionSvc*)this;
    }
    else if ( riid == IDetDescrCnvSvc::interfaceID() )  {
        *ppvInterface = (IDetDescrCnvSvc*)this;
    }
    else   {
        return ConversionSvc::queryInterface( riid, ppvInterface );
    }
    addRef();
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

/// Initialize the service.
StatusCode
DetDescrCnvSvc::initialize()     {

    MsgStream log(msgSvc(),name());
    StatusCode status = ConversionSvc::initialize();
    if (status.isFailure()) {
	log << MSG::FATAL << "Unable to initialize ConversionSvc !" << endmsg;
	return StatusCode::FAILURE;
    }

    log << MSG::INFO << " initializing " <<endmsg ;

    // get DetectorStore service
    status = service("DetectorStore", m_detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {
	log << MSG::INFO << "Found DetectorStore service" << endmsg;
    }

    // fill in the Addresses for Transient Detector Store objects

    log << MSG::INFO << " filling proxies for detector managers " <<endmsg ;

    // Obsolete --- this CLID no longer exists
    //status = addToDetStore(2512, "InDetMgr");
    //if (status != StatusCode::SUCCESS) return status;
    // now LArGeoModel does it itself
    //    status =  addToDetStore(2741, "LArMgr");
    // if (status != StatusCode::SUCCESS) return status;
    // now TileGeoModel is does it itself
    //    status =  addToDetStore(2941, "TileMgr");
    // if (status != StatusCode::SUCCESS) return status;
    // Obsolete --- this CLID no longer exists
    //status =  addToDetStore(4060, "MuonMgr");
    //if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(117659265, "CaloTTMgr");
    if (status != StatusCode::SUCCESS) return status;


    status =  addToDetStore(4548337, "CaloMgr");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(241807251, "CaloSuperCellMgr");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(125856940, "CaloIdManager");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(79554919, "LArIdManager");
    if (status != StatusCode::SUCCESS) return status;


    // IdDict:
    status =  addToDetStore(2411, "IdDict");
    if (status != StatusCode::SUCCESS) return status;

    // IdHelpers
    status =  addToDetStore(164875623, "AtlasID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2516, "PixelID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2517, "SCT_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2518, "TRT_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(129452393, "SiliconID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(163583365, "LArEM_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(99488227, "LArEM_SuperCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(3870484, "LArHEC_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(254277678, "LArHEC_SuperCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(45738051, "LArFCAL_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(12829437, "LArFCAL_SuperCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(79264204, "LArMiniFCAL_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(158698068, "LArOnlineID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(38321944, "TTOnlineID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(115600394, "LArOnline_SuperCellID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(27863673, "LArHVLineID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(80757351, "LArElectrodeID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2901, "TileID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(49557789, "Tile_SuperCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2902, "TileHWID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(2903, "TileTBID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(4170, "MDTIDHELPER");
    if (status != StatusCode::SUCCESS) return status;
    if (m_hasCSC) {
        status =  addToDetStore(4171, "CSCIDHELPER");
        if (status != StatusCode::SUCCESS) return status;
    }
    status =  addToDetStore(4172, "RPCIDHELPER");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(4173, "TGCIDHELPER");
    if (status != StatusCode::SUCCESS) return status;    
    if (m_hasSTgc) {
        status =  addToDetStore(4174, "STGCIDHELPER");
        if (status != StatusCode::SUCCESS) return status;
    }
    if (m_hasMM) {
        status =  addToDetStore(4175, "MMIDHELPER");     
        if (status != StatusCode::SUCCESS) return status;
    }
    status =  addToDetStore(108133391, "CaloLVL1_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(123500438, "CaloCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(128365736, "CaloCell_SuperCell_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(167756483, "CaloDM_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(190591643, "ZdcID");
    if (status != StatusCode::SUCCESS) return status;

    return status;
}

//-------------------------------------------------------------------------

/// Create a Generic address using explicit arguments to identify a single object.
StatusCode DetDescrCnvSvc::createAddress(long            /* svc_type */,
					 const CLID&          /* clid     */,
					 const std::string*   /* par      */,
					 const unsigned long* /* ip       */,
					 IOpaqueAddress*& refpAddress)    {
    refpAddress = 0;
    return StatusCode::FAILURE;
}

//-------------------------------------------------------------------------

StatusCode DetDescrCnvSvc::createAddress( long /* svc_type */,
					  const CLID& clid,
					  const std::string& refAddress,
					  IOpaqueAddress*& refpAddress)
{
    try {
	refpAddress = new DetDescrAddress(clid);
	DetDescrAddress* ddAddr;
	ddAddr = dynamic_cast<DetDescrAddress*> (refpAddress);
	if(!ddAddr) {
	    MsgStream log(msgSvc(),name());
	    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	    return StatusCode::FAILURE;
	}
	if (ddAddr->fromString(refAddress).isFailure()) {
	  MsgStream log(msgSvc(),name());
	  log << MSG::FATAL << "Could not assign address " << refAddress << endmsg;
	  return StatusCode::FAILURE;
	}
    }
    catch(...) {
	refpAddress = 0;
    }
    return (refpAddress != 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//-------------------------------------------------------------------------

StatusCode DetDescrCnvSvc::convertAddress( const IOpaqueAddress* pAddress,
                                           std::string& refAddress)
{
  const DetDescrAddress* addr = dynamic_cast<const DetDescrAddress*>(pAddress);
  if (!addr) return StatusCode::FAILURE;
  return addr->toString(refAddress);
}


//-------------------------------------------------------------------------

StatusCode
DetDescrCnvSvc::addToDetStore(const CLID& clid, const std::string& name) const
{

    MsgStream log(msgSvc(), "DetDescrCnvSvc");

    // Based on input parameters, create StoreGate proxies with
    // DetDescrAddresses in the detector store for the different
    // detectors.

    // fill in the Addresses for Transient Detector Store objects

    DetDescrAddress * addr;
    addr  = new DetDescrAddress(clid, name, name);
    StatusCode status = m_detStore->recordAddress(addr);
    if (status != StatusCode::SUCCESS) {
	log << MSG::INFO
	    << " unable to fill address for " << (*addr->par())
	    << " with CLID " << addr->clID()
	    << " and storage type " << addr->svcType()
	    << " to detector store " <<endmsg ;
	return StatusCode::FAILURE;
    }
    else {
	unsigned int st = addr->svcType();
	log << MSG::INFO
	    << " filling address for " << (*addr->par())
	    << " with CLID " << addr->clID()
	    << " and storage type " << st
	    << " to detector store " <<endmsg ;
    }
    return status;
}

