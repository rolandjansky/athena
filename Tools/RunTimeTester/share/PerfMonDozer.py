#!/bin/env python

import sys,time
sys.argv.append("-b")


dozerPath =  "/afs/cern.ch/atlas/project/RTT/Results/dozer/api" 
sys.path = [ dozerPath ] + sys.path
sys.path.append( dozerPath )

from CoolDozer import *
import RTTDozerUtils
import random
import os.path

class PerfMonDozer( object ):

    @RTTDozerUtils.init
    def __init__( self , argDict = {} ):
        jd = argDict.get("JobDescriptor", None )
        self.jobLog     = jd.log
        self.package    = jd.package
        self.release    = jd.paths.release
        self.nightly    = jd.paths.branch
        self.install    = jd.paths.runType
        self.cmtconfig  = jd.paths.cmtConfig
        self.project    = jd.paths.topProject
        self.jobId      = jd.trendId+'_perfMonDozer'
        self.runMe      = True
        
        if jd.trendId is None or not jd.trendId.strip(): 
            self.log.panic("Job has no trendID. Cannot perform PerfMonDozer magic.")
            self.runMe = False
            self.channelKey = None
            return

        self.log.info("Job has a trendID (%s), proceeding..." % jd.trendId)
        self.channelKey = self.jobId

    @RTTDozerUtils.run
    def run(self):
        if not self.runMe: return
        ## write latest to db
        self.write() 
            
        ## read data back and make some plots
        self.read()

    def createSchema(self):
        self.rConfig = DozerConfig("RTT",  self.log)
        
        rChannel  = DozerChannel(self.channelKey, "PerfMonDozer run")       
        rChannel += DozerData("CPU per evt", "Float", "ms")
        rChannel += DozerData("CPU per evt;error", "Float", "ms")
        
        rChannel += DozerData("Real time per evt", "Float", "ms")
        rChannel += DozerData("Real time per evt;error", "Float", "ms")

        rChannel += DozerData("Vmem per evt", "Float", "MB")
        rChannel += DozerData("Vmem per evt;error", "Float", "MB")

        rChannel += DozerData("Malloc per evt", "Float", "ms")
        rChannel += DozerData("Malloc per evt;error", "Float", "ms")

        rChannel += DozerData("NormCPU per evt", "Float", "kSi2k")
        rChannel += DozerData("Si2k constant", "Float")

        rChannel += DozerData("MemLeak per evt", "Float", "kB")
        rChannel += DozerData("MemLeak per evt:Chi2", "Float")
        rChannel += DozerData("MemLeak per evt:NDF", "Int")

        rSchema = DozerSchema("*", "*", "*", "*",  "*")
        rSchema += rChannel
        
        self.rConfig += rSchema 
        return self.rConfig.get(self.dbkey)
            
    def extractFromLog(self):
        class NotYetException(Exception):
            def __init__(self, msg):
                Exception.__init__(self, msg)
        class NoPerfMonException(Exception):
            def __init__(self, msg):
                Exception.__init__(self, msg)
        def readLog():
            h = open(self.jobLog)
            lines = h.readlines()
            h.close()
            return lines
        def extract(line):
            values, unit = [t.strip() for t in line.split('(')[1].split(')')]
            value, error = [t.strip() for t in values.split('+/-')]
            return float(value), float(error), unit
        def extractSi2k(line):
            si2kConst, rest = [t.strip() for t in line.split('Si2k:')[1].split('=>')]
            normCPU, normCPUunits = [t.strip() for t in rest.split(':')[1].strip().split()]
            return float(si2kConst), float(normCPU)
        def extractVmemFit(line):
            toks      = [t.strip() for t in line.split('INFO 000: [vmem-fit]')[1].strip().split()]
            memleak   = toks[2]
            chi2, ndf = toks[-1].split('/')
            return float(memleak), float(chi2), int(ndf)
            
        # grab the log contents
        lines = readLog()

        start = ["Py:PerfMonSvc", "Statistics for 'evt':"]
        stop  = ["Py:PerfMonSvc", "Statistics for 'fin':"]

        # reject jobs that don't have PerfMon on, or that
        # are chained jobs (for now, we deal with only single jobs)
        found = []
        for i,l in enumerate(lines):
            found_it = True
            for s in start:
                if s not in l:
                    found_it = False
                    break
            if found_it: found.append((i,l))
            
        # reject jobs that don't have PerfMon on, or that
        # are chained jobs (for now, we deal with only single jobs)
        # found =  [(i,l) for i,l in enumerate(lines) if l.find(start)!=-1]
        if len(found) > 1:
            m = "For now: not dealing with chained jobs"
            self.log.panic(m)
            raise NotYetException(m)

        if len(found)==0:
            m = "PerfMon not switched on in this job. Nothing to do."
            self.log.panic(m)
            raise NoPerfMonException(m)

        # OK, let's deal with this job
        startlineno, startline = found[0]

        searchTemplate = "Py:PerfMonSvc        INFO %s:"
        searchTerms = [{'offset':1, 'search':'<cpu>',    'label':'CPU per evt'},
                       {'offset':2, 'search':'<real>',   'label':'Real time per evt'},
                       {'offset':3, 'search':'<vmem>',   'label':'Vmem per evt'},
                       {'offset':4, 'search':'<malloc>', 'label':'Malloc per evt'}]

        dict = {}
        
        for searchTerm in searchTerms:
            lineno = startlineno + searchTerm['offset']
            line = lines[lineno]
            if line.find(searchTemplate % searchTerm['search']) !=-1:
                value, error, unit = extract(line)
                dict[searchTerm['label']] = value
                dict[searchTerm['label']+';error'] = error

        # Si2K
        lineno = startlineno + 10
        line   = lines[lineno]
        if line.find('Si2k:')!=-1:
            si2kConst, normCPU      = extractSi2k(line)
            dict['Si2k constant']   = si2kConst
            dict['NormCPU per evt'] = normCPU

        # Vmem fit
        for line in lines[lineno:]:
            if line.find('INFO 000: [vmem-fit]')!=-1:
                memleak, chi2, ndf = extractVmemFit(line)
                dict['MemLeak per evt'] = memleak
                dict['MemLeak per evt:Chi2'] = chi2
                dict['MemLeak per evt:NDF'] = ndf
        return dict

    def write(self):
        if self.channelKey in self.schema:
            channel = self.schema[self.channelKey]
            terms = self.extractFromLog()

            for key, newValue in terms.items():
                self.log.info( "setting value '%s' for DozerData '%s'" % ( str(newValue), key) )
                channel[key].setValue(newValue)
        
        # add to the DB
        self.log.debug("will add DozerSchema as payload...")
        self.db.addSchemaPayload( self.schema, DozerIOV().today() )
        self.log.debug("done!")

    def read(self):
        try:
            me = self.schema.pathChunk() + "/" + self.channelKey
            recordSet = self.db.get( me, since=DozerIOV.lastNDays(30))
        except:
            recordSet = None
            
        if not recordSet: 
            self.log.warn("Unable to retrieve channel with key: %s" % self.channelKey)
            return

        graph = recordSet.getDozerGraph()

    
        for plotable in graph.plotables():
            
            plotName = plotable.replace(" ", "_").lower()
            if ';error' in plotName: continue
            try:
                graph.trend( plotable, True, "perfMonDozer_%s.png" % plotName )
            except TypeError:
                pass

        
## dummy test running 
if __name__ == "__main__":
      print "Running PerfMonDozer..."
      class Paths:
          def __init__(self):
              self.release = 'rel_0'
              self.branch = 'dev'
              self.runType = 'build'
              self.targetCMTCONFIG='i686-slc4-gcc34-opt'
              self.topProject = 'AtlasProduction'
              
      class Desc:
          def __init__(self):
              self.log = '/afs/cern.ch/atlas/project/RTT/data/brinick/Results/rel_0/dev/build/i686-slc4-gcc34-opt/offline/CoolDozer/AthenaCoolDozer/CoolDozer_fakePerfMon_jobOptions/1/CoolDozer_fakePerfMon_jobOptions1_log'
              self.package    = 'CoolDozer'
              self.trendId    = 'ZZZ_BrinickTestJob'
              self.paths      = Paths()
              
      testCfg = { "JobDescriptor"    : Desc(),
                  "DryRun"           : True }
      dozerApp = PerfMonDozer( testCfg )
      print 'Initialisation done; calling run()'
      dozerApp.run()
      print 'Done'
