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

#First do Calo-Reco
pdr.flag_domain('calo')
protectedInclude ("CaloRec/CaloRec_jobOptions.py")
AODFix_postCaloRec()

#then run ID reco:

pdr.flag_domain('id')
if DetFlags.detdescr.ID_on():
    protectedInclude( "InDetRecExample/InDetRec_jobOptions.py" )
    AODFix_postInDetRec()

# functionality : FTK reconstruction
if DetFlags.detdescr.FTK_on() :
    protectedInclude( "FTK_RecExample/FTKRec_jobOptions.py")
         
        
# functionality : Muon reconstruction
pdr.flag_domain('muon')
if DetFlags.detdescr.Muon_on() :
    try:
        from RecExConfig.RecConfFlags import recConfFlags
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        from MuonRecExample.MuonRecFlags import muonRecFlags
        muonRecFlags.applyResilience = athenaCommonFlags.AllowIgnoreConfigError()
    except Exception:
        treatException("Could not set muonRecFlags.applyResilience")        


# hack the merged jobo should test on rec.ScopingLevel=5 to run cosmic reco
#    4 the essential collision reco 3 high priority 2 medium priodity 1 nice to have



    try:    
        include ("MuonRecExample/MuonRec_jobOptions.py")
    except Exception:
        treatException("Problem with MuonRecExample/MuonRec_jobOptions.py. Switching off Moore and Muonboy")
        from MuonRecExample.MuonRecFlags import muonRecFlags
        muonRecFlags.doStandalone=False

else:
    from MuonRecExample.MuonRecFlags import muonRecFlags
    muonRecFlags.doStandalone=False
AODFix_postMuonRec()

    
pdr.flag_domain('forward')

if DetFlags.detdescr.Forward_on():
    try:
        protectedInclude ("ForwardRec/ForwardRec_jobOptions.py")

    except Exception:
        treatException("Problem including ForwardRec/ForwardRec_jobOptions.py !!")

AODFix_postForwardRec()

