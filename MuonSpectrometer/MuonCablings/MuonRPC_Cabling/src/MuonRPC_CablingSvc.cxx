/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "MuonRPC_Cabling/CablingRPC.h"
#include "MuonCondInterface/IRPCCablingDbTool.h"
#include "MuonCondInterface/IRPCTriggerDbTool.h"

#include "RPCcablingInterface/RDOindex.h"
#include "MuonCablingTools/OutputDef.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"

#ifdef RPCCAB_MONCPU
#include "GeoModelKernel/GeoPerfUtils.h"
#endif

#include <set>
#include <string>

using namespace MuonRPC_Cabling;

const std::string* RPCCorrMap = 0;
const std::string* RPCConfMap = 0;

MuonRPC_CablingSvc::MuonRPC_CablingSvc(const std::string& name, ISvcLocator* pSvcLocator) :
    AthService(name, pSvcLocator),
    m_padHashIdHelper(0),
    m_condDataTool("RPCCablingDbTool"),
    m_condTriggerTool("RPCTriggerDbTool")
{
    declareProperty( "ConfFileName", m_conf_filename="LVL1confAtlas.data" );
    declareProperty( "CorrFileName", m_corr_filename="LVL1confAtlas.corr" );    
    declareProperty( "CosmicConfiguration", m_cosmic_configuration=false );
    declareProperty( "DatabaseRepository",  m_database_repository="ATLAS.data" );

    //L. Bellagamba 04/08/2009 -> Cabling maps from Cool
    declareProperty( "RPCMapfromCool",m_RPCMapfromCool=true);
    declareProperty( "TheRpcCablingDbTool",  m_condDataTool,      "a tool reading RPC cabling maps from COOL");
    //L. Bellagamba 15/04/2010 -> Trigger roads from Cool
    declareProperty( "RPCTriggerRoadsfromCool",m_RPCTriggerRoadsfromCool=false);
    declareProperty( "TheRpcTriggerDbTool",  m_condTriggerTool,      "a tool reading RPC triiger roads from COOL");
}

MuonRPC_CablingSvc::~MuonRPC_CablingSvc()
{
}

