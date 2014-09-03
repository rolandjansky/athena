from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'SystemRec_config' )

from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

from AthenaCommon.Resilience import treatException,protectedInclude

# use to flag domain
import PerfMonComps.DomainsRegistry as pdr

from AODFix.AODFix import *
AODFix_Init()


runningFatrasID=False #True woudl imply that ID reco runs before Calo Reco

try:
    from FastSimulationConfig.FastSimulationFlags import jobproperties
    if jobproperties.FastSimulation.doFatrasID():
        runningFatrasID=True
except:
    mlog.warning("Could not access 'jobproperties.FastSimulation.doFatrasID'. Assume regular reco job")
    pass


if runningFatrasID:
    #First do ID/Fatras reco:
    pdr.flag_domain('id')
    if DetFlags.detdescr.ID_on():
        try:
            include("FatrasExample/Fatras_jobOptions.py")
        except Exception:
            treatException("Problem with Fatras_jobOptions. Switched off.")

    #Then run Calo-Reco
    pdr.flag_domain('calo')
    protectedInclude ("CaloRec/CaloRec_jobOptions.py")
    
else: #Regular case, dont' run Fatras
    #First do Calo-Reco
    pdr.flag_domain('calo')
    protectedInclude ("CaloRec/CaloRec_jobOptions.py")
    AODFix_postCaloRec()

    #then run ID reco:

    pdr.flag_domain('id')
    if DetFlags.detdescr.ID_on():
        protectedInclude( "InDetRecExample/InDetRec_jobOptions.py" )
        AODFix_postInDetRec()

del runningFatrasID #Not needed any more
        
# functionality : Muon reconstruction
pdr.flag_domain('muon')
from MuonRecExample.MuonRecFlags import muonRecFlags
if DetFlags.detdescr.Muon_on() :
    try:
        from RecExConfig.RecConfFlags import recConfFlags
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        muonRecFlags.applyResilience = athenaCommonFlags.AllowIgnoreConfigError()
    except Exception:
        treatException("Could not set muonRecFlags.applyResilience")        


# hack the merged jobo should test on rec.ScopingLevel=5 to run cosmic reco
#    4 the essential collision reco 3 high priority 2 medium priodity 1 nice to have



    try:    
        # now merged
        #        if not rec.Commissioning:
        include ("MuonRecExample/MuonRec_jobOptions.py")
        #        elif not rec.readESD and not rec.readAOD:
        #            include("MuonCommRecExample/MuonCommRec_jobOptions.py")            
    except Exception:
        treatException("Problem with MuonRecExample/MuonRec_jobOptions.py. Switching off Moore and Muonboy")
        muonRecFlags.doMoore=False
        muonRecFlags.doMuonboy=False
        muonRecFlags.doStandalone=False

else:
    muonRecFlags.doMoore=False
    muonRecFlags.doMuonboy=False
    muonRecFlags.doStandalone=False
AODFix_postMuonRec()

    
pdr.flag_domain('forward')

if DetFlags.detdescr.Forward_on():
    try:
        protectedInclude ("ForwardRec/ForwardRec_jobOptions.py")

    except Exception:
        treatException("Problem including ForwardRec/ForwardRec_jobOptions.py !!")

AODFix_postForwardRec()

