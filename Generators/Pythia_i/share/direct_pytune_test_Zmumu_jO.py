#______________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch
# demonstrate how one can call a PYTUNE tune and change some parameters set by the tune on top from jO;
#--------------------------------------------------------------------------------------------------------------
# use-case: one wishes to study an effect of parameter variation for a certain Pythia tune available via PYTUNE
#--------------------------------------------------------------------------------------------------------------
# problem:
# changing parameters on top of the values set explicitly by PYUTNE cannot be done by using MSTP(5) or Pythia.Tune_Name
# (which also uses MSTP(5) ) as in these cases PYTUNE call is made within PYINIT in Pythia.
# As at this point the parameter setting has already been done by Pythia the value set by PYTUNE will be assumed
# for any parameter that the tune sets explicitly.
#--------------------------------------------------------------------------------------------------------------
# solution:
# set the tune directly by SUBROUTINE PYTUNE call, than any parameters varied on top will have effect 
#--------------------------------------------------------------------------------------------------------------
# demonstration that the solution works:
# Say one wants to study the effect of increasing PARP(82) in Perugia 10, where it has a value: PARP(82)=2.05 
# So increase the value to say PARP(82)=2.2 on top but keep Perugia 10 for all other parameters
# check the parameter values using parameter queries after initialization
# finally generate a test sample of 10 Zmumu Pythia events using PYGIVE for setup 
#______________________________________________________________________________________________________________

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

#-------------------------------------------------------------------------------------------------------------
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
#-------------------------------------------------------------------------------------------------------------
# use only recommended ATLAS parameter settings (i.e. the ones necessary for succsfull && consistent simulation within Athena)
# all other ATLAS defaults are switched off
Pythia.Tune_Name="ATLAS_-1"
# call Perugia 10 in the mode that will allow parameter overwriting (using MSTP(5) or Pythia.Tune_Name="PYTUNE_325" wouldn't)
Pythia.Direct_call_to_pytune=327
# keep ATLAS stable particles definition
Pythia.PygiveCommand += [ "mstj(22)=2" ]
# now set PARP(82) to the desired value
Pythia.PygiveCommand += [ "parp(82)=2.2" ]
# check that PARP(82) has the desired value after initialization
Pythia.Param_Query_AfterInit += [ "PARP(82)=" ]
#-------------------------------------------------------------------------------------------------------------
#______________________________________________________________________________________________________________