StatusCode MuonRPC_CablingSvc::initialize()
{
   
    msg(MSG::INFO) << "Initializing " << name() << " - package version " 
		   << PACKAGE_VERSION << endreq;

    // get the pointer to the MuonIdHelpers
    StoreGateSvc * detStore;
    StatusCode status = service("DetectorStore",detStore);
    if (status.isFailure()) {
	msg(MSG::FATAL) << "DetectorStore service not found !" << endreq;  
    } else {
	// Get the RPC id helper from the detector store
	//    const DataHandle<RpcIdHelper> rpcHelper;
	const RpcIdHelper* rpcHelper;
	status = detStore->retrieve(rpcHelper, "RPCIDHELPER");
	if (status.isFailure()) {
	    msg(MSG::FATAL) << "Could not get RpcIdHelper !" << endreq;
	    return StatusCode::FAILURE;
	} else {
	    msg(MSG::DEBUG) << "Found the RpcIdHelper. " << endreq;
	    RDOindex::setRpcIdHelper(rpcHelper);
	    m_pRpcIdHelper = rpcHelper;
	}
    }    
    
    bool tryRecoveringByReadingFromFile = false;
    bool tryRecoveringByReadingTrigRoadsFromFile = false;


    if(!m_RPCMapfromCool && m_RPCTriggerRoadsfromCool) {
      msg(MSG::ERROR)<< " !!! RPC cabling map from files and trigger roads from COOL - option not implemented !!!"<< endreq;
      return StatusCode::FAILURE;
    }    

    if(m_RPCMapfromCool)
    {
	StatusCode sc = m_condDataTool.retrieve();
	if ( sc.isFailure() )
	{
	    //log<<MSG::ERROR<<"Could not retrieve MuonAlignmentDbTool"<<endreq;
	    msg(MSG::ERROR) << "Could not retrieve RPCCablingDbTool" << endreq;
	}
	else
	{
	    //log<<MSG::INFO<<"MuonAlignmentDbTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endreq;
            msg(MSG::INFO)<<"RPCCablingDbTool retrieved with statusCode = "<<sc<<" with handle "<<m_condDataTool<<endreq;
	}



	//std::vector<std::string> folderNames = m_coolReadingTool->mapFolderNames();
	std::vector<std::string> folderNames;
	folderNames.push_back((m_condDataTool)->mapConfFolderName());
	folderNames.push_back((m_condDataTool)->mapCorrFolderName());
                                                               

	msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endreq;
	// bool aFolderFound = false;
	short ic=0;
	for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	    ++ic;
	    msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	    if (detStore->contains<CondAttrListCollection>(*ifld)) {
		// aFolderFound=true;
		msg(MSG::INFO)<<"     found in the DetStore"<<endreq;
		const DataHandle<CondAttrListCollection> mapDataClob;
		if (detStore->regFcn(&IRPCcablingSvc::initMappingModel,
				     dynamic_cast<IRPCcablingSvc *>(this),
				     mapDataClob,
				     *ifld)!=StatusCode::SUCCESS)
		{
		    msg(MSG::WARNING)<<"Unable to register call back for initMappingModel against folder <"<<(*ifld)<<">";
		    //return StatusCode::FAILURE;
		}
		else msg(MSG::INFO)<<"initMappingModel registered for call-back against folder <"<<(*ifld)<<">"<<endreq;
	    }
	    else
	    {	
		msg(MSG::WARNING)<<"Folder "<<(*ifld)
				 <<" NOT found in the DetStore --- failing to init ???"<<endreq;
		tryRecoveringByReadingFromFile = true;
		break;
	    }
	}
    }

    if ((!m_RPCMapfromCool) || tryRecoveringByReadingFromFile) 
    {
	// init maps init time 
	int I=0;
	std::list<std::string> keys;
	if (initMappingModel(I,keys).isFailure())
	{
	    return StatusCode::FAILURE;
	}	
    }    

    if(m_RPCTriggerRoadsfromCool)
    {
	StatusCode sc = m_condTriggerTool.retrieve();
	if ( sc.isFailure() )
	{
	    msg(MSG::ERROR) << "Could not retrieve RPCTriggerDbTool" << endreq;
	}
	else
	{
	    msg(MSG::INFO)<<"RPCTriggerDbTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condTriggerTool<<endreq;
	}


	std::vector<std::string> trigfolderNames;
	trigfolderNames.push_back((m_condTriggerTool)->EtaTableFolderName());
	trigfolderNames.push_back((m_condTriggerTool)->PhiTableFolderName());
	msg(MSG::INFO)<<"Register call-back  against "<<trigfolderNames.size()<<" folders listed below "<<endreq;
	// bool aFolderFound = false;
	short ic=0;
	for (std::vector<std::string>::const_iterator ifld =trigfolderNames.begin(); ifld!=trigfolderNames.end(); ++ifld )
	{
	    ++ic;
	    msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	    if (detStore->contains<CondAttrListCollection>(*ifld)) {
		// aFolderFound=true;
		msg(MSG::INFO)<<"     found in the DetStore"<<endreq;
		const DataHandle<CondAttrListCollection> TrigRoadClob;
       		if (detStore->regFcn(&ICallBackMuonRPC_Cabling::initTrigRoadsModel,
				     dynamic_cast<ICallBackMuonRPC_Cabling *>(this),
				     TrigRoadClob,
				     *ifld)!=StatusCode::SUCCESS)
		{
		    msg(MSG::WARNING)<<"Unable to register call back for initTrigRoadsModel against folder <"<<(*ifld)<<">";
		    //return StatusCode::FAILURE;
		}
		else msg(MSG::INFO)<<"initTrigRoadsModel registered for call-back against folder <"<<(*ifld)<<">"<<endreq;
	    }
	    else
	    {	
		msg(MSG::WARNING)<<"Folder "<<(*ifld)
				 <<" NOT found in the DetStore --- failing to init ???"<<endreq;
		tryRecoveringByReadingTrigRoadsFromFile = true;
		break;
	    }
	}
    }
    if((!m_RPCTriggerRoadsfromCool) || tryRecoveringByReadingTrigRoadsFromFile)
    {
	// init maps init time 
	int I=0;
	std::list<std::string> keys;
	if (initTrigRoadsModel(I,keys).isFailure())
	{
	    return StatusCode::FAILURE;
	}	
    }    
    
    return StatusCode::SUCCESS;
}

StatusCode MuonRPC_CablingSvc::finalize()
{

    CablingRPC::delete_instance();
    return StatusCode::SUCCESS;
}


StatusCode MuonRPC_CablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{
    msg(MSG::VERBOSE) << "queryInterface Start" << endreq;
    if(IRPCcablingSvc::interfaceID().versionMatch(riid) ) 
    {
        msg(MSG::VERBOSE) << "versionMatch=true" << endreq;
        *ppvIF = (IRPCcablingSvc*)(this);
    } else {
        msg(MSG::VERBOSE) << "cannot find the interface!" << endreq;
        return AthService::queryInterface(riid, ppvIF); 
      }
  msg(MSG::VERBOSE) << "queryInterface succesfull" << endreq;
  addRef();  // is this needed ?? yes it is ! 
  return StatusCode::SUCCESS;
}


// Implement the specifics ... 

const CablingRPCBase::RDOmap&
MuonRPC_CablingSvc::give_RDOs() const
{   
    return CablingRPC::s_instance->give_RDOs();  
}

const CMAparameters::CMAlist 
MuonRPC_CablingSvc::give_CMAs(const int logic_sector,const ViewType side,
                     const int station,const int cabling_code) const
{
     return CablingRPC::s_instance->give_CMAs
                                 (logic_sector,side,station,cabling_code);
}

