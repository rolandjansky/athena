#=======================================================================
# File:   CTB_G4Sim/share/CaloRunConditionsData.py
#
# Contents:
#        Run conditions to simulate the 'good validated' calo CTB data. 
#
#=======================================================================
"""
 Run conditions to simulate the 'good validated' calo CTB data. 

  The minimal information required per run is the 'SimLayout' that must 
 be equal to 'ctbh8_calodata'. This implies that you will not have IDET 
 or Muon detectors and the ancillary upstream MBPSID it will be set to 
 the configuration of the 'PeriodA'. 
  Note that the IDET Oracle tag is 'InnerDetector-CTB-01' but in here we 
 do not have IDET at all.    
  Energy, type of particle or number of events SHOULD NOT BE WRITTEN HERE!!!. 
  For the moment we will run all the runs with the extended barrel  

  The run number must be with more than 4 digits to avoid problems 
 with the run conditions defined in CtbRunConditionsData.py for the 
 combined period. In this file the run number here corresponds to the real 
 run number.       
 Information collected by A. Solodkov and V. Giangiobbe 

"""
__version__="$Revision: 1.6 $"
#
from G4AtlasApps.SimFlags import RunCondition 
from G4AtlasApps import AtlasG4Eng

AtlasG4Eng.G4Eng.log_Level.info('The contents of CaloRunConditionsData is'+\
' now in CTB_G4Sim.CaloRunConditionsData')
AtlasG4Eng.G4Eng.log_Level.warning(' March 2007, CaloRunConditionsData is an'+\
' OBSOLETE '+\
'module. Look at : %s' % AtlasG4Eng.G4Eng.Dict_WebLinks.get('recipes'))


#=======================================================================
