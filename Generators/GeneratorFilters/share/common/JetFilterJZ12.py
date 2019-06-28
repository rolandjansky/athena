include ("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq,filtSeq,runArgs.ecmEnergy, 0.6)
JZSlice(12, filtSeq)


