/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL1_STANDALONE

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "RPCcablingSim/RPCcablingSimSvc.h"
#include "RPCcablingInterface/RDOindex.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include "MuonCablingTools/OutputDef.h"

#include "PathResolver/PathResolver.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <set>

using namespace RPCcablingSim;



RPCcablingSimSvc::RPCcablingSimSvc(const std::string& name,ISvcLocator* sl) :
    Service( name, sl )  
{
    declareProperty( "ConfFileName", m_conf_filename="LVL1conf.data",
        "Name of the configuration file to be loaded" );
    declareProperty( "CorrFileName", m_corr_filename="LVL1conf.corr",
        "Name of the correction file to be loaded" );
    declareProperty( "HackFor1031", m_1031_conf=false,
        "hack for the trigger menu at 10^31" );
    m_padHashIdHelper = 0;
    
}

// queryInterface 
StatusCode RPCcablingSimSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if( IRPCcablingSvc::interfaceID().versionMatch(riid) ) 
    { 
        *ppvIF = (IRPCcablingSvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }

    addRef();
    return StatusCode::SUCCESS;
} 

StatusCode RPCcablingSimSvc::initialize() 
{
    StatusCode sc;

    MsgStream mlog(messageService(), name());
    
    mlog << MSG::INFO << "Initializing " << name() << " - package version " 
	 << PACKAGE_VERSION << endreq;

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;

    // get the pointer to the MuonIdHelpers
    StoreGateSvc * detStore;
    StatusCode status = service("DetectorStore",detStore);
    if (status.isFailure()) {
        mlog << MSG::FATAL << "DetectorStore service not found !" << endreq;  
    } else {
        // Get the RPC id helper from the detector store
        const DataHandle<RpcIdHelper> rpcHelper;
        status = detStore->retrieve(rpcHelper, "RPCIDHELPER");
        if (status.isFailure()) {
            mlog << MSG::FATAL << "Could not get RpcIdHelper !" << endreq;
	    return StatusCode::FAILURE;
        } else {
            mlog << MSG::DEBUG << " Found the RpcIdHelper. " << endreq;
            RDOindex::setRpcIdHelper(rpcHelper);
	    m_pRpcIdHelper = rpcHelper;
        }
    }    
    
    
    
    // implement the search of LVL1conf.data trought the pathresolver utility.
    std::string conf_filename;
    conf_filename = PathResolver::find_file (m_conf_filename, "DATAPATH");
    if (conf_filename.empty())
    {
       mlog << MSG::ERROR 
            << "Cannot locate " << m_conf_filename 
	    << " from ${DATAPATH}" << endreq;
       return StatusCode::FAILURE;
    }

    std::string corr_filename;
    corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
    if (corr_filename.empty())
    {
       mlog << MSG::ERROR 
            << "Cannot locate " << m_corr_filename 
	    << " from ${DATAPATH}" << endreq;
       return StatusCode::FAILURE;
    }


    const CablingRPCBase* cabling = RPCcablingSim::CablingRPC::
                             instance(conf_filename,corr_filename,m_1031_conf);

    if ( !cabling->isLoaded() ) 
    {
        mlog << MSG::ERROR <<   "RPC cabling model is not loaded!"  << std::endl 
            << endreq;
        return StatusCode::FAILURE;
    } else {
        mlog << MSG::INFO << *cabling << endreq;
    }
    
    
    // build fast map between offlineID and PAD online ID
    if (!buildOfflineOnlineMap())
    {
        mlog << MSG::ERROR << " Cannot build the map between offlineID and " 
	     << "PAD online ID!" << endreq;
        return StatusCode::FAILURE;
    }

    mlog << MSG::DEBUG << "DUMP THE FULL RPC ROBID LIST: " 
         << m_fullListOfRobIds.size() << endreq;
    if(mlog.level() <= MSG::DEBUG) 
    {
      for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
      {
	mlog << MSG::DEBUG << "RobId=" << MSG::hex 
	     << m_fullListOfRobIds[i] << MSG::dec << endreq; 
      }
    }
    /*
    // Incident Svc
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
    sc = incidentSvc.retrieve();
    if (sc.isFailure()) {
	//log << MSG::ERROR << "IncidentSvc not found" << endreq;
	mlog<<MSG::ERROR<< "IncidentSvc not found" << endreq;
	return StatusCode::FAILURE;
    } 
    incidentSvc->addListener(this, "StartRun");   
    */
    
    // here get and provide the HashFunction
    if (m_padHashIdHelper) delete m_padHashIdHelper;
    m_padHashIdHelper = new RpcPadIdHash();	
    
   
    mlog << MSG::DEBUG << "DUMP THE OFFLINE ID MAP OF THE PADs: " << endreq;
    for(int side=0;side<2;++side)
        for(int Sector=0;Sector<32;++Sector)
            for(int Pad=0;Pad<10;++Pad)
            {
                Identifier ID;
                if(giveOffflineID(side,Sector,Pad,ID))
                mlog << MSG::DEBUG <<  "Side="   << side 
                                   << " Sector=" << Sector
                                   << " Pad="    << Pad << ": identifier is "
                                   << m_pRpcIdHelper->show_to_string(ID) 
                                   << endreq;
            }

    return StatusCode::SUCCESS;
}

