# jobOptions for BeamCondSvc - just define conditions data access

include.block("InDetBeamSpotService/BeamCondSvc.py")
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos","/Indet/Beampos")
