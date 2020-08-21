# all these tests, except egamma, should be in egammaGetter
from CaloRec.CaloRecFlags import jobproperties
from egammaRec.egammaRecFlags import jobproperties
from RecExConfig.RecFlags import rec
from AthenaCommon.Resilience import treatException
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
jobproperties.egammaRecFlags.print_JobProperties("full")

# The simple cases where we can disable a flag

# Truth
if not rec.doTruth():
    jobproperties.egammaRecFlags.doEgammaTruthAssociation = False

# GSF and vertex building need the inner detector
if not DetFlags.detdescr.ID_on():
    jobproperties.egammaRecFlags.doBremFinding = False
    jobproperties.egammaRecFlags.doVertexBuilding = False

# We can not run without having the Calo
if not (rec.readESD() or jobproperties.CaloRecFlags.doCaloTopoCluster()):
    jobproperties.egammaRecFlags.doEgammaCaloSeeded = False
    jobproperties.egammaRecFlags.doEgammaForwardSeeded = False


# Function to schedule the GSF


def setupGSF():
    try:
        from egammaAlgs.egammaSelectedTrackCopy import (
            egammaSelectedTrackCopy)
        egammaSelectedTrackCopy(doPrint=False)
    except Exception:
        treatException(
            "Could not set up the egamma track Selection for GSF."
            "Switch Brem Building off !")
        jobproperties.egammaRecFlags.doBremFinding = False
    try:
        from egammaAlgs.EMBremCollectionBuilder import (
            EMBremCollectionBuilder)
        EMBremCollectionBuilder(doPrint=False)
    except Exception:
        treatException(
            "Could not set up EMBremCollectionBuilder."
            "Switch Brem Building off !")
        jobproperties.egammaRecFlags.doBremFinding = False
    try:
        from egammaAlgs.EMGSFCaloExtensionBuilder import (
            EMGSFCaloExtensionBuilder)
        EMGSFCaloExtensionBuilder(doPrint=False)
    except Exception:
        treatException("Could not set up EMGSFCaloExtensionBuilder.")

# Function to schedule the conversion vertices reconstructions


def setupVertices():
    # Conversion vertex builder can not run in the default mode without GSF
    try:
        from egammaAlgs.EMVertexBuilder import EMVertexBuilder
        EMVertexBuilder(doPrint=False)
    except Exception:
        treatException(
            "Could not set up the conversion vertex building."
            "Switch vertex building off !")
        jobproperties.egammaRecFlags.doVertexBuilding = False

# Function to schedule the Topo cluster based egamma


def setupTopoSeededEgamma():
    try:
        from egammaRec.topoEgammaGetter import topoEgammaGetter
        topoEgammaGetter(ignoreExistingDataObject=True)
    except Exception:
        treatException("Could not set up  topoEgammaGetter. Switch it off !")
        # If we wanted Topo based cluster seeded egamma it just failed
        jobproperties.egammaRecFlags.doEgammaCaloSeeded = False
        topoEgammaGetter(disable=True)

# Function to schedule the  Fwd egamma


def setupFwdSeededEgamma():
    try:
        from egammaRec.egammaForwardGetter import egammaForwardGetter
        egammaForwardGetter(ignoreExistingDataObject=True)
    except Exception:
        treatException("Could not set up egammaForwardGetter. Switch it off !")
        jobproperties.egammaRecFlags.doEgammaForwardSeeded = False
        egammaForwardGetter(disable=True)

# Function to schedule the Truth Association


def setupTruthAssociation():
    try:
        from egammaRec.egammaTruthAssociationAlg import (
            egammaTruthAssociationGetter)
        egammaTruthAssociationGetter(ignoreExistingDataObject=True)
    except Exception:
        treatException(
            "Could not set up egammaTruthAssociationAlg. Switched off !")
        jobproperties.egammaRecFlags.doEgammaTruthAssociation = False
        egammaTruthAssociationGetter(disable=True)


# Do the actual scheduling

if jobproperties.egammaRecFlags.doBremFinding():
    setupGSF()

if jobproperties.egammaRecFlags.doVertexBuilding():
    setupVertices()

if jobproperties.egammaRecFlags.doEgammaCaloSeeded():
    setupTopoSeededEgamma()

if jobproperties.egammaRecFlags.doEgammaForwardSeeded():
    setupFwdSeededEgamma()

if (jobproperties.egammaRecFlags.doEgammaTruthAssociation()
        and jobproperties.egammaRecFlags.doEgammaCaloSeeded()):
    setupTruthAssociation()
