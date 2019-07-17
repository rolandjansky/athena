## Truth jet filter common config for all JZx and JZxW

#from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
#if not hasattr(prefiltSeq, 'xAODCnv'):
#    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
#prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

# Min and max momenta for the slices
minDict = {0:-1,1:20,2:60,3:160,4:400,5:800,6:1300,7:1800,8:2500,9:3200,10:3900,11:4600,12:5300}
maxDict = {0:20,1:60,2:160,3:400,4:800,5:1300,6:1800,7:2500,8:3200,9:3900,10:4600,11:5300,12:7000}

def JZSlice(x,filtSeq):
#    include("GeneratorFilters/GenerateTruthJets.py")
##    from GenerateTruthJets import PrepareTruthJetInputs, ScheduleAntiKtTruthJets
#    PrepareTruthJetInputs(prefiltSeq)
#    ScheduleAntiKtTruthJets(jetradius=jetR,algseq=prefiltSeq)

#    include("GeneratorFilters/QCDJetFilter.py") 
##    from QCDJetFilter import AddJetFilter
#    AddJetFilter(filtSeq,ecmEnergy)
    
    from AthenaCommon.SystemOfUnits import GeV
#    filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt6TruthJets"
    filtSeq.QCDTruthJetFilter.MinPt = minDict[x]*GeV
    filtSeq.QCDTruthJetFilter.MaxPt = maxDict[x]*GeV
