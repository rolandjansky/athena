#build subtracted cells
from HIJetRec.SubtractedCellGetter import SubtractedCellGetter
subtr_gett=SubtractedCellGetter()



######
import PerfMonComps.DomainsRegistry as pdr

pdr.flag_domain('calo')
from CaloRec.CaloRecFlags import jobproperties
from AthenaCommon.Resilience import treatException        
from RecExConfig.RecFlags  import rec
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.DetFlags import DetFlags

jobproperties.CaloRecFlags.doCaloCluster=True
jobproperties.CaloRecFlags.doEmCluster=True
jobproperties.CaloRecFlags.clusterCellGetterName='HIJetRec.SubtractedCellGetter.SubtractedCellGetter'

rec.doEgamma=True
from egammaRec.egammaRecFlags import jobproperties
jobproperties.egammaRecFlags.Enabled=True
jobproperties.egammaRecFlags.cellContainerName='SubtractedCells'
jobproperties.egammaRecFlags.doEgammaCaloSeeded=True
jobproperties.egammaRecFlags.doEgammaForwardSeeded=False
jobproperties.egammaRecFlags.doTopoCaloSeeded=False

if DetFlags.haveRIO.Calo_on() :    
    try:
        from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
        CaloClusterSWCmbGetter()
    except Exception:    
        treatException("Problem with CaloSWCmbCluster. Switched off.")
        jobproperties.CaloRecFlags.doCaloCluster=False        
else:
        jobproperties.CaloRecFlags.doCaloCluster=False        
#
# functionality : LAr Calorimeter clustering
#
if DetFlags.haveRIO.LAr_on() :

    try:
        include( "LArClusterRec/LArCluster_jobOptions.py" )
        
        # introduce multisize possibility
        # include( "LArClusterRec/LArCluster_MultiSize_jobOptions.py" )
    except Exception:        
        treatException("Problem with LArCluster. Switched off.")
        jobproperties.CaloRecFlags.doEmCluster=False    

    # write digits of EM clusters
    if jobproperties.CaloRecFlags.doEMDigits() and globalflags.DataSource()=='data' and globalflags.InputFormat() == 'bytestream': 
        try:
            include ("LArClusterRec/LArDigits_fromEMCluster_jobptions.py")
        except Exception:
            treatException("Problem with LArDigitsFromEMClust. Switched off.")
            jobproperties.CaloRecFlags.doEMDigits=False
else:
    jobproperties.CaloRecFlags.doEmCluster=False    


#now run egamma
pdr.flag_domain('egamma')
if rec.doEgamma() : protectedInclude( "egammaRec/egammaRec_jobOptions.py" )
from AODFix.AODFix import AODFix_postEgammaRec
AODFix_postEgammaRec()
