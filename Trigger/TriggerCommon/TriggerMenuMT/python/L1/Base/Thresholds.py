# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re
from copy import deepcopy
from collections import OrderedDict as odict

from AthenaCommon.Logging import logging

from ..Config.TypeWideThresholdConfig import getTypeWideThresholdConfig
from .ThresholdType import ThrType
from .Limits import CaloLimits as CL
from .TopoAlgorithms import AlgCategory

from past.builtins import cmp


log = logging.getLogger("Menu.L1.Base.Thresholds")


class MenuThresholdsCollection( object ):

    def __init__(self):
        self.thresholds     = odict() # holds all thresholds
        self.thresholdNames = set()   # holds all threshold names

    def __iter__(self):
        return iter(self.thresholds.values())

    def __call__(self):
        return self.thresholds.values()

    def __iadd__(self, thr):
        if thr is None or thr.name in self.thresholdNames:
            return self
        self.thresholds[thr.name] = thr 
        self.thresholdNames.add(thr.name)
        return self
    
    def __contains__(self, thrname):
        return (thrname in self.thresholdNames)
    
    def __len__(self):
        return len(self.thresholds)

    def __getitem__(self, thrName):
        if type(thrName) != str:
            raise TypeError("Can not access threshold by %r, it is a %s" % (thrName, type(thrName)))
        if thrName not in self:
            raise KeyError("Threshold %s is not in the menu" % thrName)
        return self.thresholds[thrName]

    def __setitem__(self, thrName, thr):
        thr.name = thrName
        self += thr


    def names(self, ttype = None):
        if not ttype:
            return self.thresholdsNames
        return set([thr.name for thr in self if thr.ttype == ttype])


    def typeWideThresholdConfig(self, ttype):
        return getTypeWideThresholdConfig(ttype)

    def json(self):
        confObj = odict()
        for ttype in (ThrType.Run3Types() + ThrType.NIMTypes() + [ThrType.TOPO, ThrType.MUTOPO ]):
            confObj[ttype.name] = odict()
            confObj[ttype.name]["type"] = ttype.name
            confObj[ttype.name]["thresholds"] = odict()
        for thr in self:
            if thr.isLegacy():
                continue
            try:
                confObj[thr.ttype.name]["thresholds"][thr.name] = thr.json()
            except KeyError:
                raise RuntimeError("Run 3 threshold %s is of unsupported type %s" % (thr.name, thr.ttype.name))
        # add extra information to each type of threshold (e.g. isolation definition, min pt for objects, etc.)
        for ttype in ThrType.Run3Types():
            confObj[ttype.name].update( self.typeWideThresholdConfig(ttype) )
        return confObj


    def jsonLegacy(self):
        confObj = odict()
        for ttype in (ThrType.LegacyTypes() + [ThrType.R2TOPO]):
            confObj[ttype.name] = odict()
            confObj[ttype.name]["type"] = ttype.name
            confObj[ttype.name]["thresholds"] = odict()
        for thr in self:
            if not thr.isLegacy():
                continue
            if thr.ttype.name in confObj:
                confObj[thr.ttype.name]["thresholds"][thr.name] = thr.json()
        for ttype in ThrType.LegacyTypes():
            confObj[ttype.name].update( self.typeWideThresholdConfig(ttype) )
        return confObj



class Threshold( object ):
    __slots__ = ['name', 'ttype', 'mapping', 'thresholdValues', 'run']

    # global variable for threshold registration, if set, new thresholds will be registered with l1configForRegistration
    l1configForRegistration = None

    @staticmethod
    def setMenuConfig(mc):
        Threshold.l1configForRegistration = mc

    def __init__(self, name, ttype, mapping = -1, run = 0):
        self.name            = name
        self.ttype           = ThrType[ttype]
        self.mapping         = int(mapping)
        self.run             = int(run)
        self.thresholdValues = []
        
        if Threshold.l1configForRegistration:
            Threshold.l1configForRegistration.registerThreshold(self)

    def __str__(self):
        return self.name

    def getVarName(self):
        """returns a string that can be used as a varname"""
        return self.name.replace('.','')

    def isLegacy(self):
        return self.run == 2

    def thresholdInGeV(self):
        if len(self.thresholdValues)==0:
            return 0
        else:
            return float(self.thresholdValues[0].value)

    def json(self):
        confObj = odict()
        confObj["todo"] = "implement"
        if self.ttype == ThrType.ZB:
            confObj["seed"] = self.seed
            confObj["seed_multi"] = self.seed_multi
            confObj["bc_delay"] = self.mapping

        return confObj



