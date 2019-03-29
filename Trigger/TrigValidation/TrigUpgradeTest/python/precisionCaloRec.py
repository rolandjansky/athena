#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.Resilience import treatException,protectedInclude
import PerfMonComps.DomainsRegistry as pdr
from AODFix.AODFix import *
AODFix_Init()
from CaloRec.CaloRecFlags import jobproperties
from egammaRec.egammaRecFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from egammaRec import egammaKeys
from egammaRec.egammaKeys import egammaKeysDict
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,ERROR
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
jobproperties.egammaRecFlags.print_JobProperties("full")

def precisionCaloRecoSequence(RoIs):
	egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
        	name = 'egammaTopoClusterCopier' ,
        	#InputTopoCollection=jobproperties.egammaRecFlags.inputTopoClusterCollection(),
        	InputTopoCollection= "caloclusters",
		OutputTopoCollection=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
        	#OutputTopoCollection="ClustersName",
		OutputTopoCollectionShallow="tmp_"+jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
        	doAdd = False
 		)

	
	from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
        (precisionRecoSequence, caloclusters) = HLTFSTopoRecoSequence(RoIs)

	algo = egammaTopoClusterCopier()
	algo.InputTopoCollection = caloclusters
	algo.OutputLevel= VERBOSE
	precisionRecoSequence += algo
	sequenceOut = algo.OutputTopoCollection 

	return (precisionRecoSequence, sequenceOut)

