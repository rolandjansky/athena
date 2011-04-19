theApp.Dlls += [ "DPDUtils" ]
theApp.Dlls += [ "DPDUtilsDict" ]
import ROOT
ThinningSvcWrapper = ROOT.ThinningSvcWrapper()

#
# filter is available on the python side for
#
# INavigable4MomentumCollection
# ElectronContainer
# Analysis::MuonContainer
# JetCollection
# Rec::TrackParticleContainer
# CaloClusterContainer
#
# to filter electrons do:
#
# ec   = PyParticleTools.getElectrons("ElectronAODCollection")
# vbec = ROOT.std.vector(bool)()
# # loop over electrons and keep some
# for i in range(ec.size()) :
#     if ec[i].author() == 1 and ec[i].isem() == 0 :
#         vbec.push_back(True)
#     else:
#         vbec.push_back(False)
# # invoke the filter
# ThinningSvcWrapper.filter(ec,vbec)
#


