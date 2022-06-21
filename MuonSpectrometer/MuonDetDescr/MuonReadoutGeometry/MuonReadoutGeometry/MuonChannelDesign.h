/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_MUONCHANNELDESIGN_H
#define MUONREADOUTGEOMETRY_MUONCHANNELDESIGN_H

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {

    struct MuonChannelDesign {
    public:
        enum ChannelType { etaStrip, phiStrip };
        enum DetType { MM, STGC, Other };
        ChannelType type{ ChannelType::etaStrip };
        DetType  detType{ DetType::Other };
        int nch{-1};             // total #of active strips
        int febSide{0};
        double sAngle{0.};       // stereo angle
        double sideAngle{0.};    // trapezoid leg angle w.r.t. to the radial axis (used for MM)
        double inputPitch{0.};   // we use this param to define the pitch for MM
        double inputWidth{0.};
        double inputLength{0.};
        double deadI{0.};        // test param for sTGC
        double deadO{0.};        // test param for sTGC
        double deadS{0.};        // test param for sTGC
        double firstPos{0.};     // the position of the first active strip
        double firstPitch{0.};   // Pitch of 1st strip or number of wires in 1st group
        double groupWidth{0.};   // Number of Wires per group
        double nGroups{0.};      // Number of Wire groups
        double wireCutout{0.};
        double xSize{0.};        // radial distance (active area)
        double minYSize{0.};     // bottom length (active area)
        double maxYSize{0.};     // top length (active area)
        double thickness{0.};    // gas thickness
        double yCutout{0.};
        double minYPhiL{0.};     // (left bottom) distance between first eta and stereo strips (MM)
        double minYPhiR{0.};     // (right bottom) distance between first eta and stereo strips (MM)
        double maxYPhi{0.};      // (top) distance between last eta and stereo strips (MM)
        int nMissedTopEta{0};    // #of strips that are not connected to any FE boards (MM)
        int nMissedBottomEta{0};
        int nMissedTopStereo{0};
        int nMissedBottomStereo{0};
        int nRoutedTop{0};       // #of strips that have shorter lengths (at the corners of the PCBs) (MM)
        int nRoutedBottom{0};
        double dlStereoTop{0.};  // length between the first eta and stereo
        double dlStereoBottom{0.};
        int totalStrips{0};      // total strips per MM module

        /** distance to readout */
        double distanceToReadout(const Amg::Vector2D& pos) const;

        /** distance to channel - residual */
        double distanceToChannel(const Amg::Vector2D& pos, int nChannel) const;

        /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
        int channelNumber(const Amg::Vector2D& pos) const;

        /** calculate local wire group number, range 1=64 like identifiers. Returns -1 if out of range */
        int wireGroupNumber(const Amg::Vector2D& pos) const;

        /** calculate the sTGC wire number, with the index of first wire being 1.
         *  The method can return a value outside the range [1, nch].
         *  It returns -1 if not a sTGC chamber.*/
        int wireNumber(const Amg::Vector2D& pos) const;

        /** calculate local channel position for a given channel number */
        bool channelPosition(int channel, Amg::Vector2D& pos) const;

        /** calculate local channel width */
        double channelWidth(const Amg::Vector2D& pos) const;

        /** calculate local stereo angle */
        double stereoAngle(int ) const { return sAngle; }

        /** calculate channel length for a given channel number */
        double channelLength(int channel) const;
        
        /** calculate channel length on the given side of the x axis (for MM stereo strips) */
        double channelHalfLength(int st, bool left) const;

        /** thickness of gas gap */
        double gasGapThickness() const { return thickness; }
    };


    //============================================================================
    inline double MuonChannelDesign::distanceToReadout(const Amg::Vector2D& pos) const {

        // implemented for micromegas and sTGC strips
        // (for sTGC wires returns the full wire length)

        Amg::Vector2D chPos;
        int chNum = channelNumber(pos);
        if (!channelPosition(chNum, chPos)) return -1.;

        double stripL = channelLength(chNum);
        double dist{stripL};

        if (type == ChannelType::etaStrip) {

            if (detType == DetType::MM) {
    
                // MM: febSide tells the side of the first FEB on PCB1 (and all odd FEBs).
                double posAlongStrip = (pos.x() - chPos.x()) * std::sin(sAngle) + (pos.y() - chPos.y()) * std::cos(sAngle); 
                int FEB = (chNum-1)/512 + 1; // FEB number (starting from 1)
                dist = ((febSide > 0) == FEB%2) ? 0.5*stripL - posAlongStrip : 0.5*stripL + posAlongStrip;

            } else if (detType == DetType::STGC) {

                // sTGC: there is one sFEB/module/layer
                double posAlongStrip = pos.y() - chPos.y(); 
                dist = (febSide > 0) ? 0.5*stripL - posAlongStrip : 0.5*stripL + posAlongStrip;
            }
        }

        if (dist < 0.) return 0.;
        return std::min(dist, stripL);
    }


    //============================================================================
    inline double MuonChannelDesign::distanceToChannel(const Amg::Vector2D& pos, int chNum) const {
        
        Amg::Vector2D chPos;
        if (!channelPosition(chNum, chPos)) return -10000.;

        if (type == ChannelType::etaStrip && detType == DetType::MM) {
            return pos.x() - pos.y()*std::tan(sAngle) - chPos.x();
        }

        return pos.x() - chPos.x();
    }


    //============================================================================
    inline int MuonChannelDesign::channelNumber(const Amg::Vector2D& pos) const {

        int chNum{-1};
        if (type == ChannelType::etaStrip && detType == DetType::MM) {

            // ** MM strips: keeping cases outside the active area, but within the envelope, 
            // to avoid warnings from MuonPRDTest. Those channels are removed from digitization.
            double xMid = pos.x() - pos.y() * std::tan(sAngle);
            const int& missedBottom = sAngle ? nMissedBottomStereo : nMissedBottomEta;

            // firstPos is always 1/2 pitch above the center of the first active strip
            chNum = (int)std::floor((xMid - firstPos) / inputPitch) + missedBottom + 2;
            if (chNum < 1 || chNum > totalStrips) chNum = -1;

        } else if (type == ChannelType::phiStrip && detType == DetType::STGC) {

            // ** sTGC wires: return the wire group
            chNum = wireGroupNumber(pos);

        } else {

            // ** All other cases
            chNum = (int)std::floor( (pos.x() - firstPos) / inputPitch ) + 2;
            if (chNum < 1 || chNum > nch) chNum = -1;
        }
   
        return chNum;
    }


    //============================================================================
    inline int MuonChannelDesign::wireGroupNumber(const Amg::Vector2D& pos) const {
    
        // The wires in the 1st gas volume of QL1, QS1 can not be read for digits
        if (type != ChannelType::phiStrip || detType != DetType::STGC) return -1;
        
        // Get the wire number associated to the position
        int wire_number = wireNumber(pos);
        
        // Find the wire group associated to the wire number.
        // For wires, firstPitch is the number of wires in the 1st group.
        int grNumber{-1};
        if (wire_number <= firstPitch) {
            grNumber = 1;  
        } else {
            grNumber = (wire_number - 1 - firstPitch) / groupWidth + 2; // 20 wires per group,
            // If a hit is positionned after the last wire but inside the gas volume.
            // This is really a check for the few mm on the fringe of the gas volume
            // Especially important for QL3. We still consider the digit active.
            if (grNumber > nGroups && pos.x() < 0.5 * maxYSize) grNumber = nGroups;
        }

        // If hit is in an inactive wire region of QL1/QS1, return 63 (invalid).
        // This allows better tracking of hits.
        if (wireCutout != 0. && pos.y() < 0.5 * xSize - wireCutout) return 63;
        if (grNumber < 1 || grNumber > nGroups) return -1;
        
        return grNumber;
    }


    //============================================================================
    inline int MuonChannelDesign::wireNumber(const Amg::Vector2D& pos) const {
    
        // Only determine wire number for sTGC wire surfaces
        int wire_number{-1};
        if (type == ChannelType::phiStrip && detType == DetType::STGC) {
            if ((pos.x() > -0.5 * maxYSize) && (pos.x() < firstPos)) { // before first wire
                wire_number = 1;
            } else {
                wire_number = (pos.x() - firstPos) / inputPitch + 1;
                // Print a warning if the wire number is outside the range [1, nch]
                if (wire_number < 1 || wire_number > nch) {
                    MsgStream log(Athena::getMessageSvc(), "MuonChannelDesign");
                    if (log.level() <= MSG::DEBUG) {
                        log << MSG::DEBUG << "sTGC wire number out of range: wire number = " << wire_number << " local pos = (" << pos.x() << ", " << pos.y() << ")" << endmsg;
                    }
                }
            }
        }

        return wire_number;
    }


    //============================================================================
    inline bool MuonChannelDesign::channelPosition(int st, Amg::Vector2D& pos) const {

        if (type == ChannelType::phiStrip) {

            if (st < 1 || st > nch) return false;
 
            if (detType == DetType::STGC) {  
             
                //** sTGC Wires: return the center of the wire group (not the wire)
                if (st > nGroups || st == 63) return false; // 63 is defined as an unactive digit

                // calculate the end of the first wire group (accounts for staggering).
                // for wires, firstPitch is the number of wires in the first group
                double firstX = firstPos + (firstPitch - 1) * inputPitch;  
                double locX{0.};

                if (st == 1) {               
                    // first group: average the starting and ending x of the group
                    locX = 0.5 * (-0.5 * maxYSize + firstX);
                } else if (st == nGroups)  { 
                    // last group: average the starting and ending x of the group
                    locX = 0.5 * (0.5 * maxYSize + firstX + (nGroups - 2) * groupWidth * inputPitch);
                } else {
                    locX = firstX + groupWidth * inputPitch * (st - 1.5);
                }

                pos[0] = locX;
                pos[1] = 0.;

            } else {

                //** Default case for phi wires
                double dY   = 0.5 * (maxYSize - minYSize);
                double locY = firstPos + (st-1)*inputPitch;
                double locX{0.};

                if (std::abs(locY) > 0.5*minYSize - deadS) {
                    locX = 0.5 * (xSize - deadO - deadI) * (1. - (0.5*maxYSize-deadS - std::abs(locY)) / dY) + 0.5 * (deadI - deadO);
                }

                pos[0] = locY;
                pos[1] = locX;
            }

        } else if (detType == DetType::MM) {
                
            //** MM eta strips (strip numbering starts at 1)
            const int& nMissedBottom = (sAngle == 0.) ? nMissedBottomEta : nMissedBottomStereo;
            if (st <= nMissedBottom || st > nMissedBottom + nch) return false;

            // firstPos is 1/2 pitch above the center of the first active strip.
            pos[0] = firstPos + inputPitch * (st - nMissedBottom - 1.5);
            pos[1] = 0; // centre of strip

        } else {

            //** sTGC and default case for eta strips
            if (st < 1 || st > nch) return false;

            double x = firstPos + (st - 1.5)*inputPitch;
            if (detType == DetType::STGC) {
                if (firstPitch == inputPitch && st == nch) x -= inputWidth/4.; // case last strip is a half-strip
                if (firstPitch  < inputPitch && st ==   1) x += inputWidth/4.; // case first strip is a half-strip
            }

            pos[0] = x;
            pos[1] = 0;
        }

        return true;
    }


    //============================================================================
    inline double MuonChannelDesign::channelHalfLength(int st, bool left) const {

        if (type == ChannelType::etaStrip && detType == DetType::MM) {

            // keeping cases outside the active area (but within envelope) in case such length is needed
            if (st < 1 || st > totalStrips) return -0.5;

            // the length of MM eta strips is even on the two sides of the x axis.          
            double etaLength = inputLength + ((maxYSize-minYSize)*(st-nMissedBottomEta-0.5)*inputPitch/xSize);
            if (sAngle == 0.) return 0.5*etaLength;
 
            // the length of MM stereo strips is uneven on the two sides of the x axis.
            double sideAngle = std::atan( (maxYSize - minYSize)/(2.*xSize) );
            return left ? 0.5*etaLength*(std::cos(sAngle) + std::sin(sAngle)*std::tan(sAngle - sideAngle))
                        : 0.5*etaLength*(std::cos(sAngle) + std::sin(sAngle)*std::tan(sAngle + sideAngle));
        }
        
        // default case:
        return 0.5*channelLength(st);
    }
    

    //============================================================================
    inline double MuonChannelDesign::channelLength(int st) const {

        if (type == ChannelType::phiStrip) {

            if (st < 1 || st > nch) return -1;

            // max wire length in the trapezoid region (below cutout base for sTGC QL3)
            double lX = xSize - deadI - deadO - yCutout;

            // correction for wires on the sides of the trapezoid
            double absY = std::abs(firstPos + (st-1)*inputPitch);
            if (absY > 0.5 * minYSize - deadS) {
                double dY = 0.5*(maxYSize - minYSize);
                lX *= (0.5*maxYSize - deadS - absY) / dY;
            }
            
            return lX + yCutout;

        } else if (type == ChannelType::etaStrip) {
        
            if (detType == DetType::MM) {

                // eta strips: no need to calculate each side separately; we know they are equal
                if (sAngle == 0.) return 2*channelHalfLength(st, false); 

                // stereo strips: get the length on each side
                return channelHalfLength(st, false) + channelHalfLength(st, true);

            } else {

                // get the x-coordinate of the strip
                Amg::Vector2D chPos;
                if (!channelPosition(st, chPos)) return -1;

                // for QL3, if the strip is above the cutout base, return the base length.
                // In this case, also mind that x = 0 is on the cutout base.
                if (yCutout && chPos.x() >= 0.) return maxYSize - 2*deadS;

                // vertical distance from the small base
                double dX    = chPos.x() - (firstPos - firstPitch);
                double dXmax = xSize - deadI - deadO - yCutout;
                return minYSize + (maxYSize - minYSize) * dX / dXmax;
            } 
        }

        return inputLength;
    }


    //============================================================================
    inline double MuonChannelDesign::channelWidth(const Amg::Vector2D& /*pos*/) const {

        if (detType == DetType::STGC) {
            // eta strips: return the pitch (3.2mm), not the width (2.7mm)
            // phi wires: return width of full wire group
            return (type == ChannelType::etaStrip) ? inputPitch : groupWidth * inputPitch;  
        }

        return inputWidth;
    }

}  // namespace MuonGM
#endif  // MUONREADOUTGEOMETRY_MUONCHANNELDESIGN_H