/*
void RPCcablingSimSvc::handle(const Incident& inc)
{
    MsgStream mlog(messageService(), name());
    if (inc.type()=="BeginRun")
    {
	mlog<<MSG::INFO<<"Getting at BeginRun incident the RpcPadIdHashFunction"<<endreq;
	// here get and provide the HashFunction
	if (m_padHashIdHelper) delete m_padHashIdHelper;
	m_padHashIdHelper = new RpcPadIdHash();	
    }
}
*/

StatusCode RPCcablingSimSvc::finalize() 
{
    RPCcablingSim::CablingRPC::delete_instance();
    return StatusCode::SUCCESS;
} 

// Implement the specifics ... 

const CablingRPCBase::RDOmap& 
RPCcablingSimSvc::give_RDOs() const
{    
    return RPCcablingSim::CablingRPC::s_instance->give_RDOs ();
}

const CMAparameters::CMAlist
RPCcablingSimSvc::give_CMAs(const int logic_sector,const ViewType side,
                     const int station,const int cabling_code) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_CMAs
                                 (logic_sector,side,station,cabling_code);
}


bool 
RPCcablingSimSvc::give_global_address(unsigned int code,int& global_strip_address,
int& global_connector_address,int& low_eta_strips,int& hi_eta_strips) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_global_address(code,
                                                       global_strip_address,
                                                       global_connector_address,
                                                       low_eta_strips,
                                                       hi_eta_strips);
}

bool 
RPCcablingSimSvc::give_RoI_borders (unsigned short int SubsystemId,
                                 unsigned short int SectorId,
			         unsigned short int RoIId,
			         unsigned int& EtaLowBorder,
				 unsigned int& EtaHighBorder,
				 unsigned int& PhiLowBorder,
				 unsigned int& PhiHighBorder) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_RoI_borders(SubsystemId,SectorId,RoIId,
                                                   EtaLowBorder,EtaHighBorder,
					           PhiLowBorder,PhiHighBorder);
}

bool
RPCcablingSimSvc::give_LowPt_borders (unsigned short int SubsystemId,
                                   unsigned short int SectorId,
			           unsigned short int RoIId,
			           unsigned int& EtaLowBorder,
				   unsigned int& EtaHighBorder,
				   unsigned int& PhiLowBorder,
				   unsigned int& PhiHighBorder) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_LowPt_borders(SubsystemId,SectorId,
                                                   RoIId,
                                                   EtaLowBorder,EtaHighBorder,
					           PhiLowBorder,PhiHighBorder);
}

bool
RPCcablingSimSvc::give_HighPt_borders (unsigned short int SubsystemId,
                                    unsigned short int SectorId,
			            unsigned short int RoIId,
			            unsigned int& EtaLowBorder,
				    unsigned int& EtaHighBorder,
				    unsigned int& PhiLowBorder,
				    unsigned int& PhiHighBorder) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_HighPt_borders(SubsystemId,SectorId,
                                                   RoIId,
                                                   EtaLowBorder,EtaHighBorder,
					           PhiLowBorder,PhiHighBorder);
}

