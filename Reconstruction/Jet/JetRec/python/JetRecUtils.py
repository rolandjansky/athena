# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

def retrieveAODList():
    from JetRec.JetRecFlags import jetFlags
    from RecExConfig.RecFlags import rec

    if rec.doESD():
        return jetFlags.jetAODList()
    # then we are merging or doing a AOD ?
    # We can not simply copy what we have from input since some
    # jobs starts from empty files. See ATEAM-191.
    # We hard code the list here while waiting for a more robust solution
    l = [
        # event shape objects
    'xAOD::EventShape#Kt4EMPFlowEventShape', 'xAOD::EventShape#Kt4EMTopoEventShape', 'xAOD::EventShape#Kt4LCTopoEventShape', 'xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape', 'xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape', 'xAOD::EventShape#ParticleFlowIsoCentralEventShape', 'xAOD::EventShape#ParticleFlowIsoForwardEventShape', 'xAOD::EventShape#TopoClusterIsoCentralEventShape', 'xAOD::EventShape#TopoClusterIsoForwardEventShape', 'xAOD::EventShapeAuxInfo#Kt4EMPFlowEventShapeAux.', 'xAOD::EventShapeAuxInfo#Kt4EMTopoEventShapeAux.', 'xAOD::EventShapeAuxInfo#Kt4LCTopoEventShapeAux.', 'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux.', 'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux.', 'xAOD::EventShapeAuxInfo#ParticleFlowIsoCentralEventShapeAux.', 'xAOD::EventShapeAuxInfo#ParticleFlowIsoForwardEventShapeAux.', 'xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux.', 'xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux.',

     'xAOD::JetAuxContainer#AntiKt10LCTopoJetsAux.',  'xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.', 'xAOD::JetAuxContainer#AntiKt3PV0TrackJetsAux.', 'xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.', 'xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.', 'xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.', 'xAOD::JetAuxContainer#AntiKt4PV0TrackJetsAux.', 'xAOD::JetAuxContainer#CamKt12LCTopoJetsAux.',

     'xAOD::JetContainer#AntiKt10LCTopoJets',  'xAOD::JetContainer#AntiKt2PV0TrackJets', 'xAOD::JetContainer#AntiKt3PV0TrackJets', 'xAOD::JetContainer#AntiKt4EMPFlowJets', 'xAOD::JetContainer#AntiKt4EMTopoJets', 'xAOD::JetContainer#AntiKt4LCTopoJets', 'xAOD::JetContainer#AntiKt4PV0TrackJets',  'xAOD::JetContainer#CamKt12LCTopoJets', 
     ]

    if rec.doTruth():
        l += [
            'xAOD::JetAuxContainer#AntiKt10TruthJetsAux.', 'xAOD::JetAuxContainer#AntiKt10TruthWZJetsAux.', 'xAOD::JetAuxContainer#AntiKt4TruthJetsAux.', 'xAOD::JetAuxContainer#AntiKt4TruthWZJetsAux.', 'xAOD::JetAuxContainer#CamKt12TruthJetsAux.', 'xAOD::JetAuxContainer#CamKt12TruthWZJetsAux.',
            'xAOD::JetContainer#AntiKt10TruthJets', 'xAOD::JetContainer#AntiKt10TruthWZJets', 'xAOD::JetContainer#AntiKt4TruthJets', 'xAOD::JetContainer#AntiKt4TruthWZJets', 'xAOD::JetContainer#CamKt12TruthJets', 'xAOD::JetContainer#CamKt12TruthWZJets'            
            ]

    return l 


def buildJetAlgName(finder, mainParam):
    return finder + str(int(mainParam*10))

def buildJetContName(finder, mainParam, input):
    return buildJetAlgName(finder, mainParam) +input+"Jets" # could be more elaborated...

def interpretJetName(jetcollName,  finder = None,input=None, mainParam=None):
    # first step : guess the finder, input , mainParam, if needed
    if finder is None:
        for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']:
            if jetcollName.startswith(a):
                finder = a
                break
        if finder is None:
            print "Error could not guess jet finder type in ",jetcollName
            return 

    if input is None:
        for i in ['LCTopo','Tower','EMTopo', "Truth", "ZTrack", 'PV0Track']:
            if i in jetcollName:
                input = i
                if i== "Tower":
                    if 'Ghost' in jetcollName:
                        input = 'Tower'
                    else:
                        input = "TopoTower"
                break
        if input is None:
            print "Error could not guess input type in ",jetcollName            
            return
        
    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            print "Error could not guess main parameter in ",jetcollName
            return

    return finder, mainParam, input
