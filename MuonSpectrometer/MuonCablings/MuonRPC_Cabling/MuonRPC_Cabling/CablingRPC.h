/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CablingRPC_H
#define CablingRPC_H

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "Identifier/Identifier.h"
#endif

#include <map>
#include <vector>

#include "MuonCablingTools/RPCdef.h"
#include "MuonCablingTools/dbline.h"

#include "RPCcablingInterface/RDOindex.h"
#include "RPCcablingInterface/RPCofflineId.h"
#include "RPCcablingInterface/CablingRPCBase.h"

#include "MuonRPC_Cabling/SectorLogicSetup.h"


class MuonRPC_CablingSvc;


namespace MuonRPC_Cabling {


class CablingRPC;
std::ostream& operator<<(std::ostream&,const CablingRPC&);


class CablingRPC : public CablingRPCBase
{

    private:
    std::string m_Version;
    int m_MaxType;
    //array; for each sectorlogic number (0-63) returns the sector logic type
    int m_SectorMap[64];

    bool m_TestbeamFlag;

    typedef std::vector < SectorLogicSetup > STvec;
    typedef std::map < int,SectorLogicSetup*,std::less < int > > SLmap;


    //array; for each sectorlogic type returns the SectorLogicSetup
    STvec  m_SectorType;
    //map; for each sectorlogic number returns the SectorLogicSetup
    SLmap  m_SectorLogic;
    RDOmap m_RDOs;


    private:
    CablingRPC(void);
    void initMapsFromCOOL();
    void initMapsFromASCII();
    void clearCache();

    bool buildRDOmap();

    bool CableParamCheck(unsigned short int SubId, unsigned short int SecId ,
	                 unsigned short int PADId, unsigned short int CMAId,
	                 unsigned short int ijk, unsigned int type,
	                 unsigned short int Channel1, 
			 unsigned short int Channel2,
	                 short int Number) const;

    bool BoardParamCheck(unsigned short int SubId,    unsigned short int SecId,
	                 unsigned short int PADId,    unsigned short int CMAId,
	                 unsigned short int inputType,unsigned int       layer,
	                 unsigned int       type,  unsigned short int Channel1,
	                 unsigned short int Channel2,
			 short int Number) const;

    private:
    static CablingRPCBase* s_instance;
    static std::string ConfName;
    static std::string CorrName;
    static bool s_status;
    static bool s_cosmic_configuration;
    static std::string DataName;

    const static std::string *CorrMapPString, *ConfMapPString;
    const static std::map<std::string, std::string>* p_trigroads;
    
    static bool s_RPCMapfromCool;
    unsigned int m_absZindexInThelayerOfThisChamber[12][2][9][8][2][4];

    static void delete_instance(void);

    bool ReadConf(std::string);
    bool ReadCorr(std::string);
    bool ReadConf(const std::string*);
    bool ReadCorr(const std::string*);

        
    public:
    ~CablingRPC(void);


    void PrintMap(std::ostream&) const;
    void PrintType(std::ostream&,int,int,std::string,int,bool) const;
    void PrintSector(std::ostream&,int,int,std::string,int,bool) const;

    void SetPtoTrigRoads(const std::map<std::string, std::string>*);
    void ClearPtoTrigRoads();
    void ClearPtoCablingMap(); 

    const std::map<std::string, std::string>* GetPtoTrigRoads() const {return p_trigroads;}

    bool isLoaded(void) const {return s_status;}

    public:
    static CablingRPCBase* instance(void);
    static CablingRPCBase* instance(const std::string* ,const std::string*, std::string,bool);
    static CablingRPCBase* instance(std::string,std::string,std::string,bool);
    
    public:
    const CablingRPCBase::RDOmap& give_RDOs(void) const {return m_RDOs;}

    bool give_global_strip_address(unsigned int, int&) const;
    bool give_local_strip_address(ViewType,int,int,int,int&,int&) const;
    bool give_global_connector_address(unsigned int, int&, int&, int&) const;
    bool give_local_connector_address(ViewType,int,int,int,int&,int&,
				      int&) const;
    bool give_global_address(unsigned int,int&,int&,int&,int&) const;
    bool gstrip_2_connector(ViewType,int,int,int,int&,int&,int&,int&,int&, 
                          int&) const;

