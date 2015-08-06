# jobOptions for BeamCondSvc - just define conditions data access

include.block("InDetBeamSpotService/BeamCondSvc.py")
from IOVDbSvc.CondDB import conddb
from RecExConfig.RecFlags import rec
# If express processing, point beam spot to online folder results
if(rec.doExpressProcessing()):
    conddb.addFolder('INDET_ONL','/Indet/Onl/Beampos <key>/Indet/Beampos</key>')
else:
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos","/Indet/Beampos")
