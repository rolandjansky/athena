# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TopExamples.logger as logger
import TopExamples.ami as ami

import ROOT

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

        for line in input:
            if line.find('#') > -1:
                line = line.split('#')[0]

            line = line.strip().split()

            if len(line) >= 3:
                id = int(line[0])
                xsec = float(line[1])
                kfac = float(line[2])

                shower = 'not available'
                if len(line) >= 4:
                    shower = line[3]

                self.datasets[id] = DS(id, xsec, kfac, shower)

        input.close()

    def getFilename(self):
        return self.filename

    def hasID(self, id):
        return id in self.datasets

    def getRawXsection(self, id):
        if id in self.datasets:
            ds = self.datasets[id]
            return ds.xsec
        return 0

    def getKfactor(self, id):
        if id in self.datasets:
            ds = self.datasets[id]
            return ds.kfac
        return 0

    def getXsection(self, id):
        if id in self.datasets:
            ds = self.datasets[id]
            return ds.xsec * ds.kfac
        return 0

    def getShower(self, id):
        if id in self.datasets:
            ds = self.datasets[id]
            return ds.shower
        return 0


def queryAmi(amiSamples, tag):
    sys.stdout.write('-> %s: ' % tag)
    sys.stdout.flush()
    print(tag)
    ds = ami.askAmi(tag)
    print(len(ds), 'datasets')
    amiSamples.update(ds)


def cacheAmi(ptaglist):
    try:
        amiSamples = pickle.load(open("amiyields.pickle", "rb"))
        print(logger.OKBLUE + 'Using cached yields - to force me to query AMI delete amiyields.pickle' + logger.ENDC)
    except IOError as ioex:
        if ioex.errno == 2:
            print(logger.OKBLUE + 'Asking AMI' + logger.ENDC)

            amiSamples = {}

            for ptag in ptaglist:
                queryAmi(amiSamples, 'data16_13TeV.%.physics_Main.%.DAOD_TOPQ%' + ptag)
                queryAmi(amiSamples, 'data15_13TeV.%.physics_Main.%.DAOD_TOPQ%' + ptag)
                queryAmi(amiSamples, 'data16_13TeV.period%physics_Main.PhysCont.DAOD_TOPQ%' + ptag)
                queryAmi(amiSamples, 'data15_13TeV.period%physics_Main.PhysCont.DAOD_TOPQ%' + ptag)
                queryAmi(amiSamples, 'mc16_13TeV.%.DAOD_TOPQ%.%' + ptag)

            if len(amiSamples) == 0:
                print(logger.FAIL + 'Something went wrong when trying to get the data from ami' + logger.ENDC)
                sys.exit(1)
            else:
                pickle.dump(amiSamples, open("amiyields.pickle", "wb"))

        return amiSamples


def makeDirectory(outputDirectory):
    '''
    Create a directory if it doesn't and print a message.
    '''
    try:
        os.makedirs(outputDirectory)
        print('Made directory', outputDirectory)
    except FileExistsError:  # directory already exists
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
