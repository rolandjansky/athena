#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python
# =====================================================================
# Main script to run the NewInDetIterator
# run with ./RunIterator.py
# =====================================================================
import os
import sys

# ===============================
# Basic running options
# ===============================

# Run mode: local or batch
runMode = 'batch'
#runMode        = 'local'
# Batch queue
#QUEUE = '1nh'
QUEUE 	       = 'atlasb1'

# Prefix to identify your jobs
preName        = 'EoverPMaps'

HOME 	       = os.environ['HOME']
# Job Output
#OutputPath     = os.environ['PWD']+"/."
OutputPath = '/afs/cern.ch/user/l/lacuesta/w0/users/lacuesta/EoPMaps_LargePeriod/'
CMTDIR 	       = HOME+"/athena/cmthome/"
# extra AtlasSetup options 
ASetupOptions  = "single"  

# OutputLevel (DEBUG, INFO, VERBOSE)
OutputLevel    = 'DEBUG'


##########################################################
#		Datasets to use
##########################################################

from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = []

Data1 = setupData('Egamma') # Name for the dataset, the output subpaths will have this name
#Data1.setDataType("MC10_Singlemuons") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
Data1.setDataType("Egamma") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
Data1.setRecoTags(["f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f405_m986","f406_m992","f406_m992","f406_m992","f406_m992"])
Data1.setRuns([189598,189600,189602,189610,189639,189655,189660,189692,189693,189719,189751,189774,189781,189813,189822,189836,189845,189875]) # Run number, you can provide a list of runs
Data1.setCPUs([200]) # Number of CPUs per iteration to process this data
Data1.setEvents([-1]) # Number of total events to process
#Data1.setLimitFiles() # Limit the number of files to use (useful when runing in local a subset of data)
Data1.CreateFileList() # Creates the file list
## Data1.setCustomFileList("filelist") # Use your own filelist, WARNING: comment the CreateFileList line
Data1.setGlobalTag("COMCOND-BLKPST-005-05") # Use custom Global Tag, if not given it will use the data default one
Data1.setDetDescrVersion("ATLAS-GEO-18-01-01") # Use custom DetDescrVersion Tag, if not given it will use the data default one
DataToRun.append(Data1) # always add this line to process the defined data
### Duplicate the previous lines to have as many datasets that you want

# Data2 = setupData('Egamma_187552') # Name for the dataset, the output subpaths will have this name
# #Data1.setDataType("MC10_Singlemuons") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
# Data2.setDataType("Egamma") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
# Data2.setRecoTags(["f396_m946"])
# Data2.setRuns([187552]) # Run number, you can provide a list of runs
# Data2.setCPUs([50]) # Number of CPUs per iteration to process this data
# Data2.setEvents([-1]) # Number of total events to process
# #Data1.setLimitFiles() # Limit the number of files to use (useful when runing in local a subset of data)
# Data2.CreateFileList() # Creates the file list
# ## Data1.setCustomFileList("filelist") # Use your own filelist, WARNING: comment the CreateFileList line
# Data2.setGlobalTag("COMCOND-BLKPST-005-05") # Use custom Global Tag, if not given it will use the data default one
# Data2.setDetDescrVersion("ATLAS-GEO-18-01-01") # Use custom DetDescrVersion Tag, if not given it will use the data default one
# DataToRun.append(Data2) # always add this line to process the defined data
# ### Duplicate the previous lines to have as many datasets that you want

for data in DataToRun:
	data.Print()

# Alignment poolFile to be read on the first iteration, if empty, constants from database will be read
inputAlignmentPoolFile = ""

# split files to subjobs to have exactly equal number of events (True)
# or just approximately based on the size of the input files (False)
doDetailedSplitting = False

GoodRunList = "/afs/cern.ch/user/a/atlasdqm/www/grlgen/CombinedPerf/Egamma/Eg_standard_v5/data11_7TeV.periodAllYear_DetStatus-v32-pro09_CoolRunQuery-00-04-00_Eg_standard.xml"
os.system("get_files -jo InDetAlignExample/NewInDetIterator_EoPMaps.py >/dev/null")
print os.system("get_files -jo InDetAlignExample/NewInDetIterator_EoPMaps.py")
execfile("NewInDetIterator_EoPMaps.py")
