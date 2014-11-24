/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyHistogram.h"
#include "JetUncertainties/Helpers.h"

#include "TObject.h"
#include "TFile.h"
#include "TH3.h"
#include "TH3F.h"
#include "TH3D.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyHistogram::UncertaintyHistogram(const std::string& histName, const std::string& validHistName, const bool interpolate)
    : asg::AsgMessaging(histName)
    , m_isInit(false)
    , m_name(histName.c_str())
    , m_validName(validHistName.c_str())
    , m_interpolate(interpolate)
    , m_uncHisto(NULL)
    , m_validHisto(NULL)
    , m_nDim(0)
{
    if (histName == "")
        JESUNC_NO_DEFAULT_CONSTRUCTOR;
    ATH_MSG_DEBUG(Form("Creating UncertaintyHistogram named %s",getFullName().Data()));
}

UncertaintyHistogram::UncertaintyHistogram(const TString histName, const TString validHistName, const bool interpolate)
    : UncertaintyHistogram(std::string(histName.Data()),std::string(validHistName.Data()),interpolate)
{ }

//UncertaintyHistogram::UncertaintyHistogram(const char* histName, const char* validHistName, const bool interpolate)
//    : UncertaintyHistogram(std::string(histName),std::string(validHistName!=NULL?validHistName:""),interpolate)
//{ }

UncertaintyHistogram::UncertaintyHistogram(const UncertaintyHistogram& toCopy)
    : asg::AsgMessaging(Form("%s",toCopy.m_name.Data()))
    , m_isInit(toCopy.m_isInit)
    , m_name(toCopy.m_name)
    , m_validName(toCopy.m_validName)
    , m_interpolate(toCopy.m_interpolate)
    , m_uncHisto(NULL)
    , m_validHisto(NULL)
    , m_nDim(toCopy.m_nDim)
{
    ATH_MSG_DEBUG("Creating copy of UncertaintyHistogram named " << getFullName().Data());

    if (toCopy.m_uncHisto)
    {
        m_uncHisto = dynamic_cast<TH1*>(toCopy.m_uncHisto->Clone());
        if (!m_uncHisto)
            ATH_MSG_FATAL("Failed to copy uncertainty histogram for " << getFullName().Data());
    }
    if (toCopy.m_validHisto)
    {
        m_validHisto = dynamic_cast<TH1*>(toCopy.m_validHisto->Clone());
        if (!m_validHisto)
            ATH_MSG_FATAL("Failed to copy validity histogram for " << getFullName().Data());
    }
}

UncertaintyHistogram::~UncertaintyHistogram()
{
    ATH_MSG_DEBUG(Form("Deleting UncertaintyHistogram named %s",getFullName().Data()));
    JESUNC_SAFE_DELETE(m_uncHisto);
    JESUNC_SAFE_DELETE(m_validHisto);
}

