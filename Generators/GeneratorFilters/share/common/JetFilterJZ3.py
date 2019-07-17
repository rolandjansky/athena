include ("GeneratorFilters/FindJets.py")
CreateJets(prefiltSeq,filtSeq,runArgs.ecmEnergy, 0.6)
JZSlice(3, filtSeq)


