######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

import os.path, anydbm

class RTTRegression:
    """An RTT test to run its own TestSuite under RTT control
    This test checks that classes can be instantitiated. It also
    creates the databass needed to run RTTRegression"""
    

    def __init__(self, argDict):


        self.success =  0
        self.error   = -1

        # self.logger = argDict['logger']
        self.logger  = Logger()
        msg = 'Instantiating RTTRegression, args: %s' %str(argDict)
        self.logger.debug(msg)
        # fixtureDir is set in JobsXMLReader when reading in the config file.
        self.fixtureDir = argDict['fixtureDir']
        # the current directory
        jDescriptor    = argDict['JobDescriptor']
        self.runPath   =  jDescriptor.runPath
        self.logDir    =  jDescriptor.runPath
        # directory of the source code under test
        self.rttSrcDir =  os.path.join(self.runPath, 'Tools/RunTimeTester/src')
        self.runPath   =  jDescriptor.runPath
        fixture        =  os.path.basename(self.fixtureDir)
        self.dbName    =  os.path.join(self.runPath, fixture+'.db')
        self.refdbName =  os.path.join(self.runPath, 'refFile_'+fixture+'.db')
        # do not open the dir does not exist yet
        self.ofName    =  os.path.join(self.runPath, fixture+'_regression.log')

        
    def run(self):

        outFile = open(self.ofName, 'w')

        if not os.path.exists(self.dbName):
            msg = 'None existant path: %s' %  self.dbName
            self.logger.error(msg)
            outFile.write(msg+'\n')
            outFile.close()
            
            return self.error
        if not os.path.exists(self.refdbName):
            msg = 'None existant path: %s' %  self.refdbName
            self.logger.error(msg)
            outFile.write(msg+'\n')
            outFile.close()
            
            return self.error

        newDB = anydbm.open(self.dbName,    'r')
        oldDB = anydbm.open(self.refdbName, 'r')
        result = self.success
            
    
        onlyInNew = [k for k in newDB.keys() if k not in oldDB.keys()]

        text = 'Number of keys in reference db %d\n' % len(oldDB.keys())
        text = 'Number of keys in new       db %d\n' % len(newDB.keys())
        
        
        if onlyInNew:

            result = self.error
    
            text += '\n'
            text +='Reference - %s: date: %s\n' % (oldDB['fixtureDir'],
                                                   oldDB['date'])
            
            text +=  'New       - %s: date: %s\n' % (newDB['fixtureDir'],
                                                     newDB['date'])
            
            text += '\n'
            text +=  ' keys in new database, but not in old\n'
            text +=  str(onlyInNew)+'\n'
            text += '\n'
    
        onlyInOld = [k for k in oldDB.keys() if k not in newDB.keys()]
        if onlyInOld:

            result = self.error
        
            text +=  '\n'
            text +=  ' keys in old database, but not in new\n'
            text +=  str(onlyInOld)+'\n'
            text += '\n'
    
        keys = [k for k in oldDB.keys() if k in newDB.keys()]
        toRemove = ['fixtureDir', 'date']
        [keys.remove(k) for k in toRemove if k in keys]
        if keys:
            text +=  'differences:\n'
            text += '\n'

            for k in keys:
                
        
                if oldDB[k] != newDB[k]:

                    result = self.error
                    
                    text +=  'Key: %s\n' % k
                    text += '\n'
                    text +=  '   old:\n'
                    text +=  '   ' +str(oldDB[k])+'\n'
                    text += '\n'
                    text +=  '   new:\n'
                    text +=  '   '+str(newDB[k])+'\n'


        text += '\n'

        totTests = 0
        text +=  'Number of points examined:\n'
        for k in keys:
            line = ''
            line += k.ljust(30)
            ntestOld = len(oldDB[k].split(','))
            ntestNew = len(newDB[k].split(','))
            # assert(ntestOld == ntestNew)
            num = '%d' % ntestOld
            line += num.ljust(5)
            # print line
            totTests += ntestOld

        text +=  'No of test classes which pass: %d\n' % len(keys)
        text +=  'Total number of tests passed:  %d\n ' %totTests

        outFile.write(text)
        outFile.close()
        return result
        
        

