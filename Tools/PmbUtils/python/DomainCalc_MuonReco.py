# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_MuonReco(DomainCalcBase):

    def __init__(self):
        self.domain2col={ 'shared' : (0.8,0.8,0),#dark yellow
                          'moore_ex' : (215/255.,23./255,170./255),#purple
                          'mboy_ex' : (1,98/255.,30/255.),#orange
                          'manyevts' : (0,1,1) }
        self.job2col={'noreco' : (0,1,0),
                      'moore_only' : self.domain2col['moore_ex'],
                      'mboy_only' : self.domain2col['mboy_ex'],
                      'moore_and_mboy' : self.domain2col['shared'],
                      'moore_and_mboy_manyevts'  : self.domain2col['manyevts'],
                      'esdrereco' : (0,0.5,0) }
        return


    def getName(self):
        return "muonreco"

    def getPrettyName(self):
        return "MuonReco"

    def defineJobs(self):
        return ['noreco','moore_only','mboy_only','moore_and_mboy','moore_and_mboy_manyevts','esdrereco']

    def defineDomains(self):
        return ['shared','moore_ex','mboy_ex','manyevts']

    def domainColour(self,domain):
        return self.domain2col[domain]

    def jobColour(self,job):
        return self.job2col[job]

    def calcDomains(self,job2val):
        #job2val: dictionary with string keys and Measurement values.

        #NB: Make calculations direct and only let each job appear
        #once in calculations (to avoid unnecessary missing domain
        #entries and to get correct error estimates).

        #Get out raw job values:

        job_noreco = job2val['noreco']
        job_moore_only = job2val['moore_only']
        job_mboy_only = job2val['mboy_only']
        job_both = job2val['moore_and_mboy']
        job_manyevts = job2val['moore_and_mboy_manyevts']

        #First, the core, pixel and tile domains:
        out_moore = job_both-job_mboy_only
        out_mboy = job_both-job_moore_only
        out_shared = job_mboy_only-job_noreco-out_mboy
        out_manyevts = job_manyevts - job_both

        self.sumCheck(job_noreco + out_moore+out_mboy + out_shared,job_both)

        #Finally put in dictionary and return:
        domains={}
        domains['shared'] = out_shared
        domains['moore_ex'] = out_moore
        domains['mboy_ex'] = out_mboy
        domains['manyevts'] = out_manyevts
        return domains

    def getCDJobDefinitions(self,dbkey):
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        jobdefs = [('Zmumu_Short_NoReco','noreco'),
                   ('Zmumu_Short_MooreOnly','moore_only'),
                   ('Zmumu_Short_MboyOnly','mboy_only'),
                   ('Zmumu_Short_MboyAndMoreOnly','moore_and_mboy'),
                   ('Zmumu_MboyAndMoreOnly','moore_and_mboy_manyevts'),
                   ('standardRecoFromESD','esdrereco')]
        l=[]
        for dbjobname,jobname in jobdefs:
            l += [ PerfMonDozerJobDef(dbkey,'MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT',
                                      dbjobname,jobname) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'edward.moyse@cern.ch,rikard.sandstrom@cern.ch,thomas.kittelmann@cern.ch'

    def getLogFileNamingSchemeDefinitions(self):
        log2job={}
        log2job['muonreco_noreco.log'] = 'noreco'
        log2job['muonreco_mooreonly.log'] = 'moore_only'
        log2job['muonreco_mboyonly.log'] = 'mboy_only'
        log2job['muonreco_both.log'] = 'moore_and_mboy'
        log2job['muonreco_both_manyevts.log'] = 'moore_and_mboy_manyevts'
        log2job['muonreco_esdrereco.log'] = 'esdrereco'
        return log2job
