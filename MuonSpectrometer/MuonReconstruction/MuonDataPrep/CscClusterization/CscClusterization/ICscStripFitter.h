/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICscStripFitter.h

#ifndef ICscStripFitter_H
#define ICscStripFitter_H

// David Adams
// April 2007
//
// Interface for tool to fit the time samples on a CSC strip.
// Returns the peak charge, the time of the peak and some quality measures.

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/CscTimeStatus.h"
#include "CscStripStatus.h"
class Identifier;
//#include "Identifier/IdentifierHash.h"
//namespace Muon {
//  class CscStripPrepData;
//}

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscStripFitter("ICscStripFitter", 1, 0);

class ICscStripFitter : virtual public IAlgTool {

public:  // Embedded class.

  // This is a description of a fitted strip.
  // The status, charge and time should always be filled. The other values
  // are optional and, in some cases, their precise meaning will depend on
  // the algorithm used to fill them.
  class Result {
  public:
    int status;             // Fit status: 0 for success
    Muon::CscStripStatus stripStatus; // strip status 
    const Muon::CscStripPrepData* strip;  // Pointer to the strip.
    double sampleTime;      // Period of input samples.
    double charge;          // Peak current (electrons/sampleTime)
    double dcharge;         // Error in the peak current.
    double time;            // Time (ns) of the peak. t0 corrected time since 16.7.X
    double dtime;           // Error in the time.
    double width;           // Full width (ns) of the peak, e.g. FWHM, 2sigma, ...
    double dwidth;          // Error in the width.
    double chsq;            // Chi-square for the fit, -1 if not calculated.
    bool   phase;

    double time_beforeT0Corr;         // Time (ns) of the peak. without t0 correction
    double time_beforeBPCorr; 
    double charge_beforeGainCorr;         // Time (ns) of the peak. without t0 correction
    double charge_beforeBPCorr;         // Time (ns) of the peak. without t0 correction
    Muon::CscTimeStatus timeStatus;
    Result(Muon::CscStripStatus sstat =Muon::CscStrStatUndefined,
           Muon::CscTimeStatus tstat =Muon::CscTimeStatusUndefined) :
      status(0), stripStatus(sstat),strip(0), sampleTime(0.0), charge(0.0), dcharge(0.0),
      time(0.0), dtime(0.0), width(0.0), dwidth(0.0), chsq(-1.0), phase(0),
      time_beforeT0Corr(-99999.0), time_beforeBPCorr(-99999.0), charge_beforeGainCorr(0.0), charge_beforeBPCorr(0.0),
      timeStatus(tstat)
    {}

  };

public:  // Typedefs.

  // List of time samples.
  typedef std::vector<float> ChargeList;

public:  // Static methods.

  // Return the interface ID.
  //  static const InterfaceID& interfaceID();
  static const InterfaceID& interfaceID() {  return IID_ICscStripFitter; }


public:  // Interface methods

  // Fit a list of sample charges.
  // This method returns the time relative to the time of the first sample.
  // Implementation here fails.
  // Typical subclass will override this method.
  //virtual Result fit(const ChargeList& ChargeList, double samplingTime, IdentifierHash& stripHashId) const;
  //  virtual Result fit(const ChargeList& ChargeList, double samplingTime, Identifier& sid) const;

  virtual Result fit(const ChargeList& ChargeList,
                     double samplingTime, bool samplingPhase, 
                     Identifier& sid) const;

  // Fit a prep data strip.
  // Implementation here uses the above method.
  // Typical subclass overiding the above need not overide this.
  // This method returns the time relative to the time of the first sample.
  // The strip is set in the result.
  virtual Result fit(const Muon::CscStripPrepData& strip) const;

};

#endif
