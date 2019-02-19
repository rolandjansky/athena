from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if not hasattr(prefiltSeq, 'xAODCnv'):
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

#CreateTruthJets(prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

# Turn off ghost association algorithms
include("GeneratorFilters/JetFilter_JZX.py")
#from JetFilter_JZX import JZSlice

#JZSlice(0,prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

def CreateJets(prefiltSeq,filtSeq,ecmEnergy, jetR):
       include("GeneratorFilters/GenerateTruthJets.py")

       PrepareTruthJetInputs(prefiltSeq)
       ScheduleAntiKtTruthJets(jetradius=jetR,algseq=prefiltSeq)

       include("GeneratorFilters/QCDJetFilter.py")
#    from QCDJetFilter import AddJetFilter
       AddJetFilter(filtSeq,ecmEnergy)

       if (jetR==0.4):
          filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt4TruthJets"
       elif (jetR==1.0):
          filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt10TruthJets"
       else:
          filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt6TruthJets" 
