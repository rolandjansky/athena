#
#################################################################
#    Script for running ATLAS ID alignment jobs on Grid resources
#    Authors: Jike Wang      (jike.wang@cern.ch)
#             Song-Ming Wang (smwang@phys.sinica.edu.tw)
#################################################################

import os, string, time, datetime
import sys

# =====================================================================
#  Run Options
# =====================================================================

# Number of Iterations and events
# ===============================
FirstIteration = 0
#Iterations = 11
Iterations = 5

global GridOptions
GridOptions = {}
GridOptions["userIDnum"           ] = "user"
# use NICK NAME be here! See twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/RunIDAlignmentOnGrid to know more
GridOptions["userIDname"          ] = "jwang"
# Unless you really know how to select a Site by yourself, you would better leave here to be empty
GridOptions["siteName"            ] = "ANALY_BNL"
GridOptions["siteName"            ] = ""
GridOptions["accumulateLibDS"     ] = ""
GridOptions["solveLibDS"          ] = ""
GridOptions["fileList"            ] = ""
# You should make sure the directory GridOptions["TmpWorkDir"] really exist
GridOptions["TmpWorkDir"          ] = "/tmp/jwang"
GridOptions["dbRelease"           ] = "LATEST"
GridOptions["excludedSite"        ] = "ANALY_BNL"
#GridOptions["removeFileList"      ] = "list.txt"
GridOptions["reUseSolveLibDS"     ] = False
GridOptions["reUseAccumulateLibDS"] = False
GridOptions["doNFilesPerJobSplit" ] = True
GridOptions["getMonitoringFiles"  ] = False
# you could chose from "Grid","Local","Batch","Prun"
GridOptions["runSolveMode"        ] = "Prun"
GridOptions["runSolveInTmpDir"    ] = False
GridOptions["debugLevel"          ] = 0
GridOptions["nEventsPerJob"       ] = 100
GridOptions["submitOnlyOneJobset" ] = True
GridOptions['autoRetry'		  ] = True
GridOptions["successRatioCut"     ] = 0.95


# GridSiteName = 'ANALY_TAIWAN'
# GridSiteName = 'ANALY_TW-FTT'
# GridSiteName = 'ANALY_BNL'
# GridSiteName = 'ANALY_BNL_ATLAS_1'
# GridSiteName = 'ANALY_TRIUMF'
# GridSiteName = 'ANALY_QMUL'
# GridSiteName = 'ANALY_AGLT2'
# GridSiteName = 'ANALY_GLASGOW'
# GridSiteName = 'ANALY_DESY-HH'
# GridSiteName = 'ANALY_LAPP'
# GridSiteName = 'ANALY_LPC'
# GridSiteName = 'ANALY_LANCS'
# GridSiteName = 'ANALY_FZU'
# GridSiteName = 'ANALY_MANC'
# GridSiteName = 'ANALY_AUSTRALIA'
# GridSiteName = 'ANALY_NIKHEF-ELPROD'

# athena paths
HOME        = os.environ['HOME']
ATHENAREL   = '16.0.3.6'
TAGS        = 'opt,slc5,AtlasProduction'
QUEUE       = '8nh'
#CMTDIR = HOME+"/cmthome/"
CMTDIR      = "/afs/cern.ch/user/j/jwang/scratch0/IDAlignmentOnGrid/16.0.3.6_Grid/cmthome/"
OutputPath  = "./alignment"
preName     = "JW2"
OutputLevel = 'DEBUG'

# ======================================

global DatasetsOptions 
DatasetsOptions = {}

DatasetsOptions["CompareTagsWithAMI"          ] = True;
DatasetsOptions["doDetailedTagsConfig"        ] = True;
# explicitly fill the name of your interested dataset be here, seperated by comma(s). For example:
#DatasetsOptions["CustomedDatasetsNameList"   ] = ["mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731/", "mc09_7TeV.105001.pythia_minbias.recon.ESD.e517_s787_s767_r1250/"]
DatasetsOptions["CustomedDatasetsNameList"    ] = ["mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731/"]
# You'd better explicitly fill the geometry tag by yourself, otherwise I will retrieve the corresponding geometry tag from AMI for you. But sometimes the retrieve could fail
DatasetsOptions["CustomedDatasetsDetDescr"    ] = ["", "", ""]
DatasetsOptions["CustomedDatasetsDetDescrBoff"] = ["", "", ""]
# You'd better explicitly fill the condition tag by yourself, otherwise I will retrieve the corresponding condition tag from AMI for you. But sometimes the retrieve could fail
DatasetsOptions["CustomedDatasetsGlobalTag"   ] = ["", "", ""]   

