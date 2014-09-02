from RecExConfig.RecFlags import rec as recFlags
from MuonTrackPerformance.MuonTrackPerformanceConf import MuonMakeTrackNtuple,MuonMakeSegmentNtuple
from MuonRecExample import MuonRecTools
MuonRecTools.loadAll()

CBNT_AthenaAware += MuonMakeTrackNtuple( doTruth = recFlags.doTruth() )
CBNT_AthenaAware += MuonMakeSegmentNtuple()
                                           
