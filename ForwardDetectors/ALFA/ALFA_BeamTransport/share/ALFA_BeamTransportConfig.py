import os
base_path = os.getenv('TwissFilesPATH') + '/0090.00m'

from ALFA_BeamTransport.ALFA_BeamTransportConf import ALFA_BeamTransport

myBeamTransport = ALFA_BeamTransport("ALFA_BeamTransport")

myBeamTransport.ConfDir = base_path
myBeamTransport.UseALFA = True # use the ALFA oftics hast to be true in oure case
myBeamTransport.Debug = True # write debug output
myBeamTransport.IP = 1	# which interaction point is used in our case always 1
myBeamTransport.UseAper = False
myBeamTransport.apermb = 0.
myBeamTransport.xcol1 = 999.
myBeamTransport.xcol2 = 999.
myBeamTransport.BeamEnergy = 3500. #set beam energy in GeV
myBeamTransport.RPDistance = 237.398 #RP position
myBeamTransport.absZMagMax = 500. # untill this point the magnets are read has to be bigger then the RP pos
myBeamTransport.EtaCut = 8.
myBeamTransport.XiCut = 0.2
myBeamTransport.FPOutputBeam1 = "FPTrackerBeam1.txt"
myBeamTransport.FPOutputBeam2 = "FPTrackerBeam2.txt"
myBeamTransport.McEventCollectionName = "TruthEvent"

topSeq += myBeamTransport
