/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyHistogram.h"
#include "JetUncertainties/Helpers.h"

#include "TObject.h"
#include "TMath.h"
#include "TFile.h"
#include "TAxis.h"
#include "TH2.h"
#include "TH3.h"
#include "TH3F.h"
#include "TH3D.h"

#include <stdexcept>

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyHistogram::UncertaintyHistogram(const std::string& histName, const Interpolate::TypeEnum interpolate)
    : asg::AsgMessaging(histName)
    , m_isInit(false)
    , m_name(histName.c_str())
    , m_interpolate(interpolate)
    , m_histo(NULL)
    , m_nDim(0)
    , m_cachedProj()
{
    if (histName == "")
        JESUNC_NO_DEFAULT_CONSTRUCTOR;
    ATH_MSG_DEBUG(Form("Creating UncertaintyHistogram named %s",getName().Data()));
}

UncertaintyHistogram::UncertaintyHistogram(const TString histName, const Interpolate::TypeEnum interpolate)
    : UncertaintyHistogram(std::string(histName.Data()),interpolate)
{ }

UncertaintyHistogram::UncertaintyHistogram(const char* histName, const Interpolate::TypeEnum interpolate)
    : UncertaintyHistogram(std::string(histName),interpolate)
{ }

UncertaintyHistogram::UncertaintyHistogram(const UncertaintyHistogram& toCopy)
    : asg::AsgMessaging(Form("%s",toCopy.m_name.Data()))
    , m_isInit(toCopy.m_isInit)
    , m_name(toCopy.m_name)
    , m_interpolate(toCopy.m_interpolate)
    , m_histo(NULL)
    , m_nDim(toCopy.m_nDim)
    , m_cachedProj()
{
    ATH_MSG_DEBUG("Creating copy of UncertaintyHistogram named " << getName().Data());

    if (toCopy.m_histo)
    {
        m_histo = dynamic_cast<TH1*>(toCopy.m_histo->Clone());
        if (!m_histo)
        {
            ATH_MSG_FATAL("Failed to copy uncertainty histogram for " << getName().Data());
            throw std::runtime_error("Failed to copy histogram in UncertaintyHistogram copy constructor");
        }
    }

    if (toCopy.m_cachedProj.size())
        if (cacheProjections().isFailure())
        {
            ATH_MSG_FATAL("Failed to build the required cache for " << getName().Data());
            throw std::runtime_error("Failed to build projection cache in UncertaintyHistogram copy constructor");
        }
}

UncertaintyHistogram::~UncertaintyHistogram()
{
    ATH_MSG_DEBUG(Form("Deleting UncertaintyHistogram named %s",getName().Data()));
    JESUNC_SAFE_DELETE(m_histo);
}

StatusCode UncertaintyHistogram::initialize(TFile* histFile)
{
    // Ensure it wasn't already initialized
    if (m_isInit)
    {
        ATH_MSG_ERROR(Form("UncertaintyHistogram was already initialized: %s",getName().Data()));
        return StatusCode::FAILURE;
    }

    // Ensure we can read from the file
    if (!histFile->IsOpen())
    {
        ATH_MSG_ERROR(Form("Histogram file is not open for reading (%s)",getName().Data()));
        return StatusCode::FAILURE;
    }

    // Find the histogram by name
    TObject* histo = histFile->Get(m_name);
    if (!histo)
    {
        ATH_MSG_ERROR(Form("Histogram file does not contain a histogram named %s",getName().Data()));
        return StatusCode::FAILURE;
    }

    // Ensure the object is a histogram
    m_histo = dynamic_cast<TH1*>(histo);
    if (!m_histo)
    {
        ATH_MSG_ERROR(Form("Histogram file contains the expected key, but it's not a TH1* (%s)",getName().Data()));
        return StatusCode::FAILURE;
    }
    m_histo->SetDirectory(0);

    // Cache dimensionality
    m_nDim = m_histo->GetDimension();

    // Cache the projections if relevant
    if (m_interpolate == Interpolate::OnlyX || m_interpolate == Interpolate::OnlyY)
        if (cacheProjections().isFailure())
            return StatusCode::FAILURE;

    // Print a summary
    ATH_MSG_DEBUG(Form("%s: Found histogram",getName().Data()));

    m_isInit = true;
    return StatusCode::SUCCESS;
}



