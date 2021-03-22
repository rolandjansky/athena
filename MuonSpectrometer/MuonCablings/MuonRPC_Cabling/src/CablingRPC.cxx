/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRPC_Cabling/CablingRPC.h"

#include <cmath>
#include <TString.h> // for Form

namespace {
    // const map between RPC stationName and integer for cabling code
    const static std::map<std::string, int> rpcStats = {
        std::make_pair<std::string, int>("BML", 0),
        std::make_pair<std::string, int>("BMS", 1),
        std::make_pair<std::string, int>("BOL", 2),
        std::make_pair<std::string, int>("BOS", 3),
        std::make_pair<std::string, int>("BMF", 4),
        std::make_pair<std::string, int>("BOF", 5),
        std::make_pair<std::string, int>("BOG", 6),
        std::make_pair<std::string, int>("BME", 7),
        std::make_pair<std::string, int>("BIS", 8),
        std::make_pair<std::string, int>("BIL", 9),
        std::make_pair<std::string, int>("BIM", 10),
        std::make_pair<std::string, int>("BIR", 11)
    };
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// CABLING SETUP ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using namespace MuonRPC_Cabling;

CablingRPCBase* MuonRPC_Cabling::CablingRPC::s_instance = 0;
bool MuonRPC_Cabling::CablingRPC::s_status = false;
bool MuonRPC_Cabling::CablingRPC::s_cosmic_configuration = false;
bool MuonRPC_Cabling::CablingRPC::s_RPCMapfromCool = true;
std::string MuonRPC_Cabling::CablingRPC::ConfName = "";
std::string MuonRPC_Cabling::CablingRPC::CorrName = "";
std::string MuonRPC_Cabling::CablingRPC::DataName = "ATLAS.121108";
const std::string* MuonRPC_Cabling::CablingRPC::CorrMapPString = 0;
const std::string* MuonRPC_Cabling::CablingRPC::ConfMapPString = 0;
const std::map<std::string, std::string>* MuonRPC_Cabling::CablingRPC::p_trigroads = 0;


CablingRPC::CablingRPC() : CablingRPCBase(), m_Version(""),m_MaxType(0)
{
    DISP <<"CablingRPC---singleton constructor ---- this must be executed just once";
    DISP_INFO;
    for(int i=0;i<64;++i) m_SectorMap[i] = 0;
    m_TestbeamFlag = false;
    if(s_RPCMapfromCool){
	DISP <<"CablingRPC---The singleton will fill the maps from the COOL streams";
        DISP_INFO;
	initMapsFromCOOL();
    }
    else 
    {
	DISP <<"CablingRPC---The singleton will fill the maps from files";
        DISP_INFO;
	initMapsFromASCII();
    }

  for (int i1=0; i1<(int)rpcStats.size(); ++i1)
      for (int i2=0; i2<2; ++i2)
	  for (int i3=0; i3<9; ++i3)
	      for (int i4=0; i4<8; ++i4)
		  for (int i5=0; i5<2; ++i5)
		      for (int i6=0; i6<4; ++i6)
			  m_absZindexInThelayerOfThisChamber[i1][i2][i3][i4][i5][i6]=9999;


    DISP <<"CablingRPC---The singleton is created here"; DISP_INFO;
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

void CablingRPC::initMapsFromCOOL()
{
    if (ConfMapPString != 0) 	
    { // read the map only if the string has been already read from COOL 
	DISP <<"CablingRPC---InitMaps from COOL: going to read configuration"; DISP_INFO;
        s_status = ReadConf(ConfMapPString);
        DISP <<"CablingRPC---InitMaps from COOL: going to read corrections to configuration";  DISP_INFO;
        ReadCorr(CorrMapPString);
	DISP <<"CablingRPC---InitMaps from COOL - maps have been parsed"; DISP_INFO;
	if(!s_status)
	{
	    DISP << "CablingRPC---initMapsFromCOOL:: pointers to confMap/corrMaps "
                 <<ConfMapPString<<" and "<<CorrMapPString
                 <<"\n  RPC cabling model is not loaded!"; DISP_INFO;
	    delete s_instance;
	    s_instance = 0;
	}
    }
    else
    {
        DISP <<"CablingRPC---InitMaps from COOL cannot be executed NOW: empty string"; DISP_INFO;
    }
    
}

void CablingRPC::initMapsFromASCII()
{
    if (ConfName != "") 	
    { // read the map only if the file name has been passed by the cabling svc 
        s_status = ReadConf(ConfName);
        ReadCorr(CorrName);
	DISP <<"CablingRPC---InitMaps from ASCII executed"; DISP_INFO;
	if(!s_status)
	{
	    DISP  << "CablingRPC---initMapsFromASCII:: file names "<<ConfName<<" and "<<CorrName
                  <<"\n  RPC cabling model is not loaded!" ; DISP_INFO;
	    delete s_instance;
	    s_instance = 0;
	}
    }	
    else
    {
        DISP <<"CablingRPC---InitMaps from ASCII cannot be executed NOW: empty filenames"; DISP_INFO;
    }
}



CablingRPCBase*
CablingRPC::instance(void)
{    
    if (! s_instance) {
        s_instance = new CablingRPC();
    }
    
    if(!s_status)
    {
	if(s_RPCMapfromCool)
	{
	  //keep this comment for debugging purposes
	  //std::cout <<"CablingRPC---The singleton is expected to fill its maps from the COOL streams but the folders have not been accessed yet" << std::endl;
	    return s_instance;
	}
	else 
	{
	  //keep this comment for debugging purposes
	  //std::cout <<"CablingRPC---The singleton is expected to fill its maps from ASCII files - file names have not been set yet by the Cabling Svc" << std::endl;
	    return s_instance;
	}
    }
    return s_instance;    
}

CablingRPCBase*
CablingRPC::instance(std::string conf, std::string corr,
                     std::string data, bool cosmic)
{    
    s_RPCMapfromCool = false;
    if (! s_instance) {
        s_cosmic_configuration = cosmic;
        ConfName = conf;
        CorrName = corr;
        DataName = data;
        
        s_instance = new CablingRPC();
    }
    else if (!s_status)
    {
	// here the singleton exists but is not fully init because must wait for COOL access 
        //keep this comment for debugging purposes
        //std::cout << "CablingRPC--- Setting properties: configuration and ASCII file names"<< std::endl;
        
	s_cosmic_configuration = cosmic;
        ConfName = conf;
        CorrName = corr;
        DataName = data;
        //keep this comment for debugging purposes
	//std::cout<< "CablingRPC--- nothing else can happen here ..."<< std::endl;
    }
    return s_instance;    
}

CablingRPCBase*
CablingRPC::instance(const std::string* conf, const std::string* corr,
                     std::string data, bool cosmic)
{    
    s_RPCMapfromCool = true;
    //keep these comments for debugging purposes
    //std::cout<<"CablingRPC--- getting instance with strings from COOL: s_status = "<<s_status << std::endl;
    //std::cout<<"CablingRPC--- Pointers to Conf/Corr Maps are "<<(uintptr_t)conf<<"/"<<(uintptr_t)corr<< std::endl;
    //std::cout<<"CablingRPC--- Input Conf/Corr Maps size are "<<conf->size()<<"/"<<corr->size()<< std::endl;

    if ( !s_instance ) {
      //keep these comments for debugging purposes
      //std::cout<<"CablingRPC--- getting instance with strings: instance does not exist; make it"<< std::endl;
        s_cosmic_configuration = cosmic;
        ConfMapPString = conf;
        CorrMapPString = corr;
        DataName = data;

        s_instance = new CablingRPC();
    }
    else if (!s_status)
    {
	// here the singleton exists but is not fully init because must wait for COOL access 
        //keep these comments for debugging purposes
        //std::cout<<"CablingRPC--- Setting properties: configuration and strings from COOL folders"<< std::endl;
	s_cosmic_configuration = cosmic;
        ConfMapPString = conf;
        CorrMapPString = corr;
        DataName = data;
 
	//keep these comments for debugging purposes
	//std::cout <<"CablingRPC--- nothing else can happen here ..." << std::endl;       
    }
    return s_instance;    
}

void
CablingRPC::clearCache()
{    
    m_SectorLogic.clear();
    m_SectorType.clear();
    m_RDOs.clear();
    s_status = false;

    DISP<<"CablingRPC--- cacheCleared: s_status = "<<s_status; DISP_INFO;
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

    DISP << "CablingRPC---\n>>> RPC cabling map from file: " << file << " <<<"; DISP_INFO;

    // Open the input file
    std::ifstream LVL1conf(file.c_str());

    if (!LVL1conf)
    {
        DISP << "Cannot open " << file << "!";
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
CablingRPC::ReadConf(const std::string* map)
{   

    DISP << "CablingRPC--->> RPC cabling map from COOL <<"; DISP_INFO;
    DISP << "CablingRPC--- ReadConf: map is at    "<<(uintptr_t)(map); DISP_DEBUG;
    DISP << "CablingRPC--- ReadConf: map has size "<<map->size(); DISP_INFO;

    std::stringstream MAP;
    MAP.str(*map);
    // Create the reader for the database
    DBline data(MAP);

    // store the setup environment    
    std::string version,setup,layout;
    
    // Start reading routine
    unsigned int nlines=0;
    while (++data)
    {
        ++nlines;
        data("Version") >> version >> setup >> layout;

        if (setup == "Testbeam") m_TestbeamFlag = true;

        // Read the Sector Type enumeration
        int start = 0;
        int stop  = 0;
        if(data("SECTOR TYPES (") >> start >> "-" >> stop >> "):")
            for(int i=start;i<=stop;++i) {
		data >> m_SectorMap[i];
	    }
	

        // Set the m_MaxType variable and the type of SectorMap objects
        if(stop == 63 || stop == 8)
        {   
            for(int i=0;i<64;++i)
                if(m_SectorMap[i] > m_MaxType) m_MaxType = m_SectorMap[i];

            m_SectorType.resize(m_MaxType);
	    DISP<<"CablingRPC--- ReadConf: # of types is "<<m_MaxType; DISP_DEBUG;
	    
 
	    DISP<<"CablingRPC--- ReadConf: Loop over sector-types"; DISP_DEBUG;
            for(int i=1;i<=m_MaxType;++i)
	    { 
                m_SectorType[i-1] =
                    SectorLogicSetup(i,DataName,layout,s_cosmic_configuration);
	        SectorLogicSetup* sec = &(m_SectorType[i-1]);
                m_SectorType[i-1].SetPtoTrigRoads(p_trigroads);
                for(int j=0;j<64;++j) 
                    if(m_SectorMap[j] == i)
		    { 
                        *sec << j;
		        m_SectorLogic.insert(SLmap::value_type(j,sec));
			DISP<<" filling sectorLogicSetup Map for type "<<i<<" sector "<<j; DISP_DEBUG;
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
    DISP << "CablingRPC--- ReadConf: map n. of lines read is "<<nlines; DISP_INFO;
    
    m_Version += version + " " + setup + " " + layout;
    DISP << "CablingRPC--- ReadConf: version is "<<m_Version; DISP_INFO;
    
    // Setup of the LVL-1 configuration
    for(int i=1;i<=m_MaxType;++i) {
      if(!m_SectorType[i-1].setup()) return false;
    } 
    for(int i=1;i<=m_MaxType;++i) if(!m_SectorType[i-1].check()) return false; 

    // int secNumber = (setup=="Atlas")? 64 : 1;

    // build and init the map for the PAD RDO
    return buildRDOmap();
}


bool
CablingRPC::ReadCorr(std::string file)
{   

    DISP << "CablingRPC---\n>>> RPC cabling corrections from file: " << file << " <<<"; DISP_INFO;
    
    
     // Open the input file
    std::ifstream LVL1corr(file.c_str());

    if (!LVL1corr)
    {
        DISP << "Cannot open " << file << "!";
        DISP_WARNING;
        return false;
    }

    // Create the reader for the database
    DBline data(LVL1corr);
    // switch to new DBline I/O format
    data.setdbfmtflag(0);
    

    unsigned int nlines=0;
    // Start reading routine
    while (++data)
    {
        ++nlines;
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
	
        if(data("CABLE") >> data.dbhex() >> SubId >> SecId >> PADId >> CMAId >> ijk
	                 >> data.dbdec() >> type >> Channel1 >> Channel2 >> Number)
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
	        DISP << "Cannot apply correction: "; DISP_WARNING;
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
	
	
	if(data("BOARD") >> data.dbhex() >> SubId >> SecId >> PADId >> CMAId >> data.dbdec()
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
	        DISP << "Cannot apply correction: "; DISP_WARNING;
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
    DISP << "CablingRPC--- ReadCorr: correction map n. of lines read is "<<nlines; DISP_INFO;

    // switch back to previous DBline I/O format
    data.setdbfmtflag(1);
    
    return true;
}

bool
CablingRPC::ReadCorr(const std::string* map)
{   

    DISP << "CablingRPC--->> RPC cabling corrections from COOL <<" ; DISP_INFO;
    DISP << "CablingRPC--- ReadCorr: CorrMap is at    "<<(uintptr_t)(map); DISP_DEBUG;
    DISP << "CablingRPC--- ReadCorr: CorrMap has size "<<map->size(); DISP_INFO;

    std::stringstream MAP;
    MAP.str(*map);
    // Create the reader for the database
    DBline data(MAP);
    // switch to new DBline I/O format
    data.setdbfmtflag(0);

    unsigned int nlines = 0;
    while (++data)
    {
        ++nlines;

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
	
        if(data("CABLE") >> data.dbhex() >> SubId >> SecId >> PADId >> CMAId >> ijk
	                 >> data.dbdec() >> type >> Channel1 >> Channel2 >> Number)
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
	        DISP << "Cannot apply correction: "; DISP_WARNING;
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
	
	
	if(data("BOARD") >> data.dbhex() >> SubId >> SecId >> PADId >> CMAId >> data.dbdec()
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
	        DISP << "Cannot apply correction: "; DISP_WARNING;
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
    DISP << "CablingRPC--- ReadConf: CorrMap n. of lines read is "<<nlines; DISP_INFO;

    // switch back to previous DBline I/O format
    data.setdbfmtflag(1);
    
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
    DISP<<"CablingRPC--- buildRDOmap"; DISP_INFO;
    for (int sector = 0; sector < 64; ++sector)
    {
        DISP<<"CablingRPC--- buildRDOmap: sector "<<sector; DISP_DEBUG;
        unsigned int nRDOentries=0;
        int side = (sector<32)? 0 : 1;
        int logic_sector = sector % 32;

        if (m_SectorMap[sector])
        {
        
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
                    if(!ins.second)
                    {
                        DISP << "Error in inserting RDO " << rdo;
                        DISP_ERROR;
                        return ins.second;
                    }
                    ++nRDOentries;
	    
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
		    
                    if(!ins.second)
                    {
                        DISP << "Error in inserting RDO " << rdo;
                        DISP_ERROR;
                        return ins.second;
                    }
                    ++nRDOentries;
                }

                // increase the iterator            
                while( it != CMAs.end() &&
                       (unsigned int)(*it).first.PAD_index() == PADid ) 
                {++it;}	    
	    
            }
	}
        DISP<<"CablingRPC--- buildRDOmap: sector "<<sector<<" n of associated RDO (i.e. Pads) = "<<nRDOentries; DISP_DEBUG;
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
    int PhiIxx = PadRoI/2;
    int EtaIxx = PadRoI%2;
    
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];

    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
    
    if(!s.give_RoI_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder))
    {
        return false; // added for HLT
        /*  Commented for HLT
        // LOCAL RoI within the pad may be wrong if triggering only phi vew only, try the other local eta
        EtaIxx = (EtaIxx + 1)%2;
        CMAidentity ETA1(Eta,AllSectors,PadId,EtaIxx);
        
        if(!s.give_RoI_borders(ETA1,PHI,EtaLowBorder,EtaHighBorder,
                               PhiLowBorder,
                               PhiHighBorder))
        {
            DISP << "  Unmatched RoIId= " << RoIId << "  Side="
                 << SubsystemId << ", Sector=" << SectorId;
            DISP_DEBUG;
            
            return false;
        }
        */
    }
    
    EtaLowBorder  += logic_sector*1000000;
    EtaHighBorder += logic_sector*1000000;
    PhiLowBorder  += logic_sector*1000000;
    PhiHighBorder += logic_sector*1000000;

    return true;
}



bool 
CablingRPC::give_LowPt_borders  (unsigned short int SubsystemId,
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
    int PhiIxx = PadRoI/2;
    int EtaIxx = PadRoI%2;
    
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];

    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
    
    if(!s.give_LowPt_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder)){
        return false; // Added for HLT
            /* Commented for HLT
        // LOCAL RoI within the pad may be wrong if triggering only phi vew only, try the other local eta
        EtaIxx = (EtaIxx + 1)%2;
        CMAidentity ETA1(Eta,AllSectors,PadId,EtaIxx);
        if(!s.give_LowPt_borders(ETA1,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                               PhiHighBorder)){
            DISP << "  Unmatched RoIId= " << RoIId
                 << "  Side="  << SubsystemId << ", Sector=" << SectorId;
            DISP_DEBUG;
            return false;
        }
            */
    }
    if (EtaLowBorder==0||EtaHighBorder==0||
        PhiLowBorder==0||PhiHighBorder==0) return false;
    
    EtaLowBorder  += logic_sector*1000000;
    EtaHighBorder += logic_sector*1000000;
    PhiLowBorder  += logic_sector*1000000;
    PhiHighBorder += logic_sector*1000000;

    return true;
}



bool 
CablingRPC::give_HighPt_borders  (unsigned short int SubsystemId,
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
    int PhiIxx = PadRoI/2;
    int EtaIxx = PadRoI%2;
        
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    
    const SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];

    CMAidentity ETA(Eta,AllSectors,PadId,EtaIxx);
    CMAidentity PHI(Phi,PhiCov,PadId,PhiIxx);
    
    if(!s.give_HighPt_borders(ETA,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                           PhiHighBorder)){
        return false; // Added for HLT
        /* commented for HLT
        // LOCAL RoI within the pad may be wrong if triggering only phi vew only, try the other local eta
        EtaIxx = (EtaIxx + 1)%2;
        CMAidentity ETA1(Eta,AllSectors,PadId,EtaIxx);
        if(!s.give_HighPt_borders(ETA1,PHI,EtaLowBorder,EtaHighBorder,PhiLowBorder,
                               PhiHighBorder)){
            DISP << "  Unmatched RoIId= " << RoIId
                 << "  Side="  << SubsystemId << ", Sector=" << SectorId; 
            DISP_DEBUG;
            return false;
        }
        */
    }
    if (EtaLowBorder==0||EtaHighBorder==0||
        PhiLowBorder==0||PhiHighBorder==0) return false;
    
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
    
#ifndef NDEBUG 
    DISP << "Logic sector = " << logic_sector; DISP_DEBUG;
#endif

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

#ifndef NDEBUG 
    DISP << "CablingRPC::give_strip_code:"
               << "SubsystemId " << SubsystemId 
               << " Sector " << logic_sector 
               << " Id " << PADId
               << " CMA " << CMAId
               << " ijk " << ijk
               << " Channel " << Channel
               << " CodeList"; DISP_DEBUG;
#endif
    
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
	         << " " << decode.code();
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
	    rpcId.stationEta  = (decode.half_barrel() == Positive)?  rpc->stationEta() : 
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
    if (sector < 0 || sector > 63)
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

void
CablingRPC::SetPtoTrigRoads(const std::map<std::string, std::string>* RPC_trigroads) 
{
  p_trigroads=RPC_trigroads;
}

void
CablingRPC::ClearPtoTrigRoads() 
{
  p_trigroads=0;
}

void
CablingRPC::ClearPtoCablingMap() 
{
  ConfName = "";
  CorrName = "";
  ConfMapPString = 0;
  CorrMapPString = 0;
}

unsigned long int CablingRPC::strip_code_fromOffline (int etaPhiView, int logicSector, int cablingStation, int gasGap, int zIndexInCablingStation, int strip) const
{
  unsigned long int code = 0;
  code = 
    etaPhiView     * 100000000 +
    logicSector    * 1000000   +
    cablingStation * 100000    +
    gasGap         * 10000     +
    zIndexInCablingStation  * 100       +
    (strip-1);
  return code;
} 

RPCofflineId CablingRPC::strip_id_fromCode(unsigned long int strip_code)
{
  RPCdecoder decode(strip_code);
  RPCofflineId rpcId;
  rpcId.init();
  if (!decode) {
    //    DISP << "strip_id_fromCode error: cannot decode the following LVL1 Id"
    //	 << " " << decode.code();
    //DISP_ERROR;
    return rpcId;
  }
  int RPC_logic_sector = decode.logic_sector();
  int RPC_strip    = decode.strip_number();
  int RPC_chamber  = decode.rpc_z_index();
  int RPC_layer    = decode.rpc_layer();
  int RPC_station  = decode.lvl1_station();
  int sector       = (decode.logic_sector())%32;
  
  // retrieve the Sector Logic setup
  const SectorLogicSetup& s = m_SectorType[m_SectorMap[RPC_logic_sector] - 1];  
  // retrieve chamber
  const RPCchamber* rpc = s.find_chamber(RPC_station, RPC_chamber);
  
  rpcId.stationName = rpc->stationName();
  rpcId.stationEta  = (decode.half_barrel() == Positive)?  rpc->stationEta() : 
    -rpc->stationEta();
  rpcId.stationPhi  = (sector==31)? 1 : (sector+1)/4 +1;
  rpcId.doubletR    = rpc->doubletR();
  rpcId.doubletZ    = rpc->doubletZ();
  rpcId.doubletPhi  = (rpc->phiReadoutPannels()==2)? (sector+1)%2+1: 1;
  rpcId.gasGap      = RPC_layer + 1;
  rpcId.measuresPhi = static_cast<int>(decode.view());
  rpcId.strip       = RPC_strip + 1;
  
  return rpcId;
  
}

bool CablingRPC::largeSector(std::string stName) const 
{
  bool ls = false;
  if (
      stName == "BML"   || //2 
      stName == "BOL"   || //4 
      stName == "BME"   || //53 
      stName == "BIR"      //52 
      ) 
    ls = true;
  return ls;
}

unsigned long int CablingRPC::strip_code_fromOffId (std::string stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip)
{
  unsigned long int code = 0;
  
  //std::cout<<"strip_code_fromOffId called for "<<stationName<<" eta/phi "<<stationEta<<" "<<stationPhi<<" dbR/Z/P "<<doubletR<<" "<<doubletZ<<" "<<doubletPhi<<" gg/measPhi "<<gasGap<<" "<<measuresPhi<<" strip "<<strip<<std::endl;
  unsigned long int etaPhiView=2;
  unsigned long int phi1_16 = 2.*stationPhi;
  if (largeSector(stationName)) phi1_16 = phi1_16-1;
  int logicSector= (phi1_16-1)*2 + doubletPhi - 2;
  if (logicSector<0)   logicSector=31;
  if (measuresPhi >0)  etaPhiView = 1;
  if (stationEta >=0 ) logicSector +=32;



  unsigned long int cabStat = doubletR;
  if (stationName.substr(0,2)=="BO") {
    cabStat = 2+cabStat;
    if (cabStat>3) cabStat=2; // special case of BOF and BOG with dbr=2
  }
  else if (stationName.substr(0,3)=="BML") {
    if (abs(stationEta)==7 || (abs(stationEta)==6 && stationPhi==7)) 
      {
	cabStat=2; // special case of single RPC Chambers at the same R or dbr2
      }
  }
  if (stationName.substr(0,3)=="BIS" && logicSector>31) logicSector+=1; // BIS78 are in doubPhi==1

  if (logicSector<0 || logicSector>63) 
    {
      DISP <<"logicSector = "<<logicSector<<" out of range ";
      DISP_ERROR;
      return 999999;
    }

  int zIndexInCablingStation = computeZIndexInCablingStation(stationName, logicSector, 
							     stationEta, stationPhi, doubletR, doubletZ, 
							     cabStat); 
  if (zIndexInCablingStation > 99) return 0;
  int cablingStation         = cabStat;
 

  unsigned long int gg = gasGap-1;
  code = strip_code_fromOffline (etaPhiView, logicSector, cablingStation, gg, zIndexInCablingStation, strip);

  return code;
}

unsigned int CablingRPC::computeZIndexInCablingStation(std::string stationName, int logicSector, 
						       int stationEta, int stationPhi, int doubletR, int doubletZ, 
						       int cabStat)
{

  unsigned int zIndexInCablingStation = 999;

  int iStat=0;
  int astEta = std::abs(stationEta);
  
  std::map<std::string,int>::const_iterator stItr = rpcStats.find(stationName);
  if (stItr != rpcStats.end()) iStat = stItr->second;
  else throw std::runtime_error(Form("File: %s, Line: %d\nCablingRPC::computeZIndexInCablingStation() - StationName %s not found", __FILE__, __LINE__, stationName.c_str()));

  int side = 0;
  if (stationEta>0) side=1;

  // already computed   
  if (m_absZindexInThelayerOfThisChamber[iStat][side][astEta][stationPhi-1][doubletR-1][doubletZ-1] < 999) 
    {
      return m_absZindexInThelayerOfThisChamber[iStat][side][astEta][stationPhi-1][doubletR-1][doubletZ-1];  
    }
  // otherwise compute now 
  int cablingStation = -1;
  int sectType = m_SectorMap[logicSector];
  if (sectType < 1 || sectType > m_MaxType+1)
    {
      DISP <<"sectorType = "<<sectType<<" out of range ";
      DISP_ERROR;
      return 99999;
    }
  const SectorLogicSetup& sec = m_SectorType[sectType - 1];

  for (unsigned int jStat=1; jStat<4; ++jStat)
    {
      if (cablingStation != -1) break;
      for (unsigned int jCham=0; jCham<20; ++jCham)
	{
	  const RPCchamber* rpcC = sec.find_chamber(jStat,jCham);
	  //std::cout<<" jStat, jCham = "<<jStat<<" "<<jCham<<std::endl;
	  if (rpcC==NULL) continue;
	  //std::cout<<" RPCchamber found with name, eta, dbR, dbZ "<<rpcC->chamber_name()<<" "<<rpcC->stationEta()<<" "<<rpcC->doubletR()<<" "<<rpcC->doubletZ()<<std::endl;
	  
	  if ((rpcC->chamber_name()).substr(0,3)!=stationName ) {
	    //	    std::cout<<" rpcC->chamber_name()!=stationName "<<rpcC->chamber_name()<<" "<<stationName<<std::endl;
	    continue;
	  }
	  if (rpcC->stationEta()  != abs(stationEta)  ) {
	    //std::cout<<" rpcC->stationEta()!=stationEta "<<rpcC->stationEta()<<" "<<stationEta<<std::endl;
	    continue;
	  }
	  if (rpcC->doubletR()    !=doubletR    ) {
	    //std::cout<<" rpcC->doubletR()!=doubletR "<<rpcC->doubletR()<<" "<<doubletR<<std::endl;
	    continue;
	  }
	  if (rpcC->doubletZ()    !=doubletZ    ) {
	    //std::cout<<" rpcC->doubletZ()!=doubletZ "<<rpcC->doubletZ()<<" "<<doubletZ<<std::endl;
	    continue;
	  }
	  //std::cout<<" ... RPCchamber matches input "<<std::endl;
	  cablingStation = jStat;
	  zIndexInCablingStation = jCham;
	  break;
	}
    }
  //std::cout<<" cablingStation and zIndexInCablingStation "<<cablingStation<<" "<<zIndexInCablingStation<<std::endl;
  if ( cablingStation<1 || cablingStation>3 ||  (cabStat!=cablingStation) ) 
    {
      DISP <<"cablingStation = "<<cablingStation<<" out of range 1-3 or different w.r.t. basic calculation = "<<cabStat;
      DISP_WARNING; // it should never happen (digits from BOF/BOG dbR=2 should be turned off if pads are not there)
      return 99999;      
    }
  if ( zIndexInCablingStation>19 ) 
    {
      DISP <<"zIndexInCablingStation = "<<zIndexInCablingStation<<" out of range ";
      DISP_ERROR;
      return 99999;      
    }

  m_absZindexInThelayerOfThisChamber[iStat][side][astEta][stationPhi-1][doubletR-1][doubletZ-1] = zIndexInCablingStation;
  return zIndexInCablingStation;

}




#ifdef LVL1_STANDALONE

ShowCabling::ShowCabling(int type,int station,std::string element,int obj,bool detail):
m_type(type),m_station(station),m_obj(obj),m_element(element),m_detail(detail) {}

#endif
