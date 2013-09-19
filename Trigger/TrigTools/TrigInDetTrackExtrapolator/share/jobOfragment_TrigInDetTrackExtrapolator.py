
from TrigInDetTrackExtrapolator.TrigInDetTrackExtrapolatorConf import TrigInDetTrackExtrapolator

TrigExtrap = TrigInDetTrackExtrapolator(name="ConfiguredTrigInDetTrackExtrapolator",
                                        UseAthenaFieldService=True,
                                        MinPt=1000.0)

ToolSvc+=TrigExtrap






