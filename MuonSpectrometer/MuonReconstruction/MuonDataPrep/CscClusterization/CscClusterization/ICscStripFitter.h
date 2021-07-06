/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ICscStripFitter_H
#define ICscStripFitter_H

#include <float.h>

#include <vector>

#include "CscStripStatus.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/CscTimeStatus.h"

// Interface for tool to fit the time samples on a CSC strip.
// Returns the peak charge, the time of the peak and some quality measures.
class ICscStripFitter : virtual public IAlgTool {
public:  // Embedded class.
    // This is a description of a fitted strip.
    // The status, charge and time should always be filled. The other values
    // are optional and, in some cases, their precise meaning will depend on
    // the algorithm used to fill them.
    class Result {
    public:
        int status{0};                                                // Fit status: 0 for success
        Muon::CscStripStatus stripStatus{Muon::CscStrStatUndefined};  // strip status
        const Muon::CscStripPrepData* strip{nullptr};                 // Pointer to the strip.
        double sampleTime{0.};                                        // Period of input samples.
        double charge{0.};                                            // Peak current (electrons/sampleTime)
        double dcharge{0.};                                           // Error in the peak current.
        double time{0.};                                              // Time (ns) of the peak. t0 corrected time since 16.7.X
        double dtime{0.};                                             // Error in the time.
        double width{0.};                                             // Full width (ns) of the peak, e.g. FWHM, 2sigma, ...
        double dwidth{0.};                                            // Error in the width.
        double chsq{0.};                                              // Chi-square for the fit, -1 if not calculated.
        bool phase{false};

        double time_beforeT0Corr{-FLT_MAX};  // Time (ns) of the peak. without t0 correction
        double time_beforeBPCorr{-FLT_MAX};
        double charge_beforeGainCorr{0.};  // Time (ns) of the peak. without t0 correction
        double charge_beforeBPCorr{0.};    // Time (ns) of the peak. without t0 correction
        Muon::CscTimeStatus timeStatus{Muon::CscTimeStatusUndefined};
        Result(Muon::CscStripStatus sstat = Muon::CscStrStatUndefined, Muon::CscTimeStatus tstat = Muon::CscTimeStatusUndefined) {
            stripStatus = sstat;
            timeStatus = tstat;
        }
        Result(const Result&) = default;
        ~Result() = default;
    };

public:  // Typedefs.
    // List of time samples.
    typedef std::vector<float> ChargeList;

public:  // Static methods.
    // Return the interface ID.
    static const InterfaceID& interfaceID() { /** Must declare this, with name of interface*/
        static const InterfaceID IID_ICscStripFitter("ICscStripFitter", 1, 0);
        return IID_ICscStripFitter;
    }

public:  // Interface methods
    // Fit a list of sample charges.
    // This method returns the time relative to the time of the first sample.
    // Implementation here fails.
    // Typical subclass will override this method.
    // virtual Result fit(const ChargeList& ChargeList, double samplingTime, IdentifierHash& stripHashId) const;
    //  virtual Result fit(const ChargeList& ChargeList, double samplingTime, Identifier& sid) const;

    virtual Result fit(const ChargeList& ChargeList, double samplingTime, bool samplingPhase, Identifier& sid) const;

    // Fit a prep data strip.
    // Implementation here uses the above method.
    // Typical subclass overiding the above need not overide this.
    // This method returns the time relative to the time of the first sample.
    // The strip is set in the result.
    virtual Result fit(const Muon::CscStripPrepData& strip) const;

    virtual ~ICscStripFitter() = default;
};

#endif
