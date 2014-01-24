# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

theApp.EvtMax = 10

InputFile = '/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data'

OutputFile = 'CAFtest.root'

from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
HLTMonFlags.doMonTier0 = False

from TrigHLTOfflineMon.HLTOfflineMonFlags import HLTOfflineMonFlags
HLTOfflineMonFlags.doOfflineMuon = False
HLTOfflineMonFlags.doEgamma = False
HLTOfflineMonFlags.doIDtrk = False


include("standalone_TrigHLTMonCommon.py")
