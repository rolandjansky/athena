# jobOptions fragment to setup writing of LAr affected region

from LArConditionsCommon import LArHVDB

from LArAffectedRegion.LArAffectedRegionConf import LArAffectedRegionAlg
theLArAffectedRegionAlg = LArAffectedRegionAlg()

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
    theLArAffectedRegionAlg.doHV = True
else:
    theLArAffectedRegionAlg.doHV = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence+=theLArAffectedRegionAlg
