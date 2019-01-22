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
## Description: This defines the content of the DESDM_RPVLL output stream.
##
##-----------------------------------------------------------------------------

# ##########################################################################################
# Imports and initial setup
# ##########################################################################################

# MJF: Needs attention and cleaning up - lots of PrimaryDPDMaker dependencies!

print "Executing PhysDESDM_RPVLL"

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from LongLivedParticleDPDMaker.LongLivedDPDFlags import WriteRPVLLStream
from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import primRPVLLDESDM

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteRPVLLStream.StreamName
fileName   = buildFileName( primDPD.WriteRPVLLStream )
RPVLL      = MSMgr.NewPoolStream( streamName,fileName )

# MJF: Possibly remove if not useful?
if primRPVLLDESDM.prescaleFlags.applyPrescale:
    RPVLL.SetPrescale(primRPVLLDESDM.prescaleFlags.prescaleFactor)

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
### Now has it's own stream.
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

# ##########################################################################################
# Second, filters I'm not sure of (move above this line if definitely needed)

## VH DV search based on displaced vertex finding in the ID along with a W or Z leptonic decay (contact: Andy Haas)
if primRPVLLDESDM.doVH_DV():
    include("LongLivedParticleDPDMaker/PhysDESDM_VH_DV.py")
    pass

## Quirks search (contact: Jim Black)
if primRPVLLDESDM.doQuirks():
    include("LongLivedParticleDPDMaker/PhysDESDM_Quirks.py")
    pass



################# OR together all the filters into one, if at least one analysis selection is active

if RPVLLfilterNames:
    RPVLL.AcceptAlgs(RPVLLfilterNames)

# if the automatic trigger stream logics or command-line overrides did not activate a single selection,
# choose to not write out any events to the output file (can happen e.g. in Reco_trf.py test jobs)
else:
    from PrimaryDPDMaker.DummyFilter import DummyFilter
    dummyFilterName = "RPVLL_DESDMStream_DummyFilter"
    topSequence += DummyFilter(dummyFilterName)
    filtersToBookkeep += dummyFilterName
    vetoAlgsNames=[]
    vetoAlgsNames.append( dummyFilterName )
    RPVLL.VetoAlgs(vetoAlgsNames)

# ##########################################################################################
# Output stream definition
# ##########################################################################################

# Add everything from the input file
from AthenaCommon.KeyStore import CfgKeyStore,CfgItemList
theCKS = CfgKeyStore('KeyStore')
outList = CfgItemList( 'RPVLL' )
outList += theCKS['inputFile']

# Remove items here, as needed
outList.removeAllItems( 'RpcPadContainer#RPCPAD' ) # Avoids crash in 20.1.0.2

# Configure the stream
RPVLL.AddItem(outList())

# make sure bookkeeping info is kept for additional filters
# MJF: Does this do anything useful?
RPVLL.AddOtherAlgsToBookkeep(filtersToBookkeep)

