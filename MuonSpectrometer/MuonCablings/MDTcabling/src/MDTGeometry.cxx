/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTcabling/MDTGeometry.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"

#include "Identifier/Identifier.h"
//#include "MuonDigitContainer/MdtIdHash.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>             // for exit()
#include <string.h>             // for strcmp()
#include <math.h>
#include <algorithm>

#if (__GNUC__) && (__GNUC__ > 2) 
               // put your gcc 3.2 specific code her
	       #include <sstream>
	       typedef std::stringstream  __sstream;
	       typedef std::ostringstream __osstream;
#else
               // put your gcc 2.95 specific code here
	       #include <strstream>
	       typedef strstream  __sstream;
	       typedef ostrstream __osstream;
#endif

//#if (__GNUC__) && (__GNUC__ > 2) 
//               // put your gcc 3.2 specific code her
//	       #include <sstream>
//	       typedef std::ostringstream __osstream;
//#else
//               // put your gcc 2.95 specific code here
//	       #include <strstream>
//	       typedef ostrstream __osstream;
//#endif



using namespace std;


///////////////////////////////////////////////////////////////////////////////

static Chamber* T2muonGeo[MaxTYPE][8*(2*MaxJZZ+1)];
static int      GEOMindex[MaxTYPE][8*(2*MaxJZZ+1)];
static uint8_t  (*Recabling)[8*(2*MaxJZZ+1)][MaxTDC][TDCch][3] = 0;
static uint32_t (*ChamberHash)[3];
static unsigned int MaxHashIdChamber = 0;

static interstruc incha;

MuonMDT_CablingMap* MDTGeometry::s_cabling_map = 0;

bool MDTGeometry::MapFlag = false;
bool MDTGeometry::GeoFlag = false;
bool MDTGeometry::CheFlag = false;
bool MDTGeometry::RemFlag = false;

bool MDTGeometry::RgsOut = false;
bool MDTGeometry::GeoOut = false;
bool MDTGeometry::ReadFlag = false;

std::string MDTGeometry::RgsFile = "RegionSelector.data";
std::string MDTGeometry::GeoFile = "MDTgeometry.data";
std::string MDTGeometry::RODFile = "RODmap.data";
std::string MDTGeometry::CABFile = "tubeMap_2009_01-15.txt";
std::string MDTGeometry::MEZFile = "AtlasMEZmap.data";
std::string MDTGeometry::InstallAreaDir = ".";

bool MDTGeometry::s_status = false;
bool MDTGeometry::s_shift_LinkId = false;

Amdcsimrec* MDTGeometry::s_amdc = 0;

const int MDTGeometry::MaxMROD = 52;
const int MDTGeometry::MaxTower = 4;

float MDTGeometry::s_TubePitch   = 0.;
double MDTGeometry::s_ScaleFactor = 1.;
//double MDTGeometry::s_pi = 3.14159265358979323846;
double MDTGeometry::s_pi = M_PI;

bool MDTGeometry::s_testbeam = false;
bool MDTGeometry::s_useOldGeoAccess = true;

int MAXLay = 0;
int MAXTub = 0;
int MAXMul = 0;

#ifndef LVL1_STANDALONE
#include <TH1F.h>
const MdtIdHelper*    MDTGeometry::s_pMdtIdHelper = 0;
TH1F* MDTGeometry::s_hist_barrel_alignCorrOnRadius = 0;
TH1F* MDTGeometry::s_hist_barrel_alignCorrOnZeta   = 0;
TH1F* MDTGeometry::s_hist_endcap_alignCorrOnRadius = 0;
TH1F* MDTGeometry::s_hist_endcap_alignCorrOnZeta   = 0;
#endif

///////////////////////////////////////////////////////////////////////////////


Chambers t2muongeo(void)
{
    return T2muonGeo;
}


bool 
MDTGeometry::getOnlineIDfromOfflineID(int name, int StationEta,
    int StationPhi,int MultiLayer, int TubeLayer,int Tube, 
    unsigned short int& SubsystemId,
    unsigned short int& MrodId,
    unsigned short int& LinkId,
    unsigned short int& TdcId,
    unsigned short int& ChannelId) const
{
    CSMid temp(name,StationEta,StationPhi);
    CSMid csm;

    if(!find_csm(temp,csm,SubsystemId,MrodId,LinkId)) return false;

    const Chamber* chamber = csm.get_Chamber(StationEta);
    if(!chamber) return false;

    if(MultiLayer<1 || MultiLayer>2) return false;
    Multilayer* multilayer = chamber->multi[MultiLayer-1];
    if(!multilayer) return false;

    TdcId = 0;
    ChannelId = 0;
    bool flag = false;

    for(int k=0;k<MaxTDC/2;k++)
    {
        if(TDCgeo* tdc = multilayer->TDC[k]) 
        {
	    for(int ch=0;ch<TDCch;++ch)
	    {
	        int Layer = tdc->Lay[ch]-(MultiLayer-1)*multilayer->TubeLayers;
	        if(Layer == TubeLayer && tdc->Tub[ch] == Tube)
		{
		    flag = true;
		    TdcId = tdc->Id;
		    ChannelId = ch;
		    break;
		}
	    }
	}
	if(flag) break;
    }
    
    return flag;
}

bool 
MDTGeometry::getOfflineIDfromOnlineID(unsigned short int SubsystemId,
                                      unsigned short int MrodId,
                                      unsigned short int LinkId,
                                      unsigned short int TdcId,
                                      unsigned short int ChannelId,
                                      int& name, int& StationEta,
                                      int& StationPhi,int& MultiLayer, 
                                      int& TubeLayer,int& Tube) const
{

    if ((TdcId >= MaxTDC || ChannelId >= TDCch) &&
        (TdcId != 0xff   || ChannelId != 0xff)     ) return false;

    int address = 0;
    if(!mrod_address(SubsystemId,MrodId,address)) return false;
    const MRODscheme* mrod = m_mrods[address];

    if(mrod->Subsystem_ID() != SubsystemId || mrod->ROD_ID() != MrodId) 
        return false;

    //if(s_testbeam) ++LinkId;
    LinkId = (s_shift_LinkId)? LinkId-1:LinkId;
    if(LinkId >7) return false;

    CSMid csm = mrod->links()[LinkId];
    if(!csm) return false;

    name = csm.stationName();
    StationEta = csm.stationEta(TdcId);
    StationPhi = csm.stationPhi();

    if (TdcId == 0xff && ChannelId == 0xff) {
        StationEta = csm.stationEta();
	if(abs(StationEta)>10) StationEta/=10;
        MultiLayer = -1;
	TubeLayer  = -1;
	Tube       = -1;
	return true;
    }

    const Chamber* chamber = csm.get_chamber(TdcId);
    if(!chamber) return false;

    const TDCgeo* tdc = csm.get_tdc(TdcId);
    if(!tdc || tdc->Id != TdcId) return false;

    TDCgeo* found = 0;    
    for(int i=0;i<MaxTDC/2;++i) 
       if( (found = chamber->multi[0]->TDC[i]) == tdc) break;

    MultiLayer = (found == tdc)? 1 : 2;

    int LayShift = 0;
    if(MultiLayer == 2) LayShift =  chamber->multi[0]->TubeLayers;

    TubeLayer = tdc->Lay[ChannelId] - LayShift;
    Tube      = tdc->Tub[ChannelId];
    
    if (TubeLayer == 0 || Tube == 0) return false;
    
    return true;
}

bool
MDTGeometry::getStationRad(unsigned int StationName, 
                           unsigned int StationEta,
                           unsigned int StationPhi,
		           float& Rmin, float& Rmax) const
{

    Amdcsimrec* amdc = s_amdc;                      // get the AMDC interface
    
    int Jtype = StationName + 1;
    std::string name = amdc->GetStationType(Jtype);
    
    //int Jff = (name.c_str()[2] == 'L' || name.c_str()[2] == 'R')?
    //          (StationPhi-1)*2 + 1 : StationPhi*2;  // get Jff

    const Chamber* chamber = T2muonGeo[StationName]
                                      [(StationPhi-1)*(2*Mzz+1)+StationEta+Mzz];
    if(chamber)
    {
        Rmin = chamber->Rmin;
	Rmax = chamber->Rmax;
        return true;
    }
    else
    {
        return false;
    }
}

bool 
MDTGeometry::getRZ(unsigned short int SubsystemId,
                   unsigned short int MrodId,
                   unsigned short int LinkId,
                   unsigned short int TdcId,
                   unsigned short int ChannelId,
                   float& R, float& ortoRad, float& Z) const
 
{
    if(TdcId >= MaxTDC || ChannelId >= TDCch) return false;

    int address = 0;
    if(!mrod_address(SubsystemId,MrodId,address)) return false;
    const MRODscheme* mrod = m_mrods[address];

    if(mrod->Subsystem_ID() != SubsystemId || mrod->ROD_ID() != MrodId) 
        return false;

    LinkId = (s_shift_LinkId)? LinkId-1 : LinkId;
    if(LinkId >7) return false;

    CSMid csm = mrod->links()[LinkId];
    if(!csm) return false;

    const Chamber* chamber = csm.get_chamber(TdcId);
    if(!chamber) return false;

    const TDCgeo* tdc = csm.get_tdc(TdcId);
    if(!tdc || tdc->Id != TdcId) return false;

    R = tdc->RZ[ChannelId][0];
    Z = tdc->RZ[ChannelId][1];

    ortoRad = chamber->OrtoRadialPos;

    return true;
}

void 
MDTGeometry::getAmdbIDfromOfflineID(std::string name,int StationEta,
    int StationPhi,int MultiLayer,int TubeLayer,int Tube,
    int& Jtype,int& Jff,int& Jzz,int& Job,int& Jsl,int& Jtube) const
{
    Amdcsimrec* amdc = MDTGeometry::s_amdc;

    Jtype = amdc->GetJtyp(name);
    amdc->GetAMDCindexFromMDT(name,StationEta,StationPhi,MultiLayer,TubeLayer,
                              Tube,Jff,Jzz,Job,Jsl,Jtube);
}


const CSMid 
MDTGeometry::getCsm(unsigned short int SubsystemId,
		    unsigned short int MrodId,
		    unsigned short int LinkId) const
{
    CSMid csm("-");
    int address = 0;
    //if(s_testbeam) LinkId = LinkId+1;
    LinkId = (s_shift_LinkId)? LinkId-1 : LinkId;
    if(mrod_address(SubsystemId,MrodId,address)) 
        csm = (m_mrods[address])->links()[LinkId];

    return csm;
}

///////////////////////////////////////////////////////////////////////////////



bool
MDTGeometry::readgeo(ifstream& geometry) const
{
    Header header;
    
    while (geometry.peek()!=EOF) {   // start looping on all geom data.
      geometry.read((char *) &header, sizeof header);
      geometry.read((char *) &incha, sizeof incha);
      if(! buildMDTgeo(header.JJ,header.JFF,header.JZZ,1)) return false;
    }

    return true;
}


MDTGeometry* MDTGeometry::s_instance = 0;

MDTGeometry::MDTGeometry(void) : Mtype(s_amdc->StationTypeEnd()),
                                 Mff(s_amdc->PositionPhiEnd()),
                                 Mzz(s_amdc->PositionZEnd())
{
    m_mrods = 0;
    Recabling = 0;

    initcham();
    
    //load or try to create the mezanine map
    if (MEZFile.length()!=0) 
    {   
        FILE* f = fopen(MEZFile.c_str(), "rb");
        if (f == NULL)
        {
            std::cout << "Cannot open the mezanine mapping file " 
	              << MEZFile << std::endl;
            return;
	} 
	else
	{
            Recabling = new uint8_t[MaxTYPE][8*(2*MaxJZZ+1)][MaxTDC][TDCch][3];
            memset(Recabling,0,MaxTYPE*(8*(2*MaxJZZ+1))*24*18*3*sizeof(uint8_t));
	    fread(Recabling,sizeof(uint8_t),MaxTYPE*(8*(2*MaxJZZ+1))*MaxTDC*TDCch*3,f);
            fclose(f);
	}
    } else if (RODFile == "") {
    
        if(!create_mezmap()) {
	    std::cout << "Link to cabling map missing, mezanine map not created"
	              << std::endl;
	    // continue 
	}
    
    }
    
    
    if(ReadFlag)
    {
        ifstream geometry;               // instanciate the input file

        geometry.open(GeoFile.c_str(),ios::in | ios::binary);
        if(!geometry) 
        {
            cout << "Cannot open file " << GeoFile.c_str()
	         << " in the working directory" << endl;
            return;
        }

        bool status = readgeo(geometry); // read geometry parameters from file
	geometry.close();
	if(! status) return;
    } 
    else
    {
        if(! mdtmap()) return;           // get geometry parameters from AMDC
    }
    
    m_chambers = t2muongeo();            // load geometry data in
    
    if (s_shift_LinkId) if(! t2mucgeo()) return; // check geometry

    if (Recabling) 
    {
        delete [] Recabling;
        Recabling = 0;
    }

    // Get the boundaries of the MDT stations
    if(!setup_Stations()) return;


    // Start MROD map section 
    CSMid::s_amdc = s_amdc;
    if(RODFile.find("Atlas")!=std::string::npos || RODFile=="") 
        CSMid::isNewSchema = true;

    m_readout_towers.clear();            // clear the mrod map table

    if( RODFile != "") {
        ifstream MRODmap(RODFile.c_str());

        if(!MRODmap)
        {            
            std::cout << "Cannot open file " << RODFile.c_str()
	              << " in the working directory" << std::endl;
	    return;
        }

        DBline data(MRODmap);                // Create the reader for the map
        if(!read_mrod_map(data)) return;     // Read the data
    } else {
        if(!create_mrod_map()) {
	    std::cout << "Cannot create the MROD map from the cabling link"
	              << std::endl;
            return;
	}
    }
    
    m_mrods = new const MRODscheme* [MaxMROD*MaxTower];
    for(int i=0;i<MaxMROD*MaxTower;++i) m_mrods[i] = 0;

    if(!setup_mrods()) return;
    if(!check_mrod_map()) return;
    if(!setup_chamber_hash_map()) return;

    if (CABFile.length()!=0) {   
        ifstream CABmap(CABFile.c_str());
        if(!CABmap.is_open()) {            
            cout << "Cannot open file " << CABFile.c_str()
	         << " in the working directory" << endl;
	    return;
	}
	
	DBline remap(CABmap);                       // Create the reader for the map
	if(!read_cabling_remapping(remap)) return;  // Read the data
    }

/*
    float dPhi = (2.*s_pi)/100;
    for(int j=0;j<3;++j) 
    {
        for (int i=0;i<100;i++)
        {
	    float phi = dPhi*i;
  	    int barrel_tr = 0;
	    int barrel_ov = 0;
	    int endcap_tr = 0;
	    int endcap_ov = 0;
	
	    getBsects(j,phi,barrel_tr, barrel_ov);
	    getEsects(j,0,phi,endcap_tr, endcap_ov);
	
	    std::cout << "Station " << j << ", phi=" << phi << " (rad): barrel tr_sec="
	              << barrel_tr << " barrel ov_sec=" << barrel_ov
		      << ", endcap tr_sec=" << endcap_tr
		      << " endcap ov_sec=" << endcap_ov << std::endl;
        }
    }
    
    
    for(int i=0;i<16;++i)
        for (int j=0;j<3;++j)
	   for (int s=0;s<2;++s)
	   {
	       float Zmin;
	       float Zmax;
	       getEstatZ(i,j,s,Zmin,Zmax);
	       std::cout << "sector " << i << "  station " << j
	                 << "  side " << s << ":   Zmin=" << Zmin
			 << ", Zmax=" << Zmax << std::endl;
	   }
    */
    
    s_status = true;
}

MDTGeometry::~MDTGeometry(void)
{

  // is there anything to do? in principle delete each Chamber
  /*
    for (int Jtype=0;Jtype<Mtype;++Jtype)
    {
        for (int J=0;J<Mff*(2*Mzz+1);++J)
	{
	    T2muonGeo[Jtype][J] = 0;
	}
    }
  */
    m_readout_towers.clear();
    if(m_mrods) delete[] m_mrods;
    s_status = false;
}


const MDTGeometry*
MDTGeometry::instance(void)
{
    if (! s_instance) {
        s_instance = new MDTGeometry();
    }
    
    if(!s_status)
    {
        cout << "MDT cabling model is not loaded!" << endl;
	delete s_instance;
	s_instance = 0;
    }

    /*
    // do some test after initialization
    cout << "POST INITIALIZATION TESTS" << endl;
    unsigned short int SubsystemId = 0;
    unsigned short int MrodId = 0;
    unsigned short int LinkId = 0;
    unsigned short int TdcId = 0;
    unsigned short int ChannelId = 0;
    
    float R;
    float ortoRad;
    float Z;
    
    if (s_instance->getOnlineIDfromOfflineID(0,-6,1,1,1,1,SubsystemId,MrodId,LinkId,
                                 TdcId,ChannelId))
    {
        s_instance->getRZ(SubsystemId,MrodId,LinkId,TdcId,ChannelId,
              R, ortoRad, Z);
	 cout << "Fisrt tube and first layer Z=" << Z << endl;
	 cout << "Fisrt tube and first layer R=" << R << endl;
    }
    if (s_instance->getOnlineIDfromOfflineID(0,-6,1,1,1,36,SubsystemId,MrodId,LinkId,
                                 TdcId,ChannelId))
    {
        s_instance->getRZ(SubsystemId,MrodId,LinkId,TdcId,ChannelId,
              R, ortoRad, Z);
	 cout << "Last tube and first layer Z=" << Z << endl;
	 cout << "Last tube and first layer R=" << R << endl;
    }
    if (s_instance->getOnlineIDfromOfflineID(0,-6,1,2,4,1,SubsystemId,MrodId,LinkId,
                                 TdcId,ChannelId))
    {
        s_instance->getRZ(SubsystemId,MrodId,LinkId,TdcId,ChannelId,
              R, ortoRad, Z);
	 cout << "Fisrt tube and first layer Z=" << Z << endl;
	 cout << "Fisrt tube and first layer R=" << R << endl;
    }
    if (s_instance->getOnlineIDfromOfflineID(0,-6,1,2,4,36,SubsystemId,MrodId,LinkId,
                                 TdcId,ChannelId))
    {
        s_instance->getRZ(SubsystemId,MrodId,LinkId,TdcId,ChannelId,
              R, ortoRad, Z);
	 cout << "Last tube and first layer Z=" << Z << endl;
	 cout << "Last tube and first layer R=" << R << endl;
    }
    */
    
    return s_instance;
}


void 
MDTGeometry::initcham() 
{

    m_cham.maxType = Mtype;
    m_cham.maxJzz  = Mzz;
    for(int i=0;i<m_cham.maxType;++i) 
    {
        int Jtype = i+1;
        std::string name = s_amdc->GetStationType(Jtype);
        for(int j=0;j<3;++j) m_cham.type[i][j] = name.c_str()[j];  
    }
}

/*===========================================================================*/

bool 
MDTGeometry::read_mrod_map(DBline& data)
{
    unsigned short int start_mrod;
    unsigned short int stop_mrod;
    unsigned short int eta_tower;
    unsigned short int system_id;
    unsigned char side;
    
    while(++data)
      {
        if(data("Towers") >> start_mrod >> "-" >> stop_mrod 
                          >> "eta" >> eta_tower
                          >> "side" >> side 
                          >> "ID" >> hex() >> system_id >> dec() )
        {
            unsigned short int side_id = 0;
	    bool TestbeamFlag = false;
	    
	    if(side == 'A')       side_id = 1;
	    else if(side == 'C')  side_id = 0; 
	    else if(side == 'T')  {side_id = 2; TestbeamFlag=true;}

            ReadOutTowers towers(system_id,eta_tower,side_id,TestbeamFlag);

            (++data)("{");
            do
            {
	        unsigned short int mrod_id;
	        unsigned short int crate_id;
	        std::string link1;
	        std::string link2;
	        std::string link3;
	        std::string link4;
	        std::string link5;
	        std::string link6;
	        std::string link7;
	        std::string link8;

                Amdcsimrec* amdc = s_amdc;   // get the interface towards AMDC

	        if(data(" ") >> hex() >> mrod_id >> dec() >> "||" 
                             >> link1 >> link2 >> link3 >> link4 
                             >> link5 >> link6 >> link7 >> link8
	                     >> "|" >> crate_id)
	        {		    
			      
	            MRODscheme mrod(system_id,mrod_id,crate_id,link1,link2,
                                    link3,link4,link5,link6,link7,link8);
		    for (int i=0;i<8;++i)
		    {
			if(mrod.links()[i])
			{
			    int tmp = mrod.links()[i].stationName()+1;
		            std::string name = amdc->GetStationType(tmp);
                            int StationEta = mrod.links()[i].stationEta();
                            int StationPhi = mrod.links()[i].stationPhi();
		            int Multilayer = 1;
		            int Layer      = 1;
		            int Tube       = 1;
		            int Jtype = amdc->GetJtyp(name);
		            int Jff;
		            int Jzz;
		            int Job;
		            int Jsl;
		            int Jtube;

                            int stEta[2] = {StationEta%10,
			                   (abs(StationEta)>9)?StationEta/10:999};
			    
                            for(int k=0;k<2;++k)
			    {	  
			        if(stEta[k]!=999)
				{
                                    amdc->GetAMDCindexFromMDT(name,stEta[k],
			                              StationPhi,Multilayer,
                                                      Layer,Tube,
		                                      Jff,Jzz,Job,Jsl,Jtube);

		                    int Jcode = (Jff-1)*(2*Mzz+1)+Jzz+Mzz;
		                    mrod.csm()[i].set_chamber(
			                          m_chambers[Jtype-1][Jcode],
					          stEta[k]);
				    mrod.csm()[i].set_tower(eta_tower);

                                }
                            }
			}
		    }

		    towers << mrod;
	        }

            data++;
            }while(!data("}"));

        m_readout_towers.push_back(towers);

        }
    }

    return true;
}


