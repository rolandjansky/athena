from MuonCalibDbOperations.CalibrationSourceConfig import *
from MuonCalibDbOperations.CalibrationTargetFolderConfig import *
from MuonCalibDbOperations.CalibrationDefaultSourceConfig import *
import inspect
coolFileName = inspect.getfile( inspect.currentframe() )[:-3]  + ".db"
################################################################################
################################################################################
################################################################################
################################################################################


###################################
#set detector description version #
# release default if commented    #
###################################

atlasVersion = "ATLAS-GEO-20-00-01"

###########################################################
# if set to true, gasmon rts will be filled for chambers, #
# where rt is not set by other calibration sources	  #
###########################################################

autoFillGasmon = False

####################################################
# set to true if you are filling conditions for MC #
####################################################

isMC=False

#####################################################
#  password for ATLAS_MUONCALIB_READER@INTR.CERN.CH #
# required for automatic addition of gasmon rts	    #
#####################################################

readerPassword = "REPLACE ME"

#####################################################
# slelect toroid status				    #
# required for automatic addition of gasmon rts	    #
#####################################################

toroidOn = False

#########################################
# time for retrieving gas monitor rt    #
# eicther None, utime or "YYYYMMDD"     #
#########################################

calibrationTime = None

##########################################
# drift time offsets for gas monitor rts #
##########################################

#GasmonDriftTimeOffsetsVsR = [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ]


############################################
# path to the target sqlite-file	   #
# defaults to sciptname			   #
############################################

#coolFileName = "test.db"

##########################################################
# set to true, if you want to insert in an existing file #
##########################################################

appendToExistingFile = False

########################################################
# target IOV                                           #
# -1 in beginning or end: get from calibration source  #
# [0, -2]: Infinite IOV, use for UPD1 updates          #
# [-1, -1]: Get IOV form calibration source	       #
########################################################

targetIov = [0, -2]

#################################################
# region selector string.			#
# Only chambers in this region will be inserted #
#################################################

regionToInsert = ""

####################################################
# this tag will be resolved for the target folders #
####################################################

if toroidOn:
	globalConditionsTag = "CURRENT"
else:
	globalConditionsTag = "CURRENT"

#globalConditionsTag = "SOMETHING-ELSE"

###############################################################################
###############################################################################
##									     ##
##     CALIBRATION SOURCES - several sources can be defined                  ##
##									     ##
###############################################################################
###############################################################################

#add calibration source
#calibrationSource=MuonCalib__CalibrationSourceConfig(<site mame>, <head id>)
#<site name> is "MP", "RM" or "UM" for Munich, Rome or Michigan

calibrationSource=MuonCalib__CalibrationSourceConfig("MP", 22)

#calibrationSource.Region = ""
#calibrationSource.StoreRt = True
#calibrationSource.StoreT0 = True
#calibrationSource.T0Offset = 0.0
calibrationSource.TimeSlewingApplied = True
calibrationSource.BFieldCorrectionApplied = True


#calibrationSource2=MuonCalib__CalibrationSourceConfig("RM", 22)

#calibrationSource2.Region = ""
#calibrationSource2.StoreRt = True
#calibrationSource2.StoreT0 = True
#calibrationSource2.T0Offset = 0.0
#calibrationSource2.TimeSlewingApplied = True
#calibrationSource2.BFieldCorrectionApplied = True

###############################################################################
###############################################################################
##									     ##
##     DEFAULT CALIBRATION SOURCES                                           ##
##									     ##
###############################################################################
###############################################################################

#MuonCalib_gCalibrationDefaultSourceConfig.AddDefaultT0("[E??]", 500)

#MuonCalib_gCalibrationDefaultSourceConfig.AddDefaultT0("[B??]", 600)

#MuonCalib_gCalibrationDefaultSourceConfig.AddDefaultRt("", "Rt_BML_2_2.dat")

###############################################################################
###############################################################################
##									     ##
##     CALIBRATION TARGET FOLDERS - several targets can be defined           ##
##									     ##
###############################################################################
###############################################################################

#target folder=MuonCalib__CalibrationTargetConfig(<folder type>)
#folder type is "T0" or "RT"
target_folder_t0=MuonCalib__CalibrationTargetConfig("T0")

#target_folder_t0.Folder = "/MDT/T0"
#target_folder_t0.Tag = None
#target_folder_t0.Compress = False

target_folder_rt=MuonCalib__CalibrationTargetConfig("RT")

#target_folder_rt.Folder = "/MDT/RT"
#target_folder_rt.Tag = None
#target_folder_rt.Compress = False


target_folder_t0_blob=MuonCalib__CalibrationTargetConfig("T0")
target_folder_t0_blob.Folder = "/MDT/T0BLOB"
#target_folder_t0_blob.Tag = None
target_folder_t0_blob.Compress = True

target_folder_rt_blob=MuonCalib__CalibrationTargetConfig("RT")
target_folder_rt_blob.Folder = "/MDT/RTBLOB"
#target_folder_rt_blob.Tag = None
target_folder_rt_blob.Compress = True


#target_folder_t0_onl=MuonCalib__CalibrationTargetConfig("T0")
#target_folder_t0.Folder = "/MDT/Onl/T0"
#target_folder_t0.Tag = None
#target_folder_t0.Compress = False

#target_folder_rt_onl=MuonCalib__CalibrationTargetConfig("RT")
#target_folder_rt.Folder = "/MDT/Onl/RT"
#target_folder_rt.Tag = None
#target_folder_rt.Compress = False


################################################################################
################################################################################
################################################################################
################################################################################


include("MuonCalibDbOperations/copy_to_cool_configurator.py")
