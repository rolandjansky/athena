# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Geneology     import KitTree
from RTTSException import RTTCodingError
import copy
# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class JobGroupKits:
# class JobGroupKits:
    """A class that contains information about all kits. Each kit
    is represented by a dictionary. These dictionaries are stored in a
    dictionary where they are keyed on the group name.

    The class is derived from the Borg implementation of the Singleton pattern.
    Instantiatiating the class with no argument gives a fiexed state.
    Instantiating it with a list of kits changes the existing instances
    everywhere.
    """
    def __init__(self, rttKits=None, userKits=None):
        """If no argument is given, the common object is returned
        Passing a list of kits will change all the existing instances
        """
        
        if rttKits == None: return
        if userKits == None: userKits = []
        self.kitTree = KitTree(rttKits, userKits)

    def kit(self, kitName):
        kit = self.kitTree.getKit(kitName)
        
        if kit == None:
            if kitName == self.kitTree.topParentLabel:
                logger.debug('Asked for a TopParent kit, returning None')
            else:
                m = 'No kit available with name '+kitName
                logger.error(m)
                raise RTTCodingError(m)
        else:
                logger.debug('Found kit with name '+kitName)

        return kit

    def getAllKits(self):
        # print 'getAllKits type', self.kitTree.getAllKits().__class__
        return self.kitTree.getAllKits()

    def __str__(self):
        string = ''
        count = 0
        string = ''
        for group in self.kitTree.getKitNames():
            kkit = self.kit(group)
            string += '\n\nKit %d \n' % (count)
            string += str(kkit)
            count += 1

        return string

    def nKits(self): return self.kitTree.nKits()
if __name__ == '__main__':

    pass
