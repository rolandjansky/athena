## Example configuration for DirectPhotonFilter setting up defaults

if not hasattr( filtSeq, "DirectPhotonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DirectPhotonFilter
    filtSeq += DirectPhotonFilter()
    pass

"""
DirectPhotonFilter = filtSeq.DirectPhotonFilter

DirectPhotonFilter.NPhotons = 1

# apply cut inclusively for any photon in the event (False) or 
# ordered by pT for the given number of photons (True, default)
DirectPhotonFilter.OrderPhotons = True

# if OrderPhotons=True: can contain multiple values for leading, subleading, ...
# if OrderPhotons=False: the first value will be used for the inclusive cut
DirectPhotonFilter.Ptmin = [ 10000. ]

# optional, unlimited by default
DirectPhotonFilter.Ptmax = [ 100000000. ]

# eta range
DirectPhotonFilter.Etacut = 2.50

# Accept also prompt photons from SUSY decays
DirectPhotonFilter.AllowSUSYDecay = True
"""
