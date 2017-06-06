# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logger
import ami

import ROOT

import glob
import os
import sys
import pickle

ROOT.TFile.SetReadStreamerInfo(False)

class DS:
  def __init__(self, id, xsec, kfac, shower):
    self.id = id
    self.xsec = xsec
    self.kfac = kfac
    self.shower = shower

class TopDataPreparation:
  def __init__(self, filename):
    self.datasets = {}
    self.filename = filename

    input = open(filename)

    for l in input:
      #print l
      if l.find('#') > -1:
        l = l.split('#')[0]

      l = l.strip().split()

      #print l
      if len(l) >= 3:
        id = int(l[0])
        xsec = float(l[1])
        kfac = float(l[2])

        shower = 'not available'
        if len(l) >= 4:
          shower = l[3]

        self.datasets[id] = DS(id, xsec, kfac, shower)

      #print l
    input.close()

  def getFilename(self):
      return self.filename

  def hasID(self, id):
    return self.datasets.has_key(id)

  def getRawXsection(self, id):
    if self.datasets.has_key(id):
      ds = self.datasets[id]
      return ds.xsec

    return 0

  def getKfactor(self, id):
      if self.datasets.has_key(id):
        ds = self.datasets[id]
        return ds.kfac

      return 0

  def getXsection(self, id):
    if self.datasets.has_key(id):
      ds = self.datasets[id]
      return ds.xsec * ds.kfac

    return 0

  def getShower(self, id):
    if self.datasets.has_key(id):
      ds = self.datasets[id]
      return ds.shower

    return 0

def queryAmi(amiSamples, tag):
    sys.stdout.write('-> %s: ' % tag)
    sys.stdout.flush()
    print tag
    ds = ami.askAmi(tag)
    print len(ds), 'datasets'
    amiSamples.update(ds)

def cacheAmi(ptaglist):
    try:
        amiSamples = pickle.load(open("amiyields.pickle", "rb" ))
        print logger.OKBLUE + 'Using cached yields - to force me to query AMI delete amiyields.pickle' + logger.ENDC
    except IOError, ioex:
        if ioex.errno == 2:
            print logger.OKBLUE + 'Asking AMI' + logger.ENDC

            amiSamples = {}
            
            for ptag in ptaglist:
              #data15_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667
              queryAmi(amiSamples, 'data16_13TeV.%.physics_Main.%.DAOD_TOPQ%' + ptag)
              queryAmi(amiSamples, 'data15_13TeV.%.physics_Main.%.DAOD_TOPQ%' + ptag)
              queryAmi(amiSamples, 'data16_13TeV.period%physics_Main.PhysCont.DAOD_TOPQ%' + ptag)
              queryAmi(amiSamples, 'data15_13TeV.period%physics_Main.PhysCont.DAOD_TOPQ%' + ptag)
              queryAmi(amiSamples, 'mc15_13TeV.%.DAOD_TOPQ%.%' + ptag)

            if len(amiSamples) == 0:
                print logger.FAIL + 'Something went wrong when trying to get the data from ami' + logger.ENDC
                sys.exit(1)
            else:
                pickle.dump(amiSamples, open("amiyields.pickle", "wb"))

    return amiSamples

def getDatasetYield(directoryName):
    counter = 0

    #this sucks - think of a better way
    histogramsIKnowAbout = ["all/cutflow",
                            "ee/cutflow",
                            "mumu/cutflow",
                            "emu/cutflow",
                            "ejets/cutflow",
                            "mujets/cutflow"]

    #i  = 0
    for fileName in glob.glob(directoryName + '/*root*'):
        #print i, fileName
        #i += 1
        f = ROOT.TFile.Open(fileName)

        if not f:
            print logger.FAIL + 'ERROR: Failed to open %s' % fileName + logger.ENDC
            continue

        #Look for TDirectory file (not TTree) and read the cutflow from
        #the first one that we come to (since we only want the initial events)
        for key in f.GetListOfKeys():
            if key.ReadObj().ClassName() == 'TDirectoryFile':
                h = f.Get(key.GetName() + '/cutflow')
                if h:
                    counter += h.GetBinContent(1)
                    break

        f.Close()

    return counter

#Make a directory if it doesn't exist
def makeDirectory(outputDirectory):
    try:
        os.makedirs(outputDirectory)
        print 'Made directory', outputDirectory
    except:
        #directory already exists
        pass

def makeNameFitOnTerminal(name, maxlength):
    evenshortername = name

    if len(evenshortername) > maxlength:
        ar = evenshortername.split('.')
        maxlen = maxlength - len(ar[0]) - len(ar[1]) - len(ar[2]) - 5
        shortened = ar[1][0:maxlen] + '...'
        evenshortername = ar[0] + '.' + shortened + '.' + ar[2]

    return evenshortername

