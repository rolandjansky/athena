######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

import sys, os, logging

from Logger            import Logger

class FileComparator:

    def __init__(self, paramDict):
        self.logger          = Logger()
        self.logger.debug('instantiating file comparator')
        jDescriptor          = paramDict.get('JobDescriptor')
        #location             = paramDict.get('refFileLocation','rttDB')
        location             = 'install'
        #self.diffFile        = "differ.out"
        self.testFile        = paramDict['testFile']
        self.refFile         = paramDict['refFile']
        self.diffFile        = paramDict['diffFile']
        self.shareArea       = jDescriptor.paths.shareArea
        
        #self.success  =  0
        #self.error    = -1
        self.error     = 'error'
        self.success   = 'success'
        # ................
        
        srcDir = jDescriptor.paths.RTTSrcDir
        if srcDir not in sys.path: sys.path.append(srcDir)

        #location=self.getLocation(jDescriptor)
        if location == 'install':
            self.refFile = os.path.join(self.shareArea, self.refFile)
            self.diffFile = os.path.join(jDescriptor.runPath, self.diffFile)
            self.testFile  = os.path.join(jDescriptor.runPath, self.testFile)
        elif location == 'rttDB':
            pass
            #self.refFile = dbFileRetriever.writeRefFile(self.refFile)
            #dbFileRetriever.storeFileInDB(self.testFile)            
        else:
            self.logger.error("Ref File location unknown "+str(location))
            
            
    def getLocation(self, descriptor, whereFrom ='refFileSystem'):
        from formatCollection import formatCollection
        path, file = os.path.split(descriptor.runPath)
        from FileLocationKeyGenerator import KeyGeneratorFactory
        
        lo = KeyGeneratorFactory().create(whereFrom, path)
        return lo

    def run(self):
        self.logger.debug('Running simple file comparator')
        if not os.path.exists(self.refFile):
            self.logger.error('reference file %s does not exist or not found '%str(self.refFile))
            return 'error'

        cwd   =  os.getcwd()
        testF = ""
        refF  = ""
        testFile = self.testFile
        #reference files will be installed or will be in RTT DB
        if not os.path.isabs(self.testFile):
            testFile = os.path.join(cwd,self.testFile)

        try:
            testF = open(testFile, "rb").read()
            refF  = open(self.refFile, "rb").read()            
        except Exception, e:
            self.logger.error("Could not read files to compare ")
            self.logger.error(str(e))
            msg = 'File Comparator failed : ' +str(e)
            self.logger.error(msg)
            return self.error
        
        if testF != refF:
            print 'files are different'
            self.logger.debug("The following two files are different")
            self.logger.debug("File 1 :"+ str(testFile))
            self.logger.debug("File 2 :"+ str(self.refFile))
            diffCommand = ["diff "+testFile +" "+self.refFile+" >"+self.diffFile]
            
            from ShellCommand      import ShellCommand
            runDiff = ShellCommand(diffCommand, self.logger)
            #runDiff.wait()
            
            msg =  'File Comparator failed. \n'
            msg += 'The following files are different : \n%s\n%s' % (
                str(testFile), str(self.refFile))

            self.logger.error(msg)
            return self.error   
        else:
            self.logger.info("The following two files are the same ")
            self.logger.info("File 1: "+str(testFile))
            self.logger.info("File 2: "+str(self.refFile))
            #logger.info("File 2: "+str(self.refFile))
            return self.success
            
        

    def usage(self):
        self.logger.info("Usage : FileComparator(<dict>)")
        self.logger.info("success ==> Files same; error ==> Files differ or exception")


if __name__ == '__main__':

    print 'running file comparator'
    dict = {'testFile':'/home/en/RTT/test.txt',
            #'refFile':'HelloWorldOptions.txt',
            'refFile':'/home/en/RTT/test.txt',
            'diffFile': 'differ.out'}

    fc = FileComparator(dict)
    print str(fc.__dict__)
    fc.run()
