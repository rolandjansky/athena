/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H
#define JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H

#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"

#include "TString.h"
#include "TH1.h"

class TFile;
class TAxis;

namespace jet
{

class UncertaintyHistogram : public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        UncertaintyHistogram(const std::string& histName, const bool interpolate);
        UncertaintyHistogram(const TString histName, const bool interpolate);
        UncertaintyHistogram(const char* histName, const bool interpolate);
        UncertaintyHistogram(const UncertaintyHistogram& toCopy);
        UncertaintyHistogram & operator=(const UncertaintyHistogram &) =delete;
        virtual ~UncertaintyHistogram();
        virtual StatusCode initialize(TFile* histFile);

        // Member retrieval methods
        const TString& getName()   const { return m_name;        }
        const TH1*     getHisto()  const { return m_histo;       }
        bool           getInterp() const { return m_interpolate; }
        int            getNumDim() const { return m_nDim;        }

        // Histogram information access
        double getValue(const double var1) const;
        double getValue(const double var1, const double var2) const;
        double getValue(const double var1, const double var2, const double var3) const;

    private:
        // Private members
        bool m_isInit;
        const TString m_name;
        const bool m_interpolate;
        TH1* m_histo;
        int  m_nDim;

        // Histogram reading helpers
        double readHisto(const double var1, const double var2=0, const double var3=0) const;
        double checkBoundaries(const TAxis* axis, const int numBins, const double valInput) const;

        // Helper to have a const method for interpolation (why is there not a const version in ROOT???)
        double Interpolate(const TH1* histo, const double x) const;
        double Interpolate(const TH1* histo, const double x, const double y) const;
        double Interpolate(const TH1* histo, const double x, const double y, const double z) const;
        Int_t FindBin(const TAxis* axis, const double x) const;
        
        double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis=1, const int yAxis=2, const int otherDimBin=-1) const;
};

} // end jet namespace


#endif

