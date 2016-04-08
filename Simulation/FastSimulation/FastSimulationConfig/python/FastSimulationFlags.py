# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

 #=======================================================================
# File:   FastSimulationConfig/python/FastSimulationFlags.py
#=======================================================================
""" FastSimulation specific flags and job properties.  

"""
#
#
__author__  = 'M. Duehrssen'
__version__="$Revision: 1.4 $"
__doc__="Fast simulation specific flags . "

from AthenaCommon.Constants import *

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

from CaloRec.CaloCellFlags import jobproperties
from FatrasExample.FatrasKeys import FatrasKeyFlags

class JobPropertyWithForward(JobProperty):
    """ special flag class that forwards a set to true or false to other properties
    """
    AlwaysForward=False
    _forwardProperties=[]
    def Forward_Value(self):
        return self.get_Value()
    def _do_action(self):
        print "Set",self._context_name,"=",self.get_Value()
        for prop in self._forwardProperties:
            if prop.statusOn==False or self.AlwaysForward:
                print " ->",prop._context_name,"=",prop.get_Value(),": now set to",self.Forward_Value()
                prop.set_Value(self.Forward_Value())
                print " =>",prop._context_name,"=",prop.get_Value()
    def _undo_action(self):
        self._do_action()

class JobPropertyWithForwardWithValue(JobPropertyWithForward):
    """ special flag class that forwards a set to true and sets a special value to a other property
    """
    _forwardValue=True
    def Forward_Value(self):
        return self._forwardValue


class EventStoreName(JobPropertyWithForwardWithValue):
    """ EventStore name for fast simulation """
    statusOn=True
    allowedTypes=['str']
    StoredValue='StoreGateSvc'


class doFatrasID(JobPropertyWithForwardWithValue):
    """ If True runs Fatras in the Inner Detector  
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class FatrasID_InputMcEventCollection(JobPropertyWithForward):
    """McEventCollection for Truth input to Fatras in the ID"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'GEN_EVENT'
#
class FatrasID_OutputMcEventCollection(JobPropertyWithForward):
    """McEventCollection for Truth output from Fatras in the ID"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FatrasMcEventCollection'
#
#
#
class doFatrasCalo(JobProperty):
    """ If True runs Fatras in the Calo
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
#
#
#
class doFatrasMuon(JobProperty):
    """ If True runs Fatras in the Muon System
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class FatrasMuon_InputMcEventCollection(JobPropertyWithForward):
    """McEventCollection for Truth input to Fatras in the MS"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FatrasMcEventCollection'
