# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import time
import re
import string
from sys import settrace
import xml.dom.minidom
from AthenaCommon.Logging import logging  # loads logger


def logger():
    return logging.getLogger( "LVL1Config" )

# unique id generator
class idgen:
    a =  {}
    def get(self, kind):
        if kind not in self.a:
            self.a[kind] = 0
        self.a[kind] += 1
        return str(self.a[kind])

idgen = idgen()


#########################################################################################
# LVL1

class Logic:
    NONE = ''
    AND = '&'
    OR = '|'
    NOT = '!'

    def symbolToString(x):
        if x==Logic.AND:
            return 'AND'
        elif x==Logic.OR:
            return 'OR'
        elif x==Logic.NOT:
            return 'NOT'
        else:
            return ''
    symbolToString = staticmethod(symbolToString)

    def __init__(self, condition=None):
        self.subLogic = Logic.NONE
        self.condition = '' # hold a Lvl1Condition instance
        self.subConditions = [] # holds Logic instances
        if condition!=None and condition!='':
            self.condition = condition

    def __or__(self, x):
        a = Logic()
        if self.subLogic == Logic.OR:
            a = self
        else:
            a = Logic()
            a.subLogic = Logic.OR
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a
    def __and__(self, x):
        a = Logic()
        if self.subLogic == Logic.AND:
            a = self
        else:
            a = Logic()
            a.subLogic = Logic.AND
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a
    def __not__(self, x):
        logger().debug('checking NOT')
        a = self
        if a.subLogic==Logic.NONE:
            a.subLogic = Logic.NOT
        if a.subLogic==Logic.NOT:
            if len(a.subConditions) == 1:
                logger().debug('not is a unary operator, ignore it')
            else:
                a.subConditions.append(x)
        return a

    def __str__(self):
        s = ''
        if self.subLogic == Logic.NONE:
            if len(self.subConditions)==1:
                s += str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                logger().error('Logic NONE has more than one element')
                s += ''
            elif len(self.subConditions)==0 and self.condition!='':
                s += str(self.condition)
        elif self.subLogic == Logic.NOT:
            if len(self.subConditions)==1:
                s += '!'+str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                logger().error('Logic NOT has more than one element')
                s += ''
        elif self.subLogic == Logic.AND or self.subLogic == Logic.OR:
            if len(self.subConditions)==1:
                s += str(self.subConditions[0])
            else:
                s += '('
                for (i, a) in enumerate(self.subConditions):
                    if i > 0: s += self.subLogic
                    s += str(a)
                s +=')'
        return s
    def thresholdNames(self, include_bgrp=False):
        names = []
        if self.subLogic == Logic.NONE or \
               (self.condition!=None and self.condition!=''):
            if self.condition!=None:
                if isinstance(self.condition,LVL1MenuItem.Lvl1InternalTrigger):
                    if include_bgrp:
                        #print 'InternalThreahold: %s' % self.condition.name
                        if self.condition.name!=None and \
                               self.condition.name not in names:
                            names.append(self.condition.name)
                elif self.condition.threshold!=None and \
                       self.condition.threshold.name not in names:
                    names.append(self.condition.threshold.name)
        else:
            for a in self.subConditions:
                for b in a.thresholdNames(include_bgrp):
                    if b not in names: names.append(b)
        return names
    def normalize(self):
        if self.subLogic in (Logic.AND, Logic.OR):
            mylogic = self.subLogic
            newconditions = []
            for c in self.subConditions:
                if c.subLogic == mylogic: # X&(A&B) or X|(A|B) 
                    # expand it to X&A&B or X|A|B
                    c.normalize()
                    newconditions.extend(c.subConditions)
                else:
                    newconditions.append(c)
            self.subConditions = newconditions
        else:
            pass
        pass
    def xml(self, node):
        if self.subLogic==Logic.NONE and \
               isinstance(self.condition, LVL1MenuItem.Lvl1InternalTrigger):
            e = xml.dom.minidom.Document().createElement('InternalTrigger')
            e.setAttribute('name', self.condition.name)
            node.appendChild(e)
        elif self.subLogic==Logic.NONE and \
             isinstance(self.condition, LVL1MenuItem.Lvl1Condition):
            e = xml.dom.minidom.Document().createElement('TriggerCondition')
            e.setAttribute('name', self.condition.threshold.name+'_x'+\
                           str(self.condition.multiplicity))
            e.setAttribute('triggerthreshold', self.condition.threshold.name)
            e.setAttribute('multi', str(self.condition.multiplicity))
            node.appendChild(e)
        elif self.subLogic==Logic.AND:
            e = xml.dom.minidom.Document().createElement('AND')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        elif self.subLogic==Logic.OR:
            e = xml.dom.minidom.Document().createElement('OR')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        elif self.subLogic==Logic.NOT:
            e = xml.dom.minidom.Document().createElement('NOT')
            node.appendChild(e)
            for s in self.subConditions:
                s.xml(e)
        else:
            logger().error('Unknown node in LVL1 item logic')
            logger().error('  ==> sublogic = ', self.subLogic)
            logger().error('  ==> # subConditions = ', len(self.subConditions))

    def printIt(self):
        for a in self.subConditions:
            if a.subLogic==a.NONE and a.condition!='':
                logger().info('subCondition :', str(a.condition))
            else:
                logger().info('subCondition :', a.printIt())
        return ''

    def dumpXML(self, indent=''):
        if self.subLogic==Logic.NONE:
            if len(self.subConditions)==0:
                logger().info(indent + '<TriggerCondition ' + str(self.condition) + '>')
        else:
            logger().info(indent + '<' + Logic.symbolToString(self.subLogic) + '>')
            for a in self.subConditions:
                a.dumpXML(indent+'  ')
            logger().info(indent + '</' + Logic.symbolToString(self.subLogic) + '>')
        return ''

    def Not(x):
        a = Logic()
        a.subLogic = Logic.NOT
        a.subConditions.append(x)
        return a
    Not = staticmethod(Not)
