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
        enum Type {
            etaStrip = 0,  ///< 0 (eta strips, locX || eta, including stereo angle - MM, sTGC strips)
            phiStrip = 1,  ///< 1 (phi strips  locX || phi, no stereo angle        - sTGC wire/gangs)
            pad = 2        ///< 2 (pads        locX || eta,                        - sTGC pads)
        };
        enum DetType { MM = 0, STGC = 1 };
        int type{-1};
        int detType{-1};
        int nch{-1};            // total #of active strips
        double sAngle{0.};      // stereo angle
        double inputPitch{0.};  // we use this param to define the pitch for MM
        double inputWidth{0.};
        double inputLength{0.};
        double deadI{0.};  // this param is not used for MM
        double deadO{0.};  // this param is not used for MM
        double deadS{0.};  // this param is not used for MM
        double signY{0.};
        double firstPos{0.};    // the position of the first active strip
        double firstPitch{0.};  // Pitch of 1st strip or number of wires in 1st group
        double groupWidth{0.};  // Number of Wires per group
        double nGroups{0.};     // Number of Wire groups
        double wireCutout{0.};
        double xSize{0.};  // Module's radial distance
        double xLength{0.};
        double ysFrame{0.};
        double ylFrame{0.};
        double minYSize{0.};   // bottom length (active area)
        double maxYSize{0.};   // top length (active area)
        double thickness{0.};  // gas thickness
        double yCutout{0.};
        double minYPhiL{0.};   //(left bottom) distance between first eta and stereo strips (MM)
        double minYPhiR{0.};   //(right bottom) distance between first eta and stereo strips (MM)
        double maxYPhi{0.};    //(top) distance between last eta and stereo strips (MM)
        int nMissedTopEta{0};  //#of strips that are not connected to any FE boards (MM)
        int nMissedBottomEta{0};
        int nMissedTopStereo{0};
        int nMissedBottomStereo{0};
        int nRoutedTop{0};  //#of strips that have shorter lengths (at the corners of the PCBs) (MM)
        int nRoutedBottom{0};
        double dlStereoTop{0.};  // length between the first eta and stereo
        double dlStereoBottom{0.};
        int totalStrips{0};  // total strips per MM module

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
         *  It returns -1 if not a sTGC chamber.
         */
        int wireNumber(const Amg::Vector2D& pos) const;

        /** calculate local channel position for a given channel number */
        bool channelPosition(int channel, Amg::Vector2D& pos) const;

        /** calculate local channel width */
        double channelWidth(const Amg::Vector2D& pos) const;

        /** calculate local stereo angle */
        double stereoAngle(int channel) const;

        /** calculate channel length for a given channel number */
        double channelLength(int channel) const;

        /** thickness of gas gap */
        double gasGapThickness() const;
    };


    //============================================================================
    inline double MuonChannelDesign::distanceToReadout(const Amg::Vector2D& pos) const {
        int chNum = channelNumber(pos);

        if (chNum < 1) return -1.;
        Amg::Vector2D chPos;
        if (!channelPosition(chNum, chPos)) return -1.;

        double sA = stereoAngle(chNum);

        Amg::Vector2D chLoc((pos.x() - chPos.x()) * std::cos(sA) - (pos.y() - chPos.y()) * std::sin(sA),
                            +(pos.x() - chPos.x()) * std::sin(sA) + (pos.y() - chPos.y()) * std::cos(sA));

        double stripL = channelLength(chNum);

        double dist = signY > 0 ? chLoc.y() + 0.5 * stripL : 0.5 * stripL - chLoc.y();
        if (dist < 0.) return 0.;
        if (dist > stripL) return stripL;
        return dist;
    }


    //============================================================================
    inline double MuonChannelDesign::distanceToChannel(const Amg::Vector2D& pos, int chNum) const {
        // if channel number is out of bounds, get the nearest channel ( mostly for validation purposes )
        bool validMode = false;
        if (type == MuonChannelDesign::etaStrip && detType == MuonChannelDesign::DetType::MM) {
            if (chNum < 1 || chNum > totalStrips) {
                chNum = channelNumber(pos);
                validMode = true;
            }
        } else {
            if (chNum < 1 || chNum > nch) {
                chNum = channelNumber(pos);
                validMode = true;
            }
        }

        Amg::Vector2D chPos;
        if (!channelPosition(chNum, chPos)) return -10000.;
        // In the new geometry from November 2019 the layers are not anymore tilted, therefore we need
        // to introduce the correction for the stereo angle in the distance
        // Patrick Scholer March 18 2020
        Amg::Vector2D chLoc((pos.x() - pos.y() * std::tan(sAngle)) - chPos.x(), pos.y() - chPos.y());
        if (validMode && std::abs(chLoc.x()) > 0.5 * channelWidth(pos)) {
            MsgStream log(Athena::getMessageSvc(), "MuonChannelDesign");
            if (log.level() <= MSG::INFO)
                log << MSG::INFO << "Problem in identification of the channel: distance to nearest channel, channel width: " << chLoc.x()
                    << ", " << channelWidth(pos) << " for channel number " << chNum << endmsg;
        }

        return chLoc.x();
    }


    //============================================================================
    inline int MuonChannelDesign::channelNumber(const Amg::Vector2D& pos) const {
        if (type == MuonChannelDesign::etaStrip) {  // "eta" orientation, assumes consstd::tant stereo angle, can be MM or STGC

            double xMfirst = firstPos;
            double xMid;
            int chNum;
            if (detType == MuonChannelDesign::DetType::STGC) {  // if sTGC strip
                xMid = pos.x() - pos.y() * std::tan(sAngle);
                if (xMid < xMfirst && xMid > xMfirst - firstPitch)
                    chNum = 1;            // If position between bottom boundary and 1st strip
                else if (xMid > xMfirst)  // position higher than first Pos
                    chNum = std::floor(std::cos(sAngle) * (xMid - xMfirst) / inputPitch) + 2;
                else
                    chNum = -1;

                if (chNum < 1 || chNum > nch) return -1;  // used also for calculation of the number of strips

            } else if (detType == MuonChannelDesign::DetType::MM) {
                xMid = pos.x() - pos.y() * std::tan(sAngle);
                int strips = sAngle == 0 ? nMissedBottomEta : nMissedBottomStereo;
                // +1 accounts for the first strip by xMfirst, and the other +1 std::since we start counting at 1.
                chNum = ((int)std::floor((xMid - xMfirst) / inputPitch)) + strips + 2;

                // if position is before beginning of first active strip return 1 (no readout)
                // This serves to protect against stereo layers where there are no strips in the gas volume
                if (xMid < xMfirst - inputPitch) return 1;

                // Protecting stereo layers
                // Stereo layers have gas volume without strips. This makes sure that this area doesn't create bad identifiers
                if (sAngle && xMid > (xMfirst + inputPitch * (totalStrips - nMissedTopStereo - strips))) return totalStrips;

                if (chNum < 1 || chNum > totalStrips) return -1;

            } else
                return -1;

            return chNum;

        } else if (type == MuonChannelDesign::phiStrip) {  // "phi" orientation, local coordinates rotated

            // find transverse pannel size for a given locX
            if (detType == MuonChannelDesign::DetType::STGC)  // if sTGC wires
                return wireGroupNumber(pos);
            int chNum = std::round((pos.x() - firstPos) / inputPitch + 1);

            if (chNum < 1 || chNum > nch) return -1;
            return chNum;

        } else if (type == MuonChannelDesign::pad) {
            // DG-2015-11-27 to be implemented
            return -1;
        }
        return -1;
    }


    //============================================================================
    inline int MuonChannelDesign::wireGroupNumber(const Amg::Vector2D& pos) const {
        // The wires in the 1st gas volume of QL1, QS1 can not be read for digits
        if (type == MuonChannelDesign::phiStrip && detType == MuonChannelDesign::DetType::STGC) {  // sTGC Wires
            // First, find the wire number associated to the position
            int wire_number = wireNumber(pos);
            ////find wire group associated to wire number
            int grNumber;
            if (wire_number <= firstPitch)
                grNumber = 1;  // firstPitch in this case is number of wires in 1st group
            else {
                grNumber = (wire_number - 1 - firstPitch) / groupWidth + 2;  // 20 wires per group,
                /* If a hit is positionned after the last wire but inside the gas volume
                   This is really a check for the few mm on the fringe of the gas volume
                   Especially imporstd::tant for QL3. We still consider the digit active */
                if (grNumber > nGroups && pos.x() < 0.5 * maxYSize) grNumber = nGroups;
            }
            /* If hit is in inactive wire region of QL1/QS1, return 63
               This digit is not added as 63 is an invalid wire number
               But it allows for better tracking of hits */
            if (wireCutout != 0. && pos.y() < 0.5 * xSize - wireCutout) return 63;
            if (grNumber < 1 || grNumber > nGroups) return -1;
            return grNumber;
        } else {
            // Only implemented for sTGC wires
            return -1;
        }
        return -1;
    }


    //============================================================================
    inline int MuonChannelDesign::wireNumber(const Amg::Vector2D& pos) const {
        int wire_number{-1};
        // Only determine wire number for sTGC wire surfaces
        if (type == MuonChannelDesign::phiStrip && detType == MuonChannelDesign::DetType::STGC) {
            if ((pos.x() > -0.5 * maxYSize) && (pos.x() < firstPos)) {  // Before first wire
                wire_number = 1;
            } else {
                wire_number = (pos.x() - firstPos) / inputPitch + 1;
                // Print a warning if the wire number is outside the range [1, nch]
                if ((wire_number < 1) || (wire_number > nch)) {
                    MsgStream log(Athena::getMessageSvc(), "MuonChannelDesign");
                    if (log.level() <= MSG::WARNING)
                        log << MSG::WARNING << "sTGC wire number out of range: wire number = " << wire_number << " local pos = (" << pos.x()
                            << ", " << pos.y() << ")" << endmsg;
                }
            }
        }
        return wire_number;
    }


    //============================================================================
    inline bool MuonChannelDesign::channelPosition(int st, Amg::Vector2D& pos) const {
        double dY = 0.5 * (maxYSize - minYSize - 2. * deadS);

        if (type == MuonChannelDesign::phiStrip) {  // swap coordinates on return

            if (st < 1 || st > nch) return false;

            if (detType == MuonChannelDesign::DetType::STGC) {  // sTGC Wires: returns center of wire group
                if (st > nGroups || st == 63) return false;     // 63 is because we defined 63 as a unactive digit

                double firstX = firstPos + (firstPitch - 1) * inputPitch;  // Position of the end of the first wire group (accounts for staggering)
              
                double locX;
                if (st == 1) // first strip
                    locX = 0.5 * (-0.5 * maxYSize + firstX);
                else if (st == nGroups)  // accounts for staggering by averaging last wire group and last wire
                    locX = 0.5 * (0.5 * maxYSize + firstX + (nGroups - 2) * groupWidth * inputPitch);
                else
                    locX = firstX + groupWidth * inputPitch * (st - 1.5);

                pos[0] = locX;
                pos[1] = 0;

                return true;
            }

            double locY = firstPos + (st - 1) * inputPitch;
            double locX = 0.;

            if (std::abs(locY) > 0.5 * (minYSize - deadS)) {
                locX = 0.5 * (xSize - deadO - deadI) * (1. - (0.5 * (maxYSize - deadS) - std::abs(locY)) / dY) + 0.5 * (deadI - deadO);
            }

            pos[0] = locY;
            pos[1] = locX;

            return true;

        } else if (type == MuonChannelDesign::etaStrip) {
            if (sAngle == 0.) {
                if (detType == MuonChannelDesign::DetType::STGC) {

                    //** sTGC strips (strip numbering starts at 1)

                    if (st < 1 || st > nch) return false;

                    // return the position at the center of the copper
                    double x = firstPos + (st - 1.5)*inputPitch;
                    if (firstPitch == inputPitch && st == nch) x -= inputWidth/4.; // case last strip is a half-strip
                    if (firstPitch  < inputPitch && st ==   1) x += inputWidth/4.; // case first strip is a half-strip

                    pos[0] = x;
                    pos[1] = 0;

                } else if (detType == MuonChannelDesign::DetType::MM) {
                
                    //** MM eta strips (strip numbering starts at 1)
            
                    if (st <= nMissedBottomEta || st > totalStrips - nMissedTopEta) return false;

                    // xSize is the distance from center of the first active strip to center of the last active strip + pitch!
                    // so firstPos (-xSize/2 + pitch) is pitch/2 above the center of the first active strip.
                    pos[0] = firstPos + inputPitch * (st - nMissedBottomEta - 1.5);
                    pos[1] = 0; // centre of strip

                } else {  
                
                    //** Default case

                    pos[0] = firstPos + inputPitch * (st - 1);
                    pos[1] = 0;
                }

                return true;

            } else if (sAngle != 0. && detType == MuonChannelDesign::DetType::MM) {

                //** MM stereo strips (strip numbering starts at 1)

                if (st <= nMissedBottomStereo || st > totalStrips - nMissedTopStereo) return false;

                // xSize is the distance from center of the first active strip to center of the last active strip + pitch!
                // so firstPos (-xSize/2 + pitch) is pitch/2 above the center of the first active strip.
                pos[0] = firstPos + inputPitch * (st - nMissedBottomStereo - 1.5);
                pos[1] = 0; // centre of strip

                return true;

            } else
                return false;

        } else if (type == MuonChannelDesign::pad) {
            // DG-2015-11-27 todo
        }

        return false;
    }


    //============================================================================
    inline double MuonChannelDesign::stereoAngle(int /*st*/) const {
        // to be coded for TGC wire gangs and sTGC pads

        // if (std::sin(sAngle)>0.5) {
        //  double yUp = -0.5*maxYSize + (st-0.5) * maxYSize/nch;
        //  double yDn = -0.5*minYSize + (st-0.5) * minYSize/nch;
        //  return astd::tan((yUp-yDn)/xSize);
        //}

        return sAngle;
    }


    //============================================================================
    inline double MuonChannelDesign::channelLength(int st) const {
        double dY = 0.5 * (maxYSize - minYSize - 2 * deadS);

        if (type == MuonChannelDesign::phiStrip) {
            double locY = firstPos + (st - 1) * inputPitch;

            double gangLength = xSize - deadO - deadI;

            if (std::abs(locY) > 0.5 * minYSize - deadS) {
                gangLength = (0.5 * maxYSize - deadS - std::abs(locY)) / dY * (xSize - deadI - deadO);
            }

            return gangLength;

        } else if (type == MuonChannelDesign::etaStrip) {
            if (sAngle == 0.) {
                if (detType == MuonChannelDesign::DetType::STGC)
                    return inputLength + 2 * (st - 0.5) * dY / nch;


                else if (detType == MuonChannelDesign::DetType::MM) { // MM eta strips
                
                    if (st > nMissedBottomEta && st <= totalStrips-nMissedTopEta)
                        return inputLength + ((maxYSize-minYSize)*(st-nMissedBottomEta+0.5)*inputPitch/xSize);
                    else 
                        return -1;

                } else 
                    return -1;

            } else if (sAngle!=0. && detType==MuonChannelDesign::DetType::MM) { // MM stereo strips
                
                if (st <= nMissedBottomStereo || st > totalStrips - nMissedTopStereo) // inactive strip
                    return -1;                

                // routed strips
                if (st > totalStrips - (nMissedTopStereo + nRoutedTop)) // top routed
                    return (maxYPhi + (totalStrips-nMissedTopStereo-st)*inputPitch)/std::abs(std::sin(sAngle));

                if (st <= nMissedBottomStereo + nRoutedBottom ) // bottom routed
                    return (minYPhiR + (st-1-nMissedBottomStereo)*inputPitch)/std::abs(std::sin(sAngle));

                // unrouted strip
                double absA      = std::abs(sAngle);
                double phiAngle  = std::atan( (maxYSize - minYSize)/(2.*xSize) );
                double etaLength = inputLength + ((maxYSize - minYSize)*(st-nMissedBottomEta+0.5)*inputPitch/xSize);
                return etaLength*(std::cos(absA) + 0.5*std::sin(absA)*( std::tan(absA - phiAngle) + std::tan(absA + phiAngle)));

            } else 
                return -1;

        } else if (type == MuonChannelDesign::pad) {
            // DG-2015-11-23 todo
        }

        return inputLength;
    }


    //============================================================================
    inline double MuonChannelDesign::channelWidth(const Amg::Vector2D& /*pos*/) const {
        // TODO : calculate for TGCs

        // sTGC block:
        if (type == MuonChannelDesign::etaStrip && detType == MuonChannelDesign::DetType::STGC)
            return inputPitch;  // if sTGC strips return 3.2mm which is stored as pitch, not width (2.7mm)
        if (type == MuonChannelDesign::phiStrip && detType == MuonChannelDesign::DetType::STGC)  // if sTGC wires
            return groupWidth * inputPitch;  // if sTGC wires return width of full wire group

        if (type == MuonChannelDesign::etaStrip && detType == MuonChannelDesign::DetType::MM)
            return inputPitch;
        else
            return inputWidth;
    }


    //============================================================================
    inline double MuonChannelDesign::gasGapThickness() const { return thickness; }

}  // namespace MuonGM
#endif  // MUONREADOUTGEOMETRY_MUONCHANNELDESIGN_H
