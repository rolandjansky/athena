/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL1_STANDALONE

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "RPCcabling/RPCcablingSvc.h"
#include "RPCcablingInterface/RDOindex.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include "RPCgeometry/IRPCgeometrySvc.h"
#include "MuonCablingTools/OutputDef.h"

#include "PathResolver/PathResolver.h"

#include <set>

using namespace RPCcabling;

unsigned int 
RPCcablingSvc::checkLowCode(unsigned int code) const
{
    while (code%100 && !m_pRpcGeo->existCode(code)) {code -= 1;}
    return code;
}

unsigned int 
RPCcablingSvc::checkHighCode(unsigned int code) const
{
    while (code%100 && !m_pRpcGeo->existCode(code)) {code += 1;}
    return code;
}


RPCcablingSvc::RPCcablingSvc(const std::string& name,ISvcLocator* sl) :
    Service( name, sl )  
{
    declareProperty( "ConfFileName", m_conf_filename="RPCcabling.LVL1confAtlas.data" );
    declareProperty( "CorrFileName", m_corr_filename="RPCcabling.LVL1confAtlas.corr" );
    declareProperty( "DatabaseRepository", m_database_repository="." );
    declareProperty( "CosmicConfiguration",m_cosmic_configuration=false );
    m_padHashIdHelper = 0;
}

// queryInterface 
StatusCode RPCcablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if( IID_RPCcablingSvc.versionMatch(riid) ) 
    { 
        *ppvIF = dynamic_cast<IRPCcablingSvc*>(this); 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }

    addRef();
    return StatusCode::SUCCESS;
} 

StatusCode RPCcablingSvc::initialize() 
{
    StatusCode sc;

    MsgStream mlog(messageService(), name());
    
    mlog << MSG::INFO << "Initializing " << name() << " - package version " 
	 << PACKAGE_VERSION << endreq;

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;
    
    m_pRpcGeo=0;
    sc = service("RPCgeometrySvc", m_pRpcGeo);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR
             << "Unable to retrieve the RPC geometry Service"
             << endreq;
	return StatusCode::FAILURE;
    } 

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
    if (conf_filename == "")
    {
       mlog << MSG::ERROR 
            << "Cannot locate " << m_conf_filename 
	    << " from ${DATAPATH}" << endreq;
       return StatusCode::FAILURE;
    }

    std::string corr_filename;
    corr_filename = PathResolver::find_file (m_corr_filename, "DATAPATH");
    if (conf_filename == "")
    {
       mlog << MSG::ERROR 
            << "Cannot locate " << m_corr_filename 
	    << " from ${DATAPATH}" << endreq;
       return StatusCode::FAILURE;
    }


    const CablingRPCBase* cabling = 
        CablingRPC::instance(conf_filename,corr_filename,m_database_repository,
	                     m_cosmic_configuration);

    if ( !cabling->isLoaded() ) 
    {
        mlog << MSG::ERROR <<   "RPC cabling model is not loaded!"  << std::endl 
            << endreq;
        return StatusCode::FAILURE;
    } else {
        mlog << MSG::INFO << *cabling << endreq;
    }
    
    // here get and provide the HashFunction
    if (m_padHashIdHelper) delete m_padHashIdHelper;
    m_padHashIdHelper = new RpcPadIdHash();	
        
    
    // build fast map between offlineID and PAD online ID
    if (!buildOfflineOnlineMap())
    {
        mlog << MSG::ERROR << " CAnnot build the map between offlineID and " 
	     << "PAD online ID!" << endreq;
        return StatusCode::FAILURE;
    }

    mlog << MSG::DEBUG << "DUMP THE FULL RPC ROBID LIST: " 
         << m_fullListOfRobIds.size() << endreq;
    for(unsigned int i=0;i<m_fullListOfRobIds.size();++i)
    {
        if(mlog.level() <= MSG::DEBUG)
            mlog << MSG::DEBUG << "RobId=" << MSG::hex 
                 << m_fullListOfRobIds[i] << MSG::dec << endreq; 
    }
    
    
