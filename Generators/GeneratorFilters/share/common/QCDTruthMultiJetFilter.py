## Example configuration for QCDTruthMultiJetFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "QCDTruthMultiJetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import QCDTruthMultiJetFilter
    filtSeq += QCDTruthMultiJetFilter()
    pass

"""
QCDTruthMultiJetFilter = filtSeq.QCDTruthMultiJetFilter
QCDTruthMultiJetFilter.Njet = -1
QCDTruthMultiJetFilter.NjetMinPt = 0. *Gaudi::Units::GeV
QCDTruthMultiJetFilter.MinLeadJetPt = 0. *Gaudi::Units::GeV
QCDTruthMultiJetFilter.MaxLeadJetPt = 7000. *Gaudi::Units::GeV # , LHC kinematic limit...
QCDTruthMultiJetFilter.MaxEta = 10.0
QCDTruthMultiJetFilter.TruthJetContainer = "AntiKt4TruthJets"
QCDTruthMultiJetFilter.DoShape = True
"""
