## Pool persistency for evgen
from AthenaCommon.AppMgr import theApp
stream = theApp.getOutputStream( "StreamEVGEN" )
if stream is None:
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    stream = AthenaPoolOutputStream("StreamEVGEN")
stream.OutputFile = "evgen.pool.root"
if "OUTFILE" in dir():
    stream.OutputFile = OUTFILE
stream.ItemList += ["EventInfo#*", "McEventCollection#*"]