def Not(x):
    return Logic.Not(x)

class LVL1MenuItem:
    class Lvl1Condition:
        def __init__(self, threshold, multi):
            self.threshold = threshold
            self.multiplicity = multi
        def __str__(self):
            return str(self.threshold) + '[x' + str(self.multiplicity)+']'

    class Lvl1InternalTrigger(Lvl1Condition):
        def __init__(self, name):
            self.name = name
        def xml(self, x):
            internaltrig = xml.dom.minidom.Element('InternalTrigger')
            internaltrig.setAttribute('name', self.name)
            x.appendChild(internaltrig)
        def __str__(self):
            return str(self.name)

    def __init__(self, name, ctpid, group='1', prescale='1', priority='HIGH'):
        self.name      = name
        self.group     = group
        self.ctpid     = str(ctpid)
        self.prescale  = str(prescale)
        self.priority  = str(priority)
        self.logic = None
        self.trigger_type = 0
        
    def setLogic(self, x):
        self.logic = x
        return self

    def setCtpid(self, x):
        self.ctpid = str(x)
        return self

    def thresholdNames(self, include_bgrp=False):
        if self.logic!=None:
            return self.logic.thresholdNames(include_bgrp)
        else:
            return []

    def setTriggerType(self, n):
        ttype = int(n)
        ttype = ttype & 0xff
        self.trigger_type = ttype
        return self
    
    def addCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        return self.addAndedCondition(name, multi, threshold)

    def addOredCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        tm = TriggerPythonConfig.currentTriggerConfig()
        ok = False
        if tm != None:
            thr = tm.theLVL1Thresholds.thresholdOfName(name)
            if thr != None:
                if self.logic != None:
                    if self.logic.subLogic == Logic.OR or \
                           self.logic.subLogic == Logic.NONE:
                        self.logic = self.logic|Logic(thr.lvl1Condition(multi))
                        ok = True
                else:
                    self.setLogic(Logic(thr.lvl1Condition(multi)))
                    ok = True
        if not ok:
            logger().error('Error while trying to add ORed Condition')
            if self.logic==None:
                logger().error('Logic is not defined for this item')
            else:
                logger().error('Already setup logic: ')
                self.logic.printIt()
            logger().error('Tried to add ORed condition of ' + threshold + \
                  " multi=" + str(multi))
        return self

    def addAndedCondition(self, name, multi, threshold=None):
        """Obsolete method, use setLogic"""
        tm = TriggerPythonConfig.currentTriggerConfig()
        ok = False
        if tm != None:
            thr = tm.theLVL1Thresholds.thresholdOfName(name)
            if thr != None:
                if self.logic != None:
                    if self.logic.subLogic == Logic.AND or \
                           self.logic.subLogic == Logic.NONE:
                        self.logic = self.logic|Logic(thr.lvl1Condition(multi))
                        ok = True
                else:
                    self.setLogic(Logic(thr.lvl1Condition(multi)))
                    ok = True
        if not ok:
            logger().error('Error while trying to add ANDed Condition')
            if self.logic==None:
                logger().error('Logic is not defined for this item')
            else:
                logger().error('Already setup logic: ')
                self.logic.printIt()
            logger().error('Tried to add ANDed condition of %s multi=%s' % (threshold, multi))
        return self

    def binary_trigger_type(self, x):
        TT=''
        for i in range(0,8):
            if(x%2==1):
                TT='1'+TT
            else:
                TT='0'+TT
            x=x/2
        return TT

    def xml(self, xmenu):
        xtriggeritem = xml.dom.minidom.Document().createElement('TriggerItem')
        xtriggeritem.setAttribute('name', self.name)
        xtriggeritem.setAttribute('ctpid', self.ctpid)
        xtriggeritem.setAttribute('version', '1')
        xtriggeritem.setAttribute('definition', str(self.logic))
        xtriggeritem.setAttribute('trigger_type', self.binary_trigger_type(self.trigger_type))
        xtriggeritem.setAttribute('comment','')
        xtriggeritem.setAttribute('group','0')
        xmenu.appendChild(xtriggeritem);
        if self.logic!=None:
            self.logic.normalize()
            self.logic.xml(xtriggeritem)

