/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCHAMBERDATA_H
#define RPCCHAMBERDATA_H

#include <list>

#include "MuonCablingTools/RPCdecoder.h"
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/RPCchamber.h"

namespace RPC_CondCabling {

    class RPCchamberdata : BaseObject {
    private:
        typedef std::list<RPCchamber> RPClist;
        int m_station{0};
        bool m_fail{true};

        RPClist m_rpc;

        void reset_data();
        bool get_data(DBline&, RPCchamber::chamberParameters& params);
        bool confirm_connectors(ViewType, RPCchamber::chamberParameters& params);
        bool confirm_ijk(ViewType, RPCchamber::chamberParameters& params);

    public:
        RPCchamberdata(DBline&, int);
        virtual ~RPCchamberdata() = default;

        std::unique_ptr<RPCchamber> give_rpc();

        int station() const { return m_station; }

        virtual void Print(std::ostream&, bool) const override;
    };

    template <class X> X& operator<<(X& stream, RPCchamberdata& data) {
        data.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling

#endif
