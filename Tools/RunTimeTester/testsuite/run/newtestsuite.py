# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os, os.path

# Helper function to grossly determine if testsuitepath ressembles a testsuite
def isTestSuite(testsuitepath, templSuite=False):
    # List of what we expect in the testsuitepath
    if not os.path.isdir(testsuitepath): 
        print '%s: not a directory' % testsuitepath
        return False

    things = ['tests.py', '__init__.py']
    if templSuite:
        things.append('cfg/cfg.xml.TPL')
    else:
        things.append('cfg/cfg.xml')

    missing = [os.path.join(testsuitepath, t) for t in things if not os.path.exists(os.path.join(testsuitepath, t))]
    for m in missing:
        print '%s: missing from testsuite.' % m
    return len(missing)==0

# Write file helper
def write(what, where):
    h = open(where, 'w')
    h.write(what)
    h.close()

# Turn a testsuite .TPL file into a normal file
def tplToFile(tplpath, srcToDstDict):
    f = open(tplpath)
    conts = f.read()
    f.close()

    for src,dst in srcToDstDict.items():
        conts = conts.replace(src,dst)

    f = open(tplpath.split('.TPL')[0], 'w')
    f.write(conts)
    f.close()

    os.remove(tplpath)

# ====================================================================================
# ====================================================================================
# ====================================================================================

def usage():
    print 'python %s </abs/or/rel/path/to/testsuite/dir_1>, [</abs/or/rel/path/to/testsuite/dir_2>, ...]' % sys.argv[0]
    print
    print 'At least one testsuite is required. Multiple suites should be space separated.'
    sys.exit()

def get():
    if len(sys.argv)<2: usage()

    suites = []
    for ts in sys.argv[1:]:
        ts.strip()
        help = '-h' in ts or '--h' in ts or '-help' in ts or '--help' in ts
        if help: usage()
        suites.append(ts)
    return [os.path.abspath(s) for s in suites]

# ====================================================================================
# ====================================================================================
# ====================================================================================

def create(testsuitepath):
    if os.path.exists(testsuitepath):
        print '%s: already exists' % testsuitepath
        sys.exit(1)
        
    testsuitedir  = os.path.dirname(testsuitepath)
    testsuitename = os.path.basename(testsuitepath)

    # create it
    cmd  = 'mkdir -p %s;' % testsuitepath
    cmd += 'cd %s;' % testsuitepath
    cmd += 'mkdir Results Work cfg Package logs;'
    os.system(cmd)

    # now dump out various template files
    write(cfg(), os.path.join(testsuitepath, 'cfg/cfg.xml.TPL'))
    write(package(), os.path.join(testsuitepath, 'Package/package.xml.TPL'))
    write(init(), os.path.join(testsuitepath, '__init__.py'))
    write(tests(), os.path.join(testsuitepath, 'tests.py'))

    print 'New %s testsuite skeleton created OK' % testsuitename
    print 'You should now complete the relevant bits of:'
    print os.path.join(testsuitepath, '__init__.py')
    print os.path.join(testsuitepath, 'cfg/cfg.xml.TPL')
    print os.path.join(testsuitepath, 'Package/package.xml.TPL')
    print 
    print

# ====================================================================================
# ====================================================================================
# ====================================================================================

def install(testsuitepath):
    def checkEnvVar():
        try:
            os.environ['RTTShareDir']
        except KeyError:
            print 'Please set up the RTTShareDir env var, then re-run to install.'
            sys.exit(1)

    checkEnvVar()

    if not os.path.exists(testsuitepath):
        print 'Cannot install inexistant testsuite: %s' % testsuitepath
        sys.exit(1)

    if not isTestSuite(testsuitepath, templSuite=True): 
        print 'Cannot install, there were errors.'
        sys.exit(1)

    path = os.path.dirname(testsuitepath)
    name = os.path.basename(testsuitepath)

    m = 'Please give path to the _existing_ "testsuites" dir in which this should be installed:\n> '
    testsuitesdir = os.path.abspath(raw_input(m).strip())
    while not os.path.exists(testsuitesdir):
        testsuitesdir = os.path.abspath(raw_input(m).strip())

    installpath = os.path.join(testsuitesdir, name)
    if os.path.exists(installpath):
        print '%s: exists already, doing nothing.' % installpath
        sys.exit(1)
    
    # now install in the testsuites dir given
    cmd  = 'cd %s;' % testsuitesdir
    cmd += 'cp -r %s .;' % testsuitepath
    cmd += 'cd %s;' % name    
    os.system(cmd)

    dict = {'$_BASE_$' : os.path.dirname(testsuitesdir) ,
            '$_LIB_TOOLS_FROM_ENV_$' : os.environ['RTTShareDir'],
            '$_TESTNAME_$' : name}

    for root, dirs, files in os.walk(installpath):
        tplFiles = [os.path.join(root, f) for f in files if f.endswith('.TPL')]
        [tplToFile(t, dict) for t in tplFiles]

    print 'Installed: OK (Original %s untouched)' % testsuitepath

