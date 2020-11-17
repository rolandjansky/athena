# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Set up the job.
from AthExOnnxRuntime.AthExOnnxRuntimeConf import AthONNX__CxxApiAlgorithm
from AthOnnxruntimeService.AthOnnxruntimeServiceConf import AthONNX__ONNXRuntimeSvc

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += AthONNX__ONNXRuntimeSvc( OutputLevel = DEBUG )
algSequence += AthONNX__CxxApiAlgorithm("AthONNX")

# Get a	random no. between 0 to	10k for	test sample
from random import randint

AthONNX = algSequence.AthONNX
AthONNX.TestSample = randint(0, 9999)
# Run for 10 "events".
theApp.EvtMax = 10
