##=============================================================================
## Simplest job options. For test & development. ---  David Cote, August 2009
## Execute this like e.g.:   athena.py MinimalTopOptions.py
##=============================================================================
from AthenaCommon.AlgSequence import AlgSequence
from PrimaryDPDMaker.FilterSkeleton import FilterSkeleton
topSequence=AlgSequence()
topSequence+= FilterSkeleton('myFilterSkeleton')

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
pool=MSMgr.NewPoolStream("TestPool","myOuput.pool.root")
pool.AddItem("MissingET#MET_RefFinal")
pool.AddMetaDataItem("LumiBlockCollection#*")
pool.AddRequireAlgs('myFilterSkeleton')

