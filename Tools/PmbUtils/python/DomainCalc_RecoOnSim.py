# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_RecoOnSim(DomainCalcBase):

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
                          'trig' : (0,181./255.,18./255.)#dark green
                          }
        self.job2col={'core' : self.domain2col['core'],
                      'id' : self.domain2col['idex'],
                      'muon' : self.domain2col['muonex'],
                      'calo' : self.domain2col['caloex'],
                      'idmuon' : self.domain2col['idmuoncommon'],
                      'idmuoncalo' : self.domain2col['detcommon'],
                      'idmuoncalomuoncomb' : self.domain2col['muoncomb'],
                      'idmuoncalomuoncombegamma' : self.domain2col['egamma'],
                      'idmuoncalomuoncombegammajet' : self.domain2col['jet'],
                      'idmuoncalomuoncombegammajettau' : self.domain2col['tau'],
                      'idmuoncalomuoncombegammajettautrigger' : self.domain2col['trig'],
                      'idmuoncalomuoncombegammajettautriggerpers' : self.domain2col['output']
                      }

    def getName(self):
        return "rdotoesd"

    def getPrettyName(self):
        return 'Reco-on-MC08'

    def defineJobs(self):
        return ['core','id','muon','calo','idmuon','idmuoncalo',
                'idmuoncalomuoncomb',
                'idmuoncalomuoncombegamma',
                'idmuoncalomuoncombegammajet',
                'idmuoncalomuoncombegammajettau',
                'idmuoncalomuoncombegammajettautrigger',
                'idmuoncalomuoncombegammajettautriggerpers']

    def defineDomains(self):
        return ['core','idex','muonex','idmuoncommon','muoncomb','caloex',
                'detcommon','egamma','tau','jet','output','trig']

    def domainColour(self,domain):
        return self.domain2col[domain]

    def jobColour(self,job):
        return self.job2col[job]

    def domainOffsets(self):
        return {'trig':'auto'}

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
        job_idmuoncalomuoncomb = job2val['idmuoncalomuoncomb']
        job_idmuoncalomuoncombegamma = job2val['idmuoncalomuoncombegamma']
        job_idmuoncalomuoncombegammajet = job2val['idmuoncalomuoncombegammajet']
        job_idmuoncalomuoncombegammajettau = job2val['idmuoncalomuoncombegammajettau']
        job_idmuoncalomuoncombegammajettautrigger = job2val['idmuoncalomuoncombegammajettautrigger']
        job_idmuoncalomuoncombegammajettautriggerpers = job2val['idmuoncalomuoncombegammajettautriggerpers']

        #Calculate output quantities:
        out_core = job_core
        out_idex = job_idmuon-job_muon
        out_muonex = job_idmuon-job_id
        out_caloex = job_idmuoncalo-job_idmuon
        out_detcommon = (job_idmuon+job_calo)-(job_idmuoncalo+job_core)
        out_idmuoncommon=(job_id+job_muon+job_idmuoncalo)-(job_idmuon*2+job_calo)

        self.sumCheck(out_core+out_detcommon+out_idmuoncommon+out_idex+out_muonex+out_caloex,job_idmuoncalo)

        out_muoncomb = job_idmuoncalomuoncomb-job_idmuoncalo
        out_egamma = job_idmuoncalomuoncombegamma-job_idmuoncalomuoncomb
        out_jet = job_idmuoncalomuoncombegammajet-job_idmuoncalomuoncombegamma
        out_tau = job_idmuoncalomuoncombegammajettau-job_idmuoncalomuoncombegammajet
        out_trigger = job_idmuoncalomuoncombegammajettautrigger-job_idmuoncalomuoncombegammajettau
        out_pers = job_idmuoncalomuoncombegammajettautriggerpers-job_idmuoncalomuoncombegammajettautrigger

        self.sumCheck(job_idmuoncalo+out_muoncomb+out_egamma+out_jet+out_tau+out_trigger+out_pers,
                      job_idmuoncalomuoncombegammajettautriggerpers)

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
        domains['output'] = out_pers
        return domains

    def getCDJobDefinitions(self,dbkey):
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        recotest='Reconstruction/RecExample/RecExRecoTest'
        trigtest='Reconstruction/RecExample/RecExTrigTest'
        jobdefs = [('empty',recotest,'core'),
                   ('id',recotest,None),
                   ('muon',recotest,None),
                   ('calo',recotest,None),
                   ('idmuon',recotest,None),
                   ('idmuoncalo',recotest,None),
                   ('idmuoncalomuoncomb',recotest,None),
                   ('idmuoncalomuoncombegamma',recotest,None),
                   ('idmuoncalomuoncombegammajet',recotest,None),
                   ('nopers',recotest,'idmuoncalomuoncombegammajettau'),
                   ('noesd',trigtest,'idmuoncalomuoncombegammajettautrigger'),
                   ('esd',trigtest,'idmuoncalomuoncombegammajettautriggerpers')]
        l=[]
        for jobname,package,prettyname in jobdefs:
            l += [ PerfMonDozerJobDef(dbkey,package,'mc08_'+jobname,(jobname if prettyname==None else prettyname)) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'atlas-computing-pmb-autowarnings@cern.ch,REPLYTO:hn-atlas-software-performance-monitoring@cern.ch'

    def getLogFileNamingSchemeDefinitions(self):
        log2job={}
        log2job['def_rdotoesd_nothing_noesd.log'] = 'core'
        log2job['def_rdotoesd_indet_noesd.log'] = 'id'
        log2job['def_rdotoesd_muon_noesd.log'] = 'muon'
        log2job['def_rdotoesd_calo_noesd.log'] = 'calo'
        log2job['def_rdotoesd_idmuon_noesd.log'] = 'idmuon'
        log2job['def_rdotoesd_idmuoncalo_noesd.log'] = 'idmuoncalo'
        log2job['def_rdotoesd_idmuoncalomuoncombined_noesd.log'] = 'idmuoncalomuoncomb'
        log2job['def_rdotoesd_idmuoncaloegammamucomb_noesd.log'] = 'idmuoncalomuoncombegamma'
        log2job['def_rdotoesd_idmuoncaloegammamucombjet_noesd.log'] = 'idmuoncalomuoncombegammajet'
        log2job['def_rdotoesd_idmuoncaloegammamucombjettau_noesd.log'] = 'idmuoncalomuoncombegammajettau'
        log2job['def_rdotoesd_idmuoncaloegammamucombjettautrig_noesd.log'] = 'idmuoncalomuoncombegammajettautrigger'
        log2job['def_rdotoesd_all_plusesd.log'] = 'idmuoncalomuoncombegammajettautriggerpers'
        return log2job
