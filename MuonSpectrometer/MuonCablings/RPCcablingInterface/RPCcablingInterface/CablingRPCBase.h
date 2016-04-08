/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CABLINGRPCBASE_H
#define CABLINGRPCBASE_H

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"
#endif

#include <map>
#include <string>
#include "MuonCablingTools/BaseObject.h"
#include "MuonCablingTools/RPCdef.h"

#include "RPCcablingInterface/CMAparameters.h"
#include "RPCcablingInterface/RDOindex.h"
#include "RPCcablingInterface/RPCofflineId.h"


class CablingRPCBase;
std::ostream& operator<<(std::ostream&,const CablingRPCBase&);


class CablingRPCBase : public BaseObject
{
    public:
    typedef std::map <int,RDOindex,std::less < int > > RDOmap;


    protected:
    CablingRPCBase(void);
   
    
    public:
    virtual ~CablingRPCBase(void);

    virtual void PrintMap(std::ostream&) const=0;
    virtual void PrintType(std::ostream&,int,int,std::string,int,bool) const=0;
    virtual void PrintSector(std::ostream&,int,int,std::string,int,bool) const=0;

    virtual bool isLoaded(void) const=0;

    public:
    virtual const RDOmap& give_RDOs(void) const=0;

    virtual bool give_global_strip_address(unsigned int, int&) const=0;
    virtual bool give_local_strip_address(ViewType,int,int,int,int&,int&) const=0;
    virtual bool give_global_connector_address(unsigned int, int&, int&, int&) const=0;
    virtual bool give_local_connector_address(ViewType,int,int,int,int&,int&,
				      int&) const=0;
    virtual bool give_global_address(unsigned int,int&,int&,int&,int&) const=0;
    virtual bool gstrip_2_connector(ViewType,int,int,int,int&,int&,int&,int&,int&, 
                          int&) const=0;

    virtual const CMAparameters::CMAlist 
          give_CMAs(const int,const ViewType,const int,const int) const=0;
	  
    virtual bool give_RoI_borders (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& EtaLowBorder,
			   unsigned int& EtaHighBorder,
			   unsigned int& PhiLowBorder,
			   unsigned int& PhiHighBorder) const=0;
    
    virtual bool give_LowPt_borders (unsigned short int SubsystemId,
                             unsigned short int SectorId,
			     unsigned short int RoIId,
			     unsigned int& EtaLowBorder,
			     unsigned int& EtaHighBorder,
			     unsigned int& PhiLowBorder,
			     unsigned int& PhiHighBorder) const=0;

    virtual bool give_HighPt_borders (unsigned short int SubsystemId,
                              unsigned short int SectorId,
			      unsigned short int RoIId,
			      unsigned int& EtaLowBorder,
			      unsigned int& EtaHighBorder,
			      unsigned int& PhiLowBorder,
			      unsigned int& PhiHighBorder) const=0;
    
    virtual int give_doubletZ(unsigned short int SubsystemId,
                              unsigned short int SectorId, int RPCStation, 
			      int RPCChamber) const=0;
			      
    virtual int give_doubletPhi(int LogicSector) const=0;
   
    virtual bool give_cma_layout (ViewType side, 
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
			                 int& stop_confirm_st) const=0;

#ifndef LVL1_STANDALONE
    virtual bool give_PAD_address (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& logic_sector,
			   unsigned short int& PADId,
			   Identifier& PadId) const=0;

    virtual bool give_PAD_address (unsigned short int SubsystemId,
                           unsigned short int SectorId,
			   unsigned short int RoIId,
			   unsigned int& logic_sector,
			   unsigned short int& PADId,
			   unsigned int& PadId) const=0;

#endif

virtual const CMAparameters* give_CMA(unsigned short int SubsystemId,
                              unsigned short int SectorId,
	                      unsigned short int PADId,
                              unsigned short int CMAId ) const=0;


virtual bool correct (unsigned short int SubsystemId,
              unsigned short int SectorId,
	      unsigned short int PADId,
	      unsigned short int CMAId,
	      CMAinput           it,
	      unsigned int       layer,
	      unsigned short int Channel1,
	      unsigned short int Channel2,
	      short int number,
	      L1RPCcabCorrection type)=0;
		     

virtual unsigned long int strip_code_fromOffId(std::string stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip)=0;  

virtual RPCofflineId strip_id_fromCode(unsigned long int strip_code_cab)=0;

virtual std::list<unsigned int> give_strip_code    (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const=0;

virtual std::list<RPCofflineId> give_strip_id      (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const=0;


    friend std::ostream& operator<< (std::ostream&, const CablingRPCBase&);

};


inline std::ostream& 
operator<<(std::ostream& stream,const CablingRPCBase& logic)
{

    __osstream display;
    (&logic)->PrintMap(display);
    stream << display.str();
    return stream;
}


#ifndef LVL1_STANDALONE
//this will become redundant after we migrate to Gaudi 19
inline MsgStream& 
operator<<(MsgStream& os, const CablingRPCBase& logic) {
  os.stream() << logic;
  return os;
}


#endif



#endif  //CABLINGRPCBASE_H