class LVL1MenuItems:
    def __init__(self):
        self.items = []

    def itemNames(self):
        return map( lambda x: x.name, self.items )

    def findItemByName(self, name):
        items = filter(lambda x: x.name==name, self.items)
        if len(items)==0: return None
        else: return items[0]

    def findItemByCtpid(self, ctpid):
        items = filter(lambda x: x.ctpid==ctpid, self.items)
        if len(items)==0: return None
        else: return items[0]

    def addItem(self, itemname, mult='1', prescale='1', mask='on', \
                threshold=None):
        self.items.append( LVL1MenuItem(itemname, mult, prescale, mask, threshold) )

    @classmethod
    def getNMDPS(cls, PS):
        """
        floatPS is the raw float as entered into the python. This returns n | m | d where
        n is a 24bit positive number INTEGER PRESCALE
        m,d are 4bit positive integer used for fractional prescale.

        The fractional prescale is calculated as: f = n * (m+d+1)/(m+1)

        !! This function is not implemented correctly: n=4,m=9,d=3 describes a prescale f=5.2, this function return n=5,m=15,d=1 for PS=5.2 (which calculates a prescale f=5.3125)
        !! use getNMDfromPS instead
        """
        
        #List with all possible prescaleValues between 1 and 2. Used to calculate the
        #N,M,D combination out of a float prescale.
        L1PSNumberAuxValues = [[15,1],[14,1],[13,1],[12,1],[11,1],[10,1],[9,1],[8,1],[15,2],
                               [14,2],[13,2],[12,2],[11,2],[10,2],[15,3],[14,3],[13,3],[8,2],
                               [12, 3],[15, 4],[14, 4],[10, 3],[13, 4],[9, 3],[12, 4],[15, 5],
                               [14, 5],[13, 5],[10, 4],[15, 6],[12, 5],[14, 6],[11, 5],[13, 6],
                               [15, 7],[8, 4],[10, 5],[12, 6],[14, 7],[15, 8],[14, 8],[12, 7],
                               [10, 6],[8, 5],[15, 9],[13, 8],[11, 7],[14, 9],[12, 8],[15, 10],
                               [10, 7],[13, 9],[14, 10],[15, 11],[12, 9],[9, 7],[13, 10],[10, 8],
                               [14, 11],[15, 12],[12, 10],[8, 7],[13, 11],[14, 12],[15, 13],[10, 9],
                               [11, 10],[12, 11],[13, 12],[14, 13],[15, 14],[8, 8],[9, 9],[10, 10],
                               [11, 11],[12, 12],[13, 13],[14, 14],[15, 15],
                              ]

        floatPS = float(PS)
        # Only negative prescale allowed is -1 from Python
        if (floatPS < 0.0):
            n = -1
            m = 0
            d = 0
            f = -1
            return n,m,d,f

        # Now find n, m, d and allowed float ps for any float provided
        floatPScorr = 1.0
        tmpN = int(floatPS)
        tmpM = 0
        tmpD = 0
        diff = floatPS - tmpN
        psIdx = -1
        i=-1
        if (floatPS - tmpN != 0):
            if (floatPS <= 1):
                tmpN = 1
            else:
                for entry in L1PSNumberAuxValues: #pair is mm, dd
                    i += 1
                    floatPScorr = ( float(entry[0] + entry[1] + 1.0)) / (float(entry[0] + 1))
                    tmpF = tmpN * floatPScorr
                    if (tmpF - tmpN < 1.0):
                        tmpDiff = abs(floatPS - tmpF)
                        if (tmpDiff < diff):
                            diff = tmpDiff
                            psIdx = i
                        else:
                            break
                if psIdx >= 0:
                    tmpM = (L1PSNumberAuxValues[psIdx])[0]
                    tmpD = (L1PSNumberAuxValues[psIdx])[1]
                    #floatPScorr = tmpN * (float(tmpM + tmpD + 1.0)) / (float(tmpM + 1))
                    floatPScorr = float(tmpM + tmpD + 1.0) / (float(tmpM + 1.0))
                else:
                    floatPScorr = 1

        return tmpN, tmpM, tmpD, tmpN*float(tmpM+tmpD+1.0)/(float(tmpM+1.0))

    @classmethod
    def getNMDfromPS(cls, PS):
        """
        float PS is the raw float as entered into the python. This returns n | m | d where
        n is a 24bit positive number INTEGER PRESCALE
        m,d are 4bit positive integer used for fractional prescale.

        The fractional prescale is calculated as: f = n * (m+d+1)/(m+1)

        This function is implemented correctly: n=4,m=9,d=3 describes a prescale f=5.2, this function return n=4,m=9,d=3 for PS=5.2 (which calculates a prescale f=5.3125)
        """
        PS = float(PS)
        min_dev = 100
        for mm in range(16):
            dev = 0
            for dd in range(16):
                nn = int( float(PS)*(mm+1)/(mm+dd+1) )
                if PS == cls.getPSfromNMD(nn, mm, dd):
                    return nn, mm, dd, cls.getPSfromNMD(nn, mm, dd)

                nn = nn + 1
                dev = cls.getPSfromNMD(nn, mm, dd) - PS
                if dev<min_dev:
                    min_dev = dev
                    n = nn
                    m = mm
                    d = dd
                if dev == 0: break
            if dev == 0: break

        return n, m, d, cls.getPSfromNMD(n, m, d)


    @classmethod
    def getPSfromNMD(cls, n, m, d):
        """ Level 1 fractional prescale
        n, m, d:        Lvl1 hardware values
        return value:   floating number of prescale value

        value = n*(m+d+1)/float(m+1)
        """
        return (n * (m+d+1)) / float(m+1)


    @classmethod
    def possibleFractionalPrescales(cls):
        for v in range(10,100):
            v = v/10.
            n, m, d, f = cls.getNMDfromPS(str(v))
            print "%g => %g %s" % (v, f, ("" if v==f else "  --> no precise representation") )




    def xml(self, xconfig, smkName, pskName):

        xtriggermenu = xml.dom.minidom.Document().createElement('TriggerMenu')
        xtriggermenu.setAttribute('id', idgen.get('TriggerMenu'))
        xtriggermenu.setAttribute('name',smkName)
        xtriggermenu.setAttribute('version','1')
        xtriggermenu.setAttribute('phase','lumi')
        xconfig.appendChild(xtriggermenu)
	
        for i in self.items:
            i.xml(xtriggermenu)

        # write prescale array
        prescaleSet = map(lambda x: '1', range(256)) #1, 256))
        for i in map( lambda x: (x.ctpid, x.prescale), self.items ):
            prescaleSet[int(i[0])] = i[1]
        xprescaleset = xml.dom.minidom.Element('PrescaleSet')
        xprescaleset.setAttribute('name',pskName)
        xprescaleset.setAttribute('version','1')
        for i in range(256):# write prescale array
            prescale = xml.dom.minidom.Element('Prescale')
            # get float ps and convert to n,m,d and corrected float
            floatPS = prescaleSet[i]
            n,m,d,floatPScorr = LVL1MenuItems.getNMDfromPS(floatPS)
            prescale.setAttribute('ctpid', str(i))
            prescale.setAttribute('n', str(n))
            prescale.setAttribute('m', str(m))
            prescale.setAttribute('d', str(d))
            xtext = xml.dom.minidom.Text()
            if m==0 and d==0:
                # integer prescale
                xtext.replaceWholeText("%i" % floatPScorr)
            else:
                # non-integer prescale
                xtext.replaceWholeText(str(floatPScorr))
            prescale.appendChild(xtext)
            xprescaleset.appendChild(prescale)
        xconfig.appendChild(xprescaleset)

        # write priority array
        prioritySet = map(lambda x: 'LOW', range(256)) # 1, 256))
        for i in map( lambda x: (x.ctpid, x.priority), self.items ):
            prioritySet[int(i[0])] = i[1]
        xpriorityset = xml.dom.minidom.Element('PrioritySet')
        xpriorityset.setAttribute('name', 'standard')
        xpriorityset.setAttribute('version', '1')
        for i in range(256):
            priority = xml.dom.minidom.Element('Priority')
            priority.setAttribute('ctpid', str(i))
            xtext = xml.dom.minidom.Text()
            xtext.replaceWholeText(str(prioritySet[i]))
            priority.appendChild(xtext)
            xpriorityset.appendChild(priority)
        xconfig.appendChild(xpriorityset)