# ====================================================================================
# ====================================================================================
# ====================================================================================

def upload(testsuitepath):
    import tarfile

    def cleanTestSuite():
        for root, dirs, files in os.walk(testsuitepath):
            for thing in [os.path.join(root, f) for f in files if f.endswith('~')]:
                print 'Cleaning testsuite: %s: removing...' % thing
                os.remove(thing)

    # Method proper starts here
    if not os.path.exists(testsuitepath):
        print '%s: inexistant testsuite' % testsuitepath
        sys.exit(1)
    
    if not isTestSuite(testsuitepath, templSuite=True): 
        print 'Not uploading to tarball.'
        sys.exit(1)

    cleanTestSuite()
    
    # Finally ready to upload
    tarballName = 'rttTesting.tgz'
    tarballpath = '/afs/cern.ch/atlas/project/RTT/data/brinick/%s' % tarballName

    tmpArea = '/tmp/rtt/tarball'    
    os.system('mkdir -p %s;cp %s %s' % (tmpArea, tarballpath, tmpArea))
    if not os.path.exists(os.path.join(tmpArea, tarballName)):
        print 'Cannot grab tarball, exiting...'
        sys.exit(1)

    tgzfile = tarfile.open(tarballpath, 'r:gz')
    for thing in tgzfile:
        testsuitename = os.path.basename(testsuitepath)
        if 'testsuites/%s' % testsuitename in thing.name:
            print 'Testsuite %s already exists in official tarball. Not adding yours, sorry...' % testsuitename
            tgzfile.close()
            sys.exit(1)

    cmd  = 'cd %s;' % tmpArea
    cmd += 'tar xfz %s;' % tarballName
    cmd += 'cd testsuites;'
    cmd += 'cp -r %s .;' % testsuitepath
    cmd += 'echo "Adding testsuite %s into tarball...";' % os.path.basename(testsuitepath)
    cmd += 'cd ..;'
    cmd += 'rm -rf %s;' % tarballName
    cmd += 'echo "Building new tarball...";'
    cmd += 'tar cfz %s *;' % tarballName
    cmd += 'echo "Backing up previous official tarball...";'
    cmd += 'cp %s %s.BACKUP;' % (tarballpath, tarballpath)
    cmd += 'echo "Adding new tarball into official location...";'
    cmd += 'cp %s %s;' % (tarballName, os.path.dirname(tarballpath))
    cmd += 'cd %s;' % tmpArea
    cmd += 'rm -rf *;'
    cmd += 'echo "Done: OK."'
    os.system(cmd)
    
# ====================================================================================
# ====================================================================================
# ====================================================================================

def ask(ts):
    m  = 'What would you like to do with %s:\n' % ts
    m += '\t[1] Create it ?\n'
    m += '\t[2] Install it ? (so you can test it works)\n'
    m += '\t[3] Add it? (to the official tarball repository)\n'
    m += 'Choices: 1,2,3? '
    ans = raw_input(m).strip()
    while ans not in ['1','2','3']:
        ans = raw_input(m).strip()
    return ans

def main():
    for testsuitepath in get():
        {'1':create,'2':install,'3':upload}.get(ask(testsuitepath))(testsuitepath)    

# ====================================================================================
# ====================================================================================
# ====================================================================================

#
# Templates for creating new testsuites...
#