def makeFileNameFitOnTerminal(name, maxlength):
    evenshortername = name

    if len(evenshortername) > maxlength:
        ar = evenshortername.split('.')
        ar[3] = '.'
        evenshortername = '.'.join(ar)

    return evenshortername

def check(analysis, directory, runDirectory, samples):

    localDatasetFolders = sorted(glob.glob(directory + '/*'))

    #print localDatasetFolders

    ptaglist = []

    localMap = {}
    for l in localDatasetFolders:
        justfile = os.path.basename(l)
        #print 'justfile', justfile
        #print justfile.split('.')
        shortname = justfile.split('.')[2] + '.' + justfile.split('.')[4] + '.' + justfile.split('.')[5]
        localMap[shortname] = l
        #print 'localMap:', localMap.keys()
        taglist = (justfile.split('.')[5]).split('_')

        for tag in taglist:
          if "p" in tag and not "grp" in tag:
            IsInList = False
            for helptag in ptaglist:
              if tag in helptag:
                IsInList = True
            if not IsInList:
              ptaglist.append(tag)

    print ptaglist

    amiSamples = cacheAmi(ptaglist)

    #print amiSamples

    #Organise into dsid.derivation.tags: yield
    amiSamples2 = {}
    for l in amiSamples.keys():
        newname = l.replace('/','').split('.')
        #print newname,
        newname = newname[1] + '.' + newname[-2] + '.' + newname[-1]
        #print newname
        amiSamples2[newname] = amiSamples[l]

    #for k in sorted(amiSamples2.keys()):
    #   print k, amiSamples2[k]

    analyses = []
    bad = []

    print '%s%50s %90s %12s %12s%s' % (logger.OKBLUE, 'dsid.gridtag', 'local location', 'local yield', 'ami yield', logger.ENDC)
    for sample in samples:
        existingSampleFilenames = []

        print logger.OKBLUE + sample.name + logger.ENDC
        for ds in sample.datasets:
            splitName = ds.replace('/','').split('.')
            shortname = splitName[1] + '.' + splitName[-2] + '.' + splitName[-1]
            #print 'shortname', shortname

            #local yield
            directory = ''
            directoryYield = 0
            if localMap.has_key(shortname):
                #print 'key    ', localMap[shortname]
                directory = localMap[shortname]
                directoryYield = getDatasetYield(directory)

            #ami yield
            amiYield = 0
            if amiSamples2.has_key(shortname):
                #print 'here', amiSamples2[shortname]
                amiYield = amiSamples2[shortname]

            colourstart = ''
            colourend = ''

            #no events in local copy
            if directoryYield == 0:
                colourstart = logger.FAIL
                colourend = logger.ENDC

            #local not the same as ami
            if directoryYield != amiYield:
                colourstart = logger.FAIL
                colourend = logger.ENDC

            evenshortername = makeNameFitOnTerminal(shortname, 50)
            eventshorterfilename = makeFileNameFitOnTerminal(directory.split('/')[-1], 60)

            print '%s%50s %90s %12d %12d%s' % (colourstart, evenshortername, eventshorterfilename, directoryYield, amiYield,  colourend)

            if amiYield > 0 and directoryYield != amiYield:
                bad.append(ds)

            if localMap.has_key(shortname):
                out = open('%s/input.txt' % directory, 'w')
                for l in sorted(glob.glob('%s/*root*' % directory)):
                    out.write(l + '\n')
                out.close()

                #only run on files that contain events
                if directoryYield > 0:
                    analyses.append('cd %s;mini-to-plots %s %s/input.txt' % (runDirectory, analysis, directory))

                    #figure out the output filename from mini-to-plots so we can merge them
                    components = directory.split('/')[-1].split('.')
                    outputName = '.'.join(components[2:8])
                    existingSampleFilenames.append(outputName)

        #generate the command to merge them together
        if len(existingSampleFilenames) > 0:
            mergelist = ' '.join(existingSampleFilenames)
            analyses.append('cd %s;hadd -f %s.root %s' % (runDirectory, sample.name, mergelist))

    if '--debug' in sys.argv:
        print ''
        print ''
        print 'These commands are going to be run:'
        for l in analyses:
            print l

    if '--bad' in sys.argv:
        print ''
        print ''
        print 'These are datasets where the ami yield does not match'
        print 'TopExamples.grid.Add("Resubmit").datasets = ['

        for l in bad:
            print '"%s",' % l

        print ']'
        print "resubmit = TopExamples.grid.Samples(['Resubmit'])"
        print 'TopExamples.grid.submit(config, resubmit)'

    if '--run' in sys.argv:
        makeDirectory(runDirectory)

        print ''
        print ''
        print analyses
        for i, l in enumerate(analyses):
            print logger.OKBLUE + l + logger.ENDC
            os.system(l)
