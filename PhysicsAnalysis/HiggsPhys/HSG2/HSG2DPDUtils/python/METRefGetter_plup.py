# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################################
#
# Reconstruction Getters for MissingET.METRefAlg
#
# Author : S. Resconi
# Date :   March  2009
#
##############################################################################################

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import *      # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback                          # to allow printout of trace back
from AthenaCommon.AlgSequence import AlgSequence
from JetRec.FastJetInterfaceConfig import *
from EventShapeTools.EventEtClusterDensityConfig import * 

from RecExConfig.Configured import Configured # import base class

from AthenaCommon.Include import include
include ("MuonCombinedSelectorTools/METMuonSelectorToolConfig.py")

from CaloClusterCorrection.StandardCellWeightCalib   import getCellWeightTool

from MissingETTools.Weights import *

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault 	   
theCaloNoiseTool=CaloNoiseToolDefault() 		        	   
from AthenaCommon.AppMgr import ToolSvc 		        	   
ToolSvc+=theCaloNoiseTool

import AthenaPython.PyAthena as PyAthena
PyAthena.load_library('egammaEnumsDict')

from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
jvatool = getJetVertexAssociationTool ('AntiKt', 0.4, 'Topo')
ToolSvc += jvatool

#------------------------------------------------------------------------------------------------	            
#  Helper Function to retrieve H1 calibration Tool (similar to JetGetters.py)				            
def getStandardCalibTool(doAtlfastII=False):								            

    # define here name of H1 calib tool
    #finder    = "Cone"
    finder    = "AntiKt"
    mainparam = 0.4
    input     = "Topo"

    #def getCellWeightTool(finder="Cone",mainparam=0.4,input="Topo")
    cellcalibtool = getCellWeightTool(finder,mainparam,input,True)

    return cellcalibtool

#------------------------------------------------------------------------------------------------	            

from MissingET.MissingETConf import METRefinedAlgorithm

