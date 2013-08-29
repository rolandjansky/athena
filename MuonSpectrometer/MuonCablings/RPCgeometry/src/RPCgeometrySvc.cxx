/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// METHODS ADDED, INITIALIZATION MISSING VEDI add.cxx
/// NEED TO GET cablingSvc
/// m_pRpcGeo DA SEMPLIFICARE
///

#ifndef LVL1_STANDALONE

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SvcFactory.h"

#include "StoreGate/StoreGate.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "RPCgeometry/RPCgeometrySvc.h"
#include "RPCcablingInterface/RDOindex.h"

#include <TH1F.h>

RPCgeometrySvc::RPCgeometrySvc(const std::string& name,ISvcLocator* sl) :
    Service(name,sl),
    m_alignment_updated(false),
    m_cablingInitFromDB(false),
    m_histProp_alignCorrOnX(Gaudi::Histo1DDef("AlignCorr_X",-10.,10.,100)),
    m_histProp_alignCorrOnY(Gaudi::Histo1DDef("AlignCorr_Y",-10.,10.,100)),
    m_histProp_alignCorrOnZ(Gaudi::Histo1DDef("AlignCorr_Z",-10.,10.,100)),
    m_histProp_alignCorrOnMeanX(Gaudi::Histo1DDef("AlignCorr_MeanX",-10.,10.,100)),
    m_histProp_alignCorrOnMeanY(Gaudi::Histo1DDef("AlignCorr_MeanY",-10.,10.,100)),
    m_histProp_alignCorrOnMeanZ(Gaudi::Histo1DDef("AlignCorr_MeanZ",-10.,10.,100)),
    m_AmdcsimrecAthenaSvc ("AmdcsimrecAthenaSvc",name),
    m_hist_alignCorrOnX(0),
    m_hist_alignCorrOnY(0),
    m_hist_alignCorrOnZ(0),
    m_hist_alignCorrOnMeanX(0),
    m_hist_alignCorrOnMeanY(0),
    m_hist_alignCorrOnMeanZ(0)
{
    declareProperty( "DumpRPCGeometry", m_dump_RPC_geometry=false, 
          "Switch for dumping the chamber geometry on the standard outpu" );
    declareProperty( "DumpRPCStations", m_dump_RPC_stations=false,
          "Switch for dumping the logic sectors on the standard output" );
    declareProperty( "Debug", m_debug=false,
          "Switch for activating the debug printouts" );
    declareProperty( "DumpLVL1conf", m_dump_lvl1conf=false,
          "Switch for outputing the RPC setup into a file" );
    declareProperty( "DumpFileName", m_dump_filename="LVL1conf.dump",
          "Set the file name where to output the RPC setup" );
    
    declareProperty( "Use_cm", m_unit_in_cm=true,
          "Switch for using the cm unit measurement" );
    
    declareProperty( "H8testbeam", m_H8=false,
          "Switch for building an incomplete geometry as the testbeam one");
    
    declareProperty ( "UseOldGeoAccess", m_useOldGeoAccess=true,
          "Switch for using the old geometry access");
	  
    declareProperty( "DoNotUseAlignment", m_doNotUseAlignment=false,
          "Switch for turning off the use of alignment corrections");
    declareProperty( "DoNotInitFromDB", m_doNotInitFromDB=false,
          "Switch for turning off the cabling initialization from database");
    
    declareProperty( "DoMonitorAlignment", m_doMonitorAlignment=false);
    declareProperty( "AlignCorr_X",m_histProp_alignCorrOnX);
    declareProperty( "AlignCorr_Y",m_histProp_alignCorrOnY);
    declareProperty( "AlignCorr_Z",m_histProp_alignCorrOnZ);
    declareProperty( "AlignCorr_MeanX",m_histProp_alignCorrOnMeanX);
    declareProperty( "AlignCorr_MeanY",m_histProp_alignCorrOnMeanY);
    declareProperty( "AlignCorr_MeanZ",m_histProp_alignCorrOnMeanZ);
}

