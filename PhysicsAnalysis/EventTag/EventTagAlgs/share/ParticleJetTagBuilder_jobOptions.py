from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

#from EventShapeTools.EventDensityConfig import configEventShapeCopierAlg
#topSequence += configEventShapeCopierAlg("LCTopo")

include ( "JetMissingEtTagTools/JetTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import ParticleJetTagBuilder
ParticleJetTagBuilder      = ParticleJetTagBuilder(
     name                  = "ParticleJetTagBuilder",
     JetMissingEtTagTool   = JetMissingEtTagTool,
     AttributeList         = EventTagGlobal.AttributeList,
     MaxNumber             = 10,
     CheckAttribute        = True)
topSequence += ParticleJetTagBuilder