StatusCode UncertaintyHistogram::Initialize(TFile* histFile)
{
    // Ensure it wasn't already initialized
    if (m_isInit)
    {
        ATH_MSG_ERROR(Form("UncertaintyComponent was already initialized: %s",getFullName().Data()));
        return StatusCode::FAILURE;
    }

    // Ensure we can read from the file
    if (!histFile->IsOpen())
    {
        ATH_MSG_ERROR(Form("Histogram file is not open for reading (%s)",getFullName().Data()));
        return StatusCode::FAILURE;
    }

    // Find the histogram by name
    TObject* uncHisto = histFile->Get(m_name);
    if (!uncHisto)
    {
        ATH_MSG_ERROR(Form("Histogram file does not contain a histogram named %s",getFullName().Data()));
        return StatusCode::FAILURE;
    }

    // Ensure the object is a histogram
    m_uncHisto = dynamic_cast<TH1*>(uncHisto);
    if (!m_uncHisto)
    {
        ATH_MSG_ERROR(Form("Histogram file contains the expected key, but it's not a TH1* (%s)",getFullName().Data()));
        return StatusCode::FAILURE;
    }
    else
        m_uncHisto->SetDirectory(0);

    // Look for a validity histogram
    TObject* validHisto = histFile->Get(m_validName == "" ? "valid_" + m_name : m_validName);
    if (validHisto)
    {
        // Set the default name as we found an object
        //if (m_validName == "") m_validName = "valid_" + m_name;
        
        // Now check if it's a histogram
        m_validHisto = dynamic_cast<TH1*>(validHisto);
        if (!m_validHisto)
        {
            ATH_MSG_ERROR(Form("Histogram file contains validity key, but it's not a TH* (%s)",getFullName().Data()));
            return StatusCode::FAILURE;
        }
        else
            // Cache it so it won't disappear when we close the TFile
            m_validHisto->SetDirectory(0);
    }

    // Check dimensionality
    if (m_uncHisto)
        m_nDim = m_uncHisto->GetDimension();
    if (m_validHisto && m_validHisto->GetDimension() != m_nDim)
    {
        ATH_MSG_ERROR(Form("Got %d dimensions for the uncertainty histogram, but %d for the validity histogram (%s)",m_nDim,m_validHisto->GetDimension(),getFullName().Data()));
        return StatusCode::FAILURE;
    }

    // Print a summary
    if (m_uncHisto && m_validHisto)
        ATH_MSG_DEBUG(Form("%s: Found both uncertainty and validity histograms",getFullName().Data()));
    else if (m_uncHisto)
        ATH_MSG_DEBUG(Form("%s: Found uncertainty histogram",getFullName().Data()));
    else
    {
        ATH_MSG_ERROR(Form("%s: Failed to find uncertainty histogram",getFullName().Data()));
        return StatusCode::FAILURE;
    }

    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Generic helper methods                      //
//                                              //
//////////////////////////////////////////////////

TString UncertaintyHistogram::getFullName() const
{
    return TString(m_validName == "" ? m_name.Data() : Form("%s|%s",m_name.Data(),m_validName.Data()));
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

// Validity retrieval
bool UncertaintyHistogram::getValidity(const float var1) const
{
    if (m_nDim != 1)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 1D for a %dD histo (%s)",m_nDim,getFullName().Data()));
        return false;
    }

    return readValidityHisto(var1);
}

bool UncertaintyHistogram::getValidity(const float var1, const float var2) const
{
    if (m_nDim != 2)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 2D for a %dD histo (%s)",m_nDim,getFullName().Data()));
        return false;
    }

    return readValidityHisto(var1,var2);
}

bool UncertaintyHistogram::getValidity(const float var1, const float var2, const float var3) const
{
    if (m_nDim != 3)
    {
        ATH_MSG_ERROR(Form("Wrong dimensionality - asked for 3D for a %dD histo (%s)",m_nDim,getFullName().Data()));
        return false;
    }
    
    return readValidityHisto(var1,var2,var3);
}

// Uncertainty retrieval (crash if not valid)
double UncertaintyHistogram::getUncertainty(const float var1) const
{
    double unc = -1;
    bool success = getValidUncertainty(unc,var1);
    if (!success)
        ATH_MSG_ERROR(Form("Uncertainty is outside of validity region (%s[%f])",getFullName().Data(),var1));
    return unc;
}

double UncertaintyHistogram::getUncertainty(const float var1, const float var2) const
{
    double unc = -1;
    bool success = getValidUncertainty(unc,var1,var2);
    if (!success)
        ATH_MSG_ERROR(Form("Uncertainty is outside of validity region (%s[%f,%f])",getFullName().Data(),var1,var2));
    return unc;
}

double UncertaintyHistogram::getUncertainty(const float var1, const float var2, const float var3) const
{
    double unc = -1;
    bool success = getValidUncertainty(unc,var1,var2,var3);
    if (!success)
        ATH_MSG_ERROR(Form("Uncertainty is outside of validity region (%s[%f,%f,%f])",getFullName().Data(),var1,var2,var3));
    return unc;
}

// Uncertainty retrieval (unc only set if valid, returns true if valid)
bool UncertaintyHistogram::getValidUncertainty(double& unc, const float var1) const
{
    bool valid = getValidity(var1);
    if (valid)
        unc = readUncertaintyHisto(var1);
    return valid;
}

bool UncertaintyHistogram::getValidUncertainty(double& unc, const float var1, const float var2) const
{
    bool valid = getValidity(var1,var2);
    if (valid)
        unc = readUncertaintyHisto(var1,var2);
    return valid;
}

bool UncertaintyHistogram::getValidUncertainty(double& unc, const float var1, const float var2, const float var3) const
{
    bool valid = getValidity(var1,var2,var3);
    if (valid)
        unc = readUncertaintyHisto(var1,var2,var3);
    return valid;
}


