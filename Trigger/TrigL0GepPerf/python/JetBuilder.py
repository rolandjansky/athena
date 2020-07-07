DFJetAlgs = {}

def modifyClusters():

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
    
    clustSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigSKSeq',
                                                OutputContainer = 'OrigSKTopoClusters',                 
                                                InputContainer= 'CaloCalTopoClusters',                  
                                                modList = [  'softkiller'] , InputType='CaloCluster')   
     
    clustVorSeq = ctm.buildConstitModifSequence( 'ConstitOrigVorSeq',                                   
                                                OutputContainer = 'OrigVorTopoClusters',                
                                                InputContainer= 'CaloCalTopoClusters',                  
                                                modList = [ 'voronoi']   , InputType='CaloCluster')     
    clustVorSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigVorSKSeq',                               
                                                OutputContainer = 'OrigVorSKTopoClusters',              
                                                InputContainer= 'OrigVorTopoClusters',                  
                                                modList = [ 'softkiller']   , InputType='CaloCluster')

    topSequence += JetAlgorithm("ClusterModifiers", Tools = [clustSKSeq, clustVorSeq, clustVorSKSeq])


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


def modifyJets():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence=AlgSequence()
    
    from JetRec.JetRecConf import JetAlgorithm
    from JetRec.JetRecConf import PseudoJetGetter
    from JetRec.JetRecStandard import jtm

    getter0 = PseudoJetGetter("JetGetter_NoAlg",
                              Label = "LCTopo",
                              InputContainer = "CaloCalTopoClusters",
                              OutputContainer = "PseudoJetsNoAlg",
                              SkipNegativeEnergy = True,
    )

    getter1 = PseudoJetGetter("JetGetter_SK",
                              Label = "LCTopo",
                              InputContainer = "OrigSKTopoClusters",
                              OutputContainer = "PseudoJetsSK",
                              SkipNegativeEnergy = True,
    )

    getter2 = PseudoJetGetter("JetGetter_Vor",
                              Label = "LCTopo",
                              InputContainer = "OrigVorTopoClusters",
                              OutputContainer = "PseudoJetsVor",
                              SkipNegativeEnergy = True,
    )

    getter3 = PseudoJetGetter("JetGetter_VorSK",
                              Label = "LCTopo",
                              InputContainer = "OrigVorSKTopoClusters",
                              OutputContainer = "PseudoJetsVorSK",
                              SkipNegativeEnergy = True,
    )

    jtm += getter0
    jtm += getter1
    jtm += getter2
    jtm += getter3

    addStandardJets("AntiKt", 0.4, "lctopo", algseq=topSequence, namesuffix="NoAlg", outputGroup="output_noalg", customGetters=[getter0])
    addStandardJets("AntiKt", 0.4, "lctopo", algseq=topSequence, namesuffix="SK", outputGroup="output_sk", customGetters=[getter1])
    addStandardJets("AntiKt", 0.4, "lctopo", algseq=topSequence, namesuffix="Vor", outputGroup="output_vor", customGetters=[getter2])
    addStandardJets("AntiKt", 0.4, "lctopo", algseq=topSequence, namesuffix="VorSK", outputGroup="output_vorsk", customGetters=[getter3])
