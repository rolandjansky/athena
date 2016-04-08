#=======================================================================
# File:   CTB_G4Sim/share/CtbRunConditionsData.py
#
# Contents:
#         Run conditions to simulate the 'good validated' CTB data. 
#
#=======================================================================
"""
 Run conditions to simulate the 'good validated' CTB data in the 
 combined period. 

  The minimal information required per run is the 'SimLayout'. Energy, 
 type of particle or number of events SHOULD NOT BE WRITTEN HERE!!!. 

 Information collected by Thomas Koffas and Roberto Petti 
 that cover runs in the real-data range: 

   2101725 to 2102986 including the photon period that starts at 2102775 

 Note: 
 (1) Starting from revision version 1.15 the magnetic field for the ID 
     is taken from the  corresponding magnetic field map. The old constant 
     value stays here but it is not used.  
 (2) Starting from revision version 1.17 it is possible to use beam 
     divergence data per run. Data collected by Thomas Koffas and 
     Mayuko Kataoka.  

"""
__version__="$Revision: 1.20 $"
#
from G4AtlasApps.SimFlags import RunCondition 
from G4AtlasApps import AtlasG4Eng

AtlasG4Eng.G4Eng.log_Level.info('The contents of CtbRunConditions is'+\
' now in CTB_G4Sim.CtbRunConditions')
AtlasG4Eng.G4Eng.log_Level.warning(' March 2007, CtbRunConditions is an'+\
' OBSOLETE '+\
'module. Look at : %s' % AtlasG4Eng.G4Eng.Dict_WebLinks.get('recipes'))

#=======================================================================
