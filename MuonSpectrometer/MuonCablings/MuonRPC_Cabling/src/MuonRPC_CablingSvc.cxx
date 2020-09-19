/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "MuonRPC_Cabling/CablingRPC.h"

#include "RPC_CondCabling/RDOindex.h"
#include "MuonCablingTools/OutputDef.h"

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
    declareProperty( "ConfFilePath", m_conf_filepath="MuonRPC_Cabling/" );
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
    //M. Corradi 2015/1/8
    declareProperty("ApplyFeetPadThresholds", m_ApplyFeetPadThresholds=true,
                    "map 3 low pt thresholds from special feet pads on standard 6 (3low+3high)");
    declareProperty("ForceFeetPadThresholdsFromJO", m_ForceFeetPadThresholdsFromJO=false,
                    "JO override db setting");
    declareProperty("FeetPadThresolds", m_FeetPadThresholds,
                    "threshold numbers assigned to 3 low pt thresholds from special feet pads");
    
}

StatusCode MuonRPC_CablingSvc::initialize()
{
    ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

    StoreGateSvc * detStore=nullptr;
    ATH_CHECK(service("DetectorStore",detStore));
    ATH_CHECK(m_idHelperSvc.retrieve());
    RDOindex::setRpcIdHelper(&m_idHelperSvc->rpcIdHelper());
    
    bool tryRecoveringByReadingFromFile = false;
    bool tryRecoveringByReadingTrigRoadsFromFile = false;

    if(!m_RPCMapfromCool && m_RPCTriggerRoadsfromCool) {
        ATH_MSG_ERROR(" !!! RPC cabling map from files and trigger roads from COOL - option not implemented !!!");
        return StatusCode::FAILURE;
    }    

    if(m_RPCMapfromCool)
    {
    ATH_CHECK(m_condDataTool.retrieve());
    ATH_MSG_INFO("RPCCablingDbTool retrieved with handle "<<m_condDataTool);

	std::vector<std::string> folderNames;
	folderNames.push_back((m_condDataTool)->mapConfFolderName());
	folderNames.push_back((m_condDataTool)->mapCorrFolderName());
                                                               

	msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
	// bool aFolderFound = false;
	short ic=0;
	for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	    ++ic;
	    msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	    if (detStore->contains<CondAttrListCollection>(*ifld)) {
		// aFolderFound=true;
		msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
		const DataHandle<CondAttrListCollection> mapDataClob;
		if (detStore->regFcn(&IRPCcablingSvc::initMappingModel,
				     dynamic_cast<IRPCcablingSvc *>(this),
				     mapDataClob,
				     *ifld)!=StatusCode::SUCCESS)
		{
		    msg(MSG::WARNING)<<"Unable to register call back for initMappingModel against folder <"<<(*ifld)<<">";
		    //return StatusCode::FAILURE;
		}
		else msg(MSG::INFO)<<"initMappingModel registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
	    }
	    else
	    {	
		msg(MSG::WARNING)<<"Folder "<<(*ifld)
				 <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
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
	    msg(MSG::ERROR) << "Could not retrieve RPCTriggerDbTool" << endmsg;
	}
	else
	{
	    msg(MSG::INFO)<<"RPCTriggerDbTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condTriggerTool<<endmsg;
	}


	std::vector<std::string> trigfolderNames;
	trigfolderNames.push_back((m_condTriggerTool)->EtaTableFolderName());
	trigfolderNames.push_back((m_condTriggerTool)->PhiTableFolderName());
	msg(MSG::INFO)<<"Register call-back  against "<<trigfolderNames.size()<<" folders listed below "<<endmsg;
	// bool aFolderFound = false;
	short ic=0;
	for (std::vector<std::string>::const_iterator ifld =trigfolderNames.begin(); ifld!=trigfolderNames.end(); ++ifld )
	{
	    ++ic;
	    msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	    if (detStore->contains<CondAttrListCollection>(*ifld)) {
		// aFolderFound=true;
		msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
		const DataHandle<CondAttrListCollection> TrigRoadClob;
       		if (detStore->regFcn(&ICallBackMuonRPC_Cabling::initTrigRoadsModel,
				     dynamic_cast<ICallBackMuonRPC_Cabling *>(this),
				     TrigRoadClob,
				     *ifld)!=StatusCode::SUCCESS)
		{
		    msg(MSG::WARNING)<<"Unable to register call back for initTrigRoadsModel against folder <"<<(*ifld)<<">";
		    //return StatusCode::FAILURE;
		}
		else msg(MSG::INFO)<<"initTrigRoadsModel registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
	    }
	    else
	    {	
		msg(MSG::WARNING)<<"Folder "<<(*ifld)
				 <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
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
    msg(MSG::VERBOSE) << "queryInterface Start" << endmsg;
    if(IRPCcablingSvc::interfaceID().versionMatch(riid) ) 
    {
        msg(MSG::VERBOSE) << "versionMatch=true" << endmsg;
        *ppvIF = (IRPCcablingSvc*)(this);
    } else {
        msg(MSG::VERBOSE) << "cannot find the interface!" << endmsg;
        return AthService::queryInterface(riid, ppvIF); 
      }
  msg(MSG::VERBOSE) << "queryInterface succesfull" << endmsg;
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
    //    std::cout<<"in give_RoI_borders_id:: subsystemId/SectorId/RoIId = "<<SubsystemId<<"/"<<SectorId<<"/"<<RoIId<<std::endl;
    //    std::cout<<" since give_RoI_borders is ok, EtaLowBorder/EtaHighBorder/PhiLowBorder/PhiHighBorder "<<EtaLowBorder<<"/"<<EtaHighBorder<<"/"<<PhiLowBorder<<"/"<<PhiHighBorder<<std::endl;
    EtaLowBorder_id  =  this->protected_strip_OffId_fromCode( EtaLowBorder);
    EtaHighBorder_id =  this->protected_strip_OffId_fromCode( EtaHighBorder);
    PhiLowBorder_id  =  this->protected_strip_OffId_fromCode( PhiLowBorder);
    PhiHighBorder_id =  this->protected_strip_OffId_fromCode( PhiHighBorder);
  }
  //  else {std::cout<<" since give_RoI_borders is not ok"<<std::endl;}
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



bool MuonRPC_CablingSvc::give_Pad_Parameters(unsigned short int logic_sector,
                                             unsigned short int PADId,
                                             bool & feet,
                                             bool & eta_and_phi,
                                             unsigned short int  & cma_mask,
                                             unsigned short int  & feet_th0,
                                             unsigned short int  & feet_th1,
                                             unsigned short int  & feet_th2 )const {
 
    if ( logic_sector>=64 || PADId >= 8) return false;
    
    feet = m_RPCPadParameters_array[logic_sector][PADId].feet_on();
    eta_and_phi = m_RPCPadParameters_array[logic_sector][PADId].eta_and_phi();
    cma_mask    = m_RPCPadParameters_array[logic_sector][PADId].cma_mask();
    feet_th0    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(0);
    feet_th1    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(1);
    feet_th2    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(2);    
    

    /*    
    std::cout << "MuonRPC_CablingSvc::give_Pad_Parameters called:   "
              << feet << " "
              << eta_and_phi << " "
              << cma_mask << " "
              << feet_th2
              << std::endl;
    */
    return true;
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
  Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(rpc_strip.stationName,
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
Identifier MuonRPC_CablingSvc::protected_strip_OffId_fromCode (unsigned long int strip_code) const 
{
  RPCofflineId rpc_strip  = CablingRPC::s_instance->strip_id_fromCode (strip_code);
  if (rpc_strip.stationName=="BOG")
    {
      if (std::abs(rpc_strip.stationEta)==4 && rpc_strip.doubletR==2 && rpc_strip.measuresPhi==1)
	{
	  if (rpc_strip.strip>48) rpc_strip.strip=48;
	}
    }
  if (rpc_strip.stationName=="BME")
    {
      if (rpc_strip.doubletR==2 && rpc_strip.measuresPhi==1)
	{
	  if (rpc_strip.strip>36) rpc_strip.strip=36;
	}
    }
  Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(rpc_strip.stationName,
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
        Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID((*it).stationName,
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
    
    __osstream display;
    
    log << MSG::DEBUG << name() << " is demanding a cabling printout" <<endmsg;
    
    CablingRPC::s_instance->PrintType(display,type,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endmsg;

   return;
    }

void
MuonRPC_CablingSvc::printSector(int sector,int station,std::string element,bool detail) const
{
    MsgStream log(msgSvc(), name());
    
    __osstream display;
 
    log << MSG::DEBUG << name() << " is demanding a cabling printout" <<endmsg;

    CablingRPC::s_instance->PrintSector(display,sector,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endmsg;

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
  ATH_MSG_INFO("buildOfflineOnlineMap");
  m_RDOmap.clear();
  m_HashVec.clear();
  
  std::set< uint32_t > ROBid;
  
  IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();
  
  const CablingRPCBase* cab = CablingRPC::instance();
  ATH_MSG_DEBUG("cabling singleton at <"<<(uintptr_t)cab<<">");
  const CablingRPC::RDOmap& map = cab->give_RDOs();
  ATH_MSG_DEBUG("cabling singleton at <"<<(uintptr_t)cab<<"> hold an RDOmap of size "<<map.size());
  CablingRPC::RDOmap::const_iterator it = map.begin();
  unsigned int nEntries=0;
  // reserve enough space in the hash-vector
  m_HashVec.reserve(map.size());
  while(it != map.end()) {
    // get pointer to RDOindex class
    const RDOindex* pRDOindex = &((*it).second);
    m_HashVec.push_back(pRDOindex);
    if( m_HashVec.size() != pRDOindex->hash()+1 ) return false;
    ++nEntries;
    ATH_MSG_DEBUG("RDO Hash Vect new entry  "<<nEntries<<" vector size is "<<m_HashVec.size()<<" this pRDOindex->hash() "<<pRDOindex->hash());
	
    const unsigned short int rob_id = pRDOindex->ROBid();
    const unsigned short int rod_id = pRDOindex->RODid();
    const unsigned short int sub_id = pRDOindex->side();
    const unsigned short int sec_id = pRDOindex->SLid();
    const unsigned short int pad_id = pRDOindex->PADid();
    
    uint32_t ROD_ID = (sub_id << 16) | rod_id;
    uint32_t ROB_ID = (sub_id << 16) | rob_id;
    
    ROBid.insert(ROB_ID); 
    
    unsigned short int sub_id_index = ((sub_id==0x65)? 1:0); // convert 0x65 -> 1 (side A) and 0x66 -> 0 (side C)
    
    Identifier id;
    pRDOindex->pad_identifier( id );

    //build the offline_id vector
    m_offline_id[sub_id_index][sec_id][pad_id] = id;
    
    // build the map
    std::pair < OfflineOnlineMap::iterator, bool> ins = 
                m_RDOmap.insert(OfflineOnlineMap::value_type(id,pRDOindex));
    ATH_MSG_DEBUG("OfflineOnlineMap new entry: value  "<<m_idHelperSvc->rpcIdHelper().show_to_string(id)
                   <<" hash of the RDOindex (key) = "<<pRDOindex->hash());
                
    if(!ins.second) return false;
    
    //build the ROB->RDO map
    std::pair<std::set<IdentifierHash>::iterator, bool> insert_ROB_RDO_returnVal = m_ROB_RDO_map[ROB_ID].insert( IdentifierHash(pRDOindex->hash()) );
    if (insert_ROB_RDO_returnVal.second)
      ATH_MSG_DEBUG("A new RDO HashId = " << pRDOindex->hash() << " registered for ROB Id = " << ROB_ID);
    else
      ATH_MSG_VERBOSE("The RDO HashId = " << pRDOindex->hash() << " was already registered for ROB Id = " << ROB_ID);
    
    //build the PRD->RDO and PRD->ROB maps
    ATH_MSG_VERBOSE("Looking for PRDs corresponding to this RDO");
    std::list<Identifier> strip_id_list;
    IdentifierHash rdoHashId( (IdentifierHash::value_type)pRDOindex->hash() );
    ATH_MSG_VERBOSE("RDO HashId = " << (int)rdoHashId << " RDO Id = " << id.getString() 
                    << " ROB Id = " << MSG::hex << ROB_ID << MSG::dec
                    << " ROD Id = " << MSG::hex << ROD_ID << MSG::dec);
    
    for (unsigned short int CMAId : {2, 3, 6 ,7}) { //loop over phi CMA IDs
      for (unsigned short int ijk : {1, 2, 3}) { //loop over IJK identifiers
        strip_id_list.clear();
        for (unsigned short int channel : {0, 31}) {//check for the first and the last channel
          strip_id_list.splice(
            strip_id_list.end(),
            give_strip_id(sub_id_index, sec_id, pad_id, CMAId, ijk, channel)
          );
        }
        for (Identifier strip_id : strip_id_list) {
          Identifier idp = m_idHelperSvc->rpcIdHelper().parentID(strip_id);
          IdentifierHash prdHashId;
          int gethash_code = m_idHelperSvc->rpcIdHelper().get_hash(idp, prdHashId, &rpcModuleContext);
          if (gethash_code != 0) {
            ATH_MSG_DEBUG("Unable to get the PRD HashId! parentID(strip_id)=" << idp.getString());
            continue;
          }
          
          //fill the PRD->RDO map
          std::pair<std::set<IdentifierHash>::iterator, bool> insertRDO_returnVal = m_PRD_RDO_map[prdHashId].insert(rdoHashId);
          if (insertRDO_returnVal.second) {
            ATH_MSG_DEBUG("A new RDO HashId = " << (int)rdoHashId << " registered for PRD HashId = " << (int)prdHashId);
          } else {
            ATH_MSG_VERBOSE("The RDO HashId = " << (int)rdoHashId << " was already registered for PRD HashId = " << (int)prdHashId);
          }
          
          //fill the PRD->ROB map
          std::pair<std::set<uint32_t>::iterator, bool> insertROB_returnVal = m_PRD_ROB_map[prdHashId].insert(ROB_ID);
          if (insertROB_returnVal.second) {
            ATH_MSG_DEBUG("A new ROB Id = " << MSG::hex << ROB_ID << MSG::dec << " registered for PRD HashId = " << (int)prdHashId);
          } else {
            ATH_MSG_VERBOSE("The ROB Id = " << MSG::hex << ROB_ID << MSG::dec << " was already registered for PRD HashId = " << (int)prdHashId);
          }
        }
      }
    }
    
    
    //increase the iterator            
    ++it;
  }
  
  // reserve enough space
  m_fullListOfRobIds.reserve(ROBid.size());
  
  for(uint32_t robid : ROBid) {
    m_fullListOfRobIds.push_back(robid);
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
    //if( ID == 0xFFFFFFFF ) return false; // original, not working with 64 bit identifiers
    if ( ID == m_uninitialized_identifier ) return false;
    
    return true;
}


std::vector<uint32_t> MuonRPC_CablingSvc::giveFullListOfRobIds() const
{
    return m_fullListOfRobIds;
}

// // Dummy methods because we inherit from virtual IRPCcablingSvc
// std::vector<uint32_t> MuonRPC_CablingSvc::getPadHash (float a,float b,float c,float d) const
// {
//     msg(MSG::ERROR) << "Method getPadHash has been removed" << endmsg;
//     std::vector<uint32_t> dummy;
//     return dummy;
// }

// std::vector<uint32_t> MuonRPC_CablingSvc::getRobId (float a,float b,float c,float d) const
// {
//     msg(MSG::ERROR) << "Method getRobId has been removed" << endmsg;
//     std::vector<uint32_t> dummy;
//     return dummy;
// }


StatusCode MuonRPC_CablingSvc::initMappingModel(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
    msg(MSG::INFO)<<"initMappingModel has been called"<<endmsg;
    msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endmsg;

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
    msg(MSG::INFO)<<"Retrieving cabling singleton; to create an empty one or to get the existing one"<<endmsg;
    CablingRPC* cabling = dynamic_cast<CablingRPC*>(CablingRPC::instance());
    if(!cabling) {
      msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
      return StatusCode::FAILURE;
    }
    else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
    // clear cache in case this is not first initialization on the job
    cabling->clearCache();
    cabling->ClearPtoCablingMap();
    msg(MSG::INFO)<<"Cabling singleton cache has been cleared"<<endmsg;
    //cabling->delete_instance();
    //msg(MSG::INFO)<<"Cabling singleton deleted"<<endmsg;

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
            << " default trigger configuratyion is loaded" << endmsg;
        roads_dir=".";
      } else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from " << roads_dir << endmsg;
      }
    }
    else {
        msg(MSG::INFO) 
            << " Trigger roads will be loaded from COOL" << endmsg;
    }
    
    if(m_RPCMapfromCool)
    {
	    
	StatusCode sc = m_condDataTool->loadParameters(I, keys);
	if (sc.isFailure())
	{
	    msg(MSG::WARNING)<<"Reading Cabling maps from COOL failed; try to read from file"<<endmsg;
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

        msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endmsg;
 	ATH_MSG_DEBUG("Dump of the configuration RPC Cabling Map"); 
        ATH_MSG_DEBUG("\n"+(*RPCConfMap));	
 	ATH_MSG_DEBUG("End of Dump of the configuration RPC Cabling Map"); 
	ATH_MSG_DEBUG("Dump of the correction RPC Cabling Map");
        ATH_MSG_DEBUG("\n"+(*RPCCorrMap));
 	ATH_MSG_DEBUG("End of Dump of the configuration RPC Cabling Map"); 

        if((m_RPCTriggerRoadsfromCool && cabling->GetPtoTrigRoads() != 0) ||
           !m_RPCTriggerRoadsfromCool ){
  	  // new CablingRPC instance using the maps from Cool
          msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endmsg;
	  cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(RPCConfMap,RPCCorrMap,roads_dir, m_cosmic_configuration));
	  if(!cabling) {
	    msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
	    return StatusCode::FAILURE;
	  }
	  else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
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
      std::string conf_str = m_conf_filepath;
      conf_filename = PathResolverFindCalibFile ( conf_str.append( std::string(m_conf_filename)) );
      if (conf_filename.empty())
      {
	  msg(MSG::ERROR) 
		<< "Cannot locate " << conf_str
		<< " from ${CALIBPATH}" << endmsg;
	  return StatusCode::FAILURE;
      }
      else msg(MSG::INFO)
	<< "Cabling conf file <" << conf_str
	<< "> located at ${CALIBPATH}" << endmsg;

      std::string corr_filename;
      corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
      if (conf_filename.empty())
      {
	  msg(MSG::ERROR) 
		<< "Cannot locate " << m_corr_filename 
		<< " from ${DATAPATH}" << endmsg;
	  return StatusCode::FAILURE;
      }

      if(!m_RPCTriggerRoadsfromCool){
	cabling = 
	    dynamic_cast<CablingRPC*>(CablingRPC::instance(conf_filename,corr_filename,roads_dir,
							   m_cosmic_configuration));
	if(!cabling) {
	  msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
	  return StatusCode::FAILURE;
	}
	else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
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
          msg(MSG::INFO)<<" InitMappingModel: Trigger roads not yet loaded from COOL - postpone cabling initialization " << endmsg;
        return StatusCode::SUCCESS;
      }
      else {
        msg(MSG::ERROR) <<   "InitMappingModel: RPC cabling model is not loaded!"  << endmsg;
        return StatusCode::FAILURE;
      }
    } 
    else msg(MSG::INFO) << "InitMappingModel: RPC cabling model is loaded!" << *cabling << endmsg;
        
    // build fast map between offlineID and PAD online ID
    if (!buildOfflineOnlineMap())
    {
        msg(MSG::ERROR) << " CAnnot build the map between offlineID and " 
	     << "PAD online ID!" << endmsg;
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
         << m_fullListOfRobIds.size() << endmsg;
    if(msgLvl(MSG::DEBUG)) 
    {
      for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
      {
        msg(MSG::DEBUG) << "RobId=" << MSG::hex 
                 << m_fullListOfRobIds[i] << MSG::dec << endmsg; 
      }
    }
        

    //    if(msg.Lvl(MSG::DEBUG))
    if(false)
    {
        msg(MSG::DEBUG) << "DUMP THE OFFLINE ID MAP OF THE PADs: " << endmsg;
        for(int side=0;side<2;++side)
            for(int Sector=0;Sector<32;++Sector)
                for(int Pad=0;Pad<10;++Pad)
                {
                    Identifier ID;
                    if(giveOffflineID(side,Sector,Pad,ID))
                        msg(MSG::DEBUG)<<  "Side="   << side 
                                       << " Sector=" << Sector
                                       << " Pad="    << Pad << ": identifier is "
                                       << m_idHelperSvc->rpcIdHelper().show_to_string(ID) 
                                       << endmsg;
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

    msg(MSG::INFO) << name() << " initialized succesfully" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MuonRPC_CablingSvc::initTrigRoadsModel(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
    msg(MSG::INFO)<<"initTrigRoadsModel has been called"<<endmsg;

    std::string roads_dir;
    roads_dir = PathResolver::find_directory (m_database_repository, "DATAPATH");
    if(!m_RPCTriggerRoadsfromCool){
      if (roads_dir.empty())
      {
        msg(MSG::INFO) 
            << "Cannot locate " << m_database_repository 
            << " default trigger configuratyion is loaded" << endmsg;
        roads_dir=".";
      } else {
        msg(MSG::INFO) 
            << " Trigger roads have already been loaded from directory: " << roads_dir << endmsg;
      }
      //return StatusCode::SUCCESS;
    }
    else if(m_RPCTriggerRoadsfromCool)
      {
        msg(MSG::INFO) 
	  << " Trigger roads will be loaded from COOL" << endmsg;
	
	// retrive CablinRPC singleton - cast to CablingRPC
	msg(MSG::INFO)<<"Retrieve the pointer to the cabling singleton "<<endmsg; 
	CablingRPC* cabling = dynamic_cast<CablingRPC*>(CablingRPC::instance());
	if(!cabling) {
	  msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
	  return StatusCode::FAILURE;
	}
	else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
	// clear cache in case this is not first initialization on the job
	cabling->clearCache();
	cabling->ClearPtoTrigRoads();
	msg(MSG::INFO)<<"Cabling singleton cache has been cleared"<<endmsg;
	bool postponeCabInit = false;

    //    if(m_RPCTriggerRoadsfromCool)
    //    {
	    
	StatusCode sc = m_condTriggerTool->loadParameters(I, keys);
       if (sc.isFailure())
       {
	    msg(MSG::WARNING)<<"Reading trigger roads from COOL failed; try to read from file"<<endmsg;
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
          msg(MSG::DEBUG)<<"Pointers to ETA CM file ...."<<(uintptr_t) &RPCEtaCM_File.at(it) <<endmsg;
          msg(MSG::DEBUG)<<"Pointers to ETA Th0 ........"<<(uintptr_t) &RPCEtaTh0.at(it) <<endmsg;
          msg(MSG::DEBUG)<<"Pointers to ETA sequence ..."<<(uintptr_t) &RPCEtaSequence_Th.at(it) <<endmsg;
          msg(MSG::DEBUG)<<"Pointers to ETA Info ......."<<(uintptr_t) &RPCEtaInfo.at(it) <<endmsg;
       }
       for(unsigned int it=0; it<RPCPhiCM_File.size(); it++){
          msg(MSG::DEBUG)<<"Pointers to PHI CM file ...."<<(uintptr_t) &RPCPhiCM_File.at(it) <<endmsg;
          msg(MSG::DEBUG)<<"Pointers to PHI Th0 ........"<<(uintptr_t) &RPCPhiTh0.at(it) <<endmsg;
          msg(MSG::DEBUG)<<"Pointers to PHI Info ......."<<(uintptr_t) &RPCPhiInfo.at(it) <<endmsg;

       }
       // Trigger Roads Header 
       const std::map<std::string, std::string>* RPC_trigroads = m_condTriggerTool->GetRoads();
       std::map<std::string, std::string>::const_iterator it;
       it=RPC_trigroads->find("infos.txt");
       if (it==RPC_trigroads->end()){
           ATH_MSG_WARNING("Missing HEADER FILE infos.txt");
       } else {
         ATH_MSG_INFO("======== RPC Trigger Roads from COOL - Header infos ========");
         ATH_MSG_INFO("\n"+it->second+"\n");
         // Read FeetPadThresholds from infos.txt
         if (!m_ForceFeetPadThresholdsFromJO){             
             std::stringstream ss;
             ss << it->second;
             std::string word;
             while (ss >> word){
                 if (word=="FeetPadThresholds"){
                     m_FeetPadThresholds.assign(3,0);
                     ss >> m_FeetPadThresholds.at(0);
                     ss >> m_FeetPadThresholds.at(1);
                     ss >> m_FeetPadThresholds.at(2);
                     msg(MSG::INFO)<<"FeetPadThresholds set from COOL to: "
                                   <<m_FeetPadThresholds.at(0)<<","
                                   <<m_FeetPadThresholds.at(1)<<","
                                   <<m_FeetPadThresholds.at(2)<<endmsg;
                 }
             }
         }
       }




       cabling->SetPtoTrigRoads(RPC_trigroads);

       if(RPCCorrMap != 0 && RPCConfMap != 0) {
          msg(MSG::DEBUG)<<"Pointers to Conf/Corr Maps are "<<(uintptr_t)RPCConfMap<<"/"<<(uintptr_t)RPCCorrMap<<endmsg;
	  cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(RPCConfMap,RPCCorrMap,roads_dir, m_cosmic_configuration));
	  if(!cabling) {
	    msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
	    return StatusCode::FAILURE;
	  } 
	  else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
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
	    std::string conf_str = m_conf_filepath;
	    conf_filename = PathResolverFindCalibFile ( conf_str.append( std::string(m_conf_filename)) );
            if (conf_filename.empty())
	    {
	      msg(MSG::ERROR) 
		<< "Cannot locate " << conf_str
		<< " from ${CALIBPATH}" << endmsg;
	      return StatusCode::FAILURE;
	    }

   	    std::string corr_filename;
	    corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
            if (conf_filename.empty())
	    {
	      msg(MSG::ERROR) 
		<< "Cannot locate " << m_corr_filename 
		<< " from ${DATAPATH}" << endmsg;
	      return StatusCode::FAILURE;
	    }
	    cabling = 
	      dynamic_cast<CablingRPC*>(CablingRPC::instance(conf_filename,corr_filename,roads_dir,
							   m_cosmic_configuration));
	    if(!cabling) {
	      msg(MSG::ERROR)<<"casting of cabling singleton failed."<<endmsg;
	      return StatusCode::FAILURE;
	    }
	    else msg(MSG::DEBUG)<<"cabling singleton at <"<<(uintptr_t)cabling<<">"<<endmsg;
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
            msg(MSG::INFO)<<"InitTrigRoadsModel: Cabling maps not yet loaded from COOL - postpone cabling initialization " << endmsg;
            return StatusCode::SUCCESS;
	  }
          else {
            msg(MSG::ERROR) <<   "InitTrigRoadsModel: RPC cabling model is not loaded!"  << endmsg;
            return StatusCode::FAILURE;
          }
       } 
       else msg(MSG::INFO) << "InitTrigRoadsModel: RPC cabling model is loaded!" << *cabling << endmsg;
        
        // build fast map between offlineID and PAD online ID
       if (!buildOfflineOnlineMap())
       {
          msg(MSG::ERROR) << " CAnnot build the map between offlineID and " 
	     << "PAD online ID!" << endmsg;
          return StatusCode::FAILURE;
       }

       msg(MSG::DEBUG) << "DUMP THE FULL RPC ROBID LIST: " 
         << m_fullListOfRobIds.size() << endmsg;
       if(msgLvl(MSG::DEBUG)) 
       {
         for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
         {
           msg(MSG::DEBUG) << "RobId=" << MSG::hex 
                 << m_fullListOfRobIds[i] << MSG::dec << endmsg; 
         }
       }
        

       //    if(msg.Lvl(MSG::DEBUG))
       if(false)
       {
         msg(MSG::DEBUG) << "DUMP THE OFFLINE ID MAP OF THE PADs: " << endmsg;
         for(int side=0;side<2;++side)
           for(int Sector=0;Sector<32;++Sector)
             for(int Pad=0;Pad<10;++Pad)
             {
                    Identifier ID;
                    if(giveOffflineID(side,Sector,Pad,ID))
                        msg(MSG::DEBUG)<<  "Side="   << side 
                                       << " Sector=" << Sector
                                       << " Pad="    << Pad << ": identifier is "
                                       << m_idHelperSvc->rpcIdHelper().show_to_string(ID) 
                                       << endmsg;
	     }
       }
       // here get and provide the HashFunction
       if (m_padHashIdHelper) delete m_padHashIdHelper;
       m_padHashIdHelper = new RpcPadIdHash();
       //      msg(MSG::INFO) << name() << " initialized succesfully" << endmsg;
    }

    // this must be done both in case of source = COOL or ASCII
    // -----  Initialization of Pad configuration ------ //
    if (m_ApplyFeetPadThresholds) {
        // if using COOL check the existence of a PAD not existing in run-1 cabling
        Identifier offline_id;
        if (!giveOffflineID(0,21,7,offline_id)&&m_RPCTriggerRoadsfromCool) {
            ATH_MSG_INFO("RUN-1 like cabling, not applying FeetPadThresholds");
        }else{
            if (m_FeetPadThresholds.size()!=3){
                // if thresholds vector empty, set it to default
                m_FeetPadThresholds.assign(3,0);
                m_FeetPadThresholds.at(0)=0;
                m_FeetPadThresholds.at(1)=2;
                m_FeetPadThresholds.at(2)=5;
            }
            msg(MSG::INFO) << "Applying FeetPadThresholds : "
                           <<  m_FeetPadThresholds.at(0) << ","
                           <<  m_FeetPadThresholds.at(1) << ","
                           <<  m_FeetPadThresholds.at(2) << endmsg;
            
            const unsigned int NumFeetSectors = 8;
            unsigned int FeetSectors[NumFeetSectors]={21,22,25,26,53,54,57,58};
            const unsigned int NumSpecialFeetPads = 4;
            unsigned int SpecialFeetPads[NumSpecialFeetPads]={2,4,5,7};
            
            for (unsigned int is=0; is<NumFeetSectors; is++) {
                for (unsigned int it=0; it<NumSpecialFeetPads; it++) {
                    
                    m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_on(true);
                    for (unsigned int th=0; th<3; th++) m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_threshold(th,m_FeetPadThresholds.at(th));
                }
            }
            
        }
    }   
    msg(MSG::INFO) << name() << " initialized succesfully" << endmsg;
    return StatusCode::SUCCESS;
}


StatusCode MuonRPC_CablingSvc::giveRDO_fromPRD(const IdentifierHash prdHashId,
                                               std::vector<IdentifierHash>& rdoHashVec) const
{
  StatusCode sc = StatusCode::SUCCESS;
  
  rdoHashVec.clear();
  
  PRD_RDO_Map::const_iterator it = m_PRD_RDO_map.find(prdHashId);
  
  if (it == m_PRD_RDO_map.cend()) return sc;
  
  for (IdentifierHash rdoId : (*it).second)
    rdoHashVec.push_back(rdoId);
  
  return sc;
}


StatusCode MuonRPC_CablingSvc::giveRDO_fromPRD(const std::vector<IdentifierHash>& prdHashVec,  
                                               std::vector<IdentifierHash>& rdoHashVec) const
{
  ATH_MSG_VERBOSE("giveRDO_fromPRD called for a vector of " << prdHashVec.size() << " PRD HashIds");
  
  StatusCode sc = StatusCode::SUCCESS;
  
  rdoHashVec.clear();
  std::set<IdentifierHash> requestedRdoHashSet;
  
  for (IdentifierHash prdHashId : prdHashVec) {
    //find the requested PRD HashId in the map
    PRD_RDO_Map::const_iterator it = m_PRD_RDO_map.find(prdHashId);
    if (it == m_PRD_RDO_map.cend()) {
      ATH_MSG_DEBUG("No RDOs associated with the requested PRD HashId = " << (int)prdHashId);
      continue;
    }
    //if the PRD was found in the map, add the corresponding set of RDO HashIds to the set of requested RDO HashIds
    requestedRdoHashSet.insert((*it).second.begin(), (*it).second.end());
  }
  
  //convert set to vector
  for (IdentifierHash rdoHashId : requestedRdoHashSet)
    rdoHashVec.push_back(rdoHashId);
  
  return sc;
}


StatusCode MuonRPC_CablingSvc::giveRDO_fromROB(const uint32_t robId,  
                                               std::vector<IdentifierHash>& rdoHashVec) const
{
  StatusCode sc = StatusCode::SUCCESS;
  
  rdoHashVec.clear();
  
  ROB_RDO_Map::const_iterator it = m_ROB_RDO_map.find(robId);
  
  if (it == m_ROB_RDO_map.cend()) return sc;
  
  for (IdentifierHash rdoId : (*it).second)
    rdoHashVec.push_back(rdoId);
  
  return sc;
}
                                  
StatusCode MuonRPC_CablingSvc::giveRDO_fromROB(const std::vector<uint32_t>& robIdVec,  
                                               std::vector<IdentifierHash>& rdoHashVec) const
{
  ATH_MSG_VERBOSE("giveRDO_fromROB called for a vector of " << robIdVec.size() << " ROB Ids");
  
  StatusCode sc = StatusCode::SUCCESS;
  
  rdoHashVec.clear();
  std::set<IdentifierHash> requestedRdoHashSet;
  
  for (uint32_t robId : robIdVec) {
    //find the requested ROB Id in the map
    ROB_RDO_Map::const_iterator it = m_ROB_RDO_map.find(robId);
    if (it == m_ROB_RDO_map.cend()) {
      ATH_MSG_DEBUG("The requested ROB Id = " << robId << " not found in the ROB->RDO map!");
      continue;
    }
    //if the ROB Id was found in the map, add the corresponding set of RDO HashIds to the set of requested RDO HashIds
    requestedRdoHashSet.insert((*it).second.begin(), (*it).second.end());
  }
  
  //convert set to vector
  for (IdentifierHash rdoHashId : requestedRdoHashSet)
    rdoHashVec.push_back(rdoHashId);
  
  return sc;
}

StatusCode MuonRPC_CablingSvc::giveROB_fromPRD(const IdentifierHash prdHashId,
                                               std::vector<uint32_t>& robIdVec) const
{
  StatusCode sc = StatusCode::SUCCESS;
  
  robIdVec.clear();
  
  PRD_ROB_Map::const_iterator it = m_PRD_ROB_map.find(prdHashId);
  
  if (it == m_PRD_ROB_map.cend()) return sc;
  
  for (uint32_t robId : (*it).second)
    robIdVec.push_back(robId);
  
  return sc;
}


StatusCode MuonRPC_CablingSvc::giveROB_fromPRD(const std::vector<IdentifierHash>& prdHashVec,  
                                               std::vector<uint32_t>& robIdVec) const
{
  ATH_MSG_VERBOSE("giveROB_fromPRD called for a vector of " << prdHashVec.size() << " PRD HashIds");
  
  StatusCode sc = StatusCode::SUCCESS;
  
  robIdVec.clear();
  std::set<IdentifierHash> requestedRobIdSet;
  
  for (IdentifierHash prdHashId : prdHashVec) {
    //find the requested PRD HashId in the map
    PRD_ROB_Map::const_iterator it = m_PRD_ROB_map.find(prdHashId);
    if (it == m_PRD_ROB_map.cend()) {
      ATH_MSG_DEBUG("No ROBs associated with the requested PRD HashId = " << (int)prdHashId);
      continue;
    }
    //if the PRD was found in the map, add the corresponding set of RDO HashIds to the set of requested RDO HashIds
    requestedRobIdSet.insert((*it).second.begin(), (*it).second.end());
  }
  
  //convert set to vector
  for (IdentifierHash robId : requestedRobIdSet)
    robIdVec.push_back(robId);
  
  return sc;
}


                                  
StatusCode MuonRPC_CablingSvc::giveROB_fromRDO(const IdentifierHash rdoHashId,
                                               uint32_t& robId) const
{
  StatusCode sc = StatusCode::SUCCESS;
  unsigned int hash = (unsigned int)rdoHashId;
  
  if (hash>=m_HashVec.size()) {
    ATH_MSG_ERROR("giveROB_fromRDO called for RDO hash Id outside of range! rdoHashId=" << hash << " >= m_HashVec.size()=" << m_HashVec.size());
    return StatusCode::FAILURE;
  }
  
  unsigned short int rob_id = m_HashVec[hash]->ROBid();
  unsigned short int side  = m_HashVec[hash]->side();
  
  robId = (side << 16) | rob_id;
  
  return sc;
}

StatusCode MuonRPC_CablingSvc::giveROB_fromRDO(const std::vector<IdentifierHash>& rdoHashVec,
                                               std::vector<uint32_t>& robIdVec) const
{
  ATH_MSG_VERBOSE("giveROB_fromRDO called for a vector of " << rdoHashVec.size() << " RDO HashIds");
  
  StatusCode sc = StatusCode::SUCCESS;
  
  robIdVec.clear();
  
  for (IdentifierHash rdoHashId : rdoHashVec) {
    uint32_t robid = 0xffffffff;
    CHECK( giveROB_fromRDO(rdoHashId,robid) );
    robIdVec.push_back(robid);
  }
  
  //sort and remove duplicates
  std::sort( robIdVec.begin(), robIdVec.end() );
  robIdVec.erase( unique( robIdVec.begin(), robIdVec.end() ), robIdVec.end() );
  
  return sc;
}

//method to be used by RPCgeometry
std::vector<const RDOindex*> MuonRPC_CablingSvc::GiveHashVec() const
{
    return m_HashVec;
}

