import InDetPhysValMonitoring.InDetPhysValDecoration
decoration = InDetPhysValMonitoring.InDetPhysValDecoration.addDecoratorIfNeeded()

# add ID physics validation monitoring tool

from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
import InDetPhysValMonitoring.InDetPhysValMonitoringTool as InDetPhysValMonitoringTool

mons=[ (True                                             , InDetPhysValMonitoringTool.getInDetPhysValMonitoringTool),
       (InDetPhysValFlags.doValidateLargeD0Tracks()      , InDetPhysValMonitoringTool.getInDetLargeD0PhysValMonitoringTool),
       (InDetPhysValFlags.doValidateLooseTracks()        , InDetPhysValMonitoringTool.getInDetPhysValMonitoringToolLoose),
       (InDetPhysValFlags.doValidateTightPrimaryTracks() , InDetPhysValMonitoringTool.getInDetPhysValMonitoringToolTightPrimary),
       (InDetPhysValFlags.doValidateDBMTracks()          , InDetPhysValMonitoringTool.getInDetPhysValMonitoringToolDBM),
       (InDetPhysValFlags.doValidateGSFTracks()          , InDetPhysValMonitoringTool.getInDetPhysValMonitoringToolGSF)
     ]

for enabled, creator in mons :
    if enabled :
        monMan.AthenaMonTools += [ creator() ]

from InDetPhysValMonitoring.InDetPhysValMonitoringTool import getInDetPhysValMonitoringTool
from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
for col in InDetPhysValFlags.validateExtraTrackCollections() :
    prefix=extractCollectionPrefix(col)
    monMan.AthenaMonTools += [ getInDetPhysValMonitoringTool(name                       = 'InDetPhysValMonitoringTool'+prefix,
                                                             SubFolder                  = prefix+'Tracks/',
                                                             TrackParticleContainerName = prefix+'TrackParticles') ]
