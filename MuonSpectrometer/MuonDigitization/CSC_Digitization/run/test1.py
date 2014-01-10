# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os


NewDigitEDM = True
#NewDigitEDM = False

InputFiles =[
    # GEO-16-00-00
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000001.pool.root.1",
#    "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000002.pool.root.1",
#    "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000003.pool.root.1",
#    "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000004.pool.root.1",
#    "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000005.pool.root.1"
    ]

#DetDescrVersion = "ATLAS-GEO-16-00-00"

include("../share/RunCSC_Digitization.py")
