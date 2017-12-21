#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.grid
import DerivationTags
import Data_rel21
import MC16_TOPQ1

config = TopExamples.grid.Config()
config.code          = 'top-xaod'
config.settingsFile  = 'validation-cuts.txt'

config.gridUsername  = 'iconnell'
config.suffix        = '17-10-03'
config.excludedSites = ''
config.noSubmit      = False
config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'
config.CMake         = True
#config.groupProduction = True # for group production only; if True, gridUsername must be e.g. 'phys-top'
# by default the requested memory is set to 2GB, if you need to increase this, please disable the line below!!!
# config.memory = '4000' 

###############################################################################

###Command line interface
###If you want a script that ask you what you want to run on interactively,
###and uses lists of primary xAODs to convert them as TOPQ derivations
###Otherwise you can edit the automatic scripts below
#names, derivation, ptag = DerivationTags.InteractiveSubmission()
#samples = TopExamples.grid.Samples(names)
#TopExamples.grid.convertAODtoTOPQ(derivation, ptag, samples)
#TopExamples.grid.submit(config, samples)

###############################################################################

# Data - look in Data_rel21.py
# Change if you want TOPQ2/3/4/5
names = ['Data15_TOPQ1', 
         'Data16_TOPQ1', 
         'Data17_TOPQ1']
samples = TopExamples.grid.Samples(names)
TopExamples.grid.submit(config, samples)

###############################################################################

###MC Simulation - look in MC16_TOPQ1.py
###Using list of TOPQ1 25ns MC samples, consistent mixture of p-tags
###Edit these lines if you don't want to run everything!
names = ['TOPQ1_ttbar_PowPy8',
         'TOPQ1_ttbar_dil_PowPy8',
         'TOPQ1_tchan_lep_PowPy6',
         'TOPQ1_Wt_inc_PowPy6',
         'TOPQ1_schan_noAllHad_PowPy6',
         'TOPQ1_diboson_Sherpa',
         'TOPQ1_Zjets_Sherpa221',
         'TOPQ1_Wjets_Sherpa221',
         'TOPQ1_ttV',    
         ]
samples = TopExamples.grid.Samples(names)
TopExamples.grid.submit(config, samples)

