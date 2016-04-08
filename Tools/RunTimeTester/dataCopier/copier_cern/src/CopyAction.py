# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string,os,re, sys,time
from popen2           import Popen4
from formatCollection import formatCollection

class BaseAction:
    def __init__(self, filename):
        self.name = filename


#----------------------------------------------------------------------

class CopyNonDQ2File(BaseAction):
    def __init__(self , dsObject):
        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir
        BaseAction.__init__(self, self.name)


    def copyFromCastor(self):
        print 'castor file...'
        dest = os.path.join(self.destDir,
                                os.path.basename(self.name))
        cmd = 'globus-url-copy gsiftp://castorgrid.cern.ch'
        cmd +=  self.name +' '+'file://'+dest
    
        if not os.path.exists(dest):
            self.dataset.copyStatus = 'success'
            self.dataset.copyInfo   = 'File copied to \n'
            self.dataset.copyInfo  += str(dest)
            
            print 'executing :'+cmd
            print 'please wait...'
            runCmd =Popen4(cmd)
            answer = str(runCmd.fromchild.read())
            print answer
            pattern ='(E|e)+rror|not found'
            if re.search(pattern, answer):
                Popen4('rm -f '+dest)
                self.dataset.copyStatus ='error'
                self.dataset.copyInfo   = answer
        else:
            self.dataset.copyStatus = 'success'
            self.dataset.copyInfo   = 'File already exists '


    def copyFromAFS(self):
         print 'afs file ...'
         dest = os.path.join(self.destDir,
                                os.path.basename(self.name))

         cmd = 'cp '+filename +' '+dest
     
         if not os.path.exists(self.name):
             self.dataset.copyStatus = 'error'
             self.dataset.copyInfo   = 'file does not exist :'+self.ename
         elif not os.path.exists(dest):
             self.dataset.copyStatus = 'success'
             self.dataset.copyInfo   = 'File copied to \n'
             self.dataset.copyInfo  += str(dest)
                
             print 'executing :'+cmd
             print 'please wait...'
             runCmd = Popen4(cmd)
             answer = str(runCmd.fromchild.read())
             print answer
             pattern ='(E|e)+rror|not found'
             if re.search(pattern, answer):
                 Popen4('rm -f '+dest)
                 self.dataset.copyStatus ='error'
                 self.dataset.copyInfo   = answer
         else:
             self.dataset.copyStatus = 'success'
             self.dataset.copyInfo   = 'File already exists '


    def doIt(self):
        dict = {'afs'   : self.copyFromAFS,
                'castor': self.copyFromCastor}

        if self.dataset.dsSource in dict.keys():
            print str(self.dataset.dsSource)
            print 'Action to execute!!'
            dict[self.dataset.dsSource]()
        else:
            print "No action taken because source of file is unknown"
            print "File "+self.name
        
        
#------------------------------------------------------------------------

class CopyNonDQ2FileCERN(CopyNonDQ2File):
    def __init__(self, dsObject):
        print 'initialise CopyNonDQ2FileCERN'
        CopyNonDQ2File.__init__(self, dsObject)

    def copyFromCastor(self):
        'copy file from castor'
        self.dataset.physicalName = self.dataset.logicalName    
        self.dataset.copyStatus   = 'success'
        self.dataset.copyInfo     = 'File already at cern, no copy needed '

    def copyFromAFS(self):
        print 'copy file from afs'
        self.dataset.physicalName = self.dataset.logicalName
        self.dataset.copyStatus   = 'success'
        self.dataset.copyInfo     = 'File already at cern, no copy needed '


#------------------------------------------------------------------------
        
    

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
            pass
                                                       
        elif src == 'castor':
            pass

#-----------------------------------------------------------------------

class ListCastorFile(BaseAction):
    
    def __init__(self , dsObject):
        self.dataset = dsObject
        self.name    = self.dataset.logicalName
        self.destDir = self.dataset.destDir
        BaseAction.__init__(self, self.name)

    def doIt(self):
        filename = string.strip(self.name)
        destDir  = self.destDir
        #cmd  = '/usr/local/lcg/edg/bin/'
        cmd += 'edg-gridftp-ls gsiftp://castorgrid.cern.ch'
        cmd +=  filename
        print 'executing :'+cmd
        print 'please wait...'
        runCmd = Popen4(cmd)
        answer = str(runCmd.fromchild.read())
        answer = string.strip(answer)
        print answer
        if answer != filename :
            self.dataset.copyStatus = 'error'
            self.dataset.copyInfo   = answer


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
#---------------------------------------------------------------------

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
        self.cpstatus = {}
        stage,castor  = self.setAreas()
        print 'stage  ' +stage
        print 'castor ' +castor
        files = [os.path.basename(file) for file in self.dataset.filesToCopy]
        for file in files:
            dq2Getcmd  = 'dq2_get -s '+self.dataset.site
            dq2Getcmd += ' -rc -d '+stage
            dq2Getcmd += ' '+self.name
            dq2Getcmd += ' '+file

            temp = os.path.join(stage,  file)
            dest = os.path.join(castor, file)
            
            try:
                cmd    = Popen4('rfdir '+dest)
                result = str(cmd.fromchild.read())
                if re.search('No such file', result):
                    print dq2Getcmd
                    fromGrid   = Popen4(dq2Getcmd)
                    answer = fromGrid.fromchild.readlines()
                    self.handleStatus(self.name, answer, self.cpstatus)
                    #fromGrid.wait()

                    toCastor = Popen4('rfcp '+temp+ ' '+dest)
                    toCastor.wait()
                    
                else:
                    print 'File already available on castor'

                print 'src file  '+temp
                print 'dest file '+dest
                time.sleep(10)
                rmtemp = Popen4('rm -f '+temp)
                rmtemp.wait()
                print 'deleted '+temp
            except Exception, e:
                print 'problem getting data from grid %s'%str(e)

        if 'error' in self.cpstatus.values():
            self.dataset.copyStatus = 'error'
            self.dataset.copyInfo   = str(formatCollection(self.cpstatus))
        else:
             self.dataset.copyStatus = 'success'

    def handleStatus(self, name, result, dict):
        pattern ='ERROR|not found'
        for line in result:
             if re.search(pattern, line):
                 dict[name] = 'error'
                 
             elif re.search('Total:', line):
                 total,failed = line.split('-')
                 if string.strip(failed)[-1:]     == '0':
                     if string.strip(total)[-1:]  != '0':
                         dict[name] = 'success'

    

   
    #------------------------------------------------------------------------
    
    def setAreas(self):
        if self.dataset.destDir.startswith('/castor'):
            self.stageArea = os.path.join(self.dataset.stageArea,
                                          self.name)
            print 'stage Area :'+self.stageArea
            self.castorArea = os.path.join(self.dataset.destDir,
                                           self.name)
            print 'castor area '+self.castorArea
            Popen4('mkdir '+self.stageArea)
            Popen4('rfmkdir '+self.castorArea)
            return (self.stageArea, self.castorArea)
        
#-----------------------------------------------------------------------------            

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

        cmd  = 'dq2_ls '
        cmd += self.name
        cmd += self.filesString()
        print 'executing :'+cmd
        print 'please wait...'
        runCmd = Popen4(cmd)
        answer = str(runCmd.fromchild.read())
        answer = string.strip(answer)
        print answer

  #------------------------------------------------------------------------

