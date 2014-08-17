#
# A short example of how to retrieve the calorimeter detector description
# from python.  Run this with `athena.py -i'.
#

include ("RecExCond/RecExCommon_DetFlags.py")

DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()

include ("RecExCond/AllDet_detDescr.py")

theApp.initialize()
import ROOT
dd=ROOT.CaloDetDescrManager.instance()