bool
MDTGeometry::mrod_address(unsigned short int Subsystem_ID,
	                  unsigned short int MROD_ID,int& address)const
{
    if(MROD_ID >= MaxMROD) return false;
    int tower_add = tower_address(Subsystem_ID);
    if(tower_add < 0) return false;

    address = MaxMROD * tower_add + MROD_ID;

    return true;
}

bool
MDTGeometry::setup_chamber_hash_map() const{

    Amdcsimrec* amdc = s_amdc;          // get the interface towards AMDC
    
    MaxHashIdChamber = 0;
    for(int Jtype=1;Jtype<=Mtype;Jtype++) 
    {
        for(int Jzz=amdc->PositionZBegin();Jzz<=Mzz;Jzz++) 
        {
            for(int Jff=amdc->PositionPhiBegin();Jff<=Mff;Jff++)
	    { 
                if(m_chambers[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])
		{
		    std::string name = amdc->GetStationType(Jtype);
	            int Jgeo  = GEOMindex[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		    int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);
                    int Job = 0;
		    int Jsl = 1;
		    int Jtube = 1;

		    for (int Jobj=1;Jobj<=MJobj;++Jobj)
		    {
		        int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			std::string tech_name = amdc->TechnologyName(Itec);
			if (tech_name=="MDT") {Job = Jobj; break;}
		    }

		    if(Job)
		    {
		        int StationEta = 0;
		        int StationPhi = 0;
		        int Multilayer = 0;
		        int TubeLayer  = 0;
		        int Tube       = 0;

                        amdc->GetMDTindexFromAMDC(name,Jff,Jzz,Job,Jsl,Jtube,
                                                  StationEta,StationPhi,
                                                  Multilayer,TubeLayer,Tube);
                        int code = Jtype-1;
                    
		        unsigned int hash = 0;
					   
			Identifier id = s_pMdtIdHelper->elementID
			                    (code,StationEta,StationPhi);
                        ExpandedIdentifier exId;
			IdentifierHash hashId;
			IdContext context = s_pMdtIdHelper->module_context();
			s_pMdtIdHelper->get_expanded_id(id,exId,&context);
			s_pMdtIdHelper->get_hash(id,hashId,&context);
			    
			hash = hashId;
			if(hash > MaxHashIdChamber) MaxHashIdChamber = hash;

		    }
		}
            }
	}
    }
    
    ChamberHash = new uint32_t[MaxHashIdChamber+1][3];
    memset(ChamberHash,0,(MaxHashIdChamber+1)*3*sizeof(uint32_t));    
    
    
    for(int Jtype=1;Jtype<=Mtype;Jtype++) 
    {
        for(int Jzz=amdc->PositionZBegin();Jzz<=Mzz;Jzz++) 
        {
            for(int Jff=amdc->PositionPhiBegin();Jff<=Mff;Jff++)
	    { 
                if(Chamber* mdt=m_chambers[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])
		{
		    std::string name = amdc->GetStationType(Jtype);
	            int Jgeo  = GEOMindex[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		    int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);
                    int Job = 0;
		    int Jsl = 1;
		    int Jtube = 1;

		    for (int Jobj=1;Jobj<=MJobj;++Jobj)
		    {
		        int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			std::string tech_name = amdc->TechnologyName(Itec);
			if (tech_name=="MDT") {Job = Jobj; break;}
		    }

		    if(Job)
		      {
		        int StationEta = 0;
		        int StationPhi = 0;
		        int Multilayer = 0;
		        int TubeLayer  = 0;
		        int Tube       = 0;

                        amdc->GetMDTindexFromAMDC(name,Jff,Jzz,Job,Jsl,Jtube,
                                                  StationEta,StationPhi,
                                                  Multilayer,TubeLayer,Tube);

		       
		        int code = Jtype-1;
		        CSMid csm(code,StationEta,StationPhi);
                        CSMid found;

			unsigned short int sbs;
			unsigned short int rod;
			unsigned short int lnk;

			if( !find_csm(csm,found,sbs,rod,lnk) )//&& 
			     //csm.mnemonic().find("EEL") == std::string::npos )
			{
			    cout << csm << std::endl;
			    std::cout << "csm StationEta=" << csm.stationEta()
			              << std::endl;
			    cout << "The Chambers with " << csm 
			         << "is not present into the MROD map!" 
                                 << endl;
			    return false;
			}

			if(found)
		        {
			    const Chamber* cham[2];
			    found.get_chambers(cham);
			    if(cham[0] != mdt && cham[1] != mdt)
			    {
			        cout << "code=" << code
			             << ", StationEta=" << StationEta
				     << ", StationPhi=" << StationPhi
				     << std::endl;
			        cout << "The chamber " << found
			             << "has a wrong link to the chambers map!" 
                                     << endl;
			        return false;
                            }
		        }
			
			   
			    
		        int address = 0;
                        if(!mrod_address(sbs,rod,address)) return false;
                        const MRODscheme* mrod = m_mrods[address];
  
			uint32_t RobId = 
			        ( mrod->Subsystem_ID() << 16) | mrod->ROD_ID();
						       
			unsigned int hash = 0;
	   
			Identifier id = s_pMdtIdHelper->elementID
			                    (code,StationEta,StationPhi);
                        ExpandedIdentifier exId;
			IdentifierHash hashId;
			IdContext context = s_pMdtIdHelper->module_context();
			s_pMdtIdHelper->get_expanded_id(id,exId,&context);
			s_pMdtIdHelper->get_hash(id,hashId,&context);
			    
			hash  = hashId;
			
			if(hash <= MaxHashIdChamber) {
			    ChamberHash[hash][0] = RobId;
			    ChamberHash[hash][1] = lnk;
			    char st = name.c_str()[1];
			    if(st == 'I') ChamberHash[hash][2] = 1;
			    else if(st == 'O') ChamberHash[hash][2] = 3;
			    else ChamberHash[hash][2] = 2;
			    //std::cout << "hash=" << hash
			    //           << ",  robId=" << RobId
	                    //       << ",  link=" << lnk
                            //         << ",  station=" << ChamberHash[hash][2]
			    //       << std::endl;
                        } else {
			    std::cout << "MDTGeometry: hash map will not be built!" <<
			    std::endl;
			    std::cout << "Max MAD Hash Id is " 
			              << MaxHashIdChamber 
				      << ",  but found hash id = " << hash
			              << std::endl;
                            return false;
			} 
		    }
		}
	    }
	}
    }
    
    return true;
}

bool
MDTGeometry::setup_mrods() const
{
    ReadoutTowers::const_iterator it = m_readout_towers.begin();
    while(it != m_readout_towers.end())
    {
	unsigned short int subsystem = (*it).Subsystem_ID();
	int size = (*it).towers().size();
	for(int i=0;i<size;++i)
	{
	    unsigned short int MROD_ID = (*it).towers()[i].ROD_ID();
	    int address = 0;
	    if(mrod_address(subsystem,MROD_ID,address))
	    { 
                m_mrods[address] = &((*it).towers()[i]);
	    }
	    else 
            {
	        cout << "No mrod address for Subsystem " << subsystem
		     << " and ROD_ID " << MROD_ID << endl; 
                return false;
	    }
	}
	++it;
    }

    return true;
}

bool
MDTGeometry::find_csm(const CSMid& csm, CSMid& found,
                      unsigned short int& SubsystemId,
		      unsigned short int& RodId, 
                      unsigned short int& LinkId) const
{
    for (int i=0;i<MaxMROD*MaxTower;++i)
    {
      for (int l=0;l<8;++l)
      {
          if( m_mrods[i] )
	  {
	      CSMid link = (m_mrods[i])->links()[l];
	              
	      if(link == csm)
              {
	          found = link;
                  SubsystemId = (m_mrods[i])->Subsystem_ID();
                  RodId       = (m_mrods[i])->ROD_ID();
                  LinkId      = (s_shift_LinkId)? l+1 : l;
                  return true;
              }
	  }
      }
    }
    return false;
}

bool
MDTGeometry::check_mrod_map() const
{    
    Amdcsimrec* amdc = s_amdc;          // get the interface towards AMDC

    ofstream regionSelector;            // file storing the Region Selector map
    if(RgsOut)
    {
        regionSelector.open(RgsFile.c_str(), ios::out );
        if(!regionSelector) 
        {
            cout << "Cannot open file " << RgsFile.c_str()
	         << " in the working directory" << endl;
            return false;
        }
	regionSelector << 
	"MDT extended Id  Hash Id  ROB Id   EtaMin  EtaMax   PhiMin  PhiMax" 
	               << endl << endl;
    }

    //MdtIdHash mdt_hash;
    
    bool TestbeamFlag = (*m_readout_towers.begin()).testbeam();

    for(int Jtype=1;Jtype<=Mtype;Jtype++) 
    {
        for(int Jzz=amdc->PositionZBegin();Jzz<=Mzz;Jzz++) 
        {
            for(int Jff=amdc->PositionPhiBegin();Jff<=Mff;Jff++)
	    { 
                if(Chamber* mdt=m_chambers[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])
		{
		    std::string name = amdc->GetStationType(Jtype);
	            int Jgeo  = GEOMindex[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		    int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);
                    int Job = 0;
		    int Jsl = 1;
		    int Jtube = 1;

		    for (int Jobj=1;Jobj<=MJobj;++Jobj)
		    {
		        int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			std::string tech_name = amdc->TechnologyName(Itec);
			if (tech_name=="MDT") {Job = Jobj; break;}
		    }

		    if(Job)
		      {
		        int StationEta = 0;
		        int StationPhi = 0;
		        int Multilayer = 0;
		        int TubeLayer  = 0;
		        int Tube       = 0;

                        amdc->GetMDTindexFromAMDC(name,Jff,Jzz,Job,Jsl,Jtube,
                                                  StationEta,StationPhi,
                                                  Multilayer,TubeLayer,Tube);

                       // MuonStationName code = static_cast<MuonStationName>(0);
                       // int type = 0;

                       // while(muon_station_name(code) != "UNKNOWN")
                       // {
                       //     code = muon_station_name_index(type);
	               //     if(name[0] == muon_station_name(code)[0] &&
                       //        name[1] == muon_station_name(code)[1] &&
                       //        name[2] == muon_station_name(code)[2]) break;
                       //     type++;
                       // }
		       
		        int code = Jtype-1;
		        CSMid csm(code,StationEta,StationPhi);
                        CSMid found;

			unsigned short int sbs;
			unsigned short int rod;
			unsigned short int lnk;

			if(!find_csm(csm,found,sbs,rod,lnk) && s_shift_LinkId)
			{
			    cout << "The Chambers with " << csm 
			         << "is not present into the MROD map!" 
                                 << endl;
			    return false;
			}

			if(found )
		        {
			    const Chamber* cham[2];
			    found.get_chambers(cham);
			    if(cham[0] != mdt && cham[1] != mdt)
			    {
			        cout << "code=" << code
			             << ", StationEta=" << StationEta
				     << ", StationPhi=" << StationPhi
				     << std::endl;
			        cout << "The chamber " << found
			             << "has a wrong link to the chambers map!" 
                                     << endl;
			        return false;
                            }
		        }
			
			if(RgsOut && !TestbeamFlag)
			{
			    float PhiMin = mdt->Phipos - mdt->DeltaPhi;
			    float PhiMax = mdt->Phipos + mdt->DeltaPhi;
			    if( PhiMin<0. )     PhiMin += 2*s_pi;
			    if( PhiMax>2*s_pi ) PhiMax -= 2*s_pi;
			    double Zmin = mdt->Zmin;
			    double Zmax = mdt->Zmax;
			    double Rmin = mdt->Rmin;
			    double Rmax = mdt->Rmax;
			    double ThetaMin = 0.;
			    double ThetaMax = 0.;
			    double EtaMin = 0.;
			    double EtaMax = 0.;
			    
			    if(StationEta < 0)
			    {
			        if(name[0] == 'B')
				{
			    ThetaMax = (Zmax)? atan(Rmax/fabsf(Zmax)):asin(1.);
			    ThetaMin = (Zmin)? atan(Rmin/fabsf(Zmin)):asin(1.);
			    EtaMin = log(tan(ThetaMin/2.));
			    EtaMax = log(tan(ThetaMax/2.));
			        }
				else
				{
		            ThetaMax = (Zmin)? atan(Rmax/fabsf(Zmin)):asin(1.);
			    ThetaMin = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			    EtaMin = log(tan(ThetaMin/2.));
			    EtaMax = log(tan(ThetaMax/2.));
				}
			    //EtaMin = -EtaMin;
			    //EtaMax = -EtaMax;
			    }
			    else if (StationEta > 0)
			    {
			    ThetaMin = (Zmin)? atan(Rmax/fabsf(Zmin)):asin(1.);
			    ThetaMax = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			    EtaMin = -log(tan(ThetaMin/2.));
			    EtaMax = -log(tan(ThetaMax/2.));
			    }
			    else if (StationEta == 0)
			    {
			    ThetaMin = (Zmin)? atan(Rmin/fabsf(Zmin)):asin(1.);
			    ThetaMax = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			    EtaMin = log(tan(ThetaMin/2.));
			    //EtaMin = - EtaMin;
			    EtaMax = -log(tan(ThetaMax/2.));
			    }
			    
		            int address = 0;
                            if(!mrod_address(sbs,rod,address)) return false;
                            const MRODscheme* mrod = m_mrods[address];

			    unsigned short int RosId = mrod->MROD_crate(); 
			    unsigned short int ROBin = rod/16;
			    unsigned short int Link  = rod%2+((rod%16)/8)*2+1;  
			    unsigned short int RobId = Link  << 6 |
			                               ROBin << 4 |
			                               RosId;
						       
                            std::string MdtId = "";
			    unsigned int hash = 0;
#ifndef LVL1_STANDALONE			   
			    Identifier id = s_pMdtIdHelper->elementID
			                    (code,StationEta,StationPhi);
                            ExpandedIdentifier exId;
			    IdentifierHash hashId;
			    IdContext context = s_pMdtIdHelper->module_context();
			    s_pMdtIdHelper->get_expanded_id(id,exId,&context);
			    s_pMdtIdHelper->get_hash(id,hashId,&context);
			    
			    MdtId = exId;
			    hash  = hashId;
			    
			    
			    /*
			    cout << MdtId << " :  ZetaMin=" << Zmin
			         << ",  ZetaMax=" << Zmax
				 << ",  RMin="  << Rmin
				 << ",  RMax="  << Rmax << endl;
			    */
			    
			    //MdtModuleId muonId(code,StationEta,StationPhi);
		                                //Multilayer,TubeLayer,Tube);
                            //Identifier id = muonId.atlas_id();

                            //int hash = mdt_hash(id);
#endif			    
			    // fix for using the new system of Phi coordinates
			    //if(PhiMin >= s_pi) PhiMin -= 2*s_pi;
			    //if(PhiMax >= s_pi) PhiMax -= 2*s_pi;
			    
			    //regionSelector << id.getString() << "     "
			    regionSelector << MdtId << "     "
			    //<< "   " << setw (3) << name << "     "
			    << setw(6) << hash << "      "
			    << hex << setw(4) << setfill('0') << RobId 
			    << dec << setfill(' ') << "  "
			    << setiosflags(ios::fixed) << setprecision(3) 
			    << setw(8) << EtaMin
			    << setiosflags(ios::fixed) << setprecision(3) 
			    << setw(8) << EtaMax << " "
			    << setiosflags(ios::fixed) << setprecision(3) 
			    << setw(8) << PhiMin
			    << setiosflags(ios::fixed) << setprecision(3) 
			    << setw(8) << PhiMax << endl;
                        }
		    }
		}
	    }
	}
    }

    if(RgsOut) regionSelector.close();

    return true;
}


std::vector<uint32_t>
MDTGeometry::getRobId(float etaMin,float etaMax,float phiMin,float phiMax) const
{
    std::vector<uint32_t> robs;
    ROBid id =  getROBid(phiMin,phiMax,etaMin,etaMax);
    for(ROBid::const_iterator it=id.begin();it!=id.end();++it)
    {
        robs.push_back(*it);
    }
    return robs;
}

const MDTGeometry::ROBid 
MDTGeometry::getROBid(float phiMin,float phiMax,float etaMin,float etaMax) const
{
    ROBid IDs;                          // container for the RobId
    const Chamber* cham[2] = {0,0};
    for(int i=0;i<MaxMROD*MaxTower;++i)
    {
        if( const MRODscheme* mrod = m_mrods[i] )
	{
	    bool ovl = false;
	    int l = 0;
	    do
	    {
	        const CSMid* csm = &(mrod->links()[l]);
		csm->get_chambers(cham);
	        if( cham[0] )
	        {
	            ovl = csm->isOverlapping(etaMin,etaMax,phiMin,phiMax);
		}
		++l;
	    } while (!ovl && l<8);
	   
	    if (ovl) 
	    {
		uint32_t id = ( mrod->Subsystem_ID() << 16) | mrod->ROD_ID();
		IDs.insert(id);  
	    }
	} 
    }

/*
    Amdcsimrec* amdc = s_amdc;          // get the interface towards AMDC
    
    for(int Jtype=1;Jtype<=Mtype;Jtype++) 
    {
        for(int Jzz=amdc->PositionZBegin();Jzz<=Mzz;Jzz++) 
        {
            for(int Jff=amdc->PositionPhiBegin();Jff<=Mff;Jff++)
	    { 
                if(Chamber* mdt=m_chambers[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])
		{
		    std::string name = amdc->GetStationType(Jtype);
	            int Jgeo  = GEOMindex[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		    int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);
                    int Job = 0;
		    int Jsl = 1;
		    int Jtube = 1;

		    
		    float PhiMid = mdt->Phipos; // check only middle position
		    double Zmin = mdt->Zmin;
		    double Zmax = mdt->Zmax;
		    double Rmin = mdt->Rmin;
		    double Rmax = mdt->Rmax;
		    double ThetaMin = 0.;
		    double ThetaMax = 0.;
		    double EtaMin = 0.;
		    double EtaMax = 0.;

                    bool PhiCheck = (PhiMid >= phiMin && PhiMid <= phiMax)?
		                    true : false;
		    
		    if(Jzz < 0)
		    {
		        if(name[0] == 'B')
			{
		            ThetaMax = (Zmax)? atan(Rmax/fabsf(Zmax)):asin(1.);
		            ThetaMin = (Zmin)? atan(Rmin/fabsf(Zmin)):asin(1.);
		            EtaMin = log(tan(ThetaMin/2.));
			    EtaMax = log(tan(ThetaMax/2.));
	                }
			else
			{
		            ThetaMax = (Zmin)? atan(Rmax/fabsf(Zmin)):asin(1.);
			    ThetaMin = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			    EtaMin = log(tan(ThetaMin/2.));
			    EtaMax = log(tan(ThetaMax/2.));
			}
		    }
		    else if (Jzz > 0)
		    {
			ThetaMin = (Zmin)? atan(Rmax/fabsf(Zmin)):asin(1.);
			ThetaMax = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			EtaMin = -log(tan(ThetaMin/2.));
			EtaMax = -log(tan(ThetaMax/2.));
		    }
		    else if (Jzz == 0)
		    {
			ThetaMin = (Zmin)? atan(Rmin/fabsf(Zmin)):asin(1.);
			ThetaMax = (Zmax)? atan(Rmin/fabsf(Zmax)):asin(1.);
			EtaMin = log(tan(ThetaMin/2.));
			EtaMax = -log(tan(ThetaMax/2.));
		    }

                    bool EtaCheck = ((EtaMin >= etaMin && EtaMin <= etaMax)||
			             (EtaMax >= etaMin && EtaMax <= etaMax) )?
		                    EtaCheck = true : EtaCheck = false;

                    if ( PhiCheck && EtaCheck )
		    {

		        for (int Jobj=1;Jobj<=MJobj;++Jobj)
		        {
		            int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			    std::string tech_name = amdc->TechnologyName(Itec);
			    if (tech_name=="MDT") {Job = Jobj; break;}
		        }

		        if(Job)
		        {
		            int StationEta = 0;
		            int StationPhi = 0;
		            int Multilayer = 0;
		            int TubeLayer  = 0;
		            int Tube       = 0;

                            amdc->GetMDTindexFromAMDC(name,Jff,Jzz,Job,Jsl,
			                              Jtube,StationEta,
						      StationPhi,Multilayer,
						      TubeLayer,Tube);


		       
		            int code = Jtype-1;

		            CSMid csm(code,StationEta,StationPhi);
                            CSMid found;
			    
			    unsigned short int sbs;
			    unsigned short int rod;
			    unsigned short int lnk;  
			    
                            if(find_csm(csm,found,sbs,rod,lnk))
			    {

			        cout << "Etamin=" << etaMin << ", EtaMax="
				     << etaMax << ", Phimin=" << phiMin
				     << ", PhiMax=" << phiMax << endl;
			        cout << found << "SubSystem=" << hex 
				     << sbs << endl;
				
		                int address = 0;
                                mrod_address(sbs,rod,address);
				IDs.insert(address);
				
				//cout << "address=" << dec << address << endl;
				
                                const MRODscheme* mrod = m_mrods[address];

			        unsigned short int RosId = mrod->MROD_crate(); 
			        unsigned short int ROBin = rod/16;
			        unsigned short int Link  = rod%2+((rod%16)/8)*2
				                           +1;  
			        unsigned short int RobId = Link  << 6 |
			                                   ROBin << 4 |
			                                   RosId;
			        
							   
			    }
			   
                        }
		    }
		}
	    }
	}
    }
*/    
    return IDs;
}

std::vector<uint32_t>
MDTGeometry::getHashId(float etaMin,float etaMax,float phiMin,float phiMax) const
{
    std::vector<uint32_t> hashs;
    HASHid id =  getHASHid(phiMin,phiMax,etaMin,etaMax);
    for(HASHid::const_iterator it=id.begin();it!=id.end();++it)
    {
        hashs.push_back(*it);
    }
    return hashs;
}

