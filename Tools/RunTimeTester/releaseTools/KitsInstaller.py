# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#This scripts installs pacman releases
#Please verify that you are using the recommended
#pacman version as this may affect the installation
#process.

# Configuration
# =============
# The Script takes a configuration file
# as command line argument. Here are the
# contents of a sample config
#*******************************************************
# pacmanArea = /unix/atlas1/software/pacman/pacman-3.19
# release    = rel_3
# dest       = /unix/atlasrtt/eric
# branch     = bugfix
# cmtconfig  = i686-slc4-gcc34-opt
# todayNightly = True
#*******************************************************
#

from popen2 import Popen4
import os, os.path, time, shutil,sys                

def writeToDir(script, name):
    #name = os.path.join(os.getcwd(), "dev_kitInstaller.sh")
    print 'Writing script to run dir'
    open(name ,"w").write(script)
    os.chmod(name, 0777)
    

class ReleaseInstaller:
    def __init__(self, dest, pacmanArea, branch='dev', cmtconfig='i686-slc3-gcc323-opt', release=''):
        self.release        = release
        self.relInstallDest = dest
        self.distBaseDir    = dest
        self.pacmanArea     = pacmanArea
        self.branch         = branch
        self.cmtconfig      = cmtconfig
        self.name           = os.path.join(os.getcwd(), "temp_kitInstaller.sh")
        self.nightlies      = ['rel_0','rel_1','rel_2', 'rel_3',
                               'rel_4', 'rel_5', 'rel_6']
        
    def install(self):
        print "making run script..."
        self.makeInstallScript()
        #print "Setting up pacman..."
        #setupPacman = Popen4(self.makePacmanCommand())
        #print str(setupPacman.fromchild.readline())
        #setupPacman.wait()
        print "installing kit :"+str(self.release)
        #install = Popen4(self.makeReleaseInstallCommand())
        #print str(install.fromchild.readline())
        #install.wait()
        #os.system('source kitInstaller.sh')
        #install = Popen4('source relInstaller.sh')
        #print str(install.fromchild.readline())

    def installNightlyKit(self):
        print 'Beginning to install nightly kit'
        cwd = os.getcwd()
        print "making run script"
        self.makeNightlyKitInstallScript()
        print " nightly kit to install "+self.release
        time.sleep(30)
        if not os.path.exists(self.name):
            print "install shell script not found, exiting ..."
            sys.exit(0)
        else:
            print 'Found install script in %s'%str(cwd)
            os.system(self.name)
        
        
    def makePacmanCommand(self):
        print 'making pacman set up command'
        cwd = os.getcwd()
        pmc ='cd '+self.pacmanArea+';'
        pmc +='source setup.sh;'
        pmc +='cd '+cwd
        #pmc +='pacman -allow trust-all-caches tar-overwrite'
        print 'Pacman setup command...'
        print pmc
        return pmc

    def makeNightlyKitInstallCommand(self):
        cwd = os.getcwd()
        ric = 'cd '+self.relInstallDest+';'
        ric += 'pacman -allow trust-all-caches tar-overwrite;'
        ric += 'pacman -get http://cern.ch/atlas-computing'
        ric += '/links/buildDirectory/kitrel/nightlies/'+self.branch+'/cache:'
        ric += 'AtlasProduction_'
        ric += self.makeRelease(self.release)
        ric += '_'+self.cmtconfig.replace('-','_')+';'
        ric += 'cd '+cwd
        print 'Pacman kit install command...'
        print ric
        return ric
        
    def makeRelease(self, release):
        if  release in self.nightlies:
            return release
        print release
        parts = release.split('.')
        rel =  parts[0]
        rel += '_'
        rel += parts[1]
        rel += '_'
        rel += parts[2]

        return rel

    def makeInstallScript(self):
        #fh = open('runner.sh', 'w')
        
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'source $HOME/.bash_profile\n'
        pmc = self.makePacmanCommand()
        parts = pmc.split(';')
        for part in parts:script += part+'\n'

        ric = self.makeReleaseInstallCommand()
        parts = ric.split(';')
        for part in parts:script += part+'\n'
        
        writeToDir(script, self.name)

    def makeNightlyKitInstallScript(self):
        script = '#! /bin/bash\n'
        script += 'set +e\n'
        script += 'source $HOME/.bash_profile\n'
        script +='cd '+self.distBaseDir+'\n'
        #script += 'rm -rf cmtsite\n'
        pmc = self.makePacmanCommand()
        parts = pmc.split(';')
        for part in parts:script += part+'\n'

        ric = self.makeNightlyKitInstallCommand()
        parts = ric.split(';')
        for part in parts:script += part+'\n'
        
        script +='cd '+self.distBaseDir+'\n'
        script += 'rm -f kitInstallComplete\n'
        script += 'echo " ">kitInstallComplete\n'
        script += 'rm -f latest\n'
        script += 'ln -s '+self.release +' latest\n'
        #script += '/usr/local/bin/python2.3 /unix/atlasrtt/RTT/sendBrinickMail.py'
        writeToDir(script, self.name)

    def makeReleaseInstallCommand(self):
        ric = 'cd '+self.relInstallDest+';'
        ric += 'pacman -allow trust-all-caches tar-overwrite;'
        ric += 'pacman -get http://cern.ch/atlas-computing'
        ric += '/links/kitsDirectory/projects/cache:'
        ric += 'AtlasProduction_'
        ric += self.makeRelease(self.release)
        #ric += '_i686_slc3_gcc323_opt'
        ric +='_'+self.cmtconfig.replace('-', '_')
        print 'Pacman kit install command...'
        print ric
        return ric
     
    def findKitToInstall(self):
        print 'looking for release to install'
        lcr  ='/afs/cern.ch/atlas/software/builds/kitrel/nightlies/'+self.branch+'/latest'
        self.release= os.readlink(lcr)
        print 'nightly Tag is '+str(os.readlink(lcr))
        return self.release
    
    def makeInstallDir(self):
        cwd = os.getcwd()
        #rel = self.findKitToInstall()
        rel = self.release
        os.chdir(self.relInstallDest)
        self.dest = os.path.join(self.relInstallDest, rel)
        self.relInstallDest= self.dest
        if os.path.exists(self.dest):
            print 'removing '+rel+' from '+self.relInstallDest
            shutil.rmtree(rel)
        os.mkdir(rel)
        if os.path.exists(self.dest):
            print'install directory made: ' +self.dest

        os.chdir(cwd)
        
    def stampsForTest(self):
        print 'looking for nightly stamp'
        stampFile  = '/afs/cern.ch/atlas/software'
        stampFile += '/builds/kitrel/nightlies/dev/stamp_for_tests_'+self.cmtconfig
        self.stampDate  = time.gmtime(os.stat(stampFile).st_atime)[:3]
        self.today = time.gmtime()[:3]

