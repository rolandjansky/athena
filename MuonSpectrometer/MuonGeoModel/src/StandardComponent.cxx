/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/StandardComponent.h"

#include "MuonGeoModel/CHV_Technology.h"
#include "MuonGeoModel/CMI_Technology.h"
#include "MuonGeoModel/CRO_Technology.h"
#include "MuonGeoModel/LBI_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Technology.h"

#include <ostream>
#include <string>

namespace MuonGM {

    StandardComponent::StandardComponent() : Component() {
        posx = 0.;
        posy = 0.;
        posz = 0.;
        deadx = 0.;
        deady = 0.;
        excent = 0.;
        iswap = 0;
        index = 0;
        dead3 = 0.;
    }

    StandardComponent::StandardComponent(const StandardComponent &c) : Component(c) {
        posx = c.posx;
        posy = c.posy;
        posz = c.posz;
        deadx = c.deadx;
        deady = c.deady;
        excent = c.excent;
        iswap = c.iswap;
        index = c.index;
        dead3 = c.dead3;
    }

    StandardComponent &StandardComponent::operator=(const StandardComponent &c) {
        if (this != &c) {
            Component::operator=(c);
            posx = c.posx;
            posy = c.posy;
            posz = c.posz;
            deadx = c.deadx;
            deady = c.deady;
            excent = c.excent;
            iswap = c.iswap;
            index = c.index;
            dead3 = c.dead3;
        }
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const StandardComponent &c) {
        os << "Component " << c.name << std::endl;
        os << c.posx << " " << c.posy << " " << c.posz << " " << c.index << " " << c.name;
        os << " " << c.iswap << " " << c.dx1 << " " << c.dx2 << " " << c.dy << " ";
        os << c.excent << " " << c.deadx << " " << c.deady << std::endl;
        return os;
    }

    double StandardComponent::GetThickness() const {
        MYSQL *mysql = MYSQL::GetPointer();
        Technology *tec = mysql->GetTechnology(name);

        if (name.substr(0, 3) == "CHV") {
            CHV *chv = (CHV *)tec;
            return chv->height;
        } else if (name.substr(0, 3) == "CRO") {
            CRO *chv = (CRO *)tec;
            return chv->height;
        } else if (name.substr(0, 3) == "CMI") {
            CMI *chv = (CMI *)tec;
            return chv->height;
        } else if (name.substr(0, 3) == "LBI" || name.substr(0, 2) == "LB") {
            LBI *chv = (LBI *)tec;
            return chv->height;
        }
        return mysql->GetTechnology(name)->thickness;
    }

} // namespace MuonGM
