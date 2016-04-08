# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from Factory_HardCoded import HardCodedFactory
from RTTSException     import RTTInputError

"""
Classes that specify actions needed to be taken according to the site.
"""
# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class SiteSpecifics:     
    """ Base class """
    def __init__(self):
        hardCoded = HardCodedFactory(logger).create('base', None)
    
    def rootCommand(self):
        """How root is called at a specific site."""
        pass

    def pathToOracleInstantClient(self,gcc_version):
        return ''
    
    def addToLibs(self):
        return ''

    def addToPath(self):
        return ''

    def rootSys(self):
        """How a root env var is called at a specific site."""
        return ''

    def preAthena(self):
        """Actions required before invoking athena at a specific site."""
        return ''

    def preTogether(self): return ''
    
    def postTogether(self): return ''
    
    def togetherCommand(self):
        return '/usr/local/Borland/Together/bin/Together.sh'

    def site(self): pass

# ***********************************************************************
# ***********************************************************************
# ***********************************************************************

class UCLslc3SiteSpecifics(SiteSpecifics):
    """
    Running on SLC3 machines at UCL
    """

    def __init__(self):
        SiteSpecifics.__init__(self)

    # -----------------------------------------------------------------------
    
    def rootCommand(self):
        return '/afs/cern.ch/sw/root/v4.03.02/slc3_gcc3.2.3/root/bin/root -b -q '

    # -----------------------------------------------------------------------
    
    def rootSys(self):
        return '/afs/cern.ch/sw/root/v4.03.02/slc3_gcc3.2.3/root'

    # -----------------------------------------------------------------------
    
    def site(self): return 'UCLslc3'
    
    # -----------------------------------------------------------------------

    def preAthena(self):
        "Set up env variables required by Gaudi, but not usually set at UCL"
        envVarsToSet = 'export HOST=$HOSTNAME\n'
        envVarsToSet = envVarsToSet+'export OS=Linux'
        return envVarsToSet

    # -----------------------------------------------------------------------
    
    def pathToOracleInstantClient(self,gcc_version):
        paths = {'gcc344':'/unix/atlasrtt/Oracle/instantclient/10.2.0.2/slc3_ia32_gcc344/lib',
                 'gcc323':'/unix/atlas1/Oracle/instantclient/10.1.0.3-1/slc3_ia32_gcc323/lib'}

        path = ''
        try:
            path = paths[gcc_version]
        except KeyError:
            m  = 'KeyError: Unable to return a path to an Oracle instant client at UCL\n'
            m += 'because the compiler version (upon which the location depends), namely: ' + str(gcc_version) + '\n'
            raise RTTInputError(m)
        else:
            return path            

    # -----------------------------------------------------------------------
    
    def addToLibs(self):
        r = '/usr/lib:'        
        return r

    def addToPath(self):
        return '/usr/local/bin'

# ***********************************************************************
# ***********************************************************************
# ***********************************************************************

class CERNSiteSpecifics(SiteSpecifics):
    """
    For running at CERN
    """
    def __init__(self):
        SiteSpecifics.__init__(self)
        
    def rootCommand(self):
        return "/afs/cern.ch/sw/root/v4.03.02/slc3_gcc3.2.3/root/bin/root -b -q "
    
    def rootSys(self):
        return '/afs/cern.ch/sw/root/v4.03.02/slc3_gcc3.2.3/root'

    def site(self): return 'CERN'

class CERNslc4SiteSpecifics(CERNSiteSpecifics):
    """
    For running at CERN on SLC4
    """
    def __init__(self):
        CERNSiteSpecifics.__init__(self)

    def rootCommand(self): ''
    def rootSys(self): ''
    def site(self): return 'CERNslc4'
        
    
class PetersMacSiteSpecifics(SiteSpecifics):
    "For running on Peter Sherwood's laptop"

    def __init__(self):
        SiteSpecifics.__init__(self)
        
    def rootCommand(self):
        return '/sw/bin/root -b -q'

class LancasterSiteSpecifics(SiteSpecifics):
    def __init__(self):
        SiteSpecifics.__init__(self)

    def site(self): return 'Lancaster'

