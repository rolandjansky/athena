#______________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch 
# generate 10 Zmumu Pythia events using PYGIVE for setup + different Pythia (ATLAS/PYTUNE) tunes 
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
#-------------------------------------------------------------------------------------------------------------
#1. setup Z production : 
# old way (remains supported)
#---------------------------------------------------------------------------------
#using PythiaCommand
#Pythia.PythiaCommand += [ "pysubs msel 0",         # Users decay choice.
#                         "pydat1 parj 90 20000",   # Turn off FSR.
#                         "pydat3 mdcy 15 1 0",     # Turn off tau decays.
#                         "pysubs msub 1 1",        # Create Z bosons.
#                         "pysubs ckin 1 60.0",     # Lower invariant mass.
#                         "pydat3 mdme 174 1 0",
#                         "pydat3 mdme 175 1 0",
#                         "pydat3 mdme 176 1 0",
#                         "pydat3 mdme 177 1 0",
#                         "pydat3 mdme 178 1 0",
#                         "pydat3 mdme 179 1 0",
#                         "pydat3 mdme 182 1 0",    # Switch for Z->ee.
#                         "pydat3 mdme 183 1 0",
#                         "pydat3 mdme 184 1 1",    # Switch for Z->mumu.
#                         "pydat3 mdme 185 1 0",
#                         "pydat3 mdme 186 1 0",    # Switch for Z->tautau.
#                         "pydat3 mdme 187 1 0"]
#---------------------------------------------------------------------------------
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
#-------------------------------------------------------------------------------------------------------------

#new tunes schema:
#-------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------
#Pythia property Tune_Name of type std::string, syntax:  "TUNETYPE_TUNEINDEX"
# see setPythiaTune.cxx for implemented TUNETYPE opts, see atlasTune.cxx for available TUNEINDEX opts
#-------------------------------------------------------------------------------------
# ATLAS tunes examples:
#-------------------------------------------------------------------------------------
#uncomment this for MC08 tune 
#Pythia.Tune_Name="ATLAS_20080001"

#uncomment this for MC09 tune
#Pythia.Tune_Name="ATLAS_20090001"

#uncomment this for MC09' tune (CTEQ6L1 rune for MC09 round) 
#Pythia.Tune_Name="ATLAS_20090002"

#uncomment this for MC09c tune (simmilar to MC09, but with lowered CR) 
#Pythia.Tune_Name="ATLAS_20090003"

#uncomment this for AMBT1 tune (2010 tune to ATLAS data) 
Pythia.Tune_Name="ATLAS_20100001"
#-------------------------------------------------------------------------------------

# PYTUNE:
# ATLAS defaults are turned off, only necessary parameters for seemless running within Athena are kept

# call PYTUNE PERUGIA HARD (uses internal Pythia PDF)
#Pythia.Tune_Name="PYTUNE_321"

# call PYTUNE PERUGIA * (uses ext. PDF)
#Pythia.Tune_Name="PYTUNE_325"

# call PYTUNE D6T ; Rick Field's CDF Tune D6T (old shower/MI)
#Pythia.Tune_Name="PYTUNE_109"
#-------------------------------------------------------------------------------------

# some special settings:
# use no ATLAS parameter settings
#Pythia.Tune_Name="ATLAS_0"
# use only recommended ATLAS parameter settings (i.e. the ones necessary for succsfull && consistent simulation within Athena)
#Pythia.Tune_Name="ATLAS_-1"
# use recommended + common (PDF, mass, MI schema etc., but not parameters commonly used for tuning) ATLAS parameter
#Pythia.Tune_Name="ATLAS_-2"


#---------------------------------------
# SETTING THE TUNE USING SWITCHES (AS WAS DONE IN MC09) IS DEPRECATED BUT IN  CASE IT IS NEEDED:
# make sure to use new syntax (Pythia.Tune_Name) or old syntax (below) only
# in case the old and new syntax is mixed WARNINGS will be printed and the generation will take place with ATLAS defaults
#---------------------------------------
# uncomment these and comment-out the Pythia.Tune_Name="ATLAS_20100001" line to set MC08 the old way:
#Pythia.useAtlasPythiaTune09=False
#Pythia.useAtlasPythiaTune08=True
#---------------------------------------

#---------------------------------------
# uncomment these and comment-out the Pythia.Tune_Name="ATLAS_20100001" line to set MC09 the old way:
#Pythia.useAtlasPythiaTune09=True
#Pythia.useAtlasPythiaTune08=False
#---------------------------------------

#---------------------------------------
# uncomment these and comment-out the Pythia.Tune_Name="ATLAS_20100001" line to use no ATLAS parameter settings
#Pythia.useOldTuneSwitches=True 
#Pythia.useNoAtlasPythiaParam=True
#Pythia.useAtlasPythiaRecomm=False
#Pythia.useAtlasPythiaCommon=False
#Pythia.useAtlasPythiaTune09=False
#---------------------------------------
#-------------------------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------------------------

# ... Tauola
#include ( "EvgenJobOptions/MC10_Tauola_Fragment.py" )

# ... Photos
#include ( "EvgenJobOptions/MC10_Photos_Fragment.py" )
#______________________________________________________________________________________________________________
