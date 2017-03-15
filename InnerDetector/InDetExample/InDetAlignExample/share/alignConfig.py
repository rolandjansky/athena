import os, sys
from InDetAlignExample.NewInDet_SetupData import setupData

class alignConfigBase:
        def __init__(self, firstIter, iterations, strategies ):
                
                # dry run
                self.is_dryrun         = False
                
                #===============================
                # Number of Iterations
                #===============================
                self.FirstIteration    = firstIter
                self.Iterations        = iterations
                
                self.doTruncate        = False
                self.TruncateThreshold = 0.0
                
                #===============================
                # Basic running options
                #===============================
                
                # Run mode: local or batch
                self.runMode           = 'batch'
                
                # Batch queue: suggestions are 8nh, 1nd, atlasb1
                self.QUEUE             = '8nh'
                
                # Prefix to identify your jobs
                self.preName           = 'preName'
                
                #suffix to the folder name
                self.folderSuffix      = "suffix"
                
                self.HOME 	       = os.environ['HOME']
                
                # Alignment Output
                self.OutputPath        = os.environ['PWD']
                self.CMTDIR 	       = self.HOME+"/athena/cmthome/"
                
                # extra AtlasSetup options : "single" or "nightlies"
                self.ASetupOptions     = "single"
                
                # OutputLevel (DEBUG, INFO, VERBOSE)
                self.OutputLevel       = 'INFO'
                
                # list of extra options for NewInDetAlignAlgSetup
                self.extraOptions      = {}

	        ##########################################################
                #               Option Initialisation                    #
                ##########################################################
                options = self.optParsing()
                
                self.inputList            = options.inputList
                self.inputDir             = options.inputDir
                self.eventType            = options.eventType
                self.ConstantsFile        = options.inputConstantsFile
                self.BowingDb             = options.inputBowingDb
                self.nEventsMax           = options.nEvents
                self.preIBLgeometry       = options.preIBLgeometry
                self.isData               = options.isData
                self.ClusteringAlgo       = options.ClusteringAlgo
                self.inputErrorScalingTag = options.errorScalingTag
                self.nCpus                = options.nCpus
                self.userPtMin            = options.userPtMin
                self.userPrefix           = options.prefix
                self.userSuffix           = options.suffix
                
                
                #===============================================================
                #prefix for the job's name
                if (len(self.userPrefix) > 0):
                        self.preName = self.userPrefix
                        
                if (len(self.userSuffix) > 0):
                        self.folderSuffix = "_"+self.userSuffix

                        
                #===============================================================
                # split the possible collections
                self.inputList            = self.inputList.split()
                self.eventType            = self.eventType.split()
                self.nCpus                = self.nCpus.split()
                self.nEventsMax           = self.nEventsMax.split()
                self.userPtMin            = self.userPtMin.split()
                
                
                # Alignment Options
                # =============================
                # Global chi^2 alignment
                # Alignment method: GX2, LX2
                self.AlignmentMethod = "GX2"
                
                # Alignment poolFile to be read on the first iteration, if empty, constants from database will be read

                self.inputAlignmentPoolFile = ""
                self.inputBowingCoolFile    = ""
                self.readConstantsFromPool = False
                self.readBowingFromCool    = False

                if self.ConstantsFile != "":
                        self.inputAlignmentPoolFile = self.ConstantsFile
                        self.readConstantsFromPool = True
                        
                if self.BowingDb != "":
                        self.inputBowingCoolFile = self.BowingDb
                        self.readBowingFromCool  = True



                # split files to subjobs to have exactly equal number of events (True)
                # or just approximately based on the size of the input files (False)
                self.doDetailedSplitting = False

                # Option to use TFiles instead bin files
                # if False it will use the standard bin files
                self.useTFiles = True

                # do the solving? (Set to False only to do quick reconstruction tests)
                self.doSolve = True
                
                
                # Alignment Montioring Script to be run, if empty, no monitoring script will be run
                # A dedicated monitoring script from InDetAlignmentMonitoring package can be used (with full path)
                # Only if extraOptions["doMonitoring"]=True
                self.MonitoringScript = ""
                
                
                #===============================================================
                # Loading strategies
                for strategy in strategies:
                        strategy()
                        print( "alignConfig : strategy {0:30s} is adopted".format( strategy.__name__ ) )
                
                
                #===============================================================
                # Default extra options
                self.extraOptions["dataSource"]                  = "data"
                
                self.extraOptions["BField"]                      = True
                self.extraOptions["BField_AlgSetup"]             = self.extraOptions["BField"]  
                
                self.extraOptions["ModCut"]                      = 1
                self.extraOptions["softModeCut"]                 = 0.
                
        #----------------------------------------------------------------------------------------------------
        def optParsing(self):
                from optparse import OptionParser
                parser = OptionParser()
                parser.add_option("--inputList",          dest="inputList",          help="Input txt File with the list of the files you want to run on",                                                  default="")
                parser.add_option("--inputDir",           dest="inputDir",           help="Directory containing the input files - NOT YET IMPLEMENTED",                                                    default="")
                parser.add_option("--eventType",          dest="eventType",          help="Which type of events: collisions / cosmics",                                                                    default="collisions")
                parser.add_option("--isData",             dest="isData",             help="Set itif you want to run on real data (Default: True)",  action="store_true",                                   default=True)
                parser.add_option("--inputConstantsFile", dest="inputConstantsFile", help="In the case you want to use a specific set of initial constants write here the absolute path to the pool file", default="")
                parser.add_option("--inputBowingDb",      dest="inputBowingDb",      help="In the case you want to use a specific IBL bowing  write here the absolute path to the cool file",              default="")
                parser.add_option("--nEvents",            dest="nEvents",            help="Maximum number of events to use",                                                                               default= -1)
                parser.add_option("--nCpus",              dest="nCpus",              help="Number of CPUs to be used",                                                                                     default=1)
                parser.add_option("--preIBLgeometry",     dest="preIBLgeometry",     help="Set it when you want to run on Run1 geometry (Default: False)", action="store_true",                            default=False)
                parser.add_option("--ClusteringAlgo",     dest="ClusteringAlgo",     help="Which Clustering algorithm to use: NN (Default), Analog, Digital. NOT FULLY WORKING",                           default="NN")
                parser.add_option("--errorScalingTag",    dest="errorScalingTag",    help="Name of the error scaling tag or ES file to use",                                                               default="")
                parser.add_option("--ptmin",              dest="userPtMin",          help="Minimum pt of tracks to enter the align track selection *** in MeV ***",                                        default = 2000)
                parser.add_option("--prefix",             dest="prefix",             help="prefix to the jobs name",                                                                                       default="")
                parser.add_option("--suffix",             dest="suffix",             help="suffix to the IterN folder name --> IterN_suffix",                                                              default="")
                
                (options, sys.argv[1:]) = parser.parse_args(sys.argv[1:])
                
                return options
        
        #----------------------------------------------------------------------------------------------------
        def common_dir(self):
                return ( self.preName + "_common_" + self.folderSuffix )
                
        #----------------------------------------------------------------------------------------------------
        def iter_dir(self, iteration):
                return ( self.preName + "_Iter{0}".format( iteration ) + "_" + self.folderSuffix )

        #----------------------------------------------------------------------------------------------------
        def get_array(self, value):
                return [ value for iteration in range( self.FirstIteration, self.FirstIteration + self.Iterations ) ]
        
        #----------------------------------------------------------------------------------------------------
        def print_setup( self ):
                ##########################################################
                #               Print input parameters                   #
                ##########################################################
                
                print " \n <RunIterator> ----- User input in command line --- "
                if (len(self.folderSuffix)>0):
                        print " <RunIterator> folder suffix = "        , self.folderSuffix
                print " <RunIterator> inputList = "                    , self.inputList
                print " <RunIterator> nCpus = "                        , self.nCpus
                print " <RunIterator> nEventsMax = "                   , self.nEventsMax
                if (len(self.ConstantsFile)>0):
                        print " <RunIterator> ConstantsFile = "        , self.ConstantsFile
                if (len(self.BowingDb)>0):
                        print " <RunIterator> BowingDb = "             , self.BowingDb
                if (len(self.inputErrorScalingTag)>0):
                        print " <RunIterator> inputErrorScalingTag = " , self.inputErrorScalingTag
                if (len(self.userPtMin)>0):
                        print " <RunIterator> userPtMin = "            , self.userPtMin, " MeV"
                print " <RunIterator> eventType = "                    , self.eventType
                print " <RunIterator> iterating on "                   , len(self.eventType),"samples "
        
        #----------------------------------------------------------------------------------------------------
        def generate_dataset(self):
                DataToRun = []
                
                for i_sample in range(len(self.inputList)):
                        Data1 = setupData(self.eventType[i_sample])
                        #Need To be changed this one
                        myDataType = self.DataType + self.eventType[i_sample]
                        #Data1.setDataType("MinBias_Data_15") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
                        Data1.setDataType(myDataType) # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
    
                        # number of CPUS to be used in each iteration to process this data type
                        thisNCpus = int(self.nCpus[0]) 
                        if (len(self.nCpus) >= i_sample+1):
                                thisNCpus = int(self.nCpus[i_sample])
                                Data1.setCPUs( [ thisNCpus for iter in range(0, self.Iterations) ] )
                                
                        # number of events to be processed 
                        thisNEvents = int(self.nEventsMax[0])
                        if (len(self.nEventsMax) >= i_sample+1):
                                thisNEvents = int(self.nEventsMax[i_sample])
                        Data1.setEvents( [ thisNEvents for iter in range(0, self.Iterations) ] )
                        Data1.setLimitFiles(5000) # Limit the number of files to use (useful when runing in local a subset of data)
                        # Data1.setRuns([201280]) # Run number, you can provide a list of runs
                        # Data1.CreateFileList() # Creates the file list
                        
                        # figure out the mimimum pt
                        thisPtMin = self.userPtMin[0]
                        if (len(self.nEventsMax) >= i_sample+1):
                                thisPtMin = self.userPtMin[i_sample]
                                
                        #     
                        if self.isData:
                                Data1.setByteStream(True)
                        else:
                                Data1.setByteStream(False)
                                
                        Data1.setCustomFileList(self.inputList[i_sample])
                        
                        # Use custom Global Tag, if not given it will use the data default one (that is empty ;) )
                        if (self.eventType[i_sample] == "collisions"):
                                if self.isData:
                                        Data1.setGlobalTag("CONDBR2-BLKPA-2016-12")
                                        Data1.setDetDescrVersion("ATLAS-R2-2015-04-00-00")
                                else:
                                        Data1.setGlobalTag("OFLCOND-RUN12-SDR-28")
                                        Data1.setDetDescrVersion("ATLAS-R2-2015-03-01-00")
                                
                        if (self.eventType[i_sample] == "cosmics"):
                                # Data1.setGlobalTag("OFLCOND-MC12-IBL-20-30-50") #Cosmics
                                # Data1.setGlobalTag("CONDBR2-ES1PA-2014-01")
                                if self.isData:
                                        Data1.setGlobalTag("CONDBR2-BLKPA-2015-14")
                                        Data1.setDetDescrVersion("ATLAS-R2-2015-03-01-00")
                                else:
                                        Data1.setGlobalTag("OFLCOND-RUN12-SDR-14")
                                        Data1.setDetDescrVersion("ATLAS-R2-2015-03-01-00")
                                
                                        
                        Data1.setCosmic(False)
                        Data1.setDigitalClustering(False)
                        Data1.setPtMin(thisPtMin)
                        if "cosmics" in self.eventType[i_sample] or "Cosmics" in self.eventType[i_sample]:
                                Data1.setProjectName("data15_cos")
                                Data1.setCosmic(True)
                                Data1.setDigitalClustering(True)
                                
                        DataToRun.append(Data1) # always add this line to process the defined data
                        
                # end of loop on data types
                return DataToRun


        #----------------------------------------------------------------------------------------------------
        def use_dynamicDb(self):
                self.inputDynamicGlobalCoolFile = "/afs/cern.ch/user/m/mdanning/hias/public/13TeV/2016/newDBtests/run/SecondTest.db"
                self.readDynamicGlobalFoldersFromCool = True
                
                self.extraOptions["useDynamicAlignFolders"] = True                
                self.extraOptions["writeDynamicDB"]         = True
                
                self.extraOptions["dynamicL1IDTag"]         = "InDetAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
                self.extraOptions["dynamicL2PIXTag"]        = "InDetAlignL2PIX_R2dynamic_Post_TS1_June2016_d0corrected"
                self.extraOptions["dynamicL2SCTTag"]        = "InDetAlignL2SCT_R2dynamic_Post_TS1_June2016_d0corrected"
                self.extraOptions["dynamicL3SiTag"]         = "InDetAlign_R2dynamic_Post_TS1_June2016_d0corrected"
                self.extraOptions["dynamicL1TRTag"]         = "TRTAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
                self.extraOptions["dynamicL2TRTTag"]        = "TRTAlign_R2dynamic_Post_TS1_June2016_d0corrected" 

        #----------------------------------------------------------------------------------------------------
        def do_L11(self):
                
                #===============================
                # Basic running options
                #===============================
                
                # Prefix to identify your jobs
                self.preName           = 'L11'
                
                
                ##########
                # Pixel
                # are we running Pixel alignment (for barrel and endcaps)
                self.AlignPixel                 = self.get_array( True )
                self.AlignPixelBarrel           = self.get_array( True )
                self.AlignPixelEndcaps          = self.get_array( True )
                
                # alignment level for the full Pixel # see InDetAlignGeometryLevel wiki for more info
                # alignment level for the Pixel barrel. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
                # alignment level for the Pixel endcaps. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
                self.PixelAlignmentLevel        = self.get_array( 11 )
                self.PixelAlignmentLevelBarrel  = self.get_array( -1 )
                self.PixelAlignmentLevelEndcaps = self.get_array( -1 )
                
                ##########
                # SCT
                # are we running SCT alignment (for barrel and endcaps)
                self.AlignSCT                  =  self.get_array( True )
                self.AlignSCTBarrel            =  self.get_array( False )
                self.AlignSCTEndcaps           =  self.get_array( True )
                
                # alignment level for the full SCT # see InDetAlignGeometryLevel wiki for more info
                # alignment level for the SCT barrel. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
                # alignment level for the SCT endcaps. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
                self.SCTAlignmentLevel         =  self.get_array(  1 )
                self.SCTAlignmentLevelBarrel   =  self.get_array( -1 )
                self.SCTAlignmentLevelEndcaps  =  self.get_array(  1 )
                
                ##########
                # TRT
                # are we running TRT alignment # see InDetAlignGeometryLevel wiki for more info
                self.AlignTRT                  =  self.get_array( True )
                self.AlignTRTBarrel            =  self.get_array( True )
                self.AlignTRTEndcaps           =  self.get_array( True )
                # alignment level for the TRT parts
                self.TRTAlignmentLevel         =  self.get_array(  1 )
                self.TRTAlignmentLevelBarrel   =  self.get_array( -1 )
                self.TRTAlignmentLevelEndcaps  =  self.get_array(  1 )
                
                # Error Scaling
                # =============================
                self.ErrorScaling        = self.get_array( True )
                self.errorScalingTag     = self.inputErrorScalingTag
                
                
                #---- IBL stave bowing alignment
                self.extraOptions["applyLBibldistTweak"]  = False
                self.extraOptions["writeIBLDistDB"]       = True
                
                self.extraOptions["useTRT"]               = True
                self.extraOptions["readTRT"]              = True
                self.extraOptions["writeTRT"]             = True
                
                ## PIXEL
                self.extraOptions["pixelAlignBarrelX"]    = True
                self.extraOptions["pixelAlignBarrelY"]    = True
                self.extraOptions["pixelAlignBarrelZ"]    = True
                self.extraOptions["pixelAlignBarrelRotX"] = True
                self.extraOptions["pixelAlignBarrelRotY"] = True
                self.extraOptions["pixelAlignBarrelRotZ"] = True
                self.extraOptions["pixelAlignBarrelBowX"] = True
                
                self.extraOptions["Remove_IBL_Rz"]        = True
                
                ## end-caps
                self.extraOptions["pixelAlignEndcapX"]    = True
                self.extraOptions["pixelAlignEndcapY"]    = True
                self.extraOptions["pixelAlignEndcapZ"]    = True
                self.extraOptions["pixelAlignEndcapRotX"] = True
                self.extraOptions["pixelAlignEndcapRotY"] = True
                self.extraOptions["pixelAlignEndcapRotZ"] = True
                
                ## SCT
                self.extraOptions["sctAlignBarrelX"]      = False
                self.extraOptions["sctAlignBarrelY"]      = False
                self.extraOptions["sctAlignBarrelZ"]      = False
                self.extraOptions["sctAlignBarrelRotX"]   = False
                self.extraOptions["sctAlignBarrelRotY"]   = False
                self.extraOptions["sctAlignBarrelRotZ"]   = False
                
                ## end-caps
                self.extraOptions["sctAlignEndcapX"]      = True
                self.extraOptions["sctAlignEndcapY"]      = True
                self.extraOptions["sctAlignEndcapZ"]      = False
                self.extraOptions["sctAlignEndcapRotX"]   = False
                self.extraOptions["sctAlignEndcapRotY"]   = False
                self.extraOptions["sctAlignEndcapRotZ"]   = True
                
                ## TRT dofs
                self.extraOptions["trtAlignBarrelX"]      = True
                self.extraOptions["trtAlignBarrelY"]      = True
                self.extraOptions["trtAlignBarrelZ"]      = False
                self.extraOptions["trtAlignBarrelRotX"]   = True
                self.extraOptions["trtAlignBarrelRotY"]   = True
                self.extraOptions["trtAlignBarrelRotZ"]   = True
                
                # end-caps
                self.extraOptions["trtAlignEndcapX"]      = True
                self.extraOptions["trtAlignEndcapY"]      = True
                self.extraOptions["trtAlignEndcapZ"]      = False
                self.extraOptions["trtAlignEndcapRotX"]   = False
                self.extraOptions["trtAlignEndcapRotY"]   = False
                self.extraOptions["trtAlignEndcapRotZ"]   = True
                
        #----------------------------------------------------------------------------------------------------
        def do_L16(self):
                # Setup of alignment geometry
                # =============================
                ##########
                # Pixel
                # are we running Pixel alignment (for barrel and endcaps)
                self.AlignPixel                 = self.get_array( True )
                self.AlignPixelBarrel           = self.get_array( True )
                self.AlignPixelEndcaps          = self.get_array( False)
                
                # alignment level for the full Pixel # see InDetAlignGeometryLevel wiki for more info
                # alignment level for the Pixel barrel.  If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
                # alignment level for the Pixel endcaps. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
                self.PixelAlignmentLevel        = self.get_array( 16 )
                self.PixelAlignmentLevelBarrel  = self.get_array( -1 )
                self.PixelAlignmentLevelEndcaps = self.get_array( -1 )

                ##########
                # SCT
                # are we running SCT alignment (for barrel and endcaps)
                self.AlignSCT                  =  self.get_array( False )
                self.AlignSCTBarrel            =  self.get_array( False )
                self.AlignSCTEndcaps           =  self.get_array( False )

                # alignment level for the full SCT # see InDetAlignGeometryLevel wiki for more info
                # alignment level for the SCT barrel.  If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
                # alignment level for the SCT endcaps. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
                self.SCTAlignmentLevel         =  self.get_array(  1 )
                self.SCTAlignmentLevelBarrel   =  self.get_array( -1 )
                self.SCTAlignmentLevelEndcaps  =  self.get_array(  1 )

                ##########
                # TRT
                # are we running TRT alignment # see InDetAlignGeometryLevel wiki for more info
                self.AlignTRT                  =  self.get_array( False )
                self.AlignTRTBarrel            =  self.get_array( False )
                self.AlignTRTEndcaps           =  self.get_array( False )
                
                # alignment level for the TRT parts
                self.TRTAlignmentLevel         =  self.get_array(  1 )
                self.TRTAlignmentLevelBarrel   =  self.get_array( -1 )
                self.TRTAlignmentLevelEndcaps  =  self.get_array(  1 )

                ########
                #### To setup additional detailed settings for different alignment levels
                #### use NewInDetAlignLevels.py
                ########

                # Error Scaling
                # =============================
                self.ErrorScaling        = self.get_array( True )
                self.errorScalingTag = self.inputErrorScalingTag
                


                #--- Pixel and IBL alignment
                self.extraOptions["AlignIBLbutNotPixel"]  = True
                #self.extraOptions["AlignPixelbutNotIBL"] = True

                #---- IBL stave bowing alignment
                self.extraOptions["applyLBibldistTweak"]  = False
                self.extraOptions["writeIBLDistDB"]       = True
                
                
                self.extraOptions["useTRT"]               = True
                self.extraOptions["readTRT"]              = True
                self.extraOptions["writeTRT"]             = True
                
                ## PIXEL
                self.extraOptions["pixelAlignBarrelX"]    = False
                self.extraOptions["pixelAlignBarrelY"]    = False
                self.extraOptions["pixelAlignBarrelZ"]    = False
                self.extraOptions["pixelAlignBarrelRotX"] = False
                self.extraOptions["pixelAlignBarrelRotY"] = False
                self.extraOptions["pixelAlignBarrelRotZ"] = False
                self.extraOptions["pixelAlignBarrelBowX"] = True
                
                ## end-caps
                self.extraOptions["pixelAlignEndcapX"]    = False
                self.extraOptions["pixelAlignEndcapY"]    = False
                self.extraOptions["pixelAlignEndcapZ"]    = False
                self.extraOptions["pixelAlignEndcapRotX"] = False
                self.extraOptions["pixelAlignEndcapRotY"] = False
                self.extraOptions["pixelAlignEndcapRotZ"] = False

                ## SCT
                self.extraOptions["sctAlignBarrelX"]      = False
                self.extraOptions["sctAlignBarrelY"]      = False
                self.extraOptions["sctAlignBarrelZ"]      = False
                self.extraOptions["sctAlignBarrelRotX"]   = False
                self.extraOptions["sctAlignBarrelRotY"]   = False
                self.extraOptions["sctAlignBarrelRotZ"]   = False
                ## end-caps
                self.extraOptions["sctAlignEndcapX"]      = False
                self.extraOptions["sctAlignEndcapY"]      = False
                self.extraOptions["sctAlignEndcapZ"]      = False
                self.extraOptions["sctAlignEndcapRotX"]   = False
                self.extraOptions["sctAlignEndcapRotY"]   = False
                self.extraOptions["sctAlignEndcapRotZ"]   = False
                
                ## TRT dofs
                self.extraOptions["trtAlignBarrelX"]      = False
                self.extraOptions["trtAlignBarrelY"]      = False
                self.extraOptions["trtAlignBarrelZ"]      = False
                self.extraOptions["trtAlignBarrelRotX"]   = False
                self.extraOptions["trtAlignBarrelRotY"]   = False
                self.extraOptions["trtAlignBarrelRotZ"]   = False
                # end-caps
                self.extraOptions["trtAlignEndcapX"]      = False
                self.extraOptions["trtAlignEndcapY"]      = False
                self.extraOptions["trtAlignEndcapZ"]      = False
                self.extraOptions["trtAlignEndcapRotX"]   = False
                self.extraOptions["trtAlignEndcapRotY"]   = False
                self.extraOptions["trtAlignEndcapRotZ"]   = False

                
        #----------------------------------------------------------------------------------------------------
        def do_beamspot_constraint(self):
                self.extraOptions["doBSConstraint"]              = True
                # self.extraOptions["doPVConstraint"]            = False                
                
        #----------------------------------------------------------------------------------------------------
        def do_EoverP_constraint(self):
                #------------------------------------
                # E/p Constraints
                self.extraOptions["CorrectMomentum"]             = True
                self.extraOptions["eoverpmapconstraint"]         = "/afs/cern.ch/work/s/sthenkel/public/forTCP/301912-302872_postTSI/correctionmaps_data_2016_afterIBLTC_30x30.root"
                self.extraOptions["eoverpmapconstraintmap"]      = "LambdaCorrectionVsEtaPhi"#"FinalCorrections" 
        
        #----------------------------------------------------------------------------------------------------
        def do_d0_constraint(self):
                #------------------------------------
                # Zmumu d0 constraints
                self.extraOptions["CorrectMomentum"]             = True
                self.extraOptions["CorrectD0"]                   = True
                self.extraOptions["d0mapconstraint"]             = "/afs/cern.ch/work/s/sthenkel/public/forTCP/301912-302872_postTSI/correctionmaps_data_2016_afterIBLTC_30x30.root"
                self.extraOptions["d0mapconstraintmap"]          = "d0CorrectionVsEtaPhi"
                
        #----------------------------------------------------------------------------------------------------
        def do_z0_constraint(self):
                #------------------------------------
                # Zmumu z0 Constraints
                self.extraOptions["CorrectMomentum"]             = True
                self.extraOptions["CorrectZ0"]                   = True
                self.extraOptions["z0mapconstraint"]             = "/afs/cern.ch/work/s/sthenkel/public/forTCP/301912-302872_postTSI/correctionmaps_data_2016_afterIBLTC_30x30.root"
                self.extraOptions["z0mapconstraintmap"]          = "z0CorrectionVsEtaPhi"
                
        #----------------------------------------------------------------------------------------------------
        def do_truncation(self):
                self.doTruncate        = True
                self.TruncateThreshold = 0.05




