# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
def interpretJetName(jetcollName,  finder = None,input=None, mainParam=None):
    # first step : guess the finder, input , mainParam, if needed
    if finder is None:
        for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']:
            if jetcollName.startswith(a):
                finder = a
                break
        if finder is None:
            print ("Error could not guess jet finder type in ",jetcollName)
            return 

    if input is None:
        for i in ['LCTopo','Tower','Topo', "Truth"]:
            if i in jetcollName:
                input = i
                if i== "Tower":
                    if 'Ghost' in jetcollName:
                        input = 'Tower'
                    else:
                        input = "TopoTower"
                break
        if input is None:
            print ("Error could not guess input type in ",jetcollName)
            return
        
    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            print ("Error could not guess main parameter in ",jetcollName)
            return

    return finder, input, mainParam
    



def setupGhostAssociator(assocName, assocType, assocKeys="", seq=AlgSequence()):
    from JetMomentTools.JetMomentToolsConf import  GhostAssociator_JetINav4MomAssociation_ , GhostTrackAssociator , GhostAssociator_TruthPtAssociation_

    # make sure the associated container key is given as a list
    assocKeys = [assocKeys] if not isinstance(assocKeys, list) else assocKeys

    #  get the correct associator
    if assocType == "TrackAssociation":
        assocTool = GhostTrackAssociator(assocName, AssociationName = assocName, AssociatedContainers = assocKeys )
        assocTool.UseTrackSelector = True
        from JetRec.TrackSelectionForJets import getDefaultJetVtxTrackHelper
        assocTool.TrackSelector =  getDefaultJetVtxTrackHelper() 
    elif assocType == "TruthAssociation":
        from JetSimTools.JetTruthParticleFilterGetter import JetTruthParticleFilterGetter
        if assocKeys == [""]:
            assocKeys = [JetTruthParticleFilterGetter(seq=seq)._outputName]
        assocTool = GhostAssociator_JetINav4MomAssociation_(assocName,  AssociationName = assocName, AssociatedContainers = assocKeys, AssociatedPtMoment="pt_truth" ) 
    elif assocType == "TruthPtAssociation":
        from JetSimTools.JetTruthParticleFilterGetter import JetTruthParticleFilterGetter
        if assocKeys == [""]:
            assocKeys = [JetTruthParticleFilterGetter(seq=seq)._outputName]        
        assocTool = GhostAssociator_TruthPtAssociation_(assocName, AssociationName = assocName, AssociatedContainers = assocKeys ) 
    else:
        #print ("Can't associate to ",assocType, " will use a generic association JetINav4MomAssociation")
        assocTool = GhostAssociator_JetINav4MomAssociation_(assocName, AssociationName = assocName, AssociatedContainers = assocKeys ) 

    return assocTool


def defaultGhostAssociator( assocName, seq=AlgSequence() ):
    """Shorcuts for creating usual ghost associators """
    if assocName == "TrackAssoc":
        from JetRec.TrackSelectionForJets import trackParticleContainer
        return setupGhostAssociator( "TrackAssoc", "TrackAssociation", [trackParticleContainer] , seq=seq)
    elif assocName == "TruthPt":
        return setupGhostAssociator( "TruthPt", "TruthPtAssociation", seq=seq)
    elif assocName == "TruthAssoc":
        return setupGhostAssociator( "TruthAssoc", "TruthAssociation", seq=seq)
    elif assocName == "TruthBHadron":
        return setupGhostAssociator( "TruthBHadron", "", ["TruthBHadrons"], seq=seq)



def setupGhostAssociationTool(jetcollName, ghostsAssoc = [], finder = None,input=None, mainParam=None, seq = AlgSequence()):
    from JetMomentTools.JetMomentToolsConf import GhostAssociationTool
    from JetRec.JetGetters import getStandardInputTools, getStandardFinderTools

    # first step : guess the finder, input , mainParam, if needed
    finder, input, mainParam = interpretJetName( jetcollName, finder, input, mainParam)
    replaceNegEnergy =  'Ghost' in input

    #print ('Will create an association by re-running with this jet alg : ', finder, mainParam, input)
    
    # Create the tool 
    t= GhostAssociationTool("JetGhostAsso")
    ghostAssocTools = []
    for g in ghostsAssoc:
        if isinstance(g,str):
            ghostAssocTools.append( defaultGhostAssociator(g,seq=seq) )
        else: ## Assume g is a GhostAssociator
            ghostAssocTools.append( g)
        
    t.GhostAssociators = ghostAssocTools
    t.JetFindingSequence = getStandardInputTools( input, replaceNegEnergy =replaceNegEnergy )+getStandardFinderTools( finder , mainParam)
    t.JetFindingSequence[0].NoCleanup = True # Necessary !

    return t


def addGhostAssociation(jetcollName, ghostAssoc = [], finder = None,input=None, mainParam=None, seq=AlgSequence()):
    from JetRec.JetMomentGetter import make_JetMomentGetter

    t = setupGhostAssociationTool( jetcollName, ghostAssoc, finder ,input, mainParam, seq = seq)
    g=make_JetMomentGetter(jetcollName,[t],seq=seq)
    return g.getJetMomentCalc()

    
def setupGhostAssociationTool2(jetcollName, assocName, assocType, assocKeys="", finder = None,input=None, mainParam=None):
    from JetMomentTools.JetMomentToolsConf import GhostAssociationTool
    from JetRec.JetGetters import getStandardInputTools, getStandardFinderTools

    # first step : guess the finder, input , mainParam, if needed
    finder, input, mainParam = interpretJetName( jetcollName, finder, input, mainParam)
    
    #print ('Will create an association by re-running with this jet alg : ', finder, mainParam, input)
    
    # Create the tool 
    t= GhostAssociationTool("JetAsso"+assocName)
    t.GhostAssociators = [ setupGhostAssociator(assocName , assocType, assocKeys) ]
    # t.GhostAssociators = [ associatorClass(assocName, AssociationName = assocName, AssociatedContainers = assocKeys ) ]
    t.JetFindingSequence = getStandardInputTools( input )+getStandardFinderTools( finder , mainParam)
    t.JetFindingSequence[0].NoCleanup = True # Necessary !

    return t





def addGhostAssociation2(jetcollName, assocName, assocType, assocKeys="", finder = None,input=None, mainParam=None):
    from JetRec.JetMomentGetter import make_JetMomentGetter

    t = setupGhostAssociationTool( jetcollName, assocName, assocType, assocKeys, finder ,input, mainParam)
    g=make_JetMomentGetter(jetcollName,[t])
    return g.getJetMomentCalc()
