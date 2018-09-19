from DerivationFrameworkCore.DerivationFrameworkMaster import *


##########################################################################################
# MET
##########################################################################################

def addMETAssocMap(sequence=DerivationFrameworkJob, 
	        	jettype='PFlowJet',
	        	assocname='AntiKt4EMPFlow',
	        	jetcoll='AntiKt4EMPFlowJets',
	        	doPflow=True,
	        	dorecoil=False,
	        	algname='METAssociation'):

	print 'addMETAssocMap: adding ', assocname, " map to STDM"

	from METReconstruction.METRecoFlags import metFlags
	from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig
	from METReconstruction.METAssocConfig import getMETAssocAlg

	####### CHSParticleFlowObjects
	'''	
	from JetRec.JetRecConf import JetToolRunner
	from RecExConfig.AutoConfiguration import IsInInputFile
	containerexists = IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects")

	algexists = hasattr(sequence,"jetalgCHSPFlow")
	##print "containerexists = ", containerexists
	##print "algexists       = ", algexists

	if not (containerexists or algexists):
	    from JetRec.JetRecStandard import jtm
	    jtm += JetToolRunner("jetconstitCHSPFlow",
	                         EventShapeTools=[],
	                         Tools=[jtm.JetConstitSeq_PFlowCHS],
	                         )
	    #print "adding JetToolRunner"
	    from JetRec.JetRecConf import JetAlgorithm
	    sequence += JetAlgorithm("jetalgCHSPFlow", Tools=[jtm.jetconstitCHSPFlow])
	'''
	#######

	associators = [AssocConfig(jettype),
             	   AssocConfig('Muon'),
            	   AssocConfig('Ele'),
            	   AssocConfig('Gamma'),
            	   AssocConfig('Tau'),
            	   AssocConfig('Soft')]

	cfg = METAssocConfig(assocname,
                    	associators,
                    	doPFlow=doPflow,
                        doRecoil=dorecoil
                        #modConstKey="JetETMissNeutralParticleFlowObjects"
                        #modConstKey="CHSParticleFlowObjects"
                    	)

	metFlags.METAssocConfigs()[cfg.suffix] = cfg
	metFlags.METAssocOutputList().append(cfg.suffix)


	metAlg = getMETAssocAlg(algname)
	sequence += metAlg



def MakeMET(sequence=DerivationFrameworkJob, 
			assocname='AntiKt4EMPFlow',
			jetcoll='AntiKt4EMPFlowJets',
			setjetminptToinf=False):

	print 'MakeMET: making MET for ', assocname, " in STDM"

	from METUtilities.METMakerConfig import getMETMakerAlg

	makerAlg = getMETMakerAlg(assocname,jetColl=jetcoll,setJetMinWPtToInf=setjetminptToinf)
	sequence += makerAlg


def applyPFOAugmentation(sequence=DerivationFrameworkJob):

    print 'In applyPFOAugmentation...'

    # simple set up -- either the alg exists and contains the tool, in which case we exit
    if hasattr(sequence,"PFlowAugmentation"):
        print "PFlowCommon: PFlowAugmentation already scheduled on sequence", sequence.name
        return
    else:
        # otherwise schedule it
        pfaug = CfgMgr.DerivationFramework__CommonAugmentation("PFlowAugmentation")
        sequence += pfaug

        pfoaugtool = None
        from AthenaCommon.AppMgr import ToolSvc        
        # create and add the tool to the alg if needed
        if hasattr(ToolSvc,"PFlowAugmentationTool"):
            pfoaugtool = getattr(ToolSvc,"PFlowAugmentationTool")
        else:
            weightpfotool = CfgMgr.CP__WeightPFOTool("PFAugmentationWeightTool",DoEoverPWeight=True)
            ToolSvc += weightpfotool
            pfoaugtool = CfgMgr.DerivationFramework__PFlowAugmentationTool("PFlowAugmentationTool",
                                                                           WeightPFOTool=weightpfotool)
            ToolSvc += pfoaugtool
        if not pfoaugtool in pfaug.AugmentationTools:
            pfaug.AugmentationTools.append(pfoaugtool)        

##################################################################



