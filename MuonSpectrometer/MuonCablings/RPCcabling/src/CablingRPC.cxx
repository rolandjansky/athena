/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCcabling/CablingRPC.h"
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// CABLING SETUP ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using namespace RPCcabling;

CablingRPCBase* RPCcabling::CablingRPC::s_instance = 0;
bool RPCcabling::CablingRPC::s_status = false;
bool RPCcabling::CablingRPC::s_cosmic_configuration = false;
std::string RPCcabling::CablingRPC::ConfName = "LVL1conf.data";
std::string RPCcabling::CablingRPC::CorrName = "LVL1conf.corr";
std::string RPCcabling::CablingRPC::DataName = "ATLAS.121108";

CablingRPC::CablingRPC() : CablingRPCBase(), m_Version(""),m_MaxType(0)
{
    for(int i=0;i<64;++i) m_SectorMap[i] = 0;
    m_TestbeamFlag = false;
    s_status = ReadConf(ConfName);
    ReadCorr(CorrName);
#ifdef LVL1_STANDALONE
    if (s_status) std::cout << *this;
#endif
}

CablingRPC::~CablingRPC()
{
    m_SectorLogic.clear();
    m_SectorType.clear();
    m_RDOs.clear();

    s_instance = 0;
}


const CablingRPCBase*
CablingRPC::instance(void)
{    
    if (! s_instance) {
        s_instance = new CablingRPC();
    }
    
    if(!s_status)
    {
        std::cout << "RPC cabling model is not loaded!" << std::endl;
	delete s_instance;
	s_instance = 0;
    }
    
    return s_instance;    
}

const CablingRPCBase*
CablingRPC::instance(std::string conf, std::string corr, 
                     std::string data, bool cosmic)
{    
    if (! s_instance) {
        s_cosmic_configuration = cosmic;
        ConfName = conf;
        CorrName = corr;
	DataName = data;

        s_instance = new CablingRPC();
    }
    
    if(!s_status)
    {
        std::cout << "RPC cabling model is not loaded!" << std::endl;
	delete s_instance;
	s_instance = 0;
    }
    
    return s_instance;    
}


void
CablingRPC::delete_instance()
{    
    delete s_instance;
    s_instance = 0;
    s_status = 0;    
}



bool
CablingRPC::ReadConf(std::string file)
{   
     // Open the input file
    std::ifstream LVL1conf(file.c_str());

    //std::cout << "Il file da leggere e` " << file << std::endl;

    if (!LVL1conf)
    {
        DISP << "Cannot open " << file << "!" << std::endl;
        DISP_ERROR;
        return false;
    }

    // Create the reader for the database
    DBline data(LVL1conf);

    // store the setup environment    
    std::string version,setup,layout;
    
    // Start reading routine
    while (++data)
    {	
        data("Version") >> version >> setup >> layout;

        if (setup == "Testbeam") m_TestbeamFlag = true;

        // Read the Sector Type enumeration
        int start = 0;
        int stop  = 0;
        if(data("SECTOR TYPES (") >> start >> "-" >> stop >> "):")
            for(int i=start;i<=stop;++i) data >> m_SectorMap[i];

        // Set the m_MaxType variable and the type of SectorMap objects
        if(stop == 63 || stop == 8)
        {   
            for(int i=0;i<64;++i)
                if(m_SectorMap[i] > m_MaxType) m_MaxType = m_SectorMap[i];

            m_SectorType.resize(m_MaxType);
 
            for(int i=1;i<=m_MaxType;++i)
	    { 
                m_SectorType[i-1] =
		    SectorLogicSetup(i,DataName,layout,s_cosmic_configuration);
	        SectorLogicSetup* sec = &(m_SectorType[i-1]);
                for(int j=0;j<64;++j) 
                    if(m_SectorMap[j] == i)
		    { 
                        *sec << j;
		        m_SectorLogic.insert(SLmap::value_type(j,sec));
		    }
	     }
        }        
	
        // Loop on GEOMETRY TYPES
        for(int i=1;i<=m_MaxType;++i)
        {	    
            // Read the RPC geometry
            if(data("RPC GEOM  # :",i))
            {
	        RPCchamberdata RPCdata(data,i);
                if(!(m_SectorType[i-1] += RPCdata)) return false;
            }
            // Read the Wired OR geometry
            if(data("WIRED OR  # :",i))
            {
	        WiredORdata WORdata(data,i);
                if(!(m_SectorType[i-1] += WORdata)) return false;
	    }
            // Read the CMAs segmentation
            if(data("CMAs  # : pivot segmentation",i))
	    {
                CMApivotdata CMAdata(data,i,layout);
                if(!(m_SectorType[i-1] += CMAdata)) return false;
	    }
            // Read the CMAs cabling
            if(data("CMAs  # : eta cabling",i))
	    {
                CMAcablingdata CMAdata(data,i);
                if(!(m_SectorType[i-1] += CMAdata)) return false;
	    }
        }
    }
    
    m_Version += version + " " + setup + " " + layout;
    
    LVL1conf.close();
    
    // Setup of the LVL-1 configuration
    for(int i=1;i<=m_MaxType;++i) if(!m_SectorType[i-1].setup()) return false;
    for(int i=1;i<=m_MaxType;++i) if(!m_SectorType[i-1].check()) return false; 

    // int secNumber = (setup=="Atlas")? 64 : 1;

    // build and init the map for the PAD RDO
    return buildRDOmap();  
}


