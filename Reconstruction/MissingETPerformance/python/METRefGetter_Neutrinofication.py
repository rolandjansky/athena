# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##############################################################################################
#
# Reconstruction Getters for MissingET.METRefAlg
#
# Author : S. Resconi
# Date :   March  2009
#
# June 2009:
# Modified by Giovanni Siragusa (siragusa@uni-mainz.de)
#
# This modified version runs in a Sequence istantiated
# when a Z-->ee candidate is found.
# Calculate MissingET RefFinal after neutrinofication.
#
##############################################################################################

from __future__ import print_function

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import *      # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback                          # to allow printout of trace back

from RecExConfig.Configured import Configured # import base class

from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic

from MissingETTools.Weights import *

#------------------------------------------------------------------------------------------------	            
#  Helper Function to retrieve H1 calibration Tool (similar to JetGetters.py)				            
def getStandardCalibTool(doAtlfastII=False):								            

    # define here name of H1 calib tool 								            
    finder    = 'Cone'  										            
    mainparam = '4'											            
    input     = 'H1Topo'										            
													            
    if(doAtlfastII) : input = input+"AtlfastII" 							            
													            
    tname     =  finder + mainparam + input	# tname = Cone4H1Topo					            
    cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+tname)  					            

    # load DB services  										            
    from IOVDbSvc.CondDB import conddb  								            
    from AthenaCommon.AppMgr import ServiceMgr  							            
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc  							            
    ServiceMgr += IOVDbSvc()										            
    IOVDbSvc = ServiceMgr.IOVDbSvc									            

    # now build database info : check calibdic in Jet/JetRec/python/JetCalibrationConfig.py		            
    input = input.replace('H1','')		# old names convention  				            
    if mainparam == '4': mainparam =''  								            
    calibtype = finder + mainparam + input	# calibtype = 'ConeTopo' correspond to Cone4H1Topo	            
    key       = 'H1Weights' + calibtype 								            
    folder    = '/CALO/H1Weights/'+ key 								            
    key       = folder  			# the same as foldr

    tag = None
    
    from JetRec.JetRecFlags import jetFlags
    DB_tag = jetFlags.JetCalibrationDBTag()

    if DB_tag != "":
                        
        tag       = "CaloH1WeightsConeTopo-" + DB_tag  # check in calibdic the right tag corresponding to calibtype
    else:
        tag = "CaloH1WeightsConeTopo-00-000" # check in calibdic the right tag corresponding to calibtype

    if(doAtlfastII) : tag = "CaloH1WeightsConeTopoAtlfastII-00-000"					            
													            
    cellcalibtool.DBHandleKey = key
    conddb.addFolder("CALO",folder+' <tag>'+tag+'</tag>',
                     className = 'CaloRec::ToolConstants')

    # checking....											            
    # print (' tname is ',tname )
    # print (' input is ', input )
    # print (' tag is ', tag )
    # print (' name is', cellcalibtool.name() )
    # print (' key is ', key )
    # print (' folder is ', folder )
													            
    return cellcalibtool										            
													            