class LegacyThreshold( Threshold ):
    
    def __init__(self, name, ttype, mapping = -1):
        super(LegacyThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 2)

    def addThrValue(self, value, *args, **kwargs):
        if self.ttype == ThrType.EM or self.ttype == ThrType.TAU:
            return self.addLegacyEMThresholdValue(value, *args, **kwargs)
        if self.ttype == ThrType.JET:
            return self.addLegacyJetThresholdValue(value, *args, **kwargs)
        if self.ttype in [ThrType.JE, ThrType.TE, ThrType.XE, ThrType.XS ]:
            return self.addLegacyEnergyThresholdValue(value, *args, **kwargs)
        raise RuntimeError("addThrValue() not defined for threshold type %s (%s)" % (self.ttype, self.name))

    def addLegacyEMThresholdValue(self, value, *args, **kwargs):
        # supporting both EM and TAU
        defargs = ThresholdValue.getDefaults(self.ttype.name) 
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'em_isolation', 'had_isolation', 'had_veto', 'priority', 'isobits', 'use_relIso'], args))
        # then we evaluate the arguments: first defaults, then positional arguments, then named arguments
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        thrv = ThresholdValue( self.ttype, value,
                               etamin = p['etamin'],
                               etamax=p['etamax'],
                               phimin=p['phimin'],
                               phimax=p['phimax'],
                               priority = p['priority'],
                               name = self.name+'full')
        thrv.setIsolation( em_isolation = p['em_isolation'],
                           had_isolation = p['had_isolation'],
                           had_veto = p['had_veto'],
                           isobits = p['isobits'],
                           use_relIso = p['use_relIso'])
        thrv.checkOverlapAny( self.thresholdValues )
        self.thresholdValues.append(thrv)
        return self

    def addLegacyJetThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults(self.ttype.name)
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'window', 'priority'], args))
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        thrv = ThresholdValue( self.ttype, value, 
                               etamin=p['etamin'], 
                               etamax=p['etamax'],
                               phimin=p['phimin'],
                               phimax=p['phimax'],
                               window=p['window'], 
                               priority=p['priority'],
                               name=self.name+'full')
        self.thresholdValues.append(thrv)
        return self

    def addLegacyEnergyThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults(self.ttype.name)
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'priority'], args))
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        thrv = ThresholdValue( self.ttype,
                               value,
                               etamin=p['etamin'],
                               etamax=p['etamax'],
                               phimin=p['phimin'],
                               phimax=p['phimax'],
                               priority=p['priority'],
                               name=self.name+'full')
        self.thresholdValues.append(thrv)
        return self

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        if self.ttype == ThrType.EM:
            confObj["thrValues"] = []
            for thrV in self.thresholdValues:
                confObj["thrValues"].append( odict([
                    ("value", thrV.value),
                    ("isobits", thrV.isobits),
                    ("etamin", thrV.etamin),
                    ("etamax", thrV.etamax),
                    ("phimin", thrV.phimax),
                    ("phimax", thrV.phimin),
                    ("priority", thrV.priority)
                ]) )
        elif self.ttype == ThrType.TAU:
            confObj["value"] = self.thresholdValues[0].value
            confObj["isobits"] = self.thresholdValues[0].isobits
        elif self.ttype == ThrType.JET:
            confObj["thrValues"] = []
            for thrV in self.thresholdValues:
                confObj["thrValues"].append( odict([
                    ("value", thrV.value),
                    ("etamin", thrV.etamin),
                    ("etamax", thrV.etamax),
                    ("phimin", thrV.phimax),
                    ("phimax", thrV.phimin),
                    ("window", thrV.window),
                    ("priority", thrV.priority)
                ]) )
        elif self.ttype == ThrType.TE:
            if len(self.thresholdValues)==1:
                confObj["value"] = self.thresholdValues[0].value
            else:
                confObj["thrValues"] = []
                for thrV in self.thresholdValues:
                    confObj["thrValues"].append( odict([
                        ("value", thrV.value),
                        ("etamin", thrV.etamin),
                        ("etamax", thrV.etamax),
                        ("priority", thrV.priority)
                    ]) )
        elif self.ttype == ThrType.XE:
            confObj["value"] = self.thresholdValues[0].value
        elif self.ttype == ThrType.XS:
            confObj["value"] = self.thresholdValues[0].value
        else:
            raise RuntimeError("No json implementation for legacy threshold type %s" % self.ttype)
        return confObj



