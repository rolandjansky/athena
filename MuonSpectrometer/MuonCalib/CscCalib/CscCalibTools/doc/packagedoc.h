/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page CscCalibTools_page The CscCalibTools package

- CscCalibToolBase: this class is the base class for CSC calibration tools and provides a 
default implementation of the calibration tools. This class implements the charge to ADC 
conversion, useful for simulation where in the muon digitization the charge on the strip is 
obtained first, then converted to RDO (Raw Data Objects). For the CSC, the RDO are ADC 
samplings: thus, a conversion from charge (femtoCoulomb) to ADC counts. A reverse conversion, 
from RDO to charge is also implemented: this is useful for both the simulation and the real data 
where the raw data is byte stream or RDO. The calibration constants, needed for these 
conversions are to be obtained from the conditions database through a Gaudi service (the service 
is not yet implemented)

- ICscCalibTool: abstract interface for CSC calibration tools, it defines
the methods which the user may override in his/her derived classes

- CscCalibTool.h: it derives from CscCalibToolBase.h and uses the default implementation of the 
conversion tools ADC to charge and vice versa. It will eventually implement the charge finding 
tools, the pedestal subraction, the communication with the condition database and various 
corrections.

For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov

*/