bool
CablingRPC::ReadCorr(std::string file)
{   
     // Open the input file
    std::ifstream LVL1corr(file.c_str());

    //std::cout << "Il file da leggere e` " << file << std::endl;

    if (!LVL1corr)
    {
        DISP << "Cannot open " << file << "!" << std::endl;
        DISP_WARNING;
        return false;
    }

    // Create the reader for the database
    DBline data(LVL1corr);

    
    // Start reading routine
    while (++data)
    {
        unsigned short int SubId;
	unsigned short int SecId;
	unsigned short int PADId;
	unsigned short int CMAId;
	unsigned short int ijk;
	unsigned short int inputType;
	unsigned int       layer;
	unsigned int       type;
	unsigned short int Channel1;
	unsigned short int Channel2;
	int Number;
	
        if(data("CABLE") >> hex() >> SubId >> SecId >> PADId >> CMAId >> ijk
	                 >> dec() >> type >> Channel1 >> Channel2 >> Number)
	{
	    bool ok = CableParamCheck(SubId,SecId,PADId,CMAId,ijk,type,Channel1,
	                    Channel2,Number);
	    
	    layer = (ijk == 0 || ijk == 2 || ijk == 3)? 0 : 1;
	    
	    if (ijk > 1) {
                Channel1 += 32*(ijk%2);
	        Channel2 += 32*(ijk%2);
            }
	    
	    unsigned short int lh  = (CMAId >> 2) & 1;
	    
	    if(ijk == 0 || ijk == 1) inputType = 1;
	    else                     inputType = (lh)? 2 : 0;
	    
	    if (ijk  > 5) inputType = 3;
	    if (type > 3) type = 4; 
	    
	    if(ok && ! correct (SubId,SecId,PADId,CMAId,
	                         static_cast<CMAinput>(inputType),layer,
	                         Channel1,Channel2,Number,
	                         static_cast<L1RPCcabCorrection>(type)))
	    {
	        DISP << "Cannot apply correction: ";
	        DISP << std::hex << std::setw(4) << std::showbase << SubId << " "
		     << std::setw(4) << std::showbase << SecId << " "
		     << std::setw(3) << std::showbase << PADId << " "
		     << std::setw(3) << std::showbase << CMAId << " "
		     << std::setw(3) << std::showbase << ijk << "  "
		     << std::dec << std::setw(1) << std::showbase << type << " "
		     << std::setw(2) << std::showbase << Channel1 << " "
		     << std::setw(2) << std::showbase << Channel2 << " "
		     << std::setw(2) << std::showbase << Number << " "
		     << std::endl;
		DISP_WARNING;
	    }
	    
	}        
	
	
	if(data("BOARD") >> hex() >> SubId >> SecId >> PADId >> CMAId >> dec()
	                 >> inputType  >> layer >> type 
			 >> Channel1 >> Channel2 >> Number)
	{
	    bool ok = BoardParamCheck(SubId,SecId,PADId,CMAId,inputType,layer,
	                              type,Channel1,Channel2,Number);	
	    
	    if (inputType  > 2) inputType = 3;
	    if (type > 3)       type = 4;
	    
	    if(ok && ! correct (SubId,SecId,PADId,CMAId,
	                         static_cast<CMAinput>(inputType),layer,
	                         Channel1,Channel2,Number,
	                         static_cast<L1RPCcabCorrection>(type)))
	    {
	        DISP << "Cannot apply correction: ";
	        DISP << std::hex << std::setw(4) << std::showbase << SubId << " "
		     << std::setw(4) << std::showbase << SecId << " "
		     << std::setw(3) << std::showbase << PADId << " "
		     << std::setw(3) << std::showbase << CMAId << "  "
		     << std::dec << std::setw(1) << inputType << " "
		     << std::setw(1) << layer << " "
		     << std::dec << std::setw(1) << std::showbase << type << " "
		     << std::setw(2) << std::showbase << Channel1 << " "
		     << std::setw(2) << std::showbase << Channel2 << " "
		     << std::setw(2) << std::showbase << Number << " "
		     << std::endl;
		DISP_WARNING;
	    }
	    
	}
    }
    
    return true;
}

bool
CablingRPC::CableParamCheck(unsigned short int SubId,
	                    unsigned short int SecId,
	                    unsigned short int PADId,
	                    unsigned short int CMAId,
	                    unsigned short int ijk,
	                    unsigned int       type,
	                    unsigned short int Channel1,
	                    unsigned short int Channel2,
	                    short int Number) const
{
    if(SubId != 0x65 && SubId != 0x66) {
        DISP << "Subsystem Id out of range [0x65,0x66].";
        DISP_ERROR;
	return false;
    }
    if(SecId > 31) {
        DISP << "Sector Id out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(PADId > 8) {
        DISP << "Pad Id out of range [0/8].";
        DISP_ERROR;
	return false;
    }
    if(CMAId > 7) {
        DISP << "CMA Id out of range [0/7].";
        DISP_ERROR;
	return false;
    }
    if(ijk > 5) {
        DISP << "ijk out of range [0/5].";
        DISP_ERROR;
	return false;
    }
    if(type > 3) {
        DISP << "correction type out of range [0/3].";
        DISP_ERROR;
	return false;
    }
    if(Channel1 > 31) {
        DISP << "ijk Channel 1 out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(Channel2 > 31) {
        DISP << "ijk Channel 2 out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(Number > 32) {
        DISP << "Number out of range [0/32].";
        DISP_ERROR;
	return false;
    }
    return true;
}

