# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__all__ = ['Logic', 'Not']

from copy import copy

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
        a.logic = Logic.NOT
        a.subConditions.append(x)
        return a




    def __init__(self, condition=None):
        self.condition = condition # hold a Lvl1Condition instance
        self.logic = Logic.NONE
        self.subConditions = [] # holds Logic instances


    def __or__(self, x):
        #print self, "OR", x
        newLogic = Logic()
        newLogic.logic = Logic.OR

        if self.logic == Logic.OR:
            newLogic.subConditions += copy(self.subConditions)
        else:
            newLogic.subConditions += [copy(self)]

        if x.logic == Logic.OR:
            newLogic.subConditions += copy(x.subConditions)
        else:
            newLogic.subConditions += [copy(x)]

        return newLogic

    
    def __and__(self, x):
        #print self, "AND", x
        newLogic = Logic()
        newLogic.logic = Logic.AND

        if self.logic == Logic.AND:
            newLogic.subConditions += copy(self.subConditions)
        else:
            newLogic.subConditions += [copy(self)]

        if x.logic == Logic.AND:
            newLogic.subConditions += copy(x.subConditions)
        else:
            newLogic.subConditions += [copy(x)]

        return newLogic

    def __not__(self, x):
        log.debug('checking NOT')
        a = self
        if a.logic==Logic.NONE:
            a.logic = Logic.NOT
        if a.logic==Logic.NOT:
            if len(a.subConditions) == 1:
                log.debug('not is a unary operator, ignore it')
            else:
                a.subConditions.append(x)
        return a


    def __str__(self):
        s = ''
        if self.logic == Logic.NONE:
            if len(self.subConditions)==0 and self.condition!=None:
                return str(self.condition)
            if len(self.subConditions)==1:
                return str(self.subConditions[0])
            if len(self.subConditions)>=1:
                log.error('Logic NONE has more than one element')
                return ''

        if self.logic == Logic.NOT:
            if len(self.subConditions)==1:
                if self.subConditions[0].logic == Logic.NONE:
                    return '!'+str(self.subConditions[0])
                else:
                    return '!('+str(self.subConditions[0]) + ')'
            log.error('Logic NOT must have exactly one element but has %i' % len(self.subConditions))
            return ''

        if self.logic == Logic.AND or self.logic == Logic.OR:
            s = ''
            if len(self.subConditions)<=1:
                log.error('Logic AND/OR must have more than one sub element but has %i: %r' % (len(self.subConditions), self.subConditions))
                return ''
            else:
                for (i, a) in enumerate(self.subConditions):
                    if i > 0: s += self.logic
                    if(a.logic == Logic.NONE or a.logic == Logic.NOT):
                        s += str(a)
                    else:
                        s += '(' + str(a) + ')'
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


    def conditions(self, include_internal=False):
        cond = set([])
        if self.condition!=None:
            from Lvl1Condition import Lvl1InternalTrigger
            if isinstance(self.condition, Lvl1InternalTrigger):
                if include_internal:
                    cond.add(self.condition)
            else:
                cond.add( self.condition )
        else:
            for sc in self.subConditions:
                cond.update( sc.conditions(include_internal) )
        return sorted(list(cond))


    def normalize(self):
        if self.logic in (Logic.AND, Logic.OR):
            mylogic = self.logic
            newconditions = []
            for c in self.subConditions:
                if c.logic == mylogic: # X&(A&B) or X|(A|B) 
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
        if self.logic==Logic.NONE:
            return self.condition.xml(ind,step)

        elif self.logic==Logic.AND or self.logic==Logic.OR or self.logic==Logic.NOT:
            logic = Logic.symbolToString(self.logic)
            s = ind * step * ' ' + '<%s>\n' % logic
            for c in self.subConditions:
                s += c.xml(ind+1, step) + "\n"
            s += ind * step * ' ' + '</%s>' % logic
            return s

        else:
            log.error('Unknown node in LVL1 item logic')
            log.error('  ==> sublogic = %s' % self.logic)
            log.error('  ==> # subConditions = %i' % len(self.subConditions))


    def printIt(self):
        for a in self.subConditions:
            if a.logic==a.NONE and a.condition!=None:
                log.info('subCondition :', str(a.condition))
            else:
                log.info('subCondition :', a.printIt())
        return ''


def Not(x):
    return Logic.Not(x)
