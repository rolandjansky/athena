# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Import the configution-method we want to use (here: Delay and Cali-OFCs)
from LArCalibProcessing.LArCalib_Delay_OFCCaliConfig import LArDelay_OFCCaliCfg

#Import the MainServices (boilerplate)
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

#Import the flag-container that is the arguemnt to the configuration methods
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
addLArCalibFlags(ConfigFlags)

#This allows set flags from the command-line (not strictly required for the AP) 
ConfigFlags.fillFromArgs()

#Now we set the flags as required for this particular job:
#The following flags help finding the input bytestream files: 
ConfigFlags.LArCalib.Input.Dir = "/scratch/wlampl/calib21/HECFCAL_Oct20"
ConfigFlags.LArCalib.Input.Type="calibration_LArElec-Delay"
ConfigFlags.LArCalib.Input.RunNumbers=[404512,]
ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files

#Set the database (sqlite-file) containing the input conditions that
#come typcially from the same calibration campaign 
#(in this case, Pedestal and AutoCorr) 
ConfigFlags.LArCalib.Input.Database="db.sqlite"

#Some configs depend on the sub-calo in question, here HEC
#(sets also the preselection of LArRawCalibDataReadingAlg)
ConfigFlags.LArCalib.Input.SubDet="HEC"

#Configure the Bad-Channel database we are reading 
#(the AP typically uses a snapshot in an sqlite file
ConfigFlags.LArCalib.BadChannelDB="BadChannelSnapshot.db"
ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"

#Output of this job:
#ROOT file:
ConfigFlags.LArCalib.Output.ROOTFile="ofccali.root"

#POOL file:
ConfigFlags.LArCalib.Output.POOLFile="ofccali.pool.root"

#sqlite file (can be the same as the input-sqlite, but slightly different syntax
ConfigFlags.IOVDb.DBConnection="sqlite://;schema=db.sqlite;dbname=CONDBR2"

#The global tag we are working with
ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-00"


#Other potentially useful flags-settings:

#Define the global output Level:
#from AthenaCommon.Constants import * 
#ConfigFlags.Exec.OutputLevel=VERBOSE

#Feed-though preselection for bytestream input:
#ConfigFlags.LArCalib.Preselection.BEC=[1]
#ConfigFlags.LArCalib.Preselection.Side=[0]

#Print the input files we found 
print ("Input files to be processed:")
for f in ConfigFlags.Input.Files:
    print (f)


#Lock the flag-container (required)
ConfigFlags.lock()

#Get the Main services (EventLoopMgr, StoreGate, ... )
cfg=MainServicesCfg(ConfigFlags)

#Merge our own config into it
cfg.merge(LArDelay_OFCCaliCfg(ConfigFlags))


#At this point we can alter the configuration
#(should not be necessary!!!)
#cfg.getEventAlgo("LArCaliWaveBuilder").OutputLevel=VERBOSE

#from AthenaConfiguration.ComponentFactory import CompFactory 
#cfg.addEventAlgo(CompFactory.LArPedestals2Ntuple(ContainerKey = "Pedestal",
#                                                 AddFEBTempInfo = False
#                                             ))

#run the application
print("Start running...")
cfg.run() 
#For jobs with no bytestream-input the last line is
#cfg.run(1)
#to process exactly one 'fake' event as the job does all it's work in stop()
