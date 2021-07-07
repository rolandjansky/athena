/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SECTORLOGICSETUP_H
#define SECTORLOGICSETUP_H

#include <algorithm>
#include <list>
#include <map>

#include "MuonCablingTools/BaseObject.h"
#include "MuonCablingTools/RPCdecoder.h"
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/CMAcablingdata.h"
#include "RPC_CondCabling/CMApivotdata.h"
#include "RPC_CondCabling/RPCchamberdata.h"
#include "RPC_CondCabling/WiredORdata.h"

namespace RPC_CondCabling {

    class SectorLogicSetup : public BaseObject {
    public:
        typedef std::multimap<CMAinput, int, std::less<CMAinput> > StationMap;
        typedef std::map<int, RPCchamber, std::less<int> > RPCmap;
        typedef std::map<int, WiredOR, std::less<int> > WORmap;
        typedef std::map<CMAidentity, EtaCMA, std::less<CMAidentity> > EtaCMAmap;
        typedef std::map<CMAidentity, EvenPhiCMA, std::less<CMAidentity> > EvenPhiCMAmap;
        typedef std::map<CMAidentity, OddPhiCMA, std::less<CMAidentity> > OddPhiCMAmap;

        typedef std::list<int> SECTORlist;

    private:
        std::string m_positive_sector;
        std::string m_negative_sector;

        int m_sector_type;
        SECTORlist m_sectors;

        StationMap m_stations;

        RPCmap m_RPCs;
        WORmap m_WORs;
        EtaCMAmap m_etaCMAs;
        EvenPhiCMAmap m_evenphiCMAs;
        OddPhiCMAmap m_oddphiCMAs;

        std::string m_online_database;
        std::string m_layout;
        bool m_cosmic;

        void no_elements(const std::string&, int);
        void get_station(CMAinput, int);

        EtaCMAmap::const_iterator find_etaCMA(int Eta, int Phi) const;
        EvenPhiCMAmap::const_iterator find_evenphiCMA(int Eta, int Phi) const;
        OddPhiCMAmap::const_iterator find_oddphiCMA(int Eta, int Phi) const;

        EtaCMAmap::iterator find_etaCMA(int Eta, int Phi);
        EvenPhiCMAmap::iterator find_evenphiCMA(int Eta, int Phi);
        OddPhiCMAmap::iterator find_oddphiCMA(int Eta, int Phi);

        const static std::map<std::string, std::string>* s_trigroads;

    public:
        SectorLogicSetup(int, const std::string&, const std::string&, bool, IMessageSvc*);
        SectorLogicSetup(const SectorLogicSetup&) = default;
        virtual ~SectorLogicSetup() = default;

        SectorLogicSetup& operator=(const SectorLogicSetup&) = default;

        std::string positive_sector() const { return m_positive_sector; }
        std::string negative_sector() const { return m_negative_sector; }
        std::string online_database() const { return m_online_database; }
        std::string layout() const { return m_layout; }
        int sector_type() const { return m_sector_type; }
        bool cosmic() const { return m_cosmic; }
        const SECTORlist& sectors() const { return m_sectors; }
        const StationMap& giveStations() const { return m_stations; }

        const RPCmap& giveRPC() const { return m_RPCs; }
        const WORmap& giveWOR() const { return m_WORs; }
        const EtaCMAmap& giveEtaCMA() const { return m_etaCMAs; }
        const EvenPhiCMAmap& giveEvenPhiCMA() const { return m_evenphiCMAs; }
        const OddPhiCMAmap& giveOddPhiCMA() const { return m_oddphiCMAs; }

        const RPCchamber* find_chamber(int, int) const;

        RPCchamber* find_chamber(int, int);
        WiredOR* find_wor(int, int);
        WiredOR* previousWOR(const WiredOR&);
        EtaCMA* previousCMA(const EtaCMA&);
        EvenPhiCMA* previousCMA(const EvenPhiCMA&);
        OddPhiCMA* previousCMA(const OddPhiCMA&);
        std::list<const EtaCMA*> find_eta_CMAs_in_PAD(int);

        bool setup();
        bool check();

        bool global_strip_add(ViewType, HalfType, int, int, int, int&) const;
        bool local_strip_add(ViewType, int, int, int&, int&) const;
        bool global_conn_add(ViewType, HalfType, int, int, int, int&, int&, int&) const;
        bool local_conn_add(ViewType, int, int, int&, int&, int&) const;

        const CMAparameters::CMAlist give_CMAs(const int, const ViewType, const int, const int) const;

        bool give_RoI_borders(CMAidentity ETA, CMAidentity PHI, unsigned int& firstEtaCode, unsigned int& lastEtaCode,
                              unsigned int& firstPhiCode, unsigned int& lastPhiCode) const;

        bool give_LowPt_borders(CMAidentity ETA, CMAidentity PHI, unsigned int& firstEtaCode, unsigned int& lastEtaCode,
                                unsigned int& firstPhiCode, unsigned int& lastPhiCode) const;

        bool give_HighPt_borders(CMAidentity ETA, CMAidentity PHI, unsigned int& firstEtaCode, unsigned int& lastEtaCode,
                                 unsigned int& firstPhiCode, unsigned int& lastPhiCode) const;

        bool give_LowPt_layout(CMAidentity ID, unsigned short int& start_pivot_ch, unsigned int& start_pivot_code,
                               unsigned short int& stop_pivot_ch, unsigned int& stop_pivot_code, unsigned short int& start_confirm_ch,
                               unsigned int& start_confirm_code, unsigned short int& stop_confirm_ch,
                               unsigned int& stop_confirm_code) const;

        bool give_HighPt_layout(CMAidentity ID, unsigned short int& start_pivot_ch, unsigned int& start_pivot_code,
                                unsigned short int& stop_pivot_ch, unsigned int& stop_pivot_code, unsigned short int& start_confirm_ch,
                                unsigned int& start_confirm_code, unsigned short int& stop_confirm_ch,
                                unsigned int& stop_confirm_code) const;

        const CMAparameters* give_CMA(CMAidentity CMA) const;

        bool correct(CMAidentity CMA, L1RPCcabCorrection type, CMAinput it, unsigned int layer, unsigned short int Channel1,
                     unsigned short int Channel2, short int num) const;

        std::list<unsigned int> give_strip_code(CMAidentity CMA, int logic_sector, unsigned short int lh, unsigned short int ijk,
                                                unsigned short int Channel) const;

        bool operator+=(RPCchamberdata&);
        bool operator+=(WiredORdata&);
        bool operator+=(CMApivotdata&);
        bool operator+=(CMAcablingdata&);

        HalfType side() const;

        SectorLogicSetup& operator<<(int);

        void PrintElement(std::ostream&, int, const std::string&, int, bool) const;
        friend std::ostream& operator<<(std::ostream&, const SectorLogicSetup&);

        static void SetPtoTrigRoads(const std::map<std::string, std::string>*);
        const std::map<std::string, std::string>* GetPtoTrigRoads() const { return s_trigroads; }  // LBTAG
    };

}  // namespace RPC_CondCabling

#endif
