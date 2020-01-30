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
    printfunc ("Forcing use of new Event Reseeding for AtRndmGenSvc!")
    ServiceMgr.AtRndmGenSvc.UseOldBrokenSeeding = False
    ServiceMgr.AtRndmGenSvc.EventReseeding = True
    printfunc (ServiceMgr.AtRndmGenSvc)

if hasattr(ServiceMgr, "AtRanluxGenSvc"):
    printfunc ("Forcing use of new Event Reseeding for AtRanluxGenSvc!")
    ServiceMgr.AtRanluxGenSvc.UseOldBrokenSeeding = False
    ServiceMgr.AtRanluxGenSvc.EventReseeding = True
    printfunc (ServiceMgr.AtRanluxGenSvc)

