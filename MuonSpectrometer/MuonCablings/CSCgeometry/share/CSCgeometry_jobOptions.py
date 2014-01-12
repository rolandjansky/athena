###############################################################
#
# CSCgeometry jobOptions fragment
# authors: E. Cheu (copied from TGCgeometry service)
#
#==============================================================

include.block("CSCgeometry/CSCgeometry_jobOptions.py")

from AthenaCommon.AppMgr import ServiceMgr

from CSCgeometry.CSCgeometryConf import CSCgeometrySvc

ServiceMgr += CSCgeometrySvc()
CSCgeometrySvc = CSCgeometrySvc()

###CSCgeometrySvc.WriteLUTFile = True
###CSCgeometrySvc.FileName = "MyCSCgeometry.lut"
