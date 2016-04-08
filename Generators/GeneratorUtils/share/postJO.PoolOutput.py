## Pool persistency for evgen
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
stream = AthenaPoolOutputStream("StreamEVGEN")
stream.OutputFile = "evgen.pool.root"
if "OUTFILE" in dir():
    stream.OutputFile = OUTFILE
stream.ItemList += ["EventInfo#*", "McEventCollection#*"]
