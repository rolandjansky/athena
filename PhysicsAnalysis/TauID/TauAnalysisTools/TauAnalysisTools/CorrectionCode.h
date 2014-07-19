// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORRECTIONCODE_H
#define CORRECTIONCODE_H

namespace TauAnalysisTools {

  class CorrectionCode {

  public:
    enum ECorrectionCode {
      OK                 = 1, ///< Tool applied successfully
      OutOfValidityRange = 2, ///< Correction can't be applied on object
      Error              = 0  ///< General error during execution
    };

    /// Constructor
    CorrectionCode( ECorrectionCode code = OK ) : m_code( code ) {}

    /// Automatic conversion to an enum value
    operator ECorrectionCode() const { return m_code; }



  private:
    /// The correction code
    ECorrectionCode m_code;

  }; // class CorrectionCode

  CorrectionCode CorrectionCodeMin(CorrectionCode c1, CorrectionCode c2);
  CorrectionCode CorrectionCodeMax(CorrectionCode c1, CorrectionCode c2);  

} // namespace TauAnalysisTools



#endif // CORRECTIONCODE_H