bool
CablingRPC::BoardParamCheck(unsigned short int SubId,
	                    unsigned short int SecId,
	                    unsigned short int PADId,
	                    unsigned short int CMAId,
	                    unsigned short int inputType,
			    unsigned int       layer,
	                    unsigned int       type,
	                    unsigned short int Channel1,
	                    unsigned short int Channel2,
	                    short int Number) const
{
    if(SubId != 0x65 && SubId != 0x66) {
        DISP << "Subsystem Id out of range [0x65,0x66].";
        DISP_ERROR;
	return false;
    }
    if(SecId > 31) {
        DISP << "Sector Id out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(PADId > 8) {
        DISP << "Pad Id out of range [0/8].";
        DISP_ERROR;
	return false;
    }
    if(CMAId > 7) {
        DISP << "CMA Id out of range [0/7].";
        DISP_ERROR;
	return false;
    }
    if(inputType > 2) {
        DISP << "Board input out of range [0/2].";
        DISP_ERROR;
	return false;
    }
    if(layer > 1) {
        DISP << "Board layer out of range [0/1].";
        DISP_ERROR;
	return false;
    }
    if(type > 3) {
        DISP << "correction type out of range [0/3].";
        DISP_ERROR;
	return false;
    }
    if(Channel1 > 63) {
        DISP << "ijk Channel 1 out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(Channel2 > 63) {
        DISP << "ijk Channel 2 out of range [0/31].";
        DISP_ERROR;
	return false;
    }
    if(Number > 64) {
        DISP << "Number out of range [0/64].";
        DISP_ERROR;
	return false;
    }
    return true;
}

bool
CablingRPC::buildRDOmap()
{
    for (int sector = 0; sector < 64; ++sector)
    {
        int side = (sector<32)? 0 : 1;
        int logic_sector = sector % 32;

        if (m_SectorMap[sector]) {
        
	// get the Sector Logic Setup
        SectorLogicSetup Sector = m_SectorType[m_SectorMap[sector] - 1];
        
        // get the Eta CMA map from the Sector Logic Setup
        const SectorLogicSetup::EtaCMAmap CMAs = Sector.giveEtaCMA();
        SectorLogicSetup::EtaCMAmap::const_iterator it = CMAs.begin();
        
	bool isFirst = false;

	// loop over the whole set of Eta CMAs
        while(it != CMAs.end())
	{
	    // get the set of parameters for idenfying the first RPC strip
	    unsigned int ID = (*it).second.id().Ixx_index();
	    bool inversion  = (*it).second.inversion();
	    if ((ID==1 && inversion) || (ID==0 && !inversion)) isFirst = true;
	    else isFirst = false;
	    unsigned int PADid = (*it).first.PAD_index();
	    unsigned int cabling;
            if( (*it).second.get_cabling(Pivot,0,0,0,cabling) ) //return false;
	    {
	        unsigned int RPC_strip = cabling%100;
	        unsigned int RPC_chamber = (cabling/100)%100;
	        unsigned int RPC_layer = 0;
                unsigned int RPC_station = (*it).second.whichCMAstation(Pivot);
                unsigned int lvl1_sector = sector;

                RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
                std::string name = rpc->stationName();
	        int sEta = (side)? rpc->stationEta() : -rpc->stationEta();
	        int sPhi = (logic_sector==31)? 1 : (logic_sector+1)/4 +1;
                int dR   = rpc->doubletR();
                int dZ   = rpc->doubletZ();
	        int dP   = (rpc->phiReadoutPannels()==2)? (logic_sector+1)%2+1: 1;

                // build the Level-1 code index
                RPCdecoder decode(Eta,lvl1_sector,RPC_station,RPC_layer,
                                  RPC_chamber,RPC_strip);

                // instanciate the corresponding RDO index
                //RDOindex rdo(PADid,decode.code());
	        RDOindex rdo(PADid,decode.code(),name,sEta,sPhi,dR,dZ,dP);
	    
                // compute the key for retrieving RDO into the map
                int key = side*10000 + logic_sector*100 + PADid;
                // insert the RDO into the map
                std::pair < RDOmap::iterator, bool> ins = 
                    m_RDOs.insert(RDOmap::value_type(key,rdo));
		//std::cout << "Normal Matrix: key=" << key << ",  rdo=" << rdo
		//          << std::endl;    
                if(!ins.second)
                {
	            DISP << "Error in inserting RDO " << rdo;
                    DISP_ERROR;
                    return ins.second;
                }
	    
	        //DISP << rdo;
	        //DISP_DEBUG;
            }
	    else if(!(*it).second.get_cabling(Pivot,0,0,0,cabling) && 
	              isFirst)
            {
		// try to catch configrm cabling
		bool existLow  = false;
		bool existHigh = false;
		
		if( !(existLow=(*it).second.get_cabling(LowPt,0,0,0,cabling)) )
		   existHigh = (*it).second.get_cabling(HighPt,0,0,0,cabling);
		  
		if (!existLow && !existHigh)
		{
		    DISP << "Error while configuring the RDO map";
                    DISP_ERROR;
                    return false;
		}
		
		unsigned int RPC_strip = cabling%100;
	        unsigned int RPC_chamber = (cabling/100)%100;
	        unsigned int RPC_layer = 0;
		unsigned int RPC_station = 0;
		if(existLow) RPC_station = (*it).second.whichCMAstation(LowPt);
		else         RPC_station = (*it).second.whichCMAstation(HighPt);
                unsigned int lvl1_sector = sector;

                RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
                std::string name = rpc->stationName();
	        int sEta = (side)? rpc->stationEta() : -rpc->stationEta();
	        int sPhi = (logic_sector==31)? 1 : (logic_sector+1)/4 +1;
                int dR   = rpc->doubletR();
                int dZ   = rpc->doubletZ();
	        int dP   = (rpc->phiReadoutPannels()==2)? (logic_sector+1)%2+1: 1;

                // build the Level-1 code index
                RPCdecoder decode(Eta,lvl1_sector,RPC_station,RPC_layer,
                                  RPC_chamber,RPC_strip);

                // instanciate the corresponding RDO index
	        RDOindex rdo(PADid,decode.code(),name,sEta,sPhi,dR,dZ,dP);
	    
                // compute the key for retrieving RDO into the map
                int key = side*10000 + logic_sector*100 + PADid;
                // insert the RDO into the map
                std::pair < RDOmap::iterator, bool> ins = 
                    m_RDOs.insert(RDOmap::value_type(key,rdo));
		    
		//std::cout << "Dummy Matrix: key=" << key << ",  rdo=" << rdo
		//          << std::endl;
                if(!ins.second)
                {
	            DISP << "Error in inserting RDO " << rdo;
                    DISP_ERROR;
                    return ins.second;
                }
	    }

            // increase the iterator            
            while( it != CMAs.end() &&
                   (unsigned int)(*it).first.PAD_index() == PADid ) 
	      {++it;}	    
	    
	}
	
	}
    }

    RDOmap::iterator pad_beg = m_RDOs.begin();
    RDOmap::iterator pad_end = m_RDOs.end();

    int hashID=0;

    for (; pad_beg!=pad_end; ++pad_beg)
    {
        (*pad_beg).second.set_hash(hashID);
        ++hashID;
    }

    return true;
}

