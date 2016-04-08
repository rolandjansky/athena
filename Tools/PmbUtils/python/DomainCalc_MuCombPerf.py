# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch) + Rikard Sandstr\"om
#  Initial version: August 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_MuCombPerf(DomainCalcBase):

    def __init__(self,userjobidprefix=None,logfiles=True,name=None,prettyname=None):
      self.__userjobidprefix = userjobidprefix
      self.__name= name if name else  'mucombperf'
      self.__prettyname = prettyname if prettyname else 'MuonCombPerf'
      self.__logfiles=logfiles
      self.domain2col={ 'shared_MuidStaco' : (0.8,0.8,0),#dark yellow
                        'shared_MooreMuGirl' : (154./255, 205./255,50./255),
                        'Moore' : (215/255.,23./255,170./255),#purple
                        'MuidSA' : (1,98/255.,30/255.),#orange
                        'MuidCB' : (0,0,1),
                        'MuGirl' : (1,20/255.,147/255.),#pink
                        'MuTagIMO' : (1,165./255,0),
                        'Staco' : (100/255.,149/255.,237/255.),
                        'MuTag' : (1,0.,0.),
                        'Muonboy' : (75/255.,231/255.,221/255.),
                        'CaloMuon' : (130/255.,0/255.,64/255.),
                        'TBMuonboy' : (49/255.,0,0),
                        'TBMoore' : (49/255.,79/255.,79/255.)
                        }
      return

    def getName(self):
        return self.__name

    def getPrettyName(self):
        return self.__prettyname

    def defineJobs(self):
        return ['noMuidNoStaco','all','noMuGirl','noMuTag','noMuid','noMuidCB','noMuTagIMO',
                'noMuidSA','noStaco','noStacoCB','noMoore','noCalo', 'TBMuonboy', 'TBMoore']#14

    def defineDomains(self):
        return ['shared_MuidStaco','MuidSA','MuidCB','Moore','shared_MooreMuGirl','MuGirl','MuTagIMO','CaloMuon','Staco','MuTag','Muonboy','TBMuonboy','TBMoore']#13

    def domainColour(self,domain):
        return self.domain2col[domain]

    def calcDomains(self,job2val):
        #job2val: dictionary with string keys and Measurement values.

        #NB: Make calculations direct and only let each job appear
        #once in calculations (to avoid unnecessary missing domain
        #entries and to get correct error estimates).

        #NB: We have 10 inputs and 9 outputs, but this is because we
        #are here not interested in the id+muon+calo baseline.

        #Get out raw job values:
        job_noMuidNoStaco = job2val['noMuidNoStaco']
        job_all = job2val['all']
        job_noMuGirl = job2val['noMuGirl']
        job_noMuTag = job2val['noMuTag']
        job_noMuid = job2val['noMuid']
        job_noMuidCB = job2val['noMuidCB']
        job_noMuTagIMO = job2val['noMuTagIMO']
        job_noMuidSA = job2val['noMuidSA']
        job_noStacoCB = job2val['noStacoCB']
        job_noStaco = job2val['noStaco']
        job_noMoore = job2val['noMoore']
        job_noCalo = job2val['noCalo']
        job_TBMuonboy = job2val['TBMuonboy']
        job_TBMoore = job2val['TBMoore']

        #Calculate output quantities:
        extra_MuidCol = job_all-job_noMuid
        extra_Muid = job_all-job_noMuidSA
        out_MuGirl = job_all-job_noMuGirl
        out_MuTagIMO = job_all-job_noMuTagIMO
#         out_Moore = extra_MuidCol-extra_Muid-out_MuGirl-out_MuTagIMO
        out_Moore = job_all-job_noMoore-extra_Muid-out_MuTagIMO
        out_MuidSA = job_noMuidCB-job_noMuidSA
        out_MuidCB = job_all-job_noMuidCB
        extra_stacocol = job_all-job_noStaco
        out_Staco = job_all-job_noStacoCB
        out_MuTag = job_all-job_noMuTag
        out_Muonboy = extra_stacocol-out_Staco-out_MuTag
        out_shared_MuidStaco = job_all-job_noMuidNoStaco-extra_MuidCol-extra_stacocol
        out_shared_MooreMuGirl = extra_MuidCol-extra_Muid-out_MuGirl-out_MuTagIMO-out_Moore
        out_CaloMuon = job_all-job_noCalo
        out_TBMuonboy = job_TBMuonboy-job_all
        out_TBMoore = job_TBMoore-job_all
        #FIXME: Make calculations direct.

        self.sumCheck(out_Moore+out_MuidSA+out_MuidCB+out_MuGirl+out_MuTagIMO+out_Staco+out_MuTag+out_Muonboy+out_shared_MuidStaco+out_shared_MooreMuGirl,(job_all-job_noMuidNoStaco))

        #Finally put in dictionary and return:
        domains={}
        domains['Moore']=out_Moore
        domains['MuidSA']=out_MuidSA
        domains['MuidCB']=out_MuidCB
        domains['MuGirl']=out_MuGirl
        domains['MuTagIMO']=out_MuTagIMO
        domains['CaloMuon']=out_CaloMuon
        domains['Staco']=out_Staco
        domains['MuTag']=out_MuTag
        domains['Muonboy']=out_Muonboy
        domains['shared_MuidStaco']=out_shared_MuidStaco
        domains['shared_MooreMuGirl']=out_shared_MooreMuGirl
        domains['TBMuonboy']=out_TBMuonboy
        domains['TBMoore']=out_TBMoore
        return domains

    def getCDJobDefinitions(self,dbkey):
        if not self.__userjobidprefix:
            return None
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        l=[]
        for job in self.defineJobs():
            l += [ PerfMonDozerJobDef(dbkey,'Reconstruction/MuonIdentification/MuonCombinedValidation/MuonRecConfigTests',
                                      self.__userjobidprefix+'_'+job,job) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'rikard.sandstrom@cern.ch,thomas.kittelmann@cern.ch'

    def getLogFileNamingSchemeDefinitions(self):
        if not self.__logfiles:
            return None
        log2job={}
        log2job['mcp_noMuidNoStaco.log'] = 'noMuidNoStaco'
        log2job['mcp_all.log'] = 'all'
        log2job['mcp_noMuGirl.log'] = 'noMuGirl'
        log2job['mcp_noMuTag.log'] = 'noMuTag'
        log2job['mcp_noMuid.log'] = 'noMuid'
        log2job['mcp_noMuidCB.log'] = 'noMuidCB'
        log2job['mcp_noMuTagIMO.log'] = 'noMuTagIMO'
        log2job['mcp_noMuidSA.log'] = 'noMuidSA'
        log2job['mcp_noStaco.log'] = 'noStaco'
        log2job['mcp_noStacoCB.log'] = 'noStacoCB'
        log2job['mcp_noMoore.log'] = 'noMoore'
        log2job['mcp_noCalo.log'] = 'noCalo'
        log2job['mcp_TBMuonboy.log'] = 'TBMuonboy'
        log2job['mcp_TBMoore.log'] = 'TBMoore'
        return log2job
        