const MDTGeometry::HASHid 
MDTGeometry::getHASHid(float phiMin,float phiMax,float etaMin,float etaMax) const
{
    HASHid IDs;                          // container for the HashId
    const Chamber* cham[2] = {0,0};
    for(int i=0;i<MaxMROD*MaxTower;++i)
    {
        if( const MRODscheme* mrod = m_mrods[i] )
	{
	    bool ovl = false;
	    int l = 0;
	    do
	    {
	        const CSMid* csm = &(mrod->links()[l]);
		csm->get_chambers(cham);
	        if( cham[0] )
	        {
	            ovl = csm->isOverlapping(etaMin,etaMax,phiMin,phiMax);
		}
		++l;
	    } while (!ovl && l<8);
	   
	    if (ovl) 
	    {
		uint32_t id = ( mrod->Subsystem_ID() << 16) | mrod->ROD_ID();
		IDs.insert(id);  
	    }
	} 
    }

    return IDs;
}

bool 
MDTGeometry::checkHashId(unsigned int hash) const {
    if (hash > MaxHashIdChamber) return false;
    if (ChamberHash[hash][0]!=0) return true;
    return false;
}

bool 
MDTGeometry::checkHashIdForROB(unsigned int hash, uint32_t robId) const {
    if (hash > MaxHashIdChamber) return false;
    if (ChamberHash[hash][0]==robId) return true;
    return false;
}

bool 
MDTGeometry::checkHashIdForLink(unsigned int hash, uint32_t robId, 
                                                   uint32_t link) const {
    if (hash > MaxHashIdChamber) return false;
    if (ChamberHash[hash][0]==robId && ChamberHash[hash][1]==link) return true;
    return false;
}

int
MDTGeometry::hash2Station(unsigned int hash) const {
    if (hash > MaxHashIdChamber) return 0;
    return ChamberHash[hash][2];
}
/*===========================================================================*/

bool
MDTGeometry::t2mucgeo() const
{

    // lay,tdcpk
    int ff,i,n=0,sect,Jtype,Jff,Jzz,nmul,ntdc,ntdcm=0,lay,count=0,fl;
    int nmulo,ch,chh,cc=0,tub;
    float Ypos[MaxLay],Ylim[2],Xpos=0;
    char str[2][8] = {"R(cm)= ","Z(cm)= "};
    Chamber *chamber,*other,*chamsta[8];

    for(Jtype=0;Jtype<Mtype;Jtype++) {
      for(Jzz=-Mzz;Jzz<=Mzz;Jzz++) {
        for(ff=0;ff<8;ff++) chamsta[ff] = 0;
          for(Jff=1;Jff<=8;Jff++) 
            if((chamber = m_chambers[Jtype][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])) break;
 
/******************** Start checking chamber for all sector ******************/
            while(Jff<=8) {
              chamsta[Jff-1] = chamber;
              sect = Jff;
              count++;
              nmul = 0;                               // Count MDT multilayer.
              for(i=0;i<MaxMul;i++) if(chamber->multi[i]) nmul++;

              if((ntdc = chamber->NumTDC)) {            //Start checking TDCs.
                ntdcm = ntdc/nmul;
                if(!ntdc%2) {
        printf("t2mucgeo error: wrong TDC number for chamber %s at Jzz = %3d\n"
                               ,chamber->Type,Jzz);
                return false;
                }
		
                i = nmul;
		// Check on TDC linker:
		// exclude BMS at Jzz = +/- 6 and +/- 4, BIR at Jzz = +/- 1
		// because multilayers are not symmetric
                while (ntdc--) {
                  if(!((ntdc+1)%ntdcm)) i--;
                  if(chamber->TDC[ntdc]!=chamber->multi[i]->TDC[ntdc-i*ntdcm] &&
		     (chamber->Type[0]!='B' && chamber->Type[1]!='M' &&
		      chamber->Type[2]!='S' && abs(Jzz)!=6 && abs(Jzz)!=4) &&
		     (chamber->Type[0]!='B' && chamber->Type[1]!='I' &&
		      chamber->Type[2]!='R' && abs(Jzz)!=1) ){
        printf("t2mucgeo error: wrong TDC linker for chamber %s at Jzz = %3d\n"
                                   ,chamber->Type,Jzz);
                    return false;
                  }
                }
                
		i = nmul;
                while(i--) {
		  bool sflag = false;
		  if (!s_shift_LinkId) {
	              if( chamber->Type[2]=='R' && Jff==8 && abs(Jzz)==3 ) sflag=true;
	              if( chamber->Type[2]=='R' && abs(Jzz)==4 && i==0 ) sflag=true;
	          }
                  if((n=checkR(chamber->Type[0],chamber->multi[i],ntdcm,sflag)) &&
		      !s_testbeam){
           printf("t2mucgeo error: wrong TDC Rad for chamber %s at Jzz = %3d\n"
                                   ,chamber->Type,Jzz);
                    printf("               TDC number = %2d\n",n);   
                    return false; 
                  }
                  if((n=checkZ(chamber->Type[0],chamber->multi[i],ntdcm,sflag)) &&
		      !s_testbeam ){
          printf("t2mucgeo error: wrong TDC Zeta for chamber %s at Jzz = %3d\n"
                                   ,chamber->Type,Jzz); 
                    printf("               TDC number = %2d\n",n);
                    return false; 
                  }
                  if((n=checkD(chamber->multi[i],ntdcm)) &&
		      !s_testbeam ){
          printf("t2mucgeo error: wrong TDC dim. for chamber %s at Jzz = %3d\n"
                                   ,chamber->Type,Jzz);
                    printf("               TDC number = %2d\n",n); 
                    return false;
                  }
                  if(checkL(chamber->Type[0],chamber->multi[i],ntdcm/*,sflag*/) &&
		     !s_testbeam ){
          printf("t2mucgeo error: wrong tube lay for chamber %s at Jzz = %3d\n"
                                   ,chamber->Type,Jzz);
                     return false;
                  }
                }  
              }
/***************** Print chamber geometry on standard output *****************/
              if(CheFlag) {
          printf("CHAMBER %s at Jzz = %3d, Jff = %1d: ",chamber->Type,Jzz,Jff);
                printf("%d multilayer ",nmul);
                if(nmul) printf("each of %1d layers of %2d tubes\n\n",
                                 chamber->multi[nmul-1]->TubeLayers,
	                         chamber->multi[nmul-1]->TubesNumber);
                else printf("\n\n");
                i = nmul;

                if(i) {
                  n = chamber->multi[i-1]->TubesNumber/2;
                  if(chamber->multi[i-1]->TubesNumber%2) n++;
                  n = n + ntdcm - 1;
                }

                while(i--) {
                  ch = 23;
                  cc      = (chamber->Type[0]=='B') ? 0        : 1;
                  Ylim[0] = (chamber->Type[0]=='B') ? chamber->multi[i]->Rmin :
                             chamber->multi[i]->Zmin ;
                  Ylim[1] = (chamber->Type[0]=='B') ? chamber->multi[i]->Rmax :
                             chamber->multi[i]->Zmax ;
                  for(lay=0;lay<MaxLay;lay++) Ypos[lay] = 0.;
                  lay = chamber->multi[i]->TubeLayers;
                  while(lay--) {
                    ntdc = ntdcm;
                    Ypos[lay] = chamber->multi[i]->TDC[0]->RZ[ch][cc];
                    while(ch>0&&(chamber->multi[i]->TDC[0]->RZ[ch][cc]==
                                 chamber->multi[i]->TDC[0]->RZ[ch-1][cc])) --ch;
                    if(lay) --ch;
                    printf("%s%8.1f   ",str[cc],Ypos[lay]);
                    for(ntdc=0;ntdc<ntdcm;ntdc++) {
                      tub = 0;
                      for(chh=0;chh<TDCch;chh++) 
                if(chamber->multi[i]->TDC[ntdc]->RZ[chh][cc]==Ypos[lay]) tub++;
                      myprin('O',tub/2);
                      if(tub%2) printf("o");
                      printf(" ");
                    }
	      
                    if(lay+1==chamber->multi[i]->TubeLayers) 
		                           printf("  %s%8.1f",str[cc],Ylim[1]);
                    else if(lay==0)
                                           printf("  %s%8.1f",str[cc],Ylim[0]);
                    printf("\n");
                  }
                  if(i) { 
                    printf("%s%8.1f  ",str[cc],chamber->Ymid);
                    myprin('-',n+2);
                    printf("\n");
                  }
                }

                if(chamber->multi[0]) {
                  if(cc) cc = 0;
                  else cc = 1;
                  myprin(' ',18); printf("^"); myprin(' ',n-2); printf("^\n");
                  myprin(' ',18); printf("|"); myprin(' ',n-2); printf("|\n");
            printf("    %s%8.1f",str[cc],chamber->multi[0]->TDC[0]->RZ[0][cc]);
                  myprin(' ',n-2);
                  ch = 0;
		  if(!s_testbeam)  // tube Z coordinate increase
		  {
                  while(chamber->multi[0]->TDC[ntdcm-1]->RZ[ch+1][cc]!=0.&&
                        chamber->multi[0]->TDC[ntdcm-1]->RZ[ch][cc]<=
                        chamber->multi[0]->TDC[ntdcm-1]->RZ[ch+1][cc])
                          Xpos = chamber->multi[0]->TDC[ntdcm-1]->RZ[++ch][cc];
                  } else
		  {
		  while(chamber->multi[0]->TDC[ntdcm-1]->RZ[ch+1][cc]!=0.&&
                        chamber->multi[0]->TDC[ntdcm-1]->RZ[ch][cc]>=
                        chamber->multi[0]->TDC[ntdcm-1]->RZ[ch+1][cc])
                          Xpos = chamber->multi[0]->TDC[ntdcm-1]->RZ[++ch][cc];
		  }
                  printf("%s%8.1f\n",str[cc],Xpos);  
                }
                printf("\n\n");
              }
/****************** Start test other sectors for consistence *****************/
              while(++Jff<=8) {
                fl = 0;
                if((other = m_chambers[Jtype][(Jff-1)*(2*Mzz+1)+Jzz+Mzz])) {
                  if(strcmp(chamber->Type,other->Type)) {
                    printf("t2mucgeo error: bad name %s for chamber type %s\n",
                                     other->Type, chamber->Type);
                    return false;
                  }
                  for(ff=0;ff<8;ff++) {
                    if((chamsta[ff])&&(chamsta[ff]->NumTDC==other->NumTDC)&&
                       (chamsta[ff]->Rmin==other->Rmin)&&
                       (chamsta[ff]->Rmax==other->Rmax)&&
                       (chamsta[ff]->Zmin==other->Zmin)&&
                       (chamsta[ff]->Zmax==other->Zmax)&&
                       (chamsta[ff]->Ymid==other->Ymid)) 
                                            {fl=0;chamber = chamsta[ff];break;}
                     else fl = 1;  
                  }
                  if(fl) {
                    chamber = other;
                    break;
                  }
                  count++;
                  nmulo = 0;
		  nmul = 0;

		  // count nultilayers of reference chamber
                  for(i=0;i<2;i++) if(chamber->multi[i]) nmul++;
		  // count nultilayers of chamber to be tested
                  for(i=0;i<2;i++) if(other->multi[i]) nmulo++;
		  // check identity between multilayers number
                  if(nmul!=nmulo) fl = 1;
		  // compute the TDC number per multilayer
	          if(nmulo) ntdcm = other->NumTDC/nmulo;
 
 /*
                  while(nmulo--) {
            if((n=checkTDC(chamber->multi[nmulo],other->multi[nmulo],ntdcm))) {
                    printf(
"t2mucgeo error: TDC mismatch for type %s at Jzz = %3d, Jff = %1d, Jff = %1d\n"
                                       ,chamber->Type,Jzz,sect,Jff);
                    printf(
		        "                 multilayer = %d, TDC number = %2d\n",
                                       nmulo+1,n);
                    return false;
                    }
                  }
*/
                  if(fl) {
                    printf(
     "t2mucgeo errorflag %2d for chamber %s at Jzz = %3d, Jff = %d, Jff = %d\n"
                                   ,fl,chamber->Type,Jzz,sect,Jff);
                    return false;
                  }
                }
              }
            }

        }
      }

    if(CheFlag) {
        printf("\nt2mucgeo: %4d chamber checked!\n",count);
    }

    return true;
}

/*===========================================================================*/

int
MDTGeometry::checkR(char type,Multilayer *multi,int TDCnum, bool sflag) const
{
    int tdcpk,res,ch,lay,tub,tublim,last=1;
    float Rad,shift;
  
    tdcpk = TDCch/multi->TubeLayers;
    res = multi->TubesNumber%tdcpk;
  
    while(TDCnum--) {
        if(type=='B') {
            ch = 0;
            for(lay=0;lay<multi->TubeLayers;lay++) {
                Rad = multi->TDC[TDCnum]->RZ[ch][0];
                if (res) tublim = res;
                else {tublim = tdcpk;}
		
		if (sflag && TDCnum) tublim = tdcpk;
		
                //	if (last&&(lay%2)) tublim--;
                for(tub=1;tub<tublim;tub++)
		  {
		    // cout <<"reference RAD = " << Rad
		    // <<"  matrching RAD = " 
		    //   << multi->TDC[TDCnum]->RZ[ch+1][0] << endl; 
                    if(Rad!=multi->TDC[TDCnum]->RZ[++ch][0]) return(TDCnum+1);
		  }
                ch++;
            }
            last = 0;
            res  = (sflag)? res : 0;    
        }else if (type=='E') {
            ch = 0;
            for(lay=0;lay<multi->TubeLayers-1;lay++) {
                if (res) tublim = res;
                else {tublim = tdcpk;}
                //	if (last&&(lay%2)) tublim--;
                for(tub=0;tub<tublim;tub++) {
                    Rad = multi->TDC[TDCnum]->RZ[ch][0];
		    shift = -s_TubePitch/2. + s_TubePitch*(lay%2);
 
			 
		    if(multi->Type[0] == 'S')
		    {
		        //cout <<"reference RAD = " << Rad
		        // <<"  matching RAD = " 
		        // << multi->TDC[TDCnum]->RZ[ch+tublim][0]+shift << endl;
			 
		        float match=multi->TDC[TDCnum]->RZ[ch+tublim][0]+shift;
                        if(fabsf(Rad-match) > 0.02 &&
                         !(last&&((lay+1)%2)&&tub==tublim-1)) return(TDCnum+1);
		    } else
		    {
		        //cout <<"reference RAD = " << Rad
		        // <<"  matching RAD = " 
		        // << multi->TDC[TDCnum]->RZ[ch+tublim][0]-shift << endl;
		        float match=multi->TDC[TDCnum]->RZ[ch+tublim][0]-shift;
			if(!(last&&((lay+1)%2)&&tub==tublim-1) && 
			    fabsf(Rad-match) > 0.02 )
                            return (TDCnum+1); 
		    }
                    ch++;
                }
            }
            last = 0;
            res  = 0;    
        }else {
            printf("checkR error: the type %c is wrong!",type);
            return (1);
        }
    }
    return(0);
}

/*===========================================================================*/

int
MDTGeometry::checkZ(char type,Multilayer *multi,int TDCnum,bool sflag) const
{
    int tdcpk,res,ch,lay,tub,tublim,last=1;
    float Zeta,shift;
  
    tdcpk = TDCch/multi->TubeLayers;
    res = multi->TubesNumber%tdcpk;
  
    while(TDCnum--) {
        if(type=='B') {
            ch = 0;
            for(lay=0;lay<multi->TubeLayers-1;lay++) {
                if (res) tublim = res;
                else {tublim = tdcpk;}
		
		if(sflag && TDCnum) tublim = tdcpk;
		
                //	if (last&&(lay%2)) tublim--;
                for(tub=0;tub<tublim;tub++) {
                    Zeta = multi->TDC[TDCnum]->RZ[ch][1];
		    shift = -s_TubePitch/2. + s_TubePitch*(lay%2);
		    
		    if(multi->Zmax <= 0.) shift = -shift;
		    
		    //cout <<"reference Z = " << Zeta
		    //     <<"  matching Z = " 
		    //     << multi->TDC[TDCnum]->RZ[ch+tublim][1] << endl;
		    
		    if(multi->Type[0] == 'S')
		    {
		      //std::cout << "Type s, shift = " << + shift << endl;
		      float match = multi->TDC[TDCnum]->RZ[ch+tublim][1]+shift;
                        if(fabsf(Zeta-match) > 0.005 &&
			 !(last&&((lay+1)%2)&&tub==tublim-1)) return(TDCnum+1);
                    } else
		    {
		      //std::cout << "Type z, shift = " << - shift << endl;
		      float match = multi->TDC[TDCnum]->RZ[ch+tublim][1]-shift;
			if(!(last&&((lay+1)%2)&&tub==tublim-1) && 
			   fabsf(Zeta-match) > 0.005)
                        return (TDCnum+1); 
		    }
                    ch++;
                }
            }
            last = 0;
            res  = (sflag)? res : 0;         
        }else if (type=='E') {
            ch = 0;
            for(lay=0;lay<multi->TubeLayers;lay++) {
                Zeta = multi->TDC[TDCnum]->RZ[ch][1];
                if (res) tublim = res;
                else {tublim = tdcpk;}
                //	if (last&&(lay%2)) tublim--;
                for(tub=1;tub<tublim;tub++)
		{
		    float match = multi->TDC[TDCnum]->RZ[++ch][1];
                    if(fabsf(Zeta-match) > 0.0001 ) return(TDCnum+1);
		}
                ch++;
            }
            last = 0;
            res  = 0;         
        }else {
            printf("checkR error: the type %c is wrong!",type);
            return(1);
        }
    }
    return(0);
}

/*===========================================================================*/

int
MDTGeometry::checkL(char type,Multilayer *multi,int TDCnum /*, bool sflag*/) const
{
     int ch,cl=0,lay,last=1;
    float *Xpos;
    TDCgeo *tdc;
  
    //if (sflag) last = 0;
  
    if(type=='E') cl = 1;
    lay = multi->TubeLayers;
    Xpos = new float [lay];
  
    while (TDCnum--) {
        tdc = multi->TDC[TDCnum];
        lay = multi->TubeLayers;
        ch = TDCch;
	
	//if(sflag && TDCnum==0) last = 1;
	
        if(last) {
            while (tdc->RZ[--ch][0]==0.) ;
            if(ch>=lay-1) last = 0;
            while(lay--) {
                Xpos[lay] = tdc->RZ[ch][cl];
                while( ch>0&&(tdc->RZ[ch][cl]==tdc->RZ[ch-1][cl]) ) --ch;
		if(lay) --ch;
            }
            if(ch) return(1);
        } else {
            //ch--;
	    while (tdc->RZ[--ch][0]==0.) ;
            while(lay--) {
                if(tdc->RZ[ch][cl]!=Xpos[lay]) return(1);
                while(ch>0&&(tdc->RZ[ch][cl]==tdc->RZ[ch-1][cl]) ) --ch;
		if(lay) --ch;
            }
            if(ch) return(1);
        }
    }
    
    delete [] Xpos;
    
    return(0); 
}

/*===========================================================================*/

int
MDTGeometry::checkD(Multilayer *multi,int TDCnum) const
{
    int ch,fl=0;
    float Rmin=99999.,Rmax=0.,Zmin=99999.,Zmax=0.;
    TDCgeo *tdc=0;
  
    while(TDCnum--) {
        tdc = multi->TDC[TDCnum];
        Rmin = 99999.;
        Rmax = 0.;
        Zmin = 99999.;
        Zmax = -99999.;
        for(ch=0;ch<TDCch;ch++) 
        {	
        if((tdc->RZ[ch][0]<=Rmin)&&(tdc->RZ[ch][0]!=0.)) Rmin = tdc->RZ[ch][0];
        if((tdc->RZ[ch][0]>=Rmax)&&(tdc->RZ[ch][0]!=0.)) Rmax = tdc->RZ[ch][0];
        if((tdc->RZ[ch][1]<=Zmin)&&(tdc->RZ[ch][0]!=0.)) Zmin = tdc->RZ[ch][1];
        if((tdc->RZ[ch][1]>=Zmax)&&(tdc->RZ[ch][0]!=0.)) Zmax = tdc->RZ[ch][1];
	}
        if(fabsf(tdc->Rmin-Rmin+s_TubePitch/2.)>=nor(.01)) {fl = 1; printf("1\n"); break;}
        if(fabsf(tdc->Rmax-Rmax-s_TubePitch/2.)>=nor(.01)) {fl = 1; printf("2\n"); break;}
        if(fabsf(tdc->Zmin-Zmin+s_TubePitch/2.)>=nor(.01)) {fl = 1; printf("3\n"); break;}
        if(fabsf(tdc->Zmax-Zmax-s_TubePitch/2.)>=nor(.01)) {fl = 1; printf("4\n"); break;}
    }
    

    if(fl) {
        for(ch=0;ch<TDCch;ch++) 
        {
	std::cout << "tdc->RZ[ch][0] = " << tdc->RZ[ch][0] << 
	             ",  tdc->RZ[ch][1] = " << tdc->RZ[ch][1] << std::endl;
        }
      printf("TDC Rmin = %9.2f    Computed Rmin = %9.2f\n",tdc->Rmin,Rmin-s_TubePitch/2.);
      printf("TDC Rmax = %9.2f    Computed Rmax = %9.2f\n",tdc->Rmax,Rmax+s_TubePitch/2.);
      printf("TDC Zmin = %9.2f    Computed Zmin = %9.2f\n",tdc->Zmin,Zmin-s_TubePitch/2.);
      printf("TDC Zmax = %9.2f    Computed Zmax = %9.2f\n",tdc->Zmax,Zmax+s_TubePitch/2.);
      return(TDCnum+1); 
    }
  
    return(0);
}

