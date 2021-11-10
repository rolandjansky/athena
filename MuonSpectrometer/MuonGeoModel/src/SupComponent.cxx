/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/SupComponent.h"

#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/SUP_Technology.h"

namespace MuonGM {

    double SupComponent::xAMDB0(const MYSQL& mysql, const StandardComponent &c) {
        const SUP *t = dynamic_cast<const SUP*>(mysql.GetTechnology(c.name));
        return t->xAMDB0;
    }
    double SupComponent::yAMDB0(const MYSQL& mysql, const StandardComponent &c) {
        const SUP *t = dynamic_cast<const SUP*>(mysql.GetTechnology(c.name));
        return t->yAMDB0;
    }
    double SupComponent::zAMDB0(const MYSQL& mysql, const StandardComponent &c) {
        const SUP *t = dynamic_cast<const SUP*>(mysql.GetTechnology(c.name));
        return t->zAMDB0;
    }
    double SupComponent::bottomsizewrtAMDB0(const MYSQL& mysql, const StandardComponent &c) {
        const SUP *t = dynamic_cast<const SUP*>(mysql.GetTechnology(c.name));
        return t->bottomsizewrtAMDB0;
    }
    double SupComponent::topsizewrtAMDB0(const MYSQL& mysql, const StandardComponent &c) {
        const SUP *t = dynamic_cast<const SUP*>(mysql.GetTechnology(c.name));
        return t->topsizewrtAMDB0;
    }
} // namespace MuonGM
