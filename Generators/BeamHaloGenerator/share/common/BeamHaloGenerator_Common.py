
from BeamHaloGenerator.BeamHaloGeneratorConf import BeamHaloGeneratorAlg
genSeq +=BeamHaloGeneratorAlg()
evgenConfig.generators += ["BeamHaloGenerator"]
genSeq.BeamHaloGeneratorAlg.doMonitoringPlots = True
genSeq.BeamHaloGeneratorAlg.inputFile="beamhalogen.events"

###############################################################
## Beamhalo default. Redefine in JO if used for cavern background
###############################################################
genSeq.BeamHaloGeneratorAlg.inputType="FLUKA-RB"
genSeq.BeamHaloGeneratorAlg.interfacePlane = 22600.0 
