/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/JetHelpers.h"

#include "TH1.h"
#include <TMath.h>
#include "TAxis.h"

//We move all the Histogram reading helpers from "JetUncertainties/UncertaintyHistogram.h" here because we want to use them in the FFJetSmearingTool too

//Helpers to interpolate

double JetHelpers::Interpolate(const TH1* histo, const double x)
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    int xbin = (histo->GetXaxis())->FindBin(x);
    double x0,x1,y0,y1;
    
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


double JetHelpers::Interpolate(const TH1* histo, const double x, const double y)
{
    // Call the unified method for consistency
    return Interpolate2D(histo,x,y);
}

double JetHelpers::Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis, const int yAxis, const int otherDimBin)
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    const TAxis* fXaxis = xAxis == 1 ? histo->GetXaxis() : xAxis == 2 ? histo->GetYaxis() : xAxis == 3 ? histo->GetZaxis() : nullptr;
    const TAxis* fYaxis = yAxis == 1 ? histo->GetXaxis() : yAxis == 2 ? histo->GetYaxis() : yAxis == 3 ? histo->GetZaxis() : nullptr;

    if (!fXaxis || !fYaxis || histo->GetDimension() != 3)
    {
        histo->Error("Interpolate2D","Failed to parse axes from inputs");
        return 0;
    }

    double f=0;
    double x1=0,x2=0,y1=0,y2=0;
    double dx,dy;
    int bin_x = fXaxis->FindBin(x);
    int bin_y = fYaxis->FindBin(y);
    if(bin_x<1 || bin_x>fXaxis->GetNbins() || bin_y<1 || bin_y>fYaxis->GetNbins()) {
       histo->Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(fXaxis->GetNbins()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(fYaxis->GetNbins()+1));
       return 0;
    }
    int quadrant = 0; // CCW from UR 1,2,3,4
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
    int bin_x1 = fXaxis->FindBin(x1);
    if(bin_x1<1) bin_x1=1;
    int bin_x2 = fXaxis->FindBin(x2);
    if(bin_x2>fXaxis->GetNbins()) bin_x2=fXaxis->GetNbins();
    int bin_y1 = fYaxis->FindBin(y1);
    if(bin_y1<1) bin_y1=1;
    int bin_y2 = fYaxis->FindBin(y2);
    if(bin_y2>fYaxis->GetNbins()) bin_y2=fYaxis->GetNbins();

    double q11;
    double q12;
    double q21;
    double q22;
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
            histo->Error("Interpolate2D","Unsupported axis combination: (x,y)=(%d,%d) with one bin fixed",xAxis,yAxis);
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
            histo->Error("Interpolate2D","Unsupported axis combination: (x,y)=(%d,%d)",xAxis,yAxis);
            return 0;
        }
    }
    
    double d = 1.0*(x2-x1)*(y2-y1);
    f = 1.0*q11/d*(x2-x)*(y2-y)+1.0*q21/d*(x-x1)*(y2-y)+1.0*q12/d*(x2-x)*(y-y1)+1.0*q22/d*(x-x1)*(y-y1);
    return f;
}

double JetHelpers::Interpolate(const TH1* histo, const double x, const double y, const double z)
{
    // Copied from ROOT directly and trivially modified, all credit to ROOT authors of TH1, TH2, and TH3 Interpolate methods
    // This is done because I want a const version of interpolation, and none of the methods require modification of the histogram
    const TAxis* fXaxis = histo->GetXaxis();
    const TAxis* fYaxis = histo->GetYaxis();
    const TAxis* fZaxis = histo->GetZaxis();

    // Find the bin by bin edges
    int ubx = fXaxis->FindBin(x);
    int uby = fYaxis->FindBin(y);
    int ubz = fZaxis->FindBin(z);
    
    // Check if the value(s) are outside of the bin range(s)
    if ( ubx < 1 || ubx > histo->GetNbinsX() || uby < 1 || uby > histo->GetNbinsY() || ubz < 1 || ubz > histo->GetNbinsZ() )
    {
       histo->Error("Interpolate","Cannot interpolate outside histogram domain. (x: %f vs [%f,%f], y: %f vs [%f,%f], z: %f vs [%f,%f])",x,fXaxis->GetBinLowEdge(1),fXaxis->GetBinLowEdge(histo->GetNbinsX()+1),y,fYaxis->GetBinLowEdge(1),fYaxis->GetBinLowEdge(histo->GetNbinsY()+1),z,fZaxis->GetBinLowEdge(1),fZaxis->GetBinLowEdge(histo->GetNbinsZ()+1));
       return 0;
    }

    // Now switch from bin edges to bin centres
    // Note that we want to support edge cases, so it is possible that ub* == ob*
    // This functionality is not in original ROOT TH3::Interpolate()
    // This functionality is inspired by TH2::Interpolate()
    int obx = ubx + 1;
    int oby = uby + 1;
    int obz = ubz + 1;
    
    // Calculate distance weights before checking under/overflow bins
    // No longer here, see note below

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
        
    }
    
    // Moved from the point which says "see note below" to better handle non-uniform bins
    // Particularly important for logarithmic bins, which are commonly used in this tool
    // Done following studies/suggestion by P-A Delsart
    double xw = fXaxis->GetBinCenter(obx) - fXaxis->GetBinCenter(ubx);
    double yw = fYaxis->GetBinCenter(oby) - fYaxis->GetBinCenter(uby);
    double zw = fZaxis->GetBinCenter(obz) - fZaxis->GetBinCenter(ubz);
    
    // Not a boundary case, resume normal ROOT::TH3::Interpolate()
    double xd = (x - fXaxis->GetBinCenter(ubx)) / xw;
    double yd = (y - fYaxis->GetBinCenter(uby)) / yw;
    double zd = (z - fZaxis->GetBinCenter(ubz)) / zw;
    
    
    double v[] = { histo->GetBinContent( ubx, uby, ubz ), histo->GetBinContent( ubx, uby, obz ),
                     histo->GetBinContent( ubx, oby, ubz ), histo->GetBinContent( ubx, oby, obz ),
                     histo->GetBinContent( obx, uby, ubz ), histo->GetBinContent( obx, uby, obz ),
                     histo->GetBinContent( obx, oby, ubz ), histo->GetBinContent( obx, oby, obz ) };
    
    
    double i1 = v[0] * (1 - zd) + v[1] * zd;
    double i2 = v[2] * (1 - zd) + v[3] * zd;
    double j1 = v[4] * (1 - zd) + v[5] * zd;
    double j2 = v[6] * (1 - zd) + v[7] * zd;
    
    
    double w1 = i1 * (1 - yd) + i2 * yd;
    double w2 = j1 * (1 - yd) + j2 * yd;
    
    
    double result = w1 * (1 - xd) + w2 * xd;
    
    return result;
}