class EMThreshold (Threshold):
    
    def __init__(self, name, ttype = 'eEM', mapping = -1):
        super(EMThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 3 if ttype=='eEM' else 2)
        mres = re.match("(?P<type>[A-z]*)[0-9]*(?P<suffix>[VHI]*)",name).groupdict()
        self.suffix = mres["suffix"]
        self.rhad = "None"
        self.reta = "None"
        self.wstot = "None"

    def isV(self):
        return 'V' in self.suffix

    def isI(self):
        return 'I' in self.suffix

    def setIsolation(self, rhad = "None", reta = "None", wstot = "None"):
        allowed = [ "None", "Loose", "Medium", "Tight" ]
        if rhad not in allowed:
            raise RuntimeError("Threshold %s of type %s: isolation wp %s not allowed for rhad, must be one of %s", self.name, self.ttype, rhad, ', '.join(allowed) )
        self.rhad = rhad
        self.reta = reta
        self.wstot = wstot
        return self

    def addThrValue(self, value, *args, **kwargs):
        # supporting both EM and TAU
        defargs = ThresholdValue.getDefaults(self.ttype.name) 
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'em_isolation', 'had_isolation', 'had_veto', 'priority', 'isobits', 'use_relIso'], args))
        
        # then we evaluate the arguments: first defaults, then positional arguments, then named arguments
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)

        thrv = ThresholdValue(self.ttype, value,
                              etamin = p['etamin'], etamax=p['etamax'], phimin=p['phimin'], phimax=p['phimax'],
                              priority = p['priority'], name = self.name+'full')

        thrv.setIsolation( em_isolation = p['em_isolation'],
                           had_isolation = p['had_isolation'],
                           had_veto = p['had_veto'],
                           isobits = p['isobits'],
                           use_relIso = p['use_relIso'])     
        self.thresholdValues.append(thrv)            
        return self

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        confObj["rhad"] = self.rhad
        confObj["reta"] = self.reta
        confObj["wstot"] = self.wstot
        confObj["thrValues"] = []
        for thrV in self.thresholdValues:
            tvco = odict()
            tvco["value"] = thrV.value
            tvco["etamin"] = thrV.etamin
            tvco["etamax"] = thrV.etamax
            tvco["priority"] = thrV.priority
            confObj["thrValues"].append( tvco )
        return confObj


class MuonThreshold( Threshold ):

    def __init__(self, name, run = 3, tgcFlags = "", mapping = -1):
        super(MuonThreshold,self).__init__(name = name, ttype = 'MU', mapping = mapping, run = run)
        self.thr = None
        self.baThr = None
        self.ecThr = None
        self.fwThr = None
        self.thrIdx = None
        self.baIdx = None
        self.ecIdx = None
        self.fwIdx = None
        self.tgcFlags = ""
        self.region = "ALL"
        self.rpcExclROIList = None

    def setRegion(self,det):
        """@det can be any combination of 'BA','EC',FW' or can be 'ALL' (case doesn't matter)"""
        tmp = set([x.strip() for x in det.split(',')])
        for x in tmp:
            if not x.upper() in ['BA','EC','FW','ALL']:
                raise RuntimeError("Unknown detector specification %s for muon threshold %s" % (x, self.name))
        tmp = sorted([x.upper() for x in tmp])
        if 'ALL' in tmp:
            tmp = ['ALL']
        self.region = ','.join(tmp)
        return self

    def setTGCFlags(self, flags):
        """flags can be a logical expression like 'F & C | F & H | C & H'"""
        self.tgcFlags = flags
        return self

    def setExclusionList(self, exclusionList):
        self.rpcExclROIList = exclusionList

    def setThrValue(self, thr = None, ba = None, ec = None, fw = None):
        """
        pT parameters thr, ba, ec, fw are in GeV
        Specifying thr sets all: ba, ec, and fw to that value
        ba, ec, fw can then be used to overwrite it for a certain region
        """
        self.thr = thr
        self.baThr = thr
        self.ecThr = thr
        self.fwThr = thr
        if ba is not None:
            self.baThr = ba
        if ec is not None:
            self.ecThr = ec
        if fw is not None:
            self.fwThr = fw
        if self.baThr is None or self.ecThr is None or self.fwThr is None:
            raise RuntimeError("In muon threshold %s setThrValue() at least one region is unspecified" % self.name)

        # set the threshold index from the pT value
        muonRoads = getTypeWideThresholdConfig(ThrType.MU)["roads"]
        if self.baThr in muonRoads["rpc"]:
            self.baIdx = muonRoads["rpc"][self.baThr]
        else:
            raise RuntimeError("Muon PT threshold %i does not have a defined road in the barrel" % self.baThr)
        if self.ecThr in muonRoads["tgc"]:
            self.ecIdx = muonRoads["tgc"][self.ecThr]
        else:
            raise RuntimeError("Muon PT threshold %i does not have a defined road in the endcap" % self.ecThr)
        if self.fwThr in muonRoads["tgc"]:
            self.fwIdx = muonRoads["tgc"][self.fwThr]
        else:
            raise RuntimeError("Muon PT threshold %i does not have a defined road in the endcap" % self.fwThr)
        return self

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        if self.isLegacy():
            confObj["thr"] = self.thr
        else:
            confObj["baThr"] = self.baThr
            confObj["ecThr"] = self.ecThr
            confObj["fwThr"] = self.fwThr
            confObj["baIdx"] = self.baIdx
            confObj["ecIdx"] = self.ecIdx
            confObj["fwIdx"] = self.fwIdx
            confObj["tgcFlags"] = self.tgcFlags
            confObj["region"] = self.region
            if self.rpcExclROIList:
                confObj["rpcExclROIList"] = self.rpcExclROIList
        return confObj



