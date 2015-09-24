# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_JetStream
##
## Author:      Eric Feng (Chicago)
## Created:     August 2008
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
## 
## Description: Here, all necessary job flags for the Primary Jet DPD
##              are defined.
##
## $Id: PrimaryDPDFlags_JetStream.py,v 1.3 2009-01-14 20:46:32 mkaneda Exp $
##
##=============================================================================

__doc__ = """Here, all necessary job flags for the Primary Jet DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "Eric Feng <Eric.Feng@cern.ch>"

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_JetStream(JobPropertyContainer):
    """ The Primary Jet DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_JetStream)

#short-cut to get the PrimaryDPDFlags_JetStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_JetStream import primJetDPD'
#Note that name has to be different to avoid problems with pickle
primJetDPD=jobproperties.PrimaryDPDFlags_JetStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Jet output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(ApplySlimming)


##--------------------------------
## Selections
##--------------------------------
class doOfflineSelection(JobProperty):
    """  Set the properties of the jet Et filter #1."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(doOfflineSelection)

class JetEt1Filter(JobProperty):
    """  Set the properties of the jet Et filter #1."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 0.0*Units.GeV
    JetEtMax           = 80*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 50
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt1Filter)

class JetEt2Filter(JobProperty):
    """  Set the properties of the jet Et filter #2."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 80*Units.GeV
    JetEtMax           = 140*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 30
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt2Filter)

class JetEt3Filter(JobProperty):
    """  Set the properties of the jet Et filter #3."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 140*Units.GeV
    JetEtMax           = 200*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 20
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt3Filter)

class JetEt4Filter(JobProperty):
    """  Set the properties of the jet Et filter #4."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 200*Units.GeV
    JetEtMax           = 300*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 10
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt4Filter)

class JetEt5Filter(JobProperty):
    """  Set the properties of the jet Et filter #5."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 300*Units.GeV
    JetEtMax           = 400*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 5
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt5Filter)

class JetEt6Filter(JobProperty):
    """  Set the properties of the jet Et filter #6."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    JetCollectionName  = "AntiKt4TopoEMJets"
    JetEtMin           = 400*Units.GeV
    JetEtMax           = 1.0E9*Units.GeV
    JetEtaMin          = 0.
    JetEtaMax          = 9999.
    nLeadingsForCheck  = 999999999
    minNumberPassed    = 1
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetEt6Filter)

class TileSel1Filter(JobProperty):
    """  Set the properties of the Tile filter #7."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    CellContName       = "AllCalo"
    DigitsContName     = "TileDigitsFlt"
    RawChannelContName = ""
    TileSelMinEnCell   = -100*Units.GeV
    TileSelMaxEnCell   = 700*Units.GeV
    TileSelMinEnChan   = -100*Units.GeV
    TileSelMaxEnChan   = 500*Units.GeV
    TileSelMinEnGap    = -100*Units.GeV
    TileSelMaxEnGap    = 500*Units.GeV
    TileSelMinEnMBTS   = -100*Units.GeV
    TileSelMaxEnMBTS   = 500*Units.GeV
    TileSelSkipMasked  = True
    TileSelSkipMBTS    = True
    TileSelCheckDCS    = False
    TileSelCheckJumps  = False
    TileSelCheckDMUs   = False
    TileSelCheckOverLG = True
    TileSelCheckOverHG = False
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(TileSel1Filter)


