// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MUGEOMETRY_H
#define PHYSICS_MU_MUGEOMETRY_H

#include "AmdcCore/Amdcsimrec.h"

#include "MDTcabling/t2mugeo.h"
#include "MDTcabling/ReadOutTowers.h"

#include "MuonCablingTools/dbline.h"

#include <list>
#include <set>
#include <string>
#include <utility>
#include <fstream>

#ifndef LVL1_STANDALONE
#include "MuonIdHelpers/MdtIdHelper.h"
#endif

class MDTcablingSvc;
class MuonMDT_CablingMap;
class MdtCsmMap;
class TH1F;

class MDTGeometry {

public:
    typedef std::list<ReadOutTowers> ReadoutTowers;
    typedef std::set< uint32_t > ROBid;
    typedef std::set< uint32_t > HASHid;

public:
    ~MDTGeometry(void);

public:

    static const MDTGeometry* instance(void);

    Chambers chambers(void) const;
    const ChamType& get_cham(void) const;

    bool getOnlineIDfromOfflineID(int name, int StationEta,
                                  int StationPhi,int MultiLayer, int TubeLayer,
				  int Tube, 
                                  unsigned short int& SubsystemId,
				  unsigned short int& MrodId,
				  unsigned short int& LinkId,
				  unsigned short int& TdcId,
				  unsigned short int& ChannelId) const;

    bool getOfflineIDfromOnlineID(unsigned short int SubsystemId,
				  unsigned short int MrodId,
				  unsigned short int LinkId,
				  unsigned short int TdcId,
				  unsigned short int ChannelId,
                                  int& name, int& StationEta,
                                  int& StationPhi,int& MultiLayer, 
                                  int& TubeLayer,int& Tube) const;
				  
    bool getStationRad(unsigned int StationName, unsigned int StationEta,
                       unsigned int StationPhi, float& Rmin, float& Rmax) const;
		       
    void getBstatR(int sector, int station,float& Rmin, float& Rmax) const;
    void getBstatT(int sector, int station,int& st1, int& st2) const;
    void getEstatZ(int sector, int station,int side,float& Zmin, float& Zmax) const;
    void getEstatT(int sector, int station,int side,int& st1, int& st2) const;
    void getBsects(int station,float Phi,int& st, int& so) const;
    void getEsects(int station,int side,float Phi,int& st,int& so) const;
    
    float getPhiDist(float PhiMin,float PhiMax,float Phi) const;
    
    bool getRZ(unsigned short int SubsystemId,
               unsigned short int MrodId,
               unsigned short int LinkId,
               unsigned short int TdcId,
               unsigned short int ChannelId,
               float& R, float& ortoRad, float& Z) const;

    void getAmdbIDfromOfflineID(std::string name,int StationEta,int StationPhi,
                                int MultiLayer,int TubeLayer,int Tube,
                                int& Jtype,int& Jff,int& Jzz,int& Job,int& Jsl,
                                int& Jtube) const;

    const CSMid getCsm(unsigned short int SubsystemId,
		       unsigned short int MrodId,
		       unsigned short int LinkId) const;
		       
    const ROBid  getROBid(float phiMin, float phiMax,
                          float etaMin,float etaMax) const;
    
    std::vector<uint32_t> getRobId (float etaMin, float etaMax,
                                    float phiMin,float phiMax) const;
    
    const HASHid getHASHid(float phiMin, float phiMax,
                           float etaMin,float etaMax) const;
    
    std::vector<uint32_t> getHashId(float etaMin, float etaMax,
                                    float phiMin,float phiMax) const;

    bool checkHashId(unsigned int) const;
    bool checkHashIdForROB(unsigned int, uint32_t) const;
    bool checkHashIdForLink(unsigned int, uint32_t , uint32_t) const;
    int  hash2Station(unsigned int) const;
    
private:
    MDTGeometry(void);

private:

    bool mdtmap(void) const;
    bool buildMDTgeo(const int , const int , const int, const int ) const;
    bool readgeo(std::ifstream& geometry) const;
    bool t2mucgeo(void) const;
    bool isRecabled(int,int) const;
    void setTDCdim(TDCgeo*,const float) const;
    TDCgeo* createTDC(const unsigned short int) const;
    
    bool setup_chamber_hash_map() const;
    
    bool read_mrod_map(DBline& data);
    bool create_mrod_map(void);
    bool setup_mrods(void) const;
    bool check_mrod_map(void) const;
    
    bool read_cabling_remapping(DBline& data);
    bool create_mezmap(void);
    
    std::string giveCSMtext(int subdetectorId, int rodId, MdtCsmMap* csm);

    void resetRemap(TDCremap map[MaxTDC][TDCch]) const;    
    
