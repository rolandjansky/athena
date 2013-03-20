# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################################
#
# METTrack Getters for MissingET.METAlg
#
# Author : Jet Goodson after S. Resconi
# Date :   27 March  2010
#
#How to run this:
#include this file
#then add the line:
#METGetterTrack()
#before your alg (ie, the d3pd)
##############################################################################################

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import *      # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger
import traceback                          # to allow printout of trace back

from RecExConfig.Configured import Configured            # import base class

from MissingET.METRefGetter import getStandardCalibTool  # to get calib from DB

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault           
theCaloNoiseTool=CaloNoiseToolDefault()                                   
from AthenaCommon.AppMgr import ToolSvc                                   
ToolSvc+=theCaloNoiseTool

class METGetterTrack ( Configured ):
	   
    def configure(self):
        mlog = logging.getLogger ('METGetterTrack::configure:')
        mlog.info ('entering')
        
        # now configure the algorithm       
        try:
            from MissingET.MissingETConf import METAlg
            theMETAlg=METAlg("METAlg")
        except:                                               
            mlog.error("could not import MissingET.METAlg")   
            print traceback.format_exc()                       
            return False       
	        
                
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.detdescr.ID_on():   
            try:                                                                     
                from MissingET.MissingETConf import METTrackTool                             
                theMETTrackTool = METTrackTool("METTrack");                           

                
                # if doFastCaloSim set calibrator tool for ATLFAST2                           
                from CaloRec.CaloCellFlags import jobproperties                               
                if jobproperties.CaloCellFlags.doFastCaloSim:                                 
                    doAtlfastII=True                                                         
                else:                                                                         
                    doAtlfastII=False
                    
                cellcalibtool  = getStandardCalibTool(doAtlfastII);
                calibtool_name = cellcalibtool.name();
                  
                # add track select user interface                                             
                theMETTrackTool.trackd0          = 1.5            # cut on trackd0           
                theMETTrackTool.trackz0          = 1.50            # cut on trackz0           
                theMETTrackTool.trackPtMin       = 500.0           # cut on trackPtMin       
                theMETTrackTool.trackPtMax       = 9999999.0                                 
                theMETTrackTool.trackPixelHits   = 1                                         
                theMETTrackTool.trackSCTHits     = 6                                         
                theMETTrackTool.trackChi2OverNdf = 999999                                     
                theMETTrackTool.UseInsideOut     = 1
                
                theMETTrackTool.outKey           = "MET_Track"                                                           
                
            except:                                                                           
                mlog.error("could not get handle to METTrackTool Quit")                       
                print traceback.format_exc()                                                 
                return False                                                                 
	       
            # add cellcalibtool                                                               
            theMETTrackTool += cellcalibtool                                                 
            
            # add  METTrackTool to list of tools                                             
            theMETAlg.AlgTools+= [ theMETTrackTool.getFullName() ]                     
            
            # add tools to alg                                                               
            theMETAlg += theMETTrackTool


#------------------------------------------------------------------------------------------------
	        # add algorithm to topSequence (this should always come at the end)
	
        mlog.info(" now adding to topSequence")       
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()               
        topSequence += theMETAlg
	                   
        return True     
            
