# Example of LeptonPairFilter configuration as used in MadGraphControl_SimplifiedModel_BB_tC1.py

evgenLog.info('Using LeptonPairFilter accepting SS lepton pairs with massive parents')
from GeneratorFilters.GeneratorFiltersConf import LeptonPairFilter
filtSeq += LeptonPairFilter('SS3LFilter')
myFilter = filtSeq.SS3LFilter
myFilter.NLeptons_Min = 0
myFilter.NLeptons_Max = -1
myFilter.Ptcut = 10000
myFilter.Etacut = 2.8
myFilter.NSFOS_Min = -1
myFilter.NSFOS_Max = -1
myFilter.NOFOS_Min = -1
myFilter.NOFOS_Max = -1
myFilter.NSFSS_Min = -1
myFilter.NSFSS_Max = -1
myFilter.NOFSS_Min = -1
myFilter.NOFSS_Max = -1
myFilter.NPairSum_Max = -1
myFilter.NPairSum_Min = 1
myFilter.UseSFOSInSum = False
myFilter.UseSFSSInSum = True
myFilter.UseOFOSInSum = False
myFilter.UseOFSSInSum = True
myFilter.OnlyMassiveParents = True
