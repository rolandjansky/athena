###############################################################
#
# TGCgeometryTester jobOptions fragment
# authors: Shlomit Tarem, Zvi Tarem, Natalia Panikashvili
#
#==============================================================

include.block("TGCgeometry/TGCgeometryTester_jobOptions.py")

if not 'TGCgeometryTester' in theApp.TopAlg:
    theApp.TopAlg += ["TGCgeometryTester"]

include("TGCgeometry/TGCgeometry_jobOptions.py")

