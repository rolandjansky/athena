# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

class HelperConfigToolParams:
    """Class used to transport parameters required to construct
    a HelperConfigTool object.
    This includes  the tree_vector, leaf_vector and parameters
    for the RepeatedConditions, both for FastReduction Condition nodes,
    and Conditions that will filter for the node.
    """
    
    def __init__(self, treevec=[], repcondargs=[], filterparams=[]):
  

        self.treevec = treevec
        self.repcondargs = repcondargs
        self.filterparams = filterparams

    @property
    def treevec(self):
        return self.__treevec
    
    @property
    def leafvec(self):
        return self.__leafvec

    @treevec.setter
    def treevec(self, treevec):

        if not treevec:
            self.__treevec = []
            self.__leafVec = []
            return
        
        assert len(treevec) > 1 # root plus at least one node
        assert treevec[0] == 0
        for i in range(1, len(treevec)):
            assert treevec[i] >= 0, 'bad tree vec: %s' % str(treevec)
            assert treevec[i] < i, 'bad tree vec: %s' % str(treevec)

        is_leaf = [True for i in range(len(treevec))]
        is_leaf[0] = False
        for i in treevec[1:]:
            is_leaf[i] = False

        self.__leafvec = [i for i in range(len(is_leaf)) if is_leaf[i]]
        self.__treevec = treevec

    @property
    def repcondargs(self):
        return self.__repcondargs
    
    @repcondargs.setter
    def repcondargs(self, repcondargs):
        for ca in repcondargs:
            assert ca.tree_id <= len(self.treevec)
            assert ca.tree_pid <= len(self.treevec) - 1
        self.__repcondargs = repcondargs

    @property
    def filterparams(self):
        return self.__filterparams

    @filterparams.setter
    def filterparams(self, filterparams):
        self.__filterparams = filterparams
        
    def __str__(self):

        s = [self.__class__.__name__ + ':']
        to_show = ['treevec', 'leafvec', 'repcondargs', 'filterparams']
        width = max([len(a) for a in to_show])
        [s.append('%s %s' % (a.ljust(width), getattr(self, a))) for a in to_show]
        return '\n'.join(s)