//////////////////////////////////////////////////
//                                              //
//  Histogram reading helpers                   //
//                                              //
//////////////////////////////////////////////////

double UncertaintyHistogram::readHisto(const TH1* histo, const float var1, const float var2, const float var3, const bool interpolate) const
{
    static int numWarn   = 0;
    const int maxNumWarn = 100;
    
    // Ensure the component was initialized
    if (!m_isInit)
    {
        ATH_MSG_ERROR(Form("Component was not initialized (%s)",getFullName().Data()));
        return JESUNC_ERROR_CODE;
    }

    
    // Ensure that the histogram exists
    if (!histo)
    {
        ATH_MSG_ERROR(Form("Histogram to read is NULL (%s)",getFullName().Data()));
        return JESUNC_ERROR_CODE;
    }

    // First dimension, always applicable
    const double lowX  = histo->GetXaxis()->GetBinLowEdge(1);
    const double highX = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
    float valX = var1;
    if (valX < lowX || valX >= highX)
    {
        if (++numWarn < maxNumWarn)
            ATH_MSG_WARNING(Form("First variable value is %f, outside of the x axis range of (%f,%f) for %s.  Using closest valid value.  (Only first %d instances printed, this is %d)",valX,lowX,highX,getFullName().Data(),maxNumWarn,numWarn+1));

        if (valX < lowX)
            valX = lowX;
        else
            valX = (1.0-1.e-4)*highX;
    }

    if (m_nDim == 1)
    {
        if (interpolate) return Interpolate(histo,valX);
        else             return histo->GetBinContent(FindBin(histo->GetXaxis(),valX));
    }

    // Second dimension, if applicable
    const double lowY  = histo->GetYaxis()->GetBinLowEdge(1);
    const double highY = histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1);
    float valY = var2;
    if (valY < lowY || valY >= highY)
    {
        if (++numWarn < maxNumWarn)
            ATH_MSG_WARNING(Form("Second variable value is %f, outside of the y axis range of (%f,%f) for %s.  Using closest valid value.  (Only first %d instances printed, this is %d)",valY,lowY,highY,getFullName().Data(),maxNumWarn,numWarn+1));

        if (valY < lowY)
            valY = lowY;
        else
            valY = (1.0-1.e-4)*highY;
    }

    if (m_nDim == 2)
    {
        if (interpolate) return Interpolate(histo,valX,valY);
        else             return histo->GetBinContent(FindBin(histo->GetXaxis(),valX),FindBin(histo->GetYaxis(),valY));
    }

    // Third dimension, if applicable
    const double lowZ  = histo->GetZaxis()->GetBinLowEdge(1);
    const double highZ = histo->GetZaxis()->GetBinLowEdge(histo->GetNbinsZ()+1);
    float valZ = var3;
    if (valZ < lowZ || valZ >= highZ)
    {
        if (++numWarn < maxNumWarn)
            ATH_MSG_WARNING(Form("Third variable value is %f, outside of the z axis range of (%f,%f) for %s.  Using closest valid value.  (Only first %d instances printed, this is %d)",valZ,lowZ,highZ,getFullName().Data(),maxNumWarn,numWarn+1));

        if (valZ < lowZ)
            valZ = lowZ;
        else
            valZ = (1.0-1.e-4)*highZ;
    }

    if (interpolate) return Interpolate(histo,valX,valY,valZ);
    return histo->GetBinContent(FindBin(histo->GetXaxis(),valX),FindBin(histo->GetYaxis(),valY),FindBin(histo->GetZaxis(),valZ));
}

bool UncertaintyHistogram::readValidityHisto(const float var1, const float var2, const float var3) const
{
    // If the validity histogram doesn't exist, it's always valid
    if (!m_validHisto)
        return true;
    
    // Don't interpolate!  Validity is true or false, not something in between
    double validity = readHisto(m_validHisto,var1,var2,var3,false);
    if (fabs(validity-1.0) < 1.0e-3)
        return true;
    else if (fabs(validity) < 1.0e-3)
        return false;

    ATH_MSG_ERROR(Form("Validity histogram value is not of the expected form, got %f (%s)",validity,getFullName().Data()));
    return false;
}

double UncertaintyHistogram::readUncertaintyHisto(const float var1, const float var2, const float var3) const
{
    return readHisto(m_uncHisto,var1,var2,var3,m_interpolate);
}


