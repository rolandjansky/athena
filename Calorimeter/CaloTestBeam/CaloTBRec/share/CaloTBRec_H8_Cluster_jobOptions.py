#*******************************************************************
#
# JobOption to analyse Combined testbeam data : Calo reconstruction
#   
#  same jobOpt as RecExCommon, with DetFlags.makeRIO.Calo_on()
#*******************************************************************

#abc -------- Now Cluster reconstruction : --------------
#abc

if doCaloCluster:
  include( "CaloRec/CaloCluster_jobOptions.py" )
  #abc but needs to overwrite a few things :
  CmbTowerBldr.TowerBuilderTools=[
      "LArTowerBuilderTool/LArCmbTwrBldr" ]
  LArCmbTwrBldr.IncludedCalos = [ "LAREM" ]

if doEmCluster:
  #sliding window
  include( "LArClusterRec/LArCluster_jobOptions.py" )
if doEMTBCluster:
  #special TB clustering
  include( "LArClusterRec/LArTBCluster_jobOptions.py" )
  include( "CaloClusterCorrection/CaloTB_ClusterCorrection.py" )

if doCaloTopoCluster:
  include( "CaloRec/CaloTopoCluster_jobOptions.py" )
  include( "CaloRec/EMTopoCluster_jobOptions.py" )
  topSequence.CaloTopoCluster.TopoMoments.MomentsNames += [
     "ENG_FRAC_EM" ,
     "ENG_FRAC_MAX" ,
     "ENG_FRAC_CORE" ,
     "SECOND_ENG_DENS" ,
  ]
  #Apply identical settings for calonoisetool (do not know, who is using it) and CaloNoiseToolDefault 
  if not doSim:
     ToolSvc.CaloNoiseToolDefault.UseSymmetry=FALSE
     ToolSvc.LArAutoCorrTotalToolDefault.NMinBias=0
     ToolSvc.CaloNoiseToolDefault.WithOF=FALSE
  if doLArOFC:
     ToolSvc.CaloNoiseToolDefault.WithOF=TRUE
     if not doSim:
         ToolSvc.LArOFCToolDefault.FromDatabase=TRUE  
