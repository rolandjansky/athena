# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string,os,re
from popen2           import Popen4
from formatCollection import formatCollection

class BaseAction:
    def __init__(self, filename):
        self.name = filename
        
    def isCastorFile(self):
        file = self.name
        result = False
        pieces = string.split(file, '/')
        try:
            if pieces[1] == 'castor':
                result=True
        except:
            print 'filename did not start with: /castor! '
        return result
    

class CopyCastorFile(BaseAction):
    def __init__(self , dsObject):
        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir
        BaseAction.__init__(self, self.name)

    def doIt(self):
        filename = self.name
        destDir  = self.destDir
        src      = self.dataset.dsSource
        print 'src : %s '%src
        if src == 'afs':
            print 'afs file ...'
            dest = os.path.join(destDir,
                                os.path.basename(filename))

            cmd = 'cp '+filename +' '+dest

            if not os.path.exists(filename):
                self.dataset.copyStatus = 'error'
                self.dataset.copyInfo   = 'file does not exist :'+filename

            elif not os.path.exists(dest):
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File copied to \n'
                self.dataset.copyInfo  += str(dest)
                
                print 'executing :'+cmd
                print 'please wait...'
                runCmd = Popen4(cmd)
                answer = str(runCmd.fromchild.read())
                print answer
                pattern ='(E|e)+rror'
                if re.search(pattern, answer):
                    Popen4('rm -f '+dest)
                    self.dataset.copyStatus ='error'
                    self.dataset.copyInfo   = answer
            else:
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File already exists '
                                                       
        elif src == 'castor':
            print 'castor file'
            dest = os.path.join(destDir,
                                os.path.basename(filename))
            cmd = '/afs/.cern.ch/project/gd/LCG-share/3.0.0/globus/bin/'
            #cmd  = '/usr/local/lcg/globus/bin/'
            cmd += 'globus-url-copy gsiftp://castorgrid.cern.ch'
            cmd +=  filename +' '+'file://'+dest
    
            if not os.path.exists(dest):
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File copied to \n'
                self.dataset.copyInfo  += str(dest)
                
                print 'executing :'+cmd
                print 'please wait...'
                runCmd =Popen4(cmd)
                answer = str(runCmd.fromchild.read())
                print answer
                pattern ='(E|e)+rror'
                if re.search(pattern, answer):
                    Popen4('rm -f '+dest)
                    self.dataset.copyStatus ='error'
                    self.dataset.copyInfo   = answer
            else:
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File already exists '
        else:
            print 'File src unknown, jumping...!!'
            self.dataset.copyStatus = 'error'
            self.dataset.copyInfo   = 'please give full castor or afs path!'
                                                


class ListCastorFile(BaseAction):
    
    def __init__(self , dsObject):
        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir
        BaseAction.__init__(self, self.name)

    def doIt(self):
        filename = string.strip(self.name)
        destDir  = self.destDir
        
        if not self.isCastorFile():
            print 'Not a castor file!!'
            self.dataset.copyStatus ='error'
            self.dataset.copyInfo   ='Not a castor file!!'
        else:
            dest = os.path.join(destDir,
                                os.path.basename(filename))
            cmd  = '/usr/local/lcg/edg/bin/'
            cmd += 'edg-gridftp-ls gsiftp://castorgrid.cern.ch'
            cmd +=  filename
            if not os.path.exists(dest):
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File copied to \n'
                self.dataset.copyInfo  += str(dest)
                print 'executing :'+cmd
                print 'please wait...'
                runCmd = Popen4(cmd)
                answer = str(runCmd.fromchild.read())
                answer = string.strip(answer)
                print answer
                if answer != filename :
                    self.dataset.copyStatus = 'error'
                    self.dataset.copyInfo   = answer                
            else:
                self.dataset.copyStatus = 'success'
                self.dataset.copyInfo   = 'File already exists '


"""
DQ2 Classes
-----------
dsObject = dataset object created by dataset
           maker
name     = dataset name
destDir  = dir where files are copied to.
          The base destDir is set in a config file.
          This is then 
"""


class CopyDQ2Files:
    def __init__(self , dsObject):

        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir

    def filesString(self):
        s = ''
        for file in self.dataset.filesToCopy:
            s += ' '+os.path.basename(file)

        return s

    def doCopy(self):
        self.mkDestDir()
        cmd  = 'dq2_get -s '+self.dataset.site+' -r -d '+self.destDir
        cmd += ' '+self.name
        cmd += self.filesString()
        print 'executing :'+cmd
        print 'please wait...'
        runCmd = Popen4(cmd)
        answer = runCmd.fromchild.readlines()
        self.handleCmdOutput(answer)
        #print str(self.dataset.__dict__)

    def handleCmdOutput(self, output):
         pattern ='ERROR|not found'
         for line in output:
             print line
             if re.search(pattern, line):
                 self.dataset.copyStatus = 'error'
                 self.dataset.copyInfo   = line
             elif re.search('Total:', line):
                 total,failed = line.split('-')
                 if string.strip(failed)[-1:]     == '0':
                     if string.strip(total)[-1:]  != '0':
                         self.dataset.copyStatus   = 'success'
                         print 'sucess success,success, success,success,success,success'
                 self.dataset.copyInf = line
             else:
                 self.dataset.copyInfo    = formatCollection(output)
             

    def mkDestDir(self):
        loc = os.path.join(self.destDir, self.name)
        if not os.path.exists(loc):
            cmd = 'mkdir '+loc
            runCmd = Popen4(cmd)
            runCmd.wait()
        self.destDir = loc
        
        print'Datasets dir: '+ self.destDir
        

            

class ListDQ2Files:
    def __init__(self , dsObject):

        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir

    def filesString(self):
        s = ''
        for file in self.dataset.filesToCopy:
            s += ' '+os.path.basename(file)

        return s

    def doCopy(self):

        cmd  = 'dq2_get -rc -d '+self.destDir
        cmd += ' '+self.name
        cmd += self.filesString()
        print 'executing :'+cmd
        print 'please wait...'
        runCmd = Popen4(cmd)
        answer = str(runCmd.fromchild.read())
        answer = string.strip(answer)
        print answer

  