def cfg():
    return """<?xml version="1.0"?>

<!DOCTYPE rttconfig SYSTEM "file:$_BASE_$/DTD/RTT.dtd">

<rttconfig>
   <mode>LSFBatch</mode>
   <release>14.5.0</release>
   <refRelease>14.0.10</refRelease>
   <cmtConfig>i686-slc4-gcc34-opt</cmtConfig>
   <branch>dev</branch>
   <runType><releaseRun/></runType>
   <topProject>AtlasProduction</topProject>
   <distArea>/afs/cern.ch/atlas/software/builds</distArea>
   <workBasePath>$_BASE_$/testsuites/$_TESTNAME_$/Work</workBasePath>
   <resultsBasePath>$_BASE_$/testsuites/$_TESTNAME_$/Results</resultsBasePath>
   <resultsBaseURL>$_BASE_$/testsuites/$_TESTNAME_$/Results</resultsBaseURL>
   <site>CERNslc4</site>
   <dataSetCatalog>$_BASE_$/xml/rttDataSetCatalog.xml</dataSetCatalog>
   <jobGroupConfig>$_BASE_$/xml/TopLevelJobGroup.xml</jobGroupConfig>
   <dCubeCfgFile>$_BASE_$/DCubeClient/python/DCubeForRTTCfg.xml</dCubeCfgFile>

   <refDB>
      <file>$_BASE_$/testsuites/$_TESTNAME_$/refFile.db</file>
      <enable>1</enable>
   </refDB>
   <moniDB>
      <file>$_BASE_$/testsuites/$_TESTNAME_$/moniFile.db</file>
      <enable>1</enable>
   </moniDB>

   <confFileSrc>

   <!--
      <rttProvides>
         
         <packageConfFile>
            <package></package>
            <pathToFile>$_BASE_$/testsuites/$_TESTNAME_$/Package/package.xml</pathToFile>
            <cmtpath></cmtpath>
            <version></version>
            <installArea></installArea>
            <shareArea></shareArea>
         </packageConfFile>

      </rttProvides>
         
   -->

   </confFileSrc>
      
   <packageAccept></packageAccept>
      
   <libToolsLoc>$_LIB_TOOLS_FROM_ENV_$</libToolsLoc>
      
   <releaseType>project</releaseType>
   
   <disableMailTo />
   <devRun />
      
   <chainStoreBasePath>$_BASE_$/testsuites/$_TESTNAME_$/ChainStore</chainStoreBasePath>
      
</rttconfig>
"""

def package():
    return """<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>

      <!--
          <athena>
             <doc>blah</doc>
             <doclink>http://cern.ch</doclink>
             <options>AthExHelloWorld/HelloWorldOptions.py</options>
             <package>Control/AthenaExamples/AthExHelloWorld</package>
             <group>HelloWorld</group>
             <queue>short</queue>
          </athena>

       -->

       </jobList>
          
       <jobGroups>
          
       <!-- <jobGroup name="AthenaHelloWorld" parent="Athena"></jobGroup> -->
          
       </jobGroups>
          
    </rtt>
</unifiedTestConfiguration>
"""

def tests():
    return """from utility import log
from checker import Checker

def run():
    for klass in [ExampleChecker]:
        yield klass

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

'''Each new Checker child class that is created
should be (if you want it to be run) added to the list
in the run() method above.
Checker child class constraints:
1) Must inherit from Checker as shown in MyChecker example below
2) Must take a tester instance in constructor 
   (that Checker binds to self.tester. You also get self.summaryFile which contains an instance
   of RTTSummaryFile (imported at top of this file. The source can be found in run/ dir.))
3) Each test function called by the run() method in your Checker class should be decorated with the log function
4) Each test function called by the run() method in your Checker class should return a boolean indicating outcome
'''

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NOTE: This class (because of its name ExampleChecker) will not be run by the framework.
# It is ignored, and is only here so as to provide, erm, an example...
# Either write a new checker class, or modify this one including changing its name.
# In either case, you should then add the checker class name to the list in run() above.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class ExampleChecker(Checker):
    def __init__(self, tester):
        Checker.__init__(tester)

    def run(self):
        self.myTestOne()
        self.myTestTwo()

    @log
    def myTestOne(self):
        "Each test function should be decorated by the log function."
        return True

    @log
    def myTestTwo(self):
        return False

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================
"""

def init():
    return """'''Short testsuite description goes here'''

def run():
   "Any initialisation code goes in this method. Is run before RTT is launched."
   pass
"""

if __name__ == '__main__':
    main()
