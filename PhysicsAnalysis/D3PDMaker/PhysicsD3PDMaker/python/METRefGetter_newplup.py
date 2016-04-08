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

class METRefGetter_newplup ( Configured ):

    suffix                  = '' # suffix used to differentiate instances of tools and containers
    sequence                = AlgSequence()

    ele_EgammaInputCollectionKey= "ElectronAODCollection" 
    ele_noCut                   = False
    ele_isEM                    = PyAthena.egammaPID.ElectronIDMediumPP
    ele_EtCut                   = 10.0*GeV
    ele_calibType               = "RefCalib"
    
    gamma_EleInputCollectionKey   = "ElectronAODCollection"
    gamma_EgammaInputCollectionKey= "PhotonAODCollection" 
    gamma_EtCut                 = 10.0*GeV
    gamma_noCut                     = False  # True = apply no selection
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
    
    jet_JetInputCollectionKey   = "AntiKt4LCTopoJets"
    jet_vxcontainerkey       = "VxPrimaryCandidate"
    jet_UseCustomVxContainer = False
    jet_JetPtCut                = 20.0*GeV
    jet_JetMaxPtCut             = 1000000.0*GeV
    jet_ApplyJetScale           = "Yes"
    jet_UseJetMomentForScale    = True
    jet_JetMomentForScale       = "LCJES"
    jet_ApplyJetJVF             = "No"
    jet_JetMomentForJVF         = "JVF"
    jet_calibType               = "LocHad"
    jet_RemoveResiduals         = True
    jet_MinimumMETContribution  = 0.5
    
    jet_RunSoftJetsTool         = True
    jet_SoftJetsInputCollectionKey = jet_JetInputCollectionKey
    jet_SoftJetsCalibType       = "LocHad"
    jet_SoftJetsPtCut           = 5.0*GeV
    jet_SoftJetsMaxPtCut        = 20.0*GeV
    jet_ApplySoftJetsScale      = "No"
    jet_SoftJetsUseJetMomentForScale  = False
    jet_SoftJetsJetMomentForScale     = "LCJES"
    jet_SoftJetsRemoveResiduals       = True
    jet_SoftJetsMinimumMETContribution= 0.5

    cellout_vxcontainerkey       = "VxPrimaryCandidate"
    cellout_calibType            = "Eflow"   #Eflow, Tracks
    cellout_trackType            = "STTrack" #STTrack, STTrackPV, STTrackCone
    cellout_calibType2           = "Tracks"   
    cellout_trackType2           = "STTrack" 
    cellout_calibType3           = "Tracks"   
    cellout_trackType3           = "STTrackPV" 
    
    cellout_pttrack             = 150.
    cellout_JetInputCollectionKey = jet_JetInputCollectionKey
    photon_doPhotonTool         = True

    cryo_ApplyCorrection        = "Off"
    muon_container              = "StacoMuonCollection"
    muon_algorithm              = "Staco" # or Muid
    muon_isolationAlg           = "dRJet" # or TrkIso

    plupSuppCorr                = "Off" #Off, STVF
    celloutCorrection           = "Off" #Off, STVF
    
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
        mlog = logging.getLogger ('METRefGetter_newplup::configure:')
        mlog.info ('entering')

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
                theMETRefinedJetTool.RemoveResiduals       = True
                theMETRefinedJetTool.MinimumMETContribution= 0.5

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
        # configure tools: METSoftJetsTool
        if self.jet_RunSoftJetsTool:
            try:
                from MissingET.MissingETConf import METRefinedJetTool
                theMETSoftJetsTool    = METRefinedJetTool("METSoftJets"+self.suffix);
                if rec.readAOD():
                    theMETSoftJetsTool.JetInputCollectionKey = self.jet_JetInputCollectionKey
                    theMETSoftJetsTool.JVFTool = jvatool
                    theMETSoftJetsTool.UseCustomVxContainer = False
                    theMETSoftJetsTool.JetPtCut              = self.jet_SoftJetsPtCut       # cut on jet pt
                    theMETSoftJetsTool.JetMaxPtCut           = self.jet_SoftJetsMaxPtCut       # cut on jet pt
                    theMETSoftJetsTool.CalibType	     = self.jet_SoftJetsCalibType
                    theMETSoftJetsTool.ApplyJetScale         = self.jet_ApplySoftJetsScale
                    theMETSoftJetsTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                    theMETSoftJetsTool.JetMomentForScale     = self.jet_JetMomentForScale
                    theMETSoftJetsTool.MissingETOutKey       = "MET_SoftJets"+self.suffix
                    theMETSoftJetsTool.RemoveResiduals       = self.jet_SoftJetsRemoveResiduals
                    theMETSoftJetsTool.MinimumMETContribution= self.jet_SoftJetsMinimumMETContribution
                else:
                    theMETSoftJetsTool.JetInputCollectionKey = self.jet_JetInputCollectionKey
                    theMETSoftJetsTool.JVFTool = jvatool
                    theMETSoftJetsTool.UseCustomVxContainer = False
                    theMETSoftJetsTool.JetPtCut              = self.jet_SoftJetsPtCut
                    theMETSoftJetsTool.JetMaxPtCut           = self.jet_SoftJetsMaxPtCut
                    theMETSoftJetsTool.CalibType	     = self.jet_SoftJetsCalibType
                    theMETSoftJetsTool.ApplyJetScale         = self.jet_ApplySoftJetsScale
                    theMETSoftJetsTool.UseJetMomentForScale  = self.jet_UseJetMomentForScale
                    theMETSoftJetsTool.JetMomentForScale     = self.jet_JetMomentForScale
                    theMETSoftJetsTool.BackNavigationTo      = "Topo"            # to "Topo" or to "Cell"	     
                    theMETSoftJetsTool.MissingETOutKey       = "MET_SoftJets"+self.suffix
                    theMETSoftJetsTool.RemoveResiduals       = True
                    theMETSoftJetsTool.MinimumMETContribution= 0.5

                # if doFastCaloSim set calibrator tool for ATLFAST2
                from CaloRec.CaloCellFlags import jobproperties
                if jobproperties.CaloCellFlags.doFastCaloSim:
                    doAtlfastII=True
                else:
                    doAtlfastII=False
                cellcalibtool  = getStandardCalibTool(doAtlfastII); 
                calibtool_name = cellcalibtool.name();
                theMETSoftJetsTool.CalibToolName     = 'H1WeightToolCSC12Generic/'+ calibtool_name
            except:
                mlog.error("could not get handle to METSoftJetsTool Quit")  	   
                print traceback.format_exc()			  		   
                return False

            # add cellcalibtool				  		   
            theMETSoftJetsTool += cellcalibtool

            # add METSoftJetsTool to list of tools 
            theMETRefAlg.AlgTools+= [ theMETSoftJetsTool.getFullName() ]

            # add tools to alg 
            theMETRefAlg += theMETSoftJetsTool

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
            theMETRefinedClusterTool.TrackType  	       =  self.cellout_trackType      # STTrack, STTrackPV,STTrackCone             
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
        #--------------------------------------------------------------------------------
        # configure tools: METRefinedClusterTool (2)  => only for STVF pile-up suppression
        if self.plupSuppCorr == "STVF":
            try:									 
            	from MissingET.MissingETConf import METRefinedClusterTool	     
            	from MissingET.MissingETConf import METTrackSelectorTool
            	from AthenaCommon.AppMgr import ToolSvc
            	ToolSvc += METTrackSelectorTool("METTrackSelTool"+self.suffix)
            	    
            	theMETRefinedClusterTrackTool = METRefinedClusterTool("METRefClusterTrack"+self.suffix);
            	theMETRefinedClusterTrackTool.ClusterInputCollectionKey = "CaloCalTopoCluster"  						   
            	theMETRefinedClusterTrackTool.VxContainerKey = self.cellout_vxcontainerkey
            	theMETRefinedClusterTrackTool.CalibType 	   =  self.cellout_calibType2	  # Eflow, Tracks			       
            	theMETRefinedClusterTrackTool.TrackType 	   =  self.cellout_trackType2	  # STTrack, STTrackPV,STTrackCone	       
            	theMETRefinedClusterTrackTool.MissingETOutKey		   = "MET_CellOut_Track"+self.suffix					        									   
            	mytrackselector = METTrackSelectorTool("METTrackSelTool"+self.suffix)							       
            	mytrackselector.JetContainerName	       = self.cellout_JetInputCollectionKey					       
            	mytrackselector.MinPtTrack		       = self.cellout_pttrack							       
            	theMETRefinedClusterTrackTool.METTrackSelTool		   = mytrackselector							   
            																	   
            except:								       
            	mlog.error("could not get handle to METRefinedClusterTool Quit")       
            	print traceback.format_exc()					       
            	return False	    
	    	
            # add cellcalibtool 					       
            theMETRefinedClusterTrackTool += cellcalibtool
	
	    # add METRefinedClusterTool to list of tools 
            theMETRefAlg.AlgTools+= [ theMETRefinedClusterTrackTool.getFullName() ]
	    		
	    # add tools to alg 
            theMETRefAlg += theMETRefinedClusterTrackTool   
            #--------------------------------------------------------------------------------
            # configure tools: METRefinedClusterTool (3) => only for STVF pile-up suppression
            try:									 
            	from MissingET.MissingETConf import METRefinedClusterTool	     
            	from MissingET.MissingETConf import METTrackSelectorTool
            	from AthenaCommon.AppMgr import ToolSvc
            	ToolSvc += METTrackSelectorTool("METTrackSelTool"+self.suffix)
            	    
            	theMETRefinedClusterTrackPVTool = METRefinedClusterTool("METRefClusterTrackPV"+self.suffix);
            	theMETRefinedClusterTrackPVTool.ClusterInputCollectionKey = "CaloCalTopoCluster"						   
            	theMETRefinedClusterTrackPVTool.VxContainerKey = self.cellout_vxcontainerkey
            	theMETRefinedClusterTrackPVTool.CalibType	   =  self.cellout_calibType3	  # Eflow, Tracks		       
            	theMETRefinedClusterTrackPVTool.TrackType	   =  self.cellout_trackType3	  # STTrack, STTrackPV,STTrackCone	       
            	theMETRefinedClusterTrackPVTool.MissingETOutKey 	   = "MET_CellOut_TrackPV"+self.suffix					        										   
            	mytrackselector = METTrackSelectorTool("METTrackSelTool"+self.suffix)							       
            	mytrackselector.JetContainerName	       = self.cellout_JetInputCollectionKey					       
            	mytrackselector.MinPtTrack		       = self.cellout_pttrack							       
            	theMETRefinedClusterTrackPVTool.METTrackSelTool 	   = mytrackselector							   
        
            except:								       
            	mlog.error("could not get handle to METRefinedClusterTool Quit")       
            	print traceback.format_exc()					       
            	return False	    
	    	
            # add cellcalibtool 					       
            theMETRefinedClusterTrackPVTool += cellcalibtool
	
	    # add METRefinedClusterTool to list of tools 
            theMETRefAlg.AlgTools+= [ theMETRefinedClusterTrackPVTool.getFullName() ]
	    		
	    # add tools to alg 
            theMETRefAlg += theMETRefinedClusterTrackPVTool 
	    	     
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
            if self.jet_RunSoftJetsTool:
                theMETRefFinal        = METFinalTool("RefFinal"+self.suffix,	     
	               WhichCalc      = "RefCalib",	       # Refined Calib applied 
                       plupSupp       =  self.plupSuppCorr,		    # Off, STVF
                       EleRefKey      = "MET_RefEle"+self.suffix,     # Specify key for Ref Ele        
	               GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
	               MuoRefKey      = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon	
	               TauRefKey      = "MET_RefTau"+self.suffix,     # Specify key for Ref Tau       
                       JetRefKey      = "MET_RefJet"+self.suffix,     # Specify key for Ref Jet       
                       SoftJetsKey    = "MET_SoftJets"+self.suffix,	# Specify key for SoftJets    
                       CellOutKey     = "MET_CellOut"+self.suffix,    # Specify key for Ref CellOut   
		       muonKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                       cryoTerm       = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off    
                       cryoKey        = "MET_Cryo"+self.suffix,       # Key for Cryo corr: MET_Cryo, MET_CryoCone
                       outKey	      = "MET_RefFinal"+self.suffix
                )
            if not self.jet_RunSoftJetsTool and self.plupSuppCorr != "STVF" :
                theMETRefFinal        = METFinalTool("RefFinal"+self.suffix,	     
	               WhichCalc      = "RefCalib",	       # Refined Calib applied 
                       plupSupp       =  self.plupSuppCorr,		    # Off, STVF
                       EleRefKey      = "MET_RefEle"+self.suffix,     # Specify key for Ref Ele        
	               GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
	               MuoRefKey      = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon	
	               TauRefKey      = "MET_RefTau"+self.suffix,     # Specify key for Ref Tau       
                       JetRefKey      = "MET_RefJet"+self.suffix,     # Specify key for Ref Jet       
                       CellOutKey     = "MET_CellOut"+self.suffix,    # Specify key for Ref CellOut   
                       muonKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                       cryoTerm       = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off    
                       cryoKey        = "MET_Cryo"+self.suffix,       # Key for Cryo corr: MET_Cryo, MET_CryoCone
                       outKey	      = "MET_RefFinal"+self.suffix
                )
            if not self.jet_RunSoftJetsTool and self.plupSuppCorr == "STVF" :
                theMETRefFinal        = METFinalTool("RefFinal"+self.suffix,	     
	               WhichCalc      = "RefCalib",	       # Refined Calib applied 
                       plupSupp       =  self.plupSuppCorr,		    # Off, STVF
                       EleRefKey      = "MET_RefEle"+self.suffix,     # Specify key for Ref Ele        
	               GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
	               MuoRefKey      = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon	
	               TauRefKey      = "MET_RefTau"+self.suffix,     # Specify key for Ref Tau       
                       JetRefKey      = "MET_RefJet",                 # Specify key for default Ref Jet 
                       SoftJetsKey    = "MET_SoftJets",               # Specify key for default SoftJets   
                       CellOutKey     = "MET_CellOut_Eflow",          # Specify key for default CellOutEflow
                       JetRefCorrKey  = "MET_RefJet"+self.suffix,     # Specify key for Ref Jet Corr
                       CellOutEflowKey     = "MET_CellOut"+self.suffix,    # Specify key for Ref CellOut   
                       celloutTrackKey  = "MET_CellOut_Track"+self.suffix,    # Specify key for Ref CellOut   
                       celloutTrackPVKey= "MET_CellOut_TrackPV"+self.suffix,    # Specify key for Ref CellOut   
                       muonKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                       cryoTerm       = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off    
                       cryoKey        = "MET_Cryo"+self.suffix,       # Key for Cryo corr: MET_Cryo, MET_CryoCone
                       outKey	      = "MET_RefFinal"+self.suffix
                )
            if self.plupSuppCorr == "STVF":
                theMETCellOutSTVF        = METFinalTool("CellOutCorr"+self.suffix,	 
		       WhichCalc      = "RefCalib",	       # Refined Calib applied 
                       plupSupp       =  self.plupSuppCorr,		   # Off, STVF
                       cellOutCorr    =  self.celloutCorrection,
		       EleRefKey      = "MET_RefEle"+self.suffix,     # Specify key for Ref Ele        
		       GammaRefKey    = "MET_RefGamma"+self.suffix,   # Specify key for Ref Gamma 
		       MuoRefKey      = "MET_RefMuon_"+self.muon_algorithm+self.suffix,    # Specify key for Ref Muon  
		       TauRefKey      = "MET_RefTau"+self.suffix,     # Specify key for Ref Tau 
                       JetRefKey      = "MET_RefJet",                 # Specify key for Ref Jet 
                       SoftJetsKey    = "MET_SoftJets",               # Specify key for default SoftJets   
                       CellOutKey     = "MET_CellOut_Eflow",          # Specify key for default CellOutEflow
                       JetRefCorrKey  = "MET_RefJet"+self.suffix,     # Specify key for Ref Jet       
                       CellOutEflowKey = "MET_CellOut"+self.suffix,    # Specify key for Ref CellOut   
                       celloutTrackKey = "MET_CellOut_Track"+self.suffix,    # Specify key for Ref CellOut   
                       celloutTrackPVKey= "MET_CellOut_TrackPV"+self.suffix,    # Specify key for Ref CellOut   
                       muonKey        = "MET_Muon_Total_"+self.muon_algorithm+self.suffix,    # Key for Muon contrib
                       cryoTerm       = self.cryo_ApplyCorrection,   # "On"/"Off"= cryostat corr On / Off    
                       cryoKey        = "MET_Cryo"+self.suffix,       # Key for Cryo corr: MET_Cryo, MET_CryoCone
                       outKey	      = "MET_CellOutCorr"+self.suffix
                )

        except:
            mlog.error("could not get handle to METRefFinal Quit")  	   
            print traceback.format_exc()			  		   
            return False					  		   

	# add METFinalTool to list of tools 
        theMETAlg.AlgTools+= [ theMETMuonFinal.getFullName() ]
        theMETAlg.AlgTools+= [ theMETRefFinal.getFullName() ]
        if self.plupSuppCorr == "STVF":
            theMETAlg.AlgTools+= [ theMETCellOutSTVF.getFullName() ]
	# add tools to alg 	
        theMETAlg += theMETMuonFinal						 
        theMETAlg += theMETRefFinal
        if self.plupSuppCorr == "STVF":
            theMETAlg += theMETCellOutSTVF

	#------------------------------------------------------------------------------------------------
        # add algorithm to the specified Sequence (this should always come at the end)

        mlog.info(" now adding to sequence")
        self.sequence += theMETAlg

        return True


