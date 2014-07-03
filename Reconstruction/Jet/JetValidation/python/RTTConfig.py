# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


containerToRebuild = ["AntiKt4LCTopoJetsTest", "AntiKt10LCTopoJetsTest",]
def scheduleRTTJetTests():
                        
    global containerToRebuild
    
    from JetRec.JetRecFlags import jetFlags
    from RecExConfig.RecFlags import rec
    jetFlags.useTruth = rec.doTruth()
    
    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecConf import JetAlgorithm
    from JetRec.JetRecUtils import interpretJetName

    from JetRec.JetRecStandard import calib_topo_ungroomed_modifiers, topo_ungroomed_modifiers

    calibarg = 'calib' if jetFlags.applyCalibrationName()!="none" else None

    #calibarg = 'calib' if jetFlags.applyCalibrationName!= "none" else "none"

    # arguments to give to addJetFinder
    # format is 'input' : dict_of_args
    addJetFinderArgs = { 'LCTopo' : dict(gettersin='lctopo', modifiersin=calibarg ,ghostArea=0.01,  ), 
                         'EMTopo' : dict(gettersin='emtopo', modifiersin=calibarg ,ghostArea=0.01,   ), 
                         'ZTrack' : {},
                         }

    ptminFilterDic = {
        "AntiKt4LCTopoJetsTest" : 7000,
        "AntiKt4EMTopoJetsTest" : 5000,
        "AntiKt10LCTopoJetsTest":50000,
        "CamKt12LCTopoJetsTest" :50000,
        }
    
    tools = []
    for jname in containerToRebuild:
        # decompose arg name 
        finder, mainParam, input = interpretJetName(jname)

        # call addJetFinderArgs with the relavant args for this collection
        t=jtm.addJetFinder(jname, finder, mainParam, ptmin=2000,
                           ptminFilter= ptminFilterDic.get(jname,-1),
                           **addJetFinderArgs[input] )

        tools.append( t )

                         
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence= AlgSequence()
    
    topSequence += JetAlgorithm( "JetAlgorithmTest", Tools = [jtm.tracksel, jtm.tvassoc, ]+tools )


def saveJetContainers():
    global containerToRebuild

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence= AlgSequence()

    for jname in containerToRebuild:
        topSequence.StreamAOD.ItemList += ["xAOD::JetContainer_v1#"+jname, "xAOD::JetAuxContainer_v1#"+jname+"Aux."]
    
    




# if we need customized one-line call, let's define them here :
def scheduleRDOtoESD():
    return scheduleRTTJetTests()
def scheduleESDtoAOD():
    return scheduleRTTJetTests()

def scheduleTest():
    return scheduleRTTJetTests(["AntiKt4LCTopoJetsTest"],[])



