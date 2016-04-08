## Generic JO header for use in all evgen job options

## Pull in *really* generic stuff shared with StdAnalysisSetup.py
include("GeneratorUtils/StdJOSetup.py")

## Provide a special evgen logger
evgenLog = logging.getLogger("Evgen")

## Needed to set IS_SIMULATION metadata
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

## Provide a bit of compatibility with Generate_trf JOs via dummy
## evgenConfig and StreamEVGEN objects
if not "evgenConfig" in dir():
     if not "EvgenConfig" in dir():
          class EvgenConfig(object):
               pass
     evgenConfig = EvgenConfig()
#
if not "StreamEVGEN" in dir():
     from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
     streamEVGEN = AthenaPoolOutputStream("StreamEVGEN")
     StreamEVGEN = streamEVGEN  # compatibility alias, breaking convention!
