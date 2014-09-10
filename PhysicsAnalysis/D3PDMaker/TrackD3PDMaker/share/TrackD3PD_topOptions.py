# Input and output files.
if not globals().get('poolFileInput'):
    poolFileInput = "AOD.pool.root"

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'trackD3PD.root'

theApp.EvtMax = 10

# Turn this on to enable the detector description.
# Without this, some event information that depends on the geometry
# will not be available.  On the other hand, Athena will initialize
# much faster.
if not globals().has_key('use_dd'):
    use_dd = False

if use_dd:
    from AthenaCommon.DetFlags      import DetFlags
    DetFlags.detdescr.all_setOn()
    import AtlasGeoModel.GeoModelInit
    svcMgr.GeoModelSvc.AutomaticGeometryVersion = True



#####################################################
# Set up reading the input file.
#

if type(poolFileInput) == type(""):
    poolFileInput = [poolFileInput]

# Disable inputFilePeeker cache.
# The cache just goes by the file name.
# So if, for example, you're reading the input file via a symlink and you
# change where the symlink points, the old cached info will still be used.
# This can be terribly confusing.
# So disable the cache for now.
import PyUtils.AthFile as af
af.server.flush_cache()
try:
    af.server.disable_pers_cache()
except AttributeError: # backward compat...
    pass

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = poolFileInput
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(poolFileInput)
from RecExConfig.InputFilePeeker import inputFileSummary


# Backwards compatibility for taus.
if ('TauRecExtraDetailsContainer' in
    inputFileSummary['eventdata_itemsDic'].get
      ('Analysis::TauDetailsContainer',[])):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from TauTools.TauToolsConf import TauCommonDetailsMerger
    topSequence += TauCommonDetailsMerger("TauCommonDetailsMerger")


#####################################################
# Set up the D3PD maker.
#

from TrackD3PDMaker.TrackD3PD import TrackD3PD
alg = TrackD3PD ( tupleFileOutput )