class TauThreshold( Threshold ):

    def __init__(self, name, ttype = 'eTAU', mapping = -1):
        super(TauThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 3 if ttype=='eTAU' else 2)
        self.et = None

    def setEt(self, et):
        self.et = et

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        confObj["value"] = self.et
        return confObj


class JetThreshold( Threshold ):

    def __init__(self, name, ttype = 'jJ', mapping = -1):
        super(JetThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 3 if ttype=='jJ' else 2)
        self.pt = None
        self.ranges = [] # full range if empty

    def setPt(self,pt):
        """sets pt value"""
        self.pt = pt
        return self

    def addRange(self,etamin, etamax):
        """
        range for which the pt is valid
        outside those the threshold is not defined
        """
        etamin, etamax = sorted([etamin,etamax])
        self.ranges += [ odict([("etamin", etamin),("etamax", etamax)]) ]
        return self

    def json(self):
        confObj = odict()
        confObj["value"] = self.pt
        confObj["mapping"] = self.mapping
        if len(self.ranges)==0:
            confObj["ranges"] = [ odict([("etamin", -49),("etamax", 49)]) ]
        else:
            confObj["ranges"] = self.ranges

        return confObj



class XEThreshold( Threshold ):

    def __init__(self, name, ttype, mapping = -1):
        super(XEThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 3 if ttype.startswith('gXE') or ttype.startswith('jXE') else 2)
        self.xe = None

    def setXE(self, xe):
        """xe value in GeV"""
        self.xe = xe
        return self

    def json(self):
        confObj = odict()
        confObj["value"] = self.xe
        confObj["mapping"] = self.mapping
        return confObj


class NimThreshold( Threshold ):

    def __init__(self, name, ttype, mapping = -1):
        super(NimThreshold,self).__init__(name = name, ttype = ttype, mapping = mapping, run = 3)

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        return confObj


class MBTSSIThreshold( Threshold ):

    def __init__(self, name, mapping = -1):
        super(MBTSSIThreshold,self).__init__(name = name, ttype = 'MBTSSI', mapping = mapping, run = 3)
        self.voltage = None

    def setVoltage(self, voltage):
        self.voltage = voltage

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        confObj["voltage"] = self.voltage
        return confObj


class MBTSThreshold( Threshold ):

    def __init__(self, name, mapping = -1):
        super(MBTSThreshold,self).__init__(name = name, ttype = 'MBTS', mapping = mapping, run = 3)
        self.sectors = []

    def addSector(self, mbtsSector):
        self.sectors += [ mbtsSector.name ]

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        confObj["sectors"] = self.sectors
        return confObj