bool
CablingRPC::give_global_strip_address(unsigned int code, int& address) const
{
    RPCdecoder decode;
    if(decode(code))
    {    
        int strip        = decode.strip_number();
        int z_index      = decode.rpc_z_index();
        int station      = decode.lvl1_station();
        int logic_sector = decode.logic_sector();
	ViewType side    = decode.view();
        HalfType half    = decode.half_barrel();

        const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector]-1];
        return s.global_strip_add(side,half,station,z_index,strip,address);
    }
    return false;
}

bool
CablingRPC::give_global_connector_address(unsigned int code, int& address, 
int& low_eta_strips, int& hi_eta_strips) const
{    
    RPCdecoder decode;
    if(decode(code))
    {    
        int strip        = decode.strip_number();
        int z_index      = decode.rpc_z_index();
        int station      = decode.lvl1_station();
        int logic_sector = decode.logic_sector();
	ViewType side    = decode.view();
        HalfType half    = decode.half_barrel();

        const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector]-1];

        return s.global_conn_add(side,half,station,z_index,strip,address,
                                  low_eta_strips,hi_eta_strips);
    }
    return false;
}

bool
CablingRPC::give_global_address(unsigned int code,int& global_strip_address,
int& global_connector_address,int& low_eta_strips,int& hi_eta_strips) const
{
    bool result = true;
    result &=this->give_global_strip_address(code,global_strip_address);
    result &=this->give_global_connector_address(code,global_connector_address,
                                                 low_eta_strips,hi_eta_strips);
    return result;
}

bool
CablingRPC::gstrip_2_connector(ViewType side, int logic_sector,
int lvl1_station,int global_strip_address,int& global_connector_address,
int& local_connector_address,int& rpc_index,int& strip_number, 
int& low_eta_strips,int& hi_eta_strips) const
{
    bool result = false;
    if(this->give_local_strip_address(side,logic_sector,lvl1_station,
                         global_strip_address,rpc_index,strip_number))
    {
        RPCdecoder decode;
        if(decode(side,logic_sector,lvl1_station,0,rpc_index,strip_number))
	{
	    result = true;
            result &= this->give_global_connector_address(decode.code(),
                      global_connector_address,low_eta_strips,hi_eta_strips);
            result &= this->give_local_connector_address(side,logic_sector,
			    lvl1_station,global_connector_address,
                            local_connector_address,rpc_index,strip_number);
        }
    }
    return result;
}


bool
CablingRPC::give_local_strip_address(ViewType side, int logic_sector, 
int station, int global_address,int& rpc_index, int& strip_number) const
{
    RPCdecoder decode;
    if(!decode.OK(logic_sector,RPCdecoder::Logic_Sector)) return false;
    if(!decode.OK(station,RPCdecoder::LVL1_Station)) return false;
    if(logic_sector < 32 && global_address > 0) return false;
    if(logic_sector >= 32 && global_address < 0) return false; 

    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
    return
      s.local_strip_add(side,station,global_address,rpc_index,strip_number);
}