double UncertaintyHistogram::Interpolate(const TH1* histo, const double x) const
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    // Probable reason is that FindBin isn't const, but there should be a const version...
    Int_t xbin = FindBin(histo->GetXaxis(),x);
    Double_t x0,x1,y0,y1;
    
    if(x<=histo->GetBinCenter(1)) {
       return histo->GetBinContent(1);
    } else if(x>=histo->GetBinCenter(histo->GetNbinsX())) {
       return histo->GetBinContent(histo->GetNbinsX());
    } else {
       if(x<=histo->GetBinCenter(xbin)) {
          y0 = histo->GetBinContent(xbin-1);
          x0 = histo->GetBinCenter(xbin-1);
          y1 = histo->GetBinContent(xbin);
          x1 = histo->GetBinCenter(xbin);
       } else {
          y0 = histo->GetBinContent(xbin);
          x0 = histo->GetBinCenter(xbin);
          y1 = histo->GetBinContent(xbin+1);
          x1 = histo->GetBinCenter(xbin+1);
       }
    return y0 + (x-x0)*((y1-y0)/(x1-x0));
    }
}

double UncertaintyHistogram::Interpolate(const TH1* histo, const double x, const double y) const
{
    // Call the unified method for consistency
    return Interpolate2D(histo,x,y);
    
    /*
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    // Probable reason is that FindBin isn't const, but there should be a const version...
    const TAxis* fXaxis = histo->GetXaxis();
    const TAxis* fYaxis = histo->GetYaxis();

    Double_t f=0;
    Double_t x1=0,x2=0,y1=0,y2=0;
    Double_t dx,dy;
    Int_t bin_x = FindBin(fXaxis,x);
    Int_t bin_y = FindBin(fYaxis,y);
    if(bin_x<1 || bin_x>histo->GetNbinsX() || bin_y<1 || bin_y>histo->GetNbinsY()) {
       Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(histo->GetNbinsX()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(histo->GetNbinsY()+1));
       return 0;
    }
    Int_t quadrant = 0; // CCW from UR 1,2,3,4
    // which quadrant of the bin (bin_P) are we in?
    dx = fXaxis->GetBinUpEdge(bin_x)-x;
    dy = fYaxis->GetBinUpEdge(bin_y)-y;
    if (dx<=fXaxis->GetBinWidth(bin_x)/2 && dy<=fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 1; // upper right
    if (dx>fXaxis->GetBinWidth(bin_x)/2 && dy<=fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 2; // upper left
    if (dx>fXaxis->GetBinWidth(bin_x)/2 && dy>fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 3; // lower left
    if (dx<=fXaxis->GetBinWidth(bin_x)/2 && dy>fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 4; // lower right
    switch(quadrant) {
    case 1:
       x1 = fXaxis->GetBinCenter(bin_x);
       y1 = fYaxis->GetBinCenter(bin_y);
       x2 = fXaxis->GetBinCenter(bin_x+1);
       y2 = fYaxis->GetBinCenter(bin_y+1);
       break;
    case 2:
       x1 = fXaxis->GetBinCenter(bin_x-1);
       y1 = fYaxis->GetBinCenter(bin_y);
       x2 = fXaxis->GetBinCenter(bin_x);
       y2 = fYaxis->GetBinCenter(bin_y+1);
       break;
    case 3:
       x1 = fXaxis->GetBinCenter(bin_x-1);
       y1 = fYaxis->GetBinCenter(bin_y-1);
       x2 = fXaxis->GetBinCenter(bin_x);
       y2 = fYaxis->GetBinCenter(bin_y);
       break;
    case 4:
       x1 = fXaxis->GetBinCenter(bin_x);
       y1 = fYaxis->GetBinCenter(bin_y-1);
       x2 = fXaxis->GetBinCenter(bin_x+1);
       y2 = fYaxis->GetBinCenter(bin_y);
       break;
    }
    Int_t bin_x1 = FindBin(fXaxis,x1);
    if(bin_x1<1) bin_x1=1;
    Int_t bin_x2 = FindBin(fXaxis,x2);
    if(bin_x2>histo->GetNbinsX()) bin_x2=histo->GetNbinsX();
    Int_t bin_y1 = FindBin(fYaxis,y1);
    if(bin_y1<1) bin_y1=1;
    Int_t bin_y2 = FindBin(fYaxis,y2);
    if(bin_y2>histo->GetNbinsY()) bin_y2=histo->GetNbinsY();
    Int_t bin_q22 = histo->GetBin(bin_x2,bin_y2);
    Int_t bin_q12 = histo->GetBin(bin_x1,bin_y2);
    Int_t bin_q11 = histo->GetBin(bin_x1,bin_y1);
    Int_t bin_q21 = histo->GetBin(bin_x2,bin_y1);
    Double_t q11 = histo->GetBinContent(bin_q11);
    Double_t q12 = histo->GetBinContent(bin_q12);
    Double_t q21 = histo->GetBinContent(bin_q21);
    Double_t q22 = histo->GetBinContent(bin_q22);
    Double_t d = 1.0*(x2-x1)*(y2-y1);
    f = 1.0*q11/d*(x2-x)*(y2-y)+1.0*q21/d*(x-x1)*(y2-y)+1.0*q12/d*(x2-x)*(y-y1)+1.0*q22/d*(x-x1)*(y-y1);
    return f;
    */
}