# The trigger filters; these run with additional offline filters
class JetTriggerFilter(JobProperty):
    """  Set the properties of the jet trigger filters."""
    statusOn         = True
    allowedTypes     = ['bool']
    StoredValue      = True
    TriggerNames     = [ [ "EF_j10_a4tchadloose", 1 ],  #  Hz @6x10^33         
                         [ "EF_j10_a4tchadloose_L1MBTS", 1 ],  #  Hz @6x10^33
                         [ "EF_j15_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j15_a4tchad_L1MBTS", 1 ],  #  Hz @6x10^33       
                         [ "EF_j15_a4tchad_L1TE20", 1 ],  #  Hz @6x10^33       
                         [ "EF_j25_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j25_a4tchad_L1MBTS", 1 ],  #  Hz @6x10^33       
                         [ "EF_j25_a4tchad_L1TE20", 1 ],  #  Hz @6x10^33       
                         [ "EF_j35_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j35_a4tclcw", 1 ],  #  Hz @6x10^33              
                         [ "EF_j35_a4tthad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j45_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j55_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j80_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j110_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_j145_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_j180_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_j220_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_j280_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_j360_a4tchad", 1 ],  #  Hz @6x10^33              
                         [ "EF_j360_a4tclcw", 1 ],  #  Hz @6x10^33             
                         [ "EF_j460_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_L1J350_NoAlg", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj10_a4tchadloose", 1 ],  #  Hz @6x10^33        
                         [ "EF_fj10_a4tchadloose_L1MBTS", 1 ],  #  Hz @6x10^33 
                         [ "EF_fj15_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj15_a4tchad_L1MBTS", 1 ],  #  Hz @6x10^33      
                         [ "EF_fj15_a4tchad_L1TE20", 1 ],  #  Hz @6x10^33      
                         [ "EF_fj25_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj25_a4tchad_L1MBTS", 1 ],  #  Hz @6x10^33      
                         [ "EF_fj25_a4tchad_L1TE20", 1 ],  #  Hz @6x10^33      
                         [ "EF_fj35_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj45_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj45_a4tchad_L2FS_L1MBTS", 1 ],  #  Hz @6x10^33 
                         [ "EF_fj45_a4tchad_L2FS_L1TE20", 1 ],  #  Hz @6x10^33 
                         [ "EF_fj55_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj80_a4tchad", 1 ],  #  Hz @6x10^33             
                         [ "EF_fj110_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj145_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj180_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj220_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj280_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj360_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_fj460_a4tchad", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk9", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk9_L1HA8", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk9_L1RD0", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk18", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk18_L1HA8", 1 ],  #  Hz @6x10^33            
                         [ "EF_hadCalib_trk18_L1RD0", 1 ],  #  Hz @6x10^33            
                         [ "EF_j35_a10tcem", 1 ],  #  Hz @6x10^33              
                         [ "EF_j110_a10tcem_L2FS", 1 ],  #  Hz @6x10^33        
                         [ "EF_j240_a10tcem", 1 ],  #  Hz @6x10^33             
                         [ "EF_j240_a10tclcw", 1 ],  #  Hz @6x10^33            
                         [ "EF_2j240_a10tcem", 1 ],  #  Hz @6x10^33            
                         [ "EF_2j240_a10tclcw", 1 ],  #  Hz @6x10^33           
                         [ "EF_2j240_a10tcem_L2FS", 1 ],  #  Hz @6x10^33       
                         [ "EF_2j240_a10tclcw_L2FS", 1 ],  #  Hz @6x10^33      
                         [ "EF_j360_a10tcem", 1 ],  #  Hz @6x10^33             
                         [ "EF_j360_a10tclcw", 1 ],  #  Hz @6x10^33            
                         [ "EF_j460_a10tcem", 1 ],  #  Hz @6x10^33             
                         [ "EF_j460_a10tclcw", 1 ] ]  #  Hz @6x10^33           
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetTriggerFilter)

# The calibration trigger filters; these run without additional offline filters
class JetCalibrationTriggerFilter(JobProperty):
    """  Set the properties of the jet trigger filters."""
    statusOn         = True
    allowedTypes     = ['bool']
    StoredValue      = False
    TriggerNames     = [ [ "EF_j10_a4tchad",      1 ],  #  Hz @10^33
                         [ "EF_j15_a4tchad",      1 ],  #  Hz @10^33
                         [ "EF_j20_a4tchad",      1 ],  #  Hz @10^33
                         [ "EF_j30_a4tchad",      1 ],  #  Hz @10^33
                         [ "EF_fj30_a4tchad",     1 ] ] #  Hz @10^33
    pass
jobproperties.PrimaryDPDFlags_JetStream.add_JobProperty(JetCalibrationTriggerFilter)

