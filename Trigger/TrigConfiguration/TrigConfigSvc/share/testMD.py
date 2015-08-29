import user
import sys
import ROOT
import PyCintex
#import AthenaROOTAccess.transientTree

# for a single file give a file handle
#f = ROOT.TFile.Open ('AOD.pool.root')

# for a set of files give a list of names
f = ['AOD.pool.root']


from TrigConfigSvc.TriggerConfigARA import TriggerConfigARA
trigconf = TriggerConfigARA(f)

#print "Making transient collection ..."
#tt = AthenaROOTAccess.transientTree.makeTree(f)

# normally you would get the run and lb number from the collection tree (from each event)
# you can call update on every new lb (or even event), information is only read when IOV becomes invalid
trigconf.update(5200,0)

# NOTE: if you are using prescales you should call trigconf.update() whenever the lumiblock changes, since the l1 prescales can change at the lb boundary

# print summary
print trigconf

# access to L1 TriggerItems through trigconf.L1Items
for (name,item) in trigconf.L1Items.items():
    print item.ctpId(), item

# access to HLT Chains through trigconf.HLTChains
for (name,ch) in trigconf.HLTChains.items():
    print ch.chain_counter(), ch

# the list of all items and chain names
trigconf.getConfiguredChainNames()

# to check if a item or chain is in a menu use trigconf.isConfigured(name)
testnames = ["L1_TAU9I_XE30", "17AND4", "EF_e10_mu6"]
for name in testnames:
    if trigconf.isConfigured(name):
        print name,"is configured"
    else:
        print name,"is not configured"

# the full prescale of a chain can be gotten using
print "Full prescale factor of chain 'EF_e10_mu6':",trigconf.getFullChainPrescaleFactor("EF_e10_mu6")


# this would do nothing, since the IOV is still valid
trigconf.update(5200,17)

# this triggers reading of the new menu (that of run 5300)
trigconf.update(5400,39)
print trigconf
