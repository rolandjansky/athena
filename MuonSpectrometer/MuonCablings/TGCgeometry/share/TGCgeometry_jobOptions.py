###############################################################
#
# TGCgeometry jobOptions fragment
# authors: Shlomit Tarem, Zvi Tarem, Natalia Panikashvili
#
#==============================================================

include.block("TGCgeometry/TGCgeometry_jobOptions.py")

# if not 'TGCgeometry' in theApp.Dlls:
#     theApp.Dlls += ["TGCgeometry"]
#     theApp.ExtSvc += ["TGCgeometrySvc"]
#     
# TGCgeometrySvc = Service("TGCgeometrySvc")
# TGCgeometrySvc.FileName = "TGCgeometry.lut"
# if DetDescrVersion == "ATLAS-CSC-01-02-00":
#     TGCgeometrySvc.FileName = "TGCgeometry."+DetDescrVersion+".lut"
from TGCgeometry.TGCgeometryConfig import TGCgeometrySvc