std::list<unsigned int> 
RPCcablingSimSvc::give_strip_code (unsigned short int SubsystemId,
                                unsigned short int SectorId,
			        unsigned short int PADId,
				unsigned short int CMAId,
				unsigned short ijk,
				unsigned short int Channel) const
{
    return RPCcablingSim::CablingRPC::s_instance->give_strip_code(SubsystemId,SectorId,PADId,
				                   CMAId,ijk,Channel);
}


std::list<Identifier> 
RPCcablingSimSvc::give_strip_id   (unsigned short int SubsystemId,
                                unsigned short int SectorId,
			        unsigned short int PADId,
				unsigned short int CMAId,
				unsigned short ijk,
				unsigned short int Channel) const
{
    std::list<Identifier> id;
    
    std::list<RPCofflineId> offlineIds = RPCcablingSim::CablingRPC::s_instance->
                    give_strip_id(SubsystemId,SectorId,PADId,CMAId,ijk,Channel);

    std::list<RPCofflineId>::const_iterator it = offlineIds.begin();
    while(it != offlineIds.end())
    {	
        Identifier rpcId = m_pRpcIdHelper->channelID((*it).stationName,
	                                             (*it).stationEta,
						     (*it).stationPhi,
						     (*it).doubletR,
						     (*it).doubletZ,
						     (*it).doubletPhi,
						     (*it).gasGap,
						     (*it).measuresPhi,
						     (*it).strip);
        id.push_back(rpcId);
	++it;
    }
    
    return id;
}