bool
CablingRPC::give_local_connector_address(ViewType side, int logic_sector, 
int station, int global_address,int& local_address, int& rpc_index, 
int& strip_number) const
{
    RPCdecoder decode;
    if(!decode.OK(logic_sector,RPCdecoder::Logic_Sector)) return false;
    if(!decode.OK(station,RPCdecoder::LVL1_Station)) return false;
    if(logic_sector < 32 && global_address > 0) return false;
    if(logic_sector >= 32 && global_address < 0) return false; 

    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
    return s.local_conn_add(side,station,global_address,local_address,
                             rpc_index,strip_number);
}


const CMAparameters::CMAlist 
CablingRPC::give_CMAs(const int logic_sector,const ViewType side,
                     const int station,const int cabling_code) const
{
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
    CMAparameters::CMAlist 
        result = s.give_CMAs(logic_sector,side,station,cabling_code);
    return result;
}


bool 
CablingRPC::give_RoI_borders  (unsigned short int SubsystemId,
                               unsigned short int SectorId,
			       unsigned short int RoIId,
			       unsigned int& EtaLowBorder,
			       unsigned int& EtaHighBorder,
			       unsigned int& PhiLowBorder,
			       unsigned int& PhiHighBorder) const
{
    int logic_sector = SectorId + SubsystemId*32;
    int PadId  = (RoIId)/4;
    int PadRoI = (RoIId)%4;
    int EtaIxx = PadRoI%2;
    int PhiIxx = PadRoI/2;
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
   
//    const RPCchamber* cham = s.find_chamber(2,1);
    //if(cham->inversion(logic_sector)) EtaIxx = (EtaIxx + 1)%2;

    //if(PadId==5 && SectorId==24 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==13 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==0 && SectorId==31 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==10 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
   
    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
   
    if(!s.give_RoI_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder))
			    
    {
        DISP << "Side="  << SubsystemId << ", Sector=" << SectorId
	     << ", RoI=" << RoIId;
	DISP_DEBUG;
        DISP << "  Cannot retrieve the borders for the CMAs in PAD " << PadId
	     << ", Eta Ixx = " << EtaIxx << ", Phi Ixx = " << PhiIxx;
	DISP_DEBUG;
	return false;
    }
    
    EtaLowBorder  += logic_sector*1000000;
    EtaHighBorder += logic_sector*1000000;
    PhiLowBorder  += logic_sector*1000000;
    PhiHighBorder += logic_sector*1000000;

    return true;
}

bool 
CablingRPC::give_LowPt_borders (unsigned short int SubsystemId,
                                unsigned short int SectorId,
			        unsigned short int RoIId,
			        unsigned int& EtaLowBorder,
			        unsigned int& EtaHighBorder,
			        unsigned int& PhiLowBorder,
			        unsigned int& PhiHighBorder) const
{
    int logic_sector = SectorId + SubsystemId*32;
    int PadId  = (RoIId)/4;
    int PadRoI = (RoIId)%4;
    int EtaIxx = PadRoI%2;
    int PhiIxx = PadRoI/2;
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
    
//    const RPCchamber* cham = s.find_chamber(2,1);
    //if(cham->inversion(logic_sector)) EtaIxx = (EtaIxx + 1)%2;

    //if(PadId==5 && SectorId==24 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==13 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==0 && SectorId==31 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==10 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
    
    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
    
    if(!s.give_LowPt_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder))
			    
    {
        DISP << "Side="  << SubsystemId << ", Sector=" << SectorId
	     << ", RoI=" << RoIId;
        DISP_DEBUG;
	DISP << "  Cannot retrieve the borders for the CMAs in PAD " << PadId
	     << ", Eta Ixx = " << EtaIxx << ", Phi Ixx = " << PhiIxx;
	DISP_DEBUG;
	return false;
    }
    
    EtaLowBorder  += logic_sector*1000000;
    EtaHighBorder += logic_sector*1000000;
    PhiLowBorder  += logic_sector*1000000;
    PhiHighBorder += logic_sector*1000000;

    return true;
}


bool 
CablingRPC::give_HighPt_borders (unsigned short int SubsystemId,
                                 unsigned short int SectorId,
			         unsigned short int RoIId,
			         unsigned int& EtaLowBorder,
			         unsigned int& EtaHighBorder,
			         unsigned int& PhiLowBorder,
			         unsigned int& PhiHighBorder) const
{
    int logic_sector = SectorId + SubsystemId*32;
    int PadId  = (RoIId)/4;
    int PadRoI = (RoIId)%4;
    int EtaIxx = PadRoI%2;
    int PhiIxx = PadRoI/2;
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;

    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
    
//    const RPCchamber* cham = s.find_chamber(2,1);
    //if(cham->inversion(logic_sector)) EtaIxx = (EtaIxx + 1)%2;
    
    //if(PadId==5 && SectorId==24 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==13 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==0 && SectorId==31 && SubsystemId==0) EtaIxx = (EtaIxx + 1)%2;
    //if(PadId==4 && SectorId==10 && SubsystemId==1) EtaIxx = (EtaIxx + 1)%2;
    
    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
    
    if(!s.give_HighPt_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder))
			    
    {
        DISP << "Side="  << SubsystemId << ", Sector=" << SectorId
	     << ", RoI=" << RoIId;
	DISP_DEBUG;
        DISP << "  Cannot retrieve the borders for the CMAs in PAD " << PadId
	     << ", Eta Ixx = " << EtaIxx << ", Phi Ixx = " << PhiIxx;
	DISP_DEBUG;
	return false;
    }
    
    EtaLowBorder  += logic_sector*1000000;
    EtaHighBorder += logic_sector*1000000;
    PhiLowBorder  += logic_sector*1000000;
    PhiHighBorder += logic_sector*1000000;

    return true;
}			    