double UncertaintyHistogram::Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis, const int yAxis, const int otherDimBin) const
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    // Probable reason is that FindBin isn't const, but there should be a const version...
    const TAxis* fXaxis = xAxis == 1 ? histo->GetXaxis() : xAxis == 2 ? histo->GetYaxis() : xAxis == 3 ? histo->GetZaxis() : NULL;
    const TAxis* fYaxis = yAxis == 1 ? histo->GetXaxis() : yAxis == 2 ? histo->GetYaxis() : yAxis == 3 ? histo->GetZaxis() : NULL;

    if (!fXaxis || !fYaxis)
    {
        Error("Interpolate2D","Failed to parse axes from inputs");
        return 0;
    }

    Double_t f=0;
    Double_t x1=0,x2=0,y1=0,y2=0;
    Double_t dx,dy;
    Int_t bin_x = FindBin(fXaxis,x);
    Int_t bin_y = FindBin(fYaxis,y);
    if(bin_x<1 || bin_x>fXaxis->GetNbins() || bin_y<1 || bin_y>fYaxis->GetNbins()) {
       Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(fXaxis->GetNbins()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(fYaxis->GetNbins()+1));
       return 0;
    }
    Int_t quadrant = 0; // CCW from UR 1,2,3,4
    // which quadrant of the bin (bin_P) are we in?
    dx = fXaxis->GetBinUpEdge(bin_x)-x;
    dy = fYaxis->GetBinUpEdge(bin_y)-y;
    if (dx<=fXaxis->GetBinWidth(bin_x)/2 && dy<=fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 1; // upper right
    if (dx>fXaxis->GetBinWidth(bin_x)/2 && dy<=fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 2; // upper left
    if (dx>fXaxis->GetBinWidth(bin_x)/2 && dy>fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 3; // lower left
    if (dx<=fXaxis->GetBinWidth(bin_x)/2 && dy>fYaxis->GetBinWidth(bin_y)/2)
    quadrant = 4; // lower right
    switch(quadrant) {
    case 1:
       x1 = fXaxis->GetBinCenter(bin_x);
       y1 = fYaxis->GetBinCenter(bin_y);
       x2 = fXaxis->GetBinCenter(bin_x+1);
       y2 = fYaxis->GetBinCenter(bin_y+1);
       break;
    case 2:
       x1 = fXaxis->GetBinCenter(bin_x-1);
       y1 = fYaxis->GetBinCenter(bin_y);
       x2 = fXaxis->GetBinCenter(bin_x);
       y2 = fYaxis->GetBinCenter(bin_y+1);
       break;
    case 3:
       x1 = fXaxis->GetBinCenter(bin_x-1);
       y1 = fYaxis->GetBinCenter(bin_y-1);
       x2 = fXaxis->GetBinCenter(bin_x);
       y2 = fYaxis->GetBinCenter(bin_y);
       break;
    case 4:
       x1 = fXaxis->GetBinCenter(bin_x);
       y1 = fYaxis->GetBinCenter(bin_y-1);
       x2 = fXaxis->GetBinCenter(bin_x+1);
       y2 = fYaxis->GetBinCenter(bin_y);
       break;
    }
    Int_t bin_x1 = FindBin(fXaxis,x1);
    if(bin_x1<1) bin_x1=1;
    Int_t bin_x2 = FindBin(fXaxis,x2);
    if(bin_x2>fXaxis->GetNbins()) bin_x2=fXaxis->GetNbins();
    Int_t bin_y1 = FindBin(fYaxis,y1);
    if(bin_y1<1) bin_y1=1;
    Int_t bin_y2 = FindBin(fYaxis,y2);
    if(bin_y2>fYaxis->GetNbins()) bin_y2=fYaxis->GetNbins();

    Double_t q11;
    Double_t q12;
    Double_t q21;
    Double_t q22;
    if (otherDimBin > 0)
    {
        // X,Y variable and Z fixed
        if (xAxis == 1 && yAxis == 2)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_x1,bin_y1,otherDimBin));
            q12 = histo->GetBinContent(histo->GetBin(bin_x1,bin_y2,otherDimBin));
            q21 = histo->GetBinContent(histo->GetBin(bin_x2,bin_y1,otherDimBin));
            q22 = histo->GetBinContent(histo->GetBin(bin_x2,bin_y2,otherDimBin));
        }
        // X,Z variable and Y fixed
        else if (xAxis == 1 && yAxis == 3)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_x1,otherDimBin,bin_y1));
            q12 = histo->GetBinContent(histo->GetBin(bin_x1,otherDimBin,bin_y2));
            q21 = histo->GetBinContent(histo->GetBin(bin_x2,otherDimBin,bin_y1));
            q22 = histo->GetBinContent(histo->GetBin(bin_x2,otherDimBin,bin_y2));
        }
        // Y,Z variable and X fixed
        else if (xAxis == 2 && yAxis == 3)
        {
            q11 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_x1,bin_y1));
            q12 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_x1,bin_y2));
            q21 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_x2,bin_y1));
            q22 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_x2,bin_y2));
        }
        // Y,X variable and Z fixed
        else if (xAxis == 2 && yAxis == 1)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_y1,bin_x1,otherDimBin));
            q12 = histo->GetBinContent(histo->GetBin(bin_y1,bin_x2,otherDimBin));
            q21 = histo->GetBinContent(histo->GetBin(bin_y2,bin_x1,otherDimBin));
            q22 = histo->GetBinContent(histo->GetBin(bin_y2,bin_x2,otherDimBin));
        }
        // Z,X variable and Y fixed
        else if (xAxis == 3 && yAxis == 1)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_y1,otherDimBin,bin_x1));
            q12 = histo->GetBinContent(histo->GetBin(bin_y1,otherDimBin,bin_x2));
            q21 = histo->GetBinContent(histo->GetBin(bin_y2,otherDimBin,bin_x1));
            q22 = histo->GetBinContent(histo->GetBin(bin_y2,otherDimBin,bin_x2));
        }
        // Z,Y variable and X fixed
        else if (xAxis == 3 && yAxis == 2)
        {
            q11 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_y1,bin_x1));
            q12 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_y1,bin_x2));
            q21 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_y2,bin_x1));
            q22 = histo->GetBinContent(histo->GetBin(otherDimBin,bin_y2,bin_x2));
        }
        else
        {
            Error("Interpolate2D","Unsupported axis combination: (x,y)=(%d,%d) with one bin fixed",xAxis,yAxis);
            return 0;
        }
    }
    else
    {
        // X,Y variable, no Z
        if (xAxis == 1 && yAxis == 2)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_x1,bin_y1));
            q12 = histo->GetBinContent(histo->GetBin(bin_x1,bin_y2));
            q21 = histo->GetBinContent(histo->GetBin(bin_x2,bin_y1));
            q22 = histo->GetBinContent(histo->GetBin(bin_x2,bin_y2));
        }
        // Y,X variable, no Z
        else if (xAxis == 2 && yAxis == 1)
        {
            q11 = histo->GetBinContent(histo->GetBin(bin_y1,bin_x1));
            q12 = histo->GetBinContent(histo->GetBin(bin_y1,bin_x2));
            q21 = histo->GetBinContent(histo->GetBin(bin_y2,bin_x1));
            q22 = histo->GetBinContent(histo->GetBin(bin_y2,bin_x2));
        }
        else
        {
            Error("Interpolate2D","Unsupported axis combination: (x,y)=(%d,%d)",xAxis,yAxis);
            return 0;
        }
    }
    
    Double_t d = 1.0*(x2-x1)*(y2-y1);
    f = 1.0*q11/d*(x2-x)*(y2-y)+1.0*q21/d*(x-x1)*(y2-y)+1.0*q12/d*(x2-x)*(y-y1)+1.0*q22/d*(x-x1)*(y-y1);
    return f;
}

