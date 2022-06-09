# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# make the logger known to flake8:
logRecExCommon_topOptions = logRecExCommon_topOptions  # noqa: F821 (from RecExCommon_topOptions.py)

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from RecExConfig.RecFlags import rec

from AthenaCommon.Resilience import treatException,protectedInclude
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper

# use to flag domain
import PerfMonComps.DomainsRegistry as pdr

from AODFix import AODFix
AODFix.AODFix_Init()

#First do Calo-Reco
pdr.flag_domain('calo')
if DetFlags.detdescr.Calo_on() and rec.doESD():
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    CAtoGlobalWrapper(CaloRecoCfg, ConfigFlags)

    #Move L1Calo Trigger tower decoration here for simplicity:
    if globalflags.DataSource()=='data' and rec.doESD() and rec.doCalo() and rec.doTrigger():
        include("TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py")


AODFix.AODFix_postCaloRec()

#make the egammaTopoClusters containers, used for seeding
if jobproperties.CaloRecFlags.doCaloTopoCluster():
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    CAtoGlobalWrapper(egammaTopoClusterCopierCfg,ConfigFlags,**HIDict)  # noqa: F821 (HIDict from RecExCommon_topOptions.py)

#then run ID reco:

pdr.flag_domain('id')
if DetFlags.detdescr.ID_on():
    if jobproperties.InDetJobProperties.useNewConfig():
        logRecExCommon_topOptions.info('Wrapping InDet new configuration')
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from InDetConfig.TrackRecoConfig import InDetTrackRecoCfg
        CAtoGlobalWrapper(InDetTrackRecoCfg,ConfigFlags)
    else:
        protectedInclude( "InDetRecExample/InDetRec_jobOptions.py" )
        AODFix.AODFix_postInDetRec()

# functionality : Muon reconstruction
pdr.flag_domain('muon')
if DetFlags.detdescr.Muon_on() :
    try:
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from MuonRecExample.MuonRecFlags import muonRecFlags
        muonRecFlags.applyResilience = athenaCommonFlags.AllowIgnoreConfigError()
    except Exception:
        treatException("Could not set muonRecFlags.applyResilience")        


# hack the merged jobo should test on rec.ScopingLevel=5 to run cosmic reco
#    4 the essential collision reco 3 high priority 2 medium priodity 1 nice to have

    if  muonRecFlags.useNewConfig():
        logRecExCommon_topOptions.info('Wrapping Muon new configuration')
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from MuonConfig.MuonReconstructionConfig import MuonReconstructionCfg
        # TODO Keep here for the moment, since we still have debugging to do.
        # from AthenaCommon.Logging import logging
        # log = logging.getLogger( "Py:conf2toConfigurable" )
        # log.setLevel(DEBUG)
        CAtoGlobalWrapper(MuonReconstructionCfg,ConfigFlags)
    else:
        try:    
            include ("MuonRecExample/MuonRec_jobOptions.py")
        except Exception:
            treatException("Problem with MuonRecExample/MuonRec_jobOptions.py. Switching off Moore and Muonboy")
            from MuonRecExample.MuonRecFlags import muonRecFlags
            muonRecFlags.doStandalone=False

else:
    from MuonRecExample.MuonRecFlags import muonRecFlags
    muonRecFlags.doStandalone=False
AODFix.AODFix_postMuonRec()

    
pdr.flag_domain('forward')

if DetFlags.detdescr.Forward_on():
    try:
        protectedInclude ("ForwardRec/ForwardRec_jobOptions.py")

    except Exception:
        treatException("Problem including ForwardRec/ForwardRec_jobOptions.py !!")

AODFix.AODFix_postForwardRec()
