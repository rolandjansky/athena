#!/bin/env python
##
# @file perfMonDozer.py 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief new implementation of RTT PerfMonDozer client 

import sys
sys.argv.append("-b")
sys.path = [ "/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/latest" ] + sys.path
sys.rtt = True

from CoolDozer import *
## will use pmon.gz files 
import PerfMonComps.PerfMonSerializer as pmon_ser
import re
import logging


class unit( object ):

    lut = { "kiB" : -10, "MiB" : 0, "GiB" : 10, "TiB" : 20 }

    @classmethod
    def __call__( src,  dest ):
        
        pass


##
# @class perfMonDozer::perfMonDozer
# @extends python::RTTDozerApp::RTTDozerApp
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief interface between PerMonSvc and CoolDozer for RTT
#
class perfMonDozer( RTTDozerApp ):


    rePerfMonFile = re.compile( "^Py:PerfMonSvc[\s]+INFO Writing out collected data\.\.\.")
    rePerfMonLine = re.compile("^Py:PerfMonSvc")

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict argument dictionary passed from RTT
    def __init__(self, argDict={} ):
        super( perfMonDozer, self ).__init__( argDict )
        self.pmonFile = None # "ntuple.pmon.gz"

    ## general DozerSchema getter
    # @param self "Me, myself and Irene"
    # @return DozdrSchema instance
    def schema( self ):

        # old channel, this one would be empty
        #oldChannel  = DozerChannel( self.jobId, "PerfMonDozer run")
        #oldChannel += DozerData("CPU per evt", "Float", "ms")
        #oldChannel += DozerData("CPU per evt;error", "Float", "ms")
        #oldChannel += DozerData("Real time per evt", "Float", "ms")
        #oldChannel += DozerData("Real time per evt;error", "Float", "ms")
        #oldChannel += DozerData("Vmem per evt", "Float", "MB")
        #oldChannel += DozerData("Vmem per evt;error", "Float", "MB")
        #oldChannel += DozerData("Malloc per evt", "Float", "MB")
        #oldChannel += DozerData("Malloc per evt;error", "Float", "MB")
        #oldChannel += DozerData("NormCPU per evt", "Float", "kSi2k")
        #oldChannel += DozerData("Si2k constant", "Float")
        #oldChannel += DozerData("MemLeak per evt", "Float", "kB")
        #oldChannel += DozerData("MemLeak per evt:Chi2", "Float")
        #oldChannel += DozerData("MemLeak per evt:NDF", "Int")
        
        # general info about the job
        infoChannel = DozerChannel("jobInfo", "holds job statistics")
        infoChannel += DozerData("Host", "String" )
        infoChannel += DozerData("N CPUs", "Int", "" )
        infoChannel += DozerData("Max swap", "Float", "MB" )
        infoChannel += DozerData("Max mem", "Float", "MB" )
        infoChannel += DozerData("Host type", "String" )
        infoChannel += DozerData("Host model", "String" )
        infoChannel += DozerData("CPU factor", "Float", "kSi2k" )
        infoChannel += DozerData("Si2k", "Float")
        infoChannel += DozerData("Events", "Int" )
        infoChannel += DozerData("Algorithms", "Int" )
        
        ## init phase 
        iniChannel = DozerChannel("ini", "PerfMon data from initialisation phase")
        iniChannel += DozerData("Nentries", "Int", "")
        iniChannel += DozerData("CPU per evt", "Float", "ms")
        iniChannel += DozerData("CPU per evt;error", "Float", "ms")
        iniChannel += DozerData("Real time per evt", "Float", "ms")
        iniChannel += DozerData("Real time per evt;error", "Float", "ms")
        iniChannel += DozerData("Vmem per evt", "Float", "MB")
        iniChannel += DozerData("Vmem per evt;error", "Float", "MB")
        iniChannel += DozerData("Malloc per evt", "Float", "MB")
        iniChannel += DozerData("Malloc per evt;error", "Float", "MB")
        iniChannel += DozerData("Nalloc per evt", "Float", "MB")
        iniChannel += DozerData("Nalloc per evt;error", "Float", "MB")
        
        ## just after event 0
        evt0Channel = DozerChannel("evt0", "PerfMon data from event 0")
        evt0Channel += DozerData("Nentries", "Int", "")
        evt0Channel += DozerData("CPU per evt", "Float", "ms")
        evt0Channel += DozerData("CPU per evt;error", "Float", "ms")
        evt0Channel += DozerData("Real time per evt", "Float", "ms")
        evt0Channel += DozerData("Real time per evt;error", "Float", "ms")
        evt0Channel += DozerData("Vmem per evt", "Float", "MB")
        evt0Channel += DozerData("Vmem per evt;error", "Float", "MB")
        evt0Channel += DozerData("Malloc per evt", "Float", "MB")
        evt0Channel += DozerData("Malloc per evt;error", "Float", "MB")
        evt0Channel += DozerData("Nalloc per evt", "Float", "MB")
        evt0Channel += DozerData("Nalloc per evt;error", "Float", "MB")
        
        ## after processing of all events
        evtChannel = DozerChannel("evt", "PerfMon data from event processing phase")
        evtChannel += DozerData("Nentries", "Int", "")
        evtChannel += DozerData("CPU per evt", "Float", "ms")
        evtChannel += DozerData("CPU per evt;error", "Float", "ms")
        evtChannel += DozerData("Real time per evt", "Float", "ms")
        evtChannel += DozerData("Real time per evt;error", "Float", "ms")
        evtChannel += DozerData("Vmem per evt", "Float", "MB")
        evtChannel += DozerData("Vmem per evt;error", "Float", "MB")
        evtChannel += DozerData("Malloc per evt", "Float", "MB")
        evtChannel += DozerData("Malloc per evt;error", "Float", "MB")
        evtChannel += DozerData("Nalloc per evt", "Float", "MB")
        evtChannel += DozerData("Nalloc per evt;error", "Float", "MB")
        
        ## after finalization of all algs/tools
        finChannel = DozerChannel("fin", "PerfMon data from finalisation phase")
        finChannel += DozerData("Nentries", "Int", "")
        finChannel += DozerData("CPU per evt", "Float", "ms")
        finChannel += DozerData("CPU per evt;error", "Float", "ms")
        finChannel += DozerData("Real time per evt", "Float", "ms")
        finChannel += DozerData("Real time per evt;error", "Float", "ms")
        finChannel += DozerData("Vmem per evt", "Float", "MB")
        finChannel += DozerData("Vmem per evt;error", "Float", "MB")
        finChannel += DozerData("Malloc per evt", "Float", "MB")
        finChannel += DozerData("Malloc per evt;error", "Float", "MB")
        finChannel += DozerData("Nalloc per evt", "Float", "MB")
        finChannel += DozerData("Nalloc per evt;error", "Float", "MB")
        
        ## fit channel
        fitChannel = DozerChannel("fit", "fit results from PerfMon")
        fitChannel += DozerData("MemLeak per evt", "Float", "kB")
        fitChannel += DozerData("MemLeak per evt:Chi2", "Float")
        fitChannel += DozerData("MemLeak per evt:NDF", "Int")
    
        ## createw general schema
        schema  = DozerSchema()
        ## add channels
        schema += oldChannel 
        schema += infoChannel
        schema += iniChannel
        schema += evt0Channel
        schema += evtChannel
        schema += finChannel
        schema += fitChannel
        # return schema to app
        return schema

    ## fill in jobInfo channel
    # @param self "Me, myself and Irene"
    def addInfo( self ):
  
        if ( "HOST_NAME" in self.infos["hosts_infos"] ):
            self.dozerSchema["jobInfo"]["Host"].setValue( self.infos["hosts_infos"]["HOST_NAME"]  )
        if ( "ncpus" in self.infos["hosts_infos"] ):
            try:
                ncpus = int(self.infos["hosts_infos"]["ncpus"])
                self.dozerSchema["jobInfo"]["N CPUs"].setValue( ncpus )
            except ValueError:
                self.epanic("cannot set 'N CPUs' data, ['hosts_infos']['ncpus'] = " + self.infos["hosts_infos"]["ncpus"] + " isn't int?")
                
        if ( "cpuf" in self.infos["hosts_infos"] ):
            try:
                cpuf = float( self.infos["hosts_infos"]["cpuf"] )
                self.dozerSchema["jobInfo"]["CPU factor"].setValue( cpuf )
            except:
                self.epanic("cannot set 'CPU factor' data, ['hosts_infos']['cpuf'] = " + self.infos["hosts_infos"]["cpuf"] + " isn't float?")

        if ( "type" in self.infos["hosts_infos"] ):
            self.dozerSchema["jobInfo"]["Host type"].setValue( self.infos["hosts_infos"]["type"] )

        if ( "model" in self.infos["hosts_infos"] ):
            self.dozerSchema["jobInfo"]["Host model"].setValue(  self.infos["hosts_infos"]["model"] )
     
        if ( "si2k" in self.infos["hosts_infos"] ):
            try:
                si2k = float(self.infos["hosts_infos"]["si2k"])
                self.dozerSchema["jobInfo"]["Si2k"].setValue( si2k )
            except ValueError:
                self.epanic("cannot set 'Si2k' data, ['hosts_infos']['si2k'] = " + self.infos["hosts_infos"]["si2k"] + " isn't float?") 
        
        if ( "maxmem" in self.infos["hosts_infos"] ):
            try:
                maxmem = self.infos["hosts_infos"]["maxmem"]
                if ( maxmem.endswith("M") ): maxmem = maxmem[0:-1]
                maxmem = float( maxmem )
                self.dozerSchema["jobInfo"]["Max mem"].setValue( maxmem )
            except:
                self.epanic("cannot set 'Max mem' data, ['hosts_infos']['maxmem'] = " + self.infos["hosts_infos"]["maxmem"] + " isn't float?")

        if ( "maxswp" in self.infos["hosts_infos"] ):
            try:
                maxswp= self.infos["hosts_infos"]["maxswp"]
                if ( maxswp.endswith("M") ): maxswp = maxswp[0:-1]
                maxswp = float( maxswp )
                self.dozerSchema["jobInfo"]["Max swap"].setValue( maxswp )
            except:
                self.epanic("cannot set 'Max swap' data, ['hosts_infos']['maxswp'] = " + self.infos["hosts_infos"]["maxswp"] + " isn't float?")


    ##
    # @param self "Me, myself and Irene"
    # @param channel channel name
    # @param source a dict 
    def addPhase( self, channel, source ):
        self.info("adding phase: " + channel)

        if ( "nentries" in source ):
            self.dozerSchema[channel]["Nentries"].setValue( source["nentries"])

        if ( "vmem" in source ):
            val, err, unit = source["vmem"]
            self.dozerSchema[channel]["Vmem per evt"].setValue(val)
            self.dozerSchema[channel]["Vmem per evt;error"].setValue(err)

        if ( "real" in source ):
            val, err, unit = source["real"]
            self.dozerSchema[channel]["Real time per evt"].setValue(val)
            self.dozerSchema[channel]["Real time per evt;error"].setValue(err)
            
        if ( "cpu" in source ):
            val, err, unit = source["cpu"]
            self.dozerSchema[channel]["CPU per evt"].setValue(val)
            self.dozerSchema[channel]["CPU per evt;error"].setValue(err)

        if ( "malloc" in source ):
            va, err, unit = source["malloc"]
            self.dozerSchema[channel]["Malloc per evt"].setValue(val)
            self.dozerSchema[channel]["Malloc per evt;error"].setValue(err)
            
        if ( "nalloc" in source ):
            val, err, unit = source["nalloc"]
            self.dozerSchema[channel]["Nalloc per evt"].setValue(val)
            self.dozerSchema[channel]["Nalloc per evt;error"].setValue(err)
        

    ## writer
    # @param self "Me, myself and Irene"
    # @brief filling schema with data
    def writer(self):
        
        self.PerfMonLines = []
        self.PerfMonFiles = []
        try:
            for line in open( self.logFile, "r"):
                if ( self.rePerfMonLine.match( line ) ):
                    self.PerfMonLines.append( line )
                    self.debug("found line: " + line.strip() )
                    if self.rePerfMonFile.match( line ):
                        self.PerfMonFiles.append( line )
                        self.debug("found file: " + line.strip() )
        except IOError:
            self.epanic("cannot open job's log file '" + self.logFile + "' for reading!")
       
        
        for line in self.PerfMonFiles:
            search = re.compile("\[.+\]").search( line )
            if ( search and self.pmonFile == None ):
                self.pmonFile = search.group().lstrip("[").rstrip("]")
                self.info("will read PerfMonSvn data from " + self.pmonFile + " file")
     
        if ( not self.pmonFile ):
            self.panic("cannot find PerfMonSvn file!")
        else:
            try:
                self.infos, self.data = pmon_ser.pmon_load(self.pmonFile)
            except:
                self.epanic( "!!! problem deserialising perfMon file: " + self.pmonFile + " !!!" )
                return 1
        
        
        self.addInfo( )

        if ( "perf_summary" in self.infos ):
            for phase in ["ini", "evt0", "evt", "fin" ]:
                if ( phase in self.infos["perf_summary"] ):
                    self.debug( "[perf_summary][" + phase + "] record found" )
                    self.addPhase( phase, self.infos["perf_summary"][phase] )
                else:
                    self.error( "[perf_summary][" + phase + "] record not found" )
                
        print self.dozerSchema
        sys.exit(0)
      

if __name__ == "__main__":

    ##
    # @class Paths
    # @brief fake RTT Paths
    class Paths:
        def __init__(self):
            self.release    = "rel_0"
            self.branch     = "dev"
            self.runType    = "build"
            self.cmtConfig  = "i686-slc4-gcc34-opt"
            self.topProject = "AtlasProduction"
            self.packageTag = "CoolDozer-00-00-33"
    ##
    # @class Desc
    # @brief fake RTT JobDescriptor for testing
    class Desc:
        def __init__(self):
            self.package    = "CoolDozer"
            self.trendId    = "testJob_evo"
            self.runPath    = "."
            self.paths      = Paths()
            self.log        = "RecExRecoTest_RTT_esd1_log"
               
    testCfg = { "JobDescriptor"    : Desc(),
                "LastNDays"        : 30,
                "Fit"              : True, 
                "Histos"           : True, 
                "DryRun"           : True,
                "Style"            : "Dozer" }

    app = perfMonDozer( testCfg )
    app.run()
    
