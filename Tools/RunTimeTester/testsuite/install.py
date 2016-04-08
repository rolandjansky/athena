# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import os.path
import getopt

def usage():
    print "Please supply path to directory in which to install."
    sys.exit(1)
    
def getargs():
    try:
        return sys.argv[1].strip()
    except:
        usage()
        
    return basepath

def yesnoQuestion(question):
    ans = raw_input(question)
    while ans.lower() not in ['y','n']:
        ans = raw_input(question)
    return ans.lower()    
        
def checkExists(basepath):
    if not os.path.exists(basepath):
        if yesnoQuestion('%s: inexistant dir. Create it? [y|n]: ' % basepath)=='n': sys.exit(1)
        os.system('mkdir -p %s' % basepath)
    elif os.listdir(basepath):
        if yesnoQuestion('%s: exists, but dir not empty, really install there? [y|n]: ' % basepath) == 'n': sys.exit(1)

def checkEnvVars():
    try:
        os.environ['RTTShareDir']
        os.environ['RTTSrcDir']
    except KeyError:
        print 'Please first set up the RTTShareDir, RTTSrcDir and RTTLogDir env vars, if not done.'
        sys.exit(1)

def setupInstall(basepath):
    print 'Copying stuff to %s' % basepath
    pathToThisModule = os.path.dirname(os.path.abspath(__file__))
    os.system('cd %s;cp -r %s/* .' % (basepath, pathToThisModule))

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

def installFramework(basepath):
    print 'Creating files from templates...'

    dict = {'$_BASE_$':basepath,
            '$_LIB_TOOLS_FROM_ENV_$':os.environ['RTTShareDir']}

    tplToFile(os.path.join(basepath, 'xml/TopLevelJobGroup.xml.TPL'), dict)
    tplToFile(os.path.join(basepath, 'xml/rttDataSetCatalog.xml.TPL'), dict)

    for testname in os.listdir(os.path.join(basepath, 'testsuites')):
        dict['$_TESTNAME_$'] = testname
        start = os.path.join(basepath, 'testsuites', testname)
        for root, dirs, files in os.walk(start):
            tplFiles = [os.path.join(root, f) for f in files if f.endswith('.TPL')]
            [tplToFile(t, dict) for t in tplFiles]

def cleanup(basepath):
    print 'Cleaning up...'
    # Now remove the install.py file, no longer relevant
    os.remove(os.path.join(basepath, 'install.py'))
    print 'Done.'

def howtorun(basepath):
    print '************************************************************************'
    print 'To run the framework:'
    print '\t1) cd %s' % basepath
    print '\t2) ./run.sh --jobs'
    print '\t3) ./run.sh --tests -d <path_to_testsuites_dir>'
    print '************************************************************************'

if __name__ == '__main__':
    checkEnvVars()
    basepath = getargs()
    checkExists(basepath)
    setupInstall(basepath)
    installFramework(basepath)
    cleanup(basepath)
    howtorun(basepath)


