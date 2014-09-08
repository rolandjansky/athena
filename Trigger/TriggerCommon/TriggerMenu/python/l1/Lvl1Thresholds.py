# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Limits import CaloLimits as CL
IsolationOff = CL.IsolationOff

from Lvl1MenuUtil import log
from copy import deepcopy

class ThresholdValue:

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
        if ttype == 'EM':
            defaults.update({'priority':   1,
                             'em_isolation' : IsolationOff,
                             'had_isolation' : IsolationOff,
                             'had_veto' : IsolationOff,
                             })
        if ttype == 'JET':
            defaults.update({'window': 8})

        if ttype in ThresholdValue.defaultThresholdValues: # user defined
            defaults.update( ThresholdValue.defaultThresholdValues[ttype] )

        return defaults

    def __init__(self, thrtype, value, etamin, etamax, phimin, phimax,
                 em_isolation = IsolationOff, had_isolation = IsolationOff, had_veto = IsolationOff,
                 window=0, priority=1, name=''):
        self.name   = name
        self.type   = thrtype
        self.value  = value
        self.etamin = etamin
        self.etamax = etamax
        self.phimin = phimin
        self.phimax = phimax
        self.em_isolation = em_isolation
        self.had_isolation = had_isolation
        self.had_veto = had_veto
        self.window = window
        self.priority = priority

    def xml(self, ind=1, step=2):
        s  = ind * step * ' ' + '<TriggerThresholdValue em_isolation="%i" etamin="%i" etamax="%i" had_isolation="%i" had_veto="%i" ' % (self.em_isolation, self.etamin, self.etamax, self.had_isolation, self.had_veto)
        s += 'name="%s" phimin="%i" phimax="%i" priority="%i" thresholdval="%g" type="%s" window="%i"/>\n' % (self.name, self.phimin, self.phimax, self.priority, self.value, self.type, self.window)
        return s

    def __str__(self):
        return "name=%s, value=%s, eta=(%s-%s)" % (self.name, self.value, self.etamin, self.etamax)


class LVL1Threshold(object):
    __slots__ = ['name','ttype','mapping','active','seed','seed_ttype', 'seed_multi', 'bcdelay', 'cableinfo', 'thresholdValues']

    def __init__(self, name, ttype, mapping = -1, active = 1, seed='', seed_type = '', seed_multi = 0, bcdelay = 0):
        self.name        = name
        self.ttype       = ttype
        self.mapping     = int(mapping)
        self.active      = int(active)
        self.seed        = seed
        self.seed_ttype  = seed_type
        self.seed_multi  = int(seed_multi)
        self.bcdelay     = int(bcdelay)
        self.cableinfo       = None
        self.thresholdValues = []

    def __str__(self):
        return self.name


    def setCableInput(self):
        from Cabling import Cabling
        self.cableinfo = Cabling.getInputCable(self.ttype, self.mapping, self.seed_ttype)
        return self


    def addThrValue(self, value, *args, **kwargs):
        if self.ttype == 'MUON':
            return self.addMuonThresholdValue(value, *args, **kwargs)
        if self.ttype == 'EM' or self.ttype == 'TAU':
            return self.addEMThresholdValue(value, *args, **kwargs)
        if self.ttype == 'JET' or self.ttype == 'JF' or self.ttype == 'JB':
            return self.addJetThresholdValue(value, *args, **kwargs)
        if self.ttype in ['JE', 'TE', 'XE', 'XS', 'CALREQ', 'MBTSSI', 'TRT', 'ZDC', 'BCM', 'BCMCMB', 'LUCID', 'NIM']:
            return self.addEnergyThresholdValue(value, *args, **kwargs)
        raise RuntimeError("addThrValue() not defined for threshold type %s" % self.ttype)


    def addMuonThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults('MUON')
        # during the transition period we will need to keep supporting
        # formerly positional arguments, which were etamin, etamax, phimin, and phimax
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax'], args))
        # then we evaluate the arguments: first defaults, then positional arguments, then named arguments
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        
        thrv = ThresholdValue(self.ttype, value, etamin = p['etamin'], etamax=p['etamax'], phimin=p['phimin'], phimax=p['phimax'],
                              priority = p['priority'], name = self.name+'full')

        self.thresholdValues.append(thrv)
        return self


    def addEMThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults('EM')
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'em_isolation', 'had_isolation', 'had_veto', 'priority'], args))
        
        # then we evaluate the arguments: first defaults, then positional arguments, then named arguments
        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        thrv = ThresholdValue(self.ttype, value,
                              etamin = p['etamin'], etamax=p['etamax'], phimin=p['phimin'], phimax=p['phimax'],
                              em_isolation = p['em_isolation'], had_isolation = p['had_isolation'], had_veto = p['had_veto'],
                              priority = p['priority'], name = self.name+'full')
        self.thresholdValues.append(thrv)
        return self



    def addJetThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults(self.ttype)
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'window', 'priority'], args))

        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)
        
        thrv = ThresholdValue(self.ttype, value, etamin=p['etamin'], etamax=p['etamax'], phimin=p['phimin'], phimax=p['phimax'],
                              window=p['window'], priority=p['priority'], name=self.name+'full')
        self.thresholdValues.append(thrv)
        return self


    def addEnergyThresholdValue(self, value, *args, **kwargs):
        defargs = ThresholdValue.getDefaults(self.ttype)
        posargs = dict(zip(['etamin', 'etamax', 'phimin', 'phimax', 'priority'], args))

        p = deepcopy(defargs)
        p.update(posargs)
        p.update(kwargs)

        thrv = ThresholdValue(self.ttype, value, etamin=p['etamin'], etamax=p['etamax'], phimin=p['phimin'], phimax=p['phimax'],
                              priority=p['priority'], name=self.name+'full')
        self.thresholdValues.append(thrv)
        return self

    def thresholdInGeV(self):
        if len(self.thresholdValues)==0:
            return 0
        else:
            return float(self.thresholdValues[0].value)

    def xml(self, ind=1, step=2):
        """ Returns XML representation of the LVL1 Threshold """
        from Lvl1MenuUtil import idgen
        seed       = ' seed="%s"' % self.seed if self.ttype=='ZB' else ''
        seed_multi = ' seed_multi="%i"' % self.seed_multi if self.ttype=='ZB' else ''
        bcdelay    = ' bcdelay="%i"' % self.bcdelay if self.ttype=='ZB' else ''
        inputboard = "ctpcore" if self.cableinfo.isDirectIn else "ctpin"
        s = ind * step * ' ' + '<TriggerThreshold active="%i" bitnum="%i" id="%i" mapping="%i" name="%s" type="%s" input="%s"%s%s%s version="1">\n' % \
            (self.active, self.cableinfo.bitnum, int(idgen.get('TriggerThreshold')), self.mapping, self.name, self.ttype, inputboard, seed, seed_multi, bcdelay)
        for thrv in self.thresholdValues:
            s += thrv.xml(ind+1,step)
        if self.cableinfo.isDirectIn:
            s += (ind+1) * step * ' ' + '<Cable connector="%s" input="CTPCORE" name="%s">\n' % (self.cableinfo.connector, self.cableinfo.name)
            s += (ind+2) * step * ' ' + '<Signal range_begin="%i" range_end="%i" clock="%i"/>\n' % (self.cableinfo.range_begin, self.cableinfo.range_end, self.cableinfo.clock)
        else:
            s += (ind+1) * step * ' ' + '<Cable connector="%s" input="%s" ctpin="%s" name="%s">\n' % (self.cableinfo.connector, self.cableinfo.slot, self.cableinfo.slot, self.cableinfo.name)
            s += (ind+2) * step * ' ' + '<Signal range_begin="%i" range_end="%i"/>\n' % (self.cableinfo.range_begin, self.cableinfo.range_end)
        s += (ind+1) * step * ' ' + '</Cable>\n'
        s += ind * step * ' ' + '</TriggerThreshold>\n'
        return s