/*===========================================================================*/

int
MDTGeometry::checkTDC(Multilayer *multi1,Multilayer *multi2,int TDCnum) const
{
    int ch,fl=0;
    TDCgeo *tdc1=0,*tdc2=0;
  
    while(TDCnum--) {
        tdc1 = multi1->TDC[TDCnum];
        tdc2 = multi2->TDC[TDCnum];
        if(tdc1->Rmin!=tdc2->Rmin) {fl = 1;
	            std::cout << "DRmin="<<setprecision(10)<<tdc1->Rmin-tdc2->Rmin<< std::endl;}
        if(tdc1->Rmax!=tdc2->Rmax) {fl = 1;
	            std::cout << "DRmax="<<setprecision(10)<<tdc1->Rmax-tdc2->Rmax<< std::endl;}
        if(tdc1->Zmin!=tdc2->Zmin) {fl = 1;
	            std::cout << "DZmin="<<setprecision(10)<<tdc1->Zmin-tdc2->Zmin<< std::endl;}
        if(tdc1->Zmax!=tdc2->Zmax) {fl = 1;
	            std::cout << "DZmax="<<setprecision(10)<<tdc1->Zmax-tdc2->Zmax<< std::endl;}
        for(ch=0;ch<TDCch;ch++) {
            if(tdc1->RZ[ch][0]!=tdc2->RZ[ch][0]) {fl = 1;
std::cout << "ch=" << ch << ": DR="<<setprecision(10)<<tdc1->RZ[ch][0]-tdc2->RZ[ch][0]<< std::endl;}
            if(tdc1->RZ[ch][1]!=tdc2->RZ[ch][1]) {fl = 1;
std::cout << "ch=" << ch << ": DZ="<<setprecision(10)<<tdc1->RZ[ch][1]-tdc2->RZ[ch][1]<< std::endl;}        
	}
        if (fl) break;
    }

    if (fl) {
      printf("TDC1 Rmin = %13.6f    TDC2 Rmin = %13.6f\n",tdc1->Rmin,tdc2->Rmin);
      printf("TDC1 Rmax = %13.6f    TDC2 Rmax = %13.6f\n",tdc1->Rmax,tdc2->Rmax);
      printf("TDC1 Zmin = %13.6f    TDC2 Zmin = %13.6f\n",tdc1->Zmin,tdc2->Zmin);
      printf("TDC1 Zmax = %13.6f    TDC2 Zmax = %13.6f\n",tdc1->Zmax,tdc2->Zmax);
       for(ch=0;ch<TDCch;ch++)
           printf("TDC1 R = %13.6f, Z =  %13.6f    TDC2 R = %13.6f, Z = %13.6f\n",
              tdc1->RZ[ch][0],tdc1->RZ[ch][1],tdc2->RZ[ch][0],tdc2->RZ[ch][1]);
        return(TDCnum+1);
    }

    return(0);
}

/*===========================================================================*/

void 
MDTGeometry::myprin(char c,int i) const {

  int j;
  
  for(j=0;j<i;j++) printf("%c",c);
}

/*===========================================================================*/

bool
MDTGeometry::mdtmap() const
{
    Amdcsimrec* amdc = s_amdc;          // get the interface towards AMDC

    const int Mseq    = 8;
    const int Mtyp    = 60;
    const int Mjzz    = 31;

    int NumCham[2][16][Mjzz][Mseq];  //

    for (int i=0;i<2;++i)
        for(int j=0;j<16;++j) 
            for(int k=0;k<Mzz*2+1;++k)
                for (int m=0;m<Mseq;++m) NumCham[i][j][k][m] = 0;

    int ExistCham[Mtyp];                // counter for existing chamber type
    for (int i=0;i < Mtype;++i) *(ExistCham+i) = 0;

    int Iphi      = 0;   //phi sector (0/15)
    int Isystem   = 0;   //system (0 = barrel, 1 = endcap)
    int TZeroCham = 0;   //counter for chambers in between the two half barrel
	

    if(MapFlag)
    {
	cout << "T2MUMAP debugging printout:" << endl;
    }
	
 

//  ===== INITIALIZE THE GEOMETRY MAP<-----------------------------------------
    for (int Jtype=0;Jtype<Mtype;++Jtype)
    {
        for (int J=0;J<Mff*(2*Mzz+1);++J)
	{
	    T2muonGeo[Jtype][J] = 0;
	    GEOMindex[Jtype][J] = 0;
	}
    }


//  ========== DECLARE THE VARIABLES HOSTING THE GEOMETRY PARAMETERS ==========

    int MLayer;         // number of MDT multilayers
    int MDTlay[2];      // tube layers per multilayer
    int RPCcha;         // number of RPC chambers
    int MDTitec[2];     // tec number for MDT multilayers
    int MDTista[2];     // sta number for MDT multilayers
    float ZposCha;      // Z postion of the chamber
    float Zlength;      // chamber lenght along Z axis
    float RposCha;      // R position of the chamber
    float RmidCha;      // middle radius of the chamber
    float OrtRadp;      // orto radial shift of chamber
    float PposCha;      // phi position of the chamber
    float Lwidth;       // Y-width on long side
    float Swidth;       // Y-width on short side
    float Zwidth[2];    // multilayers Z-width
    float Rwidth[2];    // multilayers R-width
    float ZposMul[2];   // Z-position of multilayers
    float YposMul[2];   // R-position of multilayers
    float YposTub[10];  // radial coordinates of first tube per layer (1/10)
    float XposTub[10];  // zeta coordinates of first tube per layer (1/10)
    float TubePitch[2]; // pitch of tube per multilayer

//  ============== OPEN OUTPUT FILE FOR DUMPING THE GEOMETRY PARAMETERS =======

    ofstream geometry;  // instanciate the output file
    if(GeoOut)
    {
        geometry.open(GeoFile.c_str(), ios::out | ios::binary);
        if(!geometry) 
        {
            cout << "Cannot open file " << GeoFile.c_str()
	         << " in the working directory" << endl;
            return false;
        }
    }


//  =========== START LOOPING ON MULTI-CHAMBERS<-------------------------------
    for (int Jtype=amdc->StationTypeBegin();Jtype<=Mtype;++Jtype)
    {
        std::string name = amdc->GetStationType(Jtype);
	int MJzz = amdc->PositionZEnd();
	
	int IsValid;
	int MJgeo = amdc->StationGeomEnd();
	double DimLocX,DimLocY,DimLocZ,CenterLocX,CenterLocY,CenterLocZ;
	
	for(int Jgeo = amdc->StationGeomBegin();Jgeo < MJgeo;++Jgeo) {

	  int Jcut = 0;
	  amdc->GetStationDimensions(name,Jgeo,Jcut,IsValid,
	                             DimLocX,DimLocY,DimLocZ,
		                     CenterLocX,CenterLocY,CenterLocZ );
	
	  if (IsValid > 0){
	
	    // start looping over all the station
            for (int Jff=amdc->PositionPhiBegin();Jff<=Mff;++Jff) {
	      for (int Jzz=amdc->PositionZBegin();Jzz<=MJzz;++Jzz) {
	      
	        float SignZ = (Jzz<0)? -1. : +1.;
		  
                int PosiIsValid,PosiJgeo,PosiJcut,PosiIsBarrel;
                double PosiPhi,PosiZ,PosiR,PosiS;
                double PosiAlfa, PosiBeta,PosiGamma;
			     
                amdc->GetStationPositionParam(name,Jff,Jzz,PosiIsValid,
		                              PosiJgeo,PosiJcut,PosiIsBarrel,
		                              PosiPhi,PosiZ,PosiR,PosiS,
				              PosiAlfa,PosiBeta,PosiGamma);
					      
	        if ( PosiIsValid > 0 && PosiJgeo == Jgeo ) {
		
		  ++(ExistCham[Jtype-1]);
		  if(Jzz == 0) ++TZeroCham;

		  int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);

		  // get only MDT chambers
                  if(name.c_str()[0] == 'B' || name.c_str()[0] == 'E') {
		  
		    Iphi = (name.c_str()[2]=='L'||name.c_str()[2]=='R')?
			   (Jff-1)*2 : Jff*2 - 1;

                    Isystem = (name.c_str()[0]=='E')?  1 : 0;

                    ++(NumCham[Isystem][Iphi][Jzz+Mzz][0]);
		    
                    if(NumCham[Isystem][Iphi][Jzz+Mzz][0] < Mseq - 1) 
		    {
                       NumCham[Isystem][Iphi][Jzz+Mzz]
			     [ NumCham[Isystem][Iphi][Jzz+Mzz][0] ]=Jtype;
                    } else 
		    {
                      cout << "GEOMAP: warning stacking number more "
                           << "than MSEQ" << endl;
                    }

//                  ======================== INITIALIZE THE INCHA STRUCTURE

                    MLayer     = 0;
	            for(int i=0;i<2;++i) MDTlay[i]  = 0;
	            RPCcha     = 0;
	            ZposCha    = 0.;
	            Zlength    = 0.;
	            RposCha    = 0.;
	            RmidCha    = 0.;
	            OrtRadp    = 0.;
	            PposCha    = 0.;
                    Lwidth     = 0.;
                    Swidth     = 0.;
 	            for(int i=0;i<2;++i) Zwidth[i]  = 0.;
	            for(int i=0;i<2;++i) Rwidth[i]  = 0.;
	            for(int i=0;i<2;++i) ZposMul[i] = 0.;
	            for(int i=0;i<2;++i) YposMul[i] = 0.;
		    for(int i=0;i<2;++i) MDTitec[i] = 0;
                    for(int i=0;i<2;++i) MDTista[i] = 0;
		    
                    for(int i=0;i<10;++i) YposTub[i]  = 0.;
                    for(int i=0;i<10;++i) XposTub[i]  = 0.;
                    for(int i=0;i<2;++i) TubePitch[i] = 0.;


                    ZposCha = nor(PosiZ);
		    RposCha = nor(PosiR);
		    OrtRadp = nor(PosiS);
		    PposCha = PosiPhi;

		    enum tech {SPA,MDT,RPC};
		    int exist_tech[3] = {0,0,0};
		    

		    for (int Jobj=1;Jobj<=MJobj;++Jobj) {
		    
                      std::string GeomTechnoName;
	              int GeomIsValid,GeomTechnoIndex;
                      int GeomSplitX,GeomSplitY,GeomShapeIndex;
                      double GeomDx,GeomDy,GeomDz,GeomWs,GeomWl;
                      double GeomLe,GeomEx,GeomD1,GeomD2,GeomD3;
		      GeomDy = 0.;
                      amdc->GetStationObjectParam( name,Jgeo,Jobj,GeomIsValid,
		                                 GeomTechnoName,GeomTechnoIndex, 
		                                 GeomSplitX,GeomSplitY,
				                 GeomShapeIndex,GeomDx,GeomDy,
					         GeomDz,GeomWs,GeomWl,GeomLe,  
					         GeomEx,GeomD1,GeomD2,GeomD3 );


		     
		      int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
		      int Ista = amdc->ISTA(Jtype,Jgeo,Jobj);
                      std::string tech_name = GeomTechnoName;


                      if (tech_name=="SPA") exist_tech[SPA] = Jobj;
                      if (tech_name=="CHV") exist_tech[SPA] = Jobj;
			    
                      if (tech_name=="RPC") 
		      {
		        ++RPCcha; 
			exist_tech[RPC] = Jobj;
		      }
		      
		      if (tech_name=="MDT") 
                      {
                        exist_tech[MDT] = Jobj;
			++MLayer;
			//int MLi = (exist_tech[SPA])? 1 : 0;
                        int MLi = MLayer - 1;

                        if(nor(GeomLe)    > Zlength)  Zlength = nor(GeomLe);
			if(nor(GeomWl)/2. > Lwidth)   Lwidth  = nor(GeomWl)/2.;
                        if(nor(GeomWs)/2. > Swidth)   Swidth  = nor(GeomWs)/2.;

			MDTlay[MLi]  = amdc->NLAZ(Itec,Ista);
			Zwidth[MLi]  = nor(GeomLe)/2.;
			Rwidth[MLi]  = nor(amdc->STAEE(Itec,Ista))/2.;
                              
                        
			ZposMul[MLi]   = nor(GeomDy) + nor(GeomLe)/2.;
			YposMul[MLi]   = nor(GeomDz) + nor(amdc->STAEE(Itec,Ista))/2.;
			TubePitch[MLi] = nor(amdc->STAPP(Itec,Ista));
			
			
			if(!s_TubePitch) s_TubePitch = TubePitch[MLi];
                        
			
			// itec and ista should be the same for both 
			// multilayers. The are stored separately to increase
			// the code safeness.
			MDTitec[MLi] = Itec;
			MDTista[MLi] = Ista;
			
			/*
                        for (int lay=0;lay<MDTlay[MLi];++lay)
			{
			  float xco = nor(amdc->STAOO(Itec,Ista,lay+1));
                          float yco = nor(amdc->STATT(Itec,Ista,lay+1));

                          YposTub[lay+MLi*5] = yco - Rwidth[MLi];
                          XposTub[lay+MLi*5] = xco;
			}
			*/
                      }
		      
                    }  //end loop on station objects
		    
		    
		    if (MLayer==2 && YposMul[0] > YposMul[1])
		    {
		        std::cout << "Jtype=" << Jtype << ",  Jff=" << Jff
			          << ",  Jzz=" << Jzz << std::endl;
		        float tmp;
		        //imp=MDTlay[1];    MDTlay[1]=MDTlay[0];   MDTlay[0]=imp; 
			tmp=Zwidth[1];    Zwidth[1]=Zwidth[0];   Zwidth[0]=tmp;
			tmp=Rwidth[1];    Rwidth[1]=Rwidth[0];   Rwidth[0]=tmp;
			tmp=ZposMul[1];  ZposMul[1]=ZposMul[0]; ZposMul[0]=tmp;
			tmp=YposMul[1];  YposMul[1]=YposMul[0]; YposMul[0]=tmp;

			tmp = TubePitch[1];
			TubePitch[1] = TubePitch[0];
			TubePitch[0] = tmp;
			
			int imp;
			imp=MDTlay[1];    MDTlay[1]=MDTlay[0];   MDTlay[0]=imp;
			imp=MDTitec[1];  MDTitec[1]=MDTitec[0]; MDTitec[0]=imp;
			imp=MDTista[1];  MDTista[1]=MDTista[0]; MDTista[0]=imp;
		    }
		    
		    
		    for (int MLi=0;MLi<MLayer;++MLi)
		    {
		        for (int lay=0;lay<MDTlay[MLi];++lay)
                        {
			    float xco = nor(amdc->STAOO(MDTitec[MLi],MDTista[MLi],lay+1));
                            float yco = nor(amdc->STATT(MDTitec[MLi],MDTista[MLi],lay+1));

                            YposTub[lay+MLi*5] = yco - Rwidth[MLi];
                            XposTub[lay+MLi*5] = xco;
			}
                    }
		    
		    
		    
		    if (int Jobj = exist_tech[SPA])
		    {
		      int Ista = amdc->ISTA(Jtype,Jgeo,Jobj);
                      int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
		      
                      std::string GeomTechnoName;
	              int GeomIsValid,GeomTechnoIndex;
                      int GeomSplitX,GeomSplitY,GeomShapeIndex;
                      double GeomDx,GeomDy,GeomDz,GeomWs,GeomWl;
                      double GeomLe,GeomEx,GeomD1,GeomD2,GeomD3;
		      
                      amdc->GetStationObjectParam( name,Jgeo,Jobj,GeomIsValid,
		                                 GeomTechnoName,GeomTechnoIndex,
		                                 GeomSplitX,GeomSplitY,
				                 GeomShapeIndex,GeomDx,GeomDy,
					         GeomDz,GeomWs,GeomWl,GeomLe,  
					         GeomEx,GeomD1,GeomD2,GeomD3 );
		      
                      float Dzspa = nor(GeomDz);
                      float Wspa  = (GeomTechnoName == "CHV")? nor(amdc->STAPP(Itec,Ista))/2. :
		                                     nor(amdc->STAEE(Itec,Ista))/2.;

                      RmidCha = (Isystem)? ZposCha + SignZ*(Dzspa+Wspa):
                                           RposCha + Dzspa + Wspa;
		      
		      //if(s_testbeam) RmidCha = ZposCha + SignZ*(Dzspa+Wspa);
		      if(s_testbeam) RmidCha = RposCha + Dzspa + Wspa;

		    } 
		    else if (exist_tech[MDT])
		    {
		      RmidCha = (Isystem)? ZposCha + SignZ*YposMul[0]:
                                                 RposCha + YposMul[0];
                      
		      //if(s_testbeam) RmidCha = ZposCha + SignZ*YposMul[0];
		      if(s_testbeam) RmidCha = RposCha + YposMul[0];

		    } 
		    else if (int Jobj = exist_tech[RPC])
		    {
		      int Ista = amdc->ISTA(Jtype,Jgeo,exist_tech[RPC]);
		      int Itec = amdc->ITEC(Jtype,Jgeo,exist_tech[RPC]);
		      
		      std::string GeomTechnoName;
	              int GeomIsValid,GeomTechnoIndex;
                      int GeomSplitX,GeomSplitY,GeomShapeIndex;
                      double GeomDx,GeomDy,GeomDz,GeomWs,GeomWl;
                      double GeomLe,GeomEx,GeomD1,GeomD2,GeomD3;
		      
                      amdc->GetStationObjectParam( name,Jgeo,Jobj,GeomIsValid,
		                                 GeomTechnoName,GeomTechnoIndex, 
		                                 GeomSplitX,GeomSplitY,
				                 GeomShapeIndex,GeomDx,GeomDy,
					         GeomDz,GeomWs,GeomWl,GeomLe,  
					         GeomEx,GeomD1,GeomD2,GeomD3 );
		      
                      RmidCha=nor(PosiR)+nor(GeomDz)+nor(amdc->STATT(Itec,Ista,9))/2.;
		    }
		    	
		    if (!RmidCha) 
		    {
		      printf("MDTGeometry: error in computing Rmid!\n");
                      return false;
		    }



//                  =============== FILL THE INCHA STRUCTURE ==============
                    GEOMindex[Jtype-1][(Jff-1)*(2*MJzz+1)+Jzz+MJzz] = Jgeo;
		    incha.MLayer = MLayer;
		    
	            for (int i=0;i<2;++i) incha.MDTlay[i] = MDTlay[i];

	            incha.RPCcha  = RPCcha;
	            incha.ZposCha = ZposCha;
	            incha.Zlength = Zlength;
	            incha.RposCha = RposCha;
	            incha.RmidCha = RmidCha;
	            incha.OrtRadp = OrtRadp;
		    incha.Alpha   = PosiAlfa;
		    incha.Beta    = PosiBeta;
		    incha.Gamma   = PosiGamma;
	            incha.PposCha = PposCha;
		    incha.Lwidth  = Lwidth;
                    incha.Swidth  = Swidth;

                    for (int i=0;i<2;++i) incha.Zwidth[i]  = Zwidth[i];
                    for (int i=0;i<2;++i) incha.Rwidth[i]  = Rwidth[i];
 	            for (int i=0;i<2;++i) incha.ZposMul[i] = ZposMul[i];
                    for (int i=0;i<2;++i) incha.YposMul[i] = YposMul[i];

                    for (int i=0;i<10;++i) incha.YposTub[i] = YposTub[i];
                    for (int i=0;i<10;++i) incha.XposTub[i] = XposTub[i];

                    for (int i=0;i<2;++i) incha.TubePitch[i] =TubePitch[i];

//                  ========== DUMP THE GEOMETRY PARAMETERS IF REQUIRED
                    if(GeoOut) 
                    {
                      Header head; 
                      head.JJ  = Jtype;
                      head.JFF = Jff;
                      head.JZZ = Jzz;
                      geometry.write((char *) &head, sizeof head);
                      geometry.write((char *) &incha, sizeof incha);
                    }

		    if(! buildMDTgeo(Jtype,Jff,Jzz,exist_tech[MDT])) return false;
		    
		  } // end test on MDT chambers
		}  // end test on position validity

	      }  // end loop in Jzz
	    }  // end loop on Jff

          }  // end test on station validity
        }  // end loop on geometry index
    
    }  // end loop on station type


