# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_RecoOnData(DomainCalcBase):

    def __init__(self):
        self.domain2col={ 'core' : (0.8,0.8,0),#dark yellow
                          'idex' : (215/255.,23./255,170./255),#purple
                          'muonex' : (1,98/255.,30/255.),#orange
                          'idmuoncommon' : (0,0,1),
                          'muoncomb' : (1,130/255.,53/255.),#orange
                          'caloex' : (82./255,207./255,199./255),
                          'detcommon' : (106/255.,111/255.,10/255.),
                          'egamma' : (0.2,0.5,0.2),
                          'tau' : (75/255.,231/255.,221/255.),
                          'jet' : (207/255.,75/255.,136/255.),
                          'output' : (0,0,1),#blue
                          'trig' : (0,181./255.,18./255.),
                          'mon' : (0.5,0.5,0.5)#gray (find better?)
                          }
        self.job2col={'core' : self.domain2col['core'],
                      'id' : self.domain2col['idex'],
                      'muon' : self.domain2col['muonex'],
                      'calo' : self.domain2col['caloex'],
                      'idmuon' : self.domain2col['idmuoncommon'],
                      'idmuoncalo' : self.domain2col['detcommon'],
                      'idmuoncalotrigegammamucomb' : self.domain2col['muoncomb'],
                      'idmuoncalotrigegamma' : self.domain2col['egamma'],
                      'idmuoncalotrigegammamucombjet' : self.domain2col['jet'],
                      'idmuoncalotrigegammamucombjettau' : self.domain2col['tau'],
                      'idmuoncalotrig' : self.domain2col['trig'],
                      'idmuoncalotrigegammamucombjettaumon' : self.domain2col['mon'],
                      'all' : self.domain2col['output']
                      }

    def getName(self):
        return "rawtoesd_old"

    def defineJobs(self):
        return ['core','id','muon','calo','idmuon','idmuoncalo',
                'idmuoncalotrig',
                'idmuoncalotrigegamma',
                'idmuoncalotrigegammamucomb',
                'idmuoncalotrigegammamucombjet',
                'idmuoncalotrigegammamucombjettau',
                'idmuoncalotrigegammamucombjettaumon',
                'all']

    def defineDomains(self):
        return ['core','idex','muonex','idmuoncommon','muoncomb','caloex',
                'detcommon','egamma','tau','jet','mon','trig','output']

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
        job_core = job2val['core']
        job_id = job2val['id']
        job_muon = job2val['muon']
        job_calo = job2val['calo']
        job_idmuon = job2val['idmuon']
        job_idmuoncalo = job2val['idmuoncalo']

        job_idmuoncalotrig = job2val['idmuoncalotrig']
        job_idmuoncalotrigegamma = job2val['idmuoncalotrigegamma']
        job_idmuoncalotrigegammamucomb = job2val['idmuoncalotrigegammamucomb']
        job_idmuoncalotrigegammamucombjet = job2val['idmuoncalotrigegammamucombjet']
        job_idmuoncalotrigegammamucombjettau = job2val['idmuoncalotrigegammamucombjettau']
        job_idmuoncalotrigegammamucombjettaumon = job2val['idmuoncalotrigegammamucombjettaumon']
        job_all = job2val['all']

        #Calculate output quantities:
        out_core = job_core
        out_idex = job_idmuon-job_muon
        out_muonex = job_idmuon-job_id
        out_caloex = job_idmuoncalo-job_idmuon
        out_detcommon = (job_idmuon+job_calo)-(job_idmuoncalo+job_core)
        out_idmuoncommon=(job_id+job_muon+job_idmuoncalo)-(job_idmuon*2+job_calo)

        self.sumCheck(out_core+out_detcommon+out_idmuoncommon+out_idex+out_muonex+out_caloex,job_idmuoncalo)

        out_trigger = job_idmuoncalotrig-job_idmuoncalo
        out_egamma = job_idmuoncalotrigegamma-job_idmuoncalotrig
        out_muoncomb = job_idmuoncalotrigegammamucomb-job_idmuoncalotrigegamma
        out_jet = job_idmuoncalotrigegammamucombjet-job_idmuoncalotrigegammamucomb
        out_tau = job_idmuoncalotrigegammamucombjettau-job_idmuoncalotrigegammamucombjet
        out_mon = job_idmuoncalotrigegammamucombjettaumon-job_idmuoncalotrigegammamucombjettau
        out_output = job_all-job_idmuoncalotrigegammamucombjettaumon

        self.sumCheck(job_idmuoncalo+out_trigger+out_egamma+out_muoncomb+out_jet+out_tau+out_mon+out_output,job_all)

        #Finally put in dictionary and return:
        domains={}
        domains['core']=out_core
        domains['detcommon']=out_detcommon
        domains['idmuoncommon']=out_idmuoncommon
        domains['idex']=out_idex
        domains['muonex']=out_muonex
        domains['caloex']=out_caloex
        domains['muoncomb'] = out_muoncomb
        domains['egamma'] = out_egamma
        domains['jet'] = out_jet
        domains['tau'] = out_tau
        domains['trig'] = out_trigger
        domains['mon'] = out_mon
        domains['output'] = out_output
        return domains

    def getLogFileNamingSchemeDefinitions(self):
        log2job={}
        log2job['rawtoesd_nothing.log'] = 'core'
        log2job['rawtoesd_id.log'] = 'id'
        log2job['rawtoesd_muon.log'] = 'muon'
        log2job['rawtoesd_calo.log'] = 'calo'
        log2job['rawtoesd_idmuon.log'] = 'idmuon'
        log2job['rawtoesd_idmuoncalo.log'] = 'idmuoncalo'
        log2job['rawtoesd_idmuoncalotrig.log'] = 'idmuoncalotrig'
        log2job['rawtoesd_idmuoncalotrigegamma.log'] = 'idmuoncalotrigegamma'
        log2job['rawtoesd_idmuoncalotrigegammamucomb.log'] = 'idmuoncalotrigegammamucomb'
        log2job['rawtoesd_idmuoncalotrigegammamucombjet.log'] = 'idmuoncalotrigegammamucombjet'
        log2job['rawtoesd_idmuoncalotrigegammamucombjettau.log'] = 'idmuoncalotrigegammamucombjettau'
        log2job['rawtoesd_idmuoncalotrigegammamucombjettaumon.log'] = 'idmuoncalotrigegammamucombjettaumon'
        log2job['rawtoesd_all.log'] = 'all'
        return log2job
