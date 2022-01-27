# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
# TrigDB modifier
# Usage: athenaHLT.py --postcommand 'include("TriggerJobOpts/dbmod_BFieldAutoConfig.py")' ...
#
# Use DCS currents to configure magnetic field (equivalent of BFieldFromDCS Modifier)
#
from GaudiPython.Bindings import iProperty
from AthenaCommon.Logging import logging
log = logging.getLogger('dbmod_BFieldAutoConfig.py')

log.info('Configuring job to read field maps from COOL/DCS')

# Add the DCS folder
f = '<db>COOLOFL_DCS/CONDBR2</db> /EXT/DCS/MAGNETS/SENSORDATA'
iProperty('IOVDbSvc').Folders += [f]
iProperty('CondInputLoader').Load += [['CondAttrListCollection','/EXT/DCS/MAGNETS/SENSORDATA']]
# Configure CondAlgs
iProperty('AtlasFieldCacheCondAlg').UseDCS = True
iProperty('AtlasFieldMapCondAlg').LoadMapOnStart = False
iProperty('AtlasFieldMapCondAlg').UseMapsFromCOOL = True
iProperty('HltEventLoopMgr').setMagFieldFromPtree = False
