import logging as log
DFJetAlgs = {}

def modifyClusters( topoclAlg, puSupprAlgs ):

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence=AlgSequence()

    from JetRecTools.JetRecToolsConfig import ctm
    from JetRecTools.JetRecToolsConf import CaloClusterConstituentsOrigin, ConstituentSubtractorTool, VoronoiWeightTool, SoftKillerWeightTool

    from CaloRec.CaloTopoClusterFlags import jobproperties
    jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value(False)

    from JetRec.JetRecConf import JetAlgorithm

    ctm.add( VoronoiWeightTool("JetConstit_Voronoi",
         doSpread=True, nSigma=0),
         alias = 'voronoi' )
    
    ctm.add( SoftKillerWeightTool("JetConstit_SoftKiller", SKGridSize=0.6),
            alias = 'softkiller' )

    inputContainer = topoclAlg + 'TopoClusters'
    log.info('\n Running pu suppression algs on ' + inputContainer + '\n')

    if 'SK' in puSupprAlgs:    
        clustSKSeq = ctm.buildConstitModifSequence( topoclAlg+'ConstitOrigSKSeq',
                                                    OutputContainer = topoclAlg+'SKTopoClusters',                 
                                                    InputContainer= inputContainer,                  
                                                    modList = [  'softkiller'] , InputType='CaloCluster')   
        topSequence += JetAlgorithm(topoclAlg+'SKClusterModifier', Tools = [clustSKSeq])

    if( 'Vor' in puSupprAlgs or 'VorSK' in puSupprAlgs):
        clustVorSeq = ctm.buildConstitModifSequence( topoclAlg+'ConstitOrigVorSeq',                                   
                                                     OutputContainer = topoclAlg+'VorTopoClusters',                
                                                     InputContainer= inputContainer,                  
                                                     modList = [ 'voronoi']   , InputType='CaloCluster')     
        topSequence += JetAlgorithm(topoclAlg+"VorClusterModifiers", Tools = [clustVorSeq])

    if 'VorSK' in puSupprAlgs:
        clustVorSKSeq = ctm.buildConstitModifSequence( topoclAlg+'ConstitOrigVorSKSeq',                               
                                                       OutputContainer = topoclAlg+'VorSKTopoClusters',              
                                                       InputContainer= topoclAlg+'VorTopoClusters',                  
                                                       modList = [ 'softkiller']   , InputType='CaloCluster')
        topSequence += JetAlgorithm(topoclAlg+"VorSKClusterModifiers", Tools = [clustVorSKSeq])

    #topSequence += JetAlgorithm("ClusterModifiers", Tools = [clustSKSeq, clustVorSeq, clustVorSKSeq])


def addStandardJets(jetalg, rsize, inputtype, ptmin=0., ptminFilter=0.,
                    mods="default", calibOpt="none", ghostArea=0.01,
                    algseq=None, namesuffix="",
                    outputGroup="CustomJets", customGetters=None, pretools = []):
    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecConf import JetAlgorithm
    jetnamebase = "{0}{1}{2}{3}".format(jetalg,int(rsize*10),inputtype,namesuffix)
    jetname = jetnamebase+"Jets"
    algname = "jetalg"+jetnamebase
    #OutputJets.setdefault(outputGroup , [] ).append(jetname)

    # return if the alg is already scheduled here :
    from RecExConfig.AutoConfiguration import IsInInputFile
    if algseq is None:
        #dfjetlog.warning( "No algsequence passed! Will not schedule "+algname )
        return
    elif IsInInputFile("xAOD::JetContainer",jetname):
        #dfjetlog.warning( "Collection  "+jetname+" is already in input AOD!" )
        return
    elif algname in DFJetAlgs:
        if hasattr(algseq,algname):
            #dfjetlog.warning( "Algsequence "+algseq.name()+" already has an instance of "+algname )
            pass
        else:
            #dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
            algseq += DFJetAlgs[algname]
        return DFJetAlgs[algname]

    if not jetname in jtm.tools:
        # no container exist. simply build a new one.
        # Set default for the arguments to be passd to addJetFinder
        defaultmods = {"EMTopo":"emtopo_ungroomed",
                       "LCTopo":"lctopo_ungroomed",
                       "EMPFlow":"pflow_ungroomed",
                       "EMCPFlow":"pflow_ungroomed",
                       "Truth":"truth_ungroomed",
                       "TruthWZ":"truth_ungroomed",
                       "PV0Track":"track_ungroomed",
                       "TrackCaloCluster":"tcc_ungroomed",
                       }
        if mods=="default":
            mods = defaultmods[inputtype] if inputtype in defaultmods else []
        finderArgs = dict( modifiersin= mods, consumers = [])
        finderArgs['ptmin'] = ptmin
        finderArgs['ptminFilter'] = ptminFilter
        finderArgs['ghostArea'] = ghostArea
        finderArgs['modifiersin'] = mods
        finderArgs['calibOpt'] = calibOpt
        #finderArgs.pop('modifiersin') # leave the default modifiers
        # map the input to the jtm code for PseudoJetGetter  
        getterMap = dict( LCTopo = 'lctopo', EMTopo = 'emtopo', EMPFlow = 'empflow', EMCPFlow = 'emcpflow',
                          Truth = 'truth',  TruthWZ = 'truthwz', TruthDressedWZ = 'truthdressedwz', TruthCharged = 'truthcharged',
                          PV0Track = 'pv0track', TrackCaloCluster = 'tcc' )

        # set input pseudojet getter -- allows for custom getters
        if customGetters is None:
            inGetter = getterMap[inputtype]
        else:
            inGetter = customGetters

        # create the finder for the temporary collection
        finderTool = jtm.addJetFinder(jetname, jetalg, rsize, inGetter,
                                      **finderArgs   # pass the prepared arguments 
                                      )

        alg = JetAlgorithm(algname, Tools = pretools+[finderTool])
        #dfjetlog.info( "Added "+algname+" to sequence "+algseq.name() )
        algseq += alg
        DFJetAlgs[algname] = alg;


def modifyJets( topoclAlgs ):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence=AlgSequence()
    
    from JetRec.JetRecConf import JetAlgorithm
    from JetRec.JetRecConf import PseudoJetGetter
    from JetRec.JetRecStandard import jtm

    Label = "LCTopo"
    label = "lctopo"
    if( "Calo420" in topoclAlgs or "Calo422" in topoclAlgs ): 
        Label = "EMTopo"
        label = "emtopo"
   
    nameAffix = topoclAlgs
    getter = PseudoJetGetter("JetGetter_"+nameAffix,
                              Label = Label,
                              InputContainer = nameAffix + "TopoClusters",
                              OutputContainer = "PseudoJets" + nameAffix,
                              SkipNegativeEnergy = True,
                          )
    jtm += getter
    
    addStandardJets("AntiKt", 0.4, label, algseq=topSequence, namesuffix=nameAffix, outputGroup="output_"+nameAffix, customGetters=[getter])



