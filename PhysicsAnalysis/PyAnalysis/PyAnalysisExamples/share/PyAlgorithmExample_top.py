#
# $Id: PyAlgorithmExample_top.py,v 1.4 2007-04-23 20:12:52 tmaeno Exp $
# File: PyAlgorithmExample_top.py
# Created: sss, Apr 2005
# Purpose: Example of a non-trivial python-based analysis.
#          Top-level job options.
#


# To run, replace these two assignments as appropriate for your data sample.
# You can then give this as a top-level job options file to athena.

aod_input = [ "AOD.pool.root" ]
is_fast_mc = 0


############################################################################
#
# Initial declarations.
#
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
import AthenaPoolCnvSvc.ReadAthenaPool
EventSelector = svcMgr.EventSelector

# The AOD input file
EventSelector.InputCollections = aod_input

# POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )

# initialize AnalysisCore
include ("PyAnalysisCore/InitPyAnalysisCore.py")

# Looad python algorithm code.
include ("PyAnalysisUtils/AthenaPyAlgorithm.py")

# Load the analysis code.
include ("PyAnalysisExamples/PyAlgorithmExample.py")


#########################################################################

#
# Define algorithm parameters.
#

args = Args()
args.electron_container = ["ElectronCollection",
                           "AtlfastElectronCollection"][is_fast_mc]
args.ele_pt_cut  = 20*GeV
args.ele_eta_cut = 2.5

args.muon_container = ["MuonCollection", "AtlfastMuonCollection"][is_fast_mc]
args.muo_pt_cut  = 6*GeV
args.muo_eta_cut = 2.5

args.taujet_container = ["TauJetCollection",
                         "AtlfastTauJetContainer"][is_fast_mc]
args.taujet_pt_cut = 35*GeV
args.taujet_eta_cut = 4.9
args.taujet_likeli_cut = -6
args.taujet_max_emfrac = 0.9

args.met_container = ["MET_Final", "AtlfastMissingEt"][is_fast_mc]

args.zll_deltar_cut = 5
args.zll_deltam_cut = 10*GeV
args.ztt_deltar_cut = 5
args.ztt_deltam_cut = 15*GeV
args.hzz_deltar_cut = 5

args.met_cut = 20*GeV


#########################################################################


# Initialize the application.
theApp.EvtMax = 100000
theApp.initialize()

# Make an algorithm and add it to the application.
a = PyAlgorithmExample ('alg', args)
theApp.addAlgorithm (a)

# Run the application.
theApp.run()

# Save the histograms using root_pickle.
from PyAnalysisUtils.root_pickle import dump_root
dump_root (a.h, "example.root")

#
# To load the histograms from the root file using PyROOT,
# do something like this:
#
# import ROOT
# from PyAnalysisUtils.root_pickle import load_root
# h = load_root ("example.root")
# h.zll.h.m.Draw()
#