//////////////////////////////////////////////////
//                                              //
//  Histogram information access                //
//                                              //
//////////////////////////////////////////////////

double UncertaintyHistogram::getValue(const double var1) const
{
    if (m_nDim != 1)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 1D for a %dD histo (%s)",m_nDim,getName().Data()));
        return false;
    }
    return readHisto(var1);
}

double UncertaintyHistogram::getValue(const double var1, const double var2) const
{
    if (m_nDim != 2)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 2D for a %dD histo (%s)",m_nDim,getName().Data()));
        return false;
    }
    return readHisto(var1,var2);
}

double UncertaintyHistogram::getValue(const double var1, const double var2, const double var3) const
{
    if (m_nDim != 3)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 3D for a %dD histo (%s)",m_nDim,getName().Data()));
        return false;
    }
    return readHisto(var1,var2,var3);
}


//////////////////////////////////////////////////
//                                              //
//  Histogram reading helpers                   //
//                                              //
//////////////////////////////////////////////////

double UncertaintyHistogram::readHisto(const double var1, const double var2, const double var3) const
{
    // Ensure the component was initialized
    if (!m_isInit)
    {
        ATH_MSG_ERROR(Form("Component was not initialized (%s)",getName().Data()));
        return JESUNC_ERROR_CODE;
    }

    
    // Ensure that the histogram exists
    if (!m_histo)
    {
        ATH_MSG_ERROR(Form("Histogram to read is NULL (%s)",getName().Data()));
        return JESUNC_ERROR_CODE;
    }

    // Check first dimension boundaries, always applicable
    const float valX = checkBoundaries(m_histo->GetXaxis(),m_histo->GetNbinsX(),var1);
    if (m_nDim == 1)
    {
        switch (m_interpolate)
        {
            case Interpolate::Full:
            case Interpolate::OnlyX:
                return JetHelpers::Interpolate(m_histo,valX);

            case Interpolate::None:
                return m_histo->GetBinContent((m_histo->GetXaxis())->FindBin(valX));

            default:
                ATH_MSG_ERROR("Unsupported histogram interpolation type of \"" << Interpolate::enumToString(m_interpolate).Data() << " for 1D histogram named " << m_name.Data());
                return JESUNC_ERROR_CODE;
        }
    }

    // Check second dimension boundaries, if applicable
    const float valY = checkBoundaries(m_histo->GetYaxis(),m_histo->GetNbinsY(),var2);
    if (m_nDim == 2)
    {
        // We need a 2D histogram for the projection calls
        switch (m_interpolate)
        {
            case Interpolate::Full:
                return JetHelpers::Interpolate(m_histo,valX,valY);

            case Interpolate::OnlyX:
                // Interpolate on the x projection for a given Y bin
                return JetHelpers::Interpolate(m_cachedProj.at(0).at((m_histo->GetYaxis())->FindBin(valY)).get(),valX);

            case Interpolate::OnlyY:
                // Interpolate on the y projection for a given X bin
                return JetHelpers::Interpolate(m_cachedProj.at(0).at((m_histo->GetXaxis())->FindBin(valX)).get(),valY);

            case Interpolate::None:
                return m_histo->GetBinContent((m_histo->GetXaxis())->FindBin(valX),(m_histo->GetYaxis())->FindBin(valY));

            default:
                ATH_MSG_ERROR("Unsupported histogram interpolation type of \"" << Interpolate::enumToString(m_interpolate).Data() << " for 1D histogram named " << m_name.Data());
                return JESUNC_ERROR_CODE;
        }
    }

    // Check third dimension boundaries, if applicable
    const float valZ = checkBoundaries(m_histo->GetZaxis(),m_histo->GetNbinsZ(),var3);
    
    switch (m_interpolate)
    {
        case Interpolate::Full:
            return JetHelpers::Interpolate(m_histo,valX,valY,valZ);

        case Interpolate::OnlyX:
            // Interpolate on the x projection for a given y,z bin
            return JetHelpers::Interpolate(m_cachedProj.at((m_histo->GetYaxis())->FindBin(valY)).at((m_histo->GetZaxis())->FindBin(valZ)).get(),valX);

        case Interpolate::OnlyY:
            // Interpolate on the y projection for a given x,z bin
            return JetHelpers::Interpolate(m_cachedProj.at((m_histo->GetXaxis())->FindBin(valX)).at((m_histo->GetZaxis())->FindBin(valZ)).get(),valY);

        case Interpolate::None:
            return m_histo->GetBinContent((m_histo->GetXaxis())->FindBin(valX),(m_histo->GetYaxis())->FindBin(valY),(m_histo->GetZaxis())->FindBin(valZ));

        default:
            ATH_MSG_ERROR("Unsupported histogram interpolation type of \"" << Interpolate::enumToString(m_interpolate).Data() << " for 1D histogram named " << m_name.Data());
            return JESUNC_ERROR_CODE;
    }
}


