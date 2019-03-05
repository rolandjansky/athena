from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if not hasattr(prefiltSeq, 'xAODCnv'):
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

#CreateTruthJets(prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

# Turn off ghost association algorithms
#include("GeneratorFilters/JetFilter_JZX.py")
#from JetFilter_JZX import JZSlice

#JZSlice(0,prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

def CreateJets(prefiltSeq,filtSeq,ecmEnergy, jetR, mods=""):
       include("GeneratorFilters/GenerateTruthJets.py")

       PrepareTruthJetInputs(prefiltSeq)
       ScheduleAntiKtTruthJets(jetradius=jetR,algseq=prefiltSeq,mods=mods)

       include("GeneratorFilters/QCDJetFilter.py")
#    from QCDJetFilter import AddJetFilter
       AddJetFilter(filtSeq,ecmEnergy)
       jetcollname = 'AntiKt{0}{1}TruthJets'.format(int(jetR*10),mods)
       filtSeq.QCDTruthJetFilter.TruthJetContainer = jetcollname