bool 
MuonRPC_CablingSvc::give_global_address(unsigned int code,int& global_strip_address,
int& global_connector_address,int& low_eta_strips,int& hi_eta_strips) const
{
    return CablingRPC::s_instance->give_global_address(code,
                                                       global_strip_address,
                                                       global_connector_address,
                                                       low_eta_strips,
                                                       hi_eta_strips);
}
			    
bool MuonRPC_CablingSvc::give_RoI_borders (unsigned short int SubsystemId,
					   unsigned short int SectorId,
					   unsigned short int RoIId,
					   unsigned int& EtaLowBorder,
					   unsigned int& EtaHighBorder,
					   unsigned int& PhiLowBorder,
					   unsigned int& PhiHighBorder) const
{
    return CablingRPC::s_instance->give_RoI_borders(SubsystemId,SectorId,RoIId,
                                                   EtaLowBorder,EtaHighBorder,
					           PhiLowBorder,PhiHighBorder);

}

bool MuonRPC_CablingSvc::give_RoI_borders_id (unsigned short int SubsystemId,
					   unsigned short int SectorId,
					   unsigned short int RoIId,
					   Identifier& EtaLowBorder_id,
					   Identifier& EtaHighBorder_id,
					   Identifier& PhiLowBorder_id,
					   Identifier& PhiHighBorder_id) const
{
  unsigned int EtaLowBorder=0;
  unsigned int EtaHighBorder=0;
  unsigned int PhiLowBorder=0;
  unsigned int PhiHighBorder=0;  
  bool ok = CablingRPC::s_instance->give_RoI_borders(SubsystemId,SectorId,RoIId,
						     EtaLowBorder,EtaHighBorder,
						     PhiLowBorder,PhiHighBorder);  
  if (ok){
    EtaLowBorder_id  =  this->strip_OffId_fromCode( EtaLowBorder);
    EtaHighBorder_id =  this->strip_OffId_fromCode( EtaHighBorder);
    PhiLowBorder_id  =  this->strip_OffId_fromCode( PhiLowBorder);
    PhiHighBorder_id =  this->strip_OffId_fromCode( PhiHighBorder);
  }
  return ok;
}


bool MuonRPC_CablingSvc::give_LowPt_borders (unsigned short int SubsystemId,
                                             unsigned short int SectorId,
                                             unsigned short int RoIId,
                                             unsigned int& EtaLowBorder,
                                             unsigned int& EtaHighBorder,
                                             unsigned int& PhiLowBorder,
                                             unsigned int& PhiHighBorder) const
{
    return CablingRPC::s_instance->give_LowPt_borders(SubsystemId,SectorId,
                                                      RoIId,
                                                      EtaLowBorder,EtaHighBorder,
                                                      PhiLowBorder,PhiHighBorder);
}


bool MuonRPC_CablingSvc::give_LowPt_borders_id (unsigned short int SubsystemId,
					   unsigned short int SectorId,
					   unsigned short int RoIId,
					   Identifier& EtaLowBorder_id,
					   Identifier& EtaHighBorder_id,
					   Identifier& PhiLowBorder_id,
					   Identifier& PhiHighBorder_id) const
{
  unsigned int EtaLowBorder=0;
  unsigned int EtaHighBorder=0;
  unsigned int PhiLowBorder=0;
  unsigned int PhiHighBorder=0;  
  bool ok = CablingRPC::s_instance->give_LowPt_borders(SubsystemId,SectorId,RoIId,
						     EtaLowBorder,EtaHighBorder,
						     PhiLowBorder,PhiHighBorder);  
  if (ok){
    EtaLowBorder_id  =  this->strip_OffId_fromCode( EtaLowBorder);
    EtaHighBorder_id =  this->strip_OffId_fromCode( EtaHighBorder);
    PhiLowBorder_id  =  this->strip_OffId_fromCode( PhiLowBorder);
    PhiHighBorder_id =  this->strip_OffId_fromCode( PhiHighBorder);
  }
  return ok;
}


bool MuonRPC_CablingSvc::give_HighPt_borders (unsigned short int SubsystemId,
                                              unsigned short int SectorId,
                                              unsigned short int RoIId,
                                              unsigned int& EtaLowBorder,
                                              unsigned int& EtaHighBorder,
                                              unsigned int& PhiLowBorder,
                                              unsigned int& PhiHighBorder) const
{
    return CablingRPC::s_instance->give_HighPt_borders(SubsystemId,SectorId,
                                                       RoIId,
                                                       EtaLowBorder,EtaHighBorder,
                                                       PhiLowBorder,PhiHighBorder);
}


