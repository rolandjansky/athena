#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file MTMonitoring.py
@authors P-A. Delsart, Jona Bossio
@date    03/04/2020
@brief   Python configuration for the Run III Trigger Jet Monitoring
'''

from AthenaCommon.Logging import logging
logger = logging.getLogger(__name__)

from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput

#####################################
# Offline jet collections to monitor
#####################################

OfflineJetCollections = {
  'AntiKt4EMTopoJets'  : { 'MatchTo' : 'AntiKt4EMPFlowJets' },
  'AntiKt4EMPFlowJets' : { 'MatchTo' : 'NONE' },
  #'AntiKt10EMPFlowCSSKJets' : { 'MatchTo' : 'NONE' },
}

###########################################
# L1 jet collections and chains to monitor
###########################################

L1JetCollections = {
  # The MatchedTo list must be of length 2, and contain the names of an offline collection
  # and an HLT collection. These names can be the empty string.
  'LVL1JetRoIs'  : { 'MatchTo' : ['AntiKt4EMPFlowJets','HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf'] },
  'L1_jFexSRJetRoI' : { 'MatchTo' : ['AntiKt4EMPFlowJets','HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf'] },

}

# Now seeing new L1 containers of differing types. These types
# are explicit in the C++ JEtMatcherMatcher algorithm, and correspond
# top different attributes of that algorithm.
#
# l12MatcherKey allows the setting of the appropriate, which is the
# value of entries of l12MatcherKey.
l1Coll2MatcherKey = {
  'LVL1JetRoIs': 'L1JetContainerName1',
  'L1_jFexSRJetRoI': 'L1jFexSRJetRoIContainerName',
}

for k, colls in L1JetCollections.items():
  if colls and (k not in l1Coll2MatcherKey):
    logger.error('Requesting matches to an L1 container', str(k), ' but have not supplied a Matcher ReadHandleKey')
    

# the values of Chain2L1JetCollDict are keys of L1JetCollections.
# the keys of Chain2L1JetCollDict are used to select events before histograms are filled
Chain2L1JetCollDict = { # set L1 jet collection name for L1 jet chains
  'L1_J15'  : 'LVL1JetRoIs',
  'L1_J20'  : 'LVL1JetRoIs',
  'L1_J100' : 'LVL1JetRoIs',
  'L1_jJ40' : 'L1_jFexSRJetRoI',
  'L1_jJ50' : 'L1_jFexSRJetRoI',
  'L1_jJ60' : 'L1_jFexSRJetRoI',
  # 'L1_jJ50' : 'L1_jFexSRJetRoI',
  # 'L1_jJ160' : 'L1_jFexSRJetRoI',
}
Legacy2PhaseIJetThresholdDict = {
  'J5'   : 'jJ20',
  'J12'  : 'jJ30',
  'J15'  : 'jJ40',
  '4J15' : '4jJ40',
  'J20'  : 'jJ50',
  'J25'  : 'jJ55',
  'J30'  : 'jJ60',
  'J35'  : 'jJ70',
  'J40'  : 'jJ80',
  'J45'  : 'jJ85',
  'J50'  : 'jJ90',
  'J75'  : 'jJ125',
  'J85'  : 'jJ140',
  'J100' : 'jJ160',
  'J120' : 'jJ180',
  'J400' : 'jJ500',
}
Legacy2PhaseILargeJetThresholdDict = {
  'J100' : 'jLJ140'
}

############################################
# HLT jet collections and chains to monitor
############################################

Chains2Monitor  = dict() # set HLT jet collection, reference chain and offline jet collection for turn-on curves, for AT and legacy master HLT jet chains
JetCollections  = dict() # List of HLT jet collections for AT and legacy master (stating which should be matched and to which offline jet collection

# AthenaMT
JetCollections['MT'] = {
  'HLT_AntiKt4EMTopoJets_subjesIS'                                : { 'MatchTo' : 'AntiKt4EMPFlowJets' }, # default small-R EM
  'HLT_AntiKt4EMTopoJets_subjesgscIS_ftf'                         : { 'MatchTo' : 'AntiKt4EMPFlowJets' }, # a4 calo jet w/ calo+track GSC
  'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf'                        : { 'MatchTo' : 'AntiKt4EMPFlowJets' }, # a4 pflow w/ calo+track GSC
  'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf'                     : { 'MatchTo' : 'AntiKt4EMPFlowJets' }, # a4 pflow w/ residual + calo+track GSC
  'HLT_AntiKt4EMPFlowJets_nojcalib_ftf'                           : { 'MatchTo' : 'NONE' },               # a4 pflow nojcalib
  'HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf'                       : { 'MatchTo' : 'NONE' },               # a4 pflow cssk nojcalib
  'HLT_AntiKt10EMTopoRCJets_subjesIS'                             : { 'MatchTo' : 'NONE' },               # a10r
  'HLT_AntiKt10LCTopoJets_subjes'                                 : { 'MatchTo' : 'NONE' },               # a10
  'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes'              : { 'MatchTo' : 'NONE' },               # a10t
  'HLT_AntiKt10LCTopoSoftDropBeta100Zcut10Jets_nojcalib'          : { 'MatchTo' : 'NONE' },               # a10sd lcw nojcalib
  'HLT_AntiKt10EMPFlowSoftDropBeta100Zcut10Jets_nojcalib_ftf'     : { 'MatchTo' : 'NONE' },               # a10sd pflow nojcalib
  'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf' : { 'MatchTo' : 'NONE' },               # a10sd pflow cssk nojcalib
  'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf'      : { 'MatchTo' : 'NONE' },               # a10sd pflow cssk jes
}
Chains2Monitor['MT'] = {
  # perf chain (runs no hypo)
  'HLT_j0_perf_L1J12_EMPTY'                : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  # Small-R EMTopo chains
  'HLT_j420_L1J100'                        : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j260_320eta490_L1J75_31ETA49'       : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_5j70_0eta240_L14J20'                : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_3j200_L1J100'                       : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j80_j60_L1J15'                      : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',                   'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_j45_subjesgscIS_ftf_L1J15'          : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesgscIS_ftf',            'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
   ## for Phase I chain comparisons
  'HLT_j45_320eta490_L1J15p31ETA49'      : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',        'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j280_320eta490_L1J75p31ETA49'     : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS',        'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  # Small-R PFlow chains
  'HLT_j45_pf_ftf_L1J15'                   : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_j45_pf_subjesgscIS_ftf_L1J15'       : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',           'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_j85_pf_ftf_L1J20'                   : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'HLT_j45_pf_ftf_L1J15', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_j45_pf_nojcalib_ftf_L1J15'          : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_nojcalib_ftf',              'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_j45_cssk_pf_nojcalib_ftf_L1J15'     : { 'HLTColl' : 'HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',          'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  'HLT_10j40_pf_ftf_L14J15'                : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'NONE',                 'OfflineColl' : 'NONE' },
  # Large-R reclustered chains
  'HLT_j460_a10r_L1J100'                   : { 'HLTColl' : 'HLT_AntiKt10EMTopoRCJets_subjesIS',                'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R LCTopo chains
  'HLT_j460_a10_lcw_subjes_L1J100'         : { 'HLTColl' : 'HLT_AntiKt10LCTopoJets_subjes',                    'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R trimmed chains
  'HLT_j460_a10t_lcw_jes_L1J100'           : { 'HLTColl' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', 'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10t_lcw_jes_30smcINF_L1J100'  : { 'HLTColl' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', 'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : { 'HLTColl' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', 'RefChain' : 'HLT_j85_L1J20',        'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R SoftDrop chains
  'HLT_j460_a10sd_lcw_nojcalib_L1J100'                  : { 'HLTColl' : 'HLT_AntiKt10LCTopoSoftDropBeta100Zcut10Jets_nojcalib',          'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10sd_pf_nojcalib_ftf_L1J100'               : { 'HLTColl' : 'HLT_AntiKt10EMPFlowSoftDropBeta100Zcut10Jets_nojcalib_ftf',     'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10sd_cssk_pf_nojcalib_ftf_L1J100'          : { 'HLTColl' : 'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf', 'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10sd_cssk_pf_jes_ftf_preselj225_L1J100'               : { 'HLTColl' : 'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf',      'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100' : { 'HLTColl' : 'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf', 'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Chains seeded by L1SC111-CJ15
  'HLT_j460_a10t_lcw_jes_L1SC111-CJ15'          : { 'HLTColl' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', 'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j420_a10t_lcw_jes_35smcINF_L1SC111-CJ15' : { 'HLTColl' : 'HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes', 'RefChain' : 'HLT_j85_L1J20', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  # HT and dijet scenarios
  'HLT_j0_HT1000_L1J100'                              : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_HT1000XX30et_L1J20'                        : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_HT1000_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20' : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20' : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_DIJET80j12etXX0j12eta240XX700djmass_L1J20' : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
}
# Phase1 : duplicate all relevant chains with jFex algos
temp_Phase1_chains = dict()
import re
L1pattern = re.compile(r"L1([0-9]*[J][0-9]+)")
for chainName in Chains2Monitor['MT']:
  foundL1 = L1pattern.search(chainName)
  if foundL1:
    L1Legacy =  foundL1.group(1)
    if L1Legacy in Legacy2PhaseIJetThresholdDict:
        L1PhaseI = Legacy2PhaseIJetThresholdDict[L1Legacy]
        newChain = chainName.replace(L1Legacy,L1PhaseI)
        temp_Phase1_chains[newChain] = Chains2Monitor['MT'][chainName] #uses same reference chain, not phase1 variation!
    if "a10" in chainName and L1Legacy in Legacy2PhaseILargeJetThresholdDict:   ## For now monitor a10 chains seeded by both jLJ and jJ items.
        L1PhaseI = Legacy2PhaseILargeJetThresholdDict[L1Legacy]
        newChain = chainName.replace(L1Legacy,L1PhaseI)
        temp_Phase1_chains[newChain] = Chains2Monitor['MT'][chainName] #uses same reference chain, not phase1 variation!
  if 'L1SC111-CJ15' in chainName:
    for largerSeed in ('L1SC111-CjJ40', 'L1jLJ140', 'L1jLJ160') :
      newChain = chainName.replace('L1SC111-CJ15', largerSeed)
      temp_Phase1_chains[newChain] = Chains2Monitor['MT'][chainName]      
      pass
    pass
temp_Phase1_chains.update({
  # Additional Phase I test chains (beyond the duplicated ones)
  'HLT_noalg_L1jJ80'                      : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'HLT_j25_pf_ftf_L1RD0_FILLED', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_noalg_L1jJ160'                     : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'HLT_j45_pf_ftf_L1J15', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_noalg_L1jJ140'                     : { 'HLTColl' : 'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf', 'RefChain' : 'HLT_j45_pf_ftf_L1J15', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_noalg_L1jLJ140'                    : { 'HLTColl' : 'HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf', 'RefChain' : 'HLT_j45_pf_ftf_L1J15', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_j45_pf_ftf_preselj20_L1jJ40'       : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'HLT_j45_pf_ftf_preselj20_L1J15', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_j45_pf_ftf_preselj20_L1J15'        : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'HLT_j45_pf_ftf_preselj20_L1jJ40', 'OfflineColl' : 'AntiKt4EMPFlowJets' },
  'HLT_j60_pf_ftf_preselj50_L1jJ50'       : { 'HLTColl' : 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',        'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_HT1000_L1HT190-J15s5pETA21'     : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
  'HLT_j0_HT1000_L1HT190-jJ40s5pETA21'    : { 'HLTColl' : 'HLT_AntiKt4EMTopoJets_subjesIS', 'RefChain' : 'NONE', 'OfflineColl' : 'NONE' },
})

Chains2Monitor['MT'].update(temp_Phase1_chains)


# Legacy
JetCollections['Legacy'] = {
  'HLT_xAOD__JetContainer_a4tcemsubjesISFS'    : { 'MatchTo' : 'NONE' }, # default small-R
  'HLT_xAOD__JetContainer_a10r_tcemsubjesISFS' : { 'MatchTo' : 'NONE' }, # a10r
  'HLT_xAOD__JetContainer_a10tclcwsubjesFS'    : { 'MatchTo' : 'NONE' }, # a10
  'HLT_xAOD__JetContainer_a10ttclcwjesFS'      : { 'MatchTo' : 'NONE' }, # a10t
}
Chains2Monitor['Legacy'] = {
  # Small-R chains
  'HLT_j420'                               : { 'HLTColl' : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j260_320eta490'                     : { 'HLTColl' : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    'RefChain' : 'HLT_j45_320eta490', 'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j80_j60_L1J15'                      : { 'HLTColl' : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    'RefChain' : 'NONE',              'OfflineColl' : 'NONE' },
  'HLT_5j70_0eta240'                       : { 'HLTColl' : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    'RefChain' : 'NONE',              'OfflineColl' : 'NONE' },
  'HLT_3j200'                              : { 'HLTColl' : 'HLT_xAOD__JetContainer_a4tcemsubjesISFS',    'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R reclustered chains
  'HLT_j460_a10r_L1J100'                   : { 'HLTColl' : 'HLT_xAOD__JetContainer_a10r_tcemsubjesISFS', 'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R LCTopo chains
  'HLT_j460_a10_lcw_subjes_L1J100'         : { 'HLTColl' : 'HLT_xAOD__JetContainer_a10tclcwsubjesFS',    'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  # Large-R trimmed chains
  'HLT_j460_a10t_lcw_jes_L1J100'           : { 'HLTColl' : 'HLT_xAOD__JetContainer_a10ttclcwjesFS',      'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_j460_a10t_lcw_jes_30smcINF_L1J100'  : { 'HLTColl' : 'HLT_xAOD__JetContainer_a10ttclcwjesFS',      'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
  'HLT_2j330_a10t_lcw_jes_35smcINF_L1J100' : { 'HLTColl' : 'HLT_xAOD__JetContainer_a10ttclcwjesFS',      'RefChain' : 'HLT_j175',          'OfflineColl' : 'AntiKt4EMTopoJets' },
}

#########################################################
# Protections
#########################################################

# Add missing jet collections to JetCollections dict
# (this can happen if a given chain uses a jet collection that is not listed in JetCollections)
for case in ['MT','Legacy']:
  for chain,chaindict in Chains2Monitor[case].items():
    if chaindict['HLTColl'] not in JetCollections[case]: # chain will not be monitored unless HLT collection is present in JetCollections
      JetCollections[case][chaindict['HLTColl']] = { 'MatchTo' : 'NONE'}

#########################################################
# Helpful functions
#########################################################

def getEtaRange(chain):
  etaMin,etaMax = 0,2.5 # central jets by default
  if 'eta' in chain:
    etaParts    = chain.split('eta')
    etaMinTemp  = etaParts[0].split('_')
    etaMin      = etaMinTemp[len(etaMinTemp)-1]
    etaMin      = int(etaMin)/10
    etaMax      = etaParts[1].split('_')[0]
    etaMax      = int(etaMax)/10
  return etaMin,etaMax

def getBinningFromThreshold(chain,varname):
  #default binning if nothing below applies
  xbins, xmin, xmax = 160,0.,800000.
  #pt and et binning based on threshold
  if varname == "pt" or varname == "et":
    if 'noalg' in chain:
        return 60,xmin,300000 # good enough for L1 jJ40 & jJ100
    else:
        threshold = int(chain.split("_")[1].split('j')[1])
    if threshold < 50:
      return 40, 0., 100000.
    if threshold < 120:
      return 36, 20000., 200000.

    xbins = 40
    xmin = 50000.+100000.*(int(threshold/100)-1) #example: threshold = 330 -> 250 to 450; threshold = 420 -> 350 to 550
    if threshold % 100 == 0: #gives enough low bins if threshold is an exact divider of 100 GeV such as 3j200
      xmin = 1000.*(threshold - 100.)
    xmax = xmin + 200000.
    if "a10" in chain: # efficiency curve broader for large-R jets
      xmin = xmin - 50000. 
      xmax = xmax + 50000. 
      if "pf" in chain:
        xmax = xmax + 50000. # needed to include efficiency plateau for large-R PFlow chains
        if "smc" in chain:
          xmax = xmax + 50000. # efficiency plateau even higher for a10 pdf smc chains due to imperfect calibration
  #mass binning for large-R smc chains
  elif varname == "m":
    xbins, xmin, xmax = 35, 30000., 100000.
  return xbins, xmin, xmax

def getHTBinning(chain,binwidth):
  parts = chain.split('HT')
  threshold = parts[1].split('_')[0]
  if 'XX' in threshold:
    threshold = threshold.split('XX')[0]
  xmin = int(0.9 * int(threshold))  # xmin to make the threshold visible
  xmax = xmin + 500
  xbins = int((xmax-xmin)/binwidth)-1
  return xbins, xmin, xmax

#########################################################
# Schedule more histograms for dedicated jet collections
#########################################################
from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec, EventHistoSpec, SelectSpec, ToolSpec #VarSpec can be added to define specific/custom variables
from AthenaConfiguration.ComponentFactory import CompFactory

# All offline jet collections
ExtraOfflineHists = [
  "EMFrac",
  "HECFrac",
  "Jvt",
  "JVFCorr",
  "JvtRpt",
  "NumTrkPt1000[0]",
  "TrackWidthPt1000[0]",
  "SumPtTrkPt500[0]",
]

# All online small-R jet collections
ExtraSmallROnlineHists = [
  HistoSpec('et:GeV;eta',  (100,0,750, 50,-5,5) , title='#eta vs E_{T};E_{T} [GeV];#eta;Entries'),
  "EMFrac",
  "HECFrac",
  "DetectorEta",
  "ActiveArea", 
  "EM3Frac",
  "Tile0Frac",
  "LooseBad",
]

# All online large-R jet collections
ExtraLargeROnlineHists = [
]

ExtraOnlineNJetHists = [
  "njets",
  "njetsEt20Eta0_32",
  "njetsEt30Eta0_32", 
  "njetsEt50Eta0_32",
  "njetsEt80Eta0_32",
  "njetsPt20Eta0_32",
  "njetsPt30Eta0_32",
  "njetsPt50Eta0_32",
  "njetsPt80Eta0_32",
]

# Kinematics at different scales for offline and small-R online jet collections
OfflineScaleMomenta = [ "ConstitScale", "EMScale", "PileupScale", "EtaJESScale"]
OnlineScaleMomenta  = [ "ConstitScale" ]
for var in [ "pt", "eta", "m" ]:
  for offlinescale in OfflineScaleMomenta:
    ExtraOfflineHists.append("Jet"+offlinescale+"Momentum_"+var)
  for onlinescale in OnlineScaleMomenta:
    ExtraSmallROnlineHists.append("Jet"+onlinescale+"Momentum_"+var)

OnlineScaleMomenta.append("") #Adding this for convenience in the jet matching loop below
OfflineScaleMomenta.append("")

from AthenaConfiguration.AllConfigFlags import ConfigFlags

def TrigJetMonConfig(inputFlags):

  # This is the right place to get the info, but the autoconfig of the flag
  # is not yet implemented
  AthenaMT = ConfigFlags.Trigger.EDMVersion==3

  # AthenaMT or Legacy
  InputType = 'MT' if AthenaMT else 'Legacy'

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  cfg = ComponentAccumulator()

  # Match HLT jets to offline jets
  for hltColl,collDict in JetCollections[InputType].items():
    if collDict['MatchTo'] != 'NONE':
      for jetcalibscale in OnlineScaleMomenta:
        scalestring = "_"+jetcalibscale if jetcalibscale != "" else ""
        name = 'Matching_{}{}_{}'.format(hltColl,scalestring,collDict['MatchTo'])
        alg = CompFactory.JetMatcherAlg(name, JetContainerName1=hltColl,JetContainerName2=collDict['MatchTo'],JetCalibScale=jetcalibscale)
        alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
        cfg.addEventAlgo(alg)

  # Match offline to offline jets
  for offjetColl,collDict in OfflineJetCollections.items():
    if collDict['MatchTo'] != 'NONE':
      for jetcalibscale in OfflineScaleMomenta:
        scalestring = "_"+jetcalibscale if jetcalibscale != "" else ""
        name = 'Matching_{}{}_{}'.format(offjetColl,scalestring,collDict['MatchTo'])
        alg = CompFactory.JetMatcherAlg(name, JetContainerName1=offjetColl,JetContainerName2=collDict['MatchTo'],JetCalibScale=jetcalibscale)
        alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
        cfg.addEventAlgo(alg)

  # Match L1 to offline as well as HLT jets
  for l1jetColl,collDict in L1JetCollections.items():
    for matchjetcoll in collDict['MatchTo']:

      kwds = {'name' : 'Matching_{}_{}'.format(l1jetColl,matchjetcoll),
              l1Coll2MatcherKey[l1jetColl] : l1jetColl,
              'JetContainerName2': matchjetcoll,
              'MatchL1': True
              }
              
      alg = CompFactory.JetMatcherAlg(**kwds)
      alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
      cfg.addEventAlgo(alg)

  # The following class will make a sequence, configure algorithms, and link
  # them to GenericMonitoringTools
  from AthenaMonitoring import AthMonitorCfgHelper
  helper = AthMonitorCfgHelper(inputFlags,'TrigJetMonitorAlgorithm')

  # Loop over L1 jet collections
  for jetcoll in L1JetCollections:
    l1jetconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,'',True)
    l1jetconf.toAlg(helper)

  # Loop over L1 jet chains
  for chain,jetcoll in Chain2L1JetCollDict.items():
    l1chainconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,chain)
    l1chainconf.toAlg(helper)

  # Loop over offline jet collections
  for jetcoll in OfflineJetCollections:
    offlineMonitorConf = jetMonitoringConfig(inputFlags,jetcoll,AthenaMT)
    offlineMonitorConf.toAlg(helper)

  # Loop over HLT jet collections
  for jetcoll in JetCollections[InputType]:
    monitorConf = jetMonitoringConfig(inputFlags,jetcoll,AthenaMT)
    # then we turn the full specification into properly configured algorithm and tools.
    # we use the method 'toAlg()' defined for the specialized dictionnary 'JetMonAlgSpec'
    monitorConf.toAlg(helper)

  # Loop over HLT jet chains
  for chain,chainDict in Chains2Monitor[InputType].items():
    jetcoll = chainDict['HLTColl']
    # kinematic plots
    if AthenaMT:
      chainMonitorConfT = jetChainMonitoringConfig(inputFlags,jetcoll,chain,AthenaMT,True)
      chainMonitorConfT.toAlg(helper)
    chainMonitorConfF = jetChainMonitoringConfig(inputFlags,jetcoll,chain,AthenaMT,False)
    chainMonitorConfF.toAlg(helper)
    # efficiency plots
    if chainDict['RefChain'] != 'NONE' and chainDict['OfflineColl'] != 'NONE':
      effMonitorConf = jetEfficiencyMonitoringConfig(inputFlags,jetcoll,chainDict['OfflineColl'],chain,chainDict['RefChain'],AthenaMT)
      effMonitorConf.toAlg(helper)

  cfg.merge(helper.result())
  return cfg


# Basic selection of histograms common for online and offline jets
def basicJetMonAlgSpec(jetcoll,isOnline,athenaMT):
  # we use a specialized dictionnary (JetMonAlgSpec) which will be translated into the final C++ tool
  path = 'NoTriggerSelection' if isOnline else 'standardHistos/'
  minNjetBin = 1 if isOnline else 0

  TopLevelDir  = 'HLT/JetMon/'
  TopLevelDir += 'Online/' if isOnline else 'Offline/'

  # Remap online Run 2 jet collections
  from TrigJetMonitoring import JetCollRemapping
  jetcollFolder = jetcoll
  if jetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
    jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[jetcoll]
  Conf = JetMonAlgSpec(jetcoll+"Mon",JetContainerName = jetcoll, defaultPath = path, topLevelDir=TopLevelDir, bottomLevelDir=jetcollFolder, failureOnMissingContainer=False)

  # Now start filling the histo spec list    
  Conf.appendHistos(
    # See knownHistos in JetStandardHistoSpecs.py for the list of standard specification.
    "pt",  
    "m",
    "eta",
    "phi",
    "e",
    "et",
    # or we can directly add our custom histo specification in the form of a HistoSpec:
    # the basic call is : HistoSpec( variable, histobins, title='histotile;xtitle,ytitle')
    
    # Say we want a 2nd 'pt' plot but with a different binning than in the standard spec.
    # WARNING : we can not re-use the same spec name in a given JetMonitoringAlg !!!
    # so we give a new name AND we specify the actual variable with the argument 'xvar'
    #   (the ':GeV' means the variable is to be set at GeV scale)
    #HistoSpec( 'lowpt',  (100,0,150) , title='p_{T};p_{T} [GeV];', xvar='pt:GeV'),            
    # An equivalent solution would have been to clone the existing spec like in :
    # knownHistos.pt.clone('lowpt',bins= (100,0,200) ),

    # 2D histos are usually refered to by concatenating vars with a ';' as in 'varx;vary' 
    # if the 'vax;vary' alias doesn't exist in knownHistos but 'varx' and 'vary'
    # do exist, then a spec fot 'vax;vary' will be automatically generated.
    "pt;m",    # mass vs pt
    "eta;phi", # phi vs eta
    "eta;e",   # energy vs eta
    "phi;e",   # energy vs phi

    SelectSpec( 'central', '|eta|<3.2', path, FillerTools = ["pt","et","m"] ),
    SelectSpec( 'forward', '3.2<|eta|', path, FillerTools = ["pt","et","m"] ),
    SelectSpec( 'lowmu', 'avgMu<30', path, isEventVariable=True, FillerTools = ["pt","et","m","phi","eta"]),
    SelectSpec( 'highmu', '30<avgMu', path, isEventVariable=True, FillerTools = ["pt","et","m","phi","eta"]),
    # To select on multiple variables simultaneously, simply combine the selection strings via &
    # Example below to select on ET > 100 GeV and |eta| > 3.2:
    # SelectSpec( 'ETeta', '100<et:GeV&|eta|<3.2', path, FillerTools = ["pt","et","m","eta"] )
    EventHistoSpec('njets', (25,minNjetBin,25), title='NJets;NJets;Entries' ),
    EventHistoSpec('njetsPt20', (25,minNjetBin,25), title='NJetsPt20;NJetsPt20;Entries' ),
    EventHistoSpec('njetsPt50', (25,minNjetBin,25), title='NJetsPt50;NJetsPt50;Entries' ),
    # Jet multiplicity histograms can be added by using an EventHistoSpec
    # Their specifications (pT cut, ET cut, eta cuts) must be defined in the knownEventVar dictionary within JetStandardHistoSpecs.py
    # The following line is an example for a jet multiplicity histogram with ET>40 GeV, 1.0<|eta|<2.0, and binning of (10,0,10):
    # EventHistoSpec('njetsEt40Eta1_2', (10,0,10), title='NJetsEt40Eta1_2;NJetsEt40Eta1_2;Entries' ),

    # TProfile2D : just use 3 variables. For now the sytem will automatically
    #  interpret it as a TProfile2D (the 3rd variable being profiled)
    #"phi;eta;e", # --> Average Energy vs pt and eta
     
    # another possible selections : only sub-leading jets and highJVF
    #SelectSpec( 'subleading',
    #            '', # no selection on variables
    #            SelectedIndex=1, # force 2nd (sub-leading) jet (we would set 0 for leading jets)
    #            path='standardHistos', # force the path where the histos are saved in the final ROOT file
    #            FillerTools = [
    #                "pt",
    #                "m",
    #            ] ),
    #SelectSpec( 'highJVF',
    #            '0.3<JVF[0]', # JVF is a vector<float> for each jets. Here we cut on the 0th entry of this vector
    #            FillerTools = [
    #                "pt",
    #            ] ),
  )

  return Conf


def jetMonitoringConfig(inputFlags,jetcoll,athenaMT):
   '''Function to configures some algorithms in the monitoring system.'''

   isOnline  = True if 'HLT' in jetcoll else False
   InputType = 'Legacy' if not athenaMT else 'MT'
   conf      = basicJetMonAlgSpec(jetcoll,isOnline,athenaMT)

   # Declare a configuration dictionnary for a JetContainer
   if isOnline:
     if 'AntiKt4' in jetcoll or 'a4tcem' in jetcoll:
       for hist in ExtraSmallROnlineHists: conf.appendHistos(hist)
       if 'ftf' in jetcoll: # dedicated histograms for FTF chains
         conf.appendHistos("Jvt")
         conf.appendHistos("JVFCorr")
         conf.appendHistos("JvtRpt")
         conf.appendHistos("SumPtTrkPt500[0]")
         conf.appendHistos("NumTrkPt1000[0]")
         conf.appendHistos("TrackWidthPt1000[0]")
         if 'PF' in jetcoll: # dedicated histograms for online PFlow jets
           conf.appendHistos("SumPtChargedPFOPt500[0]")
           conf.appendHistos("fCharged")
     else:
       for hist in ExtraLargeROnlineHists: conf.appendHistos(hist)
     # Add matched jets plots
     if JetCollections[InputType][jetcoll]['MatchTo'] != 'NONE':
       def defineHistoForHLTJetMatch(conf, parentAlg, monhelper , path):
           # create a monitoring group with the histo path starting from the parentAlg
           group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+'/'+conf.bottomLevelDir+'/NoTriggerSelection/')
           # define the histograms
           for histname in [ 'ptdiff', 'energydiff', 'massdiff' ]: #defines which variable difference will be plotted
             group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(JetCollections[InputType][jetcoll]['MatchTo']), xbins=100 , xmin=-100000., xmax=100000. ,)
           for histname in [ 'ptresp', 'energyresp', 'massresp' ]:
             group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(JetCollections[InputType][jetcoll]['MatchTo']), xbins=100 , xmin=-2., xmax=2. ,)
           group.defineHistogram('ptresp,ptref;ptresp_vs_ptRef',title='ptresponse vs ptRef', type="TH2F", path='MatchedJets_{}'.format(JetCollections[InputType][jetcoll]['MatchTo']), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=0., ymax=500000.,)
           group.defineHistogram('ptresp,etaref;ptresp_vs_etaRef',title='ptresponse vs etaRef', type="TH2F", path='MatchedJets_{}'.format(JetCollections[InputType][jetcoll]['MatchTo']), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=-5., ymax=5.,)
       matchedJetColl   = JetCollections[InputType][jetcoll]['MatchTo']
       jetmatchKey      = '{}.matched_{}'.format(jetcoll,matchedJetColl) #we can get specific calibration scales by adding e.g. '_EtaJESScale' to the strings
       jetptdiffKey     = '{}.ptdiff_{}'.format(jetcoll,matchedJetColl)
       jetenergydiffKey = '{}.energydiff_{}'.format(jetcoll,matchedJetColl)
       jetmassdiffKey   = '{}.massdiff_{}'.format(jetcoll,matchedJetColl)
       jetptrespKey     = '{}.ptresp_{}'.format(jetcoll,matchedJetColl)
       jetenergyrespKey = '{}.energyresp_{}'.format(jetcoll,matchedJetColl)
       jetmassrespKey   = '{}.massresp_{}'.format(jetcoll,matchedJetColl)
       jetptrefKey      = '{}.ptRef_{}'.format(jetcoll,matchedJetColl)
       jetetarefKey     = '{}.etaRef_{}'.format(jetcoll,matchedJetColl)
       name = 'jetMatched_{}_{}'.format(jetcoll,matchedJetColl)
       conf.appendHistos(ToolSpec('JetHistoMatchedFiller',name,JetMatchedKey=jetmatchKey,JetPtDiffKey=jetptdiffKey,JetEnergyDiffKey=jetenergydiffKey,
                                  JetMassDiffKey=jetmassdiffKey,JetPtRespKey=jetptrespKey,JetEnergyRespKey=jetenergyrespKey,JetMassRespKey=jetmassrespKey,
                                  JetPtRefKey=jetptrefKey,JetEtaRefKey=jetetarefKey,
                                  defineHistoFunc=defineHistoForHLTJetMatch,Group='matchedJets_'+jetcoll)
       )
   else: # offline
     for hist in ExtraOfflineHists: conf.appendHistos(hist)
     if 'AntiKt4' in jetcoll: conf.appendHistos(SelectSpec( 'LooseBadFailedJets', 'LooseBad', InverseJetSel=True, FillerTools = ["pt","phi","eta"])) #cleaning variables not applicable for large-R collections
     if 'PF' in jetcoll: # dedicated histograms for offline PFlow jets
       conf.appendHistos("SumPtChargedPFOPt500[0]")
       conf.appendHistos("fCharged")
     if OfflineJetCollections[jetcoll]['MatchTo'] != 'NONE':
       def defineHistoForOfflineJetMatch(conf, parentAlg, monhelper , path):
         # create a monitoring group with the histo path starting from the parentAlg
         group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+'/'+conf.bottomLevelDir+'/standardHistos/')
         # define the histograms
         for histname in [ 'ptdiff', 'energydiff', 'massdiff' ]: #defines which variable difference will be plotted
           group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(OfflineJetCollections[jetcoll]['MatchTo']), xbins=100 , xmin=-100000., xmax=100000. ,)
         for histname in [ 'ptresp', 'energyresp', 'massresp' ]:
           group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(OfflineJetCollections[jetcoll]['MatchTo']), xbins=100 , xmin=-2., xmax=2. ,)
         group.defineHistogram('ptresp,ptref;ptresp_vs_ptRef',title='ptresp vs ptRef', type="TH2F", path='MatchedJets_{}'.format(OfflineJetCollections[jetcoll]['MatchTo']), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=0., ymax=500000.,)
         group.defineHistogram('ptresp,etaref;ptresp_vs_etaRef',title='ptresp vs etaRef', type="TH2F", path='MatchedJets_{}'.format(OfflineJetCollections[jetcoll]['MatchTo']), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=-5., ymax=5.,)
       matchedJetColl   = OfflineJetCollections[jetcoll]['MatchTo']
       jetmatchKey      = '{}.matched_{}'.format(jetcoll,matchedJetColl)
       jetptdiffKey     = '{}.ptdiff_{}'.format(jetcoll,matchedJetColl)
       jetenergydiffKey = '{}.energydiff_{}'.format(jetcoll,matchedJetColl)
       jetmassdiffKey   = '{}.massdiff_{}'.format(jetcoll,matchedJetColl)
       jetptrespKey     = '{}.ptresp_{}'.format(jetcoll,matchedJetColl)
       jetenergyrespKey = '{}.energyresp_{}'.format(jetcoll,matchedJetColl)
       jetmassrespKey   = '{}.massresp_{}'.format(jetcoll,matchedJetColl)
       jetptrefKey      = '{}.ptRef_{}'.format(jetcoll,matchedJetColl)
       jetetarefKey     = '{}.etaRef_{}'.format(jetcoll,matchedJetColl)
       name = 'jetMatched_{}_{}'.format(jetcoll,matchedJetColl)
       conf.appendHistos(ToolSpec('JetHistoMatchedFiller',name,JetMatchedKey=jetmatchKey,JetPtDiffKey=jetptdiffKey,JetEnergyDiffKey=jetenergydiffKey,
                                  JetMassDiffKey=jetmassdiffKey,JetPtRespKey=jetptrespKey,JetEnergyRespKey=jetenergyrespKey,JetMassRespKey=jetmassrespKey,
                                  JetPtRefKey=jetptrefKey,JetEtaRefKey=jetetarefKey,
                                  defineHistoFunc=defineHistoForOfflineJetMatch,Group='matchedJets_'+jetcoll)
       )

   return conf

def l1JetMonitoringConfig(inputFlags,jetcoll,chain='',matched=False):
  from TrigJetMonitoring.L1JetMonitoringConfig import L1JetMonAlg
  name = jetcoll if chain=='' else jetcoll+'_'+chain

  if not L1JetCollections[jetcoll]['MatchTo']:
    conf = L1JetMonAlg(name,jetcoll,chain)
  else:
    conf = L1JetMonAlg(name,jetcoll,chain,matched,L1JetCollections[jetcoll]['MatchTo'][0],L1JetCollections[jetcoll]['MatchTo'][1])

  return conf

def jetChainMonitoringConfig(inputFlags,jetcoll,chain,athenaMT,onlyUsePassingJets=True):
   '''Function to configures some algorithms in the monitoring system.'''

   # Remap online Run 2 jet collections
   from TrigJetMonitoring import JetCollRemapping
   jetcollFolder = jetcoll
   if jetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
     jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[jetcoll]

   # Remap Run 2 jet chain name to Run 3 jet chain
   from TrigJetMonitoring import JetChainRemapping
   if chain in JetChainRemapping.JetChainRun2ToRun3:
     chainFolder = JetChainRemapping.JetChainRun2ToRun3[chain]
   else:
     chainFolder = chain

   if not athenaMT:
     onlyUsePassingJets = False #does not work for legacy samples yet
   jetMonAlgSpecName = chain+"TrigMon"
   if not onlyUsePassingJets:
     chainFolder = chainFolder + "/ExpertHistos"
     jetMonAlgSpecName = jetMonAlgSpecName + "_ExpertHistos"

   # Define helper functions to automatize ET & eta selection strings for NJet histograms of chains
   def getThreshold(parts):
     return parts[1].split('_')[0]

   def getEtaRangeString(chain):
     etaMin, etaMax = 0, 32
     if 'eta' in chain:
       etaParts    = chain.split('eta')
       etaMinTemp  = etaParts[0].split('_')
       etaMin      = etaMinTemp[len(etaMinTemp)-1]
       etaMax      = etaParts[1].split('_')[0]
       if int(etaMin) > 0 : etaMin = str(int(int(etaMin)/10))
       if int(etaMax) > 0 : etaMax = str(int(int(etaMax)/10))
     return 'Eta{}_{}'.format(etaMin,etaMax)

   def getNjetHistName(chain):
     NjetHistName = 'NONE'
     parts         = chain.split('j')
     # check if it is a multi-threshold multijet chain or a single-threshold multijet chain
     multiplicity  = parts[0].split('_')[1] # for single-threshold multijet chains
     if (chain.count('_j')-chain.count('_jes')) > 1  or multiplicity != '':
       NjetHistName = 'njetsEt{}{}'.format(getThreshold(parts),getEtaRangeString(chain))
     return NjetHistName


   trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
       jetMonAlgSpecName,
       JetContainerName = jetcoll,
       TriggerChain = chain,
       defaultPath = chainFolder,
       topLevelDir="HLT/JetMon/Online/",
       bottomLevelDir=jetcollFolder,
       failureOnMissingContainer=True,
       onlyPassingJets=onlyUsePassingJets,
       )
   trigConf.appendHistos(
           "pt",
           "m",
           "eta",
           "et",
           "phi",
   )
   for hist in ExtraOnlineNJetHists: trigConf.appendHistos(EventHistoSpec(hist, (20,0,25), title=hist+';'+hist+';Entries'))
   # Add NjetEt and NjetPt histograms for simple scenarios
   if 'ht' not in chain and 'HT' not in chain and 'dijet' not in chain and 'DIJET' not in chain and 'fbdj' not in chain and 'noalg' not in chain:
     NjetHistName = getNjetHistName(chain)
     from JetMonitoring.JetStandardHistoSpecs import knownEventVar
     if knownEventVar.get(NjetHistName,None) is not None and NjetHistName not in ExtraOnlineNJetHists: #avoids duplication warnings for some chains
       trigConf.appendHistos(
         EventHistoSpec(NjetHistName, (25,0,25), title=NjetHistName+';'+NjetHistName+';Entries' ),
       )
     NjetHistName = NjetHistName.replace('Et','Pt')
     if knownEventVar.get(NjetHistName,None) is not None and NjetHistName not in ExtraOnlineNJetHists:
       trigConf.appendHistos(
         EventHistoSpec(NjetHistName, (25,0,25), title=NjetHistName+';'+NjetHistName+';Entries' ),
       )
   if 'ftf' in chain and 'a10' not in chain: # track-based JVT variables for FTF chains
     trigConf.appendHistos("Jvt")
     trigConf.appendHistos("JVFCorr")
     trigConf.appendHistos("JvtRpt")

   if 'ht' in chain or 'HT' in chain:
     def defineHistoForHTChain(conf, parentAlg, monhelper , path):
         # create a monitoring group with the histo path starting from the parentAlg
         group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+jetcollFolder+'/')
         # define the histograms
         xbins, xmin, xmax = getHTBinning(chain,25) # bin width in GeV
         group.defineHistogram("jetHT;HT",title="Jet HT;H_{T} [GeV];Entries", type="TH1F", path=chainFolder, xbins=xbins , xmin=xmin, xmax=xmax ,)
     trigConf.appendHistos(ToolSpec('JetHistoHTFiller','JetHistoHTFiller_'+chain,MinPt=30.,MaxEta=3.2,FailureOnMissingContainer=False,
                                  defineHistoFunc=defineHistoForHTChain,Group='jetHT_'+jetcoll))

   return trigConf

def jetEfficiencyMonitoringConfig(inputFlags,onlinejetcoll,offlinejetcoll,chain,refChain,athenaMT):
   '''Function to configures some algorithms in the monitoring system.'''

   # Remap online Run 2 jet collections
   from TrigJetMonitoring import JetCollRemapping
   jetcollFolder = onlinejetcoll
   if onlinejetcoll in JetCollRemapping.JetCollRun2ToRun3 and not athenaMT:
     jetcollFolder = JetCollRemapping.JetCollRun2ToRun3[onlinejetcoll]

   # Remap Run 2 jet chain name to Run 3 jet chain
   from TrigJetMonitoring import JetChainRemapping
   if chain in JetChainRemapping.JetChainRun2ToRun3:
     chainFolder = JetChainRemapping.JetChainRun2ToRun3[chain]
   else:
     chainFolder = chain

   # We schedule a new JetAlg which will be acting only when a TriggerChain fired (using the TriggerChain from the base classes).
   # We'll plot 1 histo build by a dedicated JetHistoTriggEfficiency tool.
   # So we'll have to explicitely give a specification via the generic dicionnary 'ToolSpec'
   # This implies defining a little function which declares to the monitoring framework which variables to histogram and how.
   #  this is done here.
   def defineHistoForJetTrigg(conf, parentAlg, monhelper , path):
       # create a monitoring group with the histo path starting from the parentAlg
       group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+jetcollFolder+'/')
       # define the histogram, give them individual names so they don't overwrite each other
       append = "offlineCut_"+conf.name.split("_")[-1] if "offlineCut" in conf.name else "noOfflineCut"
       histname = "trigEff_vs_"+conf.Var.Name+"_"+append
       xbins, xmin, xmax = getBinningFromThreshold(chain,conf.Var.Name)
       group.defineHistogram('trigPassed,jetVar;'+histname,title=histname, type="TEfficiency", path=chainFolder, xbins=xbins , xmin=xmin, xmax=xmax ,)
   # Get jet index and eta selection for offline jets
   validchain = chain.replace('noalg','j0') 
   parts        = validchain.split('j')
   multiplicity = parts[0].split('_')[1]
   if multiplicity != '': index = int(multiplicity) - 1 # single-threhold multijet chains
   else: index = 0 # single-jet chain
   etaMin,etaMax = getEtaRange(chain)

   from JetMonitoring.JetMonitoringConfig import retrieveVarToolConf
   trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
       chain+"TrigEffMon",
       JetContainerName          = offlinejetcoll,
       TriggerChain              = refChain, # reference chain
       defaultPath               = chainFolder,
       topLevelDir               = "HLT/JetMon/Online/",
       bottomLevelDir            = jetcollFolder,
       failureOnMissingContainer = True,
       onlyPassingJets           = False,
       )
   trigConf.appendHistos(
       SelectSpec( 'eff', '{}<|eta|<{}'.format(etaMin,etaMax), chainFolder, SelectedIndex=index, FillerTools = [
           # we pass directly the ToolSpec
           ToolSpec('JetHistoTriggEfficiency', chain,
                    # below we pass the Properties of this JetHistoTriggEfficiency tool :
                    Group='jetTrigGroup_'+chain,
                    Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                                                   # so we use retrieveVarToolConf("pt") which returns a full specification for the "pt" histo variable.
                    ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg),
       ] ),
   )

   if 'smc' in chain:
     trigConf.appendHistos(
             SelectSpec( 'm50', '50<m:GeV&{}<|eta|<{}'.format(etaMin,etaMax), chainFolder, SelectedIndex=index, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain+'_offlineCut_m50',
                 Group='jetTrigGroup_'+chain+'_m50',
                 Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                 ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg
               ),
             ] ),
             SelectSpec( 'et500', '500<et:GeV&{}<|eta|<{}'.format(etaMin,etaMax), chainFolder, SelectedIndex=index, FillerTools = [
               ToolSpec('JetHistoTriggEfficiency', chain+'_offlineCut_et500',
                 Group='jetTrigGroup_'+chain+'_et500',
                 Var=retrieveVarToolConf("m"), # In this context we can not just pass a str alias to describe a histo variable
                 SortJets=True,
                 ProbeTrigChain=chain,defineHistoFunc=defineHistoForJetTrigg
               ),
             ] ),
     )

   return trigConf

if __name__=='__main__':

  import sys,argparse

  # Read arguments
  parser = argparse.ArgumentParser()
  parser.add_argument('--athenaMT',            action='store_true', dest='athenaMT',            default=False)
  parser.add_argument('--legacy',              action='store_true', dest='legacy',              default=False)
  parser.add_argument('--runTruthReco',        action='store_true', dest='runTruthReco',        default=False)
  parser.add_argument('--genOfflineR10PF',     action='store_true', dest='genOfflineR10PF',     default=False)
  parser.add_argument('--printDetailedConfig', action='store_true', dest='printDetailedConfig', default=False)
  parser.add_argument('--input',               action='store',      dest='inputFile')
  args                = parser.parse_args()
  AthenaMT            = args.athenaMT
  Legacy              = args.legacy
  RunTruth            = args.runTruthReco
  GenOfflineR10PF     = args.genOfflineR10PF
  PrintDetailedConfig = args.printDetailedConfig
  # Protections
  if AthenaMT and Legacy:
    logger.error('ERROR: Choose AthenaMT or Legacy, exiting')
    sys.exit(0)
  elif not AthenaMT and not Legacy:
    logger.error('ERROR: Choose AthenaMT or Legacy, exiting')
    sys.exit(0)

  # Input file
  if args.inputFile is not None: inputFile = args.inputFile
  else:
    logger.error('ERROR: No input file provided, exiting')
    sys.exit(0)

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Setup logs
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import INFO #,DEBUG 
  log.setLevel(INFO)

  # Set the Athena configuration flags
  ConfigFlags.Input.Files = [inputFile]
  ConfigFlags.Input.isMC = True
  ConfigFlags.Output.HISTFileName = 'AthenaMTMonitorOutput.root' if AthenaMT else 'LegacyMonitoringOutput.root'
  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg = MainServicesCfg(ConfigFlags)

  # AthenaMT or Legacy
  InputType = 'MT' if AthenaMT else 'Legacy'

  # Define the output list
  outputlist = ["xAOD::EventInfo#*","xAOD::VertexContainer#*","xAOD::JetContainer#AntiKt4*Jets","xAOD::JetAuxContainer#AntiKt4*JetsAux.-PseudoJet","xAOD::JetContainer#HLT_*","xAOD::JetAuxContainer#HLT_*Aux.-PseudoJet","xAOD::ShallowAuxContainer#HLT_*Aux.-PseudoJet"]
  # Reconstruct small-R truth jets
  if RunTruth:
    from JetRecConfig.StandardSmallRJets import AntiKt4Truth # import the standard definitions
    # Add the components from our jet reconstruction job
    from JetRecConfig.JetRecConfig import JetRecCfg
    comp = JetRecCfg(AntiKt4Truth,ConfigFlags)
    cfg.merge(comp)
    # add jets to the output list
    key = "{0}Jets".format(AntiKt4Truth.basename)
    outputlist += ["xAOD::JetContainer#"+key,"xAOD::JetAuxContainer#"+key+"Aux.-PseudoJet"]

  # Reconstruct offline large-R PFlow CSSK+SD jets
  if GenOfflineR10PF:
    from JetRecConfig.JetDefinition import JetConstitSeq, JetDefinition, xAODType
    EMPFlowCSSK         = JetConstitSeq("EMPFlowCSSK", xAODType.ParticleFlow, ["CorrectPFO","CS","SK","CHS"], "JetETMissParticleFlowObjects", "CSSKParticleFlowObjects", label="EMPFlowCSSK")
    AntiKt10EMPFlowCSSK = JetDefinition("AntiKt",1.0,EMPFlowCSSK,ptmin=2e3,)
    AntiKt10EMPFlowCSSK.modifiers = ["ConstitFourMom","Sort","Filter:2000"]
    from JetRecConfig.JetGrooming import JetSoftDrop
    from JetRecConfig.StandardLargeRJets import standardrecomods,substrmods
    AntiKt10EMPFlowCSSKSoftDrop = JetSoftDrop(AntiKt10EMPFlowCSSK,modifiers=standardrecomods+substrmods,ZCut=0.1,Beta=1.0) # standard SoftDrop
    # Add the components from our jet reconstruction job
    from JetRecConfig.JetRecConfig import JetRecCfg
    comp = JetRecCfg(AntiKt10EMPFlowCSSKSoftDrop,ConfigFlags)
    cfg.merge(comp)
    # add jets to the output list
    key = "{0}Jets".format(AntiKt10EMPFlowCSSKSoftDrop.basename)
    outputlist += ["xAOD::JetContainer#"+key,"xAOD::JetAuxContainer#"+key+"Aux.-PseudoJet"]

  # Write new jet collections to AOD
  if RunTruth or GenOfflineR10PF:
    # Get the output stream components
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))

  cfg.merge(PoolReadCfg(ConfigFlags))

  # The following class will make a sequence, configure algorithms, and link
  # them to GenericMonitoringTools
  from AthenaMonitoring import AthMonitorCfgHelper
  helper = AthMonitorCfgHelper(ConfigFlags,'TrigJetMonitorAlgorithm')
  cfg.merge(helper.result()) # merge it to add the sequence needed to add matchers

  # Match HLT jets to offline jets
  for hltColl,collDict in JetCollections[InputType].items():
    if collDict['MatchTo'] != 'NONE':
      for jetcalibscale in OnlineScaleMomenta:
        scalestring = "_"+jetcalibscale if jetcalibscale != "" else ""
        name = 'Matching_{}{}_{}'.format(hltColl,scalestring,collDict['MatchTo'])
        alg = CompFactory.JetMatcherAlg(name, JetContainerName1=hltColl,JetContainerName2=collDict['MatchTo'],JetCalibScale=jetcalibscale)
        alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
        cfg.addEventAlgo(alg,sequenceName='AthMonSeq_TrigJetMonitorAlgorithm') # Add matchers to monitoring alg sequence

  # Match offline to offline jets
  for offjetColl,collDict in OfflineJetCollections.items():
    if collDict['MatchTo'] != 'NONE':
      for jetcalibscale in OfflineScaleMomenta:
        scalestring = "_"+jetcalibscale if jetcalibscale != "" else ""
        name = 'Matching_{}{}_{}'.format(offjetColl,scalestring,collDict['MatchTo'])
        alg = CompFactory.JetMatcherAlg(name, JetContainerName1=offjetColl,JetContainerName2=collDict['MatchTo'],JetCalibScale=jetcalibscale)
        alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
        cfg.addEventAlgo(alg,sequenceName='AthMonSeq_TrigJetMonitorAlgorithm')

  # Match L1 to offline as well as HLT jets
  for l1jetColl,collDict in L1JetCollections.items():
    for matchjetcoll in collDict['MatchTo']:
      if matchjetcoll != 'NONE':
        name = 'Matching_{}_{}'.format(l1jetColl,matchjetcoll)
        alg = CompFactory.JetMatcherAlg(name, L1JetContainerName1=l1jetColl,JetContainerName2=matchjetcoll,MatchL1=True)
        alg.ExtraInputs += [('xAOD::TrigCompositeContainer','StoreGateSvc+%s' % getRun3NavigationContainerFromInput(ConfigFlags))]
        cfg.addEventAlgo(alg,sequenceName='AthMonSeq_TrigJetMonitorAlgorithm')
  
  # Loop over L1 jet collectoins
  for jetcoll in L1JetCollections:
    l1jetconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,'',True)
    l1jetconf.toAlg(helper)

  # Loop over L1 jet chains
  for chain,jetcoll in Chain2L1JetCollDict.items():
    l1chainconf = l1JetMonitoringConfig(ConfigFlags,jetcoll,chain)
    l1chainconf.toAlg(helper)

  # Loop over offline jet collections
  for jetcoll in OfflineJetCollections:
    offlineMonitorConf = jetMonitoringConfig(ConfigFlags,jetcoll,AthenaMT)
    offlineMonitorConf.toAlg(helper)

  # Loop over HLT jet collections
  for jetcoll in JetCollections[InputType]:
    monitorConf = jetMonitoringConfig(ConfigFlags,jetcoll,AthenaMT)
    # then we turn the full specification into properly configured algorithm and tools.
    # we use the method 'toAlg()' defined for the specialized dictionnary 'JetMonAlgSpec'
    monitorConf.toAlg(helper)

  # Loop over HLT jet chains
  for chain,chainDict in Chains2Monitor[InputType].items():
    jetcoll = chainDict['HLTColl']
    # kinematic plots
    if AthenaMT:
      chainMonitorConfT = jetChainMonitoringConfig(ConfigFlags,jetcoll,chain,AthenaMT,True)
      chainMonitorConfT.toAlg(helper)
    chainMonitorConfF = jetChainMonitoringConfig(ConfigFlags,jetcoll,chain,AthenaMT,False)
    chainMonitorConfF.toAlg(helper)
    # efficiency plots
    if chainDict['RefChain'] != 'NONE' and chainDict['OfflineColl'] != 'NONE':
      effMonitorConf = jetEfficiencyMonitoringConfig(ConfigFlags,jetcoll,chainDict['OfflineColl'],chain,chainDict['RefChain'],AthenaMT)
      effMonitorConf.toAlg(helper)

  cfg.merge(helper.result())
  
  # Print config
  cfg.printConfig(withDetails=PrintDetailedConfig)

  cfg.run()