// queryInterface 
StatusCode RPCgeometrySvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if(IID_IRPCgeometrySvc.versionMatch(riid) ) 
    { 
        *ppvIF = (IRPCgeometrySvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode RPCgeometrySvc::initialize() 
{
    StatusCode sc;

    MsgStream log(messageService(), name());

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;

    
    if (log.level() <= MSG::INFO)  {    
        log << MSG::INFO << "Start of run initialisation" << endreq;
        log << MSG::INFO << "==== Global  configuration ====" << endreq;
        log << MSG::INFO << m_unit_in_cm << endreq;
        log << MSG::INFO << m_H8 << endreq;
        log << MSG::INFO << m_doNotUseAlignment << endreq;
        log << MSG::INFO << m_useOldGeoAccess << endreq;
        log << MSG::INFO << m_doNotInitFromDB << endreq;
        log << MSG::INFO << m_doMonitorAlignment << endreq;
        log << MSG::INFO << "===== Debug configuration =====" << endreq;
        log << MSG::INFO << m_dump_RPC_geometry << endreq;
        log << MSG::INFO << m_dump_RPC_stations << endreq;
        log << MSG::INFO << m_debug << endreq;
        log << MSG::INFO << "==== Output  configuration ====" << endreq;
        log << MSG::INFO << m_dump_lvl1conf << endreq;
        log << MSG::INFO << m_dump_filename << endreq;
    }
    
    // retrieve the AMDC geometry, if not initialized set a call back
    if (m_AmdcsimrecAthenaSvc.retrieve().isFailure()) {
        log << MSG::FATAL << "Failed to retrieve service " 
                          << m_AmdcsimrecAthenaSvc << endreq;
        return StatusCode::FAILURE;
    }
    
    if ( !m_AmdcsimrecAthenaSvc->InitializedSvc() ) {
        if (m_doNotUseAlignment) {
            log << MSG::WARNING 
	        << "AMDC needs callback but RPCgeometry is forced not to use it."
	        << "Callback to updateAMDC not registered."
	        << endreq;
            m_alignment_updated = true;
	} else {
	    StoreGateSvc* m_detStore;
	    sc = service("DetectorStore", m_detStore);
	    if (!sc.isSuccess() || 0 == m_detStore)  {
	        log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
	        return StatusCode::FAILURE;
	    }
	
            sc = m_detStore->regFcn(
                    &AmdcsimrecAthenaSvc::UpdatedSvc,(&*m_AmdcsimrecAthenaSvc),
                    &RPCgeometrySvc::updateAMDC,this,true);
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
    
    // get cabling svc
    const IRPCcablingServerSvc* RpcCabGet = 0;
    sc = service("RPCcablingServerSvc", RpcCabGet);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RPCcablingServerSvc !" << endreq;
        return StatusCode::FAILURE;
    }
    else log << MSG::VERBOSE  << " RPCcablingServerSvc retrieved" << endreq;

    sc = RpcCabGet->giveCabling(m_rpcCabling);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RPCcablingSvc from the Server !" << endreq;
        m_rpcCabling = 0;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::VERBOSE << " RPCcablingSvc obtained " << endreq;
    }
     
    std::string svcName=m_rpcCabling->rpcCabSvcType();
    
    log<<MSG::INFO<<"Rpc Cabling Svc name is "<<svcName<<endreq;

    if (svcName.find("sim")!=std::string::npos)
    {
	m_cablingInitFromDB = true;
    }
    else
    {
        if(m_doNotInitFromDB) {
	    log << MSG::WARNING 
	        << "RPCcabling needs callback but RPCgeometry is forced not to use it."
	        << "Callback to initFromCablingSvc not registered."
	        << endreq;
            m_cablingInitFromDB = true;
	} else {
	    StoreGateSvc* m_detStore;
	
	    sc = service("DetectorStore", m_detStore);
	
	    if (!sc.isSuccess() || 0 == m_detStore)  {
	        log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
	        return StatusCode::FAILURE;
	    }
	
	    sc = m_detStore->regFcn(&IRPCcablingSvc::initMappingModel, 
				&*m_rpcCabling,
				&RPCgeometrySvc::initFromCablingSvc, 
				this);
	
	    if (sc.isFailure()) {
                log << MSG::FATAL 
		    << "Unable to register callback on initFromCablingSvc "
                    << "from IRPCcablingSvc::initMappingModel " << endreq;
                return StatusCode::FAILURE;
            }
	    log << MSG::INFO << "Registered callback on initFromCablingSvc  "
                << "from IRPCcablingSvc::initMappingModel " << endreq;
        }
    }
    
    // try to build geometry
    sc = buildRPCGeometry(); 
    
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
            log << MSG::INFO << "Postpone RPCGeometry creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }

   
    /*
    // build the map a la region selector for the PAD added from cabling
    m_hashVec.clear();
    m_hashVec = m_rpcCabling->GiveHashVec();
    
    std::vector<const RDOindex*>::const_iterator hash = m_hashVec.begin();
    for( ; hash!= m_hashVec.end(); ++hash)
    {
        unsigned short int sub = ((*hash)->side()==0x65)? 1 : 0;
	unsigned short int sec =  (*hash)->SLid();
	unsigned short int pad =  (*hash)->PADid();
	
	PADdim dimension;
	
	if(etaPhiDim(sub,sec,pad,dimension)) m_regions.push_back(dimension);
    }
    
    
    DimensionMap::const_iterator it = m_regions.begin();
    log << MSG::DEBUG << "DUMP THE ETA-PHI MAP OF THE PADs: " 
        << m_hashVec.size() << endreq;
    uint32_t hashID = 0;


    // DEBUG PART TO BE FIXED

      for( ; it!=m_regions.end();++it)
      {  
        log << MSG::DEBUG << "hashID=" << hashID << "  "
            << "side=" <<     (m_hashVec[hashID])->side()
            << ", sector=" << (m_hashVec[hashID])->SLid()
            << ", pad=" <<    (m_hashVec[hashID])->PADid() 
            << ":  EtaMin=" <<(*it).etaMin
            << ", EtaMax=" << (*it).etaMax
            << ", PhiMin=" << (*it).phiMin
            << ", PhiMax=" << (*it).phiMax		   
            << endreq;
	 ++hashID;
      }
    */

    /*
    std::vector<uint32_t> found = getHash(-.3, .3, 6., 0.2);
    std::vector<uint32_t>::const_iterator pippo = found.begin();
    for( ; pippo!=found.end();++pippo)
    {
         std::cout << "hashID=" << *pippo << "  "
                   << "side=" << (m_hashVec[*pippo])->side()
	           << ", sector=" << (m_hashVec[*pippo])->SLid()
		   << ", pad=" << (m_hashVec[*pippo])->PADid() 
		   << ":  EtaMin=" << m_regions[*pippo].etaMin
		   << ", EtaMax=" << m_regions[*pippo].etaMax
		   << ", PhiMin=" << m_regions[*pippo].phiMin
		   << ", PhiMax=" << m_regions[*pippo].phiMax		   
		   << std::endl;
    }
    */
   


    return StatusCode::SUCCESS;
}

StatusCode RPCgeometrySvc::buildRPCGeometry() 
{   
    MsgStream log(messageService(), name());

    if (log.level() <= MSG::DEBUG ) {        
        log << MSG::DEBUG << "AMDC initialization is "
            << m_AmdcsimrecAthenaSvc->InitializedSvc() << endreq;
        log << MSG::DEBUG << m_alignment_updated << endreq;
        log << MSG::DEBUG << m_cablingInitFromDB << endreq;
    }
    //check whether AMDC is initialized 
    if (!m_AmdcsimrecAthenaSvc->InitializedSvc() && !m_alignment_updated) {
        log << MSG::INFO << "Cannot build " << name()
                         << " now, becasue AMDC is not yet initialized" 
                         << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    // check whether the RPC cabling is initialized
    if (!m_cablingInitFromDB) {
        log << MSG::INFO << "Cannot build " << name()
                         << " now, becasue the RPC cabling is not yet initialized"
                         << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    // find histogramming service
    ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
    if(m_doMonitorAlignment) {
      if ((rootHistSvc.retrieve()).isFailure()) {
          log << MSG::ERROR << "Unable to locate THistSvc" << endreq;
          rootHistSvc.release().ignore();
          return StatusCode::FAILURE;
      }
    }
    
    log << MSG::INFO << "Building RPCgeometry LUT" << endreq;
    
    std::string histopath = std::string("/EXPERT/RPCgeometry/");
    
    // reset, create and register the histogram monitoring the alignment
    if (m_hist_alignCorrOnX==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnX = new TH1F 
         (m_histProp_alignCorrOnX.value().title().c_str(),
         (m_histProp_alignCorrOnX.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnX.value().bins(),
          m_histProp_alignCorrOnX.value().lowEdge(),
          m_histProp_alignCorrOnX.value().highEdge());

        if (m_hist_alignCorrOnX) {
            m_hist_alignCorrOnX->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnX->GetName(), 
	       m_hist_alignCorrOnX).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnX->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_alignCorrOnY==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnY = new TH1F 
         (m_histProp_alignCorrOnY.value().title().c_str(),
         (m_histProp_alignCorrOnY.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnY.value().bins(),
          m_histProp_alignCorrOnY.value().lowEdge(),
          m_histProp_alignCorrOnY.value().highEdge());

        if (m_hist_alignCorrOnY) {
            m_hist_alignCorrOnY->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnY->GetName(), 
	       m_hist_alignCorrOnY).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnY->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_alignCorrOnZ==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnZ = new TH1F 
         (m_histProp_alignCorrOnZ.value().title().c_str(),
         (m_histProp_alignCorrOnZ.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnZ.value().bins(),
          m_histProp_alignCorrOnZ.value().lowEdge(),
          m_histProp_alignCorrOnZ.value().highEdge());

        if (m_hist_alignCorrOnZ) {
            m_hist_alignCorrOnZ->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnZ->GetName(), 
	       m_hist_alignCorrOnZ).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnZ->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_alignCorrOnMeanX==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnMeanX = new TH1F 
         (m_histProp_alignCorrOnMeanX.value().title().c_str(),
         (m_histProp_alignCorrOnMeanX.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnMeanX.value().bins(),
          m_histProp_alignCorrOnMeanX.value().lowEdge(),
          m_histProp_alignCorrOnMeanX.value().highEdge());

        if (m_hist_alignCorrOnMeanX) {
            m_hist_alignCorrOnMeanX->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnMeanX->GetName(), 
	       m_hist_alignCorrOnMeanX).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnMeanX->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_alignCorrOnMeanY==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnMeanY = new TH1F 
         (m_histProp_alignCorrOnMeanY.value().title().c_str(),
         (m_histProp_alignCorrOnMeanY.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnMeanY.value().bins(),
          m_histProp_alignCorrOnMeanY.value().lowEdge(),
          m_histProp_alignCorrOnMeanY.value().highEdge());

        if (m_hist_alignCorrOnMeanY) {
            m_hist_alignCorrOnMeanY->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnMeanY->GetName(), 
	       m_hist_alignCorrOnMeanY).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnMeanY->GetName() << endreq;
            }
        }
    }
    
    if(m_hist_alignCorrOnMeanZ==0 && m_doMonitorAlignment) {
        m_hist_alignCorrOnMeanZ = new TH1F 
         (m_histProp_alignCorrOnMeanZ.value().title().c_str(),
         (m_histProp_alignCorrOnMeanZ.value().title()+";diff").c_str(),
          m_histProp_alignCorrOnMeanZ.value().bins(),
          m_histProp_alignCorrOnMeanZ.value().lowEdge(),
          m_histProp_alignCorrOnMeanZ.value().highEdge());

        if (m_hist_alignCorrOnMeanZ) {
            m_hist_alignCorrOnMeanZ->SetBit(TH1::kCanRebin);
            if( rootHistSvc->regHist(
	       histopath + m_hist_alignCorrOnMeanZ->GetName(), 
	       m_hist_alignCorrOnMeanZ).isFailure() ) {
                log << MSG::WARNING 
		    << "Can not register monitoring histogram: " 
	            << m_hist_alignCorrOnMeanZ->GetName() << endreq;
            }
        }
    }
    
    if (m_doMonitorAlignment) rootHistSvc.release().ignore();
    
    // Set RPC geometry static fields
    RPCGeometry::s_amdc    = m_AmdcsimrecAthenaSvc->GetAmdcsimrec();
    RPCGeometry::DumpMap   = m_dump_RPC_geometry;
    RPCGeometry::DumpLog   = m_dump_RPC_stations;
    RPCGeometry::DebuGeo   = m_debug;
    RPCGeometry::DumpCon   = m_dump_lvl1conf;
    RPCGeometry::FileName  = m_dump_filename;
    
    RPCGeometry::s_useOldGeoAccess = m_useOldGeoAccess;
    
    #ifndef LVL1_STANDALONE
    RPCGeometry::s_hist_alignCorrOnX     = m_hist_alignCorrOnX;
    RPCGeometry::s_hist_alignCorrOnY     = m_hist_alignCorrOnY;
    RPCGeometry::s_hist_alignCorrOnZ     = m_hist_alignCorrOnZ;
    RPCGeometry::s_hist_alignCorrOnMeanX = m_hist_alignCorrOnMeanX;
    RPCGeometry::s_hist_alignCorrOnMeanY = m_hist_alignCorrOnMeanY;
    RPCGeometry::s_hist_alignCorrOnMeanZ = m_hist_alignCorrOnMeanZ;
    #endif
    
    // set unit to be used
    if(m_unit_in_cm) RPCGeometry::s_ScaleFactor = CLHEP::centimeter;
    else             RPCGeometry::s_ScaleFactor = CLHEP::millimeter;
    
    // set the type of the AMDB geometry read
    RPCGeometry::s_testbeam = m_H8;
    
    RPCGeometry::instance();

    if ( !RPCGeometry::s_status )
    {
        log << MSG::ERROR << name() <<  ": RPC geometry model is not loaded!" 
            << endreq;
        delete RPCGeometry::s_instance;
        return StatusCode::FAILURE;
    } else {
        log << MSG::INFO << name() << ": RPC geometry model loaded!" << endreq;
    }
    
    StatusCode sc = initTowerMap();
    if(sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;

}


StatusCode RPCgeometrySvc::initTowerMap()
{
    MsgStream log(messageService(), name());
  
    m_hashVec.clear();
    m_regions.clear();
    
    m_hashVec = m_rpcCabling->GiveHashVec();
    log << MSG::INFO <<"From the cabling svc map has size: "<< m_hashVec.size()<<endreq;

    std::vector<const RDOindex*>::const_iterator hash = m_hashVec.begin();
    for( ; hash!= m_hashVec.end(); ++hash)
    {
        unsigned short int sub = ((*hash)->side()==0x65)? 1 : 0;
	unsigned short int sec =  (*hash)->SLid();
	unsigned short int pad =  (*hash)->PADid();
	
	PADdim dimension;
	
	if(etaPhiDim(sub,sec,pad,dimension)) m_regions.push_back(dimension);
    }
    


    if (log.level() <= MSG::DEBUG) {
        DimensionMap::const_iterator it = m_regions.begin();
        log << MSG::DEBUG << "DUMP THE ETA-PHI MAP OF THE PADs: " 
            << m_hashVec.size() << endreq;
        uint32_t hashID = 0;
        // DEBUG PART TO BE FIXED    
        for( ; it!=m_regions.end();++it)
        {  
            log << MSG::DEBUG << "hashID=" << hashID << "  "
                << "side=" <<     (m_hashVec[hashID])->side()
                << ", sector=" << (m_hashVec[hashID])->SLid()
                << ", pad=" <<    (m_hashVec[hashID])->PADid() 
                << ":  EtaMin=" <<(*it).etaMin
                << ", EtaMax=" << (*it).etaMax
                << ", PhiMin=" << (*it).phiMin
                << ", PhiMax=" << (*it).phiMax		   
                << endreq;
            ++hashID;
        }
    }
    return StatusCode::SUCCESS;
}


StatusCode RPCgeometrySvc::finalize() 
{
    delete RPCGeometry::s_instance;
    RPCGeometry::s_status = false;
    return StatusCode::SUCCESS;
} 

// Implement the specifics ... 

bool
RPCgeometrySvc::give_strip_index(unsigned long int code,int& Jtype, 
                              int& StationEta, int& StationPhi, int& DoubletR, 
                              int& DoubletZ,   int& DoubletP,   int& GasGap, 
                              int& MeasuresPhi,int& Strip) const
{
    return RPCGeometry::s_instance->give_strip_index(code,Jtype,StationEta,
		      StationPhi,DoubletR,DoubletZ,DoubletP,GasGap,MeasuresPhi,
		      Strip);
}


unsigned long int
RPCgeometrySvc::give_strip_code( std::string StationName, int StationEta,
			      int StationPhi, int DoubletR, int DoubletZ,
                              int DoubletP, int GasGap, int MeasuresPhi,
			      int Strip) const
{    
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_strip_code(StationName,StationEta,
		      StationPhi,DoubletR,DoubletZ,DoubletP,GasGap,MeasuresPhi,
		      Strip);
    }
    return 0;
}

unsigned long int
RPCgeometrySvc::give_strip_code(int Jtype,int Jff,int Jzz,int Jobj,int Jlay,
                             int Jsty,int Jstr,int proj) const
{
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_strip_code(Jtype,Jff,Jzz,Jobj,
                                                        Jlay,Jsty,Jstr,proj);
    }
    return 0;
}


bool
RPCgeometrySvc::give_strip_coordinates(unsigned long int code, float *xyz) const
{    
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_strip_coordinates(code,xyz);
    }
    return false;
}

bool
RPCgeometrySvc::give_strip_radius(unsigned long int code,float& radius) const
{
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_strip_radius(code,radius);
    }
    return false;
}

bool
RPCgeometrySvc::give_eta_strip_size(unsigned int code,float& size) const
{
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_eta_strip_size(code,size);
    }
    return false;  
}

bool
RPCgeometrySvc::give_phi_strip_size(unsigned int code,float& size) const
{
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->give_phi_strip_size(code,size);
    }
    return false;
}

bool
RPCgeometrySvc::existCode(unsigned int code) const
{
    if(RPCGeometry::s_status)
    {
        return RPCGeometry::s_instance->existCode(code);
    }
    return false;
}


const RPCGeometry*
RPCgeometrySvc::rpcGeometry() const
{
    return RPCGeometry::s_instance;
}


// --------------- START OF METHODS COPIED HERE FROM

unsigned int 
RPCgeometrySvc::checkLowCode(unsigned int code) const
{
    //    while (code%100 && !m_pRpcGeo->existCode(code)) {code -= 1;}
    while (code%100 && !existCode(code)) {code -= 1;} 
    return code;
}

unsigned int 
RPCgeometrySvc::checkHighCode(unsigned int code) const
{
    //    while (code%100 && !m_pRpcGeo->existCode(code)) {code += 1;}
    while (code%100 && !existCode(code)) {code += 1;}
    return code;
}

bool 
RPCgeometrySvc::etaPhiDim(unsigned short int SubsystemId,
                         unsigned short int SectorId,
                         unsigned short int PADId,
		         PADdim& dim) const
{

    MsgStream mlog(messageService(), name());
    if (mlog.level() <= MSG::DEBUG) {
        mlog<<MSG::DEBUG<<"compute etaPhiDim for subSysId, secId, PadId "<<SubsystemId<<" "<<SectorId<<" "<<PADId<<endreq;
    }
    
        
    for (int i=PADId*4;i<(PADId+1)*4;++i)
    {
       unsigned int EtaLowBorder1;
       unsigned int EtaHighBorder1;
       unsigned int PhiLowBorder1;
       unsigned int PhiHighBorder1;
       
       unsigned int EtaLowBorder2;
       unsigned int EtaHighBorder2;
       unsigned int PhiLowBorder2;
       unsigned int PhiHighBorder2;
       
       bool low = m_rpcCabling->give_LowPt_borders(SubsystemId, 
                                                   SectorId, i,
                                                   EtaLowBorder1, 
                                                   EtaHighBorder1, 
                                                   PhiLowBorder1, 
                                                   PhiHighBorder1);
       
       bool high = m_rpcCabling->give_HighPt_borders(SubsystemId, 
                                                     SectorId, i,
                                                     EtaLowBorder2, 
                                                     EtaHighBorder2, 
                                                     PhiLowBorder2, 
                                                     PhiHighBorder2); 
       
       PADdim etaphi;
       bool result = true;
       
       if(low && high) 
       {
           PADdim etaphi1;
           bool res1 = giveEtaPhi(EtaLowBorder1,EtaHighBorder1,
                                     PhiLowBorder1,PhiHighBorder1,
				     etaphi1);
           PADdim etaphi2;
           bool res2 = giveEtaPhi(EtaLowBorder2,EtaHighBorder2,
				     PhiLowBorder2,PhiHighBorder2,
				     etaphi2);
       
           result = res1 | res2;
           if(res1&res2)
	   {
	       etaphi.etaMin =(etaphi1.etaMin > etaphi2.etaMin)? etaphi2.etaMin: 
	                                                         etaphi1.etaMin;
               etaphi.etaMax =(etaphi1.etaMax < etaphi2.etaMax)? etaphi2.etaMax:
	                                                         etaphi1.etaMax;
               etaphi.phiMin =(etaphi1.phiMin > etaphi2.phiMin)? etaphi2.phiMin:
	                                                         etaphi1.phiMin;
               etaphi.phiMax =(etaphi1.phiMax < etaphi2.phiMax)? etaphi2.phiMax:
	                                                         etaphi1.phiMax;  
	   }
	   else if (res1)
	   {
	       etaphi.etaMin = etaphi1.etaMin;
               etaphi.etaMax = etaphi1.etaMax;
               etaphi.phiMin = etaphi1.phiMin;
               etaphi.phiMax = etaphi1.phiMax;
	   }
	   else if (res2)
	   {
	       etaphi.etaMin = etaphi2.etaMin;
               etaphi.etaMax = etaphi2.etaMax;
               etaphi.phiMin = etaphi2.phiMin;
               etaphi.phiMax = etaphi2.phiMax;
	   }
       }
       else if(low)    result &= giveEtaPhi(EtaLowBorder1,EtaHighBorder1,
                                   PhiLowBorder1,PhiHighBorder1,
				   etaphi);
       
       else if(high)   result &= giveEtaPhi(EtaLowBorder2,EtaHighBorder2,
                                   PhiLowBorder2,PhiHighBorder2,
				   etaphi);
       
       else continue;
       
       if(result)
       {
           if( dim.etaMin > etaphi.etaMin ) dim.etaMin = etaphi.etaMin;
           if( dim.etaMax < etaphi.etaMax ) dim.etaMax = etaphi.etaMax;
           if( dim.phiMin > etaphi.phiMin ) dim.phiMin = etaphi.phiMin;
           if( dim.phiMax < etaphi.phiMax ) dim.phiMax = etaphi.phiMax;
       }
    }
    
    //if(dim.etaMin == +99999. && dim.etaMax == -99999. &&
    //   dim.phiMin == +4*M_PI && dim.phiMax == -4*M_PI) return false;
    
       
    return true;   
}


bool 
RPCgeometrySvc::giveEtaPhi(unsigned long int EtaLowBorder,
                          unsigned long int EtaHighBorder,
                          unsigned long int PhiLowBorder,
		          unsigned long int PhiHighBorder,
                          PADdim& etaphi) const
{
    MsgStream mlog(messageService(), name());
    if (mlog.level() <= MSG::DEBUG)
        mlog << MSG::DEBUG << "EtaLowBorder=" << EtaLowBorder
                           << ", EtaHighBorder=" << EtaLowBorder
		           << ", PhiLowBorder=" << PhiLowBorder
		           << ", PhiHighBorder=" << PhiHighBorder << endreq;
    
    if(EtaLowBorder==EtaHighBorder || PhiLowBorder==PhiHighBorder) return false;
    //    if(m_pRpcGeo)
    //{
        float firstEta[3] = {0.,0.,0.};
        float lastEta[3]  = {0.,0.,0.};
        float firstPhi[3] = {0.,0.,0.};
        float lastPhi[3]  = {0.,0.,0.};
	
	float etaRad = 0;

        EtaLowBorder = checkLowCode( EtaLowBorder );    
        if(!give_strip_coordinates(EtaLowBorder, firstEta))
        {
	    mlog << MSG::ERROR
                 << "Cannot retrieve the coordinates for EtaLowBorder=" 
		 << EtaLowBorder << endreq;
	    return false;
        }   
    
        EtaHighBorder = checkHighCode( EtaHighBorder );
        if(!give_strip_coordinates(EtaHighBorder, lastEta))
        {
	    mlog << MSG::ERROR
                 << "Cannot retrieve the coordinates for EtaHighBorder=" 
		 << EtaHighBorder << endreq;
	    return false;
        }
    
        PhiLowBorder = checkLowCode( PhiLowBorder );
        if(!give_strip_coordinates(PhiLowBorder, firstPhi))
        {
	    mlog << MSG::ERROR
                 << "Cannot retrieve the coordinates for PhiLowBorder=" 
		 << PhiLowBorder << endreq;
	    return false;
        }   
    
        PhiHighBorder = checkHighCode( PhiHighBorder );
        if(!give_strip_coordinates(PhiHighBorder, lastPhi))
        {
	    mlog << MSG::ERROR
                 << "Cannot retrieve the coordinates for PhiHighBorder=" 
		 << PhiHighBorder << endreq;
	    return false;
        }
    
        	
        if(!give_strip_radius(EtaLowBorder,etaRad))
        {
	    mlog << MSG::ERROR
                 << "Cannot retrieve the gas Layer Radius for EtaLowBorder=" 
		 << EtaLowBorder << endreq;
	    return false;
        }   
    

	
	float etaMin = +99999.;
	float etaMax = -99999.;
	float phiMin = +4*M_PI;
	float phiMax = -4*M_PI;
    
        computeEtaPhi(firstEta,lastEta,etaRad,firstPhi,lastPhi,
	              etaMin, etaMax, phiMin, phiMax);    
    
        if (etaMin == 0. && etaMax == 0. && phiMin == 0. && phiMax == 0.)
	  return false;
        
	etaphi.etaMin = etaMin;
	etaphi.etaMax = etaMax;
	etaphi.phiMin = phiMin;
	etaphi.phiMax = phiMax;
    
        return true;
        //  }
        //return false;
}

void 
RPCgeometrySvc::computeEtaPhi(float etaLow[3], float etaHigh[3], float etaRad,
                             float phiLow[3], float phiHigh[3],
			     float& etaMin, float& etaMax, 
                             float& phiMin, float& phiMax) const
{
    float Theta;
    if(etaRad==0) {
        MsgStream mlog(messageService(), name());
        mlog << MSG::ERROR 
	     << "RPCcablingSvc::computeEtaPhi called with etaRad == 0!"
	     << endreq;
        
	etaMin = 0.;
	etaMax = 0.;
	phiMin = 0.;
	phiMax = 0.;
	
	return;
    }
    
    Theta  = (etaLow[2] != 0)? atan(etaRad/fabsf(etaLow[2])) : asin(1);
    etaMin = (etaLow[2]  > 0)? -log(tan(Theta/2.)) : log(tan(Theta/2.));
    
    Theta  = (etaHigh[2] != 0)? atan(etaRad/fabsf(etaHigh[2])) : asin(1);
    etaMax = (etaHigh[2]  > 0)? -log(tan(Theta/2.)) : log(tan(Theta/2.));
        
    
    if(etaMin > etaMax)
    {
        float tmp = etaMax;
	etaMax = etaMin;
	etaMin = tmp;
    }
    
    phiMin = atan2(phiLow[1],phiLow[0]);
    phiMax = atan2(phiHigh[1] ,phiHigh[0]);
    
    MsgStream mlog(messageService(), name());
    if (mlog.level() <= MSG::DEBUG) {
        mlog << MSG::DEBUG << "phiLow = { "
                          << phiLow[0] << " , " << phiLow[1] << " , "
		          << phiLow[2] << " }" << endreq;
        mlog << MSG::DEBUG << "phiMin = atan2(phiLow[1],phiLow[0]) = " << phiMin
                          << endreq;
        mlog << MSG::DEBUG << "phiHigh = { "
                          << phiHigh[0] << " , " << phiHigh[1] << " , "
		          << phiHigh[2] << " }" << endreq;
        mlog << MSG::DEBUG << "phiMax = atan2(phiHigh[1],phiHigh[0]) = " << phiMax
                          << endreq;
        mlog << MSG::DEBUG << "M_PI=" << M_PI << endreq;
    }
    
    if(phiMin < 0.) phiMin += 2*M_PI;
    if(phiMax < 0.) phiMax += 2*M_PI;
}


std::vector<uint32_t> 
RPCgeometrySvc::getRobId (float etaMin, float etaMax,
                   float phiMin,float phiMax) const
{
    std::set<uint32_t> ROBset;
    std::set<uint32_t>::const_iterator rob_set;
    
    std::vector<uint32_t> hash = getHash(etaMin, etaMax, phiMin, phiMax);
    
    std::vector<uint32_t>::const_iterator it = hash.begin();
    for( ; it!=hash.end(); ++it)
    {
        const unsigned short int rob_id = (m_hashVec[*it])->ROBid();
        const unsigned short int sub_id = (m_hashVec[*it])->side();
        uint32_t ROB_ID = (sub_id << 16) | rob_id;
	ROBset.insert(ROB_ID);
    }
    
    
    std::vector<uint32_t> ROBid;
    for(rob_set=ROBset.begin(); rob_set!=ROBset.end(); ++rob_set)
    {
        ROBid.push_back(*rob_set);
    }
    
    
    return ROBid;
}
    

std::vector<uint32_t> 
RPCgeometrySvc::getPadHash (float etaMin, float etaMax,
                           float phiMin,float phiMax) const
{
    return getHash(etaMin, etaMax, phiMin, phiMax);
}


std::vector<uint32_t> 
RPCgeometrySvc::getHash(float etaMin, float etaMax,
                              float phiMin,float phiMax) const
{
    if(phiMin < 0.) phiMin += 2*M_PI;
    if(phiMax < 0.) phiMax += 2*M_PI;
    
    if(etaMin > etaMax)
    {
        float tmp = etaMax;
	etaMax = etaMin;
	etaMin = tmp;
    }
    
    std::vector<uint32_t> vec;
    uint32_t hash = 0;
    
    DimensionMap::const_iterator it = m_regions.begin();
    for( ; it!=m_regions.end(); ++it)
    {
        bool eta = ( (etaMin>=(*it).etaMin && etaMin<=(*it).etaMax)||
                     (etaMax>=(*it).etaMin && etaMax<=(*it).etaMax)||
		     (etaMin<=(*it).etaMin && etaMax>=(*it).etaMax)  )? true : false;
    
    
        bool phi = false;
        if(phiMin<0.) phiMin += 2*M_PI;
        if(phiMax<0.) phiMax += 2*M_PI;
        
        if((*it).phiMin > (*it).phiMax)
        {
	    if(phiMax > phiMin)
	    {
	        phi = ( (phiMin>=(*it).phiMin && phiMin<=2.*M_PI)     ||
	                (phiMin>=0.           && phiMin<=(*it).phiMax)||
		        (phiMax>=(*it).phiMin && phiMax<=2.*M_PI)     ||
                        (phiMax>=0.           && phiMax<=(*it).phiMax) )? true : false;
            }
	    else
	    {
	        phi = ( (phiMin>=(*it).phiMin && phiMin<=2.*M_PI)      ||
	                (phiMin>=0.           && phiMin<=(*it).phiMax) ||
		        (phiMax>=(*it).phiMin && phiMax<=2.*M_PI)      ||
                        (phiMax>=0.           && phiMax<=(*it).phiMax) ||
		        (phiMin<=(*it).phiMin && phiMax>=(*it).phiMax)   )? true : false;
	    }
        } else
        {
            if(phiMax > phiMin)
	    {
                phi = ( (phiMin>=(*it).phiMin && phiMin<=(*it).phiMax)||
                        (phiMax>=(*it).phiMin && phiMax<=(*it).phiMax)||
		        (phiMin<=(*it).phiMin && phiMax>=(*it).phiMax) )? true : false;
            }
	    else
	    {
	        phi = ( (phiMin>=(*it).phiMin && phiMin<=(*it).phiMax)||
                        (phiMax>=(*it).phiMin && phiMax<=(*it).phiMax)||
		        (phiMin<=(*it).phiMin && 2*M_PI>=(*it).phiMax)||
		        (    0.<=(*it).phiMin && phiMax>=(*it).phiMax)   )? true : false;
	    }
        }
    
        if (eta && phi) vec.push_back(hash);
        ++hash;
    }
    
    return vec;
}


StatusCode
RPCgeometrySvc::updateAMDC(IOVSVC_CALLBACK_ARGS)
{   
    m_alignment_updated = true;
    StatusCode sc = buildRPCGeometry();
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
	    MsgStream log(messageService(), name());
            log << MSG::INFO << "Postpone RPCgeometry creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }
    return StatusCode::SUCCESS;
}


StatusCode RPCgeometrySvc::initFromCablingSvc(IOVSVC_CALLBACK_ARGS)
{   
    m_cablingInitFromDB = true;
    
    StatusCode sc = buildRPCGeometry();
    if ( sc.isFailure() ) {
        if( sc.isRecoverable() ){
	    MsgStream log(messageService(), name());
            log << MSG::INFO << "Postpone RPCgeometry creation " << endreq;
            return StatusCode::SUCCESS;
        }
        return sc;
    }
    return StatusCode::SUCCESS;
}

#endif





