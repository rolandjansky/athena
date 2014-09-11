/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage JetCalibTools

@section Overview
This package contains all the jet calibration methods.

It is intended for physicists use and is also used in standard reconstruction.

Each calibration method (or "tool") is implemented in a separate C++ class inheriting the 
IJetCalibrator interface.
They provide methods to 
  - calibrate a given jet
  - return a calibrated copy,
  - store corrections as jet moment.
  - return only the computed corrections.

The corrections can be a simple scale factor or a set of numbers depending on 
the calibration tool.

@section Usage
All calibration tools are available as Athena tool (AthAlgTool), and some 
of them are also available in ARA.

It can be used as usual with  ToolHandle, then example of use could be :

@code
// get a calibrated copy : user must delete it when no more used.
Jet* myCalJet = myCalibTool->get_calibrated_copy(myUncalJet);
// store correction as jet moment :
myCalibTool->store_calibration_correction(myUncalJet);
@endcode


@section Developers

Writing a calibration tool is writing a class inheriting  IJetCalibrator.
Some base implementation have been written, as  JetCalibratorBase.

@image html diagssimple.png

Developers can inherit JetCalibratorAthena or JetCalibratorARAAthena (also ARA 
availablilty but no DB support).
All they have to do is to implement 2 JetCalibratorBase functions :

@code
  // Performs the calculation of corrections and store them in m_corrections
  virtual bool  compute_corrections(const Jet* /*jet_in*/ ){return false;}
  // Use the values calculated in m_corrections to correct the Jet 4mometum
  virtual bool  correct_4mom(Jet* /*jet_in*/){return false;}
@endcode

All the other functions of the interface will then be provided by the JetCalibratorBase class.

See also the wiki for more details :
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetCalibTools

@htmlinclude used_packages.html

@include requirements

*/
