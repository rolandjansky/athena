#______________________________________________________________________________________________________________________________________
# Feb. 2010
# LM: use_PYINIT setup example jO
# use_PYINIT is a pyinit block entry, that enables setting frame, beam, target particles and CMS for generated events 
# these are than passed to PYINIT(FRAME,BEAM,TARGET,WIN) PYHITA call
# this jO file has some use_PYINIT setting techical examples, explaining rules for omiting parameters, error handling, white-space rules...
# examples for allowed (these have # ok, comment) and what is not allowed (# not ok comment) syntax are available below process setting
# formally the examples generate Zmumu Pythia events in pp or p~p collisions, using the ATLAS default tune for the used release (whatever that is ...)
#______________________________________________________________________________________________________________________________________

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

#_________________________________________________________________________________

#1. setup Z production using PYGIVE calls
#---------------------------------------------------------------------------------
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
#---------------------------------------------------------------------------------
#_________________________________________________________________________________
#-------------------------------------------------------------------------------------------------------------
# initialization setup:
#-------------------------------------------------------------------------------------------------------------
# general Athena syntax is : 
# Pythia.PythiaCommand +=["pyinit use_PYINIT FRAME BEAM TARGET WIN"]
# which is translated to PYTHIA PYINIT SUBROUTINE CALL as: 
# PYINIT(FRAME,BEAM,TARGET,WIN)
# allowed FRAME BEAM TARGET WIN values are same as for Pythia standalone running
# when using pyinit use_PYINIT please make sure that:
# 1) all four FRAME BEAM TARGET WIN values are passed, even if some of them correspond to LHC defaults
# 2) WIN is passed as a double (i.e. not as an integer) 
# 3) info on the Athena and PYTHIA setup can be checked is printed-out before event generation is started 
#-------------------------------------------------------------------------------------------------------------
# EXAMPLES should make it clear .... 
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS P P 10000.0"]     # ok, set p-p explicitly 
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p+ P+ 10000.0"]   # ok, set p-p explicitly 
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p+ p+   10000.0"] # ok, note extra whitespace, set p-p explicitly
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p+ p + 10000.0"]  # not ok: note extra whitespace, 
                                                                  # PythiaCommand vector splist string into blocks by white-spaces
                                                                  # error thrown by PYTHIA 
#---------------------------------------------------------------------------------
# just for fun - another not ok example:
# some ~ obscure intentional attempts to trick error-handling can possibli pass, here's one:
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p+ p ~ 10000.0"]   # not parsed correctly, ECM set to 0 
#Pythia.PythiaCommand +=["pyinit win 10000."]                      # but if you now override ECM it'll still work ... but p-p will be produced
                                                                   # note that WARNINGS are printed so used has a chance to cathc such error ... 
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p~ P+ 10000.0"]   # ok, set pbar-p, non-standard setup WARNING is printed  
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p~ 10000.0"]      # not ok: ommiting beam/target is not parsed correctly, 
                                                                  # please set full string, error thrown by PYTHIA
#---------------------------------------------------------------------------------
#Pythia.PythiaCommand +=["pyinit use_PYINIT CMS p p 10000.0"]
#Pythia.PythiaCommand +=["pyinit win 7000."]                       # ok you can override CM energy
#---------------------------------------------------------------------------------

# ... Tauola
#include ( "EvgenJobOptions/MC8_Tauola_Fragment.py" )

# ... Photos
#include ( "EvgenJobOptions/MC8_Photos_Fragment.py" )

