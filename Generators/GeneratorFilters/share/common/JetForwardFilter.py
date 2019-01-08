## Example configuration for JetForwardFilter setting up defaults

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")
include("GeneratorFilters/JetFilter_Fragment.py")

if not hasattr( filtSeq, "JetForwardFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import JetForwardFilter
    filtSeq += JetForwardFilter()
    pass

"""
JetForwardFilter = filtSeq.JetForwardFilter
# General Jets
JetForwardFilter.JetContainer = "AntiKt6TruthJets"

# Allow a hard cut on the jet system
JetForwardFilter.MinDeltaY = 0.0 #absolute deltaY!
JetForwardFilter.MaxDeltaY = 100.0
JetForwardFilter.MinPt1 = 12.0    # In CLHEP::GeV
JetForwardFilter.MaxPt1 = 70000.0 # In CLHEP::GeV
JetForwardFilter.MinPt2 = 12.0    # In CLHEP::GeV
JetForwardFilter.MaxPt2 = 70000.0 # In CLHEP::GeV

# Allow an arbitrary number of Gaussian distributions to describe the delta(y)-pt distribution
#JetForwardFilter.muXs = empty 
#JetForwardFilter.muYs = empty 
#JetForwardFilter.sigmaXs = empty 
#JetForwardFilter.sigmaYs = empty 
#JetForwardFilter.rhos = empty 
#JetForwardFilter.weights = empty 

JetForwardFilter.SuppressionFactor = 1.0
"""
