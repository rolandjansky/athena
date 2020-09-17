## Instantiate the TTbarWToLeptonFilter, including adding it to the stream requirement

if not hasattr(filtSeq, "TTbarLJetsVptFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarLJetsVptFilter
    filtSeq += TTbarLJetsVptFilter()

filtSeq.TTbarLJetsVptFilter.NumLeptons = 1
filtSeq.TTbarLJetsVptFilter.Ptcut =  22000.
filtSeq.TTbarLJetsVptFilter.Etacut=  3.0
filtSeq.TTbarLJetsVptFilter.VptMin= 100000
#filtSeq.TTbarLJetsVptFilter.VptMax= 300000
filtSeq.TTbarLJetsVptFilter.FollowTaus=True
