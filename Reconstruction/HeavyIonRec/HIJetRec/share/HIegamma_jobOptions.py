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
jobproperties.CaloRecFlags.doCaloTopoCluster = True
jobproperties.CaloRecFlags.doCaloEMTopoCluster = True
jobproperties.CaloRecFlags.clusterCellGetterName='HIJetRec.SubtractedCellGetter.SubtractedCellGetter'

rec.doEgamma=True
from egammaRec.egammaRecFlags import jobproperties
jobproperties.egammaRecFlags.Enabled=True
jobproperties.egammaRecFlags.cellContainerName='SubtractedCells'
jobproperties.egammaRecFlags.doEgammaCaloSeeded=True
jobproperties.egammaRecFlags.doEgammaForwardSeeded=False

if DetFlags.haveRIO.Calo_on() :
    #combined clusters
    if jobproperties.CaloRecFlags.doCaloCluster() :
        try:
            from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
            CaloClusterSWCmbGetter()
        except Exception:
            treatException("Problem with CaloSWCmbCluster. Switched off.")
            jobproperties.CaloRecFlags.doCaloCluster=False
    #EM clusters
    if jobproperties.CaloRecFlags.doEmCluster() :
        try: include( "LArClusterRec/LArCluster_jobOptions.py" )
        except Exception:
            treatException("Problem with LArCluster. Switched off.")
            jobproperties.CaloRecFlags.doEmCluster=False
        # write digits of EM clusters
        if jobproperties.CaloRecFlags.doEMDigits() and globalflags.DataSource()=='data' and globalflags.InputFormat() == 'bytestream':
            try: include ("LArClusterRec/LArDigits_fromEMCluster_jobptions.py")
            except Exception:
                treatException("Problem with LArDigitsFromEMClust. Switched off.")
                jobproperties.CaloRecFlags.doEMDigits=False

    #Topoclusters
    if jobproperties.CaloRecFlags.doCaloTopoCluster() :
        try: include( "CaloRec/CaloTopoCluster_jobOptions.py" )
        except Exception:
            treatException("Problem with CaloTopoCluster. Switched off.")
            jobproperties.CaloRecFlags.doCaloTopoCluster=False

    #EM Topoclusters
    if jobproperties.CaloRecFlags.doCaloEMTopoCluster() :
        try: include( "CaloRec/EMTopoCluster_jobOptions.py" )
        except Exception:
            treatException("Problem with EMTopoCluster. Switched off")
            jobproperties.CaloRecFlags.doCaloTopoCluster=False



#Run egamma
pdr.flag_domain('egamma')
if rec.doEgamma() : protectedInclude( "egammaRec/egammaRec_jobOptions.py" )
from AODFix.AODFix import AODFix_postEgammaRec
AODFix_postEgammaRec()

#Fix isolation
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if hasattr(topSequence,"EDtpIsoCentralAlg") :
    EDtpIsoCentralAlg=getattr(topSequence,"EDtpIsoCentralAlg")
    topSequence.remove(EDtpIsoCentralAlg)
    topSequence+=EDtpIsoCentralAlg

if hasattr(topSequence,"EDtpIsoForwardAlg") :
    EDtpIsoForwardAlg=getattr(topSequence,"EDtpIsoForwardAlg")
    topSequence.remove(EDtpIsoForwardAlg)
    topSequence+=EDtpIsoForwardAlg

if hasattr(topSequence,"IsolationBuilder") :
    iso=getattr(topSequence,"IsolationBuilder")
    topSequence.remove(iso)
    #EgIsoTypes doesn't exist in IsolationBulder any more, replace with ElIsoTypes and PhIsoTypes
    #iso.EgIsoTypes=iso.MuIsoTypes 
    iso.ElIsoTypes=iso.MuIsoTypes
    iso.PhIsoTypes=iso.MuIsoTypes
    iso.CellCollectionName='SubtractedCells'
    topSequence+=iso

