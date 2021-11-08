# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Testing job options for IS publication.
#
# If you want to inspect the IS content during or after execution,
# run `athenaHLT` in interactive mode (-i) and then launch `is_monitor`:
#
# > export TDAQ_IPC_INIT_REF=file:${PWD}/ipc_init.ref
# > is_monitor
#

from TrigExamples.TrigExamplesConf import TrigExISPublishing

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigExISPublishing(OutputLevel = DEBUG)
