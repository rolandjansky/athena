/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonPadDesign.h"

using MuonGM::MuonPadDesign;


bool MuonPadDesign::withinSensitiveArea(const Amg::Vector2D& pos) const
{
    double top_H1 = maxSensitiveY();
    double bot_H2 = minSensitiveY();
    double max_x = maxAbsSensitiveX(pos.y());
    bool y_in_range = (pos.y() <= top_H1 and pos.y() >= bot_H2);
    bool x_in_range = fabs(pos.x()) <= max_x;
    return y_in_range and x_in_range;
}
//----------------------------------------------------------
double MuonPadDesign::minSensitiveY() const
{
    return -0.5*Length+ysFrame;
}
//----------------------------------------------------------
double MuonPadDesign::maxSensitiveY() const
{
    return +0.5*Length-ylFrame;
}
//----------------------------------------------------------
double MuonPadDesign::maxAbsSensitiveX(const double &y) const
{
    // determine whether you're in the cutout or in the trapezoid
    double cutout_H = 0.5*Length-ylFrame-yCutout;
    bool below_cutout = (y <= cutout_H);
    // Consider the isosceles trapezoid corresponding to the active area below the cutout
    // a = small base
    // b = large base
    // h = height
    // then scale 0.5*(b-a) to the fraction of h
    double height_active_area = (Length-ysFrame-ylFrame);
    double half_a = 0.5*sPadWidth;
    double half_b_minus_a = 0.5*(lPadWidth-sPadWidth);
    double numerator = (below_cutout ?
                        y+(0.5*Length-ysFrame) :
                        Length-ysFrame-ylFrame-yCutout);
    return half_a + half_b_minus_a * numerator / height_active_area;
}
//----------------------------------------------------------
std::pair<int,int> MuonPadDesign::channelNumber( const Amg::Vector2D& pos) const {
    // perform check of the sensitive area
    std::pair<int, int> result(-1, -1);

    // padEta
    double y1 = 0.5*Length + pos.y(); //distance from small edge to hit
    double padEtadouble;
    int padEta = 0;
    // padPhi
    // DT-2015-11-29 : currently easier: attribute 'phi pad fuzzy shift' to hit rather than to pad edge
    double locPhi = 180*atan(etasign*pos.x()/(radialDistance + pos.y()))/M_PI;
    double maxlocPhi = 180*atan(0.5*sPadWidth/(radialDistance + (-0.5*Length+ysFrame)))/M_PI;
    // fuzziness for negative z takes negative of PadPhiShift
    double fuzziedX = pos.x() - (etasign*PadPhiShift /cos(locPhi*M_PI/180));
    double fuzziedlocPhi = 180*atan(etasign*fuzziedX/(radialDistance + pos.y()))/M_PI;

    //std::cout << "\tMuonPadDesign::channelPosition locPhi " << locPhi  << " maxlocPhi " << maxlocPhi << " fuzziedlocPhi " << fuzziedlocPhi << std::endl; 

    bool hit_on_frame = (y1>0 && y1<ysFrame);
    bool below_half_length = (y1<0);
    bool outside_phi_range = (std::abs(locPhi)>maxlocPhi) or (std::abs(fuzziedlocPhi)>maxlocPhi);
    // here you might want to 'ATH_MSG_DEBUG' the three conditions above? not necessary...

    // todo : this fallback was there, but currently is not implemented
    // else {
    //        ATH_MSG_ERROR("undefined distance to hit");
    //        return std::pair<int,int>(-1,-1);
    //    }
    if(withinSensitiveArea(pos) and not hit_on_frame and not below_half_length and not outside_phi_range) {
        if(y1>ysFrame+firstRowPos) {
            //+1 for firstRow, +1 because a remainder means another row (3.1=4)
            padEtadouble =((y1-ysFrame-firstRowPos)/inputRowPitch)+1+1;
            padEta=padEtadouble;
        } else if(y1>ysFrame) {
            padEta=1;
        }

        double padPhidouble = (fuzziedlocPhi-firstPhiPos)/inputPhiPitch;
        int padPhi = padPhidouble+2; //(+1 because remainder means next column e.g. 1.1=2, +1 so rightmostcolumn=1)

        //std::cout << "\tMuonPadDesign::channelPosition fuzziedlocPhi " << fuzziedlocPhi << " firstPhiPos " << firstPhiPos << " inputPhiPitch " << inputPhiPitch << std::endl;  

        // adjust indices if necessary
        if(padEta==nPadH+1){  padEta-=1; } //the top row can be bigger, therefore it is really in the nPadH row.
        if(padPhi == 0) { padPhi=1; } // adjust rightmost
        if(padPhi == nPadColumns+1){ padPhi=nPadColumns; } // adjust lefmost

        // final check on range
        bool ieta_out_of_range = (padEta>nPadH+1);
        bool iphi_out_of_range = (padPhi < 0 || padPhi > nPadColumns+1);
        bool index_out_of_range =  ieta_out_of_range or iphi_out_of_range;
        if(index_out_of_range) {
            ATH_MSG_ERROR((ieta_out_of_range ? "eta" : "phi")
                          <<" out of range "
                          <<" (x,y)=("<<pos.x()<<", "<<pos.y()<<")"
                          <<" (ieta, iphi)=("<<padEta<<", "<<padPhi<<")");
        } else {
            ATH_MSG_DEBUG("padEta,padPhi: " <<padEta<<" , "<<padPhi );
            result = std::make_pair(padEta, padPhi);
        }
    }
    //std::cout << "MuonPadDesign::channelPosition padEta " << result.first << " padPhi " << result.second << " x " << pos.x() << " y " << pos.y() << std::endl;
    return result;
}

