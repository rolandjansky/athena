#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

# Set up the reading of a file.
FNAME = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/DAOD_PHYSVAL/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.DAOD_PHYSVAL.e5458_s3126_r9364_r9315_AthDerivation-21.2.1.0.root'
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Set up the algorithm making the xAOD::EventInfo object non-const.
from AthenaCommon import CfgMgr
theJob += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg( 'EventInfoNonConstCnvAlg',
                                                     OutputLevel = VERBOSE )

# Set up the algorithm that tries to now modify the object.
theJob += CfgMgr.xAODReader__EventInfoModifierAlg( 'EventInfoModifierAlg',
                                                   OutputLevel = VERBOSE )

# Tweak the job a bit more.
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
