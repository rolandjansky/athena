#========================================================================================
# JetTowerBuilder_jobOptions.py, (c) ATLAS Detector software
# Reconstruction/Jet/JetRecTools/share/JetTowerBuilder_jobOptions.py
#
# Build towers from raw cells that have been taken
# out of topo clusters. In this way, we can create
# noise suppressed raw towers to be saved in the AOD
# and thus allows re-calibration and re-do MET.
#
# Authors : 
# \author Peter Loch <loch@physics.arizona.edu>
# \author Ariel Scwartzman <sch@slac.stanford.edu>
# \author David W. Miller <David.Miller@slac.stanford.edu>
#
# Date : 10 June 2008
#========================================================================================
#  Custom configuration for building Towers from Cells used in Clusters
#    * It is assumed that this will be used in conjunction with runJetReco_topOptions.py
#========================================================================================


# load the definition of jet helper functions
from JetRec.JetGetters import *
# load the definition of python JetTools (which are automatically generated from c++ classes):
from JetRec.JetRecConf import *  

#---------------------------------------------------------------------------------
# Step 1 : build a list of Jet tools
#---------------------------------------------------------------------------------

# Set up the JetTowerBuilder 
jetTowerBuilder = JetTowerBuilder('TowersFromClusters')
jetTowerBuilder.Cell2ClusterMapName   = "CaloCell2TopoCluster"  
jetTowerBuilder.TowerContainerName    = "CombinedTower"
jetTowerBuilder.MinimumCellEnergy     = -1000000.0
jetTowerBuilder.MinimumClusterEnergy  = 0.0
jetTowerBuilder.MinimumJetEnergy      = 500.0
jetTowerBuilder.CellEnergySignificance= 6

# Add to the tool list
mytoollist = [jetTowerBuilder]

# build an other tool
sortT = JetSorterTool('JetSorter')
sortT.SortOrder="ByEDown"

# Add to the tool list
mytoollist += [sortT]

# Add to the tool list
mytoollist += [JetSetCalibStateTool("SetCalibState")]

#---------------------------------------------------------------------------------

# Now also build a wide area cone tower jet alg for saving all towers
myOtherToolList = getStandardInputTools('Tower') + getStandardFinderTools('Cone',1000)
myOtherToolList += [sortT]

#---------------------------------------------------------------------------------
# Step 2 : prepare a dictionary of jet alg attributes
#---------------------------------------------------------------------------------
# the attributes will be used to build a unique name.
# Be consistant !

#my_att_dict = dict(customJetParams.getterDict) # copy the default dict (it only contains keys) BEFORE 13.X.0
myAttDict      = dict(jet_attribute_dict)         # copy the default dict (it only contains keys) AFTER 13.X.0
myOtherAttDict = dict(jet_attribute_dict) 

# Fill the attribute dictionary
myAttDict['_finderType']   = 'TowersFromClusters'
myAttDict['_finderSuff']   = ''
myAttDict['_finderParams'] = [6]
myAttDict['_inputType']    = ''

myOtherAttDict['_finderType']   = 'Tower'
myOtherAttDict['_finderSuff']   = 'WideAreaCone'
myOtherAttDict['_finderParams'] = [1000]
myOtherAttDict['_inputType']    = 'Tower'

#---------------------------------------------------------------------------------
# Step 3 call the builder with the attribute dict and the tool list as parameter.
#---------------------------------------------------------------------------------
jetTowerB = make_customJetGetter(myAttDict, mytoollist)
jetTowerB.OutputLevel = DEBUG

wideAreaCone = make_customJetGetter(myOtherAttDict, myOtherToolList)
wideAreaCone.OutputLevel = DEBUG

#---------------------------------------------------------------------------------
# Scheduling CBNTAA algorithms
#---------------------------------------------------------------------------------
# define a helper function
# def schedule_CBNTJet(JetDetailedCBNT = False): 
# 
#   if JetDetailedCBNT:
#   
#     #CBNTAthenaAware = True
#     include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
#     include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
#     
#     theApp.Dlls += ["JetRec"]
#     CBNTAthenaAware = True
#     include( "JetRec/Cone7TowerJet_CBNT_jobOptions.py" )
#     include( "JetRec/Cone4TowerJet_CBNT_jobOptions.py" )
#     include( "JetRec/Cone7TopoJet_CBNT_jobOptions.py" )
#     include( "JetRec/Cone4TopoJet_CBNT_jobOptions.py" )
#     include( "JetRec/Cone7TruthJet_CBNT_jobOptions.py" )
#     include( "JetRec/Cone4TruthJet_CBNT_jobOptions.py" )
#   
#   include( "JetRec/JetRec_CBNT_jobOptions.py")
#   
#   from JetRec.JetRecConf import CBNTAA_Jet
#   from JetRec.JetRecConf import CBNTAA_JetTool
#   from JetRec.JetRecConf import CBNTAA_JetCompositionTool   
#   
#   CBNT_Jet = CBNTAA_Jet("CBNT_"+jetColl)  
#   CBNT_Jet.InputJetCollection = jetColl  
#   CBNT_Jet.BlockTag = jetColl  
#   CBNT_Jet.ToolNames = ["CBNTAA_JetTool"]        
#   
#   if JetDetailedCBNT:    
#     JetCompTool = CBNTAA_JetCompositionTool("JetCompTool")    
#     CBNT_Jet.Tools += [JetCompTool]  
#     
#   topSequence = AlgSequence()  
#   topSequence.CBNT_AthenaAware  += CBNT_Jet





