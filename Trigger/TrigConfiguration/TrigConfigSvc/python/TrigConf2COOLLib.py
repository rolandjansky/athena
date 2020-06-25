# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file TrigCond2COOLLib.py
###############################################################
#
# The jobOptions to enable reading and writing of configuration
# data from COOL into AOD
#
#
#==============================================================
#
# Required libs:
import os
import re
import threading

import AthenaCommon.Logging as L

from AthenaPython import PyAthena
                                
class TmpThr(threading.Thread):
    def __init__(self, syscmd):
        threading.Thread.__init__(self)
        self.syscmd = syscmd
    def run(self):    
        msg = L.logging.getLogger( 'TrigConf2COOLLib.py' )
        output = os.popen(self.syscmd).readlines()
        maxlevel = 0
        for line in output:
            line = line.lower()
            if ' warning ' in line:
                maxlevel = max(1,maxlevel)
            if re.search(r"warn\s*\[frontier",line) is not None:
                # Catch frontier warnings which could contain the word "error" as
                # frontier will retry connections - full log shown if run in debug
                msg.info("Caught warning from frontier: %s", line)
            elif ' error ' in line and 'connection refused' not in line:
                maxlevel = max(2,maxlevel)
            elif ' fatal ' in line.lower() or 'exception ' in line.lower():
                maxlevel = max(3,maxlevel)
        output = ''.join(output)
        output = ('Log file from execution of command:\n'
               + output
               + '========================================'
               + '\nEnd of log file from TrigConf2COOLLib.py')
        if maxlevel==1:
            msg.warning(output)
        elif maxlevel==2:
            msg.error(output)
        elif maxlevel==3:
            msg.fatal(output)
        else:
            msg.debug(output)
            msg.info('Successful execution of command')

class TrigConf2CoolSyncSvc(PyAthena.Svc):
    def __init__(self, name="TrigConf2CoolSyncSvc",**kw):
        kw['name'] = name
        super(TrigConf2CoolSyncSvc,self).__init__(**kw)
        self.conf2CoolThr = []

    def initialize(self):
        self.msg.info("initializing [%s]", self.name())
        if len(self.conf2CoolThr):
            self.msg.info("wait until TrigConf2Cool thread terminates")
            for tmpThr in self.conf2CoolThr:
                tmpThr.join()
        return PyAthena.StatusCode.Success

    def setThread(self,thr):
        self.conf2CoolThr.append(thr)

    def __getstate__(self):
        if self.conf2CoolThr:
            self.msg.info("wait until TrigConf2Cool thread terminates")
            for thr in self.conf2CoolThr:
                thr.join()
        self.conf2CoolThr = []
        return PyAthena.Svc.__getstate__(self)
    
class ConfToCoolSQlite:
    """Writes the trigger configuration information to COOL/SQlite"""

    __instance = None

    class __single:

        def __init__(self):
            from tempfile import mktemp
            self.dbfilename      = mktemp('.db','hltMenu.')
            from IOVDbSvc.CondDB import conddb
            self.dbname=conddb.GetInstance() # COMP200 or CONDBR2 (consistent with what is used for reading it back)
            self.dbConnection    = "<dbConnection>sqlite://;schema=%s;dbname=%s</dbConnection>" % (self.dbfilename,self.dbname)
            self.isWritingNeeded = False
            from RecExConfig.RecFlags import jobproperties as jp
            from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
            if jp.Rec.Trigger.readLVL1configFromXML():
                self.lvl1menu    = jp.Rec.Trigger.inputLVL1configFile()
            else:
                self.lvl1menu    = jp.Rec.Trigger.outputLVL1configFile()
            if jp.Rec.Trigger.readHLTconfigFromXML():
                self.hltmenu         = jp.Rec.Trigger.inputHLTconfigFile()
            else:
                self.hltmenu         = jp.Rec.Trigger.outputHLTconfigFile()
            self.menusource = 'xml'  # either db or xml
            self.trigdb = ''
            self.smk    = 0
            self.l1psk  = 0
            self.hltpsk = 0
            self.bgsk   = 1
            self.useFrontier = tf.triggerUseFrontier()
            
        def setTriggerDBConnection(self,trigDbConnectionParameters):
            self.menusource = 'db'
            if "alias" in trigDbConnectionParameters:
                coolcon = trigDbConnectionParameters["alias"]
            else:
                technology = trigDbConnectionParameters["techno"]
                if technology == 'sqlite':
                    coolcon = 'sqlite:%s' % trigDbConnectionParameters["filename"]
                if technology == 'oracle':
                    server = trigDbConnectionParameters["server"]
                    schema = trigDbConnectionParameters["schema"]
                    user   = trigDbConnectionParameters["user"  ]
                    passwd = trigDbConnectionParameters["passwd"]
                    coolcon = technology+'://'+server+'/'+schema+';user='+user+';passwd='+passwd+';'
            # log.info(coolcon)
            self.trigdb     = coolcon
            
        def writeConf2COOL(self):
            msg = L.logging.getLogger( 'TrigConf2COOLLib.py' )
            if not self.isWritingNeeded:
                msg.info('Writing of trigger configuration to SQlite COOL is not needed')
                return
            # we write COOL sqlite file from given HLT and LVL1 menu xml
            if self.menusource == 'xml':
                msg.info("Writing menu %s and %s to COOL (%s)", self.lvl1menu, self.hltmenu, self.dbfilename)
                syscmd  = "rm -f %s; TrigConfReadWrite -i %s %s -o cool '%s;%s'" % (self.dbfilename, self.hltmenu, self.lvl1menu, self.dbfilename, self.dbname)
            else:  # db
                if self.smk==0 or self.l1psk==0 or self.hltpsk==0:
                    raise RuntimeError("Source of trigger menu configuration is the TriggerDB, but no keys are specified: %i/%i/%i" % (self.smk, self.l1psk, self.hltpsk))
                msg.info("Writing menu (keys: %i/%i/%i/%i) from triggerDB (%s) to COOL (%s)", self.smk, self.l1psk, self.hltpsk, self.bgsk, self.trigdb, self.dbfilename)
                syscmd  = "rm -f %s; TrigConf2COOLApp -e createwrite" % self.dbfilename
                syscmd += " --cooldb 'sqlite://;schema=%s;dbname=%s'" % (self.dbfilename,self.dbname)
                syscmd += " --trigdb '%s' --configkey %i --prescalekeylvl1 %i --prescalekeyhlt %i --bgkey %i" % (self.trigdb, self.smk, self.l1psk, self.hltpsk, self.bgsk)
                if self.useFrontier:
                    syscmd += " --usefrontier"
                syscmd += " --infiov"
                
            
            msg.info('executing system command to create COOL SQlite file %s with trigger configuration', self.dbfilename)
            msg.info("> " + syscmd)

            tmpThr = TmpThr(syscmd)
            tmpThr.start()
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.TrigConf2CoolSyncSvc.setThread(tmpThr)
            
                
    def __init__(self):
        """ Create singleton instance """
        if self.__instance is None:
            ConfToCoolSQlite.__instance = ConfToCoolSQlite.__single()

        # Store instance reference as the only member in the handle, not sure if this is needed
        self.__dict__['_ConfToCoolSQlite__instance'] = ConfToCoolSQlite.__instance

    def __getattr__(self, attr):
        """ Delegate access to implementation """
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        """ Delegate access to implementation """
        return setattr(self.__instance, attr, value)

