# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool
from .JetRecoConfiguration import jetRecoDictToString
from ..Menu.ChainDictTools import splitChainDict
from .JetRecoSequences import JetRecoConfiguration
from JetRecConfig.JetDefinition import buildJetAlgName, xAODType

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

        self._setJetName()

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
                raise RuntimeError(f'Exotic chains currently not configurable with more than one exotic selection!')
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
                else:
                    log.error("Likely inconsistency encountered in preselection specification for %s",self.chainName)
                    raise RuntimeError("Preselection %s specified earlier than in the last chainPart!",p["trkpresel"])

        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration import extractRecoDict
        self.recoDict = extractRecoDict(jChainParts)

    # ----------------------
    # Assemble jet collection name based on reco dictionary
    # ----------------------
    def _setJetName(self):
        try:
            subChainDict = splitChainDict(self.dict)[0]
        except IndexError:
            raise RunTimeError("Chain dictionary is empty. Cannot define jet collection name on empty dictionary")
        jetRecoDict = JetRecoConfiguration.extractRecoDict(subChainDict["chainParts"])
        prefix = JetRecoConfiguration.getHLTPrefix()
        suffix = "_"+jetRecoDict["jetCalib"]
        inputDef = JetRecoConfiguration.defineJetConstit(jetRecoDict, pfoPrefix=prefix+jetRecoDict["trkopt"])
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
        if self.recoDict["trkopt"]=="ftf":
            if self.trkpresel=="nopresel":
                clustersKey, caloRecoStep = self.getJetCaloRecoChainStep()
                chainSteps.append( caloRecoStep )
            else:
                clustersKey, jetPreselStep = self.getJetCaloPreselChainStep()
                chainSteps.append( jetPreselStep )
            jetCollectionName, jetDef, jetTrackingHypoStep = self.getJetTrackingHypoChainStep(clustersKey)
            chainSteps.append( jetTrackingHypoStep )
        else:
            jetCollectionName, jetDef, jetCaloHypoStep = self.getJetCaloHypoChainStep()
            chainSteps.append( jetCaloHypoStep )

        if self.dict["eventBuildType"]=="JetDS":
            # Select the TLA jets from the full jet container
            # rather than the filtered one seen by the hypo
            # (No diff in practice if the TLA cut is higher than the hypo filter)
            TLAStep = self.getJetTLAChainStep(jetDef.fullname())
            chainSteps+= [TLAStep]

        # Add exotic jets hypo
        if self.exotHypo != '' and ("Exotic" in self.exotHypo or "Trackless" in self.exotHypo):
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
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloHypoMenuSequence
        jetSeq, jetDef = RecoFragmentsPool.retrieve( jetCaloHypoMenuSequence, 
                                                     ConfigFlags, isPerf=self.isPerf, **self.recoDict )
        jetCollectionName = str(jetSeq.hypo.Alg.Jets)

        return jetCollectionName, jetDef ,ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetTrackingHypoChainStep(self, clustersKey):
        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "MainStep_jet_"+jetDefStr
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetTrackingHypoMenuSequence
        jetSeq, jetDef = RecoFragmentsPool.retrieve( jetTrackingHypoMenuSequence,
                                                     ConfigFlags, clustersKey=clustersKey,
                                                     isPerf=self.isPerf, **self.recoDict )
        jetCollectionName = str(jetSeq.hypo.Alg.Jets)
        return jetCollectionName, jetDef, ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetCaloRecoChainStep(self):
        stepName = "CaloRecoPTStep_jet_"+self.recoDict["clusterCalib"]
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloRecoMenuSequence
        jetSeq, clustersKey = RecoFragmentsPool.retrieve( jetCaloRecoMenuSequence,
                                                          ConfigFlags, clusterCalib=self.recoDict["clusterCalib"] )

        return str(clustersKey), ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

    def getJetCaloPreselChainStep(self):
        # Define a fixed preselection dictionary for prototyping -- we may expand the options
        preselRecoDict = {
            'recoAlg':'a4',
            'constitType':'tc',
            'clusterCalib':'em',
            'constitMod':'',
            'jetCalib':'subjesIS',
            'trkopt':'notrk',
        }
        from ..Menu.SignatureDicts import JetChainParts_Default
        preselJetParts = dict(JetChainParts_Default)
        # Get from the last chainPart... trying to anticipate potential developments
        # For now they are only in single-threshold chains anyway
        preselParts    = self.trkpresel.split('j')
        multiplicity   = preselParts[0].split('presel')[1] if preselParts[0] != 'presel' else '1'
        threshold      = preselParts[1]
        chainPartName  = multiplicity+'j'+threshold if multiplicity != '1' else 'j'+threshold
        preselJetParts.update(preselRecoDict)
        preselJetParts.update(
            {'L1threshold': 'FSNOSEED',
             'chainPartName': chainPartName,
             'multiplicity': multiplicity,
             'threshold': threshold,
             # fix selections that we don't want even if they
             # become default
             'jvt':'',
             }
        )
        preselChainDict = dict(self.dict)
        preselChainDict['chainParts'] = [preselJetParts]

        jetDefStr = jetRecoDictToString(preselRecoDict)

        stepName = "PreselStep_jet_"+jetDefStr
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloPreselMenuSequence
        jetSeq, jetDef, clustersKey = RecoFragmentsPool.retrieve( jetCaloPreselMenuSequence,
                                                                  ConfigFlags, **preselRecoDict )

        return str(clustersKey), ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[preselChainDict])

    def getJetTLAChainStep(self, jetCollectionName):
        from TriggerMenuMT.HLTMenuConfig.Jet.JetTLASequences import jetTLAMenuSequence

        stepName = "TLAStep_"+jetCollectionName
        jetSeq = RecoFragmentsPool.retrieve( jetTLAMenuSequence, None, jetsin=jetCollectionName )
        chainStep = ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])

        return chainStep


    def getJetEJsChainStep(self, jetCollectionName, thresh, exotdictstring):
        from TriggerMenuMT.HLTMenuConfig.Jet.ExoticJetSequences import jetEJsMenuSequence

        # Must be configured similar to : ExoticPTF0p0dR1p2 or TracklessdR1p2
        if 'Exotic' in exotdictstring and ('dR' not in exotdictstring \
           or 'PTF' not in exotdictstring):
            log.error('Misconfiguration of exotic jet chain - need dR and PTF options')
            exit(1)
        if 'Trackless' in exotdictstring and 'dR' not in exotdictstring:
            log.error('Misconfiguration of trackless exotic jet chain - need dR option')
            exit(1)

        trackless = int(0)
        if 'Exotic' in exotdictstring:
            ptf = float(exotdictstring.split('PTF')[1].split('dR')[0].replace('p', '.'))
            dr  = float(exotdictstring.split('dR')[1].split('_')[0].replace('p', '.'))
        elif 'Trackless' in exotdictstring:
            trackless = int(1)
            ptf = 0.0
            dr = float(exotdictstring.split('dR')[1].split('_')[0].replace('p', '.'))
        else:
            log.error('Misconfiguration of trackless exotic jet chain - need Exotic or Trackless selection')
            exit(1)

        log.debug("Running exotic jets with ptf: " + str(ptf) + "\tdR: " + str(dr) + "\ttrackless: " + str(trackless) + "\thypo: " + exotdictstring)

        stepName = "EJsStep_"+self.chainName
        jetSeq = RecoFragmentsPool.retrieve( jetEJsMenuSequence, None, jetsin=jetCollectionName, name=thresh)
        #from TrigGenericAlgs.TrigGenericAlgsConfig import PassthroughComboHypoCfg
        chainStep = ChainStep(stepName, [jetSeq], multiplicity=[1], chainDicts=[self.dict])#, comboHypoCfg=PassthroughComboHypoCfg)

        return chainStep

