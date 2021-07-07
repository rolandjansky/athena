/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WIREDOR_H
#define WIREDOR_H

#include <algorithm>
#include <list>
#include <map>
#include <vector>

#include "MuonCablingTools/ShowRequest.h"
#include "RPC_CondCabling/CMAidentity.h"
#include "RPC_CondCabling/CablingObject.h"
#include "RPC_CondCabling/RPCchamber.h"

class CMAparameters;

namespace RPC_CondCabling {

    class SectorLogicSetup;

    class WiredOR : public CablingObject {
    public:
        typedef std::map<int, const RPCchamber*, std::less<int> > RPClink;

        struct defineParams {
            defineParams() = default;
            defineParams(const defineParams&) = default;
            defineParams& operator=(const defineParams&) = default;
            defineParams(defineParams&&) = default;

            ViewType side{ViewType::Phi};  /// strip type put in wired OR
            int start{0};                  /// first RPC chamber to which wired strips belong
            int stop{0};                   /// last RPC chamber to which wired strips belong
        };
        struct parseParams : public defineParams, public cablingParameters {
            parseParams() = default;
            parseParams(const parseParams&) = default;
            parseParams& operator=(const parseParams&) = default;
            parseParams(parseParams&&) = default;
        };

    private:
        defineParams m_params{};

        typedef std::vector<int> ReadoutCh;
        typedef std::list<const CMAparameters*> CMAlist;

        ReadoutCh m_even_read_mul;
        ReadoutCh m_odd_read_mul;

        RPClink m_RPCread;
        CMAlist m_readoutCMAs;

        bool connect(SectorLogicSetup&);

    public:
        WiredOR(parseParams, IMessageSvc*);

        // WiredOR(int, int, int, int, int);
        WiredOR(const WiredOR&) = default;
        virtual ~WiredOR();

        WiredOR& operator=(const WiredOR&) = default;

        ViewType side() const;
        int start() const;
        int stop() const;
        const ReadoutCh& even_read_mul() const;
        const ReadoutCh& odd_read_mul() const;
        const RPClink& RPCread() const;
        const CMAlist& readoutCMAs() const;

        const RPCchamber* connected_rpc(int) const;

        bool check();
        bool setup(SectorLogicSetup&);

        void add_cma(const CMAparameters*);
        void add_even_read_mul(ReadoutCh&);
        void add_odd_read_mul(ReadoutCh&);

        int give_max_phi_strips() const;
        int give_max_eta_strips() const;
        int RPCacquired() const { return m_RPCread.size(); }

        void Print(std::ostream&, bool) const;

        void two_obj_error_message(const std::string&, WiredOR*);
        void error(const std::string&);
    };

    template <class X> X& operator<<(X& stream, const WiredOR& Wor) {
        Wor.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling
#endif
