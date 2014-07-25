/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SECTORLOGICSETUP_H
#define SECTORLOGICSETUP_H

#include <map>
#include <list>
#include <algorithm>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/BaseObject.h"
#include "MuonCablingTools/dbline.h"

#include "MuonCablingTools/RPCdecoder.h"

#include "MuonRPC_Cabling/RPCchamberdata.h"
#include "MuonRPC_Cabling/WiredORdata.h"
#include "MuonRPC_Cabling/CMApivotdata.h"
#include "MuonRPC_Cabling/CMAcablingdata.h"

/*
class SectorLogicSetup;

template <class X> X& operator<<(X& stream,const SectorLogicSetup& setup)
{
    char buffer[100000];
    for (int i=0;i<100000;++i) buffer[i] = '\0';
    ostrstream display(buffer,100000);
    setup.PrintElement(display,0,setup.name(),false);
    stream << display.str();
    return stream;
}


template <class X,class Y> X& operator<<(X& stream,const Y& setup)
{
    char buffer[100000];
    for (int i=0;i<100000;++i) buffer[i] = '\0';
    ostrstream display(buffer,100000);
    setup.PrintElement(display,0,setup.name(),false);
    stream << display.str();
    return stream;
}

*/


namespace MuonRPC_Cabling {

class SectorLogicSetup : public BaseObject
{
    public:
    typedef std::multimap < CMAinput, int, std::less <CMAinput> > StationMap;
    typedef std::map < int, RPCchamber, std::less < int > > RPCmap;
    typedef std::map < int, WiredOR, std::less <int> > WORmap;
    typedef std::map <CMAidentity,EtaCMA,std::less <CMAidentity> > EtaCMAmap;
    typedef std::map <CMAidentity,EvenPhiCMA,std::less <CMAidentity> >EvenPhiCMAmap;
    typedef std::map <CMAidentity,OddPhiCMA,std::less <CMAidentity> > OddPhiCMAmap;

    //typedef std::list < const CMAparameters* > CMAlist;
    typedef std::list < int > SECTORlist;

    private:
    std::string m_positive_sector;
    std::string m_negative_sector;

    int m_sector_type;
    SECTORlist m_sectors;

    StationMap m_stations;

    RPCmap RPCs;
    WORmap WORs;
    EtaCMAmap etaCMAs;
    EvenPhiCMAmap evenphiCMAs;
    OddPhiCMAmap oddphiCMAs;

    std::string m_online_database;
    std::string m_layout;
    bool m_cosmic;

    void no_elements(std::string,int);
    void get_station(CMAinput, int);
    
    EtaCMAmap::const_iterator find_etaCMA(int Eta,int Phi) const;
    EvenPhiCMAmap::const_iterator find_evenphiCMA(int Eta,int Phi) const;
    OddPhiCMAmap::const_iterator find_oddphiCMA(int Eta, int Phi) const;
    
    EtaCMAmap::iterator find_etaCMA(int Eta,int Phi);
    EvenPhiCMAmap::iterator find_evenphiCMA(int Eta,int Phi);
    OddPhiCMAmap::iterator find_oddphiCMA(int Eta, int Phi);    

    const static std::map<std::string, std::string>* p_trigroads;

    public:
    SectorLogicSetup();
    SectorLogicSetup(int,std::string,std::string,bool conf);
    SectorLogicSetup(const SectorLogicSetup&);
    ~SectorLogicSetup();

    SectorLogicSetup& operator=(const SectorLogicSetup&);

    std::string positive_sector(void)    const {return m_positive_sector;}
    std::string negative_sector(void)    const {return m_negative_sector;}
    std::string online_database(void)    const {return m_online_database;}
    std::string layout(void)             const {return m_layout;}
    int sector_type(void)                const {return m_sector_type;}
    bool cosmic(void)                    const {return m_cosmic;}
    const SECTORlist& sectors(void)      const {return m_sectors;} 
    const StationMap& giveStations(void) const {return m_stations;}

