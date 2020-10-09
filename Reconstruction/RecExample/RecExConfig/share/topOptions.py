#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://uimon.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# AllAlgs, DetDescrVersion needs be set before the include, since several
# secondary flags are configured according to that one
#
# DetDescrVersion="DC2" # mandatory for DC2 data
# AllAlgs = False # if false, all algorithms are switched off by defaults 

doCBNT=False
doNameAuditor=True
PoolESDInput=["/afs/cern.ch/user/d/droussea/public/data/top-1130.ESD.pool.root"]
# doJetRec = False # example , do not run jet reconstruction
# doTruth=False
# number of event to process
#EvtMax=2
#doTauRec=False
# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# By default write ESD, AOD and TAG simultenaously, but note that in production
#      these should be done in separate steps (results might not be bit-by-bit identical).
readESD=True
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doWriteTAG=False # uncomment if do not write TAG

doDumpTES=True

# trigger abort after loading
#PoolESDInput="bidon"

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from __main__ import topSequence


#ALLOWDATADRIVEN=True
from RecExConfig.PhotonMaker import PhotonMaker
aPhotonMaker=PhotonMaker(driving=True)
#exercise singleton
bPhotonMaker=PhotonMaker()
thebPMAlg=bPhotonMaker.PhotonBuilderHandle()

print("Py:top output ",bPhotonMaker.outputTypeKey())
# can modify the al property as follow
# bPhotonMaker.PhotonBuilderHandle().egammaContainer = "egammaContainerTruc"


print("Py: top instance property", thebPMAlg.egammaContainer)



# add topsequence alg
# if I do this alg are copied into topAlg and are run twice
#topSequence.setup()
#exec 'topSequence.__self= []'
#algorithms with property mods
print(topSequence)
# only list algorithms
print(topSequence.sequence())


print("Py:top print again list of top algorithms")
print(theApp.TopAlg)