void
RPCcablingSimSvc::printType(int type,int station,std::string element,
                             bool detail) const
{   
    MsgStream log(msgSvc(), name());
    
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[500000];
    for (int i=0;i<500000;++i) buffer[i] = '\0';
    __osstream display(buffer,500000);
#endif
    
    log << MSG::DEBUG << name() << " is demanding a cabling printout" <<endreq;
    
    RPCcablingSim::CablingRPC::s_instance->PrintType(display,type,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;
}


void
RPCcablingSimSvc::printSector(int sector,int station,std::string element,
                               bool detail) const
{
    MsgStream log(msgSvc(), name());
    
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[500000];
    for (int i=0;i<500000;++i) buffer[i] = '\0';
    __osstream display(buffer,500000);
#endif
 
    log << MSG::DEBUG << name() << " is demanding a cabling printout" <<endreq;

    RPCcablingSim::CablingRPC::s_instance->PrintSector(display,sector,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;
}


const CablingRPCBase*
RPCcablingSimSvc::getRPCCabling() const
{
    //std::cout << "into getRPCCabling of RPCcablingSim" << std::endl;
    return RPCcablingSim::CablingRPC::instance();
}


bool
RPCcablingSimSvc::buildOfflineOnlineMap()
{    
    //MsgStream log(messageService(), name());
    //log << MSG::DEBUG << name() << ": Start building fast offline/online map" 
    //    << endreq;
    
    m_RDOmap.clear();
    m_HashVec.clear();

    std::set< uint32_t > ROBid;
    

    const CablingRPCBase* cab = RPCcablingSim::CablingRPC::instance();
    const RPCcablingSim::CablingRPC::RDOmap& map = cab->give_RDOs();
    RPCcablingSim::CablingRPC::RDOmap::const_iterator it = map.begin();
    // reserve enough space in the hash-vector
    m_HashVec.reserve(map.size());
    while(it != map.end())
    {
        // get pointer to RDOindex class
	const RDOindex* pRDOindex = &((*it).second);
	
	m_HashVec.push_back(pRDOindex);
	if( m_HashVec.size() != pRDOindex->hash()+1 ) return false;
	
	const unsigned short int rob_id = pRDOindex->ROBid();
	const unsigned short int sub_id = pRDOindex->side();
        const unsigned short int sec_id = pRDOindex->SLid();
        const unsigned short int pad_id = pRDOindex->PADid();
	
	uint32_t ROB_ID = (sub_id << 16) | rob_id;
	
	ROBid.insert(ROB_ID); 
	

        // get the set of offlineID identifing the RDO
	//int name = 0;
	//int Jtype = 0;
	//int stationEta = 0;
	//int stationPhi = 0;
	//int DoubletR = 0;
	//int DoubletZ = 0;
	//int DoubletP = 0;
	//int GasGap = 0;
	//int MeasuresPhi = 0;
	//int Strip = 0;

/*	
	m_pRpcGeo->give_strip_index(pRDOindex->lvl1_code(),
	                            Jtype,stationEta,stationPhi,DoubletR,
                                    DoubletZ,DoubletP,GasGap,MeasuresPhi,
                                    Strip);
	name = Jtype - 1;

        Identifier id = m_pRpcIdHelper->channelID(name,stationEta,stationPhi,
	                  DoubletR,DoubletZ,DoubletP,GasGap,MeasuresPhi,Strip);
*/
        Identifier id;
        pRDOindex->pad_identifier( id );

        // build the map
        std::pair < OfflineOnlineMap::iterator, bool> ins = 
                    m_RDOmap.insert(OfflineOnlineMap::value_type(id,pRDOindex));
        
	if(!ins.second) return false;
	
        
	//build the offline_id vector
	m_offline_id[((sub_id==0x65)? 1:0)][sec_id][pad_id] = id;
	
	//increase the iterator            
        ++it;
    }
    
    // log << MSG::DEBUG << m_RDOmap.size() << endreq; 
    
    
    // reserve enough space
    m_fullListOfRobIds.reserve(ROBid.size());

    for(std::set< uint32_t >::const_iterator 
	  rob_itr=ROBid.begin(),
	  rob_end=ROBid.end();
	rob_itr != rob_end;
	++rob_itr) {
      m_fullListOfRobIds.push_back(*rob_itr);
    }
    
    return true;
}

bool
RPCcablingSimSvc::giveOnlineID(const Identifier compactID,
                             unsigned short int& ROBid,
                             unsigned short int& RODid,
                             unsigned short int& side,
                             unsigned short int& SLid,
                             unsigned short int& RXid,
                             unsigned short int& PADid) const

{
    OfflineOnlineMap::const_iterator it = m_RDOmap.find(compactID);
    if (it == m_RDOmap.end()) return false;
    
    const RDOindex* index = (*m_RDOmap.find(compactID)).second;
    
    ROBid = index->ROBid();
    RODid = index->RODid();
    side  = index->side();
    SLid  = index->SLid();
    RXid  = index->RXid();
    PADid = index->PADid();
    
    return true;
}

bool
RPCcablingSimSvc::giveOnlineID(const unsigned int hashID,
                             unsigned short int& ROBid,
                             unsigned short int& RODid,
                             unsigned short int& side,
                             unsigned short int& SLid,
                             unsigned short int& RXid,
                             unsigned short int& PADid) const

{
    if ( hashID >= m_HashVec.size() ) return false;
    
    const RDOindex* index = m_HashVec[hashID];
    
    ROBid = index->ROBid();
    RODid = index->RODid();
    side  = index->side();
    SLid  = index->SLid();
    RXid  = index->RXid();
    PADid = index->PADid();
    
    return true;
}

std::vector<uint32_t> 
RPCcablingSimSvc::giveFullListOfRobIds() const
{
    return m_fullListOfRobIds;
}

bool
RPCcablingSimSvc::giveOffflineID(unsigned short int Side,
                                 unsigned short int Sector,
			         unsigned short int PADID,
			         Identifier& ID) const
{
    if (Side>=2) return false;
    if (Sector>=32) return false;
    if (PADID >=10) return false;

    ID = m_offline_id[Side][Sector][PADID];

    if (ID == 0xFFFFFFFF) return false;

    return true;
}

//method to be used by RPCgeometry
std::vector<const RDOindex*> RPCcablingSimSvc::GiveHashVec() const
{
    return m_HashVec;
}

#endif
