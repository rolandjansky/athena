# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# jet triggers (single Jets)
jetTriggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.j_single)

# single electron triggers (unprescaled)

singleElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.el_single)

# single muon triggers

singleMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.mu_single)

# xe triggers
metTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.xe)

#multi-muon triggers

multiMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.mu_multi)

# multi-el triggers

multiElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.el_multi)

# photon triggers
singlePhotonTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.g_single)

multiPhotonTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.g_multi)
