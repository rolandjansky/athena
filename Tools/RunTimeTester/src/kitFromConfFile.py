# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----- Python modules -----

import os.path, sys, xml.dom.minidom

#----- RTT modules -----

from exc2string       import exc2string
from JobGroupsParser  import makeKits
# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

from MethodTracer        import MetaTracer
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer


def kitFromConfFile(packages):
    """
    returns all kits from constructed given a dictionary of
    (jobgroup: pathToConfFile) from any legal config file format.
    
    a router method that calls kit returning methods depending
    on whether the conf file  is RTT or Unified style.
    The paths is only needed by the RTT style - soon
    to be supressed!
    """
    kits = []
    
    # loop over all CVSPackage objects
    for package in packages:
        try:
            if package.noError():
                newKits = makeUnifiedConfigKits(package.packageXMLfile)
                kits.extend(newKits)
        except Exception, e:
            m  = "Unable to make job group kits for package %s\n" % str(package.name)
            m += "Traceback: %s\n%s" % (str(e),
                                        exc2string(sys.exc_info()))
            logger.error(m)
            package.setErrorMessage(str(e))                
    return kits
    
#-------------------------------------------------------------------------

def makeUnifiedConfigKits(packageXMLfile):
    logger.debug('Making job group kits from package XML file: %s' % packageXMLfile.path)

    groups = packageXMLfile['rtt/jobGroups']
    if groups: 
        return makeKits(groups[0]) # should be only one at most in list, pass it in
    return {}

if __name__ == '__main__':
    pass