bool MuonRPC_CablingSvc::give_HighPt_borders_id (unsigned short int SubsystemId,
					   unsigned short int SectorId,
					   unsigned short int RoIId,
					   Identifier& EtaLowBorder_id,
					   Identifier& EtaHighBorder_id,
					   Identifier& PhiLowBorder_id,
					   Identifier& PhiHighBorder_id) const
{
  unsigned int EtaLowBorder=0;
  unsigned int EtaHighBorder=0;
  unsigned int PhiLowBorder=0;
  unsigned int PhiHighBorder=0;  
  bool ok = CablingRPC::s_instance->give_HighPt_borders(SubsystemId,SectorId,RoIId,
						     EtaLowBorder,EtaHighBorder,
						     PhiLowBorder,PhiHighBorder);  
  if (ok){
    EtaLowBorder_id  =  this->strip_OffId_fromCode( EtaLowBorder);
    EtaHighBorder_id =  this->strip_OffId_fromCode( EtaHighBorder);
    PhiLowBorder_id  =  this->strip_OffId_fromCode( PhiLowBorder);
    PhiHighBorder_id =  this->strip_OffId_fromCode( PhiHighBorder);
  }
  return ok;
}


std::list<unsigned int> MuonRPC_CablingSvc::give_strip_code(unsigned short int SubsystemId,
                                                            unsigned short int SectorId,
                                                            unsigned short int PADId,
                                                            unsigned short int CMAId,
                                                            unsigned short ijk,
                                                            unsigned short int Channel) const
{
    return CablingRPC::s_instance->give_strip_code(SubsystemId,SectorId,PADId,
				                   CMAId,ijk,Channel);
}

unsigned long int MuonRPC_CablingSvc::strip_code_fromOffId (std::string stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip) const
{
  //  return 0;
  return CablingRPC::s_instance->strip_code_fromOffId(stationName,  stationEta, stationPhi, doubletR, doubletZ, 
						      doubletPhi, gasGap, measuresPhi, strip);
}

Identifier MuonRPC_CablingSvc::strip_OffId_fromCode (unsigned long int strip_code) const 
{
  RPCofflineId rpc_strip  = CablingRPC::s_instance->strip_id_fromCode (strip_code);
  Identifier rpcId = m_pRpcIdHelper->channelID(rpc_strip.stationName,
					       rpc_strip.stationEta,
					       rpc_strip.stationPhi,
					       rpc_strip.doubletR,
					       rpc_strip.doubletZ,
					       rpc_strip.doubletPhi,
					       rpc_strip.gasGap,
					       rpc_strip.measuresPhi,
					       rpc_strip.strip);
  return rpcId;
}