DatasetsOptions["CollisionRunList"            ] = ["141749"]
DatasetsOptions["CollisionRecoTag"            ] = ["f196"  ] 
DatasetsOptions["CollisionDetDescr"           ] = ""
DatasetsOptions["CollisionDetDescrBoff"       ] = ""
DatasetsOptions["CollisionGlobalTag"          ] = "" 
DatasetsOptions["CollisionStream"             ] = "MinBias" 

DatasetsOptions["CosBonRunList"               ] = ["141749", "141811"]
DatasetsOptions["CosBonRecoTag"               ] = ["f196"  , "f196"  ]
DatasetsOptions["CosBonDetDescr"              ] = ""
DatasetsOptions["CosBonGlobalTag"             ] = ""
DatasetsOptions["CosBonStream"                ] = "MinBias"

DatasetsOptions["CosBoffRunList"              ] = ["141749", "141811"]
DatasetsOptions["CosBoffRecoTag"              ] = ["f196"  , "f196"  ]
DatasetsOptions["CosBoffDetDescr"             ] = ""
DatasetsOptions["CosBoffGlobalTag"            ] = ""
DatasetsOptions["CosBoffStream"               ] = "MinBias"



from InDetAlignExample.IDAlignGridDatasets import ConfiguredIDAlignDatasets
# here now you can set Customed, 900GeV, 7TeV 
Datasets = ConfiguredIDAlignDatasets("Customed", "Nominal", DatasetsOptions)


# ======================================
# Iteration Options
# ======================================


# BE ATTENTION, here the array CustomedCPUs is not for every iteration, it is for every dataset!!!
# For example, the array in below means the first customed dataset spit to 300CPUs, the second customed dataset spit to 200CPUs, the second customed dataset spit to 300CPUs
GridOptions["CustomedCPUs"  ] = [ 3,   2,  3, 300, 300, 300, 300, 300, 300, 300]

# CPUs to run collision data
GridOptions["ColCPUs"       ] = [ 3,  3, 3, 300, 300, 300, 300, 300, 300, 300]

# CPUs to run cosmics
GridOptions["CosBonCPUs"    ] = [ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0  ]

# CPUs to run cosmics without B Field
GridOptions["CosBoffCPUs"   ] = [ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0  ] 

# number of files per sub-job, for every customed dataset!!! 
GridOptions["CustomedNFPJ"  ] = [ 2,   2,   2 , 2,  2,  5]

# number of files per sub-job when running collision data, for every iteration
GridOptions["ColNFPJ"       ] = [ 5,   5,   5,   5,   5,  10,  10,  10,  10,  10 ]

# number of files to be used, for every customed dataset!!!
GridOptions["CustomedNFiles"] = [ -1,  -1,   -1 , -1,  -1]

# number of files to be used, for every iteration !!!
GridOptions["SkipFiles"]      = [ 3,  7, -1, -1, -1, -1, -1, -1, -1,  -1 , -1,  -1]


# Alignment Options
# =============================
# Global chi^2 alignment
# Alignment method: GX2, LX2
AlignmentMethod = "GX2"

# Alignment poolFile to be read on the first iteration, if empty, constants from database will be read
#inputAlignmentPoolFile = "/afs/cern.ch/user/a/atlidali/w1/users/johnda/TRTAlignmentForDB/TRTAlignmentL2_3DofTRT_L1_BarrelAndEndcap_L2Endcap_InDetCosmics_06/indetalignment.root"
#inputAlignmentPoolFile = "/afs/cern.ch/user/a/atlidali/athena/15.5.2.5/InnerDetector/InDetExample/InDetAlignExample/share/CSCLevel1.root"
inputAlignmentPoolFile = ""


#These options will be written to the jobOptions
doDetailedSplitting = False
extraOptions = {}
extraOptions["doMonitoring"      ] = True
extraOptions["doTrkNtuple"       ] = True
extraOptions["doVtxNtuple"       ] = True
extraOptions["writeEigenMat"]  = True
extraOptions["writeEigenMatTxt"]  = True

