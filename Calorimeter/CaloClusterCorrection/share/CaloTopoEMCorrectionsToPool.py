#####################################################
# Example jobO to write Cluster Correction constants
# to a local POOL file.
####################################################


CALOCORR_POOLFILE = 'LFN:CaloTopoEMCorrections_13-01'


# Set up minimal environment, no event loop, no event selector
theApp.EvtSel = "NONE"
theApp.EvtMax=1

# Necessary libraries
theApp.Dlls += ["CaloRec","CaloClusterCorrection","CaloCondAthenaPoolPoolCnv"]

CaloTopoEMCorrKeys = ['ele633', 'ele420', 'gam633']

from CaloClusterCorrection.CaloTopoEMCorrections import CaloTopoEMCorrections
from CaloClusterCorrection.common import CALOCORR_EMTOPO

from CaloClusterCorrection.compat import makeFlags
from AthenaConfiguration.ComponentAccumulator import appendCAtoAthena
(corr_output_list, tag_list, ca) =\
                   CaloTopoEMCorrections.config_for_pool (makeFlags(),
                                                          CaloTopoEMCorrKeys,
                                                          CALOCORR_EMTOPO)
appendCAtoAthena (ca)

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

