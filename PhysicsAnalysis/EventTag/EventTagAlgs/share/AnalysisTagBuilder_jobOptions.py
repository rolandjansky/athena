from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

# Combined Performance Groups - Tag Fragments
include ( "JetMissingEtTagTools/JetMissingEtIdentificationTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import AnalysisTagBuilder

AnalysisTagBuilder = AnalysisTagBuilder(
  AttributeList  = EventTagGlobal.AttributeList,
  JetMissingEtIdentificationTagTool = JetMissingEtIdentificationTagTool,
  NumJetMissinEt = 1,
  CheckAttribute = True)

topSequence += AnalysisTagBuilder
