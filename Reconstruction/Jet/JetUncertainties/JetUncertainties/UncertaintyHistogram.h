/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H
#define JETUNCERTAINTIES_UNCERTAINTYHISTOGRAM_H

#include "AsgTools/AsgMessaging.h"

#include "JetUncertainties/UncertaintyEnum.h"

#include "JetUncertainties/JetHelpers.h"

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
        UncertaintyHistogram(const std::string& histName, const Interpolate::TypeEnum interpolate);
        UncertaintyHistogram(const TString histName, const Interpolate::TypeEnum interpolate);
        UncertaintyHistogram(const char* histName, const Interpolate::TypeEnum interpolate);
        UncertaintyHistogram(const UncertaintyHistogram& toCopy);
        virtual ~UncertaintyHistogram();
        virtual StatusCode initialize(TFile* histFile);

        // Member retrieval methods
        const TString&        getName()   const { return m_name;        }
        const TH1*            getHisto()  const { return m_histo;       }
        Interpolate::TypeEnum getInterp() const { return m_interpolate; }
        int                   getNumDim() const { return m_nDim;        }

        // Histogram information access
        double getValue(const double var1) const;
        double getValue(const double var1, const double var2) const;
        double getValue(const double var1, const double var2, const double var3) const;

    private:
        // Private members
        bool m_isInit;
        const TString m_name;
        const Interpolate::TypeEnum m_interpolate;
        TH1* m_histo;
        int  m_nDim;

        // Cache projections in case of 1-D interpolation in a 2-D or 3-D histogram
        // For a 3-D histogram, it's [y][z] or [x][z] or [x][y] as appropriate
        std::vector< std::vector< std::unique_ptr<TH1> > > m_cachedProj;
        StatusCode cacheProjections();

        // Histogram reading helpers
        double readHisto(const double var1, const double var2=0, const double var3=0) const;
        double checkBoundaries(const TAxis* axis, const int numBins, const double valInput) const;

};

} // end jet namespace


#endif

