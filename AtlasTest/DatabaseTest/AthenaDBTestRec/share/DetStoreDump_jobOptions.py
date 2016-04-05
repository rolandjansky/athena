# enable DetStoreDump algorithm to do SG dump of detector store
# include this at end of joboptions so it is last in list
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()
from AthenaDBTestRec.AthenaDBTestRecConf import DetStoreDump
# set mode to zero to only dump after initialize, not after first event
theDetStoreDump=DetStoreDump(Mode=1)
topSequence+=theDetStoreDump
