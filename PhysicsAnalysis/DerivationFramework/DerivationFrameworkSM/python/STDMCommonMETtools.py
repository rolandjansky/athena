from DerivationFrameworkCore.DerivationFrameworkMaster import *


##########################################################################################
# MET
##########################################################################################

METLists = {}

#simplelist = ['EMTopo','LocHadTopo','Truth','Track','EMTopoRegions','LocHadTopoRegions','TruthRegions',
#          'Core_AntiKt4EMPFlow', 'Core_AntiKt4EMTopo', 'Core_AntiKt4LCTopo',
#          'Reference_AntiKt4EMPFlow', 'Reference_AntiKt4EMTopo', 'Reference_AntiKt4LCTopo',]
xaodlist = ['Calo', 'EMTopo', 'EMTopoRegions', 'LocHadTopo', 'LocHadTopoRegions', 'Track', 'Truth', 'TruthRegions']
#xaodmaps     = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow','NewAntiKt4EMPFlowHR']
#maplist      = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow','NewAntiKt4EMPFlowHR']
xaodmaps     = ['NewAntiKt4EMPFlowHR', 'AntiKt4EMPFlow']
maplist      = ['NewAntiKt4EMPFlowHR', 'AntiKt4EMPFlow']
truthmaplist = ['Truth_AntiKt4LCTopo','Truth_AntiKt4EMTopo','Truth_AntiKt4EMPFlow']
METLists['Diagnostic'] = ['Calo','EMTopo','EMTopoRegions','LocHadTopo','LocHadTopoRegions','TruthRegions']
METLists['Assocs'] = [m for m in maplist]
METLists['TruthAssocs'] = truthmaplist

# similar as in METCommon
def addMETOutputsSTDM(slimhelper, contentlist=[], slimlist=[]):
	#print 'addMETOutputsSTDM: '
	#print 'slimhelper = ', slimhelper
	#print 'contentlist = ', contentlist
	#print 'slimlist = ', slimlist

	suffixlist = []
	for content in contentlist:
		if content in METLists.keys():
			suffixlist += METLists[content]
		else:
			suffixlist.append(content)
	
	for suffix in sorted(set(suffixlist)):
		if suffix in maplist:
			print "DFMissingET -- Add containers for METAssoc_"+suffix+" to output"
			if suffix in slimlist:				
				slimhelper.SmartCollections.append("MET_Reference_"+suffix)
			elif suffix in xaodmaps:
				slimhelper.AllVariables.append("METAssoc_"+suffix)
				slimhelper.AllVariables.append("MET_Core_"+suffix)
				slimhelper.AllVariables.append("MET_Reference_"+suffix)
			else:
				slimhelper.StaticContent.append("xAOD::MissingETAssociationMap#METAssoc_"+suffix)
				slimhelper.StaticContent.append("xAOD::MissingETAuxAssociationMap#METAssoc_"+suffix+"Aux.")
				slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_Core_"+suffix)
				slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_Core_"+suffix+"Aux.")
		elif suffix in truthmaplist:
			print "DFMissingET -- Add containers for METAssoc_"+suffix+" to output"
			slimhelper.StaticContent.append("xAOD::MissingETAssociationMap#METAssoc_"+suffix)
			slimhelper.StaticContent.append("xAOD::MissingETAuxAssociationMap#METAssoc_"+suffix+"Aux.")
			slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_Core_"+suffix)
			slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_Core_"+suffix+"Aux.")
		elif suffix in xaodlist:
			print "DFMissingET -- Add containers for MET_"+suffix+" to output"
			if suffix in slimlist:
				slimhelper.SmartCollections.append("MET_"+suffix)
			else:
				slimhelper.AllVariables.append("MET_"+suffix)
		else:
			print "DFMissingET -- Add containers for MET_"+suffix+" to output"
			slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_"+suffix)
			slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_"+suffix+"Aux.")



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

	associators = [AssocConfig(jettype),
             	   AssocConfig('Muon'),
            	   AssocConfig('Ele'),
            	   AssocConfig('Gamma'),
            	   AssocConfig('Soft')]

	cfg = METAssocConfig(assocname,
                    	associators,
                    	doPFlow=doPflow,
                        doRecoil=dorecoil,
                        modConstKey="JetETMissNeutralParticleFlowObjects"
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

    print '1111111111 artur: we are in applyPFOAugmentation!!!!!'

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
            weightpfotool = CfgMgr.CP__WeightPFOTool("PFAugmentationWeightTool",DoEoverPWeight=False)
            ToolSvc += weightpfotool
            pfoaugtool = CfgMgr.DerivationFramework__PFlowAugmentationTool("PFlowAugmentationTool",
                                                                           WeightPFOTool=weightpfotool)
            ToolSvc += pfoaugtool
        if not pfoaugtool in pfaug.AugmentationTools:
            pfaug.AugmentationTools.append(pfoaugtool)        

##################################################################



