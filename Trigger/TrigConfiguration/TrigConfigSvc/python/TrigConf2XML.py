# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TrigCond2XML.py
## @brief Core job python configurable to setup IOVDbSvc
## @author RD Schaffer <R.D.Schaffer@cern.ch>
## $Id: TrigConf2XML.py,v 1.3 2008-03-07 22:40:23 stelzer Exp $
###############################################################
#
# The jobOptions to enable writing of configuration
# data from the DB to XML#
#
#==============================================================
#
# Required libs:
import os.path

from AthenaCommon.JobProperties import jobproperties as jp
from AthenaCommon.Logging import logging

class ConfToXML:
    """Writes the trigger configuration information to XML"""

    __instance = None

    class __single:

        def __init__(self):
            
            self.trigdb          = ""
            self.dbuser          = ""
            self.dbpass          = ""
            self.dbkey           = 1
            self.lvl1pskey       = 1
            self.hltpskey        = 1
            self.outputfile      = "dbOutput"
            
        def writeConf2XML(self):
            msg = logging.getLogger( 'TriggerConfig(2XML)' )
            msg.info("Writing HLT XML file %s" % (self.outputfile) )

            syscmd  = "TrigConf2XMLApp"
            syscmd += " --trigdb %s"          % (self.trigdb)
            syscmd += " --u %s"               % self.dbuser
            syscmd += " --p %s         "      % self.dbpass
            syscmd += " --configkey %i"       % self.dbkey
            syscmd += " --prescalekeylvl1 %i" % self.lvl1pskey
            syscmd += " --prescalekeyhlt %i"  % self.hltpskey
            syscmd += " --outputfile %s"      % self.outputfile
                        
            msg.info('executing system command to create XML file')
            msg.info("> " + syscmd)
            
            output = os.popen(syscmd).readlines()
            for line in output:
                msg.info(line.strip())
                
    def __init__(self):
        """ Create singleton instance """
        if self.__instance is None:
            ConfToXML.__instance = ConfToXML.__single()

        # Store instance reference as the only member in the handle, not sure if this is needed
        self.__dict__['_ConfToXML__instance'] = ConfToXML.__instance

    def __getattr__(self, attr):
        """ Delegate access to implementation """
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        """ Delegate access to implementation """
        return setattr(self.__instance, attr, value)

theConfXMLWriter = ConfToXML()