    const CMAparameters::CMAlist 
          give_CMAs(const int,const ViewType,const int,const int) const;
	  
    bool give_RoI_borders (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& EtaLowBorder,
			   unsigned int& EtaHighBorder,
			   unsigned int& PhiLowBorder,
			   unsigned int& PhiHighBorder) const;

    bool give_LowPt_borders (unsigned short int SubsystemId,
                             unsigned short int SectorId,
			     unsigned short int RoIId,
			     unsigned int& EtaLowBorder,
			     unsigned int& EtaHighBorder,
			     unsigned int& PhiLowBorder,
			     unsigned int& PhiHighBorder) const;

    bool give_HighPt_borders (unsigned short int SubsystemId,
                              unsigned short int SectorId,
			      unsigned short int RoIId,
			      unsigned int& EtaLowBorder,
			      unsigned int& EtaHighBorder,
			      unsigned int& PhiLowBorder,
			      unsigned int& PhiHighBorder) const;
    
    int give_doubletZ(unsigned short int SubsystemId,unsigned short int SectorId, int RPCStation, int RPCChamber) const;
    int give_doubletPhi(int LogicSector) const;
   
    bool give_cma_layout (ViewType side, 
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
			                 int& stop_confirm_st) const;

#ifndef LVL1_STANDALONE
    bool give_PAD_address (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& logic_sector,
			   unsigned short int& PADId,
			   Identifier& PadId) const;

    bool give_PAD_address (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& logic_sector,
			   unsigned short int& PADId,
			   unsigned int& PadId) const;

#endif

const CMAparameters* give_CMA(unsigned short int SubsystemId,
                              unsigned short int SectorId,
	                      unsigned short int PADId,
                              unsigned short int CMAId ) const;


bool correct (unsigned short int SubsystemId,
              unsigned short int SectorId,
	      unsigned short int PADId,
	      unsigned short int CMAId,
	      CMAinput           it,
	      unsigned int       layer,
	      unsigned short int Channel1,
	      unsigned short int Channel2,
	      short int number,
	      L1RPCcabCorrection type);
		     
  bool largeSector(std::string stName) const;
  unsigned long int strip_code_fromOffline (int etaPhiView, int logicSector, int cablingStation, int gasGap, int zIndexInCablingStation, int strip) const;
  unsigned long int strip_code_fromOffId (std::string stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip); 
  
  RPCofflineId strip_id_fromCode(unsigned long int strip_code);
  
  unsigned int computeZIndexInCablingStation(std::string stationName, int logicSector, 
					     int stationEta, int stationPhi, int doubletR, int doubletZ, 
					     int cablingStation);

std::list<unsigned int> give_strip_code    (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const;

std::list<RPCofflineId> give_strip_id      (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const;


    friend std::ostream& operator<< (std::ostream&, const CablingRPC&);
    
    friend class ::MuonRPC_CablingSvc;
};


inline std::ostream& 
operator<<(std::ostream& stream,const CablingRPC& logic)
{
    __osstream display;

    logic.PrintMap(display);
    stream << display.str();
    return stream;
}


#ifndef LVL1_STANDALONE
//this will become redundant after we migrate to Gaudi 19
inline MsgStream& 
operator<<(MsgStream& os, const CablingRPC& logic) {
  os.stream() << logic;
  return os;
}
#endif


#ifdef LVL1_STANDALONE

class ShowCabling;
std::ostream& operator<<(std::ostream& ,const ShowCabling& );


class ShowCabling
{
    public:
    const int m_type;
    const int m_station;
    const int m_obj;
    const std::string m_element;
    const bool m_detail;

    public:
    ShowCabling(int,int,std::string,int,bool);
    ~ShowCabling() {}

    friend std::ostream& operator<< (std::ostream&, const ShowCabling&);
};


inline std::ostream& 
operator<<(std::ostream& stream,const ShowCabling& request)
{
    __osstream display;

    const CablingRPCBase* cabling = CablingRPC::instance();

    cabling->PrintType(display,
          request.m_type,request.m_station,request.m_element,
	          request.m_obj,request.m_detail);
    stream << display.str();
    return stream;
}


#endif


}

#endif   //CablingRPC_H
