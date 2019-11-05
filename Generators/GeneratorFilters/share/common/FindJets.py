from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if not hasattr(prefiltSeq, 'xAODCnv'):
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

#CreateTruthJets(prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

# Turn off ghost association algorithms
#include("GeneratorFilters/JetFilter_JZX.py")
#from JetFilter_JZX import JZSlice

#JZSlice(0,prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

# Min and max momenta for the slices
minDict = {0:-1,1:20,2:60,3:160,4:400,5:800,6:1300,7:1800,8:2500,9:3200,10:3900,11:4600,12:5300}
maxDict = {0:20,1:60,2:160,3:400,4:800,5:1300,6:1800,7:2500,8:3200,9:3900,10:4600,11:5300,12:7000}

def CreateJets(prefiltSeq, jetR, mods=""):
       include("GeneratorFilters/GenerateTruthJets.py")

       PrepareTruthJetInputs(prefiltSeq)
       ScheduleAntiKtTruthJets(jetradius=jetR,algseq=prefiltSeq,mods=mods)
       jetcollname = 'AntiKt{0}{1}TruthJets'.format(int(jetR*10),mods)

def AddJetsFilter(filtSeq,ecmEnergy, jetR, mods=""):       
       include("GeneratorFilters/QCDJetFilter.py")
#    from QCDJetFilter import AddJetFilter
       AddJetFilter(filtSeq,ecmEnergy)
       jetcollname = 'AntiKt{0}{1}TruthJets'.format(int(jetR*10),mods)
       filtSeq.QCDTruthJetFilter.TruthJetContainer = jetcollname

def JZSlice(x,filtSeq):
    from AthenaCommon.SystemOfUnits import GeV
    filtSeq.QCDTruthJetFilter.MinPt = minDict[x]*GeV
    filtSeq.QCDTruthJetFilter.MaxPt = maxDict[x]*GeV
