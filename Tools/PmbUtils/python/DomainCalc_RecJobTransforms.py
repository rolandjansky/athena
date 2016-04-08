# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: April 2010

#  Very simple so far - simply define jobs named directly by their
#  userJobId's, and do not define any domain definitions.

from ValuePerDomainCalcUtils import DomainCalcBase

class DomainCalc_RecJobTransforms(DomainCalcBase):

    def __init__(self):
        return

    def getName(self):
        return "rectrfs"
    def getPrettyName(self):
        return "RecJobTransforms"

    def defineJobs(self):
        return [ "aodtotag_test",
                 "mc10_rdotobs",
                 "mc10_bstoaod",
                 "mc10_default",
                 "mc10_default_short",
                 "mc10_ignore",
                 "mc10_stdcmalloc",
                 "mc10_memleak",
                 "mc_heavy_ion",
                 "data_heavy_ion",
                 "cosmis_allstream",
                 "data_7TeV_allstreams",
                 "data_7TeV_fpe",
                 "data_7TeV_errordetction",
                 "data_7TeV_memleak",
                 'data_7TeV_descop2',
                 "data_7TeV_descop4",
                 "data_7TeV_nocalo",
                 "data_7TeV_noid",
                 "data_7TeV_nomuon",
                 "data_7TeV_nojet",
                 "data_7TeV_noegamma",
                 "data_7TeV_nocombmuon",
                 "data_7TeV_notau",
                 "data_7TeV_notrig",
                 "data_7TeV_diffinput",
                 "splash_events",
                 'amitag_q120',
                 "amitag_q121",
                 "amitag_q122",
                 'amitag_q120_notrig',
                 'amitag_q121_notrig',
                 'amitag_q122_notrig',
                 'q121_dbrelease_pcache',
                 'q121_dbrelease_current' ]

    def getCDJobDefinitions(self,dbkey):
        from CoolDozerAccessUtils import PerfMonDozerJobDef
        l=[]
        for j in self.defineJobs():
            l += [ PerfMonDozerJobDef(dbkey,'Reconstruction/RecExample/RecJobTransformTests',j,j) ]
        return l
