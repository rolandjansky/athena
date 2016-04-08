# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#          John Chapman (chapman@hep.phy.cam.ac.uk)
#  Initial version: August 2009

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_Digi(DomainCalcBase):

    def __init__(self,pileup=False,detailedResults=True):
        self.details = detailedResults
        self.pileup = pileup

    def getName(self):
        return "digipile" if self.pileup else "digi"

    def getPrettyName(self):
        return "Digitization 3.5e33" if self.pileup else "Digitization"

    def defineJobs(self):
        return ['bcmtile','bcm','tile',
                'id','calo','muon',
                'idcalo','idmuon','calomuon',
                'idcalomuon','all_noLVL1_noRDO','all_noRDO','all',
                'id_nobcm','id_nopixel','id_nosct','id_notrt',
                'calo_nolar','calo_notile',
                'muon_nocsc','muon_nomdt','muon_norpc','muon_notgc']

    def defineDomains(self):
        if self.details:
            return ['core','detcommon','idcalocommon','idmuoncommon','calomuoncommon',
                    'idother','bcmex','pixelex','sctex','trtex',
                    'muonshare','cscex','mdtex','rpcex','tgcex',
                    'caloshare','larex','tileex',
                    'truth','lvl1','rdo']
        else:
            return ['core','detcommon','idcalocommon','idmuoncommon','calomuoncommon',
                    'idex','muonex','caloex',
                    'truth','lvl1','rdo']

    def calcDomains(self,job2val):
        #job2val: dictionary with string keys and Measurement values.

        #NB: Make calculations direct and only let each job appear
        #once in calculations (to avoid unnecessary missing domain
        #entries and to get correct error estimates).

        #The job configurations and calculations here are based on the
        #idea that the various sub-systems are almost entirely
        #uncorrelated in digitization jobs. To get the core
        #contribution we use a job with bcm and tile enabled (chosen
        #because those two should be *really* uncorrelated, and
        #because they are relatively light-weight). Of course, they
        #probably both share the truth overhead, so expect the truth
        #domain contribution to be around 0.

        #Get out raw job values:
        #For core:
        job_bcmtile = job2val['bcmtile']
        job_tile = job2val['tile']
        job_bcm = job2val['bcm']
        core = job_bcm+job_tile-job_bcmtile

        #job_bpipetile = job2val['bpipetile']
        #job_bpipe = job2val['bpipe']
        #core2 = job_bpipe+job_tile-job_bpipetile
        #core = job_id_nopixelscttrt-job_id_nopixelscttrt#FIXME
        #For id/muon/calo breakdowns:
        job_idcalo = job2val['idcalo']
        job_idmuon = job2val['idmuon']
        job_calomuon = job2val['calomuon']
        job_calo = job2val['calo']
        job_id = job2val['id']
        job_idcalomuon = job2val['idcalomuon']
        job_muon = job2val['muon']

        idex   = job_idcalomuon-job_calomuon
        muonex = job_idcalomuon-job_idcalo
        caloex = job_idcalomuon-job_idmuon
        idcalocommon=job_calomuon+job_idmuon-job_idcalomuon-job_muon
        calomuoncommon=job_idcalo+job_idmuon-job_idcalomuon-job_id#still negative!
        idmuoncommon=job_calomuon+job_idcalo-job_idcalomuon-job_calo
        detcommon = job_idcalomuon-idcalocommon-idmuoncommon-calomuoncommon-idex-muonex-caloex-core
        self.sumCheck(job_idcalomuon,core+detcommon+idcalocommon+idmuoncommon+calomuoncommon+idex+caloex+muonex)

        if self.details:
            #Even more fine-grained breakdown of InDet:
            job_id_nobcm = job2val['id_nobcm'] #bpipe+pixel+sct+trt
            job_id_nopixel = job2val['id_nopixel'] #bpipe+bcm+sct+trt
            job_id_nosct = job2val['id_nosct'] #bpipe+bcm+pixel+trt
            job_id_notrt = job2val['id_notrt'] #bpipe+bcm+pixel+sct
            #job_id_nopixelsct = job2val['id_nopixelsct'] #bpipe+bcm+trt
            #job_id_nopixelscttrt = job2val['id_nopixelscttrt'] #bpipe+bcm
            out_bcmex   = job_id - job_id_nobcm
            out_pixelex = job_id - job_id_nopixel
            out_sctex   = job_id - job_id_nosct
            out_trtex   = job_id - job_id_notrt
            out_idother = idex - out_bcmex - out_pixelex - out_sctex - out_trtex
            # pixsct_ex = job_id - job_id_nopixelsct
            #out_idother = job_id_nopixelscttrt-core-detcommon-idmuoncommon-idcalocommon
            #out_pixsctshare = pixsct_ex-out_pixelex-out_sctex
            #pixscttrt_ex=job_id-job_id_nopixelscttrt
            #out_idshare = pixscttrt_ex - pixsct_ex - out_trtex
            self.sumCheck(idex,out_idother+out_bcmex+out_pixelex+out_sctex+out_trtex)

        if self.details:
            #Even more fine-grained breakdown of calo:
            job_calo_nolar  = job2val['calo_nolar']
            job_calo_notile = job2val['calo_notile']
            out_larex     = job_calo - job_calo_nolar
            out_tileex    = job_calo - job_calo_notile
            out_caloshare = caloex - out_larex - out_tileex

            self.sumCheck(caloex,out_caloshare+out_larex+out_tileex)

        if self.details:
            #Even more fine-grained breakdown of muon:
            job_muon_nocsc = job2val['muon_nocsc']
            job_muon_nomdt = job2val['muon_nomdt']
            job_muon_norpc = job2val['muon_norpc']
            job_muon_notgc = job2val['muon_notgc']

            out_cscex     = job_muon - job_muon_nocsc
            out_mdtex     = job_muon - job_muon_nomdt
            out_rpcex     = job_muon - job_muon_norpc
            out_tgcex     = job_muon - job_muon_notgc
            out_muonshare = muonex - out_cscex - out_mdtex - out_rpcex - out_tgcex

            self.sumCheck(muonex, out_muonshare + out_cscex + out_mdtex + out_rpcex + out_tgcex)

        #Finally get truth, lvl1 and rdo writing breakdown:
        #job_truth = job2val['truth']
        job_all_noLVL1_noRDO = job2val['all_noLVL1_noRDO']
        job_all_noRDO        = job2val['all_noRDO']
        job_all              = job2val['all']

        ##out_truth = job_truth-core
        #the only difference between these two jobs is that DetFlags.Truth_setOn()
        #shows that the Truth info is read in anyway - might be important for pile-up digi though
        out_truth = job_all_noLVL1_noRDO-job_idcalomuon
        out_lvl1  = job_all_noRDO-job_all_noLVL1_noRDO
        out_rdo   = job_all-job_all_noRDO

        self.sumCheck(job_all,job_all_noLVL1_noRDO+out_rdo+out_lvl1)
        self.sumCheck(job_all,core+detcommon+
                      idcalocommon+idmuoncommon+calomuoncommon+
                      idex+caloex+muonex+
                      out_truth+out_rdo+out_lvl1)

        #NB: Can use the job_all_noLVL1_noRDO as well to compare with previous!


        #Temporary domains:
        domains={}
        domains['core']=core
        #domains['core2']=core2
        domains['detcommon']=detcommon
        domains['idcalocommon']=idcalocommon
        domains['idmuoncommon']=idmuoncommon
        domains['calomuoncommon']=calomuoncommon

        if self.details:
            domains['idother'] = out_idother
            #domains['idshare'] = out_idshare
            #domains['pixsctshare'] = out_pixsctshare
            domains['bcmex'] = out_bcmex
            domains['pixelex'] = out_pixelex
            domains['sctex'] = out_sctex
            domains['trtex'] = out_trtex
        else:
            domains['idex'] = idex

        if self.details:
            domains['muonshare'] = out_muonshare
            domains['cscex'] = out_cscex
            domains['mdtex'] = out_mdtex
            domains['rpcex'] = out_rpcex
            domains['tgcex'] = out_tgcex
        else:
            domains['muonex'] = muonex

        if self.details:
            domains['larex'] = out_larex
            domains['tileex'] = out_tileex
            domains['caloshare'] = out_caloshare
        else:
            domains['caloex'] = caloex

        domains['truth'] = out_truth
        domains['lvl1'] = out_lvl1
        domains['rdo'] = out_rdo

        return domains


    def getCDJobDefinitions(self,dbkey):
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        if self.pileup:
            jobdefs=[("pileup3_5E33_core_noRDOs",'bcmtile'),
                     ("pileup3_5E33_bcm_only_noRDOs",'bcm'),
                     ("pileup3_5E33_tile_only_noRDOs",'tile'),
                     ("pileup3_5E33_id_noRDOs",'id'),
                     ("pileup3_5E33_calo_noRDOs",'calo'),
                     ("pileup3_5E33_muon_noRDOs",'muon'),
                     ("pileup3_5E33_id_calo_noRDOs",'idcalo'),
                     ("pileup3_5E33_id_muon_noRDOs",'idmuon'),
                     ("pileup3_5E33_calo_muon_noRDOs",'calomuon'),
                     ("pileup3_5E33_id_calo_muon_noRDOs",'idcalomuon'),
                     ("pileup3_5E33_all_noRDOs",'all_noLVL1_noRDO'),
                     ("pileup3_5E33_all_LVL1_noRDOs",'all_noRDO'),
                     ("pileup3_5E33_id_nobcm_noRDOs",'id_nobcm'),
                     ("pileup3_5E33_id_nopixel_noRDOs",'id_nopixel'),
                     ("pileup3_5E33_id_nosct_noRDOs",'id_nosct'),
                     ("pileup3_5E33_id_notrt_noRDOs",'id_notrt'),
                     ("pileup3_5E33_calo_nolar_noRDOs",'calo_nolar'),
                     ("pileup3_5E33_calo_notile_noRDOs",'calo_notile'),
                     ("pileup3_5E33_muon_nocsc_noRDOs",'muon_nocsc'),
                     ("pileup3_5E33_muon_nomdt_noRDOs",'muon_nomdt'),
                     ("pileup3_5E33_muon_norpc_noRDOs",'muon_norpc'),
                     ("pileup3_5E33_muon_notgc_noRDOs",'muon_notgc'),
                     ("pileup3_5E33_all_LVL1_RDOs",'all')]
        else:
            jobdefs=[("core_noRDOs",'bcmtile'),
                     ("bcm_only_noRDOs",'bcm'),
                     ("tile_only_noRDOs",'tile'),
                     ("id_noRDOs",'id'),
                     ("calo_noRDOs",'calo'),
                     ("muon_noRDOs",'muon'),
                     ("id_calo_noRDOs",'idcalo'),
                     ("id_muon_noRDOs",'idmuon'),
                     ("calo_muon_noRDOs",'calomuon'),
                     ("id_calo_muon_noRDOs",'idcalomuon'),
                     ("all_noRDOs",'all_noLVL1_noRDO'),
                     ("all_LVL1_noRDOs",'all_noRDO'),
                     ("id_nobcm_noRDOs",'id_nobcm'),
                     ("id_nopixel_noRDOs",'id_nopixel'),
                     ("id_nosct_noRDOs",'id_nosct'),
                     ("id_notrt_noRDOs",'id_notrt'),
                     ("calo_nolar_noRDOs",'calo_nolar'),
                     ("calo_notile_noRDOs",'calo_notile'),
                     ("muon_nocsc_noRDOs",'muon_nocsc'),
                     ("muon_nomdt_noRDOs",'muon_nomdt'),
                     ("muon_norpc_noRDOs",'muon_norpc'),
                     ("muon_notgc_noRDOs",'muon_notgc'),
                     ('mc08_full_atlas_no_pileup','all')]
        l=[]
        for dbjobname,jobname in jobdefs:
            l += [ PerfMonDozerJobDef(dbkey,'Simulation/Digitization',dbjobname,jobname) ]
        return l

    def getDefaultRTTEmailList(self,build,cache,cmtconfig):
        return 'chapman@hep.phy.cam.ac.uk,thomas.kittelmann@cern.ch'

    def getLogFileNamingSchemeDefinitions(self):
        if self.pileup:
            return None

        log2job={}
        log2job['digitization_core.log'] = 'bcmtile'
        log2job['digitization_bcm.log'] = 'bcm'
        log2job['digitization_tile.log'] = 'tile'

        log2job['digitization_id_noRDO.log'] = 'id'
        log2job['digitization_calo_noRDO.log'] = 'calo'
        log2job['digitization_muon_noRDO.log'] = 'muon'

        log2job['digitization_id_calo_noRDO.log'] = 'idcalo'
        log2job['digitization_id_muon_noRDO.log'] = 'idmuon'
        log2job['digitization_calo_muon_noRDO.log'] = 'calomuon'

        log2job['digitization_id_calo_muon_noRDO.log'] = 'idcalomuon'
        log2job['digitization_all_noLVL1_noRDO.log'] = 'all_noLVL1_noRDO'
        log2job['digitization_all_noRDO.log'] = 'all_noRDO'
        log2job['digitization_all.log'] = 'all'

        log2job['digitization_id_nobcm_noRDO.log'] = 'id_nobcm'
        log2job['digitization_id_nopixel_noRDO.log'] = 'id_nopixel'
        log2job['digitization_id_nosct_noRDO.log'] = 'id_nosct'
        log2job['digitization_id_notrt_noRDO.log'] = 'id_notrt'
        log2job['digitization_calo_nolar_noRDO.log'] = 'calo_nolar'
        log2job['digitization_calo_notile_noRDO.log'] = 'calo_notile'
        log2job['digitization_muon_nocsc_noRDO.log'] = 'muon_nocsc'
        log2job['digitization_muon_nomdt_noRDO.log'] = 'muon_nomdt'
        log2job['digitization_muon_norpc_noRDO.log'] = 'muon_norpc'
        log2job['digitization_muon_notgc_noRDO.log'] = 'muon_notgc'

        return log2job
