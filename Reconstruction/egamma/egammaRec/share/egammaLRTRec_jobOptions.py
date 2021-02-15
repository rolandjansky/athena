# all these tests, except egamma, should be in egammaGetter
from egammaRec.egammaRecFlags import jobproperties
from AthenaCommon.Resilience import treatException
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()


# Function to schedule the GSF

def setupGSF_LRT():

    try:
        from egammaRec.LRTEgammaConfig import (
            LRTegammaSelectedTrackCopy)
        LRTegammaSelectedTrackCopy(doPrint=False)  # for debugging
    except Exception:
        treatException(
            "Could not set up the egamma track Selection for LRT GSF."
            "Switch LRT Brem Building off !")
    try:
        from egammaRec.LRTEgammaConfig import (
            LRTEMBremCollectionBuilder)
        LRTEMBremCollectionBuilder(doPrint=False)
    except Exception:
        treatException(
            "Could not set up LRTEMBremCollectionBuilder."
            "Switch LRT Brem Building off !")
    try:
        from egammaRec.LRTEgammaConfig import (
            LRTEMGSFCaloExtensionBuilder)
        LRTEMGSFCaloExtensionBuilder(doPrint=False)
    except Exception:
        treatException("Could not set up LRTEMGSFCaloExtensionBuilder.")


# Function to schedule the Topo cluster based egamma


def setupTopoSeededEgamma_LRT():
    try:
        from egammaRec.LRTtopoEgammaGetter import LRTtopoEgammaGetter
        LRTtopoEgammaGetter(ignoreExistingDataObject=True)
    except Exception:
        treatException(
            "Could not set up  LRTtopoEgammaGetter. Switch it off !")
        # If we wanted Topo based cluster seeded egamma it just failed
        jobproperties.egammaRecFlags.doEgammaCaloSeeded = False
        LRTtopoEgammaGetter(disable=True)


def setupTruthAssociation_LRT():
    try:
        from egammaRec.LRTEgammaConfig import (
            LRTegammaTruthAssociationAlg)
        LRTegammaTruthAssociationAlg(doPrint=False)
    except Exception:
        treatException(
            "Could not set up LRTegammaTruthAssociationAlg. Switched off !")
        LRTegammaTruthAssociationAlg(disable=True)


# Do the actual scheduling

if jobproperties.egammaRecFlags.doBremFinding():
    setupGSF_LRT()

if jobproperties.egammaRecFlags.doEgammaCaloSeeded():
    setupTopoSeededEgamma_LRT()

if (jobproperties.egammaRecFlags.doEgammaTruthAssociation()
        and jobproperties.egammaRecFlags.doEgammaCaloSeeded()):
    setupTruthAssociation_LRT()