/*
    // test of the symmetrization rule
    for (int Jtype=amdc->StationTypeBegin();Jtype<=Mtype;++Jtype)
    {
        std::string name = amdc->GetStationType(Jtype);
	int MJzz = amdc->PositionZEnd();
	
	// start looping over all the station
        for (int Jff=amdc->PositionPhiBegin();Jff<=Mff;++Jff) {
	    for (int Jzz=1;Jzz<=MJzz;++Jzz) {
	      
            const Chamber* ch = T2muonGeo[Jtype-1][(Jff-1)*(2*MJzz+1)+Jzz+MJzz];
            if (ch) {
	        const Chamber* sh = T2muonGeo[Jtype-1][(Jff-1)*(2*MJzz+1)-Jzz+MJzz];
		if(!sh) {
		    cout << " Chamber " << name << " at Jff=" << Jff
		         << " and Jzz=" << Jzz 
			 << " doesn't have a symmetric one" << endl;
		}
		if(name[0]=='B') {
		    if (ch->Zmin != fabs(sh->Zmax) || ch->Zmax != fabs(sh->Zmin)) {
		        cout << " The chambers " << name << " at Jff=" << Jff
		             << " and Jzz= +/-" << Jzz << " are not symmetric:"
			     << endl;
                        cout << "+" << Jzz << ": Zmin=" << ch->Zmin
		             << ", Zmax=" << ch->Zmax << endl;
                        cout << "-" << Jzz << ": Zmin=" << sh->Zmin
		             << ", Zmax=" << sh->Zmax << endl; 
		    }
		}
		if(name[0]=='E') {
		    if (ch->Zmin != fabs(sh->Zmin) || ch->Zmax != fabs(sh->Zmax)) {
		        cout << " The chambers " << name << " at Jff=" << Jff
		             << " and Jzz= +/-" << Jzz << " are not symmetric:"
			     << endl;
                        cout << "+" << Jzz << ": Zmin=" << ch->Zmin
		             << ", Zmax=" << ch->Zmax << endl;
                        cout << "-" << Jzz << ": Zmin=" << sh->Zmin
		             << ", Zmax=" << sh->Zmax << endl; 
		    }
		}
	    }
	    }
	}
    }
    */
   
    if (GeoOut) geometry.close();

    if(MapFlag)
    {
	int TotMDT = 0;
	int TotTGC = 0;
	int TotCSC = 0;
	int TotCham = 0;

	for (int Jtype=1;Jtype<=Mtype;++Jtype)
	{
	    if(int num = ExistCham[Jtype-1])
	    {
	        std::string name = amdc->GetStationType(Jtype);
		cout << "   " << setw(3) << num << " chambers of type " 
                     << name  << endl;
		if      (name.c_str()[0] == 'T') TotTGC += num;
		else if (name.c_str()[0] == 'C') TotCSC += num;
		else TotMDT += num;
	    }
	}
	cout << "  Total MDT chambers = " << TotMDT << endl;
	cout << "  Total TGC chambers = " << TotTGC << endl;
	cout << "  Total CSC chambers = " << TotCSC << endl;

        int MCham = 0;
	int MJzz  = 0;

	cout << " Chamber stacking matrix:" << endl << endl;
	cout << "        Index                   Barrel"
	     << "                      Endcap" << endl;
	cout << " +-----------------+-----------------------------+" 
	     << "-----------------------------+" << endl;

	for (int I=0;I<16;++I)
	{
	    for (int J=-Mzz;J<=Mzz;++J)
	    {
#if (__GNUC__) && (__GNUC__ > 2) 
               // put your gcc 3.2 specific code her
	       __osstream barrel;
	       __osstream endcap;
#else
               // put your gcc 2.95 specific code here
	        char barrelBuff [80];
	        char endcapBuff [80];
	        for (int i=0;i<80;++i) barrelBuff[i] = '\0';
	        for (int i=0;i<80;++i) endcapBuff[i] = '\0';
                ostrstream barrel(barrelBuff,80);
                ostrstream endcap(endcapBuff,80);
#endif	    

		for (int H=0;H<2;++H)
		{
		    for (int K=1;K<Mseq;++K)
		    {
			if(H)
			{
			    int type = NumCham[H][I][J+Mzz][K];
			    if (type) endcap << amdc->GetStationType(type) << " ";
			    else endcap << "    ";

			} else
			{
			    int type = NumCham[H][I][J+Mzz][K];
			    if (type) barrel << amdc->GetStationType(type) << " ";
			    else barrel << "    ";
			}
		    }
		    if(int num = NumCham[H][I][J+Mzz][0])
		    {
			if(num > MCham) MCham = num;
			if (abs(J) > MJzz) MJzz = abs(J);
			TotCham = TotCham + num;
		    }
		}
		cout << " |IPHI=" << setw(2) << I << ", JZZ=" << setw(3) << J
		     << " | " << barrel.str() << "| " << endcap.str()
		     << "|" << endl;
	    }
	}

	cout << " +-----------------+-----------------------------+" 
	     << "-----------------------------+" << endl;
	cout << " Maximun MDT chamber for fixed ISYSTEM,IPHI,JZZ =" << MCham 
             << endl;
	cout << " Maximun JZZ index filled = " << MJzz << endl;
	cout << " Total chamber at JZZ = 0: " << TZeroCham << endl;
	cout << " Total stacked chambers = " << TotCham << endl;
    }

    return true; 
}


bool
MDTGeometry::setup_Stations()
{   
    Amdcsimrec* amdc = s_amdc;          // get the interface towards AMDC        
    
    //initialize the Radius array
    for(int i=0;i<16;++i)
        for (int j=0;j<3;++j)
	{
	    m_bStationsRadius[i][j][0] = 99999.;
	    m_bStationsRadius[i][j][1] = 1.;
	    m_bStationsPhi[i][j][0] = 4*s_pi;
	    m_bStationsPhi[i][j][1] = -4*s_pi;
	    m_bStationsTypes [i][j][0] = -1;
	    m_bStationsTypes [i][j][1] = -1;
	    for (int k=0;k<2;++k)
	    {
	        m_eStationsZeta[i][j][k][0] = 99999.;
	        m_eStationsZeta[i][j][k][1] = -99999.;
		m_eStationsPhi[i][j][k][0] = 4*s_pi;
		m_eStationsPhi[i][j][k][1] = -4*s_pi;
	        m_eStationsTypes [i][j][k][0] = -1;
	        m_eStationsTypes [i][j][k][1] = -1;
	    }
        }
	
    for (int Jtype=1;Jtype<=Mtype;++Jtype)
    {
        std::string name = amdc->GetStationType(Jtype);
	int MJzz = amdc->PositionZEnd();
        for (int Jff=amdc->PositionPhiBegin();Jff<=Mff;++Jff)
	{
	    for (int Jzz=amdc->PositionZBegin();Jzz<=MJzz;++Jzz)
	    {
		const Chamber* cha=T2muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		if(cha && cha->multi[0])
		{
		
		    float PhiMin = cha->Phipos - cha->DeltaPhi;
                    float PhiMax = cha->Phipos + cha->DeltaPhi;
		    if(PhiMin<0) PhiMin += 2*s_pi;
		    if(PhiMax<0) PhiMax += 2*s_pi;
		    
		    //convert to the new Phi coordinate system
		    if(PhiMin > s_pi) PhiMin -= 2*s_pi;
		    if(PhiMax > s_pi) PhiMax -= 2*s_pi;
		    
		    if(PhiMin > PhiMax) {
		        float tmp = PhiMax;
			PhiMax = PhiMin;
			PhiMin = tmp;
		    }
		    
		    
		    if (name.c_str()[0] == 'B' && name.c_str()[1] != 'E')
		    {
		        int sc = 0;
			int st = 0;
		        if(name.c_str()[2] == 'L' || name.c_str()[2] == 'R' ||
			   name.c_str()[2] == 'M') sc= Jff*2-2;
			else                                 sc = Jff*2-1;
			if(name.c_str()[1] == 'M') st = 1;
			if(name.c_str()[1] == 'O') st = 2;
			
			if(m_bStationsRadius[sc][st][0] >= cha->Rmin)
			   m_bStationsRadius[sc][st][0] = cha->Rmin;
			if(m_bStationsRadius[sc][st][1] <= cha->Rmax)
			   m_bStationsRadius[sc][st][1] = cha->Rmax;
			
			if(m_bStationsPhi[sc][st][0] >= PhiMin)
			   m_bStationsPhi[sc][st][0] = PhiMin;
			if(m_bStationsPhi[sc][st][1] <= PhiMax)
			   m_bStationsPhi[sc][st][1] = PhiMax;
			   
			int ty = Jtype;
			if(ty != m_bStationsTypes[sc][st][0] &&
			   ty != m_bStationsTypes[sc][st][1]   )
			{
			    if(m_bStationsTypes[sc][st][1] != -1)
			    {
			        std::cout <<"setup_Stations error: not enough "
				     << "space for Barrel Station Types!" 
				     << std::endl;
			        return false;
			    }
			    if(m_bStationsTypes[sc][st][0] == -1)
			         m_bStationsTypes[sc][st][0]  = ty;
			    else m_bStationsTypes[sc][st][1]  = ty;
			}
		    }
		    
		    if (name.c_str()[0] == 'E' )
		    {
		        int sc = 0;
			int st = 0;
		        int sd = (Jzz<=0)? 0 : 1;
			
			if(name.c_str()[2] == 'L') sc = Jff*2-2;
			else                       sc = Jff*2-1;
			if(name.c_str()[1] == 'M') st = 1;
			if(name.c_str()[1] == 'O') st = 2;
			
			
			if(m_eStationsZeta[sc][st][sd][0] >= cha->Zmin)
			   m_eStationsZeta[sc][st][sd][0] = cha->Zmin;
			if(m_eStationsZeta[sc][st][sd][1] <= cha->Zmax)
			   m_eStationsZeta[sc][st][sd][1] = cha->Zmax;
			 
			if(m_eStationsPhi[sc][st][sd][0] >= PhiMin)
			   m_eStationsPhi[sc][st][sd][0] = PhiMin;
			if(m_eStationsPhi[sc][st][sd][1] <= PhiMax)
			   m_eStationsPhi[sc][st][sd][1] = PhiMax;  
			
			int ty = Jtype;
			if(ty != m_eStationsTypes[sc][st][sd][0] &&
			   ty != m_eStationsTypes[sc][st][sd][1]   )
			{
			    if(m_eStationsTypes[sc][st][sd][1] != -1)
			    {
			        std::cout <<"setup_Stations error: not enough "
				     << "space for Endcap Station Types!" 
				     << std::endl;
			        return false;
			    }
			    if(m_eStationsTypes[sc][st][sd][0] == -1)
			               m_eStationsTypes[sc][st][sd][0]  = ty;
			    else       m_eStationsTypes[sc][st][sd][1]  = ty;
			}
		    }
		}
		
            }
	}
    }
    
    /*
    for (int sc=0;sc<16;++sc) 
        for (int st=0;st<3;++st)
	    for (int sd=0;sd<2;++sd) {
	    
	        std::cout << "sector "   << std::setw(2) << sc
		          << "/station " << std::setw(1) << st
			  << "/side "    << std::setw(1) << sd
			  << "/   PhiMin=" 
			  << m_eStationsPhi[sc][st][sd][0] << std::endl;
	        std::cout << "sector "   << std::setw(2) << sc
		          << "/station " << std::setw(1) << st
			  << "/side "    << std::setw(1) << sd
			  << "/   PhiMax=" 
			  << m_eStationsPhi[sc][st][sd][1] << std::endl;
	    }
    */
    return true;
}

void 
MDTGeometry::setTDCdim(TDCgeo* TDC,const float pitch) const
{  
    float ZMAX = -999999.;
    float RMAX = 0.;
    float RMIN = 999999.;
    float ZMIN = 999999.;

    for (int ch=0; ch<TDCch; ++ch)
    {
        if(TDC->RZ[ch][0]) 
	{
            if(TDC->RZ[ch][1] <= ZMIN) ZMIN = TDC->RZ[ch][1];
	    if(TDC->RZ[ch][1] >= ZMAX) ZMAX = TDC->RZ[ch][1];
	    if(TDC->RZ[ch][0] <= RMIN) RMIN = TDC->RZ[ch][0];
	    if(TDC->RZ[ch][0] >= RMAX) RMAX = TDC->RZ[ch][0];
        }
    }
    
    TDC->Rmin = RMIN - pitch/2.;
    TDC->Zmin = ZMIN - pitch/2.;
    TDC->Rmax = RMAX + pitch/2.;
    TDC->Zmax = ZMAX + pitch/2.;
}

TDCgeo*
MDTGeometry::createTDC(const unsigned short int id) const
{ 
    TDCgeo* tdc = new TDCgeo;
    tdc->Id = id;
    for (int c=0;c<TDCch;c++) 
    {
        tdc->RZ [c][0] = 0.;
	tdc->RZ [c][1] = 0.;
	tdc->Lay[c]    = 0;
	tdc->Tub[c]    = 0;
    } 
    return tdc;
}

bool 
MDTGeometry::isRecabled(int TYP,int COD) const
{
  if (Recabling)
  {
      for(int tdc=0;tdc<MaxTDC;tdc++)
          for(int ch=0;ch<TDCch;ch++)
	      if(Recabling[TYP][COD][tdc][ch][0]) return true;
  }
  
  return false;
}