class ZeroBiasThreshold( Threshold ):

    __slots__ = [ 'seed','seed_ttype', 'seed_multi', 'bcdelay' ]

    def __init__(self, name, mapping = -1):
        super(ZeroBiasThreshold,self).__init__(name = name, ttype = 'ZB', mapping = mapping, run = 2)
        self.seed       = ''
        self.seed_ttype = ''
        self.seed_multi = 0
        self.bcdelay    = 0

    def setSeedThreshold(self, seed='', seed_multi = 1, bcdelay = 3564 ):
        self.seed       = seed
        self.seed_multi = int(seed_multi)
        self.bcdelay    = int(bcdelay)
        

    def json(self):
        confObj = odict()
        confObj["mapping" ]         = self.mapping
        confObj["seed"]             = self.seed
        confObj["seedMultiplicity"] = self.seed_multi
        confObj["seedBcdelay"]      = self.bcdelay
        return confObj




class ThresholdValue(object):

    defaultThresholdValues = {}
    
    @staticmethod
    def setDefaults(ttype, dic):
        ThresholdValue.defaultThresholdValues[ttype] = dic
        
    @staticmethod
    def getDefaults(ttype):
        defaults = {
            'etamin'  : -49,
            'etamax'  :  49,
            'phimin'  :   0,
            'phimax'  :  64,
            'priority':   0,
            }
        if ttype == 'EM' or ttype == 'TAU' or ttype == 'eEM' or ttype == 'eTAU':
            defaults.update({'priority':   1,
                             'em_isolation' : CL.IsolationOff,
                             'had_isolation' : CL.IsolationOff,
                             'had_veto' : CL.IsolationOff,
                             'isobits' : '00000',
                             'use_relIso' : False,
                             })
        if ttype == 'JET':
            defaults.update({'window': 8})

        if ttype in ThresholdValue.defaultThresholdValues: # user defined
            defaults.update( ThresholdValue.defaultThresholdValues[ttype] )

        return defaults

    def __init__(self, thrtype, value, etamin, etamax, phimin, phimax, window=0, priority=1, name=''):
        self.name   = name
        self.type   = thrtype
        self.value  = value
        self.etamin = etamin
        self.etamax = etamax
        self.phimin = phimin
        self.phimax = phimax
        self.em_isolation = CL.IsolationOff
        self.had_isolation = CL.IsolationOff
        self.had_veto = CL.IsolationOff
        self.window = window
        self.priority = priority

    def checkOverlapAny(self, listOfThrValues):
        for rv in listOfThrValues:
            if rv.priority != self.priority:
                continue
            if (self.etamax > rv.etamin) and (self.etamin < rv.etamax):
                # overlaps with existing range of the same priority
                raise RuntimeError( "ThresholdValue %s: Range eta %i - %i (priority %i) overlaps with existing range of the same priority" % \
                                    (self.name, self.etamin, self.etamax, self.priority) )

    def setIsolation(self, em_isolation, had_isolation, had_veto, isobits, use_relIso):
        self.em_isolation = em_isolation
        self.had_isolation = had_isolation
        self.had_veto = had_veto
        self.isobits = isobits
        self.use_relIso = use_relIso
        if self.use_relIso:
            self.had_veto=99

    def __cmp__(self, o):
        if(self.priority!=o.priority):
            return cmp(self.priority,o.priority)
        if(self.etamin!=o.etamin):
            return cmp(self.etamin,o.etamin)
        return cmp(self.name,o.name)


    def __str__(self):
        return "name=%s, value=%s, eta=(%s-%s)" % (self.name, self.value, self.etamin, self.etamax)


class TopoThreshold( Threshold ):
    """Class representing a direct input cable to the CTPCORE
    
    In the menu it is treated like a threshold, only the naming
    convention is less strict (allows"-" and can start with a number)
    """

    import re
    multibitPattern = re.compile(r"(?P<line>.*)\[(?P<bit>\d+)\]")

    def __init__(self, name, algCategory, mapping = None):
        """
        @param category of type AlgCategory 
        """
        if ','  in name:
            raise RuntimeError("%s is not a valid topo output name, it should not contain a ','" % name)
        self.algCategory = algCategory
        super(TopoThreshold,self).__init__(name = name, ttype = algCategory.key, run = 2 if algCategory==AlgCategory.LEGACY else 3)
        if algCategory not in AlgCategory.getAllCategories():
            raise RuntimeError("%r is not a valid topo category" % algCategory)



    def getVarName(self):
        """returns a string that can be used as a varname"""
        return self.name.replace('.','').replace('-','_') # we can not have '.' or '-' in the variable name

    def json(self):
        confObj = odict()
        confObj["mapping"] = self.mapping
        return confObj