//===================================================
/*<S>*/
int CablingRPC::give_doubletZ(unsigned short int SubsystemId,unsigned short int SectorId, int RPCStation, int RPCChamber) const
{
		int logic_sector = SectorId + SubsystemId*32;
		const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
		const RPCchamber* rpc = s.find_chamber(RPCStation, RPCChamber);
		int i_doubletZ = rpc->doubletZ();
		return i_doubletZ;
}

int CablingRPC::give_doubletPhi(int LogicSector) const
{
		int i_sector = LogicSector%32;
		int i_doubletPhi = (i_sector+1)%2+1;

		return i_doubletPhi;
}
/*<S>*/
//============================================



			    
bool 
CablingRPC::give_cma_layout (ViewType side, 
                          unsigned short int logic_sector,
			  unsigned short int PADId,
			  unsigned short int IxxId,
			  unsigned short int LowHi,
			  unsigned short int& start_pivot_ch,
			                 int& start_pivot_st,
			  unsigned short int& stop_pivot_ch,
			                 int& stop_pivot_st,
			  unsigned short int& start_confirm_ch,
			                 int& start_confirm_st,
			  unsigned short int& stop_confirm_ch,
			                 int& stop_confirm_st) const
{    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
        
    CMAidentity ID(Eta,AllSectors,0);
    
    if (side == Eta)
    {
        CMAidentity ETA (Eta,AllSectors,PADId,IxxId);
	ID = ETA;    
    } else
    {
        CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
	CMAidentity PHI(Phi,PhiCov,PADId,IxxId);
	ID = PHI;
    }
    
    unsigned int start_pivot_code = 0;
    unsigned int stop_pivot_code = 0;
    unsigned int start_confirm_code = 0;
    unsigned int stop_confirm_code = 0;
    
    bool sc = false;
    
    if (LowHi) sc = s.give_HighPt_layout(ID,start_pivot_ch,start_pivot_code,
			               stop_pivot_ch, stop_pivot_code,
			               start_confirm_ch, start_confirm_code,
			               stop_confirm_ch, stop_confirm_code);
    else       sc = s.give_LowPt_layout(ID,start_pivot_ch,start_pivot_code,
			               stop_pivot_ch, stop_pivot_code,
			               start_confirm_ch, start_confirm_code,
			               stop_confirm_ch, stop_confirm_code);
    if (!sc) return false;
    
    int gl_conn;
    int low_eta_st;
    int hi_eta_st;
    
    start_pivot_code   += logic_sector*1000000;
    stop_pivot_code    += logic_sector*1000000;
    start_confirm_code += logic_sector*1000000;
    stop_confirm_code  += logic_sector*1000000;
        
    sc  = give_global_address(start_pivot_code,start_pivot_st,gl_conn,
                             low_eta_st,hi_eta_st);
    sc &= give_global_address(stop_pivot_code,stop_pivot_st,gl_conn,
                             low_eta_st,hi_eta_st);			     
    sc &= give_global_address(start_confirm_code,start_confirm_st,gl_conn,
                             low_eta_st,hi_eta_st);
    sc &= give_global_address(stop_confirm_code,stop_confirm_st,gl_conn,
                             low_eta_st,hi_eta_st);  
	      			     
    return sc;		
}

#ifndef LVL1_STANDALONE

bool 
CablingRPC::give_PAD_address (unsigned short int SubsystemId,
                              unsigned short int SectorId,
			      unsigned short int RoIId,
			      unsigned int& logic_sector,
			      unsigned short int& PADId,
			      Identifier& PadId) const
{
    logic_sector = SectorId + SubsystemId*32;
    PADId  = (RoIId)/4;

    int key = SubsystemId*10000 + SectorId*100 + PADId;

#ifndef NDEBUG 
    DISP << "Pad: SubsystemId " << SubsystemId 
         << " Sector " << logic_sector 
         << " Id " << PADId;
    DISP_DEBUG;
#endif

    // checks if the key is in the map
    RDOmap::const_iterator it = m_RDOs.find(key);
    if (it == m_RDOs.end()) return false;
 
    // Retrieve the identifier elements from the map
    const RDOindex index = (*it).second;
    
#ifndef NDEBUG 
    DISP << index; 
    DISP_DEBUG;   
#endif

    // Identifier elements
    //MuonStationName name;
    //int eta = 0;
    //int phi = 0;
    //int doublet_r = 0;
    //int doublet_z = 0;
    //int doublet_phi = 0;
    //int gas_gap = 0;
    //int measures_phi = 0;
    //int strip = 0;

    //index.offline_indexes(name, eta, phi, doublet_r, doublet_z, doublet_phi, 
    //                      gas_gap, measures_phi, strip);

    // Build the pad offline identifier
    //RpcPadId rpc_id = RpcPadId(name, eta, phi, doublet_r, 
    //                           doublet_z, doublet_phi);
    index.pad_identifier(PadId);
    
    //if (!rpc_id.is_valid()) {
    //    DISP << "Invalid pad offline indices "; DISP_ERROR;
    //    DISP << "Name : "    << (int)name; DISP_ERROR;
    //    DISP << "Eta "       << eta       << "   Phi " << phi; DISP_ERROR;
    //    DISP << "Doublet r " << doublet_r << "  Doublet_z " 
    //         << doublet_z << "  Doublet_phi " << doublet_phi;  DISP_ERROR;
    //    DISP << "Gas gap "   << gas_gap   << "   Measures_phi " 
    //         << measures_phi << "  Strip " << strip;  DISP_ERROR;

	//return false;
    //}

    //PadId = rpc_id;

    return true;
}


