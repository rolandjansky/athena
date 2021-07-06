/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIVOTDATA_H
#define PIVOTDATA_H

#include <iostream>
#include <list>

#include "MuonCablingTools/RPCdef.h"
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/EtaCMA.h"
#include "RPC_CondCabling/EvenPhiCMA.h"
#include "RPC_CondCabling/OddPhiCMA.h"

namespace RPC_CondCabling {

    class CMApivotdata : BaseObject {
    private:
        typedef std::list<EtaCMA> ETAlist;
        typedef std::list<EvenPhiCMA> EvenPHIlist;
        typedef std::list<OddPhiCMA> OddPHIlist;

        bool m_fail{true};

        std::string m_layout;

        ViewType m_view{ViewType::NoView};
        CMAcoverage m_coverage{CMAcoverage::AllSectors};
        std::string m_covtag{};

        ETAlist m_etaCMA;
        EvenPHIlist m_evenphiCMA;
        OddPHIlist m_oddphiCMA;

        void reset_data(void);
        bool get_data(DBline&, CMAparameters::parseParams& parser);
        bool confirm_data(ViewType, CMAparameters::parseParams& parser);

    public:
        CMApivotdata(DBline&, int, const std::string&, IMessageSvc*);
        virtual ~CMApivotdata() = default;

        std::unique_ptr<EtaCMA> give_eta_cma();
        std::unique_ptr<EvenPhiCMA> give_evenphi_cma();
        std::unique_ptr<OddPhiCMA> give_oddphi_cma();

        void Print(std::ostream&, bool) const;
    };

    template <class X> X& operator<<(X& stream, CMApivotdata& data) {
        data.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling
#endif
