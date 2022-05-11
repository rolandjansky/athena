# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


#------------------------------------
def InDetTrkInJetTypeCfg(flags,name="TrkInJetType"):
 
    acc = ComponentAccumulator()
    
    TrkInJet = CompFactory.InDet.InDetTrkInJetType(name)
    acc.setPrivateTools(TrkInJet)

    return acc