bool 
CablingRPC::give_PAD_address (unsigned short int SubsystemId,
                              unsigned short int SectorId,
			      unsigned short int RoIId,
			      unsigned int& logic_sector,
			      unsigned short int& PADId,
			      unsigned int& PadId) const
{
    logic_sector = SectorId + SubsystemId*32;
    PADId  = (RoIId)/4;

    int key = SubsystemId*10000 + SectorId*100 + PADId;

#ifndef NDEBUG 
    DISP << "Pad: SubsystemId " << SubsystemId 
         << " Sector " << logic_sector 
         << " Id " << PADId;
    DISP_DEBUG;
#endif

    // checks if the key is in the map
    RDOmap::const_iterator it = m_RDOs.find(key);
    if (it == m_RDOs.end()) return false;
 
    // Retrieve the identifier elements from the map
    const RDOindex index = (*it).second;
    
#ifndef NDEBUG 
    DISP << index; 
    DISP_DEBUG;   
#endif

    PadId = index.hash();

    return true;
}


#endif


const CMAparameters*
CablingRPC::give_CMA(unsigned short int SubsystemId,
                     unsigned short int SectorId,
	             unsigned short int PADId,
                     unsigned short int CMAId ) const
{    
    int logic_sector = SectorId + SubsystemId*32;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep  = (CMAId >> 1) & 1;
    
    ep = (ep==1)? 0 : 1;
    
    // retrieve the Sector Logic setup
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];  

    //retrieve the CMAparameters associated to the identifiers
    if(ep)
    {   
        CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
	CMAidentity PHI(Phi,PhiCov,PADId,Ixx);
	return s.give_CMA(PHI);
    }
    else
    {
	CMAidentity ETA(Eta,AllSectors,PADId,Ixx);
        return s.give_CMA(ETA);
    }
}

bool
CablingRPC::correct (unsigned short int SubsystemId,
                     unsigned short int SectorId,
	             unsigned short int PADId,
		     unsigned short int CMAId,
		     CMAinput           it,
		     unsigned int       layer,
		     unsigned short int Channel1,
		     unsigned short int Channel2,
		     short int number,
		     L1RPCcabCorrection type)
{
    int logic_sector = (SubsystemId==0x65)? SectorId + 32 : SectorId;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep  = (CMAId >> 1) & 1;
    //unsigned short int lh  = (CMAId >> 2) & 1;
    
    ep = (ep==1)? 0 : 1;
    
    //DISP << "Logic sector = " << logic_sector << std::endl;
    //DISP_DEBUG

    // retrieve the Sector Logic setup
    SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];  

    //retrieve the CMAparameters associated to the identifiers
    if(ep)
    {   
        CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
	CMAidentity PHI(Phi,PhiCov,PADId,Ixx);
	return s.correct(PHI,type,it,layer,Channel1,Channel2,number);
    }
    else
    {
	CMAidentity ETA(Eta,AllSectors,PADId,Ixx);
        return s.correct(ETA,type,it,layer,Channel1,Channel2,number);
    }

    return false;
}

std::list<unsigned int>
CablingRPC::give_strip_code (unsigned short int SubsystemId,
                             unsigned short int SectorId,
			     unsigned short int PADId,
			     unsigned short int CMAId,
			     unsigned short ijk,
			     unsigned short int Channel) const
{
    std::list<unsigned int> CodeList;

    int logic_sector = SectorId + SubsystemId*32;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep  = (CMAId >> 1) & 1;
    unsigned short int lh  = (CMAId >> 2) & 1;
    
    ep = (ep==1)? 0 : 1;
    
    /*
    DISP << "Pad: SubsystemId " << SubsystemId 
         << " Sector " << logic_sector 
         << " Id " << PADId
	 << " CMA " << CMAId
	 << " ijk " << ijk
	 << " Channel " << Channel;
    DISP_DEBUG;
    */

    // retrieve the Sector Logic setup
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];  

    //retrieve the CMAparameters associated to the identifiers
    if(ep)
    {   
        CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
	CMAidentity PHI(Phi,PhiCov,PADId,Ixx);
	CodeList = s.give_strip_code(PHI,logic_sector,lh,ijk,Channel);
    }
    else
    {
	CMAidentity ETA(Eta,AllSectors,PADId,Ixx);
        CodeList = s.give_strip_code(ETA,logic_sector,lh,ijk,Channel);
    }

    return CodeList;
}

