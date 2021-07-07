/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WIREDORDATA_H
#define WIREDORDATA_H

#include <list>

#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/WiredOR.h"

namespace RPC_CondCabling {

    class WiredORdata : BaseObject {
    private:
        typedef std::list<WiredOR> WORlist;

        bool m_fail;
        int m_station{-1};
        WORlist m_wor;

        void reset_data();
        bool get_data(DBline&, WiredOR::parseParams&);
        bool confirm_boundary(WiredOR::parseParams&) const;

    public:
        WiredORdata(DBline&, int, IMessageSvc*);
        virtual ~WiredORdata();

        std::unique_ptr<WiredOR> give_wor(void);

        int station() const { return m_station; }

        void Print(std::ostream&, bool) const;
    };

    template <class X> X& operator<<(X& stream, WiredORdata& data) {
        data.Print(stream, false);
        return stream;
    }

}  // namespace RPC_CondCabling

#endif
