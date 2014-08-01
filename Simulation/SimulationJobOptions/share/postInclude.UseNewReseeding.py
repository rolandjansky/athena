#######################################################
## postInclude.UseNewReseeding.py
##
## John Chapman
##
## Force use of the new event reseeding.
##
#######################################################
from AthenaCommon.AppMgr import ServiceMgr

if hasattr(ServiceMgr, "AtRndmGenSvc"):
    print "Forcing use of new Event Reseeding for AtRndmGenSvc!"
    ServiceMgr.AtRndmGenSvc.UseOldBrokenSeeding = False
    ServiceMgr.AtRndmGenSvc.EventReseeding = True
    print ServiceMgr.AtRndmGenSvc

if hasattr(ServiceMgr, "AtRanluxGenSvc"):
    print "Forcing use of new Event Reseeding for AtRanluxGenSvc!"
    ServiceMgr.AtRanluxGenSvc.UseOldBrokenSeeding = False
    ServiceMgr.AtRanluxGenSvc.EventReseeding = True
    print ServiceMgr.AtRanluxGenSvc

