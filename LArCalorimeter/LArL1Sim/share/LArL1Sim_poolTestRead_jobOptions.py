#*************************************************************************************+
# +                                                                                   +
# + Author ........: F. Ledroit                                                       +
# + Institut ......: LPSC Grenoble                                                    +
# + Creation date .: 17/12/2004      Version 0.01                                     +
# + Subject .......: Job Option file to test the reading of pool persistified LArTTL1 +
#                    The test runs the calib algo and produces the calib ntuple       +
#=====================================================================================+


#setup GeoModel
DetDescrVersion = "ATLAS-GEO-02-01-00"
#DetDescrVersion = "ATLAS-GEO-03-00-00"

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
#globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
#globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# for ddcnvsvc
#include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
#theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]

# Define input
#EventSelector = Service( "EventSelector" )
#EventSelector.InputCollections  = [ "TTL1PoolFile2.root" ]
#EventSelector.InputCollections  = [ "/afs/cern.ch/user/d/droussea/scratch0/g50GeV_rel_0_CSC-00-01-00.RDO.pool.root" ]
#EventSelector.InputCollections  = [ "atlasMis_MyOutputFile.digit.RDO.00002.root" ]
ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007003.singlepart_e_Et25.digit.RDO.e322_s484_tid027349/RDO.027349._00001.pool.root.1"]
#ServiceMgr.EventSelector.InputCollections  = [ "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.107204.singlepart_mu4.digit.RDO.e347_s462_d145_tid029243/RDO.029243._00400.pool.root.1" ]
#ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.107204.singlepart_mu4.digit.RDO.e347_s462_d144_tid029220/RDO.029220._00001.pool.root.1"]
#ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.107204.singlepart_mu4.digit.RDO.e347_s462_d147_tid030079/RDO.030079._00001.pool.root.1"]
#ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.107204.singlepart_mu4.digit.RDO.e347_s462_d133_tid027180/RDO.027180._00001.pool.root.1"]



# ............ declare the used top algo.
# --------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArL1Sim" ]
theApp.TopAlg += [ "LArTTL1Calib"]
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Select the appropriate shared library
theApp.Dlls += [ "HbookCnv" ]
# Select HBOOK or ROOT persistency (HBOOK is default)
theApp.HistogramPersistency = "HBOOK"

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.useColors   = TRUE

#--------------------------------------------------------------
# Histogram output file 
#--------------------------------------------------------------
# Specify the appropriate output file type
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
#--------------------------------------------------------------
# Ntuples
#--------------------------------------------------------------
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output     = [ "FILE1 DATAFILE='tuple1.hbook' OPT='NEW'" ]