    bool find_csm(const CSMid& csm,CSMid& found, 
                  unsigned short int& SubsystemId,
                  unsigned short int& RodId, 
                  unsigned short int& LinkId) const;
		  
    void rotateChamber(Chamber*, float, float, float, 
                                 float, float, float,bool) const;
    void rotateDim(float& ,float& ,float& ,float& ,float,float,float) const;

    int tower_address(unsigned short int Subsystem_ID) const;
    bool mrod_address(unsigned short int Subsystem_ID,
	              unsigned short int MROD_ID,int& address)const;

    void initcham(void);
    int checkR(char type,Multilayer *multi,int TDCnum, bool sflag) const;
    int checkZ(char type,Multilayer *multi,int TDCnum, bool sflag) const;
    int checkL(char type,Multilayer *multi,int TDCnum/*, bool sflag*/) const;
    int checkD(Multilayer *multi,int TDCnum) const;
    int checkTDC(Multilayer *multi1,Multilayer *multi2,int TDCnum) const;
    
    void checkChamTDC(Multilayer *multi,int TDCnum) const;
    //void checkBIR(Chambers) const;
    void checkCorrections(std::string,int,int,int,int,int,std::string,
                                                        double,double) const;
    
    void myprin(char ,int ) const;
    
    bool setup_Stations(void);
    
    double  nor(double) const;
/*    
    bool remap(unsigned short int SubsystemId,
               unsigned short int MrodId,
               unsigned short int LinkId,
	       TDCremap map[MaxTDC][TDCch]) const;
	       
    int findTdc(TDCgeo* oldTdc[MaxTDC],int tubeLayers,
                int tdc,TDCremap map[MaxTDC][TDCch]) const;
*/		
    //bool TDCisEmpty(TDCremap[TDCch]) const;
    //int  getMapMultilayer(TDCremap[TDCch]) const;

private:
    static MDTGeometry* s_instance;

private:
    ChamType m_cham;
    Chambers m_chambers;

    ReadoutTowers m_readout_towers;    

    const MRODscheme** m_mrods;
    

private:
    static bool MapFlag;
    static bool GeoFlag;
    static bool CheFlag;
    static bool RemFlag;

    static bool RgsOut;
    static bool GeoOut;
    static bool ReadFlag;
    
    static float s_TubePitch;
    static double s_ScaleFactor;
    static double s_pi;

    static std::string RgsFile;
    static std::string GeoFile;
    static std::string RODFile;
    static std::string CABFile;
    static std::string MEZFile;
    
    static std::string InstallAreaDir;

    static bool s_status;
    static bool s_testbeam;
    static bool s_shift_LinkId;
    static bool s_useOldGeoAccess;
    static Amdcsimrec* s_amdc;
    
    static MuonMDT_CablingMap* s_cabling_map;

private:
    const int Mtype;
    const int Mff;
    const int Mzz;

private:
    int   m_bStationsTypes[16][3][2];
    float m_bStationsRadius[16][3][2];
    float m_bStationsPhi[16][3][2];
    
    int   m_eStationsTypes[16][3][2][2];
    float m_eStationsZeta[16][3][2][2];
    float m_eStationsPhi[16][3][2][2];

private:
    static const int MaxMROD;
    static const int MaxTower; 

#ifndef LVL1_STANDALONE
private:    
    static const MdtIdHelper*    s_pMdtIdHelper;
    static TH1F* s_hist_barrel_alignCorrOnRadius;
    static TH1F* s_hist_barrel_alignCorrOnZeta;
    static TH1F* s_hist_endcap_alignCorrOnRadius;
    static TH1F* s_hist_endcap_alignCorrOnZeta;

#endif

    friend class MDTcablingSvc;
};


inline Chambers
MDTGeometry::chambers(void) const
{
    return m_chambers;
}


inline const ChamType&
MDTGeometry::get_cham(void) const
{
    return m_cham;
}


inline int
MDTGeometry::tower_address(unsigned short int Subsystem_ID) const
{
    switch(Subsystem_ID)
    {
        case 0x61: return 0;
        case 0x62: return 1;
        case 0x63: return 2;
        case 0x64: return 3;

        default: return -1;
    }
} 

inline void
MDTGeometry::getBstatR(int sector, int station,float& Rmin, float& Rmax) const
{
    Rmin = m_bStationsRadius[sector][station][0];
    Rmax = m_bStationsRadius[sector][station][1];
}

inline void
MDTGeometry::getBstatT(int sector, int station, int& st1, int& st2) const
{
    st1 = m_bStationsTypes[sector][station][0];
    st2 = m_bStationsTypes[sector][station][1];
}

inline void
MDTGeometry::getEstatZ(int sector, int station,int side,float& Zmin, float& Zmax) const
{
    Zmin = m_eStationsZeta[sector][station][side][0];
    Zmax = m_eStationsZeta[sector][station][side][1];
}