    const RPCmap& giveRPC(void)  const {return RPCs;}
    const WORmap& giveWOR(void)  const {return WORs;}
    const EtaCMAmap& giveEtaCMA(void)  const {return etaCMAs;}
    const EvenPhiCMAmap& giveEvenPhiCMA(void) const {return evenphiCMAs;}
    const OddPhiCMAmap&  giveOddPhiCMA(void)  const {return oddphiCMAs;}
    

    const RPCchamber* find_chamber(int,int) const;
    
    RPCchamber* find_chamber(int,int);
    WiredOR* find_wor(int,int);
    WiredOR* previousWOR(const WiredOR&);
    EtaCMA* previousCMA(const EtaCMA&);
    EvenPhiCMA* previousCMA(const EvenPhiCMA&);
    OddPhiCMA*  previousCMA(const OddPhiCMA&);
    std::list< const EtaCMA* > find_eta_CMAs_in_PAD(int);


    bool setup(void);
    bool check(void);

    bool global_strip_add(ViewType,HalfType,int,int,int,int&) const;
    bool local_strip_add(ViewType,int,int,int&,int&) const;
    bool global_conn_add(ViewType,HalfType,int,int,int,int&,int&,int&) const;
    bool local_conn_add(ViewType,int,int,int&,int&,int&) const;

    const CMAparameters::CMAlist give_CMAs(const int,const ViewType,const int,const int)const;

    bool give_RoI_borders(CMAidentity ETA,CMAidentity PHI,
                          unsigned int& firstEtaCode,
                          unsigned int& lastEtaCode,
                          unsigned int& firstPhiCode,
                          unsigned int& lastPhiCode) const;

    bool give_LowPt_borders(CMAidentity ETA,CMAidentity PHI,
                            unsigned int& firstEtaCode,
                            unsigned int& lastEtaCode,
                            unsigned int& firstPhiCode,
                            unsigned int& lastPhiCode) const;

    bool give_HighPt_borders(CMAidentity ETA,CMAidentity PHI,
                             unsigned int& firstEtaCode,
                             unsigned int& lastEtaCode,
                             unsigned int& firstPhiCode,
                             unsigned int& lastPhiCode) const;
    
    bool give_LowPt_layout(CMAidentity ID,
                          unsigned short int& start_pivot_ch,
			  unsigned int& start_pivot_code,
			  unsigned short int& stop_pivot_ch,
			  unsigned int& stop_pivot_code,
			  unsigned short int& start_confirm_ch,
			  unsigned int& start_confirm_code,
			  unsigned short int& stop_confirm_ch,
			  unsigned int& stop_confirm_code) const;

    bool give_HighPt_layout(CMAidentity ID,
                          unsigned short int& start_pivot_ch,
			  unsigned int& start_pivot_code,
			  unsigned short int& stop_pivot_ch,
			  unsigned int& stop_pivot_code,
			  unsigned short int& start_confirm_ch,
			  unsigned int& start_confirm_code,
			  unsigned short int& stop_confirm_ch,
			  unsigned int& stop_confirm_code) const;


    const CMAparameters* give_CMA(CMAidentity CMA) const;
    
    bool correct(CMAidentity CMA,L1RPCcabCorrection type,CMAinput it,
                 unsigned int layer,unsigned short int Channel1,
	         unsigned short int Channel2,short int num);
    
    std::list<unsigned int> give_strip_code(CMAidentity CMA,int logic_sector,
					    unsigned short int lh,
                                            unsigned short int ijk,
					    unsigned short int Channel) const;

    bool operator += (RPCchamberdata&);
    bool operator += (WiredORdata&);
    bool operator += (CMApivotdata&);
    bool operator += (CMAcablingdata&);

    HalfType side(void) const;
    
    SectorLogicSetup& operator<<(int);

    void PrintElement(std::ostream&,int,std::string,int,bool) const;
    friend std::ostream& operator<< (std::ostream&,const SectorLogicSetup&);

    void SetPtoTrigRoads(const std::map<std::string, std::string>*); 
    const std::map<std::string, std::string>* GetPtoTrigRoads() const {return p_trigroads;} //LBTAG

};

}

#endif
