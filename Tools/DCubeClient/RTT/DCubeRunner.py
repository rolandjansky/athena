# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## 
# @module DCubeRunner
# @author Krzysztof Daniel Ciba
# @date 4 Sep 2007
# @brief Glue module to plugin DCube to Atlas RTT. 
#
# * Feb 4, 2007 - switch to DCubeClient-00-00-13
#                 and DCubeServer-00-00-08
# * Apr 10, 2008 - switch to DCubeServer-00-00-10
# * Nov 7, 2008 - switch to new DCubeServer-00-00-11
# * Nov 11, 2008 - switch to new DCubeClient-00-00-19 and DCubeServer-00-00-12 
# 

## some imports 
import xml.dom.minidom as minidom
import os, sys
import os.path 
import re

## @class DCubeRunner
#  @brief prepares and runs shell script with DCube command 
class DCubeRunner:

      ## DCubeClient API classes
      __reAPIClasses = [ re.compile("DCubeApp"), 
                         re.compile("DCubeConfig"), 
                         re.compile("DCubeTester"), 
                         re.compile("DCubePlotter"), 
                         re.compile("DCubePHPWriter") ]
      
      ## base directory for references
      baseRef = "/afs/cern.ch/atlas/project/RTT/prod/Results/reference/"

      ## DCubeServer directory
      dcubeServer = "/afs/cern.ch/atlas/project/RTT/prod/Results/dcube/latest/"

      ## DCubeClient directory
      dcubeClient = "/afs/cern.ch/atlas/project/RTT/DCubeClient/latest/python"

      ## c'tor
      # @param self "Me, myself and Irene"
      # @param argDict RTT action runner dictionary 
      def __init__( self, argDict={} ):

            try:
                  from Logger import Logger
                  self.logger = Logger()
            except ImportError:
                  self.logger = False

            self.error = None 

            self.debug("DCubeRunner constructor is getting args from dictionary...")

            self.cfgFile = None 
            if ( "DCubeCfg" in argDict ):
                  self.cfgFile = argDict["DCubeCfg"]
            else:
                  self.info( "*** No DCubeCfg in DCubeRunner action dictionary ***" )
                  self.error = "No DCubeCfg in DCubeRunner argDict"

            self.refFile = None 
            if ( "DCubeRef" in argDict ): 
                  self.refFile = argDict["DCubeRef"]
            else:
                  self.info( "*** No DCubeRef in DCubeRunner action dictionay, will use value from config XML file ***" )

            self.monFile = None 
            if ( "DCubeMon" in argDict ):
                  self.monFile = argDict["DCubeMon"]
            else:
                  self.info( "*** No DCubeMon in DCubeRunner action dictionary ***" )
                  self.error = "No DCubeMon in DCubeRunner argDict"

            if ( "DCubeJobId" in argDict ):
                  self.jobId = argDict["DCubeJobId"]
            else:
                  self.jobId = None

            jobDtor = argDict.get('JobDescriptor', None )

            if ( jobDtor != None ):
                  self.DCubeForRTTCfg = jobDtor.paths.dCubeCfgFile
                  self.outputPath = jobDtor.runPath
                  self.installArea = jobDtor.paths.installArea
                  self.cvsPath = 'offline/' + jobDtor.paths.containerPackage
                  self.cmtPath = jobDtor.paths.cmtPath
                  self.nightly = jobDtor.paths.branch # branch
                  self.install = jobDtor.paths.runType #  install
                  self.cmtconfig = jobDtor.paths.cmtConfig  # CMTCONFIG
                  self.project = jobDtor.paths.topProject # Atlas project name
                  self.release = jobDtor.paths.release # release name
            else:
                  ## dummy values for testing
                  self.DCubeForRTTCfg = "./DCubeForRTTCfg.xml"
                  self.outputPath  = "./"
                  self.installArea = "./"
                  self.cvsPath     = "offline/Tools/DCubeClient"
                  self.cmtPath     = "."
                  self.nightly     = "dev"
                  self.install     = "build"
                  self.cmtconfig   = "i686-slc5-gcc43-opt"
                  self.project     = "AtlasProduction"
                  self.release     = "rel_1"

            self.debug("[01] will use '%s' as DCubeClient configuration file for RTT" % self.DCubeForRTTCfg )
            self.debug("[02] will use '%s' as output path" % self.outputPath )
            self.debug("[03] will use '%s' as install area path" % self.installArea )
            self.debug("[04] will use '%s' as package CVS path" % self.cvsPath )
            self.debug("[05] will use '%s' as cmt path" % self.cmtPath )
            self.debug("[06] will use DCubeClient monitored file '%s'" % self.monFile )
            self.debug("[07] will use DCubeClient reference file '%s'" % self.refFile )
            self.debug("[08] will use DCubeClient configuration file '%s'" % self.cfgFile )
            self.debug("[09] will use '%s' as DCubeClient branch name" % self.nightly )
            self.debug("[10] will use '%s' as DCubeClient install name" % self.install )
            self.debug("[11] will use '%s' as DCubeClient cmtconfig name" % self.cmtconfig )
            self.debug("[12] will use '%s' as DCubeClient project name" % self.project  )
            if ( self.jobId ):
                  self.debug("[13] will use '%s' as DCubeClient jobId" % self.jobId )
            else:
                  self.debug("[13] DCubeClient jobId not set!")
                  
            self.command = [ ]
            if ( self.error == None ):
                  for line in self.__parseDCubeRTTCfg():
                        self.command.append( line ) 
            msg = "command to run is:\n"+20*"-"+"\n"
            for line in self.command:
                  msg += line
            msg += "\n"+20*"-"+"\n"
            self.debug( msg )
            self.debug( "DCubeRunner is ready to work...")
            
      ## run method
      # @param self "Me, myself and Irene"
      def run( self ):
            if ( self.error == None ):
                  try:
                        from ShellCommand import ShellCommand
                        # FIXME - self.logger could be just python False...
                        sc = ShellCommand( self.command,
                                           self.logger ) 
                        replay = sc.getReply()
                        reLine = re.compile("OVERALL STATISTICS STATUS")
                        status = 1
                        for line in replay:
                              for apiClass in self.__reAPIClasses:
                                    if ( apiClass.search( line ) ):
                                          self.debug( line )
                                          if ( reLine.search( line ) ):
                                                if ( "OK" in line ): status = 0
                                    
                        return status
                  except ImportError: 
                        self.info("No ShellCommand in PYTHONPATH! 'No Cin-Cin, no run...'")
                        return 1
            else:
                  self.info(self.error)
                  return 1

      ## XML parsing of DCubeForRTTCfg.xml 
      # @param self "Me, myself and Irene"
      def __parseDCubeRTTCfg( self ):
            self.debug( "parsing DCubeRunner configuration file %s" % self.DCubeForRTTCfg )
            try:
                  xmldoc = minidom.parse( self.DCubeForRTTCfg )   

                  self.refBaseDir = self.getText( xmldoc.getElementsByTagName("ref_dir")[0].childNodes )
                  self.debug( "[14] will use '%s' as base reference directory" % self.refBaseDir )

                  self.dcubeServer = self.getText( xmldoc.getElementsByTagName("server")[0].childNodes ) 
                  self.debug( "[15] will use DCubeServer installed in %s" % self.dcubeServer )
         
            except:
                  self.error = "error when parsing DCubeRunner configuration file, no run will be preformed"
                  return [ "" ]

            self.debug("constructing command to run DCube...")
            out = [ ]
            words = self.cmtconfig.split("-")
            if ( words[0] == "i686" ):
                  bin = "32"
            else:
                  bin = "64"
            gcc = words[2]
            opt = words[3]

            tag = ",".join( [self.release, self.nightly, bin, gcc, opt, self.project, "forceConfig" ])
            out.append("export CMTCONFIG="+self.cmtconfig+"\n")
            out.append("source ~/cmthome/setup.sh -tag=%s\n" % ( tag )  )
            out.append( "export PATH=" + self.dcubeClient + ":${PATH}\n")
            out.append( "export PYTHONPATH=" + self.dcubeClient + ":${PYTHONPATH}\n")
            
            # changing the outputPath 
            monPath = self.outputPath 

            self.outputPath = self.outputPath + os.sep +  "DCube-" + self.refFile

            # creating DCube subdirectory
            out.append("mkdir -p " + self.outputPath + "\n")

            dcubePath = self.dcubeClient
                
            dcubeConvert = os.path.join( dcubePath, "./dcubeConvert.py")

            run = os.path.join(dcubePath,"./dcube.py")
            run += " -p " # generate plot
            run += " --branch " + self.nightly # set branch
            run += " --install " + self.install # set install
            run += " --cmtconfig " + self.cmtconfig # set cmtconfig
            run += " --project " + self.project # set project
            # set jobId
            if ( self.jobId ): 
                  run += " --jobId " + self.jobId 
      
                  
            run += " -l " + self.outputPath + os.sep + self.monFile + ".dcube.log" # save log to monitored.dcube.log
            run += " -s " + self.dcubeServer  # path to the DCubeServer installation
            run += " -x " + self.outputPath + os.sep + self.monFile + ".dcube.xml " # save output to monitored.dcube.xml
                 
            # dangerous hacking at the moment... 
            if ( os.path.isabs( self.cfgFile ) or "DUMMY-TEST" in self.cfgFile ):
                  run += " -c " + self.cfgFile 
                  config = self.cfgFile
            else: 
                  # checking cfg file in reference dir
                  refCfg = "/afs/cern.ch/atlas/project/RTT/Results/reference/" + self.cvsPath + os.sep + self.cfgFile 
                  self.debug("will check the timestamp of DCube configuration file (reference volume): " + refCfg )
                  refCfgStat = None

                  if ( os.path.exists(refCfg) ):
                        refCfgStat = os.stat(refCfg)
                        self.debug("File %s modified %d" %( refCfg, refCfgStat[8]) )
                  else:
                        self.debug("DCube configuration file %s not found on 'reference' volume" % self.cfgFile )
                        
                        # checking cfg file in cvs co dir 
                        
                  cvsCfg = self.cmtPath + os.sep + self.cvsPath.lstrip("offline") + "/test/" + self.cfgFile
                  self.debug("will check the timestamp of DCube configuration file (CVS checkout volume):" + cvsCfg)
                  cvsCfgStat = None 
                  if ( os.path.exists(cvsCfg) ) :
                        cvsCfgStat = os.stat( cvsCfg )
                        self.debug("File %s modified %d" %( cvsCfg, cvsCfgStat[8]) )
                  else:
                        self.debug("DCube configuration file %s not found in CVS checkout directory" % self.cfgFile )

                        
                  # choose configuration file based on latest timestamp 
                  config = None

                  # both are present, check time stamps
                  if ( None not in ( refCfgStat, cvsCfgStat) ):
                        # install area newer
                        if ( refCfgStat[8] <= cvsCfgStat[8] ):
                              config = cvsCfg
                        else: 
                              config = refCfg

                  # reference is present
                  elif ( None != refCfgStat ):
                        config = refCfg
                  # install area is present
                  elif ( None != cvsCfgStat ):
                        config = cvsCfg
                  # both are absent  
                  else:
                        pass
        
                  if ( config != None ):
                        run += " -c " + config # add configuration file 
                  else:
                        self.debug("DCube configuration file %s not found on afs reference  volume or InstallArea directories" % self.cfgFile )
                        return [ "" ]
                  
            if ( self.refFile != None ):
                  run += " -r " + os.path.join( os.path.join(self.refBaseDir , self.cvsPath) , self.refFile ) # overwrite reference
                  run += " " + os.path.join( monPath , self.monFile)  # add monitored file at the end 
                  

            if ( "DUMMY-TEST" in self.cfgFile ):
                  dcubeConvert += " %s " % os.path.abspath(self.cfgFile)
            else:
                  dcubeConvert += " %s " % os.path.abspath(config)

            dcubeConvert += " %s\n"  % os.path.join( os.path.join(self.refBaseDir , self.cvsPath) , self.refFile )  
            out.append( dcubeConvert )
            out.append( run + "\n")
                  
            return out
            

      ## logger level INFO
      # @param self "Me, myself and Irene"
      # @param msg message
      def info( self, msg ):
            if ( bool(self.logger) ):
                  self.logger.info( str(msg) )
            else:
                  print "DCubeRunner INFO  " + str(msg) 

      ## logger level DEBUG
      # @param self "Me, myself and Irene"
      # @param msg message 
      def debug( self, msg ):
            if ( bool(self.logger) ):
                  self.logger.debug( str( msg) )
            else:
                  print "DCubeRunner DEBUG " + str(msg)

      
      ## get CDATA section
      # @param self "me, myself and Irene"
      # @param nodelist list of XML DOM Nodes
      def getText( self, nodelist ):
        out = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                out = out + node.data
        return out 

      ## split path
      # @param self "Me, myself and Irene"
      # @param path path to split
      # @param rest splitted tail
      def pathSplit(self, path, rest=[] ):
            ( head , tail ) = os.path.split( path )
            if len(head) < 1: return [tail]+ rest
            if len(tail) < 1: return [head]+ rest
            return self.pathSplit( head, [tail] + rest )

      ## find common names in path
      # @param self "Me, myself and Irene"
      # @param l1 first list with splitted path
      # @param l2 second  lust with splitted path
      # @param common list of common dirnames
      def commonPath(self,  l1, l2, common=[] ):
            if len(l1) < 1: return (common, l1, l2)
            if len(l2) < 1: return (common, l1, l2)
            if l1[0] != l2[0]: return (common, l1, l2)
            return self.commonPath( l1[1:], l2[1:], common+[l1[0]])

      ## finds relative path between p1 and p2
      # @param self "Me, myself and Irene"
      # @param p1 first path
      # @param p2 second path
      def relPath(self, p1, p2):
            ( common, l1, l2 ) = self.commonPath( self.pathSplit(p1), self.pathSplit(p2))
            p = []
            if len(l1) > 0:
                  p = [ '../' * len(l1) ]
            p = p + l2
            return os.path.join( *p )


## dummy test running
if __name__ == "__main__":
      print "running DCubeRunner..."
      testCfg = { "DCubeCfg"         : "test_dcube_cfg.xml",
                  "DCubeMon"         : "monitored.root",
                  "DCubeRef"         : "reference.root",
                  "DCubeJobId"       : "test_01",
                  "JobDescriptor"    : None }
      dc = DCubeRunner( testCfg )
      dc.run()

      
