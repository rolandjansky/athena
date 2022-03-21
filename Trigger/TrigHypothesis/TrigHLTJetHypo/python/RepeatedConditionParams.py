# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

class RepeatedConditionParams:
    """Class used to transport parameters required to construct
    a RepeatedCondition object. These parameters are the toolfactory keys
    to construct the elemental conditions and their constructor values, and
    the integer ids for the tree and parent nodes.
    """
    
    def __init__(self,  tree_id, tree_pid, clique=-1,
                 chainPartInd=-1, multiplicity=1,
                 condargs=[], invert=False):
  
        self.tree_id = tree_id
        self.tree_pid = tree_pid
        self.clique = clique # Conditions with the same args form a  clique
        self.chainPartInd = chainPartInd
        self.multiplicity = multiplicity

        # condargs is a list with elements (key, values)
        # key is a string values is a dict
        self.condargs = condargs
        self.invert = invert
  
    def __str__(self):

        s = [self.__class__.__name__ + ':']
        
        width = max([len(k) for k in self.__dict__])

        [s.append('%s %s' % (k.ljust(width), str(v)))
         for k, v in self.__dict__.items()]
        return '\n'.join(s)