bool 
MDTGeometry::buildMDTgeo(const int JJ, const int JFF, const int JZZ, 
                         const int JOB) const{

  int c,i,k,l,m,n,TubNum[MaxLay],TubCur[MaxLay],TDCnum[MaxMul],TDCpack;
  float Tubes[MaxLay][MaxTub][2],Zeta,Rad,ShiftPhi;
  char ChamType[4]={' ',' ',' ','\0'},Xstr[8][MaxTub+1],Xtube[9];
  Chamber *chamber;

  for(i=0;i<3;i++) {
    *(ChamType+i) = m_cham.type[JJ-1][i];
  }
  
  chamber = new Chamber;                                   // book new chamber.
  
  for (i=0;i<2;i++) {                            // initialize the new chamber.
    chamber->multi[i] = NULL;
  }
  for (i=0;i<MaxTDC;i++) chamber->TDC[i] = NULL;
  for (i=0;i<4;i++) chamber->Type[i] = ChamType[i];
     
  if (incha.MLayer>=MaxMul+1) {          // check for multilayer inconsistence.
    printf("T2mubgeo error: wrong multilayer number, MLayer = %2d\n",
            incha.MLayer);
    return false;
  }
  
/************************* Start of building routines *************************/
  
  if (ChamType[0]=='B' || s_testbeam) {           // START BLOCK FOR BUILDING BARREL CHAMBER.
    chamber->Zmin = incha.ZposCha + incha.ZposMul[0] - incha.Zlength/2.;
    chamber->Zmax = incha.ZposCha + incha.ZposMul[0] + incha.Zlength/2.;
    chamber->Ymid = incha.RmidCha;
    chamber->Xmid = incha.ZposCha;
    chamber->Amid = 0.; 
    
    //add half of tube pitch to the chamber Z dimension
    if (JZZ >=0) chamber->Zmax += incha.TubePitch[0]/2.;
    else         chamber->Zmin -= incha.TubePitch[0]/2.;
     
    chamber->OrtoRadialPos = incha.OrtRadp;
    if(incha.OrtRadp) ShiftPhi = atan(incha.OrtRadp/incha.RposCha);
    else ShiftPhi = 0.;
//    printf("PHI = %4.1f\n",incha.PposCha);
    chamber->Phipos = incha.PposCha + ShiftPhi;
//    printf("Jff = %2d , PHI = %4.1f\n",*JFF,chamber->Phipos);
    
    // compute the radial coverage via the tube position
    if(incha.MLayer==1) {
      float Ym1 = incha.RposCha + incha.YposMul[0];
      int   lastLay = incha.MDTlay[0] - 1;
      chamber->Rmin = Ym1 + incha.YposTub[0*5] - incha.TubePitch[0]/2.;
      chamber->Rmax = Ym1 + incha.YposTub[0*5+lastLay] + incha.TubePitch[0]/2.;
      //chamber->Rmin = incha.RposCha + incha.YposMul[0] - incha.Rwidth[0];
      //chamber->Rmax = incha.RposCha + incha.YposMul[0] + incha.Rwidth[0];
    } else if (incha.MLayer==2) {
      float Ym1 = incha.RposCha + incha.YposMul[0];
      float Ym2 = incha.RposCha + incha.YposMul[1];
      int   lastLay = incha.MDTlay[1] - 1;
      chamber->Rmin = Ym1 + incha.YposTub[0*5] - incha.TubePitch[0]/2.;
      chamber->Rmax = Ym2 + incha.YposTub[1*5+lastLay] + incha.TubePitch[0]/2.;
      //chamber->Rmin = incha.RposCha + incha.YposMul[0] - incha.Rwidth[0];
      //chamber->Rmax = incha.RposCha + incha.YposMul[1] + incha.Rwidth[1];
    } else if (incha.MLayer==0&&incha.RPCcha==1) {
      chamber->Rmin = incha.RposCha;
      chamber->Rmax = incha.RposCha + (incha.RmidCha - incha.RposCha)*2.;
    } else {
      printf("T2mubgeo error: undefined case for chamber dimension.");
      return false;
    }
    chamber->DeltaPhi = atan(incha.Lwidth/chamber->Rmin);
/************************ Start building RPC geometry ************************/
    /*
    for (k=0;k<incha.RPCcha;k++) {            // book new RPC chamber if exist.
      chamber->RPC[k] = new RPCgeo;   
      chamber->RPC[k]->Radplane[0] = incha.RPCrad[k*2];
      chamber->RPC[k]->Radplane[1] = incha.RPCrad[k*2+1];
    }
    */
/******************** Start building multilayer geometry *********************/
    for (k=0;k<MaxMul;k++) TDCnum[k] = 0;
    for (k=0;k<incha.MLayer;k++) {             // book new multilayer if exist.
      chamber->multi[k] = new Multilayer;
      chamber->multi[k]->TubeLayers = incha.MDTlay[k];
      chamber->multi[k]->TubesNumber =
                                  (int)(2.*incha.Zwidth[k]/incha.TubePitch[k]);
      if(JZZ>=0)
      {
        chamber->multi[k]->Zmin=incha.ZposCha + incha.ZposMul[k]-incha.Zwidth[k];
        chamber->multi[k]->Zmax=incha.ZposCha + incha.ZposMul[k]+incha.Zwidth[k]
                                              + nor(17);//added more than tubepitch/2
      }
      else
      {
        chamber->multi[k]->Zmin=incha.ZposCha + incha.ZposMul[k]-incha.Zwidth[k]
	                                      - nor(17);//added more than tubepitch/2;
        chamber->multi[k]->Zmax=incha.ZposCha + incha.ZposMul[k]+incha.Zwidth[k];
      }
      chamber->multi[k]->Rmin=incha.RposCha + incha.YposMul[k]-incha.Rwidth[k];
      chamber->multi[k]->Rmax=incha.RposCha + incha.YposMul[k]+incha.Rwidth[k];
      chamber->multi[k]->Ymid=incha.RposCha + incha.YposMul[k];


      // set the multilayer type
      if( fabs ( incha.XposTub[k*5] - (incha.TubePitch[k])/2.) <= 0.0020 )
      {
	  chamber->multi[k]->Type[0] = 'S';
          chamber->multi[k]->Type[1] = '\0';
      } else if ( fabs( incha.XposTub[k*5] - incha.TubePitch[k]) <= 0.0020 )
      {
	  chamber->multi[k]->Type[0] = 'Z';
          chamber->multi[k]->Type[1] = '\0';
      }
      else 
      {
	  cout << "T2mubgeo error: undefined multilayer type" << endl;
	  return false;
      }

      for (i=0;i<MaxTDC/2;i++) chamber->multi[k]->TDC[i] = NULL;
      if (chamber->multi[k]->TubesNumber>=MaxTub+1) {
        printf("T2mubgeo error: tubes number greater than %2d",MaxTub);
	return false;
      } else if (chamber->multi[k]->TubeLayers>=MaxLay+1) {
        printf("T2mubgeo error: MDT layer number greater than %1d",MaxLay);
	return false;
      }
      for(l=0;l<MaxLay;l++) {TubNum[l] = 0; TubCur[l] = 0;}
      for(l=0;l<MaxLay;l++) for(m=0;m<MaxTub;m++) for(n=0;n<2;n++)
        Tubes[l][m][n] = 0.;


      int Jtype = JJ;
      int Jff  = JFF;
      int Jzz  = JZZ;
      int Job  = JOB;
      int Jlay = 1;
      int Jtub = 1;
      std::string name = s_amdc->GetStationType(Jtype);
      int StationEta;
      int StationPhi;
      int Multilayer;
      int Layer;
      int Tube;
      
      s_amdc->GetMDTindexFromAMDC( name, Jff, Jzz, Job, Jlay, Jtub,
                             StationEta, StationPhi, Multilayer, Layer, Tube );
	  
      if(JZZ >= 0)
      {	
        for(l=0;l<MaxLay;l++) {    // store the R-Z coordinate for barrel tubes.
          m = 0;
          Zeta = chamber->multi[k]->Zmin + incha.XposTub[k*5+l];
	  Rad  = chamber->multi[k]->Ymid + incha.YposTub[k*5+l];
          while(Zeta+incha.TubePitch[k]/2.<=chamber->multi[k]->Zmax&&m<MaxTub) {
	    
	    double XYZ1[3];
            double XYZ2[3];
            double SZT1[3];
            double SZT2[3];
            double Radius;
	    
	    Multilayer = k+1;
	    Layer      = l+1;
	    Tube       = m+1;
	  
	    s_amdc->GetTubePositionFromMDTindex( name, StationEta, StationPhi, 
	              Multilayer, Layer, Tube, XYZ1, XYZ2, SZT1, SZT2, Radius );
	  
	    double XYZM[3] = { (XYZ1[0]+XYZ2[0])/2.,
	                       (XYZ1[1]+XYZ2[1])/2.,
		               (XYZ1[2]+XYZ2[2])/2. };
	  
	    double RAD  = nor(sqrt(XYZM[0]*XYZM[0] + XYZM[1]*XYZM[1]));
	    double ZETA = nor(XYZM[2]);

	    if (RAD!=0.) {
                //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
		//                 Tube,"Radius",Rad,RAD);
                //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
	        //                 Tube,"Zeta",Zeta,ZETA);
	        #ifndef LVL1_STANDALONE
	        if (s_hist_barrel_alignCorrOnRadius) 
	            s_hist_barrel_alignCorrOnRadius->Fill(Rad-RAD);
                if (s_hist_barrel_alignCorrOnZeta)
	            s_hist_barrel_alignCorrOnZeta->Fill(Zeta-ZETA);
	        #else
	        std::cout << "Rad=" << Rad << ",  RAD=" << RAD 
	                  << ":   X1=" << XYZ1[0]
		          << ", X2=" << XYZ2[0]
		          << " ... Y1=" << XYZ1[1]
		          << ", Y2=" << XYZ2[1] << std::endl;
	        std::cout << "Zeta=" << Zeta << ",  ZETA=" << ZETA 
	                  << ":   Z1=" << XYZ1[2]
		          << ", Z2=" << XYZ2[2] << std::endl;
	        #endif
            }
	    
	    if (s_useOldGeoAccess) {
	        Tubes[l][m][0] = Rad;
	        Tubes[l][m][1] = Zeta;
	    } else {
	        Tubes[l][m][0] = RAD;
	        Tubes[l][m][1] = ZETA;
	    }
	    Zeta += incha.TubePitch[k];
	    m++;
	  }
	  TubNum[l] = m;
        }
      }
      else
      {
        for(l=0;l<MaxLay;l++) {    // store the R-Z coordinate for endcap tubes.
          m = 0;
          Zeta = chamber->multi[k]->Zmax - incha.XposTub[k*5+l];
	  Rad  = chamber->multi[k]->Ymid + incha.YposTub[k*5+l];
          while(Zeta-incha.TubePitch[k]/2.>=chamber->multi[k]->Zmin&&m<MaxTub) {
	    double XYZ1[3];
            double XYZ2[3];
            double SZT1[3];
            double SZT2[3];
            double Radius;
	    
	    Multilayer = k+1;
	    Layer      = l+1;
	    Tube       = m+1;
	  
	    s_amdc->GetTubePositionFromMDTindex( name, StationEta, StationPhi, 
	              Multilayer, Layer, Tube, XYZ1, XYZ2, SZT1, SZT2, Radius );
	  
	    double XYZM[3] = { (XYZ1[0]+XYZ2[0])/2.,
	                       (XYZ1[1]+XYZ2[1])/2.,
		               (XYZ1[2]+XYZ2[2])/2. };
	  
	    double RAD  = nor(sqrt(XYZM[0]*XYZM[0] + XYZM[1]*XYZM[1]));
	    double ZETA = nor(XYZM[2]);
	    
	    if (RAD!=0.) {
	        //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
		//                 Tube,"Radius",Rad,RAD);
                //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
		//                 Tube,"Zeta",Zeta,ZETA);
	        #ifndef LVL1_STANDALONE
	        if (s_hist_endcap_alignCorrOnRadius)
	            s_hist_endcap_alignCorrOnRadius->Fill(Rad-RAD);
                if (s_hist_endcap_alignCorrOnZeta)
	            s_hist_endcap_alignCorrOnZeta->Fill(Zeta-ZETA);
	        #else
	        std::cout << "Rad=" << Rad << ",  RAD=" << RAD 
	                  << ":   X1=" << XYZ1[0]
		          << ", X2=" << XYZ2[0]
		          << " ... Y1=" << XYZ1[1]
		          << ", Y2=" << XYZ2[1] << std::endl;
	        std::cout << "Zeta=" << Zeta << ",  ZETA=" << ZETA 
	                  << ":   Z1=" << XYZ1[2]
		          << ", Z2=" << XYZ2[2] << std::endl;
	        #endif
	    }
	    
	    if (s_useOldGeoAccess) {
	        Tubes[l][m][0] = Rad;
	        Tubes[l][m][1] = Zeta;
	    } else {
	        Tubes[l][m][0] = RAD;
	        Tubes[l][m][1] = ZETA;
	    }
	    Zeta -= incha.TubePitch[k];
	    m++;
	  }
	  TubNum[l] = m;
        }
      }
      
/********************** Start bulding the TDCs readout ***********************/
      TDCpack = TDCch/chamber->multi[k]->TubeLayers;
      TDCnum[k] = chamber->multi[k]->TubesNumber/TDCpack;
      if(TDCnum[k]*TDCpack!=chamber->multi[k]->TubesNumber) TDCnum[k]++;
      if(TDCnum[k]>=(MaxTDC/2)+1) {
        printf("T2mubgeo error: TDC number for 1 multilayer greater than %2d",
	       MaxTDC/2);
	return false;
      }
      
      int TYP = JJ-1;
      int COD = (JFF-1)*(2*Mzz+1)+JZZ+Mzz;
      bool doRecabling = isRecabled(TYP,COD);
      
      if( doRecabling )
      {
	int num = 0;
        for(int tdc=0;tdc<MaxTDC;++tdc)
	{
	  int ch = 0;
	  while(Recabling[TYP][COD][tdc][ch][0]==0 && ch < TDCch-1) ch++;
	  
	  int multilayer = Recabling[TYP][COD][tdc][ch][0];
          
	  if( multilayer-1 == k )
	  {
	    if(num>=TDCnum[k])
	    {
	      std::cout << "chamber->multi[k]->TubeLayers=" 
	                << chamber->multi[k]->TubeLayers << std::endl;
              std::cout << "chamber->multi[k]->TubesNumbe=" 
	                << chamber->multi[k]->TubesNumber << std::endl;
	      std::cout << "TDCnum=" << TDCnum[k] 
	                << ",  num=" << num << std::endl;
	      std::cout << "Jtype=" << TYP
	                << ", Jcode=" << COD
	                << ", JFF=" << JFF
			<< ", JZZ=" << JZZ << std::endl;
	      std::cout << "T2mubgeo error: mismatch between the TDC number per multilayer "
	                << std::endl
			<< "computed and that retrieved for the mezanine map"
			<< std::endl;
              return false;
	    }
	    chamber->multi[k]->TDC[num] = createTDC(tdc);
	    num++;
	  }
	}
      }
       
      for (i=0;i<TDCnum[k];i++) {                    // book new TDC if exist.
        
	if( doRecabling )
	{   
	  TDCgeo* tdc = chamber->multi[k]->TDC[i];
	  if(tdc==NULL)
	  {
	    int type = JJ;
	    std::string name = s_amdc->GetStationType(type);
            /*
	    std::cout << "Chamber: " << name
		      << "  JFF: " << JFF
		      << "  JZZ: " << JZZ << std::endl;
	    std::cout << "T2mubgeo error: mezanine map file contains less TDC than expected "
	              << std::endl;
            std::cout << "TDCnum[k]=" << TDCnum[k]
		      << ",  k=" << k
		      << ",  i=" << i << std::endl;
	    */
	  }
          else
	  {
            for(int ch=0;ch<TDCch;ch++)
	    {
	      if(Recabling[TYP][COD][tdc->Id][ch][1]!=0)
	      {
	        int layer = Recabling[TYP][COD][tdc->Id][ch][1];
		int tube  = Recabling[TYP][COD][tdc->Id][ch][2];
	        tdc->Lay[ch]   = layer + k*chamber->multi[k]->TubeLayers;
	        tdc->Tub[ch]   = tube;
	        tdc->RZ[ch][0] = Tubes[layer-1][tube-1][0];
	        tdc->RZ[ch][1] = Tubes[layer-1][tube-1][1];
	      }
	    }
          }
	} 
	else
	{
	  chamber->multi[k]->TDC[i] = createTDC(i + k*TDCnum[0]);
	  c = 0;
	  for(l=0;l<chamber->multi[k]->TubeLayers;l++) {
	    m = 1;
	    
	    if (!s_shift_LinkId) {
	         if( ChamType[2]=='R' && JFF==8 && abs(JZZ)==3 && i==0 ) m=4;
	         if( ChamType[2]=='R' && abs(JZZ)==4 && k==0 && i==0 ) m=4;
	    }
	     
	    while(m<=TDCpack&&TubCur[l]<=TubNum[l]-1) {
	    chamber->multi[k]->TDC[i]->RZ[c][0] = Tubes[l][(TubCur[l])][0];
	    chamber->multi[k]->TDC[i]->RZ[c][1] = Tubes[l][(TubCur[l])][1];
	    chamber->multi[k]->TDC[i]->Lay[c] = l+1 +
	                                       k*chamber->multi[k]->TubeLayers;
	    chamber->multi[k]->TDC[i]->Tub[c] = TubCur[l] + 1;
	    m++;
	    c++;
	    (TubCur[l])++;
	    }
	  }
	}
/******************** Compute the TDC dimension occupancy ********************/

        if(chamber->multi[k]->TDC[i])
	{
          setTDCdim(chamber->multi[k]->TDC[i],incha.TubePitch[k]);
	  chamber->TDC[chamber->multi[k]->TDC[i]->Id] = chamber->multi[k]->TDC[i];
        }
      }
    }


/************************** Set the linkers to TDCs **************************/
/*    n = 0;  
    for(k=0;k<MaxMul;k++) {
      for(i=0;i<TDCnum[k];i++) 
      {
          chamber->TDC[i+n] = chamber->multi[k]->TDC[i];
          chamber->multi[k]->TDC[i]->Id = i+n;
      }
      n += TDCnum[k];
    }*/
    chamber->NumTDC = TDCnum[0] + TDCnum[1];
        
    
  } else if (ChamType[0]=='E'){     // START BLOCK FOR BUILDING ENDCAP CHAMBER.
    chamber->Rmin = incha.RposCha;
    chamber->Rmax = incha.RposCha + incha.Zlength + incha.TubePitch[0]/2.;// + 1.5;  //added tubepitch/2.
    chamber->Zmin = incha.ZposCha;
    chamber->Ymid = incha.RmidCha;
    chamber->Xmid = incha.RposCha;
    chamber->Amid = 0.;
    chamber->OrtoRadialPos = incha.OrtRadp;
    if(incha.OrtRadp) ShiftPhi = atan(incha.OrtRadp/incha.RposCha);
    else ShiftPhi = 0.;
    chamber->Phipos = incha.PposCha + ShiftPhi;
    
    // compute the chamber Z coverage using the tube position
    if(incha.MLayer==1) {
      float Ym1 = (JZZ<=0)? incha.ZposCha - incha.YposMul[0]:
                            incha.ZposCha + incha.YposMul[0];
      float pitch = incha.TubePitch[0]/2.;
      int lastLay =  incha.MDTlay[0] - 1;
      
      chamber->Zmin=(JZZ<=0)? Ym1 - incha.YposTub[0*5] + pitch:
	                      Ym1 + incha.YposTub[0*5] - pitch;
      chamber->Zmax=(JZZ<=0)? Ym1 - incha.YposTub[0*5+lastLay] - pitch:
	                      Ym1 + incha.YposTub[0*5+lastLay] + pitch;
			     
      //chamber->Zmax=(JZZ<=0)? incha.ZposCha-(incha.YposMul[0]+incha.Rwidth[0]):
      //                        incha.ZposCha+incha.YposMul[0]+incha.Rwidth[0];
    } else if (incha.MLayer==2) {
      float Ym1 = (JZZ<=0)? incha.ZposCha - incha.YposMul[0]:
                            incha.ZposCha + incha.YposMul[0];
      float Ym2 = (JZZ<=0)? incha.ZposCha - incha.YposMul[1]:
                            incha.ZposCha + incha.YposMul[1];

      float pitch = incha.TubePitch[0]/2.;
      int lastLay =  incha.MDTlay[1] - 1;
      
      chamber->Zmin=(JZZ<=0)? Ym1 - incha.YposTub[0*5] + pitch:
	                      Ym1 + incha.YposTub[0*5] - pitch;
      chamber->Zmax=(JZZ<=0)? Ym2 - incha.YposTub[1*5+lastLay] - pitch:
	                      Ym2 + incha.YposTub[1*5+lastLay] + pitch;
    
      //chamber->Zmax=(JZZ<=0)? incha.ZposCha-(incha.YposMul[1]+incha.Rwidth[1]):
      //                        incha.ZposCha+incha.YposMul[1]+incha.Rwidth[1];
    } else {
      printf("T2mubgeo error: undefined case for chamber dimension.");
      return false;
    }
    chamber->DeltaPhi = atan(incha.Swidth/chamber->Rmin);
    if(atan(incha.Lwidth/chamber->Rmax) > chamber->DeltaPhi)
      chamber->DeltaPhi = atan(incha.Lwidth/chamber->Rmax);
/********************* Start building multilayer geometry ********************/
    for (k=0;k<MaxMul;k++) TDCnum[k] = 0;
    for (k=0;k<incha.MLayer;k++) {             // book new multilayer if exist.
      chamber->multi[k] = new Multilayer;
      chamber->multi[k]->TubeLayers = incha.MDTlay[k];
      chamber->multi[k]->TubesNumber = 
                                  (int)(2.*incha.Zwidth[k]/incha.TubePitch[k]);
      chamber->multi[k]->Zmin=(JZZ<=0)? 
                              incha.ZposCha-(incha.YposMul[k]-incha.Rwidth[k]):
                              incha.ZposCha+incha.YposMul[k]-incha.Rwidth[k];
      chamber->multi[k]->Zmax=(JZZ<=0)? 
                              incha.ZposCha-(incha.YposMul[k]+incha.Rwidth[k]):
                              incha.ZposCha+incha.YposMul[k]+incha.Rwidth[k];
      chamber->multi[k]->Rmin=incha.RposCha + incha.ZposMul[k]-incha.Zwidth[k];
      chamber->multi[k]->Rmax=incha.RposCha + incha.ZposMul[k]+incha.Zwidth[k]
                                           + nor(17);//added  more than tubepitch/2
      chamber->multi[k]->Ymid=(JZZ<=0) ?      incha.ZposCha - incha.YposMul[k]:
                                              incha.ZposCha + incha.YposMul[k];

      // set the multilayer type
      if( fabs( incha.XposTub[k*5] - (incha.TubePitch[k])/2.) <= 0.0020 )
      {
	  chamber->multi[k]->Type[0] = 'S';
          chamber->multi[k]->Type[1] = '\0';
      } else if ( fabs (incha.XposTub[k*5] - incha.TubePitch[k]) <= 0.0020 )
      {
	  chamber->multi[k]->Type[0] = 'Z';
          chamber->multi[k]->Type[1] = '\0';
      }
      else 
      {
	  cout << "T2mubgeo error: undefined multilayer type" << endl;
	  return false;
      }

      for (i=0;i<MaxTDC/2;i++) chamber->multi[k]->TDC[i] = NULL;
      if (chamber->multi[k]->TubesNumber>=MaxTub+1) {
        printf("T2mubgoe error: tubes number greater than %2d",MaxTub);
	return false;
      } else if (chamber->multi[k]->TubeLayers>=MaxLay+1) {
        printf("T2mubgoe error: MDT layer number greater than %1d",MaxLay);
	return false;
      }
      for(l=0;l<MaxLay;l++) {TubNum[l] = 0; TubCur[l] = 0;}
      for(l=0;l<MaxLay;l++) for(m=0;m<MaxTub;m++) for(n=0;n<2;n++)
        Tubes[l][m][n] = 0.;
      for(l=0;l<MaxLay;l++) {            // store the R-Z coordinate for tubes.
        m = 0;
        Zeta = (JZZ<=0) ?  chamber->multi[k]->Ymid - incha.YposTub[k*5+l]:
	                   chamber->multi[k]->Ymid + incha.YposTub[k*5+l];
	Rad  = chamber->multi[k]->Rmin + incha.XposTub[k*5+l];
        
	int Jtype = JJ;
	int Jff  = JFF;
	int Jzz  = JZZ;
	int Job  = JOB;
	int Jlay = 1;
	int Jtub = 1;
	std::string name = s_amdc->GetStationType(Jtype);
	int StationEta;
	int StationPhi;
	int Multilayer;
	int Layer;
	int Tube;
	s_amdc->GetMDTindexFromAMDC( name, Jff, Jzz, Job, Jlay, Jtub,
                              StationEta, StationPhi, Multilayer, Layer, Tube );
	
	while(Rad+incha.TubePitch[k]/2.<=chamber->multi[k]->Rmax&&m<MaxTub) {
	  
	  double XYZ1[3];
	  double XYZ2[3];
	  double SZT1[3];
	  double SZT2[3];
	  double Radius;
	  
	  Multilayer = k+1;
	  Layer      = l+1;
	  Tube       = m+1;
	  
	  s_amdc->GetTubePositionFromMDTindex( name, StationEta, StationPhi, 
	              Multilayer, Layer, Tube, XYZ1, XYZ2, SZT1, SZT2, Radius );
	  
	  double XYZM[3] = { (XYZ1[0]+XYZ2[0])/2.,
	                     (XYZ1[1]+XYZ2[1])/2.,
			     (XYZ1[2]+XYZ2[2])/2. };
	  
	  double RAD  = nor(sqrt(XYZM[0]*XYZM[0] + XYZM[1]*XYZM[1]));
	  double ZETA = nor(XYZM[2]);
	  
	  if (RAD!=0.) {
                //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
		//                 Tube,"Radius",Rad,RAD);
                //checkCorrections(name,StationEta,StationPhi,Multilayer,Layer,
	        //                 Tube,"Zeta",Zeta,ZETA);
	        #ifndef LVL1_STANDALONE
	        if (s_hist_endcap_alignCorrOnRadius) 
	            s_hist_endcap_alignCorrOnRadius->Fill(Rad-RAD);
                if (s_hist_endcap_alignCorrOnZeta)
	            s_hist_endcap_alignCorrOnZeta->Fill(Zeta-ZETA);
	        #else
	        std::cout << "Rad=" << Rad << ",  RAD=" << RAD 
	                  << ":   X1=" << XYZ1[0]
		          << ", X2=" << XYZ2[0]
		          << " ... Y1=" << XYZ1[1]
		          << ", Y2=" << XYZ2[1] << std::endl;
	        std::cout << "Zeta=" << Zeta << ",  ZETA=" << ZETA 
	                  << ":   Z1=" << XYZ1[2]
		          << ", Z2=" << XYZ2[2] << std::endl;
	        #endif
            }

          if (s_useOldGeoAccess) {
	      Tubes[l][m][0] = Rad;
	      Tubes[l][m][1] = Zeta;
	  } else {
	      Tubes[l][m][0] = RAD;
	      Tubes[l][m][1] = ZETA;
	  }
	  Rad += incha.TubePitch[k];
	  m++;
	}
	TubNum[l] = m;
      }
/********************** Start bulding the TDCs readout ***********************/
      TDCpack = TDCch/chamber->multi[k]->TubeLayers;
      TDCnum[k] = chamber->multi[k]->TubesNumber/TDCpack;
      if(TDCnum[k]*TDCpack!=chamber->multi[k]->TubesNumber) TDCnum[k]++;
      if(TDCnum[k]>=(MaxTDC/2)+1) {
        printf("T2mubgeo error: TDC number for 1 multilayer graeter than %2d",
	       MaxTDC/2);
	return false;
      }

      int TYP = JJ-1;
      int COD = (JFF-1)*(2*Mzz+1)+JZZ+Mzz;
      bool doRecabling = isRecabled(TYP,COD);
      
      if( doRecabling )
      {
	int num = 0;
        for(int tdc=0;tdc<MaxTDC;++tdc)
	{
	  int ch = 0;
	  while(Recabling[TYP][COD][tdc][ch][0]==0 && ch < TDCch-1) ch++;
	  
	  int multilayer = Recabling[TYP][COD][tdc][ch][0];
	  if( (multilayer-1)==k )
	  {
	    if(num>=TDCnum[k])
	    {
	      std::cout << "TDCnum=" << TDCnum[k] 
	                << ",  num=" << num << std::endl;
	      std::cout << "TYP=" << TYP
	                << ", JFF=" << JFF
			<< ", JZZ=" << JZZ << std::endl;
	      std::cout << "T2mubgeo error: mismatch between the TDC number per multilayer "
	                << std::endl
			<< "computed and that retrieved for the mezanine map"
			<< std::endl;
              return false;
	    }
	    chamber->multi[k]->TDC[num] = createTDC(tdc);
	    num++;
	  }
	}
      }
      
      for (i=0;i<TDCnum[k];i++) {                    // book new TDC if exist.
        if(doRecabling)
	{   
	  TDCgeo* tdc = chamber->multi[k]->TDC[i];
	  if(tdc==NULL)
	  {	    
	    int type = JJ;
	    std::string name = s_amdc->GetStationType(type);
            /*
	    std::cout << "Chamber: " << name
		      << "  JFF: " << JFF
		      << "  JZZ: " << JZZ << std::endl;
	    std::cout << "T2mubgeo error: mezanine map file contains less TDC than expected "
	              << std::endl;
            std::cout << "TDCnum[k]=" << TDCnum[k]
		      << ",  k=" << k
		      << ",  i=" << i << std::endl;
	    std::cout << "T2mubgeo error: mezanine map file contains less TDC than expected "
	              << std::endl;
	    */
	  }
          else
	  {
            for(int ch=0;ch<TDCch;ch++)
	    {
	      if(Recabling[TYP][COD][tdc->Id][ch][1]!=0)
	      {
	        int layer = Recabling[TYP][COD][tdc->Id][ch][1];
		int tube  = Recabling[TYP][COD][tdc->Id][ch][2];
	        tdc->Lay[ch]   = layer + k*chamber->multi[k]->TubeLayers;
	        tdc->Tub[ch]   = tube;
	        tdc->RZ[ch][0] = Tubes[layer-1][tube-1][0];
	        tdc->RZ[ch][1] = Tubes[layer-1][tube-1][1];
	      }
	    }
	  }
	} 
	else
	{
	  chamber->multi[k]->TDC[i] = createTDC(i + k*TDCnum[0]);
	  c = 0;
	  for(l=0;l<chamber->multi[k]->TubeLayers;l++) {
	    m = 1;
	    while(m<=TDCpack&&TubCur[l]<=TubNum[l]-1) {
	      chamber->multi[k]->TDC[i]->RZ[c][0] = Tubes[l][(TubCur[l])][0];
	      chamber->multi[k]->TDC[i]->RZ[c][1] = Tubes[l][(TubCur[l])][1];
	      chamber->multi[k]->TDC[i]->Lay[c] = l+1 +
	                                       k*chamber->multi[k]->TubeLayers;
						
              chamber->multi[k]->TDC[i]->Tub[c] = TubCur[l] + 1;
	      m++;
	      c++;
	      (TubCur[l])++;
	    }
	  }
	}
/******************** Compute the TDC dimension occupancy *********************/
        if(chamber->multi[k]->TDC[i])
	{
          setTDCdim(chamber->multi[k]->TDC[i],incha.TubePitch[k]);
	  chamber->TDC[chamber->multi[k]->TDC[i]->Id] = chamber->multi[k]->TDC[i];
        }
      }
    }
/************************** Set the linkers to TDCs **************************/
/*    n = 0;
    for(k=0;k<MaxMul;k++) {
      for(i=0;i<TDCnum[k];i++) 
      {
          chamber->TDC[i+n] = chamber->multi[k]->TDC[i];
          chamber->multi[k]->TDC[i]->Id = i+n;
      }
      n += TDCnum[k];
    }*/    
    chamber->NumTDC = TDCnum[0] + TDCnum[1];
  } 

/************************* End of building routines **************************/
  
//  printf("End of building routines\n");
  
  T2muonGeo[JJ-1][(JFF-1)*(2*Mzz+1)+JZZ+Mzz] = chamber;

  /*
  cout << "Jtype=" << JJ << ", (JFF-1)*(2*Mzz+1)+JZZ+Mzz=" 
       << (JFF-1)*(2*Mzz+1)+JZZ+Mzz << " : ==> "; 
  cout << "Alfa="    << setprecision(10) << incha.Alpha 
       << ", Beta="  << setprecision(10) << incha.Beta
       << ", Gamma=" << setprecision(10) << incha.Gamma << endl;
  */
  //std::cout << "before rotation" << std::endl;
  //if((JJ-1)==7 && (JFF-1)*(2*Mzz+1)+JZZ+Mzz == 165) {std::cout << "before rotation" << std::endl;checkBIR(t2muongeo());}

  //bool fl = false;
  //if((JJ-1)==2 && JZZ==-1 && (JFF==2 || JFF==3)) fl = true;
  
  //rotateChamber(chamber,incha.ZposCha,incha.RposCha,incha.OrtRadp,
  //                      incha.Alpha, incha.Beta, incha.Gamma,fl);
  
/*
  //std::cout << "after rotation" << std::endl;
  if((JJ-1)==7 && (JFF-1)*(2*Mzz+1)+JZZ+Mzz == 165) {std::cout << "after rotation" << std::endl;checkBIR(t2muongeo());}
*/  			
  if (GeoFlag) {
  
/******************************************************************************
*                  Start printout for common block values passed              *
******************************************************************************/
    printf("\n CHAMBER %s at JFF = %2d and JZZ = %3d got by t2mubgeo!\n",
         ChamType,JFF,JZZ);
    printf(" %1d multiLayer, ZposCha = %10.2f, Zlength = %10.2f\n",
           incha.MLayer,incha.ZposCha,incha.Zlength);
    printf("               RposCha = %10.2f, YposCha = %10.2f\n",incha.RposCha,
         incha.RmidCha);
    for(i=0;i<2;i++) {
      printf(
      "   multilayer %1d: Zwidth = %9.3f, Rwidth = %9.3f, TubeLay = %1d\n",i+1,
               incha.Zwidth[i],incha.Rwidth[i],incha.MDTlay[i]);
      printf("   ZposMul = %9.3f, YposMul = %9.3f, TubePitch = %4.8f\n",
               incha.ZposMul[i],incha.YposMul[i],incha.TubePitch[i]);
      printf("   Y coordinate for tube center -->");
      for(k=0;k<5;k++) {
        printf("%7.3f,",incha.YposTub[k+i*5]);
      }
      printf("\n   X coordinate for tube center -->");
      for(k=0;k<5;k++) {
        printf("%7.3f,",incha.XposTub[k+i*5]);
      }
      printf("\n");
    }
    printf(" %1d RPC multilayer:\n",incha.RPCcha);
    /*
    for(i=0;i<2;i++) {
      printf("   multilayer %d radii: R1 = %9.3f, R2 = %9.3f\n",i+1,
             incha.RPCrad[i*2],incha.RPCrad[i*2+1]);
    }
    */
    
/******************************************************************************
*                     Start printout for chamber booked                       *
******************************************************************************/
    printf("\nChamber inserted in T2muonGeo:\n");
    printf(" type = %s, Number of readout TDC = %2d\n",chamber->Type,
             chamber->NumTDC);
 printf(" dimension: Zmin = %8.2f, Zmax = %8.2f, Rmin = %8.2f, Rmax = %8.2f\n",
             chamber->Zmin,chamber->Zmax ,chamber->Rmin,chamber->Rmax);
   if(chamber->Type[0]=='E')printf("            Zmid = %8.2f\n",chamber->Ymid);
    else printf("            Rmid = %8.2f\n",chamber->Ymid);
    for(i=0;i<2;i++) {
      if(chamber->multi[i]) {
        printf(" MDT multilayer %1d: Type %s, Layers = %1d, Tubes = %2d\n",
                 i+1,chamber->multi[i]->Type,
	         chamber->multi[i]->TubeLayers,chamber->multi[i]->TubesNumber);
   printf(" dimension: Zmin = %8.2f, Zmax = %8.2f, Rmin = %8.2f, Rmax =%8.2f\n"
      ,chamber->multi[i]->Zmin,chamber->multi[i]->Zmax,chamber->multi[i]->Rmin,
       chamber->multi[i]->Rmax);
	if(chamber->Type[0]=='E') 
	     printf("            Zmid = %8.2f\n",chamber->multi[i]->Ymid);
	else printf("            Rmid = %8.2f\n",chamber->multi[i]->Ymid);
	
/************** Start printout for first tube layer coordinate ***************/
	printf(" Coordinate R-Z of first layer tube center (cm):\n");
        Rad = chamber->multi[i]->TDC[0]->RZ[0][0];
	Zeta = chamber->multi[i]->TDC[0]->RZ[0][1];
        m = 0;
        for(k=0;k<MaxTDC/2;k++){
          if(chamber->multi[i]->TDC[k]) {
	    c = 0;
	    if(chamber->Type[0]=='B') { // store tube coordinate for barrel ch.
	      //while(chamber->multi[i]->TDC[k]->RZ[c][0]==Rad) {
	      while(chamber->multi[i]->TDC[k]->Lay[c] == 1 +
	                      chamber->multi[0]->TubeLayers*i ) {
	        sprintf(Xtube,"%8.1f",chamber->multi[i]->TDC[k]->RZ[c++][1]);
	        for (n=0;n<8;n++) Xstr[n][m] = Xtube[n];
	        m++;
	      }
	    } else {                    // store tube coordinate for endcap ch.
	      //while(chamber->multi[i]->TDC[k]->RZ[c][1]==Zeta) {
	      while(chamber->multi[i]->TDC[k]->Lay[c] == 1 +
	                       chamber->multi[0]->TubeLayers*i ) {
	        sprintf(Xtube,"%8.1f",chamber->multi[i]->TDC[k]->RZ[c++][0]);
	        for (n=0;n<8;n++) Xstr[n][m] = Xtube[n];
	        m++;
	      }
	    }
	  }
        }
        for (n=0;n<8;n++) Xstr[n][m] = '\0';
        if(chamber->Type[0]=='B') printf("\n%7.1f",Rad);
	else printf("\n%7.1f",Zeta);
        myprin(' ',1);
        for (n=0;n<chamber->multi[i]->TubesNumber;n++) printf("O");
        printf("\n");
        for (n=0;n<8;n++) printf("        %s\n",Xstr[n]); 
        printf("\n");
      }
    }
/**********************  Start printout for RPC layers ***********************/
    /*
    for(i=0;i<2;i++) {               
      if(chamber->RPC[i]) {
        printf(" RPC multilayer %1d:\n",i+1);
        printf(" dimension: R1 = %8.2f, R2 = %8.2f\n",
	    chamber->RPC[i]->Radplane[i*2+1],chamber->RPC[i]->Radplane[i*2+1]);
      }
    }
    */

    for(i=0;i<80;i++) printf("*");
    printf("\n");
  }

  return true;
}