//----------------------------------------------------------
bool MuonPadDesign::channelPosition(std::pair<int,int> pad, Amg::Vector2D& pos) const {
    std::vector<Amg::Vector2D> corners;
    channelCorners(pad, corners);
    double yCenter = 0.5*(corners.at(0)[1]+corners.at(2)[1]);
    double xCenter = 0.5*(0.5*(corners.at(0)[0]+corners.at(1)[0]) + 0.5*(corners.at(2)[0]+corners.at(3)[0]));
    pos[0] = xCenter;
    pos[1] = yCenter;
    return true;    
}
//----------------------------------------------------------
bool MuonPadDesign::channelCorners(std::pair<int,int> pad, std::vector<Amg::Vector2D> &corners) const {
    // DG-2015-11-30: todo check whether the offset subtraction is still needed
    int iEta = pad.first; // -1 + padEtaMin;
    int iPhi = pad.second; //  -1 + padPhiMin;
    // bool invalid_indices = iEta<1 || iPhi<1; // DG-2015-11-30 do we still need to check this?
    // if(invalid_indices) return false;
    //double yBot = -0.5*Length + firstRowPos + ysFrame + iEta*inputRowPitch;
    //double yTop = -0.5*Length + firstRowPos + ysFrame + (iEta+1)*inputRowPitch;

    ////// ASM-2015-12-07 : New Implementation
    double yBot = 0., yTop = 0.;
    if(iEta == 1) { 
        yBot = -0.5*Length + ysFrame; 
        yTop = yBot + firstRowPos; 
    }
    else if(iEta > 1) { 
        yBot = -0.5*Length + ysFrame + firstRowPos + (iEta-2)*inputRowPitch; 
        yTop = yBot + inputRowPitch; 
    }
    else { // Unkwown ieta
        return false;
    }
    ////// ASM-2015-12-07 : New Implementation

    //std::cout <<"\tMuonPadDesign::channelPosition iEta " << iEta << " inputRowPitch "  <<  inputRowPitch << " yBot " << yBot << " yTop " << yTop<<std::endl;

    // restrict y to the module sensitive area
    double minY = minSensitiveY();
    double maxY = maxSensitiveY();
    //std::cout <<"\tMuonPadDesign::channelPosition minSensitiveY " << minY << " maxSensitiveY " << maxY<<std::endl;
    if(yBot<minY) yBot = minY;
    if(yTop>maxY) yTop = maxY;

    // here L/R are defined as if you were looking from the IP to the
    // detector (same a clockwise/counterclockwise phi but shorter)
    double phiRight  = firstPhiPos + (iPhi-2)*inputPhiPitch;  // DG-2015-11-30: todo add fuzziness, frame, cutout
    double phiLeft   = firstPhiPos + (iPhi-1)*inputPhiPitch;  // ASM-2015-12-01 : thinking that firstPhiPos determines left edge of the first pad
    double xBotRight = -(yBot+radialDistance) * tan(M_PI*phiRight/180.); // ASM-2015-12-7 : Angles in degrees, tan needs radians
    double xBotLeft  = -(yBot+radialDistance) * tan(M_PI*phiLeft/180.);
    double xTopRight = -(yTop+radialDistance) * tan(M_PI*phiRight/180.);
    double xTopLeft  = -(yTop+radialDistance) * tan(M_PI*phiLeft/180.);
    //std::cout <<"\tMuonPadDesign::channelPosition phiRight " << phiRight << " phiLeft " << phiLeft<<std::endl;
    //std::cout <<"\tMuonPadDesign::channelPosition xBotRight " << xBotRight << " xBotLeft " << xBotLeft << " xTopRight " << xTopRight << " xTopLeft " << xTopLeft <<std::endl;

    // restrict x to the module sensitive area
    double maxAbsXBot = maxAbsSensitiveX(yBot);
    double maxAbsXTop = maxAbsSensitiveX(yTop);
    //std::cout <<"\tMuonPadDesign::channelPosition maxAbsSensitiveXBot " << maxAbsXBot << " maxAbsSensitiveXTop  " <<  maxAbsXTop<<std::endl;
    struct {
        int operator()(const double &v) { return (v > 0) ? +1 : ((v < 0) ? -1 : 0); }
    } sign;
    struct {
        bool operator()(const double &a, const double &b) { return a*a > b*b; }
    } absGreater;
    if(absGreater(xBotRight, maxAbsXBot)) xBotRight = sign(xBotRight)*maxAbsXBot;
    if(absGreater(xBotLeft,  maxAbsXBot)) xBotLeft  = sign(xBotLeft) *maxAbsXBot;
    if(absGreater(xTopRight, maxAbsXTop)) xTopRight = sign(xTopRight)*maxAbsXTop;
    if(absGreater(xTopLeft,  maxAbsXTop)) xTopLeft  = sign(xTopLeft) *maxAbsXTop;
    //std::cout <<"\tMuonPadDesign::channelPosition xBotRight " << xBotRight << " xBotLeft " << xBotLeft << " xTopRight " << xTopRight << " xTopLeft " << xTopLeft <<std::endl;

    // compute pad center
    //double yCenter = 0.5*(yBot+yTop);
    //double xCenter = 0.5*(0.5*(xBotLeft+xBotRight) + 0.5*(xTopLeft+xTopRight));
    //pos[0] = xCenter;
    //pos[1] = yCenter;
    corners.push_back(Amg::Vector2D(-1.0*etasign*xBotLeft,yBot));
    corners.push_back(Amg::Vector2D(-1.0*etasign*xBotRight,yBot));
    corners.push_back(Amg::Vector2D(-1.0*etasign*xTopLeft,yTop));
    corners.push_back(Amg::Vector2D(-1.0*etasign*xTopRight,yTop));
    //std::cout << "MuonPadDesign::channelPosition padEta " << pad.first << " padPhi " << pad.second << " x " << xCenter << " y " << yCenter << std::endl;
    return true;
    // return false; // DG-2015-12-01 \todo run validation and determine when this function fails

    // ------------- old implementation ------- [begin]
    // DG-2015-11-30 keep it temporarily, just for reference

    /* DT 2/9/2015
       The test on the indices for pads on subsequent layers is not
       implemented in MuonPadDesign,but in TrigT1NSW (IIRC).
     */
    // int padEta = pad.first-1+padEtaMin;
    // int padPhi = pad.second-1+padPhiMin;
    // if(padEta < padEtaMin || padEta > padEtaMax) return false;
    // if(padPhi < padPhiMin || padPhi > padPhiMax) return false;
    // pos[1] = firstRowPos + (padEta-0.5)*inputRowPitch - radialDistance;
    // double phi = firstPhiPos + padPhi*inputPhiPitch;
    // pos[0] =  tan(phi)*(radialDistance + pos.y());
    // return true;
    // ------------- old implementation ------- [end]
}
//----------------------------------------------------------
