/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLSTAU_MUGIRLSTAUDICT_H
#define MUGIRLSTAU_MUGIRLSTAUDICT_H

#include "MuGirlStau/StauContainer.h"
#include "MuGirlStau/StauCollection.h"

void dummy_function_for_MuGirlStau_which_forces_dictionaries_generation(
        DataVector<MuGirlNS::StauContainer> stc, DataVector<StauCollection> stcoll)
{
    DataVector<MuGirlNS::StauContainer> aastc = stc;
    DataVector<StauCollection> aastcol = stcoll;

    std::vector<const MuGirlNS::StauContainer*> stauContainer;
    stauContainer.size();
}

#endif // MUGIRLSTAU_MUGIRLSTAUDICT_H
