# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__all__ = ['Logic', 'Not']

from copy import copy,deepcopy

from Lvl1MenuUtil import log


class Logic(object):
    NONE = ''
    AND = '&'
    OR = '|'
    NOT = '!'

    @staticmethod
    def symbolToString(x):
        if x==Logic.AND:
            return 'AND'
        elif x==Logic.OR:
            return 'OR'
        elif x==Logic.NOT:
            return 'NOT'
        else:
            return ''

    @staticmethod
    def Not(x):
        a = Logic()
        a.subLogic = Logic.NOT
        a.subConditions.append(x)
        return a

    def __init__(self, condition=None):
        self.condition = condition # hold a Lvl1Condition instance
        self.subLogic = Logic.NONE
        self.subConditions = [] # holds Logic instances


    def __or__(self, x):
        if self.subLogic == Logic.OR:
            a = copy(self)
        else:
            a = Logic()
            a.subLogic = Logic.OR
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a

    
    def __and__(self, x):
        if self.subLogic == Logic.AND:
            a = deepcopy(self)
        else:
            a = Logic()
            a.subLogic = Logic.AND
            a.subConditions.append(self)
        a.subConditions.append(x)
        return a


    def __not__(self, x):
        log.debug('checking NOT')
        a = self
        if a.subLogic==Logic.NONE:
            a.subLogic = Logic.NOT
        if a.subLogic==Logic.NOT:
            if len(a.subConditions) == 1:
                log.debug('not is a unary operator, ignore it')
            else:
                a.subConditions.append(x)
        return a


    def __str__(self):
        s = ''
        if self.subLogic == Logic.NONE:
            if len(self.subConditions)==1:
                s += str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                log.error('Logic NONE has more than one element')
                s += ''
            elif len(self.subConditions)==0 and self.condition!=None:
                s += str(self.condition)
        elif self.subLogic == Logic.NOT:
            if len(self.subConditions)==1:
                s += '!'+str(self.subConditions[0])
            elif len(self.subConditions)>=1:
                log.error('Logic NOT has more than one element')
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
        names = set([])
        if self.condition!=None:
            from Lvl1Condition import Lvl1InternalTrigger
            if isinstance(self.condition, Lvl1InternalTrigger):
                if include_bgrp:
                    names.add(self.condition.name())
            else:
                names.add( self.condition.threshold.name )
        else:
            for sc in self.subConditions:
                names.update( sc.thresholdNames(include_bgrp) )
        return sorted(list(names))


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


    def xml(self, ind, step=2):
        if self.subLogic==Logic.NONE:
            return self.condition.xml(ind,step)

        elif self.subLogic==Logic.AND or self.subLogic==Logic.OR or self.subLogic==Logic.NOT:
            logic = Logic.symbolToString(self.subLogic)
            s = ind * step * ' ' + '<%s>\n' % logic
            for c in self.subConditions:
                s += c.xml(ind+1, step) + "\n"
            s += ind * step * ' ' + '</%s>' % logic
            return s

        else:
            log.error('Unknown node in LVL1 item logic')
            log.error('  ==> sublogic = %s' % self.subLogic)
            log.error('  ==> # subConditions = %i' % len(self.subConditions))


    def printIt(self):
        for a in self.subConditions:
            if a.subLogic==a.NONE and a.condition!=None:
                log.info('subCondition :', str(a.condition))
            else:
                log.info('subCondition :', a.printIt())
        return ''


def Not(x):
    return Logic.Not(x)
