/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CABLINGDATA_H
#define CABLINGDATA_H

#include <iostream>
#include <list>

#include "MuonCablingTools/RPCdef.h"
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/EtaCMA.h"
#include "RPC_CondCabling/EvenPhiCMA.h"
#include "RPC_CondCabling/OddPhiCMA.h"

namespace RPC_CondCabling {

    class CMAcablingdata : public BaseObject {
    private:
        typedef std::list<EtaCMA> ETAlist;

        bool m_fail{true};

        ViewType m_view{ViewType::Eta};
        std::string m_covtag{};

        ETAlist m_etaCMA;

        void reset_data(void);
        bool get_data(DBline&, CMAparameters::parseParams& params);
        bool confirm_data(ViewType, const CMAparameters::parseParams& parser);

    public:
        CMAcablingdata(DBline&, int);
        virtual ~CMAcablingdata() = default;

        std::unique_ptr<EtaCMA> give_eta_cma();

        virtual void Print(std::ostream&, bool) const override;
    };

    template <class X> X& operator<<(X& stream, CMAcablingdata& data) {
        data.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling
#endif