//     // build the map a la region selector for the PAD
//     OfflineOnlineHashMap::const_iterator hash = m_HashVec.begin();
//     for( ; hash!= m_HashVec.end(); ++hash)
//     {
//         unsigned short int sub = ((*hash)->side()==0x65)? 1 : 0;
// 	unsigned short int sec =  (*hash)->SLid();
// 	unsigned short int pad =  (*hash)->PADid();
	
// 	PADdim dimension;
	
// 	if(etaPhiDim(sub,sec,pad,dimension)) m_regions.push_back(dimension);
//     }
    
    
//     DimensionMap::const_iterator it = m_regions.begin();
//     mlog << MSG::DEBUG << "DUMP THE ETA-PHI MAP OF THE PADs: " 
//                        << m_HashVec.size() << endreq;
//     uint32_t hashID = 0;
//     for( ; it!=m_regions.end();++it)
//     {
//           if(mlog.level() <= MSG::DEBUG)
//           mlog << MSG::DEBUG << "hashID=" << hashID << "  "
//                              << "side=" << (m_HashVec[hashID])->side()
// 	                     << ", sector=" << (m_HashVec[hashID])->SLid()
// 		             << ", pad=" << (m_HashVec[hashID])->PADid() 
// 		             << ":  EtaMin=" << (*it).etaMin
// 		             << ", EtaMax=" << (*it).etaMax
// 		             << ", PhiMin=" << (*it).phiMin
// 		             << ", PhiMax=" << (*it).phiMax		   
// 		             << endreq;
// 	 ++hashID;
//     }
    
    /*
    std::vector<uint32_t> found = getHash(-.3, .3, 6., 0.2);
    std::vector<uint32_t>::const_iterator pippo = found.begin();
    for( ; pippo!=found.end();++pippo)
    {
         std::cout << "hashID=" << *pippo << "  "
                   << "side=" << (m_HashVec[*pippo])->side()
	           << ", sector=" << (m_HashVec[*pippo])->SLid()
		   << ", pad=" << (m_HashVec[*pippo])->PADid() 
		   << ":  EtaMin=" << m_regions[*pippo].etaMin
		   << ", EtaMax=" << m_regions[*pippo].etaMax
		   << ", PhiMin=" << m_regions[*pippo].phiMin
		   << ", PhiMax=" << m_regions[*pippo].phiMax		   
		   << std::endl;
    }
    */
    
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

StatusCode RPCcablingSvc::finalize() 
{
    CablingRPC::delete_instance();
    return StatusCode::SUCCESS;
} 


// Implement the specifics ... 


const CablingRPC::RDOmap& 
RPCcablingSvc::give_RDOs() const
{    
    return CablingRPC::s_instance->give_RDOs ();
}

const CMAparameters::CMAlist
RPCcablingSvc::give_CMAs(const int logic_sector,const ViewType side,
                     const int station,const int cabling_code) const
{
    return CablingRPC::s_instance->give_CMAs
                                 (logic_sector,side,station,cabling_code);
}


bool 
RPCcablingSvc::give_global_address(unsigned int code,int& global_strip_address,
int& global_connector_address,int& low_eta_strips,int& hi_eta_strips) const
{
    return CablingRPC::s_instance->give_global_address(code,
                                                       global_strip_address,
                                                       global_connector_address,
                                                       low_eta_strips,
                                                       hi_eta_strips);
}