#------------------------------------------------------------------------------------------------	            
class METRefGetter ( Configured ):
    
    def configure(self):
        mlog = logging.getLogger ('METRefGetter::configure:')
        mlog.info ('entering')

        
	
	# now configure the algorithm       
        try:
            from MissingET.MissingETConf import METRefinedAlgorithm
            theMETRefAlg=METRefinedAlgorithm("METRefAlg")
            
            from RecExConfig.RecFlags import rec
            if rec.readAOD():
                theMETRefAlg.UseCells = False
            else: 
                theMETRefAlg.UseCells = True
        except: 					       
            mlog.error("could not import MissingET.METRefAlg")    
            mlog.error(traceback.format_exc())
            return False	
	#------------------------------------------------------------------------------------------------
        # configure the tools: METRefinedEleTool => calibrator Tool for ATLFASTII
		
        try:
            from MissingET.MissingETConf import METRefinedEleTool	     
            theMETRefinedEleTool   = METRefinedEleTool("METRefEle")
            # refEleTool = theMETRefinedEleTool.getHandle()
            if rec.readAOD():
                theMETRefinedEleTool.EleInputCollectionKey	 = "modElectronCollection"#"ElectronAODCollection"
            else:
                theMETRefinedEleTool.EleInputCollectionKey	 = "ElectronCollection"	
            theMETRefinedEleTool.isEM			 = 0
            theMETRefinedEleTool.ElectronEtCut		 = 10.0*GeV
            theMETRefinedEleTool.CalibType		 = "EmScale" # H1 or EmScale or KFact or RefCalib
	    # if doFastCaloSim set calibrator tool for ATLFAST2
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doFastCaloSim:
                doAtlfastII=True
            else:
                doAtlfastII=False
	    
            cellcalibtool  = getStandardCalibTool(doAtlfastII) 
            calibtool_name = cellcalibtool.name()
            theMETRefinedEleTool.CalibToolName           = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
	   	
            theMETRefinedEleTool.BackNavigationTo	 = "Cell"    # to "Cell"
            theMETRefinedEleTool.MissingETOutKey     	 = "MET_RefEle"
            print ("******************* key = "+theMETRefinedEleTool.MissingETOutKey)
            #-----------------
            from MissingET.MissingETConf import METClusterResolverTool
            theMETEleResolver = METClusterResolverTool("EleResolve")
            theMETEleResolver.ReferenceClusterContainer = "CaloCalTopoCluster"
            theMETRefinedEleTool.ClusterResolverTool = theMETEleResolver.getFullName()
            
	    #------------------------------------------------------------------------------------						     
            theMETRefinedGammaTool = METRefinedEleTool("METRefGamma")
            if rec.readAOD():
                theMETRefinedGammaTool.EleInputCollectionKey = "PhotonAODCollection"
            else:
                theMETRefinedGammaTool.EleInputCollectionKey = "PhotonCollection" 		
            theMETRefinedGammaTool.isEM			     = 0
            theMETRefinedGammaTool.ElectronEtCut	     = 10.0*GeV
            theMETRefinedGammaTool.CalibType		     = "EmScale" # H1 or EmScale or KFact or RefCalib
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doFastCaloSim:
                doAtlfastII=True
            else:		
                doAtlfastII=False
            cellcalibtool  = getStandardCalibTool(doAtlfastII) 
            calibtool_name = cellcalibtool.name()
            theMETRefinedGammaTool.CalibToolName         = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
		
            theMETRefinedGammaTool.BackNavigationTo	 = "Cell"    # to "Cell"
            theMETRefinedGammaTool.MissingETOutKey       = "MET_RefGamma"	   	
            #-----------------
            theMETGammaResolver = METClusterResolverTool("GammaResolve")
            theMETGammaResolver.ReferenceClusterContainer = "CaloCalTopoCluster"
            theMETRefinedGammaTool.ClusterResolverTool = theMETGammaResolver.getFullName()            
	    	    									     
        except: 							     
            mlog.error("could not get handle to METRefinedEleTool Quit")      
            mlog.error(traceback.format_exc())
            return False						     
 	
	# add cellcalibtool				  		   
        theMETRefinedEleTool   += cellcalibtool
        theMETRefinedEleTool   += theMETEleResolver
        theMETRefinedGammaTool += cellcalibtool
        theMETRefinedGammaTool += theMETGammaResolver

	# add the METRefinedEleTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedEleTool.getFullName() ]
        theMETRefAlg.AlgTools+= [ theMETRefinedGammaTool.getFullName() ]

	# add tools to alg		        			      
        theMETRefAlg += theMETRefinedEleTool     			      
        theMETRefAlg += theMETRefinedGammaTool
        #theMETRefAlg.OutputLevel = DEBUG


        ## NEUTRINOFICATION
        #------------------------------------------------------------------------------------------------
        # configure the tools: METRefinedEleNeutrinoTool => calibrator Tool for ATLFASTII
		
        try:
            from MissingET.MissingETConf import METRefinedEleTool	     
            theMETRefinedEleNueTool   = METRefinedEleTool("METRefEleNue")
            # refEleTool = theMETRefinedEleTool.getHandle()
            if rec.readAOD():
                theMETRefinedEleNueTool.EleInputCollectionKey	 = "remElectronCollection"#"ElectronAODCollection"
            else:
                theMETRefinedEleNueTool.EleInputCollectionKey	 = "ElectronCollection"	
            theMETRefinedEleNueTool.isEM			 = 0
            theMETRefinedEleNueTool.ElectronEtCut		 = 10.0*GeV
            theMETRefinedEleNueTool.CalibType		 = "EmScale" # H1 or EmScale or KFact or RefCalib
	    # if doFastCaloSim set calibrator tool for ATLFAST2
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doFastCaloSim:
                doAtlfastII=True
            else:
                doAtlfastII=False
	    
            cellcalibtool  = getStandardCalibTool(doAtlfastII) 
            calibtool_name = cellcalibtool.name()
            theMETRefinedEleNueTool.CalibToolName           = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
	   	
            theMETRefinedEleNueTool.BackNavigationTo	 = "Cell"    # to "Cell"
            theMETRefinedEleNueTool.MissingETOutKey     	 = "MET_RefEleNue"
            print ("******************* key = "+theMETRefinedEleNueTool.MissingETOutKey)
            #-----------------
            from MissingET.MissingETConf import METClusterResolverTool
            theMETEleResolver = METClusterResolverTool("EleResolve")
            theMETEleResolver.ReferenceClusterContainer = "CaloCalTopoCluster"
            theMETRefinedEleNueTool.ClusterResolverTool = theMETEleResolver.getFullName()
            
	    #------------------------------------------------------------------------------------						     
        except: 							     
            mlog.error("NEUTRINOFICATION: could not get handle to METRefinedEleTool Quit")      
            mlog.error(traceback.format_exc())
            return False						     
 	
	# add cellcalibtool				  		   
        theMETRefinedEleNueTool   += cellcalibtool
        theMETRefinedEleNueTool   += theMETEleResolver
       
	# add the METRefinedEleTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedEleNueTool.getFullName() ]
       
	# add tools to alg		        			      
        theMETRefAlg += theMETRefinedEleNueTool     			      
        #theMETRefAlg.OutputLevel = DEBUG

        #------------------------------------------------------------------------------------------------
        # configure tools: METRefinedTauTool => calibrator Tool for ATLFASTII
        if not rec.readAOD():
            try:									 
            	from MissingET.MissingETConf import METRefinedTauTool			      
            	theMETRefinedTauTool	= METRefinedTauTool("METRefTau");

            	theMETRefinedTauTool.TauInputCollectionKey   = "TauRecContainer" # or Tau1P3PContainer
            	theMETRefinedTauTool.isTau		     = "Old"  	 # Tight, Medium, Loose
            	theMETRefinedTauTool.CalibType  	     = "H1"		 # H1 or LocHad 	
	    	# if doFastCaloSim set calibrator tool for ATLFAST2
            	from CaloRec.CaloCellFlags import jobproperties
            	if jobproperties.CaloCellFlags.doFastCaloSim:
            	    doAtlfastII=True
            	else:
            	    doAtlfastII=False				
            	
            	cellcalibtool  = getStandardCalibTool(doAtlfastII); 
            	calibtool_name = cellcalibtool.name();
            	theMETRefinedTauTool.CalibToolName	    = 'H1WeightToolCSC12Generic/'+ calibtool_name          		    
	       
            	theMETRefinedTauTool.BackNavigationTo	    = "Topo"			       
            	theMETRefinedTauTool.Cell2ClusterMapKey     = "CaloCell2TopoCluster"	       
            	theMETRefinedTauTool.MissingETOutKey	    = "MET_RefTau"				           			 
	    								     
            except:								       
            	mlog.error("could not get handle to METRefinedTauTool Quit")	       
            	mlog.error(traceback.format_exc())
            	return False							       
	
	    # add cellcalibtool 					    
            theMETRefinedTauTool += cellcalibtool
        
	    # add  METRefinedTauTool to list of tools 
            theMETRefAlg.AlgTools+= [ theMETRefinedTauTool.getFullName() ]
	
	    # add tools to alg  					    	 
            theMETRefAlg += theMETRefinedTauTool	     
        
          
              
        #------------------------------------------------------------------------------------------------
        # configure tools: METRefinedJetTool => calibrator Tool for ATLFASTII
        try:								             
            from MissingET.MissingETConf import METRefinedJetTool		          
            theMETRefinedJetTool    = METRefinedJetTool("METRefJet");
            if rec.readAOD():
                theMETRefinedJetTool.JetInputCollectionKey = "Cone4LCTopoJets" # or "Cone4CalTowerJets","Cone4H1TopoJets"
                theMETRefinedJetTool.JetPtCut              =  5.0*GeV          # cut on jet pt
                theMETRefinedJetTool.CalibType	           = "LocHad"		   # H1 or LocHad
                theMETRefinedJetTool.MissingETOutKey       = "MET_RefJet"
            else:
                theMETRefinedJetTool.JetInputCollectionKey = "Cone4H1TopoJets" # or "Cone4CalTowerJets","Cone4H1TopoJets"
                theMETRefinedJetTool.JetPtCut              =  5.0*GeV          # cut on jet pt
                theMETRefinedJetTool.CalibType	           = "H1"		   # H1 or LocHad	   
                theMETRefinedJetTool.BackNavigationTo      = "Topo"            # to "Topo" or to "Cell"	     
                theMETRefinedJetTool.MissingETOutKey       = "MET_RefJet"				     						
	      
	    # if doFastCaloSim set calibrator tool for ATLFAST2
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doFastCaloSim:
                 doAtlfastII=True
            else:
                doAtlfastII=False
            cellcalibtool  = getStandardCalibTool(doAtlfastII); 
            calibtool_name = cellcalibtool.name();
            theMETRefinedJetTool.CalibToolName     = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
	    							         
        except: 						  		   
            mlog.error("could not get handle to METRefinedJetTool Quit")  	   
            mlog.error(traceback.format_exc())
            return False	
	    
        # add cellcalibtool				  		   
        theMETRefinedJetTool += cellcalibtool
	
	# add METRefinedJetTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedJetTool.getFullName() ]
	            
	# add tools to alg 
        theMETRefAlg += theMETRefinedJetTool		 
        #------------------------------------------------------------------------------------------------
        # configure tools: METRefinedClusterTool => calibrator Tool for ATLFASTII
        try:								             
            from MissingET.MissingETConf import METRefinedClusterTool		 
            # 
            # set it to True for MiniJet Calibration      
            doMiniJets = False
                
            theMETRefinedClusterTool = METRefinedClusterTool("METRefCluster");
            if rec.readAOD():
                theMETRefinedClusterTool.ClusterInputCollectionKey = "CaloCalTopoCluster"
                theMETRefinedClusterTool.CalibType	           = "LocHad"
                theMETRefinedClusterTool.MissingETOutKey           = "MET_CellOut"
            else:
                theMETRefinedClusterTool.ClusterInputCollectionKey = "CaloTopoCluster"  
                theMETRefinedClusterTool.CalibType	           = "H1"		   # H1 or LocHad
                theMETRefinedClusterTool.MissingETOutKey           = "MET_CellOut"				     					  
                # if doFastCaloSim set calibrator tool for ATLFAST2
                from CaloRec.CaloCellFlags import jobproperties
                if jobproperties.CaloCellFlags.doFastCaloSim:
                    doAtlfastII=True
                else:
                    doAtlfastII=False
                cellcalibtool  = getStandardCalibTool(doAtlfastII); 
                calibtool_name = cellcalibtool.name();
                theMETRefinedClusterTool.CalibToolName       = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
            if doMiniJets: 	
                theMETRefinedClusterTool.ClusterInputCollectionKey = "CaloTopoCluster"
                theMETRefinedClusterTool.CalibType                 = "MiniJet"	
                theMETRefinedClusterTool.MissingETOutKey           = "MET_CellOut"		
 	        
                miniJetFinder                                      = getMiniJetTool()
                miniJetCalibrator                                  = getPionWeightTool()
                theMETRefinedClusterTool                          += miniJetFinder
                theMETRefinedClusterTool                          += miniJetCalibrator
	        						         
        
        except: 						  		   
            mlog.error("could not get handle to METRefinedClusterTool Quit")  	   
            mlog.error(traceback.format_exc())
            return False	
	    
        # add cellcalibtool				  		   
        theMETRefinedClusterTool += cellcalibtool
	
	# add METRefinedClusterTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedClusterTool.getFullName() ]
	            
	# add tools to alg 
        theMETRefAlg += theMETRefinedClusterTool		 


        #------------------------------------------------------------------------------------------------
        # add algorithm to topSequence (this should always come at the end)

        # top alg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        mlog.info(" now adding to topSequence")        
        topSequence.ZeeSequence += theMETRefAlg


        return True
