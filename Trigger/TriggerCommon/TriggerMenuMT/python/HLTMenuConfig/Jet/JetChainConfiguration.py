# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from AthenaConfiguration.AllConfigFlags import ConfigFlags

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool
from .JetRecoConfiguration import jetRecoDictToString

import copy

def jetChainParts(chainParts):
    jChainParts = []
    for p in chainParts:
        if p['trigType'] == 'j':
            jChainParts.append(p)
    return jChainParts

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class JetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        # we deliberately don't call base class constructore, since this assumes a single chain part
        # which is not the case for jets

        self.dict = copy.deepcopy(chainDict)
        
        self.chainName = self.dict['chainName']
        self.chainL1Item = self.dict['L1item']

        self.chainPart = self.dict['chainParts']
        self.L1Threshold = ''
        self.mult = 1 # from the framework point of view I think the multiplicity is 1, internally the jet hypo has to figure out what to actually do

        # these properties are in the base class, but I don't think we need them for jets
        #self.chainPartName = ''
        #self.chainPartNameNoMult = ''
        #self.chainPartNameNoMultwL1 = ''

        # expect that the L1 seed is the same for all jet parts, otherwise we have a problem
        jChainParts = jetChainParts(self.chainPart)
        # Register if this is a performance chain, in which case the HLT should be exactly j0_perf
        self.isPerf = False
        # Exotic hypo (emerging-jets, trackless jets)
        self.exotHypo = ''
        # Check if we intend to preselect events with calo jets in step 1
        self.trkpresel = "nopresel"
        for ip,p in enumerate(jChainParts):

            # Check if there is exactly one exotic hypothesis defined
            if len(p['exotHypo']) > 1:
                raise RuntimeError(f'emerging chains currently not configurable with more than one emerging selection!')
            if p['exotHypo']:
                self.exotHypo = p['exotHypo'][0]

            if p['addInfo'] == 'perf':
                # Slightly awkward check but we want to permit any L1, while
                # restricting HLT to have exactly this form and nothing else
                if self.chainName != 'HLT_j0_perf_'+self.chainL1Item:
                        raise RuntimeError(f'Invalid jet \'perf\' chain "{self.chainName}": Only "HLT_j0_perf_[L1]" is permitted!')
                self.isPerf = True
            l1th = p['L1threshold']
            if self.L1Threshold != '' and self.L1Threshold != l1th:
                raise RuntimeError('Cannot configure a jet chain with different L1 thresholds')
            self.L1Threshold = l1th
            # We require that if there is any preselection it is only written
            # in the last chainPart to avoid inconsistencies -- chain should
            # look like HLT_jA_jB_jC_preselNjX_L1BLAH
            if p["trkpresel"]!="nopresel":
                if ip+1==len(jChainParts): # Last jet chainPart, presel should go here
                    self.trkpresel=p["trkpresel"]
                    self.trkpresel_parsed_reco = {key:p[key] for key in ['recoAlg']} #Storing here the reco options from last chain part that we want to propagate to preselection (e.g. jet radius)
                else:
                    log.error("Likely inconsistency encountered in preselection specification for %s",self.chainName)
                    raise RuntimeError("Preselection %s specified earlier than in the last chainPart!",p["trkpresel"])

        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration import extractRecoDict
        self.recoDict = extractRecoDict(jChainParts)

        self._setJetName()


    # ----------------------
    # Assemble jet collection name based on reco dictionary
    # ----------------------
    def _setJetName(self):
        from ..Menu.ChainDictTools import splitChainDict
        from .JetRecoSequences import JetRecoConfiguration
        from JetRecConfig.JetDefinition import buildJetAlgName, xAODType
        subJetChainDict = {}
        for subChainDict in splitChainDict(self.dict):
            for part in subChainDict["chainParts"]:
                if part['signature'] in ["Jet", "Bjet"]:
                    subJetChainDict = subChainDict
                    break
        if not subJetChainDict:
            raise ValueError("sub Jet Chain dictionary is empty. Cannot define jet collection name on empty dictionary")
        jetRecoDict = JetRecoConfiguration.extractRecoDict(subJetChainDict["chainParts"])
        clustersKey = JetRecoConfiguration.getClustersKey(jetRecoDict)
        prefix = JetRecoConfiguration.getHLTPrefix()
        suffix = "_"+jetRecoDict["jetCalib"]
        if JetRecoConfiguration.jetDefNeedsTracks(jetRecoDict):
            suffix += "_{}".format(jetRecoDict["trkopt"])
        inputDef = JetRecoConfiguration.defineJetConstit(jetRecoDict, clustersKey = clustersKey, pfoPrefix=prefix+jetRecoDict["trkopt"])
        jetalg, jetradius, jetextra = JetRecoConfiguration.interpretRecoAlg(jetRecoDict["recoAlg"])
        actualradius = float(jetradius)/10
        self.jetName = prefix+buildJetAlgName("AntiKt", actualradius)+inputDef.label+"Jets"+suffix
        if inputDef.basetype == xAODType.CaloCluster:
             # Omit cluster origin correction from jet name
             # Keep the origin correction explicit because sometimes we may not
             # wish to apply it, whereas PFlow corrections are applied implicitly
             self.jetName = self.jetName.replace("Origin","")
        

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):                            
        log.debug("Assembling chain %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        # Only one step for now, but we might consider adding steps for
        # reclustering and trimming workflows
        chainSteps = []
        if self.recoDict["ionopt"]=="ion":
            jetCollectionName, jetDef, jetHICaloHypoStep = self.getJetHICaloHypoChainStep()
            chainSteps.append( jetHICaloHypoStep )
        elif self.recoDict["trkopt"]=="ftf":
            if self.trkpresel=="nopresel":
                clustersKey, caloRecoStep = self.getJetCaloRecoChainStep()
                chainSteps.append( caloRecoStep )
            else:
                clustersKey, preselJetDef, jetPreselStep = self.getJetCaloPreselChainStep()
                chainSteps.append( jetPreselStep )
            jetCollectionName, jetDef, jetFSTrackingHypoStep = self.getJetFSTrackingHypoChainStep(clustersKey)
            chainSteps.append( jetFSTrackingHypoStep )
        elif self.recoDict["trkopt"]=="roiftf":
            # Can't work w/o presel jets to seed RoIs
            if self.trkpresel=="nopresel":
                raise RuntimeError("RoI FTF jet tracking requested with no jet preselection to provide RoIs")
            # Set up preselection step first
            clustersKey, preselJetDef, jetPreselStep = self.getJetCaloPreselChainStep()
            chainSteps.append( jetPreselStep )
            # Standard tracking step, configure the tracking instance differently
            # Later we should convert this to a preselection-style hypo
            jetRoITrackingHypoStep = self.getJetRoITrackingHypoChainStep(preselJetDef.fullname())
            chainSteps.append( jetRoITrackingHypoStep )
            # For later
            # jetCollectionName, jetDef, jetFSTrackingHypoStep = self.getJetFSTrackingHypoChainStep(clustersKey)
            # chainSteps.append( jetFSTrackingHypoStep )
        else:
            jetCollectionName, jetDef, jetCaloHypoStep = self.getJetCaloHypoChainStep()
            chainSteps.append( jetCaloHypoStep )

        # Add exotic jets hypo
        if self.exotHypo != '' and ("emerging" in self.exotHypo or "trackless" in self.exotHypo):
            EJsStep = self.getJetEJsChainStep(jetCollectionName, self.chainName, self.exotHypo)
            chainSteps+= [EJsStep]
        
        myChain = self.buildChain(chainSteps)

        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getJetCaloHypoChainStep(self):
        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "MainStep_jet_"+jetDefStr
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloHypoMenuSequence
        jetSeq, jetDef = RecoFragmentsPool.retrieve( jetCaloHypoMenuSequence, 
                                                     ConfigFlags, isPerf=self.isPerf, **self.recoDict )
        jetCollectionName = str(jetSeq.hypo.Alg.Jets)

        return jetCollectionName, jetDef ,ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetHICaloHypoChainStep(self):
        stepName = "MainStep_HIjet"
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetHICaloHypoMenuSequence
        jetSeq, jetDef = RecoFragmentsPool.retrieve( jetHICaloHypoMenuSequence,
                                                     ConfigFlags, isPerf=self.isPerf, **self.recoDict )
        jetCollectionName = str(jetSeq.hypo.Alg.Jets)

        return jetCollectionName, jetDef ,ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetRoITrackingHypoChainStep(self, jetsInKey):
        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "RoIFTFStep_jet_"+jetDefStr
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetRoITrackingHypoMenuSequence
        jetSeq = RecoFragmentsPool.retrieve( jetRoITrackingHypoMenuSequence,
                                             ConfigFlags, jetsIn=jetsInKey, **self.recoDict )
        return ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetFSTrackingHypoChainStep(self, clustersKey):
        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "MainStep_jet_"+jetDefStr
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetFSTrackingHypoMenuSequence
        jetSeq, jetDef = RecoFragmentsPool.retrieve( jetFSTrackingHypoMenuSequence,
                                                     ConfigFlags, clustersKey=clustersKey,
                                                     isPerf=self.isPerf,
                                                     **self.recoDict )
        jetCollectionName = str(jetSeq.hypo.Alg.Jets)
        return jetCollectionName, jetDef, ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetCaloRecoChainStep(self):
        stepName = "CaloRecoPTStep_jet_"+self.recoDict["clusterCalib"]
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloRecoMenuSequence
        jetSeq, clustersKey = RecoFragmentsPool.retrieve( jetCaloRecoMenuSequence,
                                                          ConfigFlags, clusterCalib=self.recoDict["clusterCalib"] )

        return str(clustersKey), ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetCaloPreselChainStep(self):
        #Find if a a4 or a10 calo jet needs to be used in the pre-selection from the last chain dict
        import re
        assert 'recoAlg' in self.trkpresel_parsed_reco.keys(), "Impossible to find \'recoAlg\' key in last chain dictionary for preselection"
        #Want to match now only a4 and a10 in the original reco algorithm. We don't want to use a10sd or a10t in the preselection
        matched_reco = re.match(r'^a\d?\d?',self.trkpresel_parsed_reco['recoAlg'])
        assert matched_reco is not None, "Impossible to get matched reco algorithm for jet trigger preselection The reco expression {0} seems to be impossible to be parsed.".format(self.trkpresel_parsed_reco['recoAlg'])

        # Define a fixed preselection dictionary for prototyping -- we may expand the options
        preselRecoDict = {
            'recoAlg':matched_reco.group(), #Getting the outcome of the regex reco option (it should correspond to a4 or a10 depending by which chain you are configuring)
            'constitType':'tc',
            'clusterCalib':'em',
            'constitMod':'',
            'trkopt':'notrk',
            'ionopt':'noion',
        }
        ''' #Here you can set custom calibrations for large-R preselections. If you set to LCW you'll get an issue though, as the trigger expects the *same* topocluster collection to be used in the preselection and in the PFlow stage with tracking. Therefore this would need to be adapted, but it might not be so easy...
         
        if preselRecoDict['recoAlg']=='a10': #Setting LC calibrations for large-R jets
            preselRecoDict['clusterCalib']='lcw'
        '''
        from .JetRecoConfiguration import interpretJetCalibDefault
        preselRecoDict.update({'jetCalib':interpretJetCalibDefault(preselRecoDict) if preselRecoDict['recoAlg']=='a4' else 'nojcalib'}) #Adding default calibration for corresponding chain
        from ..Menu.SignatureDicts import JetChainParts_Default
        preselCommonJetParts = dict(JetChainParts_Default)
        preselCommonJetParts.update(preselRecoDict)

        preselChainDict = dict(self.dict)
        preselChainDict['chainParts']=[]

        # Get from the last chainPart in order to avoid to specify preselection for every leg
        #TODO: add protection for cases where the preselection is not specified in the last chainPart
        presel_matched = re.match(r'presel(?P<cut>\d?\d?[jacf][\d\D]+)', self.trkpresel)
        assert presel_matched is not None, "Impossible to match preselection pattern for self.trkpresel=\'{0}\'.".format(self.trkpresel)
        presel_cut_str = presel_matched.groupdict()['cut'] #This is the cut string you want to parse. For example 'presel2j50XXj40'

        for p in presel_cut_str.split('XX'):
            matched = re.match(r'(?P<mult>\d?\d?)(?P<region>[jacf])(?P<cut>\d+)', p)
            assert matched is not None, "Impossible to extract preselection cut for \'{0}\' substring. Please investigate.".format(p)
            cut_dict = matched.groupdict()
            mult,region,cut=cut_dict['mult'],cut_dict['region'],cut_dict['cut']
            chainPartName=f'{mult}j{cut}'
            if mult=='': mult='1'
            etarange = {
                "j":"0eta320", # default
                "a":"0eta490",
                "c":"0eta240",
                "f":"320eta490"}[region]

            tmpChainDict = dict(preselCommonJetParts) 
            tmpChainDict.update(
                {'L1threshold': 'FSNOSEED',
                 'chainPartName': chainPartName,
                 'multiplicity': mult,
                 'threshold': cut,
                 'etaRange':etarange,
                 'jvt':'',
                 }
            )
            preselChainDict['chainParts'] += [tmpChainDict]

        # We need to pad by the legs not in the preselection expression
        # otherwise the ComboHypo does not find the corresponding
        # legs in the DecisionObject and kills the event
        jetlegs = sum([p['signature'] in ["Jet","Bjet"] for p in self.chainPart])
        padding = jetlegs-len(preselChainDict['chainParts'])
        if padding>0:
            preselChainDict['chainParts'][-1]['chainPartName']+='_SHARED'
            preselChainDict['chainParts'][-1]['tboundary']='SHARED'
            dummyLegPart = dict(preselCommonJetParts)
            dummyLegPart.update(
                {'L1threshold': 'FSNOSEED',
                 'chainPartName': 'j0_SHARED',
                 'multiplicity': '1',
                 'threshold': '0',
                 'jvt':'',
                 'tboundary': 'SHARED'
                 }
            )
            preselChainDict['chainParts'] += [dict(dummyLegPart) for i in range(padding)]
            # Last one is not permitted to be shared as there is nothing following
            preselChainDict['chainParts'][-1]['chainPartName']='j0'
            preselChainDict['chainParts'][-1]['tboundary']=''

        assert(len(preselChainDict['chainParts'])==len(self.chainPart))
        jetDefStr = jetRecoDictToString(preselRecoDict)

        stepName = "PreselStep_jet_"+jetDefStr
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloPreselMenuSequence
        jetSeq, jetDef, clustersKey = RecoFragmentsPool.retrieve( jetCaloPreselMenuSequence,
                                                                  ConfigFlags, **preselRecoDict )

        return str(clustersKey), jetDef, ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[preselChainDict])


    def getJetEJsChainStep(self, jetCollectionName, thresh, exotdictstring):
        from TriggerMenuMT.HLTMenuConfig.Jet.ExoticJetSequences import jetEJsMenuSequence

        # Must be configured similar to : emergingPTF0p0dR1p2 or tracklessdR1p2
        if 'emerging' in exotdictstring and ('dR' not in exotdictstring \
           or 'PTF' not in exotdictstring):
            log.error('Misconfiguration of exotic jet chain - need dR and PTF options')
            exit(1)
        if 'trackless' in exotdictstring and 'dR' not in exotdictstring:
            log.error('Misconfiguration of trackless exotic jet chain - need dR option')
            exit(1)

        trackless = int(0)
        if 'emerging' in exotdictstring:
            ptf = float(exotdictstring.split('PTF')[1].split('dR')[0].replace('p', '.'))
            dr  = float(exotdictstring.split('dR')[1].split('_')[0].replace('p', '.'))
        elif 'trackless' in exotdictstring:
            trackless = int(1)
            ptf = 0.0
            dr = float(exotdictstring.split('dR')[1].split('_')[0].replace('p', '.'))
        else:
            log.error('Misconfiguration of trackless exotic jet chain - need emerging or trackless selection')
            exit(1)

        log.debug("Running exotic jets with ptf: " + str(ptf) + "\tdR: " + str(dr) + "\ttrackless: " + str(trackless) + "\thypo: " + exotdictstring)

        stepName = "EJsStep_"
        jetSeq = RecoFragmentsPool.retrieve( jetEJsMenuSequence, ConfigFlags, jetsin=jetCollectionName, name=thresh)
        #from TrigGenericAlgs.TrigGenericAlgsConfig import PassthroughComboHypoCfg
        chainStep = ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])#, comboHypoCfg=PassthroughComboHypoCfg)

        return chainStep