std::list<RPCofflineId>
CablingRPC::give_strip_id   (unsigned short int SubsystemId,
                             unsigned short int SectorId,
			     unsigned short int PADId,
			     unsigned short int CMAId,
			     unsigned short ijk,
			     unsigned short int Channel) const
{
    std::list<unsigned int> CodeList;

    int logic_sector = SectorId + SubsystemId*32;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep  = (CMAId >> 1) & 1;
    unsigned short int lh  = (CMAId >> 2) & 1;
    
    ep = (ep==1)? 0 : 1;

    // retrieve the Sector Logic setup
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];  

    //retrieve the CMAparameters associated to the identifiers
    if(ep)
    {   
        CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
	CMAidentity PHI(Phi,PhiCov,PADId,Ixx);
	CodeList = s.give_strip_code(PHI,logic_sector,lh,ijk,Channel);
    }
    else
    {
	CMAidentity ETA(Eta,AllSectors,PADId,Ixx);
        CodeList = s.give_strip_code(ETA,logic_sector,lh,ijk,Channel);
    }

    
    std::list <RPCofflineId> offlineIdList;
    std::list<unsigned int>::const_iterator it = CodeList.begin();
    while(it != CodeList.end())
    {
        RPCdecoder decode(*it);
	if(!decode) {
	    DISP << "Give_strip code error: cannot decode the following LVL1 Id"
	         << " " << decode.code() << std::endl;
            DISP_ERROR;
	} else {
	    RPCofflineId rpcId;
	    
	    int RPC_strip    = decode.strip_number();
	    int RPC_chamber  = decode.rpc_z_index();
	    int RPC_layer    = decode.rpc_layer();
            int RPC_station  = decode.lvl1_station();
            int sector       = (decode.logic_sector())%32;

            const RPCchamber* rpc = s.find_chamber(RPC_station, RPC_chamber);
	    
            rpcId.stationName = rpc->stationName();
	    rpcId.stationEta  = (decode.half_barrel())?  rpc->stationEta() : 
	                                                -rpc->stationEta();
	    rpcId.stationPhi  = (sector==31)? 1 : (sector+1)/4 +1;
            rpcId.doubletR    = rpc->doubletR();
            rpcId.doubletZ    = rpc->doubletZ();
	    rpcId.doubletPhi  = (rpc->phiReadoutPannels()==2)? (sector+1)%2+1: 1;
            rpcId.gasGap      = RPC_layer + 1;
	    rpcId.measuresPhi = static_cast<int>(decode.view());
	    rpcId.strip       = RPC_strip + 1;
	    
	    offlineIdList.push_back(rpcId);

	}
	
	++it;
    }
    
    return offlineIdList;
}
			    
void
CablingRPC::PrintMap(std::ostream& stream) const
{
    stream << std::endl << "Level-1 configuration database version ";
    stream << m_Version << " read." << std::endl;
    stream << "Contains " << m_MaxType << " Trigger Sector Types:"; 
    stream << std::endl;

    stream << "negative sectors  0 - 15 ==> ";
    for(int i=0;i<16;i++) stream << std::setw(2) << m_SectorMap[i] <<" ";
    stream << std::endl << "negative sectors 16 - 31 ==> ";
    for(int i=16;i<32;i++) stream << std::setw(2) << m_SectorMap[i] <<" ";
    stream << std::endl << "positive sectors 32 - 47 ==> ";       
    for(int i=32;i<48;i++) stream << std::setw(2) << m_SectorMap[i] <<" ";
    stream << std::endl << "positive sectors 48 - 63 ==> ";
    for(int i=48;i<64;i++) stream << std::setw(2) << m_SectorMap[i] <<" ";
    stream << std::endl;
}

void
CablingRPC::PrintType(std::ostream& stream,int type,int station,std::string element,
                     int obj,bool detail) const
{
    if(detail) PrintMap(stream);
    if(element == "Conf") return;

    if (type < 0 && type > m_MaxType)
    {
        stream << "Sector type n. " << type 
	       << "doesn't apply to any logic sector!" << std::endl;
    }
    else if (type == 0)
    {
        for(int i = 1; i <= m_MaxType; ++i)
	{
            const SectorLogicSetup& sec = m_SectorType[i - 1];
            sec.PrintElement(stream,station,element,obj,detail);
            stream << std::endl;
        }
    }
    else
    {
        const SectorLogicSetup& sec = m_SectorType[type - 1];
        sec.PrintElement(stream,station,element,obj,detail);
        stream << std::endl;
    }
}

void
CablingRPC::PrintSector(std::ostream& stream,int sector,int station,
                       std::string element,int obj,bool detail) const
{
    if(detail) PrintMap(stream);
    if (sector < 0 && sector > 63)
    {
        stream << "Sector n. " << sector << "doesn't exist!" << std::endl;
    }
    else
    {
        int type = m_SectorMap[sector];
        const SectorLogicSetup& sec = m_SectorType[type - 1];
        sec.PrintElement(stream,station,element,obj,detail);
        stream << std::endl;
    }
}


#ifdef LVL1_STANDALONE

ShowCabling::ShowCabling(int type,int station,std::string element,int obj,bool detail):
m_type(type),m_station(station),m_obj(obj),m_element(element),m_detail(detail) {}

#endif