##############################################################################
# LVL1 Threshold classes
# Maximum values for calo thresholds to disable that threshold
#
ClusterOff   = 255
IsolationOff = 63
JetOff       = 1023
EtSumOff     = 2047
EtMissOff    = 2895
JetEtOff     = 13286
# InternalTriggers
bgrp0 = LVL1MenuItem.Lvl1InternalTrigger('BGRP0')
bgrp1 = LVL1MenuItem.Lvl1InternalTrigger('BGRP1')
bgrp2 = LVL1MenuItem.Lvl1InternalTrigger('BGRP2')
bgrp3 = LVL1MenuItem.Lvl1InternalTrigger('BGRP3')
bgrp4 = LVL1MenuItem.Lvl1InternalTrigger('BGRP4')
bgrp5 = LVL1MenuItem.Lvl1InternalTrigger('BGRP5')
bgrp6 = LVL1MenuItem.Lvl1InternalTrigger('BGRP6')
bgrp7 = LVL1MenuItem.Lvl1InternalTrigger('BGRP7')
rndm0 = LVL1MenuItem.Lvl1InternalTrigger('RNDM0')
rndm1 = LVL1MenuItem.Lvl1InternalTrigger('RNDM1')
pclk0 = LVL1MenuItem.Lvl1InternalTrigger('PCLK0')
pclk1 = LVL1MenuItem.Lvl1InternalTrigger('PCLK1')

