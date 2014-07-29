#______________________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch 
#
# jO purpose:show how (and why) to query Pythia parameter values ather generator initialization and event generation
#
# run jO like: athena.py param_query_Zmumu_jO.py
#
# please note: for ATLAS physics studies the event generation follow ATLAS central production standards: 
# use the EVGEN jobtransforms and the relevant official include files for the common ATLAS parameters
# (not done in this example)
#
#----------------------------------------------------------------------------------------------------------------------
# jO description:
#
# - generate 10 Zmumu Pythia events; PYGIVE is used for the Zmumu process setup
# - Z mass and width are set manually, they values are queried before and after the generator initialization in order
#   to demonstrate that the Z-width is reset during the initialization 
# - info on # of MI in the events is extracted by parameter query after the event generation 
#----------------------------------------------------------------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()
Pythia = topAlg.Pythia

theApp.EvtMax = 10

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
                                 "PYTHIA_INIT 820021 2347532"]

#----------------------------------------------------------------------------------------------------------------------
#using PYGIVE (ATLAS independant syntax for params.)
Pythia.PygiveCommand += [ "msel=0",           # Users decay choice.
                          "parj(90)=20000",   # Turn off FSR.
                          "mdcy(15,1)=0",     # Turn off tau decays.
                          "msub(1)=1",        # Create Z bosons.
                          "ckin(1)=60.0",     # Lower invariant mass.
                          "mdme(174,1)=0",
                          "mdme(175,1)=0",
                          "mdme(176,1)=0",
                          "mdme(177,1)=0",
                          "mdme(178,1)=0",
                          "mdme(179,1)=0",
                          "mdme(182,1)=0",    # Switch for Z->ee.
                          "mdme(183,1)=0",
                          "mdme(184,1)=1",    # Switch for Z->mumu.
                          "mdme(185,1)=0",
                          "mdme(186,1)=0",    # Switch for Z->tautau.
                          "mdme(187,1)=0"]

#----------------------------------------------------------------------------------------------------------------------
# not setting the tune - ATLAS default used for the Athena release you are running will be assumed
#----------------------------------------------------------------------------------------------------------------------
# set Z mass and width (to a too narrow value):
Pythia.PygiveCommand += [ "PMAS(23,1)=91.1876",
                          "PMAS(23,2)=2.0"]
#----------------------------------------------------------------------------------------------------------------------
# 1) query the Z mass and width values (same as when using PYGIVE queries before PYINIT in Pythia standalone) :
# values are written to the stand. output && reflect the state before Pythia initialization has been completed 
Pythia.PygiveCommand += [ "PMAS(23,1)=",
                          "PMAS(23,2)="]
#----------------------------------------------------------------------------------------------------------------------
# 2) query the Z mass and width values after Pythia initialization has been completed:
# values are written to par_after_init.out; since the Z-width is recalculated by (default by) Pythia the Z-width values
# will be different for the queries 1) and 2)
Pythia.Param_Query_AfterInit += [ "PMAS(23,1)=",
                                  "PMAS(23,2)="]
#----------------------------------------------------------------------------------------------------------------------
# 3) example how to extract event-specific modelling-related info:
# get # of MI after event generation: written to file par_after_gen.out
Pythia.Param_Query_AfterGen += [ "MINT(31)=" ]
