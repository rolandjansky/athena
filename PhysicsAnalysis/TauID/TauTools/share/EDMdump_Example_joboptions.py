## @file TauTools/share/TauEDMdump_joboptions.py
## @purpose steering job option file for MyAlg residing in python/TauEDMdump.py
## @author Dugan O'Neil

## get a handle on the sequence of algorithms to be run
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


## import my algorithm and add it to the list of algorithms to be run
from TauTools.EDMdump_Example import EDMdump_Example
job += EDMdump_Example('EDMdump_Example')

# define histsvc {in/out}put streams
if not hasattr(svcMgr, 'Atlas.THistSvc'):
        svcMgr += CfgMgr.THistSvc()
hsvc = svcMgr.THistSvc
hsvc.Output += ["rec DATAFILE='TauEDMDump.root' TYP='ROOT' OPT='RECREATE'"]

