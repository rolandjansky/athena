/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArRawEvent Package

This package provides the RAW event data model classes for LArCalorimeter.

@section LArRawEventDigit Digit related classes

LArDigit is the output of the FEB (and hence the digitization package) describing the digitized waveform shape.
All LArDigits are stored in a LArDigitContainer <br>
LArCalibDigit is a LArDigit plus some information for electronics calibration data (DAC,delay,isPulsed). 
All LArCalibDigits are stored in LArCalibDigitContainer <br>
LArAccumulatedCalibDigit is the result of the accumulation inside the DSP of LArCalibDigit. It saves the sum and the sum of the squares of the ADC counts for the accumulated events with the same calibration settings.  All LArAccumulatedCalibDigits are stored in LArAccimulatedCalibDigitContainer <br>

@section LArRawEventRawChannel Raw Channel related classes
LArRawChannel is the output of the ROD processing and contain the fitted waveform results. All LArRawChannel are stored in a LArRawChannelCollection.
Each LArRawChannelCollection corresponds to data in one ROB. Therefore a collection of LArRawChannelCollection is saved in an IdentifiableContainer called LArRawChannelContainer.

@section LArRawEventTTL1  L1 trigger related classes
LArTTL1 is a digitized L1 trigger tower for the LAr. All LArTTL1 are stored in a LArTTL1Container.

@section LArRawEventOther Other LArRawEvent classes

*/
