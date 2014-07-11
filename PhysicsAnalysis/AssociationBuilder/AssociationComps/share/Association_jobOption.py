#===================================================================================
# Name:        jobOptionFragment.py
# Author:      Karsten Koeneke (karsten.koeneke@cernSPAMNOT.ch)
# Date:        March 2010
# Description: This is a small example job option fragment that shows you how to
#              use the AnalysisAssociationUtils; example only... use your imagination :-)
# Usage:       Modify the D2PD_topOptions.py in D2PDMaker/share/ to load this file
#              and then execute those topOptions. The line you should add there is
#              something like this:
#              rec.DPDMakerScripts.append("path/to/your/testarea/PhysicsAnalysis/AnalysisCommon/AnalysisAssociationUtils/share/Association_jobOption.py")
#
#===================================================================================


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units


## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 


# declare "topSequence", the top sequencer which manages all algorithms.
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Get the tool service
from AthenaCommon.AppMgr import ToolSvc


# DeltaR matching
from AnalysisAssociationUtils.AnalysisAssociationUtilsConf import DeltaRAssociationTool
ToolSvc += DeltaRAssociationTool( "ElectronDeltaRAssociationTool",
                                  inputAssociateToCollection = "SpclMC",
                                  deltaRMax                  = 0.1,
                                  userDataSvc                = "UserDataInExampleMatchStream",
                                  userDataPrefix             = "el_",
                                  writeUserData              = True
                                  )


# Get the AssociationAlgorithm and schedule the AssociationTools
from AnalysisAssociationUtils.AnalysisAssociationUtilsConf import AssociationAlgorithm
topSequence += AssociationAlgorithm( "ElectronAssociationAlgorithm",
                                     inputCollection              = "ElectronAODCollection",
                                     associationTools             = [ ToolSvc.ElectronDeltaRAssociationTool ],
                                     outputAssociationCollections = [ "ElectronMCTruthClassifierMatch", "ElectronDeltaRTruthMatch" ],
                                     dumpStoreGate                = True
                                     )


# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AOD_MyMatchTest"
fileName   = D2PDFlags.OutputDirectoryName() + "MyMatchTestStream.pool.root"
ExampleMatchStream = MSMgr.NewPoolStream( streamName, fileName )




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = []
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


# Add the created match containers to the output file
ExampleMatchStream.AddItem( ['INav4MomAssocs#ElectronMCTruthClassifierMatch'] )
ExampleMatchStream.AddItem( ['INav4MomAssocs#ElectronDeltaRTruthMatch'] )




#====================================================================
# UserDataSvc, only really needed/used when UserData is computed...
#====================================================================
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataInExampleMatchStream")
svcMgr.UserDataInExampleMatchStream.OutputStream = ExampleMatchStream.Stream
