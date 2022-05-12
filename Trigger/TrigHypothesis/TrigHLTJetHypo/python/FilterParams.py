# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

class FilterParams:
    """Class used to transport parameters required to construct
    a Filter object.
    This includes  the name of the filter type and its constructor
    arguments.
    """
    
    def __init__(self, typename='', args=None):

        self.typename = typename
        self.args = args

    def __eq__(self, other):
        return self.typename == other.typename and self.args == other.args
 
    def __str__(self):

        s = [self.__class__.__name__ + ':']
        to_show = ['typename', 'args']
        width = max([len(a) for a in to_show])

        [s.append('%s %s' % (a.ljust(width), str(getattr(self, a))))
         for a in to_show]
        
        return '\n'.join(s)
