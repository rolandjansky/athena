# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagJetD3PDObject.py
## @brief jet D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##

import JetTagD3PDMaker

import D3PDMakerCoreComps
from AthenaCommon.AlgSequence             import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
from JetD3PDMaker.JetD3PDObject import getJetD3PDObject


#
## hook function to add selected jet collection to storegate
## run algorithm in the D3PD presequence
#


def _jetFilterAlgHook (c, prefix, sgkey,
                       JetContainer,
                       **kw):

    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('JetContainer', sgkey):
        return

    # Is the algorithm already in the sequence?
    algname = sgkey +  JetTagD3PDKeys.FilteredJetMakerAlgPostfix()
    if hasattr (preseq, algname):
        return


    # select valid argument for JetTagJetFilterAlg
    filterargs = kw
    for k in filterargs.keys():
        if not hasattr(JetTagD3PDMaker.JetTagJetFilterAlg, k):
            del filterargs[k]

    del filterargs['name']

    ### if some argument do not exist set them to the flag values
    if 'PtMinCut' not in filterargs:
        filterargs['PtMinCut']=JetTagD3PDFlags.JetFilterPtMinCut()
    if 'EtaCut' not in filterargs:
        filterargs['EtaCut']=JetTagD3PDFlags.JetFilterEtaCut()
    if 'UseEMScale' not in filterargs:
        filterargs['UseEMScale']=JetTagD3PDFlags.JetFilterUseEMScale()
    
    preseq += JetTagD3PDMaker.JetTagJetFilterAlg(algname,
                                                 FilteredJetContainer = sgkey,
                                                 JetContainer=JetContainer,  
                                                 **filterargs)
    
    return


def make_JetTagJet_D3PDObject(name, prefix, object_name, sgkey,
                              label,
                              allowMissing=False,
                              **kw):


    getter = D3PDMakerCoreComps.SGDataVectorGetterTool(name + '_Getter',
                                                       SGKey = sgkey,
                                                       TypeName = 'JetCollection',
                                                       Label = label)
    
    defs = kw

    # select valid argument for VectorFillerTool
    # needed to remove argument for the hook function
    for k in defs.keys():
        if not hasattr(D3PDMakerCoreComps.VectorFillerTool, k):
            del defs[k]

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                AllowMissing=allowMissing,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata(),
                                                **defs)


#from JetD3PDMaker.JetD3PDObject import JetD3PDObject
#from JetTagD3PDMaker.AddBTagD3PDInfo import addBTagInfoToJetObject
#### use a deepcopy of the JetD3PDObject before changing it - not to break other d3pds if runned at the same time
#import copy
#JetTagJetD3PDObject=copy.deepcopy(JetD3PDObject)
#JetTagJetD3PDObject = JetD3PDObject
# # ### add btag blocks
# addBTagInfoToJetObject(JetTagJetD3PDObject,-10)

## create a new JetD3PDObject for btagging
JetTagJetD3PDObject=getJetD3PDObject(objectname='JetTagJetD3PDObject', 
                                      prefix='jet_', 
                                      btagleveloffset=JetTagD3PDFlags.LevelOffset())


### change the maker. needed to pass extra argument to the hook function
JetTagJetD3PDObject._maker = make_JetTagJet_D3PDObject
### define hook ### add the filter at the beggining
hooklist = JetTagJetD3PDObject._hooks
JetTagJetD3PDObject._hooks = [_jetFilterAlgHook]+hooklist
#JetTagJetD3PDObject.defineHook(_jetFilterAlgHook)

#
## get the configured JetTagJetD3PDObject
## jet collection storegate key should be specified. Also the key to be used for the filtered collection
## use multiple call for different jet collections
#

def _updateDict(name, value, kwin):
    kw = kwin.copy()
    if not kw.has_key (name+'_target'):
        kw[name+'_target']= value

    return kw
    

def getJetTagJetD3PDObject(filteredsgkey, origsgkey, level=4, prefix=None,
                           exclude=['TrueFlavorComponents','BTagComponents','BTagDefault','BTag'],
                           **kw):
              
             
    if prefix is None:
        import re
        prefix = re.sub('Jets','',origsgkey)
        prefix = re.sub('AOD','',prefix)
        prefix = re.sub('ReTagged','',prefix)
        prefix = re.sub('MuonNonInteracting','MNI',prefix)
        prefix = prefix.lower()
        prefix = "jet_"+prefix+"_"

    if JetTagD3PDFlags.TrackAssocLabel() != "":
        kw = _updateDict(JetTagD3PDKeys.JetTrackAssocBlockName(),
                         JetTagD3PDFlags.TrackAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName(),
                         JetTagD3PDFlags.TrackAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName(),
                         JetTagD3PDFlags.TrackAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName(),
                         JetTagD3PDFlags.TrackAssocLabel(),
                         kw)


    if JetTagD3PDFlags.MuonAssocLabel() != "":
        kw = _updateDict(JetTagD3PDKeys.JetMuonAssocBlockName(),
                         JetTagD3PDFlags.MuonAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SoftMuonInfoMuonAssocBlockName(),
                         JetTagD3PDFlags.MuonAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SoftMuonChi2InfoMuonAssocBlockName(),
                         JetTagD3PDFlags.MuonAssocLabel(),
                         kw)

    if JetTagD3PDFlags.Muon2AssocLabel() != "":
        kw = _updateDict(JetTagD3PDKeys.JetMuon2AssocBlockName(),
                         JetTagD3PDFlags.Muon2AssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SoftMuon2InfoMuon2AssocBlockName(),
                         JetTagD3PDFlags.Muon2AssocLabel(),
                         kw) 
        kw = _updateDict(JetTagD3PDKeys.SoftMuon2Chi2InfoMuon2AssocBlockName(),
                         JetTagD3PDFlags.Muon2AssocLabel(),
                         kw)

    if JetTagD3PDFlags.ElectronAssocLabel() != "":
        kw = _updateDict(JetTagD3PDKeys.JetElectronAssocBlockName(),
                         JetTagD3PDFlags.ElectronAssocLabel(),
                         kw)
        kw = _updateDict(JetTagD3PDKeys.SoftElectronInfoElectronAssocBlockName(),
                         JetTagD3PDFlags.ElectronAssocLabel(),
                         kw)

    if JetTagD3PDFlags.PhotonAssocLabel() !="":
        kw = _updateDict(JetTagD3PDKeys.JetPhotonAssocBlockName(),
                         JetTagD3PDFlags.PhotonAssocLabel(),
                         kw)

    if TruthD3PDFlags.GenParticleAssocLabel() != "":
         kw = _updateDict(JetTagD3PDKeys.JetGenSoftLeptonAssocBlockName(),
                         TruthD3PDFlags.GenParticleAssocLabel(),
                         kw)

    if JetTagD3PDFlags.MSVVtxInfoAssocLabel()!="":
        kw = _updateDict(JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName(),
                         JetTagD3PDFlags.MSVVtxInfoAssocLabel(),
                         kw)

    return JetTagJetD3PDObject (level,sgkey=filteredsgkey, 
                                prefix=prefix,
                                label="JetTag_"+origsgkey,
                                exclude=exclude,
                                JetContainer=origsgkey,
                                **kw) 

 