bool 
RPCcablingSvc::give_RoI_borders (unsigned short int SubsystemId,
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

bool
RPCcablingSvc::give_LowPt_borders (unsigned short int SubsystemId,
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

bool
RPCcablingSvc::give_HighPt_borders (unsigned short int SubsystemId,
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

std::list<unsigned int> 
RPCcablingSvc::give_strip_code (unsigned short int SubsystemId,
                                unsigned short int SectorId,
			        unsigned short int PADId,
				unsigned short int CMAId,
				unsigned short ijk,
				unsigned short int Channel) const
{
    return CablingRPC::s_instance->give_strip_code(SubsystemId,SectorId,PADId,
				                   CMAId,ijk,Channel);
}


std::list<Identifier> 
RPCcablingSvc::give_strip_id   (unsigned short int SubsystemId,
                                unsigned short int SectorId,
			        unsigned short int PADId,
				unsigned short int CMAId,
				unsigned short ijk,
				unsigned short int Channel) const
{
    std::list<Identifier> id;
    
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
RPCcablingSvc::printType(int type,int station,std::string element,
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
    
    CablingRPC::s_instance->PrintType(display,type,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;
}


void
RPCcablingSvc::printSector(int sector,int station,std::string element,
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

    CablingRPC::s_instance->PrintSector(display,sector,station,element,0,detail);
    log << MSG::DEBUG << display.str() << endreq;
}


const CablingRPCBase*
RPCcablingSvc::getRPCCabling() const
{
    return CablingRPC::instance();
}


bool
RPCcablingSvc::buildOfflineOnlineMap()
{    
    //MsgStream log(messageService(), name());
    //log << MSG::DEBUG << name() << ": Start building fast offline/online map" 
    //    << endreq;
    
    m_RDOmap.clear();
    m_HashVec.clear();
    
    std::set< uint32_t > ROBid;


    const CablingRPCBase* cab = CablingRPC::instance();
    const CablingRPC::RDOmap& map = cab->give_RDOs();
    CablingRPC::RDOmap::const_iterator it = map.begin();
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
    
    
    std::set< uint32_t >::const_iterator rob;
    for(rob=ROBid.begin();rob!=ROBid.end();++rob) 
        m_fullListOfRobIds.push_back(*rob);
    
    return true;
}

bool
RPCcablingSvc::giveOnlineID(const Identifier compactID,
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
RPCcablingSvc::giveOnlineID(const unsigned int hashID,
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
RPCcablingSvc::giveFullListOfRobIds() const
{
    return m_fullListOfRobIds;
}

// bool 
// RPCcablingSvc::etaPhiDim(unsigned short int SubsystemId,
//                          unsigned short int SectorId,
//                          unsigned short int PADId,
// 		         PADdim& dim) const
// {

//     for (int i=PADId*4;i<(PADId+1)*4;++i)
//     {
//        unsigned int EtaLowBorder1;
//        unsigned int EtaHighBorder1;
//        unsigned int PhiLowBorder1;
//        unsigned int PhiHighBorder1;
       
//        unsigned int EtaLowBorder2;
//        unsigned int EtaHighBorder2;
//        unsigned int PhiLowBorder2;
//        unsigned int PhiHighBorder2;
       
//        bool low = CablingRPC::s_instance->give_LowPt_borders(SubsystemId, 
//                                                              SectorId, i,
//                                                              EtaLowBorder1, 
// 							     EtaHighBorder1, 
// 							     PhiLowBorder1, 
// 							     PhiHighBorder1);
							     
//        bool high = CablingRPC::s_instance->give_HighPt_borders(SubsystemId, 
//                                                                SectorId, i,
//                                                                EtaLowBorder2, 
// 						 	       EtaHighBorder2, 
// 							       PhiLowBorder2, 
// 							       PhiHighBorder2); 
    
//        PADdim etaphi;
//        bool result = true;
       
//        if(low && high) 
//        {
//            PADdim etaphi1;
//            bool res1 = giveEtaPhi(EtaLowBorder1,EtaHighBorder1,
//                                      PhiLowBorder1,PhiHighBorder1,
// 				     etaphi1);
//            PADdim etaphi2;
//            bool res2 = giveEtaPhi(EtaLowBorder2,EtaHighBorder2,
// 				     PhiLowBorder2,PhiHighBorder2,
// 				     etaphi2);
       
//            result = res1 | res2;
//            if(res1&res2)
// 	   {
// 	       etaphi.etaMin =(etaphi1.etaMin > etaphi2.etaMin)? etaphi2.etaMin: 
// 	                                                         etaphi1.etaMin;
//                etaphi.etaMax =(etaphi1.etaMax < etaphi2.etaMax)? etaphi2.etaMax:
// 	                                                         etaphi1.etaMax;
//                etaphi.phiMin =(etaphi1.phiMin > etaphi2.phiMin)? etaphi2.phiMin:
// 	                                                         etaphi1.phiMin;
//                etaphi.phiMax =(etaphi1.phiMax < etaphi2.phiMax)? etaphi2.phiMax:
// 	                                                         etaphi1.phiMax;  
// 	   }
// 	   else if (res1)
// 	   {
// 	       etaphi.etaMin = etaphi1.etaMin;
//                etaphi.etaMax = etaphi1.etaMax;
//                etaphi.phiMin = etaphi1.phiMin;
//                etaphi.phiMax = etaphi1.phiMax;
// 	   }
// 	   else if (res2)
// 	   {
// 	       etaphi.etaMin = etaphi2.etaMin;
//                etaphi.etaMax = etaphi2.etaMax;
//                etaphi.phiMin = etaphi2.phiMin;
//                etaphi.phiMax = etaphi2.phiMax;
// 	   }
//        }
//        else if(low)    result &= giveEtaPhi(EtaLowBorder1,EtaHighBorder1,
//                                    PhiLowBorder1,PhiHighBorder1,
// 				   etaphi);
       
//        else if(high)   result &= giveEtaPhi(EtaLowBorder2,EtaHighBorder2,
//                                    PhiLowBorder2,PhiHighBorder2,
// 				   etaphi);
       
//        else continue;
       
//        if(result)
//        {
//            if( dim.etaMin > etaphi.etaMin ) dim.etaMin = etaphi.etaMin;
//            if( dim.etaMax < etaphi.etaMax ) dim.etaMax = etaphi.etaMax;
//            if( dim.phiMin > etaphi.phiMin ) dim.phiMin = etaphi.phiMin;
//            if( dim.phiMax < etaphi.phiMax ) dim.phiMax = etaphi.phiMax;
//        }
//     }
    
//     //if(dim.etaMin == +99999. && dim.etaMax == -99999. &&
//     //   dim.phiMin == +4*M_PI && dim.phiMax == -4*M_PI) return false;
    
       
//     return true;   
// }


// bool 
// RPCcablingSvc::giveEtaPhi(unsigned long int EtaLowBorder,
//                           unsigned long int EtaHighBorder,
//                           unsigned long int PhiLowBorder,
// 		          unsigned long int PhiHighBorder,
//                           PADdim& etaphi) const
// {
//     MsgStream mlog(messageService(), name());
//     if(EtaLowBorder==EtaHighBorder || PhiLowBorder==PhiHighBorder) return false;
//     if(m_pRpcGeo)
//     {
//         float firstEta[3] = {0.,0.,0.};
//         float lastEta[3]  = {0.,0.,0.};
//         float firstPhi[3] = {0.,0.,0.};
//         float lastPhi[3]  = {0.,0.,0.};
	
// 	float etaRad = 0;

//         EtaLowBorder = this->checkLowCode( EtaLowBorder );    
//         if(!m_pRpcGeo->give_strip_coordinates(EtaLowBorder, firstEta))
//         {
// 	    mlog << MSG::ERROR
//                  << "Cannot retrieve the coordinates for EtaLowBorder=" 
// 		 << EtaLowBorder << endreq;
// 	    return false;
//         }   
    
//         EtaHighBorder = this->checkHighCode( EtaHighBorder );
//         if(!m_pRpcGeo->give_strip_coordinates(EtaHighBorder, lastEta))
//         {
// 	    mlog << MSG::ERROR
//                  << "Cannot retrieve the coordinates for EtaHighBorder=" 
// 		 << EtaHighBorder << endreq;
// 	    return false;
//         }
    
//         PhiLowBorder = this->checkLowCode( PhiLowBorder );
//         if(!m_pRpcGeo->give_strip_coordinates(PhiLowBorder, firstPhi))
//         {
// 	    mlog << MSG::ERROR
//                  << "Cannot retrieve the coordinates for PhiLowBorder=" 
// 		 << PhiLowBorder << endreq;
// 	    return false;
//         }   
    
//         PhiHighBorder = this->checkHighCode( PhiHighBorder );
//         if(!m_pRpcGeo->give_strip_coordinates(PhiHighBorder, lastPhi))
//         {
// 	    mlog << MSG::ERROR
//                  << "Cannot retrieve the coordinates for PhiHighBorder=" 
// 		 << PhiHighBorder << endreq;
// 	    return false;
//         }
    
        	
//         if(!m_pRpcGeo->give_strip_radius(EtaLowBorder,etaRad))
//         {
// 	    mlog << MSG::ERROR
//                  << "Cannot retrieve the gas Layer Radius for EtaLowBorder=" 
// 		 << EtaLowBorder << endreq;
// 	    return false;
//         }   
    

	
// 	float etaMin = +99999.;
// 	float etaMax = -99999.;
// 	float phiMin = +4*M_PI;
// 	float phiMax = -4*M_PI;
    
//         computeEtaPhi(firstEta,lastEta,etaRad,firstPhi,lastPhi,
// 	              etaMin, etaMax, phiMin, phiMax);    
    
//         if (etaMin == 0. && etaMax == 0. && phiMin == 0. && phiMax == 0.)
// 	  return false;
        
// 	etaphi.etaMin = etaMin;
// 	etaphi.etaMax = etaMax;
// 	etaphi.phiMin = phiMin;
// 	etaphi.phiMax = phiMax;
    
//         return true;
//     }
//     return false;

// }

// void 
// RPCcablingSvc::computeEtaPhi(float etaLow[3], float etaHigh[3], float etaRad,
//                              float phiLow[3], float phiHigh[3],
// 			     float& etaMin, float& etaMax, 
//                              float& phiMin, float& phiMax) const
// {
//     float Theta;
//     if(etaRad==0) {
//         MsgStream mlog(messageService(), name());
//         mlog << MSG::ERROR 
// 	     << "RPCcablingSvc::computeEtaPhi called with etaRad == 0!"
// 	     << endreq;
        
// 	etaMin = 0.;
// 	etaMax = 0.;
// 	phiMin = 0.;
// 	phiMax = 0.;
	
// 	return;
//     }
    
//     Theta  = (etaLow[2] != 0)? atan(etaRad/fabsf(etaLow[2])) : asin(1);
//     etaMin = (etaLow[2]  > 0)? -log(tan(Theta/2.)) : log(tan(Theta/2.));
    
//     Theta  = (etaHigh[2] != 0)? atan(etaRad/fabsf(etaHigh[2])) : asin(1);
//     etaMax = (etaHigh[2]  > 0)? -log(tan(Theta/2.)) : log(tan(Theta/2.));
        
    
//     if(etaMin > etaMax)
//     {
//         float tmp = etaMax;
// 	etaMax = etaMin;
// 	etaMin = tmp;
//     }
    
//     phiMin = atan2(phiLow[1],phiLow[0]);
//     phiMax = atan2(phiHigh[1] ,phiHigh[0]);
    
//     if(phiMin < 0.) phiMin += 2*M_PI;
//     if(phiMax < 0.) phiMax += 2*M_PI;
// }




bool 
RPCcablingSvc::giveOffflineID(unsigned short int Side,
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

//method to be used by RPCgeometry
std::vector<const RDOindex*> RPCcablingSvc::GiveHashVec() const
{
    return m_HashVec;
}


#endif
