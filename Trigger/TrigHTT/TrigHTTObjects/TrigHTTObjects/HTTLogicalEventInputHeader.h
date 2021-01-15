// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTLOGICALEVENTINPUTHEADER_H
#define HTTLOGICALEVENTINPUTHEADER_H

#include <TObject.h>

#include <vector>
#include <iostream>
#include <sstream>

#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"


class HTTEventInfo;
class HTTOptionalEventInfo;

class HTTLogicalEventInputHeader: public TObject
{
    public:

        HTTLogicalEventInputHeader() = default;
        virtual ~HTTLogicalEventInputHeader() = default;

        void reset(); //reset per event variables

        void newEvent(HTTEventInfo& event) { reset(); m_event=event; }
        HTTEventInfo const & event() const { return m_event; }

        HTTOptionalEventInfo const & optional() const { return m_optional; }
        void setOptional(HTTOptionalEventInfo o) { m_optional=o; }

        //  handling towers
        const std::vector<HTTTowerInputHeader>& towers() const { return m_towers; }
        int nTowers() const { return m_towers.size(); }
        void addTower(HTTTowerInputHeader s) { m_towers.push_back(s); }
        HTTTowerInputHeader* getTower(size_t index) {return &m_towers[index];} //get the pointer

    private:
        HTTEventInfo                      m_event;
        HTTOptionalEventInfo              m_optional;
        std::vector<HTTTowerInputHeader>  m_towers;

        ClassDef(HTTLogicalEventInputHeader, 1)
};


std::ostream& operator<<(std::ostream&, const HTTLogicalEventInputHeader&);



#endif // HTTEVENTINPUTHEADER_H
