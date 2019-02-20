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

    from JetRec.JetRecStandardToolManager import calib_topo_ungroomed_modifiers, topo_ungroomed_modifiers

    #calibarg = 'calib' if jetFlags.applyCalibrationName()!="none" else None

    #calibarg = 'calib' if jetFlags.applyCalibrationName!= "none" else "none"

    # arguments to give to addJetFinder
    # format is 'input' : dict_of_args
    inputArgs = { 'LCTopo' : dict(gettersin='lctopo', modifiersin='calib' ,ghostArea=0.01,  ), 
                  'EMTopo' : dict(gettersin='emtopo', modifiersin='calib' ,ghostArea=0.01,   ), 
                  'ZTrack' : {},
                  }


    fullnameArgs = {
        "AntiKt4LCTopoJetsTest" : dict( ptminFilter=7000, calibOpt='ar'),
        "AntiKt4EMTopoJetsTest" : dict( ptminFilter=5000, calibOpt='ar'),
        "AntiKt10LCTopoJetsTest": dict( ptminFilter=50000, calibOpt='a'),
        "CamKt12LCTopoJetsTest" : dict( ptminFilter=50000, calibOpt='a'),
        }
    
    tools = []
    for jname in containerToRebuild:
        # decompose arg name 
        finder, mainParam, input = interpretJetName(jname)

        args = fullnameArgs[jname]
        args.update( inputArgs[input] )
        # call addJetFinderArgs with the relavant args for this collection
        t=jtm.addJetFinder(jname, finder, mainParam, ptmin=2000,
                           **args
                           )

        tools.append( t )

                         
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence= AlgSequence()

    if jetFlags.useTruth:
        from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles        
        tools = [ jtm.truthpartcopy, jtm.truthpartcopywz ] + scheduleCopyTruthParticles() + tools
    
    topSequence += JetAlgorithm( "JetAlgorithmTest", Tools = [jtm.tracksel, jtm.tvassoc, ]+tools )


def saveJetContainers():
    global containerToRebuild

    from AthenaCommon.AppMgr import theApp
    StreamAOD = theApp.getOutputStream( "StreamAOD" )

    for jname in containerToRebuild:
        StreamAOD.ItemList += ["xAOD::JetContainer_v1#"+jname, "xAOD::JetAuxContainer_v1#"+jname+"Aux."]
    
    




# if we need customized one-line call, let's define them here :
def scheduleRDOtoESD():
    return scheduleRTTJetTests()
def scheduleESDtoAOD():
    return scheduleRTTJetTests()

def scheduleTest():
    return scheduleRTTJetTests(["AntiKt4LCTopoJetsTest"],[])