#
class FatrasMuon_OutputMcEventCollection(JobPropertyWithForward):
    """McEventCollection for Truth output from Fatras in the MS"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FatrasMuonMcEventCollection'
#
#
#
class doFastCaloSim(JobPropertyWithForward):
    """ If True runs FastCaloSim for the calorimeter
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class FastCaloSim_InputMcEventCollection(JobPropertyWithForward):
    """McEventCollection for Truth input to FastCaloSim"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TruthEvent'
#
class doFastCaloSimCaloNoise(JobPropertyWithForward):
    """ If True adds electroic noise to the calorimeter. Needs doFastCaloSim=true
    """ 
    statusOn=False
    allowedTypes=['bool']
    StoredValue=True
#
class doFastCaloSimWithInnerDetectorTruthOnly(JobProperty):
    """ If True runs FastCaloSim for the calorimeter
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doFastCaloSimCaloAddCells(JobPropertyWithForward):
    """ If True adds FastCaloSim to an existing calorimeter reconstructed from RDO. Needs doFastCaloSim=true 
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFastCaloSimCaloH1Calibration(JobProperty):
    """ If True uses the H1 style jet weights calculated for FastCaloSim for jet/missingET calibration
    """ 
    statusOn=False
    allowedTypes=['bool']
    StoredValue=True
#
#
#
class doParametrisedMuon(JobProperty):
    """ If True runs a parametrisation of the Muon system based on Atlfast I
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
#
#
class doAtlfastICorrection(JobProperty):
    """ If True, corrects the Atlfast I results by applying efficiencies
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
#
#
class doFastCaloSimLVL1Calo(JobProperty):
    """ If True runs a LVL1 calo trigger simulaion on the cells created with FastCaloSim. STILL IN DEVELOPMENT!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFastCaloSimHLTCalo(JobProperty):
    """ If True runs a HLT calo trigger simulaion on the cells created with FastCaloSim. STILL IN DEVELOPMENT!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFatrasIDHLTID(JobProperty):
    """ If True runs a inner detector HLT simulation based on Fatras output. DO BE DEVELOPED!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFatrasMuonHLTMuon(JobProperty):
    """ If True runs a Muon HLT simulation based on Fatras output. DO BE DEVELOPED!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFatrasMuonLVL1Muon(JobProperty):
    """ If True runs a Muon LVL1 simulation based on Fatras output. DO BE DEVELOPED!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doParametrisedMuonLVL1Muon(JobProperty):
    """ If True runs a parametrisation of Muon LVL1. DO BE DEVELOPED!!!
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#

 
# Defines the container for the fast simulation flags  
class FastSimulation(JobPropertyContainer):
    """ The fast simulation flag/job property  container.
    """

# add the reconstruction flags container to the top container 
jobproperties.add_Container(FastSimulation)


# I want always the following flags in the Rec container  
_list_FastSimulation=[EventStoreName,doFatrasID,FatrasID_InputMcEventCollection,FatrasID_OutputMcEventCollection,
                      doFatrasCalo,
                      doFatrasMuon,FatrasMuon_InputMcEventCollection,FatrasMuon_OutputMcEventCollection,
                      doFastCaloSim,FastCaloSim_InputMcEventCollection,doFastCaloSimCaloNoise,doFastCaloSimWithInnerDetectorTruthOnly,doFastCaloSimCaloAddCells,doFastCaloSimCaloH1Calibration,
                      doParametrisedMuon,
                      doAtlfastICorrection,
                      doFastCaloSimLVL1Calo,doFastCaloSimHLTCalo,doFatrasIDHLTID,doFatrasMuonHLTMuon,doFatrasMuonLVL1Muon,doParametrisedMuonLVL1Muon
                     ]
for j in _list_FastSimulation: 
    jobproperties.FastSimulation.add_JobProperty(j)
del _list_FastSimulation

jobproperties.FastSimulation.doFatrasID._forwardProperties=[jobproperties.FastSimulation.doFastCaloSimWithInnerDetectorTruthOnly]
jobproperties.FastSimulation.doFatrasID.AlwaysForward=True
jobproperties.FastSimulation.doFatrasID._forwardValue=False

jobproperties.FastSimulation.doFastCaloSim._forwardProperties=[jobproperties.FastSimulation.doFastCaloSimCaloH1Calibration,jobproperties.CaloCellFlags.doFastCaloSim,jobproperties.FastSimulation.doFastCaloSimCaloNoise]
jobproperties.FastSimulation.doFastCaloSimCaloNoise._forwardProperties=[jobproperties.CaloCellFlags.doFastCaloSimNoise]
jobproperties.FastSimulation.doFastCaloSimCaloNoise.AlwaysForward=True
jobproperties.FastSimulation.doFastCaloSimCaloAddCells._forwardProperties=[jobproperties.CaloCellFlags.doFastCaloSimAddCells]
jobproperties.FastSimulation.doFastCaloSimCaloAddCells.AlwaysForward=True

jobproperties.FastSimulation.FatrasID_InputMcEventCollection._forwardProperties=[jobproperties.FatrasKeys.InputMcEventCollection]
jobproperties.FastSimulation.FatrasID_InputMcEventCollection.AlwaysForward=True

jobproperties.FastSimulation.FatrasID_OutputMcEventCollection._forwardProperties=[jobproperties.FatrasKeys.McEventCollection,jobproperties.FastSimulation.FastCaloSim_InputMcEventCollection]
jobproperties.FastSimulation.FatrasID_OutputMcEventCollection.AlwaysForward=True


#=======================================================================
