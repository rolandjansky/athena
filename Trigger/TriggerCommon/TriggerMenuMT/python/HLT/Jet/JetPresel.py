# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

import re

from .JetRecoCommon import getJetCalibDefaultString, jetChainParts
from ..Menu.SignatureDicts import JetChainParts_Default
from ..Menu.MenuComponents import NoHypoToolCreated


from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict

# Extract preselection reco dictionary
# This is 1:1 based on the main (tracking/PF) jet collection
def getPreselRecoDict(reco):

    # Define a fixed preselection dictionary for prototyping -- we may expand the options
    preselRecoDict = {
        'recoAlg':reco,
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
    preselRecoDict.update({'jetCalib':getJetCalibDefaultString(preselRecoDict) if preselRecoDict['recoAlg']=='a4' else 'nojcalib'}) #Adding default calibration for corresponding chain

    return preselRecoDict

# Find the preselection definition in the chainParts
# Simply taken from the last chainPart
def extractPreselection(fullChainDict):
    jparts = jetChainParts(fullChainDict['chainParts'])
    return jparts[-1]['trkpresel']

# Calo jet preselection hypo tool generator
# Translate the preselection expression in the main jet chainDict into a temporary chainDict
# that is only seen by the standard hypo tool generator, and used to return a configured
# hypo tool for the preselection step
def caloPreselJetHypoToolFromDict(mainChainDict):
    preselChainDict = dict(mainChainDict)
    preselChainDict['chainParts']=[]
    trkpresel = extractPreselection(mainChainDict)

    # Get from the last chainPart in order to avoid to specify preselection for every leg
    #TODO: add protection for cases where the preselection is not specified in the last chainPart
    presel_matched = re.match(r'presel(?P<cut>\d?\d?[jacf][\d\D]+)', trkpresel)
    assert presel_matched is not None, "Impossible to match preselection pattern for self.trkpresel=\'{0}\'.".format(trkpresel)
    presel_cut_str = presel_matched.groupdict()['cut'] #This is the cut string you want to parse. For example 'presel2j50XXj40'

    preselCommonJetParts = dict(JetChainParts_Default)

    for ip,p in enumerate(presel_cut_str.split('XX')):
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
            'chainPartIndex': ip,
            }
        )
        preselChainDict['chainParts'] += [tmpChainDict]

    # We need to pad by the legs not in the preselection expression
    # otherwise the ComboHypo does not find the corresponding
    # legs in the DecisionObject and kills the event
    jetlegs = sum([p['signature'] in ["Jet","Bjet"] for p in mainChainDict['chainParts']])
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
                'tboundary': 'SHARED',
                }
        )
        preselChainDict['chainParts'] += [dict(dummyLegPart) for i in range(padding)]
        # Last one is not permitted to be shared as there is nothing following
        preselChainDict['chainParts'][-1]['chainPartName']='j0'
        preselChainDict['chainParts'][-1]['tboundary']=''
    
    # Update certain keys to be consistent with the main dict
    # Also set the chainPart indices correctly from the main dict
    # These should be index-parallel now, as we only receive jet chainParts
    for porig,ppresel in zip(mainChainDict['chainParts'],preselChainDict['chainParts']):
        for key in ['chainPartIndex','signature']:
            ppresel[key] = porig[key]
        ppresel['chainPartName']=f"{ppresel['multiplicity']}j{ppresel['threshold']}_{ppresel['etaRange']}'"

    assert(len(preselChainDict['chainParts'])==len(mainChainDict['chainParts']))
    try:
        return trigJetHypoToolFromDict(preselChainDict)
    except NoHypoToolCreated as nohypo:
        raise nohypo # We only generate the hypo tool for the first jet leg
    except Exception as e:
        log.error("Failure with preselection for chain %s",mainChainDict['chainName'])
        raise e