# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: September 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_RAWtoESDTrf(DomainCalcBase):

    def __init__(self,userjobidsuf=None,logfiles=True,name=None,prettyname=None):
        self.__userjobidsuf = userjobidsuf
        self.__name= name if name else  'rawtoesd'
        self.__prettyname = prettyname if prettyname else 'RAWtoESD'
        self.__logfiles=logfiles
        return

    def getName(self):
        return self.__name

    def getPrettyName(self):
        return self.__prettyname

    def defineJobs(self):
        l= ['core','id','muon','calo','idmuon','idmuoncalo','idmuoncalomucomb',
                'nojet','jet','jetbtag','jetbtagtau','notrig',
                'notrigmon','notrigmonout',
                'trigbase','trigbaselvl2','trig','trigmon','all']
        if self.__userjobidsuf:
            l += ['tcmalloc_all']
            l += ['details_all']
            l += ['long_all']
            l += ['long_tcmalloc_all']
            if self.__userjobidsuf=='bs':
                #l += ['athenaapp_all']
                l += ['dropcfg_all']
                l += ['easytrf_ftag_jettauetmiss']
                l += ['easytrf_ftag_muons']
                l += ['easytrf_ftag_egamma']
                l += ['easytrf_ftag_jettauetmiss_3evts']
                l += ['easytrf_ftag_muons_3evts']
                l += ['easytrf_ftag_egamma_3evts']
        return l

    def defineDomains(self):
        l = ['core','det','idmu','idex','muonex','caloex','mucomb',
                'egamma','jet','btag','tau','etmiss','mon','out',
                'trigbase','triglvl2','trigef','trigmon','trigout']
        if self.__userjobidsuf:
            l.extend(['tcmalloc','long','tcm_long'])
        return l

    def domainOffsets(self):
        if self.__userjobidsuf=='bs':
            return {'caloex':'auto'}

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
        job_idmuoncalomucomb = job2val['idmuoncalomucomb']
        job_nojet = job2val['nojet']
        job_jet = job2val['jet']
        job_jetbtag = job2val['jetbtag']
        job_jetbtagtau = job2val['jetbtagtau']
        job_notrig = job2val['notrig']
        job_notrigmon = job2val['notrigmon']
        job_notrigmonout = job2val['notrigmonout']
        job_trigbase = job2val['trigbase']
        job_trigbaselvl2 = job2val['trigbaselvl2']
        job_trig = job2val['trig']
        job_trigmon = job2val['trigmon']
        job_all = job2val['all']

        #Calculate output quantities:

        #core/id/muon/calo breakdown:
        out_core = job_core
        out_idex = job_idmuon-job_muon
        out_muonex = job_idmuon-job_id
        out_caloex = job_idmuoncalo-job_idmuon
        out_det = (job_idmuon+job_calo)-(job_idmuoncalo+job_core)
        out_idmu=(job_id+job_muon+job_idmuoncalo)-(job_idmuon*2+job_calo)
        self.sumCheck(job_idmuoncalo,out_core+out_det+out_idmu+out_idex+out_muonex+out_caloex)

        #comb-reco/egamma/jet/btag/tau/etmiss:
        out_mucomb = job_idmuoncalomucomb-job_idmuoncalo
        out_egamma = job_nojet-job_idmuoncalomucomb
        out_jet = job_jet-job_nojet
        out_btag = job_jetbtag-job_jet
        out_tau = job_jetbtagtau-job_jetbtag
        out_etmiss = job_notrig-job_jetbtagtau
        self.sumCheck(job_notrig,job_idmuoncalo+out_mucomb+out_egamma+out_jet+out_btag+out_tau+out_etmiss)

        #monitoring and output contributions without trigger:
        out_mon = job_notrigmon-job_notrig
        out_out = job_notrigmonout-job_notrigmon
        self.sumCheck(job_notrigmonout,job_notrig+out_mon+out_out)

        #direct trigger contributions:
        out_trigbase = job_trigbase-job_notrig
        out_triglvl2 = job_trigbaselvl2-job_trigbase
        out_trigef = job_trig-job_trigbaselvl2
        self.sumCheck(job_trig,job_notrig+out_trigbase+out_triglvl2+out_trigef)

        #monitoring and output contributions with trigger:
        out_trigmon = job_trigmon-job_trig-out_mon
        out_trigout = job_all-job_trigmon-out_out
        self.sumCheck(job_all,job_trig+out_trigmon+out_trigout+out_mon+out_out)

        #Finally put in dictionary and return:
        domains={}
        domains['core'] = out_core
        domains['det'] = out_det
        domains['idmu'] = out_idmu
        domains['idex'] = out_idex
        domains['muonex'] = out_muonex
        domains['caloex'] = out_caloex
        domains['mucomb'] = out_mucomb
        domains['egamma'] = out_egamma
        domains['jet'] = out_jet
        domains['btag'] = out_btag
        domains['tau'] = out_tau
        domains['etmiss'] = out_etmiss
        domains['mon'] = out_mon
        domains['out'] = out_out
        domains['trigbase'] = out_trigbase
        domains['triglvl2'] = out_triglvl2
        domains['trigef'] = out_trigef
        domains['trigmon'] = out_trigmon
        domains['trigout'] = out_trigout

        #For RTT based tests we also add the effects of running for a long time or with tcmalloc:
        if self.__userjobidsuf:
            domains['tcmalloc'] = job2val['tcmalloc_all'] - job2val['all']
            domains['long'] = job2val['long_all'] - job2val['all']
            domains['tcm_long'] = job2val['long_tcmalloc_all'] - job2val['tcmalloc_all']

        return domains

    def getCDJobDefinitions(self,dbkey):
        if not self.__userjobidsuf:
            return None
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        l=[]
        for job in self.defineJobs():
            l += [ PerfMonDozerJobDef(dbkey,'Reconstruction/RecExample/RecPerfTests',
                                      job+'_'+self.__userjobidsuf if not job.startswith('easy') else job,job) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'atlas-computing-pmb-autowarnings@cern.ch,REPLYTO:hn-atlas-software-performance-monitoring@cern.ch' if self.__userjobidsuf else None

    def getLogFileNamingSchemeDefinitions(self):
        if not self.__logfiles:
            return None
        log2job={}
        for jobname in self.defineJobs():
            log2job['rawtoesdtrf_'+str(jobname)+'.log'] = jobname
        return log2job

    def getLogAndWarningThresholds_Domains(self,measname,domain,build,cache,cmtconfig):
        if measname=='vmem':
            if 'tcm' in domain:
                return (5.0,40.0)
            return (5.0,20.0)
        return (None,None)

    def getLogAndWarningThresholds_Jobs(self,measname,jobname,build,cache,cmtconfig):
        if measname=='vmem':
            return (5.0,20.0)
        return (None,None)
