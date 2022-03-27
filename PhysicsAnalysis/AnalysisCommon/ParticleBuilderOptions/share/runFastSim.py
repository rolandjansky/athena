##############################
# run Atlfast

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# load relevant libraries
from PartPropSvc.PartPropSvcConf import PartPropSvc

from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
#theApp.ExtSvc += [ "AtRndmGenSvc" ]

# set up the Atlfast sequence
include( "AtlfastAlgs/Atlfast_ConfigAlgs_NoFastShower.py" )
