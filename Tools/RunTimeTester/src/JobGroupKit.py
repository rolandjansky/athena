# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection import formatCollection
from Tools            import unique
from Geneology        import TreeMember
from RTTSException    import RTTCodingError

import logging
logger = logging.getLogger('rtt')

class JobGroupKit(TreeMember):
    def __init__(self, group, parent=None):
        
        self.group      = group
        self.parent     = parent
        self.logChecker = None

        TreeMember.__init__(self, group, parent)

        
        self.kitItems = { 'keepFilePatterns':  [],
                          'checkers':          [],
                          'auxFilePatterns':   [],
                          'actions':           [], # list of UserTestDescriptor instances
                          # 'monActions':        [], # list of UserTestDescriptor instances
                          'tests':             []  # list of UserTestDescriptor instances
                          }

    def __getitem__(self, what):
        return self.getItem(what)
    
    def __str__(self):
        string  ='group:       %s\n'%self.group
        string +='parent:      %s\n'%self.parent
        string +='log`Checker: %s\n'%str(self.logChecker)
        string +='\n'
        string += formatCollection(self.kitItems)
        return string

    def kitLists(self):
        "Towards data  structure independent hiding"
        return self.kitItems.values()
    
    def add(self, kit):
        [self.kitItems[item].extend(kit.kitItems[item]) for item in self.kitItems.keys()]
        [unique(self.kitItems[item]) for item in self.kitItems.keys()]
        # do not overwrite the log checker if
        if not self.logChecker: self.logChecker = kit.logChecker

    def extendItem(self, item, list):
        self.kitItems[item].extend(list)
        unique(self.kitItems[item])

    def setLogChecker(self, kits):
        """if this kit has no log checker, set it to the one of the
        closest ancestor"""

        ancestor = self.kitFromGroup(self.parent, kits)
        while (not self.logChecker) and ancestor:
            self.logChecker = ancestor.logChecker
            ancestor = self.kitFromGroup(ancestor.parent, kits)

        if not self.logChecker:
            logger.warning('No log checker set for group %s' % self.group)

    def kitFromGroup(self, group, kits):
        "Method used to find ancestors"
        
        for kit in kits:
            if kit.group == group: return kit
    
        return None

    def getItem(self, itemName):
        if itemName in self.kitItems:
            return self.kitItems[itemName]
        else:
            logger.error('Item '+itemName+' not in kit')
            return None

    def hasCheckers(self):
        # assume that all job groups have at least one checker: the job status check
        return len(self.kitItems['checkers'])>1 
        
    def equivalent(self, other):

        if not hasattr(other, 'group'):      return False
        if not hasattr(other, 'parent'):     return False
        if not hasattr(other, 'logChecker'): return False
        if not hasattr(other, 'kitItems'):   return False


        if self.group  != other.group:       return False
        if self.parent != other.parent:      return False

        if not self.logChecker:
            # either kits self and other have no log checker.... 
            if other.logChecker:            return False
        else:
            if not other.logChecker:        return False
            # or they are equivalent
            if not self.logChecker.equivalent(other.logChecker): return False

        # check the dictionary has the same keys.
        m_keys = self.kitItems.keys()
        o_keys = other.kitItems.keys()

        if len(m_keys) != len(o_keys): return False


        for k in m_keys:
            if k not in o_keys: return False
            # Each key corresponds to a list check length
            m_list = self.kitItems[k] 
            o_list = other.kitItems[k] 
            if len(m_list) != len(o_list): return False


        # the kit has lists of string and objects.
        # the (not wonderful) check is to compare lists of strings.
        # for lists that are already strings, use the list itself.
        # for lists of objects use the class name of the object.
        # this is clearly not a rigorous test (the objects can have
        # the same classname, but different state), but it will do for now.
        
        stringLists = [
            'keepFilePatterns',
            'auxFilePatterns'
            ]

        listDict = {}
        for k in stringLists:
            listDict[k] = (self.kitItems[k], other.kitItems[k])

        classLists = [
            'checkers',
            'actions',
            # 'monActions',
            'tests'
            ]

        for k in classLists:
            listDict[k] = (
                [kk.__class__.__name__ for kk in self.kitItems[k]], 
                [kk.__class__.__name__ for kk in other.kitItems[k]]
                )

        # sanity check - have all the keys been covered?
        if len(listDict.keys()) != len(self.kitItems.keys()):
            raise RTTCodingError('Sanity test: bad kit equivalence test - different lengths')
        
        for k in listDict.keys():
            for element in listDict[k][0]:
                if element not in listDict[k][1]: return False

        return True
        
if __name__ == '__main__':
    print JobGroupKit('xxx','yyy')
    print JobGroupKit('AthenaAtlfast')
