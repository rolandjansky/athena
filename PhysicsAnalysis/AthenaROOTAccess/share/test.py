import user  # look for .pythonrc.py for user init
import ROOT
import AthenaROOTAccess.transientTree

from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

# Put your AOD file here.
aodFile = 'AOD.pool.root'
f = ROOT.TFile.Open (aodFile)
assert f.IsOpen()

# Fill this in if you want to change the names of the transient branches.
branchNames = {}
#branchNames['ElectronCollection'] = 'ele'
#branchNames['PhotonCollection'] = 'gam'

tt = AthenaROOTAccess.transientTree.makeTree(f, branchNames = branchNames)
# tt is the transient tree "CollectionTree_trans" containing the (proxies) to
# all available transient data object in the file f.
# The original, persistent tree is declared as a friend
# of CollectionTree_trans, so that the transient tree will provide
# access to both transient data objects and to their persistent counterparts.


##############################################################################
#
# Now you should be able to do things like:
#
def tryThingsOut() :

   tt.Scan('ElectronAODCollection.e():ElectronAODCollection.cluster().e()')
   tt.Draw('PhotonAODCollection.phi()')
   #NOT YET tree.Show(0)
   #NOT YET tree.StartViewer()
   for iEvt in range(3):
       tt.GetEntry(iEvt)
       print tt.ElectronAODCollection[0].eta()
#
#
#tryThingsOut()
#
