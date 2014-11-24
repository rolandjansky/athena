/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H
#define JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H

#include "xAODJet/Jet.h"

#include "AsgTools/AsgMessaging.h"

#include "TString.h"
#include "TH1.h"

class TFile;

namespace jet
{

class UncertaintyHistogram : virtual public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        UncertaintyHistogram(const std::string& histName = "", const std::string& validHistName = "", const bool interpolate = true);
        UncertaintyHistogram(const TString histName, const TString validHistName = "", const bool interpolate = true);
        //UncertaintyHistogram(const char* histName, const char* validHistName = NULL, const bool interpolate = true);
        UncertaintyHistogram(const UncertaintyHistogram& toCopy);
        virtual ~UncertaintyHistogram();
        virtual StatusCode Initialize(TFile* histFile);

        // Member retrieval methods
        const TString& getName() const  { return m_name; }
        const TH1* getHisto() const     { return m_uncHisto; }

        // Validity retrieval
        bool getValidity(const float var1) const;
        bool getValidity(const float var1, const float var2) const;
        bool getValidity(const float var1, const float var2, const float var3) const;

        // Uncertainty retrieval (crash if not valid)
        double getUncertainty(const float var1) const;
        double getUncertainty(const float var1, const float var2) const;
        double getUncertainty(const float var1, const float var2, const float var3) const;

        // Uncertainty retrieval (unc only set if valid, returns true if valid)
        bool getValidUncertainty(double& unc, const float var1) const;
        bool getValidUncertainty(double& unc, const float var1, const float var2) const;
        bool getValidUncertainty(double& unc, const float var1, const float var2, const float var3) const;

    private:
        // Private members
        bool m_isInit;
        const TString m_name;
        TString m_validName;
        const bool m_interpolate;
        TH1* m_uncHisto;
        TH1* m_validHisto;
        int  m_nDim;

        // Generic helpers
        TString getFullName() const;

        // Histogram reading helpers
        double readHisto(const TH1* histo, const float var1, const float var2=0, const float var3=0, const bool interpolate=true) const;
        bool   readValidityHisto(const float var1, const float var2=0, const float var3=0) const;
        double readUncertaintyHisto(const float var1, const float var2=0, const float var3=0) const;

        // Helper to have a const method for interpolation (why is there not a const version in ROOT???)
        double Interpolate(const TH1* histo, const double x) const;
        double Interpolate(const TH1* histo, const double x, const double y) const;
        double Interpolate(const TH1* histo, const double x, const double y, const double z) const;
        Int_t FindBin(const TAxis* axis, const double x) const;
        
        double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis=1, const int yAxis=2, const int otherDimBin=-1) const;
};

} // end jet namespace


#endif

