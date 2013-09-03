# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ex:
# triggerRateTools.IgnoreList += ["L2_always","EF_always"]

triggerRateTools.IgnoreList += []

# ex: [name, level, item1, item2, ...],[name, level, ...]
# triggerRateTools.CplxAndList += [["Cplx_And_1","EF","EF_1","EF_2"],["Cplx_And_2","EF","EF_3","EF_4","EF_5"],["Cplx_And_3","EF","EF_3J180","EF_3J60","EF_3J25"]] 

triggerRateTools.CplxAndList += []

# ex: [name, level, item1, item2, ...],[name, level, ...]
# triggerRateTools.CplxOrList += [["Cplx_Or_1","EF","EF_1","EF_2"],["Cplx_Or_2","EF","EF_3","EF_4","EF_5"],["Cplx_Or_3","EF","EF_3J180","EF_3J60","EF_3J25"]]

triggerRateTools.CplxOrList += []

# same configuration as CplxOrList, except that for [name, level, item1, item2, item3] would create complex signatures for the combinations of "item1", "item1 or item2", "item1 or item2 or item3", etc.

triggerRateTools.CumulateList += []

triggerRateTools.PrescaleOverrideList += []

triggerRateTools.MenusList += []

triggerRateTools.StreamsList += []