if __name__ =='__main__':
    dict = {}
    if len(sys.argv) < 2:
        print 'Usage: KitsInstaller.py <config>'
        sys.exit(1)
    lines = open(sys.argv[1], 'r').readlines()
    lines = [line.strip() for line in lines]
    lines = [line for line in lines if len(line)>0]
    lines = [line for line in lines if not line.startswith('#')]
    for line in lines:
        parts = line.split('=')
        dict[parts[0].strip()] = parts[1].strip()

    print 'Configuration parameters'
    for key in dict.keys():
        print key +' = '+dict[key]

    pacmanArea   = dict['pacmanArea']
    release      = dict['release']
    dest         = dict['dest']
    cmtconfig    = dict['cmtconfig']
    branch       = dict['branch']
    todayNightly = dict['todayNightly']
    
    RI = ReleaseInstaller(dest, pacmanArea, branch, cmtconfig)
    RI.stampsForTest()
    if todayNightly == 'True':
        while RI.stampDate != RI.today:
            print "Today\'s stamp not available, will check again in 5 minutes"
            time.sleep(300)
            RI.stampsForTest()
        
        rel = RI.findKitToInstall()
        print 'Will now install latest nightly :%s' %rel
    else:
        RI.release = release
        print 'Installing the nightly from config'
    #print "Found nightly stamp, kit to install is: " +rel
    #print 'Using hard coded release %s'%release
    #RI.release = release
    RI.makeInstallDir()
    if RI.release in RI.nightlies:
        print 'now installing...'
        RI.installNightlyKit()
        time.sleep(30)
        #os.system('/home/rtt/kitInstaller.sh')
        #run=Popen4('source /home/rtt/kitInstaller.sh')
        #run.wait()
        #print 'installation done...'
    else:
        print 'found '+RI.release
        RI.install()
        