class LVL1Thresholds:
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
    cableMapping = staticmethod(cableMapping)

    def cableName(type, mapping):
        type2cable = {'MUON': 'MU',
                      'EM': 'CP1',
                      'TAU': 'CP2',
                      'JET':'JEP1',
                      'JE':'JEP1',
                      'FJET':'JEP2',
                      'JB':'JEP2',
                      'JF':'JEP2',
                      'TE':'JEP3',
                      'XE':'JEP3',
                      'XS':'JEP3',
                      'LUCID': 'NIM',
                      'TRT': 'NIM',
                      'ZDC': 'NIM',
                           'BCM': 'NIM',
                      'BCMCMB': 'NIM',
                      'MBTSSI': 'NIM',
                      'MBTS': 'NIM',
                      'CALREQ': 'NIM',
                      'NIM': 'NIM',
                      }
        x = ''
        if type in type2cable.keys():
            x = type2cable[type]
            if type == 'EM' and mapping>=8: x = type2cable['TAU']
        return x
    cableName = staticmethod(cableName)
    
    class ThresholdScale:
        def __init__(self, value=None):
            self.value = '1.0'
            if value is not None: self.value = value

        def xml(self):
            line='<ThresholdScale caloscale="'
            line+=self.value
            line+='"/>'
            return line


    class ThresholdValue:
        #
        def __init__(self, type, value, etamin, etamax, phimin, phimax, \
                     em_isolation, had_isolation, had_veto, \
                     window, priority, name=''):
            self.name = name
            self.type = type
            self.value = value
            self.etamin = etamin
            self.etamax = etamax
            self.phimin = phimin
            self.phimax = phimax
            self.em_isolation = em_isolation
            self.had_isolation = had_isolation
            self.had_veto = had_veto
            self.window = window
            self.priority = priority

        def xml(self, xlist):
            e = xml.dom.minidom.Element('TriggerThresholdValue')
            e.setAttribute('name', self.name)
            e.setAttribute('type', self.type)
            e.setAttribute('thresholdval', str(self.value))
            e.setAttribute('etamin', str(self.etamin))
            e.setAttribute('etamax', str(self.etamax))
            e.setAttribute('phimin', str(self.phimin))
            e.setAttribute('phimax', str(self.phimax))
            e.setAttribute('em_isolation', str(self.em_isolation))
            e.setAttribute('had_isolation', str(self.had_isolation))
            e.setAttribute('had_veto', str(self.had_veto))
            e.setAttribute('window', str(self.window))
            e.setAttribute('priority', str(self.priority))
            xlist.appendChild(e)

    class LVL1Threshold:
        #
        def __init__(self, name, ttype, seed_ttype):
            self.name   = name
            self.ttype  = ttype
            #seed ttype for zb trigger only
            self.seed_ttype  = seed_ttype
            self.thresholdValues = []
            self.bitnum = 0
            self.active = 1
            self.mapping = -1
            self.cable = ''
            self.range_begin = -1
            self.range_end = -1
            self.slot = ''
            self.connector = ''
            self.type2cable    = {'MUON': 'MU',
                                  'EM': 'CP1',
                                  'TAU': 'CP2',
                                  'JET':'JEP1',
                                  'JE':'JEP1',
                                  'FJET':'JEP2',
                                  'JB':'JEP2',
                                  'JF':'JEP2',
                                  'TE':'JEP3',
                                  'XE':'JEP3',
                                  'XS':'JEP3',
                                  'LUCID': 'NIM',
                                  'TRT': 'NIM',
                                  'ZDC': 'NIM',
                                  'BCM': 'NIM',
                                  'BCMCMB': 'NIM',
                                  'MBTSSI': 'NIM',
                                  'MBTS': 'NIM',
                                  'CALREQ': 'NIM',
                                  'NIM': 'NIM'}

            if self.cable is None or self.cable=='':
                if ttype in self.type2cable.keys():
                    cable = self.type2cable[ttype]
                #if its zb cable is the seed cable
                if ttype == 'ZB':
                    cable = self.type2cable[seed_ttype]
                self.cable = cable

        def __str__(self):
            return self.name

        def lvl1Condition(self, multi):
            return LVL1MenuItem.Lvl1Condition(self, multi)

        def condition(self, multi=1):
            return Logic(self.lvl1Condition(multi))

        def setCableInput(self, bitnum, range_begin, range_end, \
                          slot, connector):
            self.bitnum = int(bitnum)
            self.range_begin = int(range_begin)
            self.range_end = int(range_end)
            self.slot = slot
            self.connector = connector
            return self

        def addMuonThresholdValue(self, value, \
                                  etamin, etamax, phimin, phimax, \
                                  priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addCPThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                em_isolation, had_isolation, had_veto,
                                priority=1):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 em_isolation, had_isolation, had_veto, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addEMThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                em_isolation, had_isolation, had_veto,
                                priority=1):
            return self.addCPThresholdValue(value, etamin, etamax, \
                                            phimin, phimax,
                                            em_isolation, had_isolation,
                                            had_veto, priority)

        def addTauThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                 em_isolation, had_isolation, had_veto,
                                 priority=1):
            return self.addCPThresholdValue(value, etamin, etamax, \
                                            phimin, phimax,
                                            em_isolation, had_isolation,
                                            had_veto, priority)

        def addJetThresholdValue(self, value, etamin, etamax, phimin, phimax, \
                                 window=8, priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 window, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def addEnergyThresholdValue(self, value, \
                                etamin, etamax, phimin, phimax, \
                                priority=0):
            thrv = LVL1Thresholds.ThresholdValue(self.ttype, value, \
                                                 etamin, etamax, phimin, phimax, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 IsolationOff, \
                                                 0, priority, self.name+'full')
            self.thresholdValues.append(thrv)
            return self

        def thresholdInGeV(self):
            if len(self.thresholdValues)==0:
                return 0
            else:
                return float(self.thresholdValues[0].value)

        def xml(self, xlist):
            """ Returns XML representation of the LVL1 Threshold """
            sThresholdTypes = ('MUON',
                               'EM', 'TAU',
                               'JET', 'JE',
                               'JB', 'JF', 
                               'TE', 'XE', 'XS', 
                               'LUCID', 'TRT', 'ZDC', 'BCM', 'BCMCMB', 'MBTS', 'MBTSSI', 'CALREQ','NIM','ZB',
                               )
            sInternalThresholdTypes = ('RNDM', 'BGRP', 'PCLK')
            if self.ttype in sThresholdTypes or \
                   self.ttype in sInternalThresholdTypes:
                xtth = xml.dom.minidom.Element('TriggerThreshold')
                xtth.setAttribute('id', idgen.get('TriggerThreshold'))
                xtth.setAttribute('version', '1')
                xtth.setAttribute('name', self.name)
                xtth.setAttribute('type', self.ttype)
                xtth.setAttribute('active', str(self.active))
                xtth.setAttribute('mapping', str(self.mapping))
                xtth.setAttribute('bitnum', str(self.bitnum))
                #if zb type, add attributes for seed, seed type, seed multi and bcdelay
                if(self.ttype == 'ZB'):
                    xtth.setAttribute('seed', str(self.seed))
                    #xtth.setAttribute('seed_type', str(self.seed_ttype))
                    xtth.setAttribute('seed_multi', str(self.seed_multi))
                    xtth.setAttribute('bcdelay', str(self.bcdelay))
                    if (self.name == 'ZB_4MBTS_A'):
                        self.ttype = 'NIM'
                        xtth.setAttribute('type', self.ttype)
                for thrv in self.thresholdValues:
                    thrv.xml(xtth)
                xcable = xml.dom.minidom.Element('Cable')
                xcable.setAttribute('name', self.cable)
                xcable.setAttribute('ctpin', self.slot)
                xcable.setAttribute('connector', self.connector)
                xtth.appendChild(xcable)

                xsignal = xml.dom.minidom.Element('Signal')
                xsignal.setAttribute('range_begin', str(self.range_begin))
                xsignal.setAttribute('range_end',  str(self.range_end))
                xcable.appendChild(xsignal)
                xlist.appendChild(xtth)
            else:
                logger().error('Trigger type not defined: ', self.ttype)

    def compThreshold(thr1, thr2):
        mapping1, mapping2 = thr1.mapping, thr2.mapping
        # First sort by mapping if it's not -1
        if mapping1>=0 and mapping2>0:
            if mapping1 < mapping2: return -1
            if mapping1 > mapping2: return 1
            else: return 0
        elif mapping1>=0: return -1
        elif mapping2>=0: return 1
        else:
            # Second sort by (threshold value and then threshold name)
            import re
            thrv1, thrv2 = 0, 0
            re_thrv = re.compile('(\d+)')
            mg = re_thrv.search(thr1.name)
            if mg: thrv1 = int(mg.group(1))
            mg = re_thrv.search(thr2.name)
            if mg: thrv2 = int(mg.group(1))
            #
            if thrv1 < thrv2: return -1
            elif thrv1 > thrv2: return 1
            else:
                if thr1.name < thr2.name: return -1
                elif thr1.name > thr2.name: return 1
                else: return 0
    compThreshold = staticmethod(compThreshold)
    
    def __init__(self):
        self.thresholds = []
        self.thresholdScale = LVL1Thresholds.ThresholdScale()
    def thresholdNames(self):
        return map(lambda x: x.name, self.thresholds)

    def allThresholdsOf(self, type):
        # thr_types = ('MUON', 'EM', 'TAU', 'JET', 'FJET', 'JE', 'TE', 'XE')
        thr_types = ('MUON', 'EM', 'TAU', 'JET', 'JB','JF', 'JE', 'TE', 'XE',
                     'XS', 'FJL','FJR')
        thrs = []
        if type in thr_types:
            for thr in self.thresholds:
                if thr.ttype == type: thrs.append(thr)
        return thrs

    def thresholdOfName(self, name):
        thr = filter(lambda x: x.name==name, self.thresholds)
        if len(thr)==1:
            return thr[0]
        else:
            return None
    def thresholdOfTypeMapping(self, type, mapping):
        thrs = filter(lambda x: (x.ttype==type and x.mapping==mapping), \
                      self.thresholds)
        if len(thrs)==1:
            return thrs[0]
        elif len(thrs)>1:
            s = 'More than one (%d) LVL1 thresholds of type %s with' % \
                (len(thrs), type)
            s = '%s mapping %d found' % (s, mapping)
            logger().warning(s)
            return thrs[0]
        else:
            return None
            

    def xml(self, xconfig):
        xttlist = xml.dom.minidom.Element('TriggerThresholdList')
        self.thresholds.sort(LVL1Thresholds.compThreshold)
        type_map = []
        for th in self.thresholds:
            mapping = th.mapping
            if mapping==-1:
                mapping = LVL1Thresholds.cableMapping(th.ttype, th.range_begin)
            tm = (th.ttype, mapping)
            if tm not in type_map:
                type_map.append(tm)
                th.xml(xttlist)
            else:
                s = 'Ignoring LVL1 threshold %s of type=%s, mapping=%d' % \
                    (th.name, th.ttype, mapping)
                s = '%s since this position was already occupied' % s
                logger().warning(s)
                print tm 
        xconfig.appendChild(xttlist)

class CTPInfo:
    class Random:
        def __init__(self):
            self.name = ''
            self.rate1 = 0
            self.rate2 = 0
        def set(self, name, rate1, rate2, seed1, seed2):
            self.name = name
            self.rate1 = rate1
            self.rate2 = rate2
            self.seed1 = seed1
            self.seed2 = seed2
        def xml(self, x):
            xrandom = xml.dom.minidom.Element('Random')
            xrandom.setAttribute('name', self.name)
            xrandom.setAttribute('rate1', str(self.rate1))
            xrandom.setAttribute('rate2', str(self.rate2))
            xrandom.setAttribute('seed1', str(self.seed1))
            xrandom.setAttribute('seed2', str(self.seed2))
            x.appendChild(xrandom)
    class PrescaledClock:
        def __init__(self):
            self.name = ''
            self.clock1 = 0
            self.clock2 = 0
        def set(self, name, clock1, clock2):
            self.name = name
            self.clock1 = clock1
            self.clock2 = clock2
        def xml(self, x):
            xpc = xml.dom.minidom.Element('PrescaledClock')
            xpc.setAttribute('name',   self.name)
            xpc.setAttribute('version', '1')
            xpc.setAttribute('clock1', str(self.clock1))
            xpc.setAttribute('clock2', str(self.clock2))
            x.appendChild(xpc)
    class Deadtime:
        def __init__(self):
            self.name = ''
            self.simple = 0
            self.complex1_rate = 0
            self.complex1_level = 0
            self.complex2_rate = 0
            self.complex2_level = 0
        def set(self, name, simple, complex1_rate, complex1_level, \
                complex2_rate, complex2_level):
            self.name = name
            self.simple = simple
            self.complex1_rate = complex1_rate
            self.complex1_level = complex1_level
            self.complex2_rate = complex2_rate
            self.complex2_level = complex2_level
        def xml(self, x):
            xdt = xml.dom.minidom.Element('Deadtime')
            xdt.setAttribute('name', self.name)
            xdt.setAttribute('simple', str(self.simple))
            xdt.setAttribute('complex1_rate', str(self.complex1_rate))
            xdt.setAttribute('complex1_level', str(self.complex1_level))
            xdt.setAttribute('complex2_rate', str(self.complex2_rate))
            xdt.setAttribute('complex2_level', str(self.complex2_level))
            xdt.setAttribute('version','1')
            x.appendChild(xdt)
    class BunchGroupSet:
        class BunchGroup:
            def __init__(self):
                self.name = ''
                self.internalNumber = 0
                self.bunches = []
            def set(self, name, internalNumber, bunches):
                self.name = name
                self.internalNumber = internalNumber
                self.bunches = bunches
            def xml(self, x):
                xbg = xml.dom.minidom.Element('BunchGroup')
                xbg.setAttribute('name', self.name)
                xbg.setAttribute('version', '1')
                xbg.setAttribute('internalNumber', str(self.internalNumber))
                for b in self.bunches:
                    xb = xml.dom.minidom.Element('Bunch')
                    xb.setAttribute('bunchNumber', str(b))
                    xbg.appendChild(xb)
                x.appendChild(xbg)
        def __init__(self):
            self.name= ''
            self.bunchGroups = []
        def set(self, name):
            self.name = name
        def addBunchGroup(self, name, internalNumber, bunches):
            if internalNumber<0 or internalNumber>7:
                logger().error('Warning: tried to add bunchgroup %i, which is not between 0 and 7' % internalNumber)                
            bg = CTPInfo.BunchGroupSet.BunchGroup()
            bg.set(name, internalNumber, bunches)
            bgExists = False
            for existingBG in self.bunchGroups:
                if existingBG.internalNumber==internalNumber:
                    bgExists=True
                    break
            if not bgExists:
                self.bunchGroups.append(bg)
            else:
                logger().error('Warning: tried to add bunchgroup %i, where one with that number already exists' % internalNumber)
            return self
        def xml(self, x):
            xbgs = xml.dom.minidom.Element('BunchGroupSet')
            xbgs.setAttribute('name', self.name)
            xbgs.setAttribute('version','1')
            for bg in self.bunchGroups:
                bg.xml(xbgs)
            x.appendChild(xbgs)
    class TriggerType:
        def __init__(self):
            self.bits = [ 0, 0, 0, 0, 0, 0, 0, 0]
        def setBit(self, bit, x):
            if bit in range(0, 8):
                self.bits[bit] = x
            else:
                logger().error('Warning: tried to access bit '+str(bit)+\
                               " in LVL1 trigger type")
        def xml(self, x):
            xtt = xml.dom.minidom.Element('TriggerType')
            text = 'TriggerType bits: each bit mapped to TriggerItem bit[255 <-> 0]'
            c = xml.dom.minidom.Comment(text)
            xtt.appendChild(c)
            for bit in range(0, 8):
                text = 'bit'+str(bit)
                xbit = xml.dom.minidom.Element(text)
                text = xml.dom.minidom.Text()
                s0 = hex(self.bits[bit])
                if s0.startswith('0x'): s0 = s0[2:]
                if s0.endswith('L'): s0 = s0[:-1]
                n = len(s0)
                if n > 64:
                    logger().error('TriggerType bit has more than 256 bits, '+len(s0))
                    logger().error('Stripping off higher bits and save only least 256 bits')
                    s0 = s0[n-64:]
                elif n < 64:
                    s1 = '0000000000000000000000000000000000000000000000000000000000000000'
                    s1 = s1[n:]
                    s0 = s1 + s0
                s = ''
                for i in range(0, 8):
                    s += s0[i*8:(i+1)*8]
                    if i != 7: s += ' '
                text.replaceWholeText(s)
                xbit.appendChild(text)
                xtt.appendChild(xbit)
            x.appendChild(xtt)
    # Members of CTPInfo
    def __init__(self):
        self.random = CTPInfo.Random()
        self.prescaledClock = CTPInfo.PrescaledClock()
        self.deadtime = CTPInfo.Deadtime()
        self.bunchGroupSet = CTPInfo.BunchGroupSet()
        self.triggerType = CTPInfo.TriggerType()
    def setRandom(self, name, rate1, rate2, seed1, seed2):
        self.random.set(name, rate1, rate2, seed1, seed2)
    def setPrescaledClock(self, name, clock1, clock2):
        self.prescaledClock.set(name, clock1, clock2)
    def setDeadtime(self, name, simple, complex1_rate, \
                    complex1_level, complex2_rate, complex2_level):
        self.deadtime.set(name, simple, complex1_rate, complex1_level, \
                          complex2_rate, complex2_level)
    def setBunchGroupSet(self, name):
        self.bunchGroupSet.set(name)
        return self.bunchGroupSet
    def addBunchGroup(self, name, internalNumber, bunches):
        self.bunchGroupSet.addBunchGroup(name, internalNumber, bunches)
    def setTriggerType(self, bit, value):
        self.triggerType.setBit(bit, value)
    def xml(self, x):
        self.random.xml(x)
        self.deadtime.xml(x)
        self.bunchGroupSet.xml(x)
        self.prescaledClock.xml(x)
        # self.triggerType.xml(x)

class MuctpiInfo:
    def __init__(self):
        self.low_pt = 0
        self.high_pt = 0
        self.max_cand = 0
    def set(self, low_pt, high_pt, max_cand):
        self.low_pt = low_pt
        self.high_pt = high_pt
        self.max_cand = max_cand
    def xml(self, xconfig):
        xmuctpi = xml.dom.minidom.Element('MuctpiInfo')
        low_pt = xml.dom.minidom.Element('low_pt')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.low_pt))
        low_pt.appendChild(text)
        high_pt = xml.dom.minidom.Element('high_pt')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.high_pt))
        high_pt.appendChild(text)
        max_cand = xml.dom.minidom.Element('max_cand')
        text = xml.dom.minidom.Text()
        text.replaceWholeText(str(self.max_cand))
        max_cand.appendChild(text)
        xmuctpi.setAttribute('name', 'muctpi')
        xmuctpi.setAttribute('version', '1')
        xmuctpi.appendChild(low_pt)
        xmuctpi.appendChild(high_pt)
        xmuctpi.appendChild(max_cand)
        xconfig.appendChild(xmuctpi)