def make_METRefAlg(_suffix=''):
    name = 'METRefAlg' + _suffix
    cls = type (name, (METRefGetter_newplup,), {})
    cls.suffix = _suffix
    cls.sequence = AlgSequence()
    cls.ele_EgammaInputCollectionKey= "ElectronAODCollection"
    cls.ele_noCut                   = False
    cls.ele_isEM                    = PyAthena.egammaPID.ElectronIDMediumPP
    cls.ele_EtCut                   = 10.0*GeV
    cls.ele_calibType               = "RefCalib"
    
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

    cls.jet_JetInputCollectionKey   = "AntiKt4LCTopoJets"
    cls.jet_vxcontainerkey      = "VxPrimaryCandidate"
    cls.jet_UseCustomVxContainer = False
    cls.jet_JetPtCut                = 20.0*GeV
    cls.jet_JetMaxPtCut             = 1000000.0*GeV
    cls.jet_ApplyJetScale           = "Yes"
    cls.jet_UseJetMomentForScale    = True
    cls.jet_JetMomentForScale       = "LCJES"
    cls.jet_ApplyJetJVF             = "No"
    cls.jet_JetMomentForJVF         = "JVF"
    cls.jet_calibType               = "LocHad"
    cls.jet_RemoveResiduals         = True
    cls.jet_MinimumMETContribution  = 0.5
    
    cls.jet_RunSoftJetsTool         = True
    cls.jet_SoftJetsInputCollectionKey = "AntiKt4LCTopoJets"
    cls.jet_SoftJetsCalibType       = "LocHad"
    cls.jet_SoftJetsPtCut           = 5.0*GeV
    cls.jet_SoftJetsMaxPtCut        = 20.0*GeV
    cls.jet_ApplySoftJetsScale      = "No"
    cls.jet_SoftJetsUseJetMomentForScale  = False
    cls.jet_SoftJetsJetMomentForScale     = "LCJES"
    cls.jet_SoftJetsRemoveResiduals       = True
    cls.jet_SoftJetsMinimumMETContribution= 0.5
    
    cls.cellout_vxcontainerkey      = "VxPrimaryCandidate"
    cls.cellout_calibType           = "Eflow"
    cls.cellout_trackType           = "STTrack"
    cls.cellout_pttrack             = 150.
    cls.cellout_JetInputCollectionKey = "AntiKt4LCTopoJets"
    cls.photon_doPhotonTool         = True

    cls.cryo_ApplyCorrection        = "Off"
    cls.muon_container              = "StacoMuonCollection"
    cls.muon_algorithm              = "Staco"
    cls.muon_isolationAlg           = "dRJet"

    cls.plupSuppCorr                = "Off"
    cls.celloutCorrection           = "Off"
    return cls