double UncertaintyHistogram::checkBoundaries(const TAxis* axis, const int numBins, const double valInput) const
{
    const static int maxNumWarn = 0; //100
    static int       numWarn    = 0;
    
    // Bins are structured for [lowEdge,highEdge)
    // As such, do not need to worry about equality sign for low edge
    // However, do need to check equality sign for high edge
    // High edge is expected and supported (no warning needed)
    float val = valInput;
    const double lowVal  = axis->GetBinLowEdge(1);
    const double highVal = axis->GetBinLowEdge(numBins+1);
    if (val < lowVal || val >= highVal)
    {
        if (val != highVal && ++numWarn < maxNumWarn)
            ATH_MSG_WARNING(Form("Variable value is %f, outside of the axis range of (%f,%f) for %s.  Using closest valid value.  (Only first %d instances printed, this is %d)",val,lowVal,highVal,getName().Data(),maxNumWarn,numWarn));
    
        // Watch for the boundary sign (controls the scale factor)
        if (val < lowVal)
            val = lowVal>0  ? (1.0+1.e-4)*lowVal  : (1.0-1.e-4)*lowVal;
        else
            val = highVal>0 ? (1.0-1.e-4)*highVal : (1.0+1.e-4)*highVal;
    }

    return val;
}

StatusCode UncertaintyHistogram::cacheProjections()
{
    // Ensure the histogram exists
    if (!m_histo)
    {
        ATH_MSG_FATAL("Cannot cache histogram as it doesn't exist: " << m_name.Data());
        return StatusCode::FAILURE;
    }

    // Ensure the number of dimensions is sane
    if (m_histo->GetDimension() < 1 || m_histo->GetDimension() > 3)
    {
        ATH_MSG_FATAL("Unsupported histogram dimensionality for projection caching: " << m_histo->GetDimension());
        return StatusCode::FAILURE;
    }

    // Ensure the cache doesn't already exist
    if (m_cachedProj.size())
    {
        ATH_MSG_FATAL("Cannot cache histogram as the cache is non-empty: " << m_name.Data());
        return StatusCode::FAILURE;
    }

    // Protect vs Interpolate
    switch (m_interpolate)
    {
        case Interpolate::OnlyX:
            // Simple case of 1D
            if (m_histo->GetDimension() == 1)
                return StatusCode::SUCCESS;
            break;

        case Interpolate::OnlyY:
            //  Failure case of 1D
            if (m_histo->GetDimension() == 1)
            {
                ATH_MSG_FATAL("Cannot project in Y for a 1D histogram: " << m_name.Data());
                return StatusCode::FAILURE;
            }
            break;

        default:
            ATH_MSG_FATAL("The interpolation type is not supported for caching: " << m_name.Data());
            return StatusCode::FAILURE;
    }
    
    // If we got here, then the request makes sense
    // Start the projections
    // Intentionally include underflow and overflow bins
    // This keeps the same indexing scheme as root
    // Avoids confusion and problems later at cost of a small amount of RAM
    if (m_histo->GetDimension() == 2)
    {
        // Prepare to cache
        TH2* localHist = dynamic_cast<TH2*>(m_histo);
        m_cachedProj.resize(1); // 2D is a single slice of 3D
        if (!localHist)
        {
            ATH_MSG_FATAL("Failed to convert histogram to a TH2, please check inputs: " << m_name.Data());
            return StatusCode::FAILURE;
        }

        // Create the projections
        if (m_interpolate == Interpolate::OnlyX)
        {
            for (Long64_t binY = 0; binY < localHist->GetNbinsY()+1; ++binY)
            {
                // Single bin of Y, interpolate across X
                m_cachedProj.at(0).emplace_back(localHist->ProjectionX(Form("projx_%lld",binY),binY,binY));
            }
        }
        else if (m_interpolate == Interpolate::OnlyY)
        {
            for (Long64_t binX = 0; binX < localHist->GetNbinsX()+1; ++binX)
            {
                // Single bin of X, interpolate across Y
                m_cachedProj.at(0).emplace_back(localHist->ProjectionY(Form("projy_%lld",binX),binX,binX));
            }
        }
        else
        {
            // We shouldn't make it here due to earlier checks
            ATH_MSG_FATAL("Unexpected interpolation type, somehow escaped earlier checks: " << m_name.Data());
            return StatusCode::FAILURE;
        }
    }
    else if (m_histo->GetDimension() == 3)
    {
        // Prepare to cache
        TH3* localHist = dynamic_cast<TH3*>(m_histo);
        if (!localHist)
        {
            ATH_MSG_FATAL("Failed to convert histogram to a TH3, please check inputs: " << m_name.Data());
            return StatusCode::FAILURE;
        }

        // Create the projections
        if (m_interpolate == Interpolate::OnlyX)
        {
            m_cachedProj.resize(localHist->GetNbinsY()+1); // 3D is a full double-index scan
            for (Long64_t binY = 0; binY < localHist->GetNbinsY()+1; ++binY)
            {
                for (Long64_t binZ = 0; binZ < localHist->GetNbinsZ()+1; ++binZ)
                {
                    // Single bin of Y-Z, interpolate across X
                    m_cachedProj.at(binY).emplace_back(localHist->ProjectionX(Form("projx_%lld_%lld",binY,binZ),binY,binY,binZ,binZ));
                }
            }
        }
        else if (m_interpolate == Interpolate::OnlyY)
        {
            m_cachedProj.resize(localHist->GetNbinsX()+1); // 3D is a full double-index scan
            for (Long64_t binX = 0; binX < localHist->GetNbinsX()+1; ++binX)
            {
                for (Long64_t binZ = 0; binZ < localHist->GetNbinsZ()+1; ++binZ)
                {
                    // Single bin of X-Z, interpolate across Y
                    m_cachedProj.at(binX).emplace_back(localHist->ProjectionY(Form("projy_%lld_%lld",binX,binZ),binX,binX,binZ,binZ));
                }
            }
        }
        else
        {
            // We shouldn't make it here due to earlier checks
            ATH_MSG_FATAL("Unexpected interpolation type, somehow escaped earlier checks: " << m_name.Data());
            return StatusCode::FAILURE;
        }
    }
    else
    {
        // We shouldn't make it here due to earlier checks
        ATH_MSG_FATAL("Unexpected dimensionality: " << m_histo->GetDimension());
        return StatusCode::FAILURE;
    }

    // Ensure that ROOT doesn't try to take posession of any of the projections
    for (size_t index = 0; index < m_cachedProj.size(); ++index)
    {
        for (auto& hist : m_cachedProj.at(index))
        {
            hist->SetDirectory(nullptr);
        }
    }

    // All done
    return StatusCode::SUCCESS;
}

} // end jet namespace

