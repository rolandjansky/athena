from EventBookkeeperTools.CutFlowSvc import cutFlowSvc
cfs=cutFlowSvc
cfs.registerFilter('a')
cfs.registerFilter('a')
cfs.removeFilter('a')
f=cfs.registerFilter('a')

f.registerCut("aCut","dummy selection")
f.passEvent("aCut")
f.registerCut("cut2","blablablabla")
f.passEvent("aCutt")

print cfs




