## @file TauTools/share/TauEDMdump_joboptions.py
## @purpose steering job option file for MyAlg residing in python/TauEDMdump.py
## @author Dugan O'Neil

## get a handle on the sequence of algorithms to be run
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


## import my algorithm and add it to the list of algorithms to be run
from TauTools.TauEDMdump import TauEDMDump
job += TauEDMDump(name='TauEDMDump',
		  doTree=True,
		  doStore=False,
		  dumpLevel=2)
# If you wish, exclude variables using a blackList. Wildcards at the end of 
# variable names are OK.
job.TauEDMDump.blackList = []
tauJetBlackList = ['Tau_author','Tau_cosPhi','Tau_cosTh','Tau_cotPhi','Tau_dataType'
		   ,'Tau_e','Tau_getAthenaBarCode','Tau_getVersion','Tau_hasCharge'
		   ,'Tau_hasPdgId','Tau_m2','Tau_nDetails','Tau_numberOfTracks'
		   ,'Tau_p','Tau_p2','Tau_pdgID','Tau_pt','Tau_px','Tau_py','Tau_pz'
		   ,'Tau_sinPhi','Tau_sinTh','Tau_cotTh'
		   ,'Tau_hasMergedToCommonDetails']
job.TauEDMDump.blackList.extend(tauJetBlackList)

# define histsvc {in/out}put streams
if not hasattr(svcMgr, 'Atlas.THistSvc'):
        svcMgr += CfgMgr.THistSvc()
hsvc = svcMgr.THistSvc
hsvc.Output += ["rec DATAFILE='TauEDMDump.root' TYP='ROOT' OPT='RECREATE'"]