double UncertaintyHistogram::Interpolate(const TH1* histo, const double x, const double y, const double z) const
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    // Probable reason is that FindBin isn't const, but there should be a const version...
    const TAxis* fXaxis = histo->GetXaxis();
    const TAxis* fYaxis = histo->GetYaxis();
    const TAxis* fZaxis = histo->GetZaxis();

    // Does not work for edge cases!
    //Int_t ubx = FindBin(fXaxis,x);
    //if ( x < fXaxis->GetBinCenter(ubx) ) ubx -= 1;
    //Int_t obx = ubx + 1;
    //
    //Int_t uby = FindBin(fYaxis,y);
    //if ( y < fYaxis->GetBinCenter(uby) ) uby -= 1;
    //Int_t oby = uby + 1;
    //
    //Int_t ubz = FindBin(fZaxis,z);
    //if ( z < fZaxis->GetBinCenter(ubz) ) ubz -= 1;
    //Int_t obz = ubz + 1;
    //
    //printf("ubx,obx = %d,%d | uby,oby = %d,%d | ubz,obz = %d,%d\n",ubx,obx,uby,oby,ubz,obz);
    //printf("max x = %d, y = %d, z = %d\n",fXaxis->GetNbins(),fYaxis->GetNbins(),fZaxis->GetNbins());

    //if (ubx <=0 || uby <=0 || ubz <= 0 ||
    //    obx > fXaxis->GetNbins() || oby > fYaxis->GetNbins() || obz > fZaxis->GetNbins() ) {
    //   Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f], z: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(histo->GetNbinsX()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(histo->GetNbinsY()+1),z,fZaxis->GetBinLowEdge(1),fZaxis->GetBinLowEdge(histo->GetNbinsZ()+1));
    //   return 0;
    //}

    // Find the bin by bin edges
    Int_t ubx = FindBin(fXaxis,x);
    Int_t uby = FindBin(fYaxis,y);
    Int_t ubz = FindBin(fZaxis,z);
    
    // Check if the value(s) are outside of the bin range(s)
    if ( ubx < 1 || ubx > histo->GetNbinsX() || uby < 1 || uby > histo->GetNbinsY() || ubz < 1 || ubz > histo->GetNbinsZ() )
    {
       Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f], z: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(histo->GetNbinsX()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(histo->GetNbinsY()+1),z,fZaxis->GetBinLowEdge(1),fZaxis->GetBinLowEdge(histo->GetNbinsZ()+1));
       return 0;
    }

    // Now switch from bin edges to bin centres
    // Note that we want to support edge cases, so it is possible that ub* == ob*
    // This functionality is not in original ROOT TH3::Interpolate()
    // This functionality is inspired by TH2::Interpolate()
    Int_t obx = ubx + 1;
    Int_t oby = uby + 1;
    Int_t obz = ubz + 1;
    
    // Calculate distance weights before checking under/overflow bins
    Double_t xw = fXaxis->GetBinCenter(obx) - fXaxis->GetBinCenter(ubx);
    Double_t yw = fYaxis->GetBinCenter(oby) - fYaxis->GetBinCenter(uby);
    Double_t zw = fZaxis->GetBinCenter(obz) - fZaxis->GetBinCenter(ubz);
    
    if (x < fXaxis->GetBinCenter(ubx)) { ubx -= 1; obx -= 1; }
    if (ubx < 1) ubx = 1;
    if (obx > histo->GetNbinsX()) obx = histo->GetNbinsX();

    if (y < fYaxis->GetBinCenter(uby)) { uby -= 1; oby -= 1; }
    if (uby < 1) uby = 1;
    if (oby > histo->GetNbinsY()) oby = histo->GetNbinsY();

    if (z < fZaxis->GetBinCenter(ubz)) { ubz -= 1; obz -= 1; }
    if (ubz < 1) ubz = 1;
    if (obz > histo->GetNbinsZ()) obz = histo->GetNbinsZ();

    // Edge cases were tried with weights set including the under/overflow bins (to follow what TH2::Interpolate() does for boundaries)
    // In some cases, it performed quite poorly
    // Tests of switching to 2D interpolation with the third dimension fixed appeared to work much better
    // Thus, the below is now a switch to bilinear interpolation when bin(s) are equal in trilinear interpolation
    if (ubx == obx || uby == oby || ubz == obz)
    {
        // Bilinear interpolation
        if (ubz == obz)
            return Interpolate2D(histo,x,y,1,2,ubz);
        else if (uby == oby)
            return Interpolate2D(histo,x,z,1,3,uby);
        else if (ubx == obx)
            return Interpolate2D(histo,y,z,2,3,ubx);
        
        // Below method also works, but is *much* more slow
        
        /*
        // Get a modifiable copy of the histogram
        TH3* input = dynamic_cast<TH3*>(histo->Clone());
        if (!input)
        {
            Error("Interpolate","Failed to get a modifiable copy of input histo (boundary case)");
            return 0;
        }

        // Change the axis range to the bin in question and project
        // Note that projection axes are ordered backward for some reason
        TH1* projection = NULL;
        if (ubz == obz)
        {
            input->GetZaxis()->SetRange(ubz,ubz);
            projection = input->Project3D("yx");
        }
        else if (uby == oby)
        {
            input->GetYaxis()->SetRange(uby,uby);
            projection = input->Project3D("zx");
        }
        else if (ubx == obx)
        {
            input->GetXaxis()->SetRange(ubx,ubx);
            projection = input->Project3D("zy");
        }
        if (!projection)
        {
            Error("Interpolate","Failed to get a projection in the %s direction (boundary case)",ubz==obz?"XY":uby==oby?"XZ":ubx==obx?"YZ":"UNKNOWN");
            return 0;
        }

        double result = 0;
        if (ubz == obz)
            result = Interpolate(projection,x,y);
        else if (uby == oby)
            result = Interpolate(projection,x,z);
        else if (ubx == obx)
            result = Interpolate(projection,y,z);

        delete projection;
        delete input;
        return result;
        */
    }
    
    // Not a boundary case, resume normal ROOT::TH3::Interpolate()
    Double_t xd = (x - fXaxis->GetBinCenter(ubx)) / xw;
    Double_t yd = (y - fYaxis->GetBinCenter(uby)) / yw;
    Double_t zd = (z - fZaxis->GetBinCenter(ubz)) / zw;
    
    
    Double_t v[] = { histo->GetBinContent( ubx, uby, ubz ), histo->GetBinContent( ubx, uby, obz ),
                     histo->GetBinContent( ubx, oby, ubz ), histo->GetBinContent( ubx, oby, obz ),
                     histo->GetBinContent( obx, uby, ubz ), histo->GetBinContent( obx, uby, obz ),
                     histo->GetBinContent( obx, oby, ubz ), histo->GetBinContent( obx, oby, obz ) };
    
    
    Double_t i1 = v[0] * (1 - zd) + v[1] * zd;
    Double_t i2 = v[2] * (1 - zd) + v[3] * zd;
    Double_t j1 = v[4] * (1 - zd) + v[5] * zd;
    Double_t j2 = v[6] * (1 - zd) + v[7] * zd;
    
    
    Double_t w1 = i1 * (1 - yd) + i2 * yd;
    Double_t w2 = j1 * (1 - yd) + j2 * yd;
    
    
    Double_t result = w1 * (1 - xd) + w2 * xd;
    
    return result;
}

Int_t UncertaintyHistogram::FindBin(const TAxis* axis, const double x) const
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TAxis FindBin method
    // This is done because I want a const version of bin finding (no expanding on under/overflow)
    const double fXmin = axis->GetXmin();
    const double fXmax = axis->GetXmax();
    const Int_t fNbins = axis->GetNbins();
    const TArrayD* fXbins = axis->GetXbins();
    Int_t bin;
    if (x < fXmin) {              //*-* underflow
       bin = 0;
    } else  if ( !(x < fXmax)) {     //*-* overflow  (note the way to catch NaN
       bin = fNbins+1;
    } else {
       if (!fXbins->fN) {        //*-* fix bins
          bin = 1 + int (fNbins*(x-fXmin)/(fXmax-fXmin) );
       } else {                  //*-* variable bin sizes
          //for (bin =1; x >= fXbins->fArray[bin]; bin++);
          bin = 1 + TMath::BinarySearch(fXbins->fN,fXbins->fArray,x);
       }
    }
    return bin;
}

} // end jet namespace

