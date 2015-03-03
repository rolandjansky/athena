# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TgcD3PD.py 356510 2011-04-05 11:52:51Z kanno $
#
# D3PD objects saving information about Tgc
#
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
import TrigMuonD3PDMaker

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "Muon::TgcCoinDataContainer", "TrigT1CoinDataCollection" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "TgcD3PD" )
    _mlog.warning( "No Muon::TgcCoinDataContainer object in input; skipping." )


def makeTgcCoinDataObject( name, prefix, object_name, getter = None,
                           sgkey = None, label = None,
                           getSL = None, getHpt = None):
    if sgkey  == None: sgkey = "TrigT1CoinDataCollection"
    if label  == None: label = prefix
    if getSL  == None: getSL = True
    if getHpt == None: getHpt = True
    if not getter:
        getter = TrigMuonD3PDMaker.TgcCoinDataGetterTool \
                 ( name + "_Getter", SGKey = sgkey, Label = label,
                   GetSL = getSL, GetHpt = getHpt  )
    return D3PDMakerCoreComps.VectorFillerTool( name, Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                AllowMissing = not _haveRDO )

def makeTgcCoinDataSLObject( name, prefix, object_name, getter = None,
                             sgkey = None, label = None):
    return makeTgcCoinDataObject( name, prefix, object_name, getter = getter,
                                  sgkey = sgkey, label = label,
                                  getSL = True, getHpt = True)

def makeTgcCoinDataHptObject( name, prefix, object_name, getter = None,
                              sgkey = None, label = None):
    return makeTgcCoinDataObject( name, prefix, object_name, getter = getter,
                                  sgkey = sgkey, label = label,
                                  getSL = False, getHpt = True)

def makeTgcCoinDataTrackletObject( name, prefix, object_name, getter = None,
                                   sgkey = None, label = None):
    return makeTgcCoinDataObject( name, prefix, object_name, getter = getter,
                                  sgkey = sgkey, label = label,
                                  getSL = False, getHpt = False)


def makeTgcCoinHierarchyD3PDObject( name, prefix, object_name, getter = None,
                                    label = None,
                                    getPrior = False, getCurrent = False, getNext = False):
    if label  == None: label = prefix
    if not getter:
        getter = TrigMuonD3PDMaker.TgcCoinHierarchyGetterTool \
                 ( name + "_Getter", Label = label,
                   GetPrior = getPrior, GetCurrent = getCurrent, GetNext = getNext )
    return D3PDMakerCoreComps.VectorFillerTool( name, Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                AllowMissing = not _haveRDO )


TgcCoinDataSLD3PDObject = D3PDObject( makeTgcCoinDataSLObject,
                                      "tgc_sl_",
                                      "TgcCoinDataSLD3PDObject" )
TgcCoinDataHptD3PDObject = D3PDObject( makeTgcCoinDataHptObject,
                                        "tgc_hpt",
                                        "TgcCoinDataHptD3PDObject_var" )
TgcCoinDataTrackletD3PDObject = D3PDObject( makeTgcCoinDataTrackletObject,
                                            "tgc_tracklet",
                                            "TgcCoinDataTrackletD3PDObject" )
TgcCoinHierarchyD3PDObject = D3PDObject( makeTgcCoinHierarchyD3PDObject,
                                         "tgc_hierarchy_",
                                         "TgcCoinHierarchyD3PDObject")

def addTgcObjects( d3pdalg, level, pref, sgkey_coindata,
                   Prior = False, Current = False, Next = False):
    d3pdalg += TgcCoinDataSLD3PDObject(level, sgkey = sgkey_coindata, prefix = pref + "sl_")
    d3pdalg += TgcCoinDataHptD3PDObject(level, sgkey = sgkey_coindata, prefix = pref + "hpt_")
    d3pdalg += TgcCoinDataTrackletD3PDObject(level, sgkey = sgkey_coindata, prefix = pref + "tracklet_")
    d3pdalg += TgcCoinHierarchyD3PDObject (level,
                                           prefix = pref + "hierarchy_",
                                           getPrior = Prior, getCurrent = Current, getNext = Next,
                                           HierarchySL_target = pref + "sl_",
                                           HierarchyHptWire_target = pref + "hpt_",
                                           HierarchyHptStrip_target = pref + "hpt_",
                                           HierarchyTrackletWire_target = pref + "tracklet_",
                                           HierarchyTrackletStrip_target = pref + "tracklet_")

def TgcD3PD( d3pdalg, level = 10, addCurrentBC = True, addPriorBC = True, addNextBC = True ):
    if not _haveRDO:
        return d3pdalg

    if addCurrentBC:
        addTgcObjects(d3pdalg, level, "tgc_currentBC_","TrigT1CoinDataCollection", Current=True)
    if addPriorBC:
        addTgcObjects(d3pdalg, level, "tgc_priorBC_","TrigT1CoinDataCollectionPriorBC", Prior=True)
    if addNextBC:
        addTgcObjects(d3pdalg, level, "tgc_nextBC_","TrigT1CoinDataCollectionNextBC", Next=True)

    return d3pdalg

if _haveRDO:

    TgcCoinDataSLD3PDObject.defineBlock( 1, "CoincidenceInfo",
                                         TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                                         WriteSL = True, WriteTrackletHpt = False)
    TgcCoinDataHptD3PDObject.defineBlock( 1, "CoincidenceInfo",
                                          TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                                          WriteSL = False, WriteTrackletHpt = True)
    TgcCoinDataTrackletD3PDObject.defineBlock( 1, "CoincidenceInfo",
                                               TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                                               WriteSL = False, WriteTrackletHpt = True)


    IndexAssociation( TgcCoinHierarchyD3PDObject,
                      TrigMuonD3PDMaker.TgcCoinHierarchyCoinDataAssociationTool,
                      target = "tgc_coindata_sl_", prefix="sl_", level = 1,
                      AssocSL = True, AssocHpt = True, AssocStrip = True,
                      blockname = "HierarchySL")
    IndexAssociation( TgcCoinHierarchyD3PDObject,
                      TrigMuonD3PDMaker.TgcCoinHierarchyCoinDataAssociationTool,
                      target = "tgc_coindata_hpt_", prefix="hptwire_", level = 1,
                      AssocSL = False, AssocHpt = True, AssocStrip = False,
                      blockname = "HierarchyHptWire")
    IndexAssociation( TgcCoinHierarchyD3PDObject,
                      TrigMuonD3PDMaker.TgcCoinHierarchyCoinDataAssociationTool,
                      target = "tgc_coindata_hpt_", prefix="hptstrip_", level = 1,
                      AssocSL = False, AssocHpt = True, AssocStrip = True,
                      blockname = "HierarchyHptStrip")
    IndexAssociation( TgcCoinHierarchyD3PDObject,
                      TrigMuonD3PDMaker.TgcCoinHierarchyCoinDataAssociationTool,
                      target = "tgc_coindata_tracklet_", prefix="trackletwire_", level = 1,
                      AssocSL = False, AssocHpt = False, AssocStrip = False,
                      blockname = "HierarchyTrackletWire")
    IndexAssociation( TgcCoinHierarchyD3PDObject,
                      TrigMuonD3PDMaker.TgcCoinHierarchyCoinDataAssociationTool,
                      target = "tgc_coindata_tracklet_", prefix="trackletstrip_", level = 1,
                      AssocSL = False, AssocHpt = False, AssocStrip = True,
                      blockname = "HierarchyTrackletStrip")
