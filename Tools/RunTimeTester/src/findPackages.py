# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path, string
from Paths import Paths


# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

def findPackages(root, releasePath = '', dirs='src;cmt'):
    """
    findPackages returns a list of directories below a root directory 'root'
    which:
    1 contain the directories listed in the semi-colon separated string 'dirs'
    2 contain a directory with the same name as the current one once any -xx is dropped
      (ie Atlfast-xx-yy-zz needs to contain Atlfast)
    """

    class Bunch:
        def __init__(self, **kwds):
            self.__dict__.update(kwds)
        
    dirList=dirs.split(';')
    arg = Bunch(dirList=dirList, results=[])
    
    def visit(arg, dirname, files):
        for file in files:
            fullname = os.path.normpath(os.path.join(dirname, file))
            if os.path.isdir(fullname):
                if file.find('-') != -1:
                    dirList.append(file.split('-')[0])
                    # nDirList.append(file.split('-')[0])
                    alist = [dir for dir in dirList if dir in os.listdir(fullname)]
                    if alist == dirList:
                        arg.results.append(string.replace(dirname, releasePath,'')[1:])
                        logger.debug(string.replace(dirname, releasePath,'')[1:])
                        
                    dirList.pop()
                    
    os.path.walk(root, visit, arg)

    return arg.results

if __name__ == '__main__':
    import sys

    
    print sys.argv[1]
    findPackages(sys.argv[1])
