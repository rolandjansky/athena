include( "EventTagAlgs/EventTagFlags.py" )

from RecExConfig.RecFlags import rec
if not rec.doTrigger():
    EventTagFlags.set_TriggerOff()
    print "EventTag_jobOptions.py :: WARNING: Cannot configure GlobaltriggerTagTool if rec.doTrigger()=false"


# GlobalEventTagBuilder must run always
include( "EventTagAlgs/GlobalEventTagBuilder_jobOptions.py" )
include( "EventTagUtils/RegistrationStreamTrig_jobOptions.py")
import traceback
if EventTagFlags.doElectron():
    try:
        include( "EventTagAlgs/ElectronTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load ElectronTagBuilder_jobOptions.py"
if EventTagFlags.doPhoton():
    try:
        include( "EventTagAlgs/PhotonTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load PhotonTagBuilder_jobOptions.py"
if EventTagFlags.doMuon():
    try:
        include( "EventTagAlgs/MuonTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load MuonTagBuilder_jobOptions.py"
if EventTagFlags.doTauJet():
    try:
        include( "EventTagAlgs/TauJetTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load TauJetTagBuilder_jobOptions.py"

if EventTagFlags.doParticleJet():
    try:
        include( "EventTagAlgs/ParticleJetTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load ParticleJetTagBuilder_jobOptions.py"

if EventTagFlags.doMissingET():
    try:
        include( "EventTagAlgs/MissingETTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load MissingETTagBuilder_jobOptions.py"

if EventTagFlags.doTrigger():
    try:
        include( "EventTagAlgs/GlobalTriggerTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load GlobalTriggerTagBuilder_jobOptions.py"

if EventTagFlags.doRawSummary():
    try:
        include( "EventTagAlgs/RawInfoSummaryTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load RawInfoSummaryTagBuilder_jobOptions.py"


if EventTagFlags.doAnalysis():
    try:
        include( "EventTagAlgs/AnalysisTagBuilder_jobOptions.py" )
    except Exception:
        print traceback.format_exc() 
        print "ERROR could not load AnalysisTagBuilder_jobOptions.py"

# Define infile metadata list
from EventTagAlgs.EventTagGlobal import EventTagGlobal
tagmetadatalist = [ "TagAthenaAttributeList#"+EventTagGlobal.AttributeList ] 
tagmetadatalist += ["CollectionMetadataContainer#GlobalTriggerTagBuilder.GlobalTriggerTagTool"]
tagmetadatalist += ["CollectionMetadataContainer#GlobalEventTagBuilder.GlobalEventTagTool"]
tagmetadatalist += ["CollectionMetadataContainer#RegistrationStreamTrig"]
#tagmetadatalist += ["CollectionMetadataContainer#DPDTagTool"]