class METRefGetter_plup ( Configured ):

    suffix                  = '' # suffix used to differentiate instances of tools and containers
    sequence                = AlgSequence()

    ele_EgammaInputCollectionKey= "ElectronAODCollection" 
    ele_noCut                   = False
    ele_isEM                    = PyAthena.egammaPID.ElectronIDMediumPP
    ele_EtCut                   = 10.0*GeV
    ele_calibType               = "ExclRefCalib"     # "RefCalib"
    
    gamma_EleInputCollectionKey   = "ElectronAODCollection"
    gamma_EgammaInputCollectionKey= "PhotonAODCollection" 
    gamma_EtCut                 = 10.0*GeV
    gamma_noCut                 = False  # True = apply no selection
    gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
    gamma_calibType             = "EmScale"
    gamma_isEMMatch             = PyAthena.egammaPID.ElectronIDMediumPP
    gamma_ElectronEtCutMatch    = 10.0*GeV
    
    tau_doTauTool               = True
    tau_TauInputCollectionKey   = "TauRecContainer"
    tau_calibType               = "ExclRefCalib"
    tau_isTau                   = "BDTMedium_EleBDTMedium_MuonVeto"
    tau_TauSignalConeSize       = 0.3
    tau_TauPtCut                = 20.*GeV
    tau_RemoveResiduals         = True
    tau_MinimumMETContribution  = 0.5
    
    jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
    jet_vxcontainerkey       = "VxPrimaryCandidate"
    jet_UseCustomVxContainer = False
    jet_JetPtCut                = 20.0*GeV
    jet_JetMaxPtCut             = 1000000.0*GeV
    jet_ApplyJetScale           = "No"          #Yes
    jet_UseJetMomentForScale    = True
    jet_JetMomentForScale       = "LCJES"
    jet_ApplyJetJVF             = "No"
    jet_JetMomentForJVF         = "JVF"
    jet_calibType               = "ExclRefCalib" #"LocHad"
    jet_RemoveResiduals         = True
    jet_MinimumMETContribution  = 0.5
    jet_UseWeights              = True
    jet_PileupSuppressionMode   = "JET_JVF"
    jet_JVFCorr1                = "JVFweight"   # JVFcut, JVFweight
    jet_JVFCorr2                = "JVFcut"   # JVFcut, JVFweight
    jet_JVFCut                  = 0.0 

    cellout_UsePrimaryVertex    = False  # True= origin correction
    cellout_PUSuppression_STVF  = True
    cellout_PUSuppression_JA    = True
    cellout_vxcontainerkey      = "VxPrimaryCandidate"
    cellout_calibType           = "Eflow"   #Eflow, Tracks
    cellout_pttrack             = 400.
    cellout_JetInputCollectionKey = jet_JetInputCollectionKey
    cellout_PileupSuppressionMode1 = "TRACK_STVF"  # "TRACK_STVF", "JET_AREA"
    cellout_PileupSuppressionMode2 = "JET_AREA"  # "TRACK_STVF", "JET_AREA"
    
    #Additional CellOut Eflow object with tracks from PV
    cellout_EFlow_PV  = True
    cellout_EFlowMode1 = "EFLOW_PV"  # "TRACK_STVF", "JET_AREA"
 
    photon_doPhotonTool         = True

    cryo_ApplyCorrection        = "Off"
    muon_container              = "StacoMuonCollection"
    muon_algorithm              = "Staco" # or Muid
    muon_isolationAlg           = "dRJet" # or TrkIso

    def suffix(self):
        return self.suffix

    def jet_JetInputCollectionKey(self):
        return self.jet_JetInputCollectionKey

    def jet_JetPtCut(self):
        return self.jet_JetPtCut

    def jet_ApplyJetScale(self):
        return self.jet_ApplyJetScale

    def muon_Container(self):
        return self.muon_container

    def muon_trackMethod(self):
        if self.muon_algorithm == "Staco":
            return "CutVariable"
        if self.muon_algorithm == "Muid":
            return "CombinedPt"
        if self.muon_algorithm == "Muons":
            return "CombinedPt"

    def configure(self):
        mlog = logging.getLogger ('METRefGetter_plup::configure:')
        mlog.info ('entering')

        # new moments for jets
        from JetRec.JetRecFlags import jetFlags
        jetFlags.jetPerformanceJob = True # make sure the latest configuration is on
        from JetMomentTools.JetMomentsConfigHelpers import recommendedAreaAndJVFMoments
        # recompute moments on AntiKt4LCTopoJets. 
        # jetmomAlg = recommendedAreaAndJVFMoments('AntiKt4LCTopoNewJets',oldMomentNames=False, seq=self.sequence)
        jetmomAlg = recommendedAreaAndJVFMoments('AntiKt4TopoEMJets',oldMomentNames=False, seq=self.sequence)
 
 	# now configure the algorithm       
        try:
            from MissingET.MissingETConf import METRefinedAlgorithm
            theMETRefAlg=METRefinedAlgorithm("METRefAlg"+self.suffix)
            theMETRefAlgMu=METRefinedAlgorithm("METRefAlgMu"+self.suffix)

            from RecExConfig.RecFlags import rec
            theMETRefAlg.CompositionMapKey = "MET_RefComposition"+self.suffix

            if rec.readAOD():
                theMETRefAlg.UseCells = False
                theMETRefAlgMu.UseCells = False
            else: 
                theMETRefAlg.UseCells = True
                theMETRefAlgMu.UseCells = True
        except: 					       
            mlog.error("could not import MissingET.METRefAlg")    
            print traceback.format_exc()		       
            return False	
	#------------------------------------------------------------------------------------------------
        # configure the tools: METRefinedEleTool => calibrator Tool for ATLFASTII
		
        try:
            from MissingET.MissingETConf import METRefinedEleTool	     
            theMETRefinedEleTool   = METRefinedEleTool("METRefEle"+self.suffix)
            # refEleTool = theMETRefinedEleTool.getHandle()
            theMETRefinedEleTool.EgammaInputCollectionKey = self.ele_EgammaInputCollectionKey
            theMETRefinedEleTool.noCut                   = self.ele_noCut
            theMETRefinedEleTool.isEM                    = self.ele_isEM
            theMETRefinedEleTool.ElectronEtCut		 = self.ele_EtCut
            theMETRefinedEleTool.CalibType		 = self.ele_calibType # H1 or EmScale or KFact or RefCalib
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
            theMETRefinedEleTool.MissingETOutKey     	 = "MET_RefEle"+self.suffix
            print "******************* key = "+theMETRefinedEleTool.MissingETOutKey
            print "******************* key2 = "+theMETRefinedEleTool.MissingETOutKey
            #-----------------
            from MissingET.MissingETConf import METClusterResolverTool
            theMETEleResolver = METClusterResolverTool("EleResolve"+self.suffix)
            theMETEleResolver.ReferenceClusterContainer = "CaloCalTopoCluster"
            theMETRefinedEleTool.ClusterResolverTool = theMETEleResolver.getFullName()
            
	    #------------------------------------------------------------------------------------
            if self.photon_doPhotonTool:
                theMETRefinedGammaTool = METRefinedEleTool("METRefGamma"+self.suffix)
                theMETRefinedGammaTool.EgammaInputCollectionKey      = self.gamma_EgammaInputCollectionKey
                theMETRefinedGammaTool.EleInputCollectionKey         = self.gamma_EleInputCollectionKey
                theMETRefinedGammaTool.isEM			     = self.gamma_isEM
                theMETRefinedGammaTool.noCut			     = self.gamma_noCut
                theMETRefinedGammaTool.ElectronEtCut	             = self.gamma_EtCut
                theMETRefinedGammaTool.CalibType		     = self.gamma_calibType # H1 or EmScale or KFact or RefCalib
                theMETRefinedGammaTool.isEMMatch                     = self.gamma_isEMMatch
                theMETRefinedGammaTool.ElectronEtCutMatch            = self.gamma_ElectronEtCutMatch
                from CaloRec.CaloCellFlags import jobproperties
                if jobproperties.CaloCellFlags.doFastCaloSim:
                    doAtlfastII=True
                else:		
                    doAtlfastII=False
                cellcalibtool  = getStandardCalibTool(doAtlfastII) 
                calibtool_name = cellcalibtool.name()
                theMETRefinedGammaTool.CalibToolName         = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    
		
                theMETRefinedGammaTool.BackNavigationTo	 = "Cell"    # to "Cell"
                theMETRefinedGammaTool.MissingETOutKey       = "MET_RefGamma"+self.suffix	   	
                #-----------------
                theMETGammaResolver = METClusterResolverTool("GammaResolve"+self.suffix)
                theMETGammaResolver.ReferenceClusterContainer = "CaloCalTopoCluster"
                theMETRefinedGammaTool.ClusterResolverTool = theMETGammaResolver.getFullName()            

        except: 							     
            mlog.error("could not get handle to METRefinedEleTool Quit")      
            print traceback.format_exc()				     
            return False						     
 	
	# add cellcalibtool				  		   
        theMETRefinedEleTool   += cellcalibtool
        theMETRefinedEleTool   += theMETEleResolver
        if self.photon_doPhotonTool:
            theMETRefinedGammaTool += cellcalibtool
            theMETRefinedGammaTool += theMETGammaResolver

	# add the METRefinedEleTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedEleTool.getFullName() ]
        if self.photon_doPhotonTool:
            theMETRefAlg.AlgTools+= [ theMETRefinedGammaTool.getFullName() ]

	# add tools to alg		        			      
        theMETRefAlg += theMETRefinedEleTool     			      
        if self.photon_doPhotonTool:
            theMETRefAlg += theMETRefinedGammaTool
        #theMETRefAlg.OutputLevel = DEBUG

        #------------------------------------------------------------------------------------------------
        # configure tools: METRefinedTauTool => calibrator Tool for ATLFASTII
        if self.tau_doTauTool:
            try:									 
            	from MissingET.MissingETConf import METRefinedTauTool			      
            	theMETRefinedTauTool	= METRefinedTauTool("METRefTau"+self.suffix);

            	theMETRefinedTauTool.TauInputCollectionKey   = self.tau_TauInputCollectionKey # or Tau1P3PContainer
            	theMETRefinedTauTool.isTau		     = self.tau_isTau #"Tight"  	 # Tight, Medium, Loose, Old
            	theMETRefinedTauTool.CalibType  	     = self.tau_calibType	 # H1 or LocHad 	
            	theMETRefinedTauTool.TauSignalConeSize       = self.tau_TauSignalConeSize
            	theMETRefinedTauTool.TauPtCut                = self.tau_TauPtCut
            	theMETRefinedTauTool.RemoveResiduals       = self.tau_RemoveResiduals
            	theMETRefinedTauTool.MinimumMETContribution= self.tau_MinimumMETContribution

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
            	theMETRefinedTauTool.MissingETOutKey	    = "MET_RefTau"+self.suffix				           			 
	    								     
            except:								       
            	mlog.error("could not get handle to METRefinedTauTool Quit")	       
            	print traceback.format_exc()					       
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
            theMETRefinedJetTool    = METRefinedJetTool("METRefJet"+self.suffix);
            if rec.readAOD():
                theMETRefinedJetTool.JetInputCollectionKey = self.jet_JetInputCollectionKey
                theMETRefinedJetTool.VxContainerKey = self.jet_vxcontainerkey
                theMETRefinedJetTool.JVFTool = jvatool
                theMETRefinedJetTool.UseCustomVxContainer = self.jet_UseCustomVxContainer
                theMETRefinedJetTool.JetPtCut              = self.jet_JetPtCut       # cut on jet pt
                #theMETRefinedJetTool.JetMaxPtCut           = self.jet_JetMaxPtCut       # cut on jet pt
                theMETRefinedJetTool.CalibType	           = self.jet_calibType		   # H1 or LocHad
                theMETRefinedJetTool.ApplyJetScale         = self.jet_ApplyJetScale
                theMETRefinedJetTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                theMETRefinedJetTool.JetMomentForScale     = self.jet_JetMomentForScale
                theMETRefinedJetTool.ApplyJetJVF           = self.jet_ApplyJetJVF
                theMETRefinedJetTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                theMETRefinedJetTool.MissingETOutKey       = "MET_RefJet"+self.suffix
                theMETRefinedJetTool.RemoveResiduals       = self.jet_RemoveResiduals
                theMETRefinedJetTool.MinimumMETContribution= self.jet_MinimumMETContribution
                theMETRefinedJetTool.UseWeights            = self.jet_UseWeights
            else:
                theMETRefinedJetTool.JetInputCollectionKey = self.jet_JetInputCollectionKey
                theMETRefinedJetTool.VxContainerKey = self.jet_vxcontainerkey
                theMETRefinedJetTool.JVFTool = jvatool
                theMETRefinedJetTool.UseCustomVxContainer = self.jet_UseCustomVxContainer
                theMETRefinedJetTool.JetPtCut              = self.jet_JetPtCut
                theMETRefinedJetTool.JetMaxPtCut           = self.jet_JetMaxPtCut       # cut on jet pt
                theMETRefinedJetTool.CalibType	           = self.jet_calibType       # H1 or LocHad	
                theMETRefinedJetTool.ApplyJetScale         = self.jet_ApplyJetScale
                theMETRefinedJetTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                theMETRefinedJetTool.JetMomentForScale     = self.jet_JetMomentForScale
                theMETRefinedJetTool.ApplyJetJVF           = self.jet_ApplyJetJVF
                theMETRefinedJetTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                theMETRefinedJetTool.BackNavigationTo      = "Topo"            # to "Topo" or to "Cell"	     
                theMETRefinedJetTool.MissingETOutKey       = "MET_RefJet"+self.suffix
                theMETRefinedJetTool.RemoveResiduals       = self.jet_RemoveResiduals
                theMETRefinedJetTool.MinimumMETContribution= self.jet_MinimumMETContribution
                theMETRefinedJetTool.UseWeights            = self.jet_UseWeights

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
            print traceback.format_exc()			  		   
            return False

        # add cellcalibtool				  		   
        theMETRefinedJetTool += cellcalibtool

	# add METRefinedJetTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedJetTool.getFullName() ]

	# add tools to alg 
        theMETRefAlg += theMETRefinedJetTool
 
        #------------------------------------------------------------------------------------------------
	# configure tools: METRefinedMuonTool
        if not rec.readAOD():
            try:
                from AthenaCommon.DetFlags import DetFlags          
                if DetFlags.detdescr.Muon_on():           
                    try:          
                        from MissingET.MissingETConf import METRefinedMuonTool
                        theMETRefMuonTrackTool     = METRefinedMuonTool("METRefMuonTrack"+self.suffix,
                            MuonContainer          = self.muon_Container(),
                            outKey		   = "MET_RefMuon_Track_"+self.muon_algorithm+self.suffix,
                	    cut1Chi2OverDoF	   = -1000.,	 # or 0.  if Muid
                	    cut2Chi2OverDoF	   = 1000.,	 # or 10. if Muid
                	    CaloMuonCollection = "CaloMuonCollection",
                	    MuTagCollection    = self.muon_Container(),
                	    JetContainerName   = self.jet_JetInputCollectionKey,#theMETRefinedJetTool.JetInputCollectionKey,
                	    Muon_Selected_Type = "Isol",     # Isol = use muon tracks
                	    Muon_Add_Crack     = "On",	     # add muon tracks from cracks
                	    IsolDefinition     = self.muon_isolationAlg,
                	    fillComposition     = False
                        )
                        theMETRefMuonSpectroTool   = METRefinedMuonTool("METRefMuonSpectro"+self.suffix,
                	    MuonContainer      = self.muon_Container(), # or "MuidMuonCollection"
                	    outKey	       = "MET_RefMuon_"+self.muon_algorithm+self.suffix,
                	    cut1Chi2OverDoF    = -1000.,     # or 0.  if Muid
                	    cut2Chi2OverDoF    = 1000.,      # or 10. if Muid
                	    CaloMuonCollection = "CaloMuonCollection",
                	    MuTagCollection    = self.muon_Container(),
                	    JetContainerName   = self.jet_JetInputCollectionKey,#theMETRefinedJetTool.JetInputCollectionKey,
                	    Muon_Selected_Type = "NonIsol",  # NonIsol = use spectro muons
                	    IsolDefinition     = self.muon_isolationAlg,
                	    Muon_Add_Crack     = "Off"
                        )
                        # add theMETRefMuonTool to list of tools
                        theMETRefAlg.AlgTools+= [ theMETRefMuonTrackTool.getFullName() ]
                        theMETRefAlg.AlgTools+= [ theMETRefMuonSpectroTool.getFullName() ]
                        # add tool to alg
                        theMETRefAlg += theMETRefMuonTrackTool
                        theMETRefAlg += theMETRefMuonSpectroTool
                    except:
                        mlog.error("could not get handle to METRefinedMuonTool Quit")
                        print traceback.format_exc()
                        return False
            except: 					       
                mlog.error("could not import DetFlags")    

        #------------------------------------------------------------------------------------------------
        # configure tools: METRefinedClusterTool => calibrator Tool for ATLFASTII
        try:								             
            from MissingET.MissingETConf import METRefinedClusterTool		 
            from MissingET.MissingETConf import METTrackSelectorTool
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += METTrackSelectorTool("METTrackSelTool"+self.suffix)
                
            theMETRefinedClusterTool = METRefinedClusterTool("METRefCluster"+self.suffix);
            theMETRefinedClusterTool.ClusterInputCollectionKey = "CaloCalTopoCluster"						           
            theMETRefinedClusterTool.VxContainerKey = self.cellout_vxcontainerkey
            theMETRefinedClusterTool.CalibType  	       =  self.cellout_calibType      # Eflow, Tracks	
            theMETRefinedClusterTool.UsePrimaryVertex          =  self.cellout_UsePrimaryVertex            
            theMETRefinedClusterTool.MissingETOutKey	       = "MET_CellOut"+self.suffix					           									           
            mytrackselector = METTrackSelectorTool("METTrackSelTool"+self.suffix)						           
            mytrackselector.JetContainerName		   = self.cellout_JetInputCollectionKey 				           
            mytrackselector.MinPtTrack  		   = self.cellout_pttrack						           
            theMETRefinedClusterTool.METTrackSelTool	       = mytrackselector						           
            															           
            # if doFastCaloSim set calibrator tool for ATLFAST2 								           
            from CaloRec.CaloCellFlags import jobproperties									           
            if jobproperties.CaloCellFlags.doFastCaloSim:									           
            	doAtlfastII=True												           
            else:														           
            	doAtlfastII=False												           
            cellcalibtool  = getStandardCalibTool(doAtlfastII); 								           
            calibtool_name = cellcalibtool.name();										           
            theMETRefinedClusterTool.CalibToolName	 = 'H1WeightToolCSC12Generic/'+ calibtool_name  			        									 
        
        except: 						  		   
            mlog.error("could not get handle to METRefinedClusterTool Quit")  	   
            print traceback.format_exc()			  		   
            return False	
	    
        # add cellcalibtool				  		   
        theMETRefinedClusterTool += cellcalibtool
	
	# add METRefinedClusterTool to list of tools 
        theMETRefAlg.AlgTools+= [ theMETRefinedClusterTool.getFullName() ]
	            
	# add tools to alg 
        theMETRefAlg += theMETRefinedClusterTool	
        
       # configure tools:  METRefinedPUCorrectionTool apply various PU suppression methods
	# # ---- 1) CellOut Eflow STVF
        if self.cellout_PUSuppression_STVF:
            try:
            	from MissingET.MissingETConf import METRefinedPUCorrectionTool
 
            	theMETRefinedPUCorrectionTool = METRefinedPUCorrectionTool("METRefPUCorrSTVF"+self.suffix)
            	theMETRefinedPUCorrectionTool.VxContainerKey	    = self.cellout_vxcontainerkey
            	theMETRefinedPUCorrectionTool.PileupSuppressionMode = self.cellout_PileupSuppressionMode1
            	theMETRefinedPUCorrectionTool.MissingETOutKey	    = "MET_CellOut_Eflow_STVF"+self.suffix
            except:
            	mlog.error("could not get handle to METRefinedPUCorrectionTool Quit")	       
            	print traceback.format_exc()					       
            	return False
       
            # add track selector tool							 
            theMETRefinedPUCorrectionTool += mytrackselector				 
	
	    # add METRefinedClusterTool to list of tools				 
            theMETRefAlg.AlgTools+= [ theMETRefinedPUCorrectionTool.getFullName() ]	 
	
	    # add tools to alg 
            theMETRefAlg += theMETRefinedPUCorrectionTool
	
	    # # ---- 2) JVF WEIGHT 
            theMETRefinedPUCorrectionToolJVF = METRefinedPUCorrectionTool("METRefPUCorrJVF"+self.suffix)
            theMETRefinedPUCorrectionToolJVF.PileupSuppressionMode = self.jet_PileupSuppressionMode
            theMETRefinedPUCorrectionToolJVF.JVFCorr		   = self.jet_JVFCorr1
            theMETRefinedPUCorrectionToolJVF.MissingETOutKey	   = "MET_RefJet_JVF"+self.suffix

            # add to list of tools
            theMETRefAlg.AlgTools += [ theMETRefinedPUCorrectionToolJVF.getFullName() ]
        
            # add tool to alg
            theMETRefAlg += theMETRefinedPUCorrectionToolJVF

            # # ---- 3) JVF CUT
            theMETRefinedPUCorrectionToolJVFCUT = METRefinedPUCorrectionTool("METRefPUCorrJVFCUT")
            theMETRefinedPUCorrectionToolJVFCUT.PileupSuppressionMode = self.jet_PileupSuppressionMode
            theMETRefinedPUCorrectionToolJVFCUT.JVFCorr 	      = self.jet_JVFCorr2
            theMETRefinedPUCorrectionToolJVFCUT.JVFCut  	      = self.jet_JVFCut
            theMETRefinedPUCorrectionToolJVFCUT.MissingETOutKey       = "MET_RefJet_JVFCut"+self.suffix
 
            # add to list of tools
            theMETRefAlg.AlgTools += [ theMETRefinedPUCorrectionToolJVFCUT.getFullName() ]
        
            # add tool to alg
            theMETRefAlg += theMETRefinedPUCorrectionToolJVFCUT
	
	#------------------------------------------------------------------------------------------------
        # configure tools:  METRefinedPUCorrectionTool apply various PU suppression methods
	# # ---- 1) CellOut_EflowPV

        if self.cellout_EFlow_PV:
            try:
            	from MissingET.MissingETConf import METRefinedPUCorrectionTool
 
            	theMETRefinedPUCorrectionTool = METRefinedPUCorrectionTool("METRefEflowPV"+self.suffix)
            	theMETRefinedPUCorrectionTool.VxContainerKey	    = self.cellout_vxcontainerkey
            	theMETRefinedPUCorrectionTool.PileupSuppressionMode = self.cellout_EFlowMode1
            	theMETRefinedPUCorrectionTool.MissingETOutKey	    = "MET_CellOut_Eflow_PV"+self.suffix
            except:
            	mlog.error("could not get handle to METRefinedPUCorrectionTool Quit")	       
            	print traceback.format_exc()					       
            	return False
       
            # add track selector tool							 
            theMETRefinedPUCorrectionTool += mytrackselector				 
	
	    # add METRefinedClusterTool to list of tools				 
            theMETRefAlg.AlgTools+= [ theMETRefinedPUCorrectionTool.getFullName() ]	 
	
	    # add tools to alg 
            theMETRefAlg += theMETRefinedPUCorrectionTool
       
	
	###############################################################################
        # SoftTerm JetArea-based pile-up correction with or without rho extrapolation #
        ###############################################################################
        if self.cellout_PUSuppression_JA:
            # a la Simoniello/Loch
            
            #from AthenaCommon.AppMgr import ToolSvc

            ##############################################################
            # Set up FastJet tool for SoftTerm jets for extrapolated rho #
            ############################################################## 
            rhoExtrapFastJetDict = dict(defFastJetInterfaceConfigDict)
            rhoExtrapFastJetDict['Algorithm']	     = "Kt"
            rhoExtrapFastJetDict['JetAreaMethod']    = "ActiveAreaExplicitGhost"
            rhoExtrapFastJetDict['CalculateJetArea'] = True
            rhoExtrapFastJetDict['Radius']	     = 0.4
            rhoExtrapFastJetDict['InclusivePtMin']   = 0.*GeV
            rhoExtrapFastJetDict['GhostRepeats']     = 1
            rhoExtrapSoftJetTool = getFastJetInterfaceConfig("SoftTermJetsRhoExtrap",**rhoExtrapFastJetDict)
            print  "Configured and registered tool for extrapolated rho jets: ", 
            rhoExtrapSoftJetTool.getFullName()

            #################################################################
            # Set up FastJet tool for SoftTerm jets for rho from full event #
            ################################################################# 
            rhoFullFastJetDict = dict(defFastJetInterfaceConfigDict)
            rhoFullFastJetDict['Algorithm']	   = "Kt"
            rhoFullFastJetDict['JetAreaMethod']    = "ActiveAreaExplicitGhost"
            rhoFullFastJetDict['CalculateJetArea'] = True
            rhoFullFastJetDict['Radius']	   = 0.8
            rhoFullFastJetDict['InclusivePtMin']   = 0.*GeV
            rhoFullFastJetDict['GhostRepeats']     = 1
            rhoFullSoftJetTool = getFastJetInterfaceConfig("SoftTermJetsRhoFull",**rhoFullFastJetDict)
            print  "Configured and registered tool for full rho jets: ", rhoFullSoftJetTool.getFullName()

            ####################################################################
            # Set up FastJet tool for filtered jets for extrapolated rho & JVF #
            ####################################################################
            filterExtrapFastJetDict = dict(defFastJetInterfaceConfigDict)
            filterExtrapFastJetDict['Algorithm']	= "Kt"
            filterExtrapFastJetDict['JetAreaMethod']	= "ActiveAreaExplicitGhost"
            filterExtrapFastJetDict['CalculateJetArea'] = True
            filterExtrapFastJetDict['Radius']		= 0.6
            filterExtrapFastJetDict['InclusivePtMin']	= 0.*GeV
            filterExtrapFastJetDict['GhostRepeats']	= 1
            filterExtrapSoftJetTool = getFastJetInterfaceConfig("SoftTermJetsFilteredExtrap",
            							**filterExtrapFastJetDict)
            print  "Configured and registered tool for filter jets with extrapolation: ", 
            filterExtrapSoftJetTool.getFullName()
 	
            #######################################################			   
            # Set up FastJet for filtered jets for full rho & JVF #
            ####################################################### 
            filterFullFastJetDict = dict(defFastJetInterfaceConfigDict)
            filterFullFastJetDict['Algorithm']        = "Kt"
            filterFullFastJetDict['JetAreaMethod']    = "ActiveAreaExplicitGhost"
            filterFullFastJetDict['CalculateJetArea'] = True
            filterFullFastJetDict['Radius']	      = 0.4
            filterFullFastJetDict['InclusivePtMin']   = 0.*GeV
            filterFullFastJetDict['GhostRepeats']     = 1
            filterFullSoftJetTool = getFastJetInterfaceConfig("SoftTermJetsFilteredFull",
            						      **filterFullFastJetDict)
            print  "Configured and registered tool for extrapolated rho jets: ", 
            filterFullSoftJetTool.getFullName()

            ##########################################################################
            # Configure METRefinedPUCorrectionTool for JetArea with extrapolated rho #
            ##########################################################################
        
            useLocalRho = True   # FIXME we need _global_ MET reco flags: METRecoFlags.useLocalRho =
            #	    this way the behaviour of METRefGetter can be controled in
            #	    topJobOptions!
            			 
            # local rho from SoftTerm + extrapolation
            if useLocalRho :	 
            	theMETRefinedPUCorrectionToolJA = METRefinedPUCorrectionTool("METRefPUCorrJetAreaLocal")
            	theMETRefinedPUCorrectionToolJA.VxContainerKey  	= "VxPrimaryCandidate"
            	theMETRefinedPUCorrectionToolJA.PileupSuppressionMode	= "JET_AREA"
            	theMETRefinedPUCorrectionToolJA.MissingETOutKey 	= "MET_CellOut_Eflow_JetArea"+self.suffix
            	theMETRefinedPUCorrectionToolJA.SlidingEtaWindowSize	= 1.6
            	theMETRefinedPUCorrectionToolJA.DataPeriodTag		= "DATA_2012"
            	theMETRefinedPUCorrectionToolJA.FilterScaleFactor	= 1.0
            	theMETRefinedPUCorrectionToolJA.RhoCalculatorMode	= "SoftTermExtrapolated"
            	theMETRefinedPUCorrectionToolJA.HaveSoftTermClusterTool = True   
            	theMETRefinedPUCorrectionToolJA.SoftTermClusterTool	= rhoExtrapSoftJetTool
            	theMETRefinedPUCorrectionToolJA.UseFourMomentumArea	= True
            	    
            # global rho from all clusters (a la Lene)
            else:

            	######################
            	# Use EventShapeTool #
            	######################
            	
           	    
            	rhoConfigDict = dict(defaultJetAreaConfigDict)
            	rhoConfigDict['Algorithm']	       = "kt"
            	rhoConfigDict['Radius'] 	       = 0.4
            	rhoConfigDict['JetAreaMethod']         = "VoronoiArea"
            	rhoConfigDict['VoronoiEffectiveRfact'] = 0.9
            	rhoConfigDict['InclusivePtMin']        = 0.*GeV
            	rhoConfigDict['EtaMin'] 	       = -2.
            	rhoConfigDict['EtaMax'] 	       =  2.
            	rhoConfigDict['UseAreaFourMomentum']   = True
            	eventEtDensity = configureJetAreaDensity("METGlobalDensity",**rhoConfigDict)
            	print "Configured and registered tool ", eventEtDensity.getFullName()

            	theMETRefinedPUCorrectionToolJA = METRefinedPUCorrectionTool("METRefPUCorrJetAreaGlobal")
            	theMETRefinedPUCorrectionToolJA.VxContainerKey  	       = "VxPrimaryCandidate"
            	theMETRefinedPUCorrectionToolJA.PileupSuppressionMode	       = "JET_AREA"
            	theMETRefinedPUCorrectionToolJA.MissingETOutKey 	       = "MET_CellOut_Eflow_JetArea"+self.suffix
            	theMETRefinedPUCorrectionToolJA.RhoCalculatorMode	       = "FullEventCluster"   
            	theMETRefinedPUCorrectionToolJA.HaveSoftTermClusterTool        = True	
            	theMETRefinedPUCorrectionToolJA.SoftTermClusterTool	       = rhoExtrapSoftJetTool
            	theMETRefinedPUCorrectionToolJA.GlobalEventEtDensityCalculator = eventEtDensity       
	
	    # set debug level
            #theMETRefinedPUCorrectionToolJA.OutputLevel = DEBUG

            ####################################################################################
            # Configure METRefinedPUCorrectionTool for JetArea with extrapolated rho & JVF cut #
            ####################################################################################
        

            theMETRefinedPUCorrectionToolJAJVFCUT = METRefinedPUCorrectionTool("METRefPUCorrJetAreaJVFCUT") 
            theMETRefinedPUCorrectionToolJAJVFCUT.VxContainerKey	= "VxPrimaryCandidate"
            theMETRefinedPUCorrectionToolJAJVFCUT.PileupSuppressionMode = "JET_AREA_JVF"
            theMETRefinedPUCorrectionToolJAJVFCUT.MissingETOutKey	= "MET_CellOut_Eflow_JetAreaJVF"+self.suffix
            theMETRefinedPUCorrectionToolJAJVFCUT.RhoCalculatorMode	= "SoftTermExtrapolated"
            theMETRefinedPUCorrectionToolJAJVFCUT.UseFourMomentumArea	= True
            theMETRefinedPUCorrectionToolJAJVFCUT.SlidingEtaWindowSize  = 1.6
            theMETRefinedPUCorrectionToolJAJVFCUT.DataPeriodTag 	= "DATA_2012"
            theMETRefinedPUCorrectionToolJAJVFCUT.FilterScaleFactor	= 1.0
            #
            # Using the same RHO jets as the first correction tool - can use cache, no SoftTermClusterTool
            # needed! If you schedule one here, jet finding and rho calculation will run again!!!
            #
            theMETRefinedPUCorrectionToolJAJVFCUT.HaveSoftTermClusterTool   = False
            theMETRefinedPUCorrectionToolJAJVFCUT.HaveSoftTermReclusterTool = True
            theMETRefinedPUCorrectionToolJAJVFCUT.SoftTermReclusterTool     = filterExtrapSoftJetTool
            #theMETRefinedPUCorrectionToolJAJVFCUT.OutputLevel  	     = DEBUG

            ######################################################################
            # SoftTerm JetArea-based pile-up correction with full  rho & JVF cut #
            ######################################################################

            # a la Testa
            #
            # In the original implementation we need two FastJet tools - one for the rho
            # calculation, and one for the reconstruction of SoftTerm jets. Both tools can
            # be identical. We first configure the rho calculation

            ###################################################################
            # Set up pile-up suppression tool for JetArea with full rho & JVF #
            ###################################################################
        
            theMETRefinedPUCorrectionToolJAJVF = METRefinedPUCorrectionTool("METRefPUCorrJetAreaJVF") 
            theMETRefinedPUCorrectionToolJAJVF.VxContainerKey		 = "VxPrimaryCandidate"
            theMETRefinedPUCorrectionToolJAJVF.PileupSuppressionMode	 = "JET_AREA_JVF"
            theMETRefinedPUCorrectionToolJAJVF.MissingETOutKey  	 = "MET_CellOut_Eflow_JetAreaRhoEta5JVF"+self.suffix
            theMETRefinedPUCorrectionToolJAJVF.RhoCalculatorMode	 = "FullEventEflow"
            theMETRefinedPUCorrectionToolJAJVF.UseFourMomentumArea	 = False
            theMETRefinedPUCorrectionToolJAJVF.EtaMin			 = -5.
            theMETRefinedPUCorrectionToolJAJVF.EtaMax			 =  5.
            theMETRefinedPUCorrectionToolJAJVF.HaveSoftTermClusterTool   = True
            theMETRefinedPUCorrectionToolJAJVF.SoftTermClusterTool	 = rhoFullSoftJetTool
            theMETRefinedPUCorrectionToolJAJVF.HaveSoftTermReclusterTool = True
            theMETRefinedPUCorrectionToolJAJVF.SoftTermReclusterTool	 = filterFullSoftJetTool
            #theMETRefinedPUCorrectionToolJAJVF.OutputLevel		  = DEBUG

            #############################
            # Correction tool  sequence #
            #############################

            # jet area with extrapolated rho (Rosa)
            algName = theMETRefAlg.getFullName()
            mlog.info("Add "+theMETRefinedPUCorrectionToolJA.getFullName()+" to "+algName)
            theMETRefAlg.AlgTools+= [ theMETRefinedPUCorrectionToolJA.getFullName() ]
            theMETRefAlg += theMETRefinedPUCorrectionToolJA

            # jet area with extrapolated rho (Rosa)
            mlog.info("Add "+theMETRefinedPUCorrectionToolJAJVFCUT.getFullName()+" to "+algName)
            theMETRefAlg.AlgTools+= [ theMETRefinedPUCorrectionToolJAJVFCUT.getFullName() ]
            theMETRefAlg += theMETRefinedPUCorrectionToolJAJVFCUT

            # jet area with full rho and JVF cut (Marianna)
            mlog.info("Add "+theMETRefinedPUCorrectionToolJAJVF.getFullName()+" to "+algName)
            theMETRefAlg.AlgTools+= [ theMETRefinedPUCorrectionToolJAJVF.getFullName() ]
            theMETRefAlg += theMETRefinedPUCorrectionToolJAJVF
            	    
 
        
  	    	     
 	#------------------------------------------------------------------------------------------------
        # configure tools: METMuonTool
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.detdescr.Muon_on():	
            try:
                from MissingET.MissingETConf import METMuonTool 	 
                theMETMuonBoyTrackTool = METMuonTool("MuonIsol"+self.suffix,			 
            		MuonContainer	       = self.muon_Container(),
            		outKey  	       ="MET_Muon_Isol_"+self.muon_algorithm+self.suffix,
            		ApplyChi2OverDoFCut    ="Off",
            		cut1Chi2OverDoF        = 0. ,
            		cut2Chi2OverDoF        = 10.,
            		CaloMuonCollection     = "CaloMuonCollection",
            		MuTagCollection        = self.muon_Container(),
            		JetContainerName       = theMETRefinedJetTool.JetInputCollectionKey,
            		Muon_Selected_Type     = "Isol",    # Isol = use muon tracks
            		Add_MuTag              = "On",	    # add muon tracks from cracks
            		IsolDefinition         = self.muon_isolationAlg,
            		TrackMethod            = self.muon_trackMethod()
	    	)
                theMETMuonBoySpectroTool = METMuonTool("MuonNonIsol"+self.suffix,
            		MuonContainer	       = self.muon_Container(),
            		outKey  	       ="MET_Muon_NonIsol_"+self.muon_algorithm+self.suffix,
            		ApplyChi2OverDoFCut    ="Off",  		       
            		cut1Chi2OverDoF        = 0. ,			       
            		cut2Chi2OverDoF        = 10.,	    
            		CaloMuonCollection     = "CaloMuonCollection",
            		MuTagCollection        = self.muon_Container(),
            		JetContainerName       = theMETRefinedJetTool.JetInputCollectionKey,
            		Muon_Selected_Type     = "NonIsol", # NonIsol = use spectro muons
            		IsolDefinition         = self.muon_isolationAlg,
            		TrackMethod            = self.muon_trackMethod()
            	)
                theMETMuonTool    = METMuonTool("Muon"+self.suffix,
	    		MuonContainer	       = self.muon_Container(),
            		outKey  	       ="MET_Muon_"+self.muon_algorithm+self.suffix,
            		ApplyChi2OverDoFCut    ="Off",  		   
            		cut1Chi2OverDoF        = 0.,			   
            		cut2Chi2OverDoF        = 10.,
            		CaloMuonCollection     = "CaloMuonCollection",
            		MuTagCollection        = self.muon_Container(),
            		JetContainerName       = theMETRefinedJetTool.JetInputCollectionKey,
            		Muon_Selected_Type     = "All",     # All = use spectro muons
            		IsolDefinition         = self.muon_isolationAlg,
            		TrackMethod            = self.muon_trackMethod()
	    	)

            except:								 
            	mlog.error("could not get handle to METMuonTool Quit")      
            	print traceback.format_exc()					 
            	return False							 
 	
	    # add the METMuonTool to list of tools 
            theMETRefAlg.AlgTools+= [ theMETMuonBoyTrackTool.getFullName() ]
            theMETRefAlg.AlgTools+= [ theMETMuonBoySpectroTool.getFullName() ] 
            theMETRefAlgMu.AlgTools+= [ theMETMuonTool.getFullName() ]

	    # add tool to alg							  
            theMETRefAlg += theMETMuonBoyTrackTool 	    
            theMETRefAlg += theMETMuonBoySpectroTool		  
            theMETRefAlgMu += theMETMuonTool 			  

        #------------------------------------------------------------------------------------------------
        mlog.info(" now adding to sequence")
        self.sequence += theMETRefAlg
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.detdescr.Muon_on():
            self.sequence += theMETRefAlgMu
        #
        #
        #
        #
        #
        #
        #
        #
        #
        #
        try:
            from MissingET.MissingETConf import METAlg
            theMETAlg=METAlg("METAlg"+self.suffix)
        except:
            mlog.error("could not import MissingET.METAlg")    
            print traceback.format_exc()		       
            return False
	#------------------------------------------------------------------------------------------------
        try:								     
            from MissingET.MissingETConf import METCryoTool	     
	   
            theMETCryoTopoTool = METCryoTool("CryoTopo"+self.suffix);
	    		     
            theMETCryoTopoTool.JetCollectionName  = theMETRefinedJetTool.JetInputCollectionKey
	    # if doFastCaloSim set calibrator tool for ATLFAST2
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doFastCaloSim:
                doAtlfastII=True
            else:
                doAtlfastII=False
		
            cellcalibtool  = getStandardCalibTool(doAtlfastII); 
            calibtool_name = cellcalibtool.name();
            theMETCryoTopoTool.CalibToolName     = 'H1WeightToolCSC12Generic/'+ calibtool_name               	    	    

            theMETCryoTopoTool.outKey            = "MET_Cryo"+self.suffix

            # add cellcalibtool				  		   
            theMETCryoTopoTool += cellcalibtool
	
            # add theMETCryoTool to list of tools 
            theMETAlg.AlgTools+= [ theMETCryoTopoTool.getFullName() ]
 	
            # add tool to alg		        			      
            theMETAlg += theMETCryoTopoTool
        except: 							        
            mlog.error("could not get handle to METCryoTopoTool Quit")	        
            print traceback.format_exc()
            return False

	#------------------------------------------------------------------------------------------------
        # configure tools: METFinal
        try:
            from MissingET.MissingETConf import METFinalTool
            theMETMuonFinal      = METFinalTool("MuFinal"+self.suffix,
                   WhichCalc     = "MuonCalc",           # calculate final muon term
                   outKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,       # output key
		   muonBOYTrack  = "MET_Muon_Isol_"+self.muon_algorithm+self.suffix,  # key for track muons
		   muonBOYSpectro= "MET_Muon_NonIsol_"+self.muon_algorithm+self.suffix, # key for spectro muons
	    )
            theMETRefFinal        = METFinalTool("RefFinal"+self.suffix,	     
	           WhichCalc	  = "RefCalib", 	   # Refined Calib applied 
                   EleRefKey	  = "MET_RefEle"+self.suffix,	  # Specify key for Ref Ele	   
	           GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
	           MuoRefKey	  = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon   
	           TauRefKey	  = "MET_RefTau"+self.suffix,	  # Specify key for Ref Tau	  
                   JetRefKey	  = "MET_RefJet"+self.suffix,	  # Specify key for Ref Jet	  
                   CellOutKey	  = "MET_CellOut"+self.suffix,    # Specify key for Ref CellOut   
                   muonKey	  = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                   cryoTerm	  = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off	 
                   cryoKey	  = "MET_Cryo"+self.suffix,	  # Key for Cryo corr: MET_Cryo, MET_CryoCone
                   outKey	  = "MET_RefFinal"+self.suffix
            )
            if self.cellout_PUSuppression_STVF:
	    
                theMETRefFinal_STVF   = METFinalTool("RefFinal_STVF"+self.suffix,			     
                       WhichCalc      = "RefCalib",  # Refined Calib applied			     
                       EleRefKey      = "MET_RefEle"+self.suffix,     # Specify key for Ref Ele        
	               GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
	               MuoRefKey      = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon	
	               TauRefKey      = "MET_RefTau"+self.suffix,     # Specify key for Ref Tau 			       
                       JetRefKey      = "MET_RefJet_JVFCut"+self.suffix, # Specify key for Ref Jet	     
                       CellOutKey     = "MET_CellOut_Eflow_STVF"+self.suffix,	 # Specify key for Ref CellOut       
                       muonKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                       cryoTerm       = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off    
                       cryoKey        = "MET_Cryo"+self.suffix,       # Key for Cryo corr: MET_Cryo, MET_CryoCone
                       outKey	      = "MET_RefFinal_STVF"+self.suffix
					 
            )	
        except:
            mlog.error("could not get handle to METRefFinal Quit")  	   
            print traceback.format_exc()			  		   
            return False					  		   

	# add METFinalTool to list of tools 
        theMETAlg.AlgTools+= [ theMETMuonFinal.getFullName() ]
        theMETAlg.AlgTools+= [ theMETRefFinal.getFullName() ]
        if self.cellout_PUSuppression_STVF:
            theMETAlg.AlgTools+= [ theMETRefFinal_STVF.getFullName() ]
	
	# add tools to alg 	
        theMETAlg += theMETMuonFinal						 
        theMETAlg += theMETRefFinal
        if self.cellout_PUSuppression_STVF:
            theMETAlg += theMETRefFinal_STVF

	#------------------------------------------------------------------------------------------------
        # add algorithm to the specified Sequence (this should always come at the end)

        mlog.info(" now adding to sequence")
        self.sequence += theMETAlg

        return True


