
# Example
#Reco_tf.py --steering 'doRAWtoALL' --ignoreErrors 'True'  --inputBSFile /tmp/amorley/data16_13TeV.00310809.calibration_BeamSpot.merge.RAW/data16_13TeV.00310809.calibration_BeamSpot.merge.RAW._lb0743._SFO-ALL._0001.1 --outputAODFile /tmp/amorley/AOD.pool.root --conditionsTag all:CONDBR2-ES1PA-2016-03 --AMITag 'c1042' --autoConfiguration='everything' --maxEvents '-1' --preInclude 'all:InDetBeamSpotExample/preIncludeRecoForBeamspot.py' --postExec  'all:from IOVDbSvc.CondDB import conddb; conddb.addOverride("/Indet/AlignL1/ID" ,"IndetAlignL1ID-RUN2-BLK-UPD4-02"); conddb.addOverride("/Indet/AlignL2/PIX" ,"IndetAlignL2PIX-RUN2-BLK-UPD4-02"); conddb.addOverride("/Indet/AlignL2/SCT" ,"IndetAlignL2SCT-RUN2-BLK-UPD4-02"); conddb.addOverride("/Indet/AlignL3"     ,"IndetAlignL3-RUN2-BLK-UPD4-02"); conddb.addOverride("/Indet/IBLDist"     ,"InDetIBLDist-RUN2-BLK-UPD4-01"); conddb.addOverride("/TRT/AlignL1/TRT"   ,"TRTAlignL1-RUN2-BLK-UPD4-02"); conddb.addOverride("/TRT/AlignL2"       ,"TRTAlignL2-RUN2-BLK-UPD4-02");' --preExec  'all:from AtlasGeoModel.InDetGMJobProperties import GeometryFlags; GeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True); from RecExConfig.RecFlags import rec;rec.UserAlgs.set_Value_and_Lock("InDetBeamSpotFinder/BeamspotRefitVertex.py")' --postInclude 'all:BeamSpotFinder/postInclude.addSCTonlyToAODFile.py' --geometryVersion all:ATLAS-R2-2015-04-00-00 --maxEvents 10

from InDetRecExample.InDetKeys import InDetKeys
containerName= "SCTonlyVertex"
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import RefitTracksAndVertex
thisRefitTracksAndVertex = RefitTracksAndVertex(name               = 'RefitTracksAndVertex',
                                               TrackFitter         = ToolSvc.InDetTrackFitter,
                                               VertexFitterTool    = InDetVxFitterTool,
                                               VertexListInput     = InDetKeys.xAODVertexContainer(),
                                               OutputVertexContainer = containerName)
#thisRefitTracksAndVertex.SelEtaMin = -0.8
#thisRefitTracksAndVertex.SelEtaMax =  0.8
#thisRefitTracksAndVertex.OutputLevel = 1

from RecExConfig.RecFlags import rec
topSequence += thisRefitTracksAndVertex