inline void
MDTGeometry::getEstatT(int sector, int station,int side, int& st1, int& st2) const
{
    st1 = m_eStationsTypes[sector][station][side][0];
    st2 = m_eStationsTypes[sector][station][side][1];
}

inline float
MDTGeometry::getPhiDist(float PhiMin,float PhiMax,float Phi) const {
    float distMax = fabsf(PhiMax - Phi);
    float distMin = fabsf(Phi - PhiMin);
    return (distMax<distMin)? distMax : distMin;
}

inline void 
MDTGeometry::getBsects(int station,float Phi,int& st, int& so) const{

    st = 99;
    so = 99;
    float dphi = 0.;
    
    if(Phi > s_pi) Phi -= 2.*s_pi;
    
    for(int i=0;i<16;++i)
    {
        float PhiMin = m_bStationsPhi[i][station][0];
        float PhiMax = m_bStationsPhi[i][station][1];
	if (PhiMin*PhiMax > 0) {
	    
//	    if(PhiMin > 0) {
	    
	        if( Phi>=PhiMin && Phi<=PhiMax ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
		}
/*
	    } else {
	    

	        if( Phi<=PhiMin && Phi>=PhiMax ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
		}

           }
*/
	    
	} else {
	    
	    if( PhiMax<s_pi/2. ) {
	    
	        if( (Phi>=PhiMin && Phi<=0.) || (Phi>=0. && Phi<=PhiMax) ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
	        }
		
            } else {
	        
		if( (Phi<=PhiMin && Phi>=-2*s_pi) || (Phi<=2*s_pi && Phi>=PhiMax) ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
	        }
	    }
	}
    }
}

inline void 
MDTGeometry::getEsects(int station,int side,float Phi,int& st, int& so) const {

    st = 99;
    so = 99;
    float dphi = 0.;
    
    if(Phi > s_pi) Phi -= 2.*s_pi;
    
    for(int i=0;i<16;++i)
    {
        float PhiMin = m_eStationsPhi[i][station][side][0];
        float PhiMax = m_eStationsPhi[i][station][side][1];
	if (PhiMin*PhiMax > 0) {
	
	        if( Phi>=PhiMin && Phi<=PhiMax ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
		}
	    
	        
	} else {
	
	    if( PhiMax<s_pi/2. ) {
	    
	        if( (Phi>=PhiMin && Phi<=0.) || (Phi>=0. && Phi<=PhiMax) ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
	        }
		
            } else {
	        
		if( (Phi<=PhiMin && Phi>=-s_pi) || (Phi<=s_pi && Phi>=PhiMax) ) {
	            if(st==99) {
		        st=i; 
		        dphi = getPhiDist(PhiMin,PhiMax,Phi);
		    } else {
		        float tmpdphi = getPhiDist(PhiMin,PhiMax,Phi);
		        if ( tmpdphi>dphi ) {
		            so = st;
			    st = i;
			    dphi = tmpdphi;
		        } else so = i;
		    }
	        }
	    }
	
	}
    }
}

inline double
MDTGeometry::nor(double val) const
{
    return val/s_ScaleFactor;
}

inline void
MDTGeometry::checkChamTDC(Multilayer *multi,int TDCnum) const
{
    int p = 0;
    std::cout << "========== START DUMPING THE TDC ==========" 
	      << std::endl;
    while(p<=23) {
        std::cout << "Z=" << multi->TDC[TDCnum]->RZ[p][1] 
	          << ",  R=" << multi->TDC[TDCnum]->RZ[p][0]
	          << std::endl;
	p++;
    }
	    
    std::cout << "=========== STOP DUMPING THE TDC =========="
              << std::endl;
}
/*
inline bool
MDTGeometry::TDCisEmpty(TDCremap map[TDCch]) const {
    bool isEmpty = true;
    for (int i=0;i<TDCch;++i) if(map[i].Multilayer) return !isEmpty;
    return isEmpty;
}

inline int  
MDTGeometry::getMapMultilayer(TDCremap map[TDCch]) const {

    int multilayer = 0;
    
    for (int i=0;i<TDCch;++i) {
        if(map[i].Multilayer){
	    if (multilayer && multilayer != map[i].Multilayer) return -1;
	    else multilayer = map[i].Multilayer;
	}
    }
    
    return multilayer; 
}

*/

/*
inline void
MDTGeometry::checkBIR(Chambers chambers) const
{
    Chamber *chamber = chambers[7][165];
    checkChamTDC(chamber->multi[0],3);
    checkChamTDC(chamber->multi[1],3);
}
*/

#endif // PHYSICS_MU_MUGEOMETRY_H