std::list<Identifier> MuonRPC_CablingSvc::give_strip_id(unsigned short int SubsystemId,
                                                        unsigned short int SectorId,
                                                        unsigned short int PADId,
                                                        unsigned short int CMAId,
                                                        unsigned short ijk,
                                                        unsigned short int Channel) const
{    std::list<Identifier> id;
    
    
    std::list<RPCofflineId> offlineIds = CablingRPC::s_instance->
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
MuonRPC_CablingSvc::printType(int type,int station,std::string element,bool detail) const
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
    
    CablingRPC::s_instance->PrintType(display,type,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;

   return;
    }

void
MuonRPC_CablingSvc::printSector(int sector,int station,std::string element,bool detail) const
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

    CablingRPC::s_instance->PrintSector(display,sector,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;

   return;
}



const CablingRPCBase*
MuonRPC_CablingSvc::getRPCCabling() const
{
    return CablingRPC::instance();
    }


bool
MuonRPC_CablingSvc::buildOfflineOnlineMap()
{

    msg(MSG::INFO) << "buildOfflineOnlineMap"<<endreq;
    m_RDOmap.clear();
    m_HashVec.clear();
    
    std::set< uint32_t > ROBid;

    const CablingRPCBase* cab = CablingRPC::instance();
    msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cab<<">"<<endreq;
    const CablingRPC::RDOmap& map = cab->give_RDOs();
    msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cab<<"> hold an RDOmap of size "<<map.size()<<endreq;
    CablingRPC::RDOmap::const_iterator it = map.begin();
    unsigned int nEntries=0;
    // reserve enough space in the hash-vector
    m_HashVec.reserve(map.size());
    while(it != map.end())
    {
        // get pointer to RDOindex class
	const RDOindex* pRDOindex = &((*it).second);
	m_HashVec.push_back(pRDOindex);
	if( m_HashVec.size() != pRDOindex->hash()+1 ) return false;
	++nEntries;
        msg(MSG::DEBUG)<<"RDO Hash Vect new entry  "<<nEntries<<" vector size is "<<m_HashVec.size()<<" this pRDOindex->hash() "<<pRDOindex->hash()<<endreq;
	
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

//
//	m_pRpcGeo->give_strip_index(pRDOindex->lvl1_code(),
//	                            Jtype,stationEta,stationPhi,DoubletR,
//                                    DoubletZ,DoubletP,GasGap,MeasuresPhi,
//                                    Strip);
//	name = Jtype - 1;
//
//        Identifier id = m_pRpcIdHelper->channelID(name,stationEta,stationPhi,
//	                  DoubletR,DoubletZ,DoubletP,GasGap,MeasuresPhi,Strip);
//
        Identifier id;
        pRDOindex->pad_identifier( id );

        // build the map
        std::pair < OfflineOnlineMap::iterator, bool> ins = 
                    m_RDOmap.insert(OfflineOnlineMap::value_type(id,pRDOindex));
        msg(MSG::DEBUG)<<"OfflineOnlineMap new entry: value  "<<m_pRpcIdHelper->show_to_string(id)
                       <<" hash of the RDOindex (key) = "<<pRDOindex->hash()<<endreq;
                    
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


    
//give access to onlineID from compact OfflineID (return == true if OK)
bool MuonRPC_CablingSvc::giveOnlineID(const Identifier compactID,
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

    
bool MuonRPC_CablingSvc::giveOnlineID(const unsigned int hashID,
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

    
bool MuonRPC_CablingSvc::giveOffflineID(unsigned short int Side,
                                        unsigned short int Sector,
                                        unsigned short int PADID,
					Identifier& ID) const
{
    if (Side>=2) return false;
    if (Sector>=32) return false;
    if (PADID>=10) return false;

    ID = m_offline_id[Side][Sector][PADID];
    if( ID == 0xFFFFFFFF ) return false; 

    return true;
}

std::vector<uint32_t> MuonRPC_CablingSvc::giveFullListOfRobIds() const
{
    return m_fullListOfRobIds;
}

// // Dummy methods because we inherit from virtual IRPCcablingSvc
// std::vector<uint32_t> MuonRPC_CablingSvc::getPadHash (float a,float b,float c,float d) const
// {
//     msg(MSG::ERROR) << "Method getPadHash has been removed" << endreq;
//     std::vector<uint32_t> dummy;
//     return dummy;
// }

// std::vector<uint32_t> MuonRPC_CablingSvc::getRobId (float a,float b,float c,float d) const
// {
//     msg(MSG::ERROR) << "Method getRobId has been removed" << endreq;
//     std::vector<uint32_t> dummy;
//     return dummy;
// }


StatusCode MuonRPC_CablingSvc::initMappingModel(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
    msg(MSG::INFO)<<"initMappingModel has been called"<<endreq;
    msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endreq;

#ifdef RPCCAB_MONCPU
    std::ofstream cpuMemStats;
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;
    cpuMemStats.open("MuonRPC_cabling_InitMappingModel_Stats");
    mem = GeoPerfUtils::getMem();
    cpu = float(GeoPerfUtils::getCpu()/100.);
    cpuMemStats <<"entry point: \t SZ= " <<mem << " Kb \t Time = " << cpu << " seconds " << std::endl;
#endif

    bool tryRecoveringByReadingFromFile = false;
    bool postponeCabInit = false;
    // retrive (if existing) or create empty CablinRPC singleton - cast to CablingRPC
    msg(MSG::INFO)<<"Retrieving cabling singleton; to create an empty one or to get the existing one"<<endreq;
    CablingRPC* cabling = dynamic_cast<CablingRPC*>(CablingRPC::instance());
    msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
    // clear cache in case this is not first initialization on the job
    cabling->clearCache();
    cabling->ClearPtoCablingMap();
    msg(MSG::INFO)<<"Cabling singleton cache has been cleared"<<endreq;
    //cabling->delete_instance();
    //msg(MSG::INFO)<<"Cabling singleton deleted"<<endreq;

#ifdef RPCCAB_MONCPU
    umem = GeoPerfUtils::getMem();
    ucpu = float(GeoPerfUtils::getCpu()/100.);
    cpuMemStats <<"CablingRPC created \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;
#endif

    std::string roads_dir;
    roads_dir = PathResolver::find_directory (m_database_repository, "DATAPATH");
    if(!m_RPCTriggerRoadsfromCool){
      if (roads_dir.empty())
      {
        msg(MSG::INFO) 
            << "Cannot locate " << m_database_repository 
            << " default trigger configuratyion is loaded" << endreq;
        roads_dir=".";
      } else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from " << roads_dir << endreq;
      }
    }
    else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from COOL" << endreq;
    }
    
    if(m_RPCMapfromCool)
    {
	    
	StatusCode sc = m_condDataTool->loadParameters(I, keys);
	if (sc.isFailure())
	{
	    msg(MSG::WARNING)<<"Reading Cabling maps from COOL failed; try to read from file"<<endreq;
	    tryRecoveringByReadingFromFile = true;
	}

#ifdef RPCCAB_MONCPU
        umem = GeoPerfUtils::getMem();
        ucpu = float(GeoPerfUtils::getCpu()/100.);
        cpuMemStats <<"maps loaded from COOL \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
        mem = umem;
        cpu = ucpu;
#endif

	RPCCorrMap = m_condDataTool->GetStringRPCCorrMap();
	RPCConfMap = m_condDataTool->GetStringRPCConfMap();

        msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endreq;
 	ATH_MSG_DEBUG("Dump of the configuration RPC Cabling Map"); 
        ATH_MSG_DEBUG("\n"+(*RPCConfMap));	
 	ATH_MSG_DEBUG("End of Dump of the configuration RPC Cabling Map"); 
	ATH_MSG_DEBUG("Dump of the correction RPC Cabling Map");
        ATH_MSG_DEBUG("\n"+(*RPCCorrMap));
 	ATH_MSG_DEBUG("End of Dump of the configuration RPC Cabling Map"); 

        if((m_RPCTriggerRoadsfromCool && cabling->GetPtoTrigRoads() != 0) ||
           !m_RPCTriggerRoadsfromCool ){
  	  // new CablingRPC instance using the maps from Cool
          msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endreq;
	  cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(RPCConfMap,RPCCorrMap,roads_dir, m_cosmic_configuration));
 	  msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
	  if (!cabling->isLoaded())
	  {
	    // this can happen is someone has already built an empty Cabling Singleton 
	    // before initMappingModel is called
	    cabling->initMapsFromCOOL();

#ifdef RPCCAB_MONCPU
            umem = GeoPerfUtils::getMem();
            ucpu = float(GeoPerfUtils::getCpu()/100.);
            cpuMemStats <<"RPCcabling init with maps from COOL \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
            mem = umem;
            cpu = ucpu;
#endif

	  } 
	}
        else {	
          postponeCabInit=true;
	}
    }

    // read from file if requested of if reading from COOL failed 
    if ( (!m_RPCMapfromCool) || tryRecoveringByReadingFromFile)
    {
      // implement the search of LVL1conf.data trought the pathresolver utility.
      std::string conf_filename;
      conf_filename = PathResolver::find_file (m_conf_filename, "DATAPATH");
      if (conf_filename.empty())
      {
	  msg(MSG::ERROR) 
		<< "Cannot locate " << m_conf_filename 
		<< " from ${DATAPATH}" << endreq;
	  return StatusCode::FAILURE;
      }
      else msg(MSG::INFO)
	<< "Cabling conf file <" << m_conf_filename
	<< "> located at ${DATAPATH}" << endreq;

      std::string corr_filename;
      corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
      if (conf_filename.empty())
      {
	  msg(MSG::ERROR) 
		<< "Cannot locate " << m_corr_filename 
		<< " from ${DATAPATH}" << endreq;
	  return StatusCode::FAILURE;
      }

      if(!m_RPCTriggerRoadsfromCool){
	cabling = 
	    dynamic_cast<CablingRPC*>(CablingRPC::instance(conf_filename,corr_filename,roads_dir,
							   m_cosmic_configuration));
	msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
	if (!cabling->isLoaded())
	{
	    // this can happen if someone has already built an empty Cabling Singleton 
	    // before initMappingModel is called
	    cabling->initMapsFromASCII();
#ifdef RPCCAB_MONCPU
            umem = GeoPerfUtils::getMem();
            ucpu = float(GeoPerfUtils::getCpu()/100.);
            cpuMemStats <<"RPCcabling init with maps from file \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
            mem = umem;
            cpu = ucpu;
#endif
	}
      }
      else postponeCabInit = true;
    }

    if ( !cabling->isLoaded() ) 
    {
      if(postponeCabInit){
          msg(MSG::INFO)<<" InitMappingModel: Trigger roads not yet loaded from COOL - postpone cabling initialization " << endreq;
        return StatusCode::SUCCESS;
      }
      else {
        msg(MSG::ERROR) <<   "InitMappingModel: RPC cabling model is not loaded!"  << endreq;
        return StatusCode::FAILURE;
      }
    } 
    else msg(MSG::INFO) << "InitMappingModel: RPC cabling model is loaded!" << *cabling << endreq;
        
    // build fast map between offlineID and PAD online ID
    if (!buildOfflineOnlineMap())
    {
        msg(MSG::ERROR) << " CAnnot build the map between offlineID and " 
	     << "PAD online ID!" << endreq;
        return StatusCode::FAILURE;
    }

#ifdef RPCCAB_MONCPU
    umem = GeoPerfUtils::getMem();
    ucpu = float(GeoPerfUtils::getCpu()/100.);
    cpuMemStats <<"buildOfflineOnlineMap done \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;
#endif

    msg(MSG::DEBUG) << "DUMP THE FULL RPC ROBID LIST: " 
         << m_fullListOfRobIds.size() << endreq;
    if(msgLvl(MSG::DEBUG)) 
    {
      for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
      {
        msg(MSG::DEBUG) << "RobId=" << MSG::hex 
                 << m_fullListOfRobIds[i] << MSG::dec << endreq; 
      }
    }
        

    //    if(msg.Lvl(MSG::DEBUG))
    if(false)
    {
        msg(MSG::DEBUG) << "DUMP THE OFFLINE ID MAP OF THE PADs: " << endreq;
        for(int side=0;side<2;++side)
            for(int Sector=0;Sector<32;++Sector)
                for(int Pad=0;Pad<10;++Pad)
                {
                    Identifier ID;
                    if(giveOffflineID(side,Sector,Pad,ID))
                        msg(MSG::DEBUG)<<  "Side="   << side 
                                       << " Sector=" << Sector
                                       << " Pad="    << Pad << ": identifier is "
                                       << m_pRpcIdHelper->show_to_string(ID) 
                                       << endreq;
                }
    }
    // here get and provide the HashFunction
    if (m_padHashIdHelper) delete m_padHashIdHelper;
    m_padHashIdHelper = new RpcPadIdHash();	

#ifdef RPCCAB_MONCPU
    umem = GeoPerfUtils::getMem();
    ucpu = float(GeoPerfUtils::getCpu()/100.);
    cpuMemStats <<"PadHAshIdHelper done \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;
    cpuMemStats.close();
#endif

    msg(MSG::INFO) << name() << " initialized succesfully" << endreq;
    return StatusCode::SUCCESS;
}

StatusCode MuonRPC_CablingSvc::initTrigRoadsModel(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
    msg(MSG::INFO)<<"initTrigRoadsModel has been called"<<endreq;
    //    msg(MSG::INFO)<<"ToolHandle in initTrigRoadsModel - <"<<m_condTriggerTool<<">"<<endreq;

    std::string roads_dir;
    roads_dir = PathResolver::find_directory (m_database_repository, "DATAPATH");
    if(!m_RPCTriggerRoadsfromCool){
      if (roads_dir.empty())
      {
        msg(MSG::INFO) 
            << "Cannot locate " << m_database_repository 
            << " default trigger configuratyion is loaded" << endreq;
        roads_dir=".";
      } else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from " << roads_dir << endreq;
      }
      return StatusCode::SUCCESS;
    }
    else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from COOL" << endreq;
    }

    // retrive CablinRPC singleton - cast to CablingRPC
    msg(MSG::INFO)<<"Retrieve the pointer to the cabling singleton "<<endreq; 
    CablingRPC* cabling = dynamic_cast<CablingRPC*>(CablingRPC::instance());
    msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
    // clear cache in case this is not first initialization on the job
    cabling->clearCache();
    cabling->ClearPtoTrigRoads();
    msg(MSG::INFO)<<"Cabling singleton cache has been cleared"<<endreq;
    bool postponeCabInit = false;

    if(m_RPCTriggerRoadsfromCool)
    {
	    
       StatusCode sc = m_condTriggerTool->loadParameters(I, keys);
       if (sc.isFailure())
       {
	    msg(MSG::WARNING)<<"Reading trigger roads from COOL failed; try to read from file"<<endreq;
            return StatusCode::FAILURE;
       }

       // Eta roads and parameters
       const std::vector<std::string> RPCEtaCM_File = m_condTriggerTool->GetStringRPCEtaCM_File();
       const std::vector<std::string> RPCEtaTh0 = m_condTriggerTool->GetStringRPCEtaTh0();
       //	const std::vector<std::string> RPCEtaTh1 = m_condTriggerTool->GetStringRPCEtaTh1();
       //	const std::vector<std::string> RPCEtaTh2 = m_condTriggerTool->GetStringRPCEtaTh2();
       const std::vector<std::string> RPCEtaSequence_Th = m_condTriggerTool->GetStringRPCEtaSequence_Th();
       const std::vector<std::string> RPCEtaInfo = m_condTriggerTool->GetStringRPCEtaInfo();

       // Phi roads and parameters
       const std::vector<std::string> RPCPhiCM_File = m_condTriggerTool->GetStringRPCPhiCM_File();
       const std::vector<std::string> RPCPhiTh0 = m_condTriggerTool->GetStringRPCPhiTh0();
       const std::vector<std::string> RPCPhiInfo = m_condTriggerTool->GetStringRPCPhiInfo();


       for(unsigned int it=0; it<RPCEtaCM_File.size(); it++){
          msg(MSG::DEBUG)<<"Pointers to ETA CM file ...."<<(uintptr_t) &RPCEtaCM_File.at(it) <<endreq;
          msg(MSG::DEBUG)<<"Pointers to ETA Th0 ........"<<(uintptr_t) &RPCEtaTh0.at(it) <<endreq;
          msg(MSG::DEBUG)<<"Pointers to ETA sequence ..."<<(uintptr_t) &RPCEtaSequence_Th.at(it) <<endreq;
          msg(MSG::DEBUG)<<"Pointers to ETA Info ......."<<(uintptr_t) &RPCEtaInfo.at(it) <<endreq;
       }
       for(unsigned int it=0; it<RPCPhiCM_File.size(); it++){
          msg(MSG::DEBUG)<<"Pointers to PHI CM file ...."<<(uintptr_t) &RPCPhiCM_File.at(it) <<endreq;
          msg(MSG::DEBUG)<<"Pointers to PHI Th0 ........"<<(uintptr_t) &RPCPhiTh0.at(it) <<endreq;
          msg(MSG::DEBUG)<<"Pointers to PHI Info ......."<<(uintptr_t) &RPCPhiInfo.at(it) <<endreq;

       }
       // Trigger Roads Header 
       const std::map<std::string, std::string>* RPC_trigroads = m_condTriggerTool->GetRoads();
       std::map<std::string, std::string>::const_iterator it;
       it=RPC_trigroads->find("infos.txt");
       ATH_MSG_INFO("======== RPC Trigger Roads from COOL - Header infos ========");
       ATH_MSG_INFO("\n"+it->second+"\n");	

       cabling->SetPtoTrigRoads(RPC_trigroads);

       if(RPCCorrMap != 0 && RPCConfMap != 0) {
          msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endreq;
	  cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(RPCConfMap,RPCCorrMap,roads_dir, m_cosmic_configuration));
 	  msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
	  if (!cabling->isLoaded())
	  {
	    // this can happen is someone has already built an empty Cabling Singleton 
	    // before initMappingModel is called
	    cabling->initMapsFromCOOL();
	  } 
       }
       else {	
          if(m_RPCMapfromCool)postponeCabInit=true;
          else {
            
	    // implement the search of LVL1conf.data trought the pathresolver utility.
	    std::string conf_filename;
	    conf_filename = PathResolver::find_file (m_conf_filename, "DATAPATH");
            if (conf_filename.empty())
	    {
	      msg(MSG::ERROR) 
		<< "Cannot locate " << m_conf_filename 
		<< " from ${DATAPATH}" << endreq;
	      return StatusCode::FAILURE;
	    }

   	    std::string corr_filename;
	    corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
            if (conf_filename.empty())
	    {
	      msg(MSG::ERROR) 
		<< "Cannot locate " << m_corr_filename 
		<< " from ${DATAPATH}" << endreq;
	      return StatusCode::FAILURE;
	    }
	    cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(conf_filename,corr_filename,roads_dir,
							   m_cosmic_configuration));
	    msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endreq;
	    if (!cabling->isLoaded())
	    {
	      // this can happen if someone has already built an empty Cabling Singleton 
	      // before initMappingModel is called
	      cabling->initMapsFromASCII();
	    }
	  }
       }
       if ( !cabling->isLoaded() ) 
       {
          if(postponeCabInit){
            msg(MSG::INFO)<<"InitTrigRoadsModel: Cabling maps not yet loaded from COOL - postpone cabling initialization " << endreq;
            return StatusCode::SUCCESS;
	  }
          else {
            msg(MSG::ERROR) <<   "InitTrigRoadsModel: RPC cabling model is not loaded!"  << endreq;
            return StatusCode::FAILURE;
          }
       } 
       else msg(MSG::INFO) << "InitTrigRoadsModel: RPC cabling model is loaded!" << *cabling << endreq;
        
        // build fast map between offlineID and PAD online ID
       if (!buildOfflineOnlineMap())
       {
          msg(MSG::ERROR) << " CAnnot build the map between offlineID and " 
	     << "PAD online ID!" << endreq;
          return StatusCode::FAILURE;
       }

       msg(MSG::DEBUG) << "DUMP THE FULL RPC ROBID LIST: " 
         << m_fullListOfRobIds.size() << endreq;
       if(msgLvl(MSG::DEBUG)) 
       {
         for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
         {
           msg(MSG::DEBUG) << "RobId=" << MSG::hex 
                 << m_fullListOfRobIds[i] << MSG::dec << endreq; 
         }
       }
        

       //    if(msg.Lvl(MSG::DEBUG))
       if(false)
       {
         msg(MSG::DEBUG) << "DUMP THE OFFLINE ID MAP OF THE PADs: " << endreq;
         for(int side=0;side<2;++side)
           for(int Sector=0;Sector<32;++Sector)
             for(int Pad=0;Pad<10;++Pad)
             {
                    Identifier ID;
                    if(giveOffflineID(side,Sector,Pad,ID))
                        msg(MSG::DEBUG)<<  "Side="   << side 
                                       << " Sector=" << Sector
                                       << " Pad="    << Pad << ": identifier is "
                                       << m_pRpcIdHelper->show_to_string(ID) 
                                       << endreq;
	     }
       }
       // here get and provide the HashFunction
       if (m_padHashIdHelper) delete m_padHashIdHelper;
       m_padHashIdHelper = new RpcPadIdHash();	
      // 
      msg(MSG::INFO) << name() << " initialized succesfully" << endreq;
    }
    return StatusCode::SUCCESS;
}

//method to be used by RPCgeometry
std::vector<const RDOindex*> MuonRPC_CablingSvc::GiveHashVec() const
{
    return m_HashVec;
}

