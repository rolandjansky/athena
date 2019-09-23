# all these tests, except egamma, should be in egammaGetter
from CaloRec.CaloRecFlags import jobproperties
from egammaRec.egammaRecFlags import jobproperties
from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags  import recAlgs
from AthenaCommon.Resilience import treatException
from AthenaCommon.DetFlags import DetFlags
from egammaRec import egammaKeys
from egammaRec.egammaKeys import egammaKeysDict
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
jobproperties.egammaRecFlags.print_JobProperties("full")

######The simple cases where we can disable a flag
# Truth
if not rec.doTruth(): 
	jobproperties.egammaRecFlags.doEgammaTruthAssociation = False

#GSF and vertex building need the inner detector 
if not DetFlags.detdescr.ID_on(): 
        jobproperties.egammaRecFlags.doBremFinding=False
        jobproperties.egammaRecFlags.doVertexBuilding=False

#GSF also is seeded by the LarEMClusters for now. So we need these as well
#if not (rec.readESD() or jobproperties.CaloRecFlags.doEmCluster()):
#        jobproperties.egammaRecFlags.doBremFinding=False

#We can not run Forward without having the TopoClusters        
if not (rec.readESD() or jobproperties.CaloRecFlags.doCaloTopoCluster()):
        jobproperties.egammaRecFlags.doEgammaForwardSeeded=False

#######Helper Functions

#Function to schedule the GSF 
def setupGSF():
        try:
                from egammaAlgs.egammaSelectedTrackCopy import egammaSelectedTrackCopy
                egammaSelectedTrackCopy(doPrint=True)
        except Exception:
                treatException("Could not set up the egamma track Selection for GSF. Switch Brem Building off !")
                jobproperties.egammaRecFlags.doBremFinding=False 
        try:
                from egammaAlgs.EMBremCollectionBuilder import EMBremCollectionBuilder
                EMBremCollectionBuilder(doPrint=True)
        except Exception:
                treatException("Could not set up EMBremCollectionBuilder. Switch Brem Building off !")
                jobproperties.egammaRecFlags.doBremFinding=False 
        try:
                from egammaAlgs.EMGSFCaloExtensionBuilder import EMGSFCaloExtensionBuilder
                EMGSFCaloExtensionBuilder(doPrint=True)
        except Exception:
                treatException("Could not set up EMGSFCaloExtensionBuilder.")

#Function to schedule the conversion vertices reconstructions      
def setupVertices():
        # Conversion vertex builder can not run in the default mode without GSF
        try:
                from egammaAlgs.EMVertexBuilder import EMVertexBuilder
                EMVertexBuilder(doPrint=True)
        except Exception:
                treatException("Could not set up the conversion vertex building. Switch vertex building off !")
                jobproperties.egammaRecFlags.doVertexBuilding=False

#Function to schedule the Topo cluster based egamma
def setupTopoSeededEgamma():
        try:
                from egammaRec.topoEgammaGetter import topoEgammaGetter
                topoEgammaGetter(ignoreExistingDataObject=True)
        except Exception:
                treatException("Could not set up  topoEgammaGetter. Switch it off !")
                #If we wanted Topo based cluster seeded egamma it just failed
                jobproperties.egammaRecFlags.doEgammaCaloSeeded=False
                topoEgammaGetter(disable=True)


#Function to schedule the SW seeded egamma (only if we do not do super clusters). 
def setupSWSeededEgamma():    
        try:
                from egammaRec.egammaGetter import egammaGetter
                egammaGetter(ignoreExistingDataObject=True)
        except Exception: 
                treatException("Could not set up egammaGetter. Switch it off !")
                #If we wanted SW based cluster seeded egamma it just failed 
                jobproperties.egammaRecFlags.doEgammaCaloSeeded=False
                egammaGetter(disable=True)

#Function to schedule the  Fwd egamma
def setupFwdSeededEgamma():            
        try:
                from egammaRec.egammaForwardGetter import egammaForwardGetter
                egammaForwardGetter(ignoreExistingDataObject=True)
        except Exception: 
                treatException("Could not set up egammaForwardGetter. Switch it off !")
                jobproperties.egammaRecFlags.doEgammaForwardSeeded=False
                egammaForwardGetter(disable=True)

#Function to schedule the Truth Association
def setupTruthAssociation():            
        try:
                from egammaRec.egammaTruthAssociationAlg import egammaTruthAssociationGetter
                egammaTruthAssociationGetter(ignoreExistingDataObject=True)
        except Exception:
                treatException("Could not set up egammaTruthAssociationAlg. Switched off !")
                jobproperties.egammaRecFlags.doEgammaTruthAssociation=False
                egammaTruthAssociationGetter(disable=True)


######## Do the actual scheduling
if jobproperties.egammaRecFlags.doBremFinding():
        setupGSF()
if jobproperties.egammaRecFlags.doVertexBuilding():
        setupVertices()
#Calo seeded egamma
# Either we can do TopoSeeded superClusters
# Or we can do SW seeded
# Or we can do nothing (i.e no ESD nor CaloTopo nor LarEM clusters) 
if jobproperties.egammaRecFlags.doEgammaCaloSeeded():        
        if jobproperties.egammaRecFlags.doSuperclusters() and (jobproperties.CaloRecFlags.doCaloTopoCluster() or rec.readESD()):
                setupTopoSeededEgamma()        
        elif not jobproperties.egammaRecFlags.doSuperclusters() and (jobproperties.CaloRecFlags.doEmCluster() or rec.readESD()):
                setupSWSeededEgamma()
        else:
                jobproperties.egammaRecFlags.doEgammaCaloSeeded=False

if jobproperties.egammaRecFlags.doEgammaForwardSeeded():
        setupFwdSeededEgamma()

if jobproperties.egammaRecFlags.doEgammaTruthAssociation() and jobproperties.egammaRecFlags.doEgammaCaloSeeded(): 
        setupTruthAssociation()