GridOptions["writeEigenMat"]  = True     
GridOptions["writeEigenMatTxt"]  = True


GridOptions["doMonitoring"       ] = extraOptions["doMonitoring"]
GridOptions["doTrkNtuple"        ] = extraOptions["doTrkNtuple"]
GridOptions["doVtxNtuple"        ] = extraOptions["doVtxNtuple"]




extraOptions["softModeCut"]       = 0.02 
extraOptions["siAlignmentTag"]    = "InDetAlign_Nominal_RDeltaR_01"
extraOptions["trtAlignmentTag"]   = "TRTAlign_Nominal_RDeltaR_01"
#extraOptions["siAlignmentTag"   ] = "InDet_Cosmic_2009_01"
#extraOptions["siAlignmentTag"   ] = "InDetAlign_Collision_2009_08"
#extraOptions["trtAlignmentTag"  ] = "TRTAlign_Collision_2009_04"
#extraOptions["beamSpotTag"      ] = "IndetBeampos-ES1-UPD2"
extraOptions["ModCut"            ] = 4
extraOptions["doBSConstraint"    ] = False
extraOptions["useOldPreProcessor"] = False
#extraOptions["CosmicGRL"        ] = "CosmicsGRL.xml"
#extraOptions["CollisionGRL"     ] = "ColisionGRL.xml"
extraOptions["useTRT"            ] = False
extraOptions["readTRT"           ] = False
extraOptions["writeTRT"          ] = False
#extraOptions["TRTCalibTextFile" ]  = ""
#extraOptions["TRTCalibTextFile" ]  = "/afs/cern.ch/user/a/atlidali/w0/users/johnda/fall2008ReprocessingFieldOnNewT0s.txt"


useTFile 	    = False
# do the solving? (Set to false only to do quick reconstruction tests)
doAccumulate 	    = True
doSolve 	    = True
doCompareMonitoring = True


# Sub-detector Alignment
AlignPixel                 =  [True,True,True,True,True,True,True,True,True,True]
PixelAlignmentLevel        =  [2,   3,   3,   3,   3,   3,   1,   1,   2,   2   ]
AlignPixelBarrel           =  [True,True,True,True,True,True,True,True,True,True]
PixelAlignmentLevelBarrel  =  [2,   3,   3,   3,   3,   3,   1,   1,   2,   2   ] 
AlignPixelEndcaps          =  [True,True,True,True,True,True,True,True,True,True]
PixelAlignmentLevelEndcaps =  [2,   3,   3,   3,   3,   3,   1,   1,   2,   2   ]   

AlignSCT                   =  [True,True,True,True,True,True,True,True,True,True]
SCTAlignmentLevel          =  [2,   3,   3,   3,   3,   3,   1,   1,   2,   2   ] 
AlignSCTBarrel             =  [True,True,True,True,True,True,True,True,True,True]
SCTAlignmentLevelBarrel    =  [2,   3,   3,   3,   3,   3,   2,   2,   2,   2   ] 
AlignSCTEndcaps            =  [True,True,True,True,True,True,True,True,True,True]
SCTAlignmentLevelEndcaps   =  [2,   3,   3,   3,   3,   3,   2,   2,   2,   2   ] 


AlignTRT           	   =  [False,False,False,False,False,False,False,False,False,False]
TRTAlignmentLevel   	   =  [2,    2,    2,    2,    2,    2,    1,    1,    1,    1    ] 

#AlignInDet                = [False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]

# Error Scaling
# =============================
ErrorScaling        	  =  [False,False,False,False,False,False,False,False,False,False]
#errorScalingTag = "IndetTrkErrorScaling_cscalign00_00"
#errorScalingTag = "IndetTrkErrorScaling_Day1-00"
#errorScalingTag = "/afs/cern.ch/atlas/calib/id/calib_2/users/lacuesta/NewAlignment_From_InDet_Cosmic_2009_01/mycoolEC200.db"


# Alignment Montioring Script to be run, if empty, no monitoring script will be run
#MonitoringScript = "/afs/cern.ch/user/a/atlidali/athena/15.5.2.5/InnerDetector/InDetMonitoring/InDetAlignmentMonitoring/share/InDetMonitoringBon.py"
MonitoringScript = ""

os.system("get_files -jo InDetAlignExample/IteratorGrid.py >/dev/null")
execfile("IteratorGrid.py")
