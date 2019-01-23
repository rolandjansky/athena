##-----------------------------------------------------------------------------
## Name: PhysDESDM_RPVLL.py
##
## Author: Nick Barlow (Cambridge)
## Email:  Nicholas.Robert.Barlow@cern.ch
## Author: Christian Ohm (Stockholm)
## Email:  Christian.Ohm@cern.ch
## Author: Mike Flowerdew (Munich MPI)
## Email:  Michael.Flowerdew@cern.ch
##
## Description: This defines the content of the DRAW_RPVLL output stream.
##
##-----------------------------------------------------------------------------

# ##########################################################################################
# Imports and initial setup
# ##########################################################################################

print "Executing PhysDRAW_RPVLL"

from LongLivedParticleDPDMaker.LongLivedDPDFlags import WriteRPVLLStream
from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import primRPVLLDESDM
print dir(primRPVLLDESDM)
print primRPVLLDESDM.__module__
print primRPVLLDESDM.__name__
print primRPVLLDESDM
 
RPVLL_augmentationKernels = []

RPVLL_filterTools = []


RPVLLfilterNames = [] # Filters should append their final selection algo names to this list
filtersToBookkeep = [] # Not sure what this does?

# ##########################################################################################
# Filter configuration
# ##########################################################################################

## For each analysis, include a job options file that defines the filter 
## for that analysis and adds it to the top filter

# ##########################################################################################
# First, the filters which are *definitely* planned for Run 2



## Dilepton search (contact Mike Flowerdew)
if primRPVLLDESDM.doDiLep():
    include("LongLivedParticleDPDMaker/PhysDESDM_DiLep.py")
    pass

## RPV search based on displaced vertex finding in the ID (contact: Nick Barlow)
if primRPVLLDESDM.doDV():
    include("LongLivedParticleDPDMaker/PhysDESDM_DV.py")
    pass

# Kink-based chargino search (contact: Shimpei Yamamoto)
if primRPVLLDESDM.doKinkedTrack():
    include("LongLivedParticleDPDMaker/PhysDESDM_KinkedTrack.py")
    pass

## Emerging jet search (contact James Beacham)
if primRPVLLDESDM.doEmerging():
    include("LongLivedParticleDPDMaker/PhysDESDM_Emerging.py")
    pass

## Stopped R-hadron search (contact: Andy Haas)
if primRPVLLDESDM.doStopped():
    include("LongLivedParticleDPDMaker/PhysDESDM_Stopped.py")
    pass

## Highly ionizing particles search (contact: Avishek Chatterjee)
if primRPVLLDESDM.doHips():
    include("LongLivedParticleDPDMaker/PhysDESDM_Hips.py")
    pass

## Heavy neutral lepton search (contact: Avishek Chatterjee)
if primRPVLLDESDM.doHnl():
    include("LongLivedParticleDPDMaker/PhysDESDM_HNL.py")
    pass

# Exotics displaced hadronic jets =search (contact: Heather Russell)
if primRPVLLDESDM.doHV():
    include("LongLivedParticleDPDMaker/PhysDESDM_HV.py")
    pass

# Exotics VH low track jets search (contact: Amber Roepe)
if primRPVLLDESDM.doVH():
    include("LongLivedParticleDPDMaker/PhysDESDM_VH.py")
    pass


### output stream

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteRAWPhysDPD_RPVLL.StreamName
fileName   = buildFileName( primDPD.WriteRAWPhysDPD_RPVLL )

if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    fileName+=".dat"
    pass
if fileName.endswith("root") :
    fileName = fileName.rstrip(".root")
    fileName+=".dat"
    pass
RPVLL = MSMgr.NewByteStream( streamName, fileName )
RPVLL.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()

RPVLL.AddAcceptAlgs(RPVLLfilterNames)

#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)