def make_METRefAlg(_suffix=''):
    name = 'METRefAlg' + _suffix
    cls = type (name, (METRefGetter_plup,), {})
    cls.suffix = _suffix
    cls.sequence = AlgSequence()
    cls.ele_EgammaInputCollectionKey= "ElectronAODCollection"
    cls.ele_noCut                   = False
    cls.ele_isEM                    = PyAthena.egammaPID.ElectronIDMediumPP
    cls.ele_EtCut                   = 10.0*GeV
    cls.ele_calibType               = "ExclRefCalib"  #"RefCalib"
    
    cls.gamma_EleInputCollectionKey   = "ElectronAODCollection"
    cls.gamma_EgammaInputCollectionKey= "PhotonAODCollection"
    cls.gamma_EtCut                 = 10.0*GeV
    cls.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
    cls.gamma_calibType             = "EmScale"
    cls.gamma_isEMMatch             = PyAthena.egammaPID.ElectronIDMediumPP
    cls.gamma_ElectronEtCutMatch    = 10.0*GeV


    cls.tau_doTauTool               = True
    cls.tau_TauInputCollectionKey   = "TauRecContainer"
    cls.tau_isTau                   = "BDTMedium_EleBDTMedium_MuonVeto"
    cls.tau_calibType               = "ExclRefCalib"
    cls.tau_TauSignalConeSize       = 0.3
    cls.tau_TauPtCut                = 20.*GeV
    cls.tau_RemoveResiduals         = True
    cls.tau_MinimumMETContribution  = 0.5

    cls.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
    cls.jet_vxcontainerkey      = "VxPrimaryCandidate"
    cls.jet_UseCustomVxContainer = False
    cls.jet_JetPtCut                = 20.0*GeV
    cls.jet_JetMaxPtCut             = 1000000.0*GeV
    cls.jet_ApplyJetScale           = "No"   #"Yes"
    cls.jet_UseJetMomentForScale    = True
    cls.jet_JetMomentForScale       = "LCJES"
    cls.jet_ApplyJetJVF             = "No"
    cls.jet_JetMomentForJVF         = "JVF"
    cls.jet_calibType               = "ExclRefCalib" #"LocHad"
    cls.jet_RemoveResiduals         = True
    cls.jet_MinimumMETContribution  = 0.5
    cls.jet_UseWeights              = True
    cls.jet_PileupSuppressionMode   = "JET_JVF"
    cls.jet_JVFCorr1                = "JVFweight"   
    cls.jet_JVFCorr2                = "JVFcut"   
    cls.jet_JVFCut                  = 0.0 

    cls.cellout_UsePrimaryVertex    = False  # True= origin correction
    cls.cellout_PUSuppression_STVF  = True
    cls.cellout_PUSuppression_JA    = True
    cls.cellout_vxcontainerkey      = "VxPrimaryCandidate"
    cls.cellout_calibType           = "Eflow"
    cls.cellout_pttrack             = 150.
    cls.cellout_JetInputCollectionKey = "AntiKt4LCTopoNewJets"
    cls.cellout_PileupSuppressionMode1 = "TRACK_STVF"  
    cls.cellout_PileupSuppressionMode2 = "JET_AREA"  
    cls.cellout_EFlow_PV  = True
    cls.cellout_EFlowMode1 = "EFLOW_PV"  # "TRACK_STVF", "JET_AREA"

    cls.photon_doPhotonTool         = True

    cls.cryo_ApplyCorrection        = "Off"
    cls.muon_container              = "StacoMuonCollection"
    cls.muon_algorithm              = "Staco"
    cls.muon_isolationAlg           = "dRJet"

    return cls
