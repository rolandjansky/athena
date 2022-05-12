# -*- coding: utf-8 -*-
 
# -*- coding: utf-8 -*-
###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10000
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pythia8_i.Pythia8_iConf import Pythia8_i

job +=Pythia8_i()

include("pythia8.common.py")
include("pythia8.elastic.py")
#include("pythia8.single_diffraction.py")


#include("MC9.105001.pythia_minbias.py")




#sj#from TruthExamples.TruthExamplesConf import DumpMC
#sj#job += DumpMC()

# start my own
#from HepMCAnalysis_i.HepMCAnalysis_iConf import HepMCAnalysis
from ALFA_BeamTransport.ALFA_BeamTransportConf import ALFA_BeamTransport

myBeamTransport = ALFA_BeamTransport("ALFA_BeamTransport")
#myBeamTransport.somthing = what


myBeamTransport.ConfDir = "../../../FPTracker/share" #configuration folder with twiss files
myBeamTransport.UseALFA = True # use the ALFA oftics hast to be true in oure case
myBeamTransport.Debug = True # write debug output
myBeamTransport.IP = 1	# which interaction point is used in our case always 1
#some FPTracker suff don't change
myBeamTransport.UseAper = False
myBeamTransport.apermb = 0.

myBeamTransport.xcol1 = 999.
myBeamTransport.xcol2 = 999.

myBeamTransport.BeamEnergy = 7000. #set beam energy in GeV
myBeamTransport.RPDistance = 237.398 #RP position
myBeamTransport.absZMagMax = 500. # untill this point the magnets are read has to be bigger then the RP pos

#some cut for Production

myBeamTransport.EtaCut = 8.
myBeamTransport.XiCut = 0.2

#the two output files if debug i on
myBeamTransport.FPOutputBeam1 = "Beam1.dat1_new"
myBeamTransport.FPOutputBeam2 = "Beam2.dat1_new"

myBeamTransport.McEventCollectionName = "GEN_EVENT"

job += myBeamTransport




from ALFA_BeamAnalysis.ALFA_BeamAnalysisConf import ALFA_BeamAnalysisOtherProcess

myALFA_BeamAnalysisOtherProcess = ALFA_BeamAnalysisOtherProcess("ALFA_BeamAnalysisOtherProcess")

myALFA_BeamAnalysisOtherProcess.OutputFile="ALFA_BeamAnalysisOtherProcess.root"
myALFA_BeamAnalysisOtherProcess.FileOutput=True
myALFA_BeamAnalysisOtherProcess.BeamEnergy=7000.
myALFA_BeamAnalysisOtherProcess.Debug=True

# these cuts need to be the same as in ALFA_BeamTransport
myALFA_BeamAnalysisOtherProcess.EtaCut = 8.
myALFA_BeamAnalysisOtherProcess.XiCut = 0.2

job += myALFA_BeamAnalysisOtherProcess


#from ALFA_BeamAnalysis.ALFA_BeamAnalysisConf import ALFA_BeamAnalysis

#myALFA_BeamAnalysis = ALFA_BeamAnalysis("ALFA_BeamAnalysis")

#myALFA_BeamAnalysis.OutputFile="ALFA_BeamAnalysis.root"
#myALFA_BeamAnalysis.FileOutput=True
#myALFA_BeamAnalysis.BeamEnergy=7000.
#myALFA_BeamAnalysis.Debug=True

#job += myALFA_BeamAnalysis





#from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

#myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = 'Pythia8_Process_elastic.root')
#myHepMCAnalysis.JetFinder=False
#myHepMCAnalysis.UserAnalysis=True




#job += myHepMCAnalysis

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream( "Stream1" )
#Stream1.OutputFile = "elastic.pool.root"
#Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]





# end my own

#
# End of job options file
#
###############################################################
