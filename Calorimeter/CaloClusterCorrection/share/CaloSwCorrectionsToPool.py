#####################################################
# Example jobO to write Cluster Correction constants
# to a local POOL file.
####################################################


CALOCORR_POOLFILE = 'LFN:CaloSwCorrections_13-03'


# Set up minimal environment, no event loop, no event selector
theApp.EvtSel = "NONE"
theApp.EvtMax=1

# Necessary libraries
theApp.Dlls += ["CaloRec","CaloClusterCorrection","CaloCondAthenaPoolPoolCnv"]

CaloSwCorrKeys = ['ele55', 'ele35', 'ele37',
                  'gam55', 'gam35', 'gam37']

from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections
from CaloClusterCorrection.common import CALOCORR_SW

(corr_output_list, tag_list) =\
                   CaloSwCorrections.config_for_pool (CaloSwCorrKeys,
                                                      CALOCORR_SW)

# include the basic setup for the conditions output stream
pool_file = CALOCORR_POOLFILE
if pool_file.startswith ('LFN:'):
    pool_file = pool_file[4:] + '.pool.root'
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg = OutputConditionsAlg (outputFile = pool_file,
                                           ObjectList = corr_output_list,
                                           WriteIOV = False)

#OutputConditionsAlg.OutputLevel=DEBUG
#ToolSvc.ConditionsAlgStream.OutputLevel=DEBUG


from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.DetectorStore.Dump = True

try:
    ll = theApp.OutStream
    ll.remove ('AthenaOutputStream/Stream1')
    theApp.OutStream = ll
except ValueError:
    pass

