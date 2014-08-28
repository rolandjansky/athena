####################################################
# PAU steering options
# If commented, default will be used
####################################################

####################################################
# Input and output
####################################################

# Input dataset. Feeds EventSelector.InputCollections
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ "AOD.pool.root" ]
# athenaCommonFlags.FilesInput = [ "ESD.pool.root" ]

# from AthenaCommon.AppMgr import theApp
athenaCommonFlags.EvtMax = -1
# athenaCommonFlags.EvtMax=1000
# from AthenaCommon.AppMgr import ServiceMgr
# ServiceMgr.EventSelector.SkipEvents=10


from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags

# Output file. Feeds THistSvc.Output
if not PAUflags.FileName.statusOn:
    PAUflags.FileName = "PAUReco.root"

####################################################
# Steering options. Change to fit your needs
####################################################

## To use a GRL, add the appropriate filenames to the list below.
## Find the most up-to-date good run list(s) here:
## https://twiki.cern.ch/twiki/bin/view/AtlasProtected/GoodRunListsForData
## and include the file name(s) in GoodRunsListVec list. Example:
##
# PAUflags.GoodRunsListVec  = [ 'allruns_collisions.xml' ]

## set the production release here (default 15.5)
## without 3rd, 4th digits (i.e. 14.2.25 => 14.2 )
##
# PAUflags.ProductionRelease = ???

PAUflags.DumpNtuple                  = True  # set to false to not dump PAUReco, HggUserData TTrees
PAUflags.DumpHistograms              = True  # set to false to not dump Histograms
PAUflags.DumpTracks                  = True  # dump all reco tracks
PAUflags.FilteringTracks             = True  # apply track filter
PAUflags.DumpAdditionalEtCones       = True  # add more calo isolation vars
PAUflags.DumpAdditionalPartonEtCones = False  # add more parton isolation vars
PAUflags.DumpCells                   = False # dump all calo cells (BEWARE: ntuple gets BIGGER! --- HUGE on ESD)
PAUflags.DumpFilteredCells           = False # dump filtered calo cells
PAUflags.DumpClusters                = False # dump all SW and topo clusters
PAUflags.DumpUEDClusters             = False # dump all 420 topocluster info before and after local had calib
PAUflags.DumpCollectionTree          = False # dump CollectionTree TTree as from CBNT_AthenaAware
PAUflags.DumpElectronTrigger         = True  # dump trigger information for electrons
PAUflags.DoReRunJetFinding           = True  # re-run low pT jet finding, for UE/pileup corrections to isolation cones
PAUflags.RunEgammaFilter             = False # run the EMClusterCommStream_EGammaCountingFilter: selects events with >=1 egamma object
PAUflags.ElectronToPhoton            = True  # perform electron->photon extrapolation and dump to ntuple

########################################################
include("PhotonAnalysisUtils/PAUanalysis_jobOptions.py")
########################################################
