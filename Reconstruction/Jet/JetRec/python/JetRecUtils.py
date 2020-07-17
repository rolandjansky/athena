# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

def retrieveAODList():
    from JetRec.JetRecFlags import jetFlags, JetContentDetail
    from RecExConfig.RecFlags import rec

    if rec.doWriteESD():
        return jetFlags.jetAODList()
    # then we are merging or doing a AOD ?
    # We can not simply copy what we have from input since some
    # jobs starts from empty files. See ATEAM-191.
    # We hard code the list here while waiting for a more robust solution

    l = [
        # event shape objects
        'xAOD::EventShape#Kt4EMPFlowEventShape',                    'xAOD::EventShapeAuxInfo#Kt4EMPFlowEventShapeAux.',
        'xAOD::EventShape#Kt4EMTopoOriginEventShape',               'xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux.',
        'xAOD::EventShape#Kt4LCTopoOriginEventShape',               'xAOD::EventShapeAuxInfo#Kt4LCTopoOriginEventShapeAux.',
        'xAOD::EventShape#Kt4EMTopoEventShape',                     'xAOD::EventShapeAuxInfo#Kt4EMTopoEventShapeAux.',
        'xAOD::EventShape#Kt4LCTopoEventShape',                     'xAOD::EventShapeAuxInfo#Kt4LCTopoEventShapeAux.',
        'xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape', 'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux.',
        'xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape', 'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux.',
        'xAOD::EventShape#ParticleFlowIsoCentralEventShape',        'xAOD::EventShapeAuxInfo#ParticleFlowIsoCentralEventShapeAux.',
        'xAOD::EventShape#ParticleFlowIsoForwardEventShape',        'xAOD::EventShapeAuxInfo#ParticleFlowIsoForwardEventShapeAux.',
        'xAOD::EventShape#TopoClusterIsoCentralEventShape',         'xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux.',
        'xAOD::EventShape#TopoClusterIsoForwardEventShape',         'xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux.',

        'xAOD::PFOContainer#CHSChargedParticleFlowObjects',         'xAOD::ShallowAuxContainer#CHSChargedParticleFlowObjectsAux.',
        'xAOD::PFOContainer#CHSNeutralParticleFlowObjects',         'xAOD::ShallowAuxContainer#CHSNeutralParticleFlowObjectsAux.',

        'xAOD::JetContainer#AntiKt4EMPFlowJets',                    'xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.',
        'xAOD::JetContainer#AntiKt4EMTopoJets',                     'xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.',
        'xAOD::JetContainer#AntiKt4LCTopoJets',                     'xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.',
        ]

    if jetFlags.detailLevel()>=JetContentDetail.Full:
        l += [
            'xAOD::JetContainer#AntiKt10LCTopoJets',                    'xAOD::JetAuxContainer#AntiKt10LCTopoJetsAux.',
            'xAOD::JetContainer#AntiKt2PV0TrackJets',                   'xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.',
            'xAOD::JetContainer#AntiKt4PV0TrackJets',                   'xAOD::JetAuxContainer#AntiKt4PV0TrackJetsAux.',
            #
            'xAOD::CaloClusterContainer#EMOriginTopoClusters',          'xAOD::ShallowAuxContainer#EMOriginTopoClustersAux.',
            'xAOD::CaloClusterContainer#LCOriginTopoClusters' ,         'xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.',
            ]

        if rec.doTruth():
            l += [
                'xAOD::JetContainer#AntiKt10TruthJets',                 'xAOD::JetAuxContainer#AntiKt10TruthJetsAux.',
                'xAOD::JetContainer#AntiKt10TruthWZJets',               'xAOD::JetAuxContainer#AntiKt10TruthWZJetsAux.',
                'xAOD::JetContainer#AntiKt4TruthJets',                  'xAOD::JetAuxContainer#AntiKt4TruthJetsAux.',
                'xAOD::JetContainer#AntiKt4TruthWZJets',                'xAOD::JetAuxContainer#AntiKt4TruthWZJetsAux.',
                'xAOD::JetContainer#CamKt12TruthJets',                  'xAOD::JetAuxContainer#CamKt12TruthJetsAux.',
                'xAOD::JetContainer#CamKt12TruthWZJets',                'xAOD::JetAuxContainer#CamKt12TruthWZJetsAux.',
                ]

    if jetFlags.detailLevel()>=JetContentDetail.Validation:
        l += [
            'xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets',
            'xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.',
            ]

    return l
    ## inputcontent = objKeyStore['inputFile'].list()
    ## typeToSave = [ 'xAOD::JetContainer', 'xAOD::JetAuxContainer', 'xAOD::JetTrigAuxContainer' , 'xAOD::EventShape', 'xAOD::EventShapeAuxInfo' ]

    ## def saveThisObject(o):
    ##     # we must not write out any HLT jet containers - writing of those is controlled from trigger software, not offline jet software
    ##     if  "HLT_" in o:
    ##         return False
    ##     # return True if o is of a desired type
    ##     return any( o.startswith( typ ) for typ in typeToSave )

    ## esdjets = [ o for o in inputcontent if saveThisObject(o) ]

    ## return esdjets

# define the convention that we write R truncating the decimal point
# if R>=1, then we write R*10
def formatRvalue(parameter):
    from AthenaCommon import Logging
    jetlog = Logging.logging.getLogger('JetRecUtils')
    # impose precision limits where there could be ambiguity
    if int(10*parameter)>=1 and int(100*parameter % 10):
        jetlog.warning('Radius parameter {0} exceeds allowable precision of 0.1'.format(parameter))
    if int(parameter)>=1:
        return "{0:.0f}".format(10*parameter)
    else:
        return "{0:.1g}".format(10*parameter).replace('.','')

def buildJetAlgName(finder, mainParam, variableRMassScale=0, variableRMinRadius=0):  # variableRMassScale (Rho) in MeV
    if ( variableRMassScale > 0 ):
        rmaxstr = formatRvalue(mainParam)
        rminstr = formatRvalue(variableRMinRadius)
        return finder + "VR" + str(int(variableRMassScale/1000)) + "Rmax" + rmaxstr + "Rmin" + rminstr
    return finder + formatRvalue(mainParam)

def buildJetContName(finder, mainParam, input, variableRMassScale=0, variableRMinRadius=0):
    return buildJetAlgName(finder, mainParam, variableRMassScale, variableRMinRadius) +input+"Jets" # could be more elaborated...

def interpretJetName(jetcollName,  finder = None,input=None, mainParam=None):
    myname = "JetRecUtils: "
    from AthenaCommon import Logging
    jetlog = Logging.logging.getLogger('JetRecUtils')
    # first step : guess the finder, input , mainParam, if needed
    if finder is None:
        for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']:
            if jetcollName.startswith(a):
                finder = a
                break
        if finder is None:
            jetlog.warning( "Error could not guess jet finder type in ",jetcollName )
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
            jetlog.warning(  "Error could not guess input type in ",jetcollName )
            return

    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            jetlog.warning( "Error could not guess main parameter in ",jetcollName )
            return

    return finder, mainParam, input