class Lvl1CaloInfo:
    AverageOfThr = 1
    LowerThr = 2
    HigherThr = 3
    def __init__(self):
        self.name = ''
        self.globalScale = 1
        self.jetWeights = [0]*12
        self.xsParams = { 'xsSigmaScale' : 0, 'xsSigmaOffset': 0,
                          'xeMin'        : 0, 'xeMax'        : 0,
                          'teSqrtMin'    : 0, 'teSqrtMax'    : 0 }
    def setName(self, name):
        self.name = name
    def setGlobalScale(self, x):
        self.globalScale = x
    def setJetWeights(self, weights):
        self.jetWeights = weights
    def calcJetWeights(lvl1_thresholds, option=AverageOfThr):
        ws = []
        thrvs = []
        thrs = lvl1_thresholds.allThresholdsOf('JET')
        for t in thrs:
            thrvs.append(t.thresholdInGeV())
        if option==Lvl1CaloInfo.AverageOfThr:
            ws = thrvs
        elif option==Lvl1CaloInfo.LowerThr:
            ws = thrvs
        elif option==Lvl1CaloInfo.HigherThr:
            ws = thrvs
        return ws
    calcJetWeights = staticmethod(calcJetWeights)
    def setXsParams( self, **args ):
        for k in args:
            if not k in self.xsParams:
                raise RuntimeError, "'%s' is not a MET significance parameter" %  k
            self.xsParams[k]  = args[k]
    def xml(self, x):
        xcal = xml.dom.minidom.Element('CaloInfo')
        xcal.setAttribute('global_scale', str(self.globalScale))
        xcal.setAttribute('name', 'CaloInfo')
        xcal.setAttribute('version', '1')
        for (i, w) in enumerate(self.jetWeights):
            jw = xml.dom.minidom.Element('JetWeight')
            jw.setAttribute('num', str(i+1))
            text = xml.dom.minidom.Text()
            text.replaceWholeText(str(w))
            jw.appendChild(text)
            xcal.appendChild(jw)
        xs = xml.dom.minidom.Element('METSignificance')
        for n,v in self.xsParams.items():
            xs.setAttribute(n, str(v))
        xcal.appendChild(xs)
        x.appendChild(xcal)
#
# EOF LVL1 classes
#########################################################################################