class LVL1Thresholds:

    @staticmethod
    def cableMapping(type, range_begin):
        """Calculate the mapping of the input cable of the threshold to the
        CTP from its (type, range_begin)"""
        mapping = -1
        if type == 'MUON':
            mapping = range_begin/3
        elif type=='EM':
            mapping = range_begin/3
        elif type=='TAU':
            mapping = range_begin/3
        elif type=='JET':
            mapping = range_begin/3
        elif type=='JE':
            mapping = range_begin-24
        elif type=='JB' or type=='FJL':
            mapping = range_begin/2
        elif type=='JF' or type=='FJR':
            mapping = (range_begin-8)/2
        elif type=='TE':
            mapping = range_begin
        elif type=='XE':
            mapping = range_begin-4
        return mapping


    @staticmethod
    def compThreshold(thr1, thr2):
        """
        Comparison of two LVL1Thresholds for the purpose of sorting
        """
        # First sort by type
        if thr1.ttype!= thr2.ttype:
            return cmp(thr1.ttype,thr2.ttype)

        # Second sort by mapping if it's not -1
        if thr1.mapping >= 0 or thr2.mapping >= 0:
            return cmp(thr1.mapping,thr2.mapping)

        # If both mappings are -1 sort by threshold value and then threshold name
        import re
        thrv1, thrv2 = 0, 0
        re_thrv = re.compile('(\d+)')
        mg = re_thrv.search(thr1.name)
        if mg: thrv1 = int(mg.group(1))
        mg = re_thrv.search(thr2.name)
        if mg: thrv2 = int(mg.group(1))

        if thrv1 != thrv2:
            return cmp(thrv1,thrv2)

        return cmp(thr1.name, thr2.name)

    # LVL1Thresholds constructor
    def __init__(self):
        self.thresholds = []

    def __iter__(self):
        return self.thresholds.__iter__()

    def __call__(self):
        return self.thresholds

    def __iadd__(self, thr):
        if thr is None: return self
        if self.thresholdOfName(thr.name) == None:
            self.thresholds += [ thr ]
        return self
    
    def __contains__(self, thrname):
        return thrname in [x.name for x in self.thresholds] 
    
    def __len__(self):
        return len(self.thresholds)

    def thresholdNames(self):
        return [x.name for x in self.thresholds]

    def allThresholdsOf(self, type):
        return [thr for thr in self.thresholds if thr.ttype==type]

    def thresholdOfName(self, name):
        for thr in self.thresholds:
            if thr.name == name:
                return thr
        return None

    def xml(self, ind=1, step=2):
        self.thresholds.sort(LVL1Thresholds.compThreshold)
        #self.thresholds.sort()
        s = ind * step * ' ' + '<TriggerThresholdList>\n'
        for thr in self.thresholds:
            s += thr.xml(ind+1,step)
        s += ind * step * ' ' + '</TriggerThresholdList>\n'
        return s

