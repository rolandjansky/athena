#! /usr/bin/env python

# module: PostProcessValgrind.py

from Logger import Logger
import os
import subprocess
import glob

class PostProcessValgrind:
   def __init__(self,argDict={}):
      
      self.logger    = Logger()
      descriptor = argDict.get('JobDescriptor', None)

      self.RttName = descriptor.name
      self.DisName = descriptor.jobDisplayName

      self.NightlyType  = descriptor.paths.branch     # ==> e.g. dev, devval, 15.4.X.Y, etc.
      self.ProjectName  = descriptor.paths.topProject # ==> AtlasProduction, AtlasTier0, etc.
      # self.cmtConfigStr = descriptor.paths.cmtConfig  # ==> e.g. i686-slc4-gcc34-opt
      self.PackageTag   = descriptor.paths.packageTag # ==> e.g. PackageTag-00-01-20
      self.resPath      = descriptor.resPath
      
   def run(self):

      releaseName = os.environ['AtlasVersion']
      atlasArea   = os.environ['AtlasArea']
      cmtConfig   = os.environ['CMTCONFIG']
      
      fStr = [ '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n',
               '<html> <head>\n',
               '<title>Valgrind Report for ' + self.RttName + ' release ' + releaseName + ' of project ' + self.ProjectName + '</title>\n',
               '</head> <body>\n',
               '<h1>Valgrind Report for: ' + self.RttName + '</h1>\n',
               '<table border="0">\n',
               '<tr> <td>release</td> <td>' + releaseName + '</td> </tr> \n',
               '<tr> <td>project</td> <td>' + self.ProjectName + '</td> </tr> \n',
               '<tr> <td>AtlasArea points to </td> <td>' + atlasArea + '</td> </tr> \n',
               '<tr> <td>CMTCONFIG </td> <td>' + cmtConfig + '</td> </tr> \n' ]
      
      fStrVetoed = [ '</table>\n', '<h3>JOB WAS VETOED ! </h3>\n', '</body>\n', '</html>\n' ]
      
      f = open("Report.html", "w")
      f.writelines(fStr)
      if os.path.isfile('veto.log'):
         f.writelines(fStrVetoed)
         f.close()
         return -1
      f.close()
      
      # post processing the valgrind output
      self.logger.info("PostProcessValgrind run()")

      runNoTool=True
      # did we run memcheck ?
      if len(glob.glob("valgrind.out.process.*"))>0:
         runNoTool=False
         self.logger.info( os.system( 'ls -l processValgrindOutput.sh') )
         
         #try:
         #   r, w = popen2.popen4( 'processValgrindOutput.sh' )
         #except IOError:
         #   self.logger.info( 'not good. something is wrong' )
         #   exit(-1)
         
         try:
            os.environ['VG_RDIR'] = self.resPath
            p = subprocess.Popen(["./processValgrindOutput.sh"], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
            (child_stdout, child_stdin) = (p.stdout, p.stdin)
            # subprocess.STDOUT
            out = child_stdout.readlines()
            for i in out:
               self.logger.info( i )
            child_stdout.close()
            
         except IOError:
            self.logger.info( 'not good. something is wrong' )
            return -1
            
      if len(glob.glob("callgrind.out.process.*"))>0:
         runNoTool=False
         # grep -l 'bin/athena.py' valgrind.out.process.*
         try:
            # r, w = popen2.popen4( 'callgrind_annotate `grep -l "bin/athena.py" callgrind.out.process.*` > callgraph.log' )
            # p = subprocess.Popen(["callgrind_annotate `grep -c -e 'bin/athena.py' -e 'summary: [1-9]' callgrind.out.process.* | grep ':2$' | sed 's/:2$//g'`"], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
            p = subprocess.Popen(["callgrind_annotate --threshold=90 --inclusive=yes --tree=calling t/callgrind.out.process.*"], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
            (child_stdout, child_stdin) = (p.stdout, p.stdin)
            # subprocess.STDOUT
            out = child_stdout.readlines()
            
            f = open("Report.html", "a")
            f.writelines("Top consumers:</br><pre>\n")
            for i in out:
               self.logger.info(i)
               f.writelines(i)
            child_stdout.close()
            f.writelines("</pre>\n")
            f.close()
            
         except IOError:
            self.logger.info( 'not good. something is wrong' )
            return -1
         self.logger.info( os.system( 'gzip callgrind.out.process.*') )
         
      if len(glob.glob("massif.out.process.*"))>0:
         runNoTool=False
         # grep -l 'bin/athena.py' valgrind.out.process.*
         try:
            r, w = popen2.popen4( 'ms_print `grep -l "bin/athena.py" massif.out.process.*` > massif.log' )
         except IOError:
            self.logger.info( 'not good. something is wrong' )
            return -1
         self.logger.info( os.system( 'gzip massif.out.process.*') )

      if runNoTool:
         try:
            os.environ['VG_RDIR'] = self.resPath
            p = subprocess.Popen(["./dhat-postprocess.pl"], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
            (child_stdout, child_stdin) = (p.stdout, p.stdin)
            # subprocess.STDOUT
            out = child_stdout.readlines()
            for i in out:
               self.logger.info( i )
            child_stdout.close()
            
         except IOError:
            self.logger.info( 'not good. something is wrong' )
            return -1

      # out = r.readlines()
      # for i in out:
      #    self.logger.info( i )
      # r.close()
      
      return 0