void
MDTGeometry::rotateChamber(Chamber* cham, 
                           float ZposCha, float RposCha, float /*OrtoRadp*/,
                           float Alfa, float Beta, float Gamma,bool fl) const
{
    
    Alfa  = -Alfa*s_pi/180;
    Beta  = -Beta*s_pi/180;
    Gamma = -Gamma*s_pi/180;
    
    Multilayer* multi1 = cham->multi[0];
    Multilayer* multi2 = cham->multi[1];
    
    float Zero = 0.;
    float X0   = 0.;
    float X1   = 0.;
    float Y    = 0.;
    
    if(cham->Type[0]=='B')  // variables for computing chamber->Amid.
    {
        X0 = cham->Zmin;
	X1 = cham->Zmax;
	Y  = cham->Ymid;
    }
    else
    {
        X0 = cham->Rmin;
	X1 = cham->Rmax;
	Y  = cham->Ymid;
    }
    
    if(cham->Type[0]=='B' || s_testbeam)
    {
        //implement rotation around t axis
	rotateDim(cham->Zmin, cham->Zmax, Zero, Zero, ZposCha, Zero, Alfa);
	Zero = 0.;
	if(multi1)
	{
	    rotateDim(multi1->Zmin,multi1->Zmax,Zero,Zero,ZposCha,Zero,Alfa);
	}
	Zero = 0.;
	if(multi2)
	{
	    rotateDim(multi2->Zmin,multi2->Zmax,Zero,Zero,ZposCha,Zero,Alfa);
	}
	    
	for (int i=0;i<MaxTDC;++i)
        {
	    if(cham->TDC[i])
	    {
	        TDCgeo* tdc = cham->TDC[i];
		
		Zero = 0.;
		rotateDim(tdc->Zmin,tdc->Zmax,Zero,Zero,ZposCha,Zero,Alfa);
		//tdc->Zmin = cos(Alfa)*(tdc->Zmin - ZposCha) + ZposCha;
	        //tdc->Zmax = cos(Alfa)*(tdc->Zmax - ZposCha) + ZposCha;
		if(tdc->Zmin > tdc->Zmax) swap(tdc->Zmin,tdc->Zmax);
                
		for (int j=0;j<TDCch;++j)
                {
		    if(tdc->RZ[j][1] != 0.)
		    {
		        if(fl) std::cout << "Alpha (" << Alfa << ") rotation "
			                 << "TDC=" << i << " ch=" << j << ": "
			                 << "prev z=" << tdc->RZ[j][1];
		        float z = (tdc->RZ[j][1] - ZposCha);
                        tdc->RZ[j][1] = z*cos(Alfa) + ZposCha;
		    
		         if(fl) std::cout << ",  post z=" << tdc->RZ[j][1]
			                  << std::endl; 
		    
		        X0 = (X0 - ZposCha)*cos(Alfa) + ZposCha;
		        X1 = (X1 - ZposCha)*cos(Alfa) + ZposCha;
                    }
		}
            }
        }


	//implement rotation around z axis
	Zero = 0.;
	rotateDim(cham->Rmin, cham->Rmax, Zero, Zero, RposCha, Zero, Beta);
	Zero = 0.;
	if(multi1)
	{
	    rotateDim(multi1->Rmin,multi1->Rmax,Zero,Zero,RposCha,Zero,Beta);
	}
	Zero = 0.;
	if(multi2)
	{
	    rotateDim(multi2->Rmin,multi2->Rmax,Zero,Zero,RposCha,Zero,Beta);
	}
	    
	for (int i=0;i<MaxTDC;++i)
        {
	    if(cham->TDC[i])
	    {
	        TDCgeo* tdc = cham->TDC[i];
		
		Zero = 0.;
		rotateDim(tdc->Rmin,tdc->Rmax,Zero,Zero,RposCha,Zero,Beta);
		//tdc->Rmin = cos(Beta)*(tdc->Rmin - RposCha) + RposCha;
	        //tdc->Rmax = cos(Beta)*(tdc->Rmax - RposCha) + RposCha;
		if(tdc->Rmin > tdc->Rmax) swap(tdc->Rmin,tdc->Rmax);
                
		for (int j=0;j<TDCch;++j)
                {
		    if(tdc->RZ[j][0] != 0.)
		    {
		        if(fl) std::cout << "Beta (" << Beta << ") rotation "
			                 << "TDC=" << i << " ch=" << j << ": "
			                 << "prev r=" << tdc->RZ[j][0];
		        float r = (tdc->RZ[j][0] - RposCha);
                        tdc->RZ[j][0] = r*cos(Beta) + RposCha;
			
			if(fl) std::cout << ",  post r=" << tdc->RZ[j][0]
			                 << std::endl; 
                    }
		}
            }
        }
	


	//implement rotation around s axis
	
	rotateDim(cham->Zmin, cham->Zmax, cham->Rmin, cham->Rmax, 
	          ZposCha,RposCha,Gamma);
	
	if(multi1)
	{
	
	    rotateDim(multi1->Zmin, multi1->Zmax, multi1->Rmin, multi1->Rmax, 
	              ZposCha,RposCha,Gamma);	
	}
	if(multi2)
	{
	    rotateDim(multi2->Zmin, multi2->Zmax, multi2->Rmin, multi2->Rmax, 
	              ZposCha,RposCha,Gamma);
	}
	    
	for (int i=0;i<MaxTDC;++i)
        {
	    if(cham->TDC[i])
	    {
	        TDCgeo* tdc = cham->TDC[i];
		
	        rotateDim(tdc->Zmin, tdc->Zmax, tdc->Rmin, tdc->Rmax, 
		          ZposCha,RposCha,Gamma);
	                
		for (int j=0;j<TDCch;++j)
                {
		    if(tdc->RZ[j][0] != 0.)
		    {
		    
		        if(fl) std::cout << "Gamma (" << Gamma << ") rotation "
			                 << "TDC=" << i << " ch=" << j << ": "
			                 << "prev r=" << tdc->RZ[j][0]
					 << ", z="    << tdc->RZ[j][1];
		        float r = (tdc->RZ[j][0] - RposCha);
		        float z = (tdc->RZ[j][1] - ZposCha);
		        tdc->RZ[j][1] =  z*cos(Gamma) + r*sin(Gamma) + ZposCha;
                        tdc->RZ[j][0] = -z*sin(Gamma) + r*cos(Gamma) + RposCha;
		       
		        if(fl) std::cout << ",  post r=" << tdc->RZ[j][0]
					 << ", z="    << tdc->RZ[j][1]
					 << std::endl;
		       
		        float Y0 = Y;
		        float Y1 = Y;
		    
		        X0 = (X0-ZposCha)*cos(Gamma)+(Y0-RposCha)*sin(Gamma) + ZposCha;
		        Y0 = -(X0-ZposCha)*sin(Gamma)+(Y0-RposCha)*cos(Gamma) + RposCha;

		        X1 = (X1-ZposCha)*cos(Gamma)+(Y1-RposCha)*sin(Gamma) + ZposCha;
		        Y1 = -(X1-ZposCha)*sin(Gamma)+(Y1-RposCha)*cos(Gamma) + RposCha;
		    
		        cham->Amid = (Y1-Y0)/(X1-X0);
		    }
                }
            }
        }
		
    }
    
    if(cham->Type[0]=='E')
    {
    }
}


void
MDTGeometry::rotateDim(float& Xmin,float& Xmax,float& Ymin,float& Ymax,
                       float XposCha,float YposCha,float alfa) const
{

    //cout << "Alfa" << alfa << endl;
    //cout << "Xmin=" << Xmin << ", Xmax=" << Xmax 
    //     << ",   Ymin=" << Ymin << ", Ymax=" << Ymax << endl;
    //cout << "Xmin - XposCha = " << Xmin - XposCha
    //     << ",   Xmax - XposCha = " << Xmax - XposCha << endl;
    //cout << "Ymin - YposCha = " << Ymin - YposCha
    //     << ",   Ymax - YposCha = " << Ymax - YposCha << endl;


    pair<float,float> corn1(Xmin-XposCha,Ymin-YposCha);
    pair<float,float> corn2(Xmin-XposCha,Ymax-YposCha);
    pair<float,float> corn3(Xmax-XposCha,Ymin-YposCha);
    pair<float,float> corn4(Xmax-XposCha,Ymax-YposCha);
    
    corn1.first  =  corn1.first*cos(alfa) + corn1.second*sin(alfa);
    corn1.second = -corn1.first*sin(alfa) + corn1.second*cos(alfa);
    
    corn2.first  =  corn2.first*cos(alfa) + corn2.second*sin(alfa);
    corn2.second = -corn2.first*sin(alfa) + corn2.second*cos(alfa);
    
    corn3.first  =  corn3.first*cos(alfa) + corn3.second*sin(alfa);
    corn3.second = -corn3.first*sin(alfa) + corn3.second*cos(alfa);
    
    corn4.first  =  corn4.first*cos(alfa) + corn4.second*sin(alfa);
    corn4.second = -corn4.first*sin(alfa) + corn4.second*cos(alfa);

    float X[4] = {corn1.first+XposCha, corn2.first+XposCha,
                  corn3.first+XposCha, corn4.first+XposCha};
    float Y[4] = {corn1.second+YposCha, corn2.second+YposCha,
                  corn3.second+YposCha, corn4.second+YposCha};
    
    
    //cout << "X[0]=" << X[0] << ", X[1]=" << X[1]
    //     << "X[2]=" << X[2] << ", X[3]=" << X[3] << endl;
    
    
    Xmin = 999999.;
    Xmax = -999999.;
    
    Ymin = 999999.;
    Ymax = -999999.;
    
    for (int i=0;i<4;++i) if(X[i] < Xmin) Xmin = X[i];
    for (int i=0;i<4;++i) if(X[i] > Xmax) Xmax = X[i];
    
    for (int i=0;i<4;++i) if(Y[i] < Ymin) Ymin = Y[i];
    for (int i=0;i<4;++i) if(Y[i] > Ymax) Ymax = Y[i];  

}


void
MDTGeometry::resetRemap(TDCremap map[MaxTDC][TDCch]) const
{
    for(int i=0;i<MaxTDC;++i)
    {
        for(int j=0;j<TDCch;++j)
	{
	    map[i][j].Multilayer = 0;
	    map[i][j].Layer      = 0;
	    map[i][j].Tube       = 0;
	}
    }
}

bool
MDTGeometry::read_cabling_remapping(DBline& data)
{    
    Recabling = new uint8_t[MaxTYPE][8*(2*MaxJZZ+1)][MaxTDC][TDCch][3];
    
    while(++data) {
        uint16_t subsystemId = 0x0;
	uint16_t mrodId = 0x0;
	uint16_t linkId = 0x0;
        if( data("Start Chamber:") >> hex() >> subsystemId >> hex() >> mrodId 
	                           >> hex() >> linkId >> dec() ) {
	    if(RemFlag) std::cout << "Start Chamber:   " << std::hex 
	                          << subsystemId << "   " 
			          << mrodId << "   "
			          << linkId << std::dec << std::endl;
            
	    // compute the tube shift due to mismatch between 
	    // online and offline numbering
//	    int tubeShift[2] = {0,0};
	    const CSMid csm = this->getCsm(subsystemId,mrodId,linkId);

	    if (! csm) {
	        std::cout << "Chamber  " << std::hex 
	                  << subsystemId << "   " << mrodId << "   "
			  << linkId << std::dec 
                          << " doesn't exists in the ROD map" << std::endl;
                do 
		{
	            ++data;
	        } while (! data("End Chamber") );
	    }
	    else
	    {  
	    /* 
	        std::string type = csm.name_str();
	        if (type == "BIR") {
	            switch(csm.stationEta()) {
		        case 1:  tubeShift[0] = -6; tubeShift[1] =  0; break;
		    
		        case 3:  if( csm.stationPhi()==8 ) {
		                     tubeShift[0] = -3; tubeShift[1] = -3;
			         } else {tubeShift[0] = 0; tubeShift[1] = 0;}
			         break;
		    
		        case -3: if( csm.stationPhi()==8 ) {
		                     tubeShift[0] = -3; tubeShift[1] = -3;
			         } else {tubeShift[0] = 0; tubeShift[1] = 0;}
			         break;
		    
		        case 4:  tubeShift[0] = -3; tubeShift[1] = 0;
			         break;

                        case -4: tubeShift[0] = -3; tubeShift[1] = 0;
			         break; 
		           
		        default: tubeShift[0] =  0; tubeShift[1] =  0; break;
		    }
	        }
	    */

	        Amdcsimrec* amdc = s_amdc;   // get the interface towards AMDC
	    
	        int tmp = csm.stationName()+1;
                std::string name = amdc->GetStationType(tmp);
                int StationEta = csm.stationEta();
                int StationPhi = csm.stationPhi();
                int Multilayer = 1;
                int Layer      = 1;
                int Tube       = 1;
                int Jtype = amdc->GetJtyp(name);
                int Jff;
                int Jzz;
                int Job;
                int Jsl;
                int Jtube;
	    
	        do {
	            ++data;
		    int tdcId = 0;
		    int etaId = 999;
		    if ( data("Start TDC:") >> tdcId >> etaId) {
		        int stationEta = (abs(StationEta)>9)? etaId:StationEta;
		        if(RemFlag) std::cout << "Start TDC: " << tdcId 
		                              << "  at StationEta=" 
					      << stationEta << std::endl;

                        amdc->GetAMDCindexFromMDT(name,stationEta,
		   	                          StationPhi,Multilayer,
                                                  Layer,Tube,
		                                  Jff,Jzz,Job,Jsl,Jtube);
                        int JTYPE = Jtype - 1;
                        int JCODE = (Jff-1)*(2*Mzz+1)+Jzz+Mzz;
		        
			do {
		            ++data;
			    int chId = 0;
			    int mulId = 0;
		            int layId = 0;
		            int tubId = 0;
			    if ( data("channel")>>chId>>":">>mulId>>layId>>tubId) {
			        if(RemFlag) std::cout << "channel " << std::setw(2)
			                              << chId  << " :  "
					    	      << mulId << "   "
						      << layId << "   "
						      << tubId << std::endl;
			    
			        Recabling[JTYPE][JCODE][tdcId][chId][0] = mulId; 
			        Recabling[JTYPE][JCODE][tdcId][chId][1] = layId;
			        //Recabling[JTYPE][JCODE][tdcId][chId][2] = tubId + tubeShift[mulId-1];
			        Recabling[JTYPE][JCODE][tdcId][chId][2] = tubId;
			    } 
		        } while (! data("End TDC") );
		    }
	        } while (! data("End Chamber") );
	    }   
	}
    }
    

    // if write permission is granted to the user then write the file
    if( !access(InstallAreaDir.c_str(), W_OK) )
    {
        std::string fileName = InstallAreaDir + "/" + "AtlasMEZmap.data";
        FILE* f = fopen(fileName.c_str(), "wb");
        fwrite(Recabling, sizeof(uint8_t),MaxTYPE*(8*(2*MaxJZZ+1))*MaxTDC*TDCch*3 , f);
        fclose(f);
    }
    
    delete [] Recabling;
    
    return true;
}


