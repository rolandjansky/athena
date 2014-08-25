# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################################
#
# Reconstruction Getters for MissingET.METAlg
#
# Author : S. Resconi
# Date :   May 2009
#
# June 2009:
# Modified by Giovanni Siragusa (siragusa@uni-mainz.de)
#
# This modified version runs in a Sequence istantiated
# when a Z-->ee candidate is found.
#
##############################################################################################

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import *      # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback                          # to allow printout of trace back

from RecExConfig.Configured import Configured            # import base class

					           
class METGetterAOD ( Configured ):
    
    def configure(self):
        mlog = logging.getLogger ('METGetterAOD::configure:')
        mlog.info ('entering')
	
	# now configure the algorithm       
        try:
            from MissingET.MissingETConf import METAlg
            theMETAlg=METAlg("METAlg")
        except: 					       
            mlog.error("could not import MissingET.METAlg")    
            print traceback.format_exc()		       
            return False	
 			      	
	#------------------------------------------------------------------------------------------------
        # configure tools: METFinal
        try:								             
            from MissingET.MissingETConf import METFinalTool		
     	              
            theMETRefFinal     = METFinalTool("RefFinal",	 
		   WhichCalc   = "RefCalib",  	   # Refined Calib applied 
		   EleRefKey   = "MET_RefEle",     # Specify key for Ref Ele	   
		   GammaRefKey = "MET_RefGamma",   # Specify key for Ref Gamma 
		   #MuoRefKey   = "MET_RefMuon",    # Specify key for Ref Muon	  
		   #TauRefKey   = "MET_RefTau",     # Specify key for Ref Tau	  
		   JetRefKey   = "MET_RefJet",     # Specify key for Ref Jet	  
		   CellOutKey  = "MET_CellOut",    # Specify key for Ref CellOut   
		   muonKey     = "MET_MuonBoy",    # Key for Muon contrib: MET_Muon, MET_MuonBoy 
		   cryoTerm    = "Off",       	   # "On"/"Off"= cryostat corr On / Off    
		   cryoKey     = "MET_Cryo",       # Key for Cryo corr: MET_Cryo, MET_CryoCone
		   outKey      = "MET_RefFinal"
	    )   
	    							         
        except: 						  		   
            mlog.error("could not get handle to METRefFinal Quit")  	   
            print traceback.format_exc()			  		   
            return False					  		   
        
	# add METFinalTool to list of tools 
        theMETAlg.AlgTools+= [ theMETRefFinal.getFullName() ]
	
	# add tools to alg 	
        theMETAlg += theMETRefFinal			 
	
	#------------------------------------------------------------------------------------------------
        # add algorithm to topSequence (this should always come at the end)

        mlog.info(" now adding to topSequence")        
		    
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()                
        topSequence.ZeeSequence += theMETAlg

        return True	
