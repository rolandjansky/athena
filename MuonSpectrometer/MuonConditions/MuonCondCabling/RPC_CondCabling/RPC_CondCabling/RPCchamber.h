/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCHAMBER_H
#define RPCCHAMBER_H

#include <list>
#include <vector>

#include "MuonCablingTools/RPCdecoder.h"
#include "MuonCablingTools/ShowRequest.h"
#include "RPC_CondCabling/CablingObject.h"

namespace RPC_CondCabling {

    class EtaCMA;
    class WiredOR;
    class SectorLogicSetup;

    class RPCchamber : public CablingObject {
    public:
        /// Helper struct containing the minimum number of
        /// parameters on top of the CablingObject in order
        /// to create an RPC chamber object.
        struct chamberDefineParams {
            chamberDefineParams() = default;                                       // default constructor
            chamberDefineParams(chamberDefineParams&&) = default;                  // move constructor
            chamberDefineParams(const chamberDefineParams&) = default;             // copy constructor
            chamberDefineParams& operator=(const chamberDefineParams&) = default;  // assignment operator

            std::string chamberName{""};  // RPC chamber name

            int stationEta{-INT_MAX};  /// StaionEta as defined into the offline ID
            int doubletR{-INT_MAX};    /// DoubletR as defined into the offline ID
            int doubletZ{-INT_MAX};    /// DoubletZ as defined into the offline ID

            int phiReadOutPanels{0};  /// Phi readout multiplicity

            int stripsInEtaCon{0};  /// Number of eta strips into connectors
            int stripsInPhiCon{0};  /// Number of phi strips into connectors

            int etaStrips{0};  /// Number of eta strips
            int phiStrips{0};  /// Number of phi strips

            int etaConnectors{0};  /// Number of eta connectors
            int phiConnectors{0};  /// Number of phi connectors

            int ijk_EtaReadOut{0};  /// readout schema of the gas gaps in eta:
                                    ///     01 = gap 0 in layer 0, gap 1 in layer 1
                                    ///     10 = gap 0 in layer 1, gap 1 in layer 0
            int ijk_PhiReadOut{0};  /// readout schema of the gas gaps in phi:
                                    ///     01 = gap 0 in layer 0, gap 1 in layer 1
                                    ///     10 = gap 0 in layer 1, gap 1 in layer 0
        };
        struct chamberParameters : public chamberDefineParams, public cablingParameters {
            chamberParameters() = default;                                     // default constructor
            chamberParameters(chamberParameters&&) = default;                  // move constructor
            chamberParameters(const chamberParameters&) = default;             // copy constructor
            chamberParameters& operator=(const chamberParameters&) = default;  // assignment operator
        };

    private:
        typedef std::vector<int> ReadoutCh;
        typedef std::list<const EtaCMA*> CMAlist;
        typedef std::list<const WiredOR*> WORlist;

        chamberDefineParams m_params{};
        int m_eta_strip_global{0};  // Start number of eta strips in global coordinate
        int m_eta_conn_global{0};   // Start number of eta connector in global coo.

        ReadoutCh m_eta_read_mul;

        CMAlist m_readoutCMAs;
        WORlist m_readoutWORs;

        int residual(ViewType, int) const;
        void error(const std::string&) const;

    public:
        RPCchamber(const RPCchamber::chamberParameters& params, IMessageSvc* msgSvc);
        RPCchamber(const RPCchamber&) = default;
        virtual ~RPCchamber();

        RPCchamber& operator=(const RPCchamber&) = default;

        void set_eta_st_global(int);
        void set_eta_co_global(int);

        int eta_strips() const;
        int eta_connectors() const;
        int eta_strip_global() const;
        int eta_conn_global() const;
        int phi_strips() const;

        int phi_connectors() const;
        int ijk_etaReadout() const;
        int ijk_phiReadout() const;

        std::string chamber_name() const;
        std::string stationName() const;
        int stationEta() const;
        int doubletR() const;
        int doubletZ() const;
        int phiReadoutPannels() const;

        int strips_in_Eta_Conn() const;
        int strips_in_Phi_Conn() const;
        const ReadoutCh& eta_read_mul() const;

        const CMAlist& readoutCMAs() const;
        const WORlist& readoutWORs() const;

        std::string extendedName(int) const;
        bool inversion(int) const;

        bool local_strip(ViewType, int, int&) const;
        bool global_strip(ViewType, HalfType, int, int&) const;

        bool local_connector(ViewType, int, int&, int&, int&) const;
        bool global_connector(ViewType, HalfType, int, int&, int&, int&) const;

        bool Gstrip_2_Lnumber(ViewType, int, int&) const;
        bool Gconn_2_Lnumber(ViewType, int, int&, int&) const;

        void Print(std::ostream&, bool) const;

        bool check();
        bool setup(SectorLogicSetup&);

        void add_cma(const EtaCMA*);
        void add_wor(const WiredOR*);
        void add_eta_channel(int);
    };

    template <class X> X& operator<<(X& stream, const RPCchamber& cham) {
        cham.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling
#endif
