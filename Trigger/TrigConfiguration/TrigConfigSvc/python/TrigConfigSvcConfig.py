# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigConfigSvc.TrigConfigSvcConf import (TrigConf__LVL1ConfigSvc,
                                             TrigConf__L1TopoConfigSvc,
                                             TrigConf__HLTConfigSvc,
                                             TrigConf__DSConfigSvc,
                                             TrigConf__TrigConfigSvc)

# Hack: xml.etree module is hidden by mistake in LCG56c
from PyUtils.xmldict import import_etree
etree = import_etree()
import xml.etree.cElementTree as ET

from os.path import exists, join, abspath

from AthenaCommon.Logging import logging  # loads logger
from PyUtils.Decorators import memoize

@memoize
def findFileInXMLPATH(filename):
    """ Use XMLPATH to find files (cache result through memoize decorator)"""

    if filename=="NONE":
        return filename

    filename = str(filename)

    mlog = logging.getLogger("TrigConfigSvcConfig.py")
    mlog.debug("Searching XML config file for HLT")
    if filename.find('./') == 0: ## this expected to be local file, name starts from ./
        return filename
    else:
        mlog.debug("Nonlocal XML config file for HLT")
        from os import environ
        ## even if ./ not as file name prefix look first in PWD
        if exists(filename):
            mlog.info(filename+" XML configuration file taken from working directory")
            return filename

        ## search XMLPATH path
        if 'XMLPATH' not in environ: ## XMLPATH is not known ... no search is performed
            mlog.info("XML file: "+filename + " not found and XMLPATH not given" )
            return filename

        xmlpath = environ['XMLPATH']
        paths = xmlpath.split(":")
        for path in paths:

            test = join(path, filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

            test = join(path, "TriggerMenuXML",filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

            test = join(path, "TriggerMenuMT",filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

        return filename



class DefaultHLTConfigSvc( TrigConf__HLTConfigSvc ):
    #__slots__ = []
    def __init__( self, name="HLTConfigSvc" ):
        super( DefaultHLTConfigSvc, self ).__init__( name )  #

    def getAlgorithmsRun2(self):
        """Produces pair of lists with algorithms scheduled for  L2 and EF"""

        mlog = logging.getLogger("TrigConfigSvcConfig.py")
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if TriggerFlags.readMenuFromTriggerDb():
            from TrigConfigSvc.TrigConfigSvcUtils import getAlgorithmsForMenuRun2
            mlog.info("Will load algos from DB")
            allalgs = getAlgorithmsForMenuRun2(TriggerFlags.triggerDbConnection(),TriggerFlags.triggerDbKeys()[0])
        else:
            mlog.info("Will load algos from xml")
            allalgs = []
            doc = ET.parse( findFileInXMLPATH(self.XMLMenuFile) )
            algs = self.getAllAlgorithms(doc)
            l2TEs, efTEs = self.getTEsByLevel(doc)

            for te in l2TEs:
                if te in algs.keys():
                    allalgs += algs[te]

            for te in efTEs:
                if te in algs.keys():
                    allalgs += algs[te]

        return allalgs



    def getAlgorithmsByLevel(self):
        """Produces pair of lists with algorithms scheduled for  L2 and EF"""

        mlog = logging.getLogger("TrigConfigSvcConfig.py")
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if TriggerFlags.readMenuFromTriggerDb():
            from TrigConfigSvc.TrigConfigSvcUtils import getAlgorithmsForMenu
            mlog.info("Will load algos from DB")
            l2algs, efalgs = getAlgorithmsForMenu(TriggerFlags.triggerDbConnection(),TriggerFlags.triggerDbKeys()[0])
        else:
            mlog.info("Will load algos from xml")
            l2algs = []
            efalgs = []
            doc = ET.parse( findFileInXMLPATH(self.XMLMenuFile) )
            algs = self.getAllAlgorithms(doc)
            l2TEs, efTEs = self.getTEsByLevel(doc)

            for te in l2TEs:
                if te in algs.keys():
                    l2algs += algs[te]

            for te in efTEs:
                if te in algs.keys():
                    efalgs += algs[te]

        return l2algs, efalgs

    def getTEsByLevel(self, doc = None):
        """Produce 2 lists of all TEs used in signatures of level L2 and EF"""
        l2TEs = []
        efTEs = []
        #print "INFO parsing: " + self.XMLMenuFile
        if not doc: doc = ET.parse( findFileInXMLPATH(self.XMLMenuFile) )

        #print "INFO got chains " + str(chainlist)
        for ch in doc.getiterator("CHAIN"):
            for te in ch.getiterator("TRIGGERELEMENT"):
                if ch.get("level") == "L2":
                    l2TEs.append(te.get("te_name"))
                else:
                    efTEs.append(te.get("te_name"))

        # having TEs of each level which are mentioned in chains we need to scan sequences now
        # and find if there are some recursive sequences to be called
        seqinp = self.getAllSequenceInputs()
        for te in l2TEs:
            if te in seqinp.keys():
                l2TEs.extend(seqinp[te]) # note that this recursive (we extend list on which we loop)

        for te in efTEs:
            if te in seqinp.keys():
                efTEs.extend(seqinp[te]) # note that this recursive (we extend list on which we loop)

        # make them unique
        l2TEs = self.unique(l2TEs)
        efTEs = self.unique(efTEs)

        # eliminate from EF list those TEs mentioned in the L2
        temp = []
        for te in efTEs:
            if te not in l2TEs:
                temp.append(te)
        efTEs = temp
        return l2TEs, efTEs

    def unique(self, l):
        #d = {}
        #for item in l:
        #    d[item] = item
        #return d.keys()
        return list(set(l))


    def getAllSequenceInputs (self, doc = None):
        """ Produce dictionaries where key is outout TE name and values are tuples of TEs """
        if not doc: doc = ET.parse( findFileInXMLPATH(self.XMLMenuFile) )
        inp = {}
        for seq in doc.getiterator("SEQUENCE"):
            #print "INFO Discovered algorithms in the sequence: " + seq.getAttribute("algorithm")
            inp[seq.get("output")] = seq.get("input").split()
        return inp

    def getAllAlgorithms(self, doc = None):
        """ Produce dictionary where key is output TE name of the sequence and value is a list of algos in sequence"""
        #print "INFO parsing: " + self.XMLMenuFile
        if not doc: doc = ET.parse( findFileInXMLPATH(self.XMLMenuFile) )
        #print "INFO getting sequences "
        algos = {}
        for seq in doc.getiterator("SEQUENCE"):
            algos[seq.get("output")] = seq.get("algorithm").split()

        return algos



class TestingHLTConfigSvc ( DefaultHLTConfigSvc ):
    def __init__( self, name="HLTConfigSvc" ):
        super( TestingHLTConfigSvc, self ).__init__( name )  #

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel=VERBOSE


class HLTConfigSvc ( DefaultHLTConfigSvc ):
    def __init__( self, name="HLTConfigSvc" ):
        super( HLTConfigSvc, self ).__init__( name )  #

    def setDefaults(self, handle):
        pass



class DefaultLVL1ConfigSvc ( TrigConf__LVL1ConfigSvc ):
    __slots__ = []
    def __init__( self, name="LVL1ConfigSvc" ):
        super( DefaultLVL1ConfigSvc, self ).__init__( name )  #

    def setDefaults(self, handle):
        pass


class TestingLVL1ConfigSvc ( TrigConf__LVL1ConfigSvc ):
    __slots__ = []
    def __init__( self, name="LVL1ConfigSvc" ):
        super( TestingLVL1ConfigSvc, self ).__init__( name )  #

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel=VERBOSE




class LVL1ConfigSvc ( DefaultLVL1ConfigSvc ):
    __slots__ = []
    def __init__( self, name="LVL1ConfigSvc" ):
        super( LVL1ConfigSvc, self ).__init__( name )

    @property
    def XMLFile(self):
        '''The xml file name for backward compatibility with old property name'''
        return  self.XMLMenuFile

    @XMLFile.setter
    def XMLFile(self, xmlfile):
        log = logging.getLogger("LVL1ConfigSvc")
        log.warning( "LVL1ConfigSvc property XMLFile will soon be deprecated. Please use XMLMenuFile instead" )
        self.XMLMenuFile = xmlfile

    def setDefaults(self, handle):
        pass

class L1TopoConfigSvc ( TrigConf__L1TopoConfigSvc ) :
    """L1Topo configuration"""
    __slots__ = []
    def __init__(self, name="L1TopoConfigSvc" ):
        super (L1TopoConfigSvc, self).__init__(name)

    def setDetaults(self, handle):
        pass

class DSConfigSvc ( TrigConf__DSConfigSvc ) :
    """Detector Store implementation of the TrigConfigSvc"""
    __slots__ = []
    def __init__(self, name="DSConfigSvc" ):
        super (DSConfigSvc, self).__init__(name)

    def setDetaults(self, handle):
        pass

class TrigConfigSvc( TrigConf__TrigConfigSvc ):
    __slots__ = []
    def __init__(self, name="TrigConfigSvc" ):
        super( TrigConfigSvc, self).__init__(name)

    def setDefaults(self, handle):
        pass


# singleton class used for all
# clients of any TrigConfigSvc
# usage:
# from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
# svc = SetupTrigConfigSvc()
# svc.SetStates( ["ds"] )
# svc.InitialiseSvc()
#
# after InitialiseSvc() is called, the state is fixed
# possible states are: ds, xml


class SetupTrigConfigSvc(object):
    """ A python singleton class for configuring the trigger configuration services"""

    class __impl(object):
        """ Implementation of the singleton interface """

        def __init__(self):
            """
            state == xml -> read the trigger configuration from 2 xml files, one for L1, one for HLT
            stats == ds  -> read the trigger configuration from the detector store = esd header
            state == none -> service is not directly serving the run3 configuration
            """
            self.states = ["xml"]
            self.allowedStates = set(['none','xml','ds'])
            self.initialised = False

            from AthenaCommon.Logging import logging
            self.mlog = logging.getLogger("TrigConfigSvcConfig.py")

            # svc properties:
            self.l1topoXmlFile = 'NONE'
            self.hltXmlFile = 'HLT_XML_FILE_NOT_SET'
            self.l1XmlFile  = 'L1_XML_FILE_NOT_SET'

        def spam(self):
            """ Test method, return singleton id """
            return id(self)


        def SetStates(self, state):
            if self.initialised:
                raise (RuntimeError, 'state cannot be changed anymore, the athena service has already been added!')

            if not type(state) is list:
                state = [state]

            if not set(state) <= self.allowedStates:
                raise (RuntimeError, 'unknown state %s, cannot set it!' % state)
            else:
                self.states = state

        def GetConfigurable(self):
            try:
                self.InitialiseSvc()
            except Exception:
                self.mlog.debug( 'ok, TrigConfigSvc already initialised' )

            return self.GetPlugin()


        def InitialiseSvc(self):
            if self.initialised:
                raise (RuntimeError, 'athena service has already been added, do nothing.')

            self.initialised = True

            from AthenaCommon.AppMgr import ServiceMgr


            self.mlog.info( "initialising TrigConfigSvc using state %s", self.states )
            if 'xml' in self.states:
                from TriggerJobOpts.TriggerFlags import TriggerFlags


                if TriggerFlags.doLVL2() or TriggerFlags.doEF() or TriggerFlags.doHLT() or TriggerFlags.configForStartup()=='HLToffline':
                    self.mlog.info( "setup HLTConfigSvc and add instance to ServiceMgr (xml file="+self.hltXmlFile+")" )
                    hlt = HLTConfigSvc("HLTConfigSvc")
                    hlt.XMLMenuFile = self.hltXmlFile
                    hlt.doMergedHLT = TriggerFlags.doHLT()
                    ServiceMgr += hlt
                else:
                    self.mlog.info( "Will not setup HLTConfigSvc, since TriggerFlags doLVL2(), doEF(), and doHLT() are all False" )
                    self.states[self.states.index("xml")] = "xmll1"

                self.mlog.info( "setup LVL1ConfigSvc and add instance to ServiceMgr (xml file="+self.l1XmlFile+")" )
                l1 = LVL1ConfigSvc("LVL1ConfigSvc")
                l1.XMLMenuFile = self.l1XmlFile
                ServiceMgr += l1

                self.mlog.info( "setup L1TopoConfigSvc and add instance to ServiceMgr (xml file="+self.l1topoXmlFile+")" )
                l1topo = L1TopoConfigSvc()
                l1topo.XMLMenuFile = self.l1topoXmlFile
                ServiceMgr += l1topo


            if 'ds' in self.states:

                self.mlog.info( "setup DSConfigSvc and add instance to ServiceMgr" )
                ds = DSConfigSvc("DSConfigSvc")
                ServiceMgr += ds

            self.mlog.info( "setup TrigConfigSvc and add instance to ServiceMgr" )
            trigSvc = TrigConfigSvc("TrigConfigSvc")
            trigSvc.PriorityList = self.states
            ServiceMgr += trigSvc
            from AthenaCommon.AppMgr import theApp
            theApp.CreateSvc += [ ServiceMgr.TrigConfigSvc.getFullName() ]

        def GetPlugin(self):
            if not self.initialised:
                raise (RuntimeError, 'athena service has not been added, cannot return plugin!.')
            from AthenaCommon.AppMgr import ServiceMgr
            return ServiceMgr.TrigConfigSvc

    # storage for the instance reference
    __instance = None

    def __init__(self):
        """ Create singleton instance """
        # Check whether we already have an instance
        if SetupTrigConfigSvc.__instance is None:
            # Create and remember instance
            SetupTrigConfigSvc.__instance = SetupTrigConfigSvc.__impl()

        # Store instance reference as the only member in the handle
        self.__dict__['_SetupTrigConfigSvc__instance'] = SetupTrigConfigSvc.__instance

    def __getattr__(self, attr):
        """ Delegate access to implementation """
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        """ Delegate access to implementation """
        return setattr(self.__instance, attr, value)


def TrigConfigSvcCfg( flags ):
    from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg
    return TrigConfigSvcCfg( flags )

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()
    acc = TrigConfigSvcCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")
