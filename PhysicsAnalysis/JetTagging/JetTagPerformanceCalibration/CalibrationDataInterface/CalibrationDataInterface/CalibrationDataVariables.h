/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataVariables                                                                 //
//                                                                                          //
// This class (struct, actually) is nothing but a light-weight container of (kinematic or   //
// other) variables. Encapsulating these variables has the advantage of providing a         //
// framework-independent interface.                                                         //
// A drawback is that it is not a priori obvious which variables will be relevant           //
// for which calibration. The following rules of thumb apply:                               //
/* Begin_Html
<ul>
  <li>specifying more information than strictly necessary doesn't hurt!</li>
  <li>always specify the jet collection</li>
  <li>the tag weight variable is relevant only in case of so-called continuous tagging</li>
</ul>
End_Html */
//                                                                                          //
// CalibrationDataVariables.h, (c) ATLAS Detector software                                  //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAVARIABLES_H
#define ANALYSISCALIBRATIONDATAVARIABLES_H

#include <string>
#include <utility>

namespace Analysis 
{

  /** @class CalibrationDataVariables

      This class (struct, actually) is nothing but a light-weight container of
      (kinematic or other) variables.

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataVariables {
  public:
    std::string jetAuthor;
    double jetPt;                    // in MeV
    double jetEta;
    double jetTagWeight;             // actual output of the tagging algorithm (relevant only for "continuous" tagging)
  };

  /** @enum CalibrationStatus

      The following enums are intended to provide meaningful status codes. They are
      used internally (by the containers) but are available also at the interface level.
  */

  enum CalibrationStatus {
    kSuccess = 0,           // all OK
    kRange = 1,             // given coordinates outside the range of validity of the calibration
    kExtrapolatedRange = 2, // given coordinates even outside the extrapolation range 
    kError = 3              // "technical" error (typically: nonexistent object)
  };

}

#endif // ANALYSISCALIBRATIONDATAVARIABLES_H