bool
MDTGeometry::create_mezmap() {
    
    //std:;cout << "MDTGeometry::create_mezmap():" << std::endl;
    //std::cout << "  MaxType=" << MaxTYPE << std::endl;
    //std::cout << "  MaxCODE=" << 8*(2*MaxJZZ+1) << std::endl;
    //std::cout << "  MaxTDC="  << MaxTDC << std::endl;
    //std::cout << "  MaxCH="   << TDCch  << std::endl;

    Recabling = new uint8_t[MaxTYPE][8*(2*MaxJZZ+1)][MaxTDC][TDCch][3];
    memset(Recabling,0,MaxTYPE*(8*(2*MaxJZZ+1))*24*18*3*sizeof(uint8_t));
    
    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >* listOfSubdet;
    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >::const_iterator it_sub;

    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >* listOfROD;
    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >::const_iterator it_rod;

    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >* listOfCsm;
    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >::const_iterator it_csm;

    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;

    listOfSubdet = s_cabling_map->getListOfElements();

    if (RemFlag) {
        std::cout << "Got the list of subdetectors" << std::endl;
        std::cout << "Number of subdetectors: " 
	          << listOfSubdet->size() << std::endl;
    }


    // loop on the subdetectors
    for (it_sub=listOfSubdet->begin(); 
         it_sub !=listOfSubdet->end(); ++it_sub) {

        int subdetectorId = ((*it_sub).second)->moduleId();
	if (RemFlag) {
            std::cout << "Now in subdetector: 0x" << std::hex
                      << (int) subdetectorId << std::dec << std::endl;
        }
        // loop on the RODs of this subdtector
        listOfROD = ((*it_sub).second)->getListOfElements();
        for (it_rod=listOfROD->begin(); 
	     it_rod !=listOfROD->end(); ++it_rod) {

            int rodId = ((*it_rod).second)->moduleId();
	    if (RemFlag) {
                std::cout << "Now in ROD: 0x" << std::hex
                          << (int)  rodId << std::dec << std::endl;
            }
            // loop on the CSMs of this ROD
            listOfCsm = ((*it_rod).second)->getListOfElements();
            for (it_csm=listOfCsm->begin(); 
	         it_csm !=listOfCsm->end(); ++it_csm) {

                int csmId = ((*it_csm).second)->moduleId();
		
		if (RemFlag) {
                    std::string csmText = giveCSMtext(subdetectorId,rodId, 
		                                         (*it_csm).second);
                    std::cout << "Now in csm: 0x" << std::hex
                              << (int) csmId << std::dec
			      << " : " << csmText << std::endl;
		}
		
		listOfAmt = ((*it_csm).second)->getListOfElements();
		
		for (it_amt=listOfAmt->begin();
		     it_amt !=listOfAmt->end() ; ++it_amt) {
		    
		    int amtId = ((*it_amt).second)->moduleId();
		    
		    for (int chanId = 0 ; chanId<24 ; ++chanId) {
		    
                        int station=0;
                        int eta=0;
                        int phi=0;
                        int multi=0;
                        int layer=0;
                        int tube=0;

                        bool cabling = s_cabling_map->getOfflineId(
			               subdetectorId,rodId,csmId,amtId,chanId,
                                       station,eta,phi,multi,layer,tube);

                        if (cabling) {
			    int Jtype = station + 1;
		            int Jff;
		            int Jzz;
		            int Job;
		            int Jsl;
		            int Jtube;
                            std::string name = s_amdc->GetStationType(Jtype);
                            s_amdc->GetAMDCindexFromMDT(name,
			                              eta,phi,multi,layer,tube,
		                                      Jff,Jzz,Job,Jsl,Jtube);

                            int Jcode = (Jff-1)*(2*Mzz+1)+Jzz+Mzz;
                            /*
			    if(RemFlag) {
			        std::cout << " Tdc=" << amtId
				          << ", Ch=" << chanId
					  << " mapped to: multilayer=" << multi
					  << ", layer = " << layer
					  << ", tube=" << tube
					  << "....  Jtype=" << Jtype
					  << ", Jcode=" << Jcode << std::endl;
			    }

                            if (Jtype-1>=MaxTYPE) {
			        std::cout << "ERROR: Jtype-1=" << Jtype-1
				          << " while MaxType=" << MaxTYPE
					  << std::endl;
			    }
			    
			    if (Jcode>=8*(2*MaxJZZ+1)) {
			        std::cout << "ERROR: Jcode=" << Jcode
				          << " while Max=" << 8*(2*MaxJZZ+1)
					  << std::endl;
			    }
			    
			    if (amtId>=MaxTDC) {
			        std::cout << "ERROR: amtId=" << amtId
				          << " while MaxTDC=" << MaxTDC
					  << std::endl;
			    }
			    
			    if (chanId>=TDCch) {
			        std::cout << "ERROR: chanId=" << chanId
				          << " while MaxCha=" << TDCch
					  << std::endl;
			    }
*/
                            Recabling[Jtype-1][Jcode][amtId][chanId][0] = multi; 
			    Recabling[Jtype-1][Jcode][amtId][chanId][1] = layer;
			    Recabling[Jtype-1][Jcode][amtId][chanId][2] = tube;
			
			}
		    
                    }
		}
            }
	}
    }

    return true;
}

/*
bool
MDTGeometry::create_mezmap_file() {
    
    Recabling = new uint8_t[MaxTYPE][8*(2*MaxJZZ+1)][MaxTDC][TDCch][3];

    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >* listOfSubdet;
    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >::const_iterator it_sub;

    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >* listOfROD;
    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >::const_iterator it_rod;

    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >* listOfCsm;
    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >::const_iterator it_csm;

    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;

    listOfSubdet = s_cabling_map->getListOfElements();


    std::cout << "Got the list of subdetectors" << std::endl;
    std::cout << "Number of subdetectors: " << listOfSubdet->size() << std::endl;


    int ntotal = 0;
    // loop on the subdetectors
    for (it_sub=listOfSubdet->begin(); 
         it_sub !=listOfSubdet->end(); ++it_sub) {

        int subdetectorId = ((*it_sub).second)->moduleId();
        std::cout << "Now in subdetector: 0x" << std::hex
                  << (int) subdetectorId << std::dec << std::endl;

        // loop on the RODs of this subdtector
        listOfROD = ((*it_sub).second)->getListOfElements();
        for (it_rod=listOfROD->begin(); 
	     it_rod !=listOfROD->end(); ++it_rod) {

            int rodId = ((*it_rod).second)->moduleId();
            std::cout << "Now in ROD: 0x" << std::hex
                      << (int)  rodId << std::dec << std::endl;

            // loop on the CSMs of this ROD
            listOfCsm = ((*it_rod).second)->getListOfElements();
            for (it_csm=listOfCsm->begin(); 
	         it_csm !=listOfCsm->end(); ++it_csm) {

                int csmId = ((*it_csm).second)->moduleId();
                std::cout << "Now in csm: 0x" << std::hex
                          << (int) csmId << std::dec << std::endl;

                int StationEta[2] = {-999,-999};
		int sector = 0;
		std::string name = "";
		
		listOfAmt = ((*it_csm).second)->getListOfElements();

                for (it_amt=listOfAmt->begin(); 
		     it_amt !=listOfAmt->end() ; ++it_amt) {

                    int amtId = ((*it_amt).second)->moduleId();
                    std::cout << "Now in amt: 0x" << std::hex
                              << (int) amtId  << std::dec;

                    int station=0;
                    int phi=0;
                    int multi=0;
                    int layer=0;
                    int tube=0;
		    
		    int* eta = &StationEta[0];
		    if (*eta!=-999) eta = &StationEta[1];

                    int chanId = 0;

                    // convert the first cabled channel of the TDC
		    while(! s_cabling_map->getOfflineId(subdetectorId,
		                                        rodId,csmId,
                                                        amtId,chanId,
                                                        station,*eta,phi,multi,
                                                        layer,tube) ) ++chanId;
                    
		    
		    if (StationEta[1] == StationEta[0]) StationEta[1] = -999;
		    
		    int Jtype = station+1;
		    name = s_amdc->GetStationType(Jtype);
		    sector = (name[2]=='L' || name[2]=='R' || name[2]=='M')?
		                 phi*2-1 : phi*2;
		    
                    std::cout << "name=" << name
		              << ", StationEta=" <<  *eta
			      << ", StationPhi=" << phi 
			      << ", Multilayer=" << multi
			      << ", Layer=" << layer
			      << ", Tube=" << tube << std::endl;	
		}
		
                // get the Sector info and the StationEta info
#if (__GNUC__) && (__GNUC__ > 2) 
                // put your gcc 3.2 specific code here
                __sstream dump;
#else
                // put your gcc 2.95 specific code here
                char data[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
                __sstream dump(data,8);
#endif
                std::string side = (StationEta[0] > 0)? "A" : "C";
		dump << name;
		if (StationEta[1]!=-999) dump 
		              << fabs(StationEta[0]) << fabs(StationEta[1]); 
                else dump << fabs(StationEta[0]);
		
		dump << side << std::setfill('0') << std::setw(2) << sector
		         << ends;
		     
		std::cout << " " << dump.str() << std::endl;	
			     
            }
	}
    }
    return true;
}
*/

std::string
MDTGeometry::giveCSMtext(int subdetectorId, int rodId, MdtCsmMap* csm) {

    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
    std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;
    
    std::string name  = "";
    int StationEta[2] = {-999,-999};
    int Sector        = 0;
		
    listOfAmt = csm->getListOfElements();
    int csmId = csm->moduleId();

    for (it_amt=listOfAmt->begin();it_amt !=listOfAmt->end() ; ++it_amt) {

        int amtId = ((*it_amt).second)->moduleId();
        
	int station=0;
        int phi=0;
        int multi=0;
        int layer=0;
        int tube=0;
		    
        int* eta = &StationEta[0];
	if (*eta!=-999) eta = &StationEta[1];

        int chanId = 0;

        // convert the first cabled channel of the TDC
	while(!s_cabling_map->getOfflineId(
	                          subdetectorId,rodId,csmId,amtId,chanId,
                                  station,*eta,phi,multi,layer,tube) ) ++chanId;
                    
		    
        if (StationEta[1] == StationEta[0]) StationEta[1] = -999;
		    
        int Jtype = station+1;
        name      = s_amdc->GetStationType(Jtype);
        Sector    = (name[2]=='L'||name[2]=='R'||name[2]=='M')? phi*2-1 : phi*2;
    }
		
    // get the Sector info and the StationEta info
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __sstream dump;
#else
    // put your gcc 2.95 specific code here
    char data[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
    __sstream dump(data,8);
#endif
    std::string side = (StationEta[0] > 0)? "A" : "C";
    
    if(name=="EIL" && fabs(StationEta[1])==5) {
        name = "EILM";
	StationEta[1] = -999;
    }
    
    if(name=="EEL" && Sector==5) {
        int signStEta = StationEta[0]/fabs(StationEta[0]);
        if (fabs(StationEta[0])==1) StationEta[0]=2*signStEta;
    }
    
    dump << name;
    if (StationEta[1]!=-999) dump << fabs(StationEta[0]) << fabs(StationEta[1]); 
    else                     dump << fabs(StationEta[0]);
		
    dump << side << std::setfill('0') << std::setw(2) << Sector << std::ends;
		     
    return dump.str().c_str();
}


bool 
MDTGeometry::create_mrod_map()
{
    if(s_cabling_map==0) return false;
    
    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >* listOfSubdet;
    std::map<uint8_t, MdtSubdetectorMap*, std::less<uint8_t> >::const_iterator it_sub;

    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >* listOfROD;
    std::map<uint8_t, MdtRODMap*, std::less<uint8_t> >::const_iterator it_rod;

    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >* listOfCsm;
    std::map<uint8_t, MdtCsmMap*, std::less<uint8_t> >::const_iterator it_csm;

    //std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >* listOfAmt;
    //std::map<uint8_t, MdtAmtMap*, std::less<uint8_t> >::const_iterator it_amt;

    listOfSubdet = s_cabling_map->getListOfElements();
    
    bool TestbeamFlag = false;
    
    for (it_sub=listOfSubdet->begin(); 
         it_sub !=listOfSubdet->end(); ++it_sub) {

        int subdetectorId = ((*it_sub).second)->moduleId();
	
	unsigned short int side_id      = 0;
	unsigned short int eta_offset   = 0;
	unsigned short int crate_offset = 0;
	
	if (subdetectorId==0x61 || subdetectorId==0x63) side_id = 1;
	if (subdetectorId==0x62 || subdetectorId==0x64) side_id = 0;
	
	if (subdetectorId==0x63 || subdetectorId==0x64) eta_offset = 3;
	
	if (subdetectorId==0x62) crate_offset = 0x4;
	if (subdetectorId==0x63) crate_offset = 0x8;
	if (subdetectorId==0x64) crate_offset = 0x12;
	
	ReadOutTowers* towers = 0;
	
        // loop on the RODs of this subdtector
        listOfROD = ((*it_sub).second)->getListOfElements();
	
        for (it_rod=listOfROD->begin(); 
	     it_rod !=listOfROD->end(); ++it_rod) {

            int rodId = ((*it_rod).second)->moduleId();
	    
	    unsigned short int eta_tower = 0;
	    
	    if (rodId==0x0 || rodId==0x10 || rodId==0x20) {
	        if (rodId==0x10 || rodId==0x20) 
		                            m_readout_towers.push_back(*towers);
	        
		eta_tower = eta_offset + rodId/16;
	        towers = 
		new ReadOutTowers(subdetectorId,eta_tower,side_id,TestbeamFlag);
	    }
	    
	    unsigned short int crate_id = 0;
	    
	    if (rodId>=0x30) {
                if (subdetectorId<=0x62) crate_id = 7 + (rodId-0x30);
		else crate_id = 9 + (rodId-0x30);
	    } else {
	        crate_id = crate_offset + (rodId%8)/2 + 1;
	    }
	    
            // loop on the CSMs of this ROD
	    std::string link0 = "-";
	    std::string link1 = "-";
	    std::string link2 = "-";
	    std::string link3 = "-";
            std::string link4 = "-";
	    std::string link5 = "-";
	    std::string link6 = "-";
	    std::string link7 = "-";
	    //std::string linkCSM[8] = {"-", "-", "-", "-", "-", "-", "-", "-"};
	    listOfCsm = ((*it_rod).second)->getListOfElements();
            for (it_csm=listOfCsm->begin(); 
	         it_csm !=listOfCsm->end(); ++it_csm) {

                int csmId = ((*it_csm).second)->moduleId();
		
		std::string csmText = giveCSMtext(subdetectorId,rodId, 
		                                         (*it_csm).second);
		
		if (csmId==0) link0 = csmText;
		if (csmId==1) link1 = csmText;
		if (csmId==2) link2 = csmText;
		if (csmId==3) link3 = csmText;
		if (csmId==4) link4 = csmText;
		if (csmId==5) link5 = csmText;
		if (csmId==6) link6 = csmText;
		if (csmId==7) link7 = csmText;
		
		//linkCSM[csmId] = csmText;
		//std::cout << "linkCSM[csmId]=" << linkCSM[csmId]  << std::endl;
		//std::cout << "csmText=" << csmText  << std::endl;
            }
	    //std::cout << "linkCSM[0]=" << linkCSM[0]  << std::endl;
	    //MRODscheme mrod( subdetectorId,rodId,crate_id,
	     //               linkCSM[0],linkCSM[1],linkCSM[2],linkCSM[3],
	//		    linkCSM[4],linkCSM[5],linkCSM[6],linkCSM[7] );

            /*
            std::cout << "link0=" << link0 << std::endl;
	    std::cout << "link1=" << link1 << std::endl;
	    std::cout << "link2=" << link2 << std::endl;
	    std::cout << "link3=" << link3 << std::endl;
	    std::cout << "link4=" << link4 << std::endl;
	    std::cout << "link5=" << link5 << std::endl;
	    std::cout << "link6=" << link6 << std::endl;
	    std::cout << "link7=" << link7 << std::endl;
	    */
            MRODscheme mrod( subdetectorId,rodId,crate_id,
	                     link0,link1,link2,link3,link4,link5,link6,link7 );

            //std::cout << mrod << std::endl;

            for (int i=0;i<8;++i) {
                
		if(mrod.links()[i]) {
		
		    int tmp = mrod.links()[i].stationName()+1;
		    std::string name = s_amdc->GetStationType(tmp);
                    int StationEta = mrod.links()[i].stationEta();
                    int StationPhi = mrod.links()[i].stationPhi();
		    int Multilayer = 1;
		    int Layer      = 1;
		    int Tube       = 1;
		    int Jtype = s_amdc->GetJtyp(name);
		    int Jff;
		    int Jzz;
		    int Job;
		    int Jsl;
		    int Jtube;

                    int stEta[2] = {StationEta%10,
			               (abs(StationEta)>9)?StationEta/10:999};
			    
                    for(int k=0;k<2;++k) {
	  
                        if(stEta[k]!=999) {
			
                            s_amdc->GetAMDCindexFromMDT(name,stEta[k],
			                                StationPhi,Multilayer,
                                                        Layer,Tube,
		                                        Jff,Jzz,Job,Jsl,Jtube);

                            int Jcode = (Jff-1)*(2*Mzz+1)+Jzz+Mzz;
                            mrod.csm()[i].set_chamber(
			              m_chambers[Jtype-1][Jcode],stEta[k]);
                            mrod.csm()[i].set_tower(eta_tower);

                        }
                    }
                }
            }

            *towers << mrod;
        }
	m_readout_towers.push_back(*towers);
    }
    
    return true;
}
 /*   
    unsigned short int start_mrod;
    unsigned short int stop_mrod;
    unsigned short int eta_tower;
    unsigned short int system_id;
    unsigned char side;
    
    while(++data)
      {
        if(data("Towers") >> start_mrod >> "-" >> stop_mrod 
                          >> "eta" >> eta_tower
                          >> "side" >> side 
                          >> "ID" >> hex() >> system_id >> dec() )
        {
            unsigned short int side_id = 0;
	    bool TestbeamFlag = false;
	    
	    if(side == 'A')       side_id = 1;
	    else if(side == 'C')  side_id = 0; 
	    else if(side == 'T')  {side_id = 2; TestbeamFlag=true;}

            ReadOutTowers towers(system_id,eta_tower,side_id,TestbeamFlag);

            (++data)("{");
            do
            {
	        unsigned short int mrod_id;
	        unsigned short int crate_id;
	        std::string link1;
	        std::string link2;
	        std::string link3;
	        std::string link4;
	        std::string link5;
	        std::string link6;
	        std::string link7;
	        std::string link8;
                Amdcsimrec* amdc = s_amdc;   // get the interface towards AMDC

	        if(data(" ") >> hex() >> mrod_id >> dec() >> "||" 
                             >> link1 >> link2 >> link3 >> link4 
                             >> link5 >> link6 >> link7 >> link8
	                     >> "|" >> crate_id)
	        {		    
			      
	            MRODscheme mrod(system_id,mrod_id,crate_id,link1,link2,
                                    link3,link4,link5,link6,link7,link8);
		    for (int i=0;i<8;++i)
		    {
			if(mrod.links()[i])
			{
			    int tmp = mrod.links()[i].stationName()+1;
		            std::string name = amdc->GetStationType(tmp);
                            int StationEta = mrod.links()[i].stationEta();
                            int StationPhi = mrod.links()[i].stationPhi();
		            int Multilayer = 1;
		            int Layer      = 1;
		            int Tube       = 1;
		            int Jtype = amdc->GetJtyp(name);
		            int Jff;
		            int Jzz;
		            int Job;
		            int Jsl;
		            int Jtube;

                            int stEta[2] = {StationEta%10,
			                   (abs(StationEta)>9)?StationEta/10:999};
			    
                            for(int k=0;k<2;++k)
			    {	  
			        if(stEta[k]!=999)
				{
                                    amdc->GetAMDCindexFromMDT(name,stEta[k],
			                              StationPhi,Multilayer,
                                                      Layer,Tube,
		                                      Jff,Jzz,Job,Jsl,Jtube);

		                    int Jcode = (Jff-1)*(2*Mzz+1)+Jzz+Mzz;
		                    mrod.csm()[i].set_chamber(
			                          m_chambers[Jtype-1][Jcode],
					          stEta[k]);
				    mrod.csm()[i].set_tower(eta_tower);

                                }
                            }
			}
		    }

		    towers << mrod;
	        }

            data++;
            }while(!data("}"));

        m_readout_towers.push_back(towers);

        }
    }

    return true;
}
*/

void MDTGeometry::checkCorrections(std::string name,
          int StationEta, int StationPhi, int Multilayer, int Layer, int Tube,
          std::string typ, double ComputedValue,double CorrectedValue) const
{
    if (fabsf(ComputedValue-CorrectedValue) > nor(25) ) {
        std::cout << "Large " << typ << " correction for " << name 
                  << ", StationEta=" << StationEta
                  << ", StationPhi=" << StationPhi
                  << ", Multilayer=" << Multilayer
                  << ", Layer=" << Layer
                  << ", Tube=" << Tube 
                  << ": Delta" << typ << "=" << ComputedValue-CorrectedValue 
		  << ", computed" << typ << "=" << ComputedValue
		  << ", corrected" << typ << "=" << CorrectedValue << std::endl;
    }
}
