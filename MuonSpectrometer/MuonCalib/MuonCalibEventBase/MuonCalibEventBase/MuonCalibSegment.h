/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibSegment.h
//   Header file for class MuonCalibSegment
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONCALIBSEGMENT_H
#define MUONCALIB_MUONCALIBSEGMENT_H

// stl includes
#include <iostream>
#include <vector>

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibEventBase/CscCalibHitBase.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/RpcCalibHitBase.h"
#include "MuonCalibEventBase/TgcCalibHitBase.h"

namespace MuonCalib {
    /**
    @class MuonCalibSegment
    A MuonCalibSegment is a reconstructed three dimensional track segment in the MuonSpectrometer.

    It contains information needed for Mdt calibration:
        - Hits on track
        - Hits close to the track are not implemented yet, though the infrastructure is present
        - Straight line track parameters and a goodness of fit.
        - The transformation to go from station-coordinates (local) to global coordinates.

    The MuonCalibSegment is input for the DCFitters (drift-circle fitters) and should contain all information
    needed by the fitter to do their job.

    Functionality is forseen to store nearby hits, which are not assigned to the segment, as well. The infrastructure is available within
    this class.

    @author Niels.Van.Eldik@cern.ch and Zdenko.Van.Kesteren@cern.ch
    */
    class MuonCalibSegment {
    public:
        /* typedefs */
        typedef std::vector<MdtCalibHitBase*> MdtHitVec;  //!< typedef for a collection of MdtCalibHitBase s
        typedef MdtHitVec::iterator MdtHitIt;             //!< typedef for an interator for the collection of MdtCalibHitBase s
        typedef MdtHitVec::const_iterator MdtHitCit;      //!< typedef for a const_interator for the collection of MdtCalibHitBase s

        typedef std::vector<CscCalibHitBase*> CscHitVec;  //!< typedef for a collection of CscCalibHitBase s
        typedef CscHitVec::iterator CscHitIt;             //!< typedef for an interator for the collection of CscCalibHitBase s
        typedef CscHitVec::const_iterator CscHitCit;      //!< typedef for a const_interator for the collection of CscCalibHitBase s

        typedef std::vector<RpcCalibHitBase*> RpcHitVec;  //!< typedef for a collection of RpcCalibHitBase s
        typedef RpcHitVec::iterator RpcHitIt;             //!< typedef for an interator for the collection of RpcCalibHitBase s
        typedef RpcHitVec::const_iterator RpcHitCit;      //!< typedef for a const_interator for the collection of RpcCalibHitBase s

        typedef std::vector<TgcCalibHitBase*> TgcHitVec;  //!< typedef for a collection of TgcCalibHitBase s
        typedef TgcHitVec::iterator TgcHitIt;             //!< typedef for an interator for the collection of TgcCalibHitBase s
        typedef TgcHitVec::const_iterator TgcHitCit;      //!< typedef for a const_interator for the collection of TgcCalibHitBase s

    public:
        MuonCalibSegment(
            double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Amg::Transform3D& locToGlo,
            unsigned int qualityFlag = 0);  //!< constructor fully initializing the segment-parameters, do we need default constructor?
        MuonCalibSegment& operator=(const MuonCalibSegment& seg);  //!< assignment operator
        MuonCalibSegment(const MuonCalibSegment& seg);             //!< copy constructor
        virtual ~MuonCalibSegment();                               //!< destructor

        /* set routines */
        inline void set(double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir);
        inline void SetSegment(const Amg::Vector3D& pos, const Amg::Vector3D& dir) {
            m_localPosition = pos;
            m_localDirection = dir;
        }
        void setErrors(double error_dy0, double error_dtheta) {
            m_dy0 = error_dy0;
            m_dtheta = error_dtheta;
        }  //!< sets Local errors on MuonCalibSegment parameters

        void setAuthor(int author) { m_author = author; }  //!< sets author field
        void setFittedT0(double t0) { m_fittedT0 = t0; }   //!< sets t0 field

        /* member access */

        /* number of hits in segment */
        unsigned int hitsOnTrack() const;  //!< retrieve the sum of all XxxCalibHits assigned to the MuonCalibSegment
        unsigned int closeHits() const;    //!< retrieve the sum of all hits close to the MuonCalibSegment.

        unsigned int mdtHitsOnTrack() const;  //!< retrieve the number of MdtCalibHitBase s assigned to this segment
        unsigned int mdtCloseHits() const;    //!< retrieve the number of nearby mdt hits.

        unsigned int cscHitsOnTrack() const;  //!< retrieve the number of CscCalibHitBase s assigned to this segment
        unsigned int cscCloseHits() const;    //!< retrieve the number of nearby csc hits.

        unsigned int rpcHitsOnTrack() const;  //!< retrieve the number of RpcCalibHitBase s assigned to this segment
        unsigned int rpcCloseHits() const;    //!< retrieve the number of nearby rpc hits.

        unsigned int tgcHitsOnTrack() const;  //!< retrieve the number of TgcCalibHitBase s assigned to this segment
        unsigned int tgcCloseHits() const;    //!< retrieve the number of nearby tgc hits.

        unsigned int hitsPerML(int ML) const;  //!< number of hits per multilayer of this segment
        unsigned int missedHits() const;       //!< number of missed hits of this segment (out of time + empty tubes)
        unsigned int deltaHits() const;        //!< number of delta hits of this segment
        unsigned int emptyTubes() const;       //!< number of empty tubes of this segment
        unsigned int outOfTimeHits() const;    //!< number of hits out of time of this segment

        /* hit vectors */
        const MdtHitVec& mdtHOT() const;    //!< retrieve the full set of MdtCalibHitBase s assigned to this segment
        const MdtHitVec& mdtClose() const;  //!< retrieve the full set of nearby mdt hits of this segment.

        const CscHitVec& cscHOT() const;    //!< retrieve the full set of CscCalibHitBase s assigned to this segment
        const CscHitVec& cscClose() const;  //!< retrieve the full set of nearby mdt hits of this segment.

        const RpcHitVec& rpcHOT() const;    //!< retrieve the full set of RpcCalibHitBase s assigned to this segment
        const RpcHitVec& rpcClose() const;  //!< retrieve the full set of nearby mdt hits of this segment.

        const TgcHitVec& tgcHOT() const;    //!< retrieve the full set of TgcCalibHitBase s assigned to this segment
        const TgcHitVec& tgcClose() const;  //!< retrieve the full set of nearby mdt hits of this segment.

        /* iterators */
        MdtHitIt mdtHOTBegin();        //!< retrieve iterator to the first MdtCalibHitBase* of the set assigned to this segment.
        MdtHitIt mdtHOTEnd();          //!< retrieve iterator to the last MdtCalibHitBase* of the set assigned to this segment.
        MdtHitIt mdtCloseHitsBegin();  //!< retrieve iterator to the first mdt hit of the set nearby this segment.
        MdtHitIt mdtCloseHitsEnd();    //!< retrieve iterator to the last mdt hit of the set nearby this segment.

        CscHitIt cscHOTBegin();        //!< retrieve iterator to the first CscCalibHitBase* of the set assigned to this segment.
        CscHitIt cscHOTEnd();          //!< retrieve iterator to the last CscCalibHitBase* of the set assigned to this segment.
        CscHitIt cscCloseHitsBegin();  //!< retrieve iterator to the first csc hit of the set nearby this segment.
        CscHitIt cscCloseHitsEnd();    //!< retrieve iterator to the last csc hit of the set nearby this segment.

        RpcHitIt rpcHOTBegin();        //!< retrieve iterator to the first RpcCalibHitBase* of the set assigned to this segment.
        RpcHitIt rpcHOTEnd();          //!< retrieve iterator to the last RpcCalibHitBase* of the set assigned to this segment.
        RpcHitIt rpcCloseHitsBegin();  //!< retrieve iterator to the first rpc hit of the set nearby this segment.
        RpcHitIt rpcCloseHitsEnd();    //!< retrieve iterator to the last rpc hit of the set nearby this segment.

        TgcHitIt tgcHOTBegin();        //!< retrieve iterator to the first TgcCalibHitBase* of the set assigned to this segment.
        TgcHitIt tgcHOTEnd();          //!< retrieve iterator to the last TgcCalibHitBase* of the set assigned to this segment.
        TgcHitIt tgcCloseHitsBegin();  //!< retrieve iterator to the first tgc hit of the set nearby this segment.
        TgcHitIt tgcCloseHitsEnd();    //!< retrieve iterator to the last tgc hit of the set nearby this segment.

        /** const_iterators */
        MdtHitCit mdtHOTBegin() const;  //!< retrieve const_iterator to the first MdtCalibHitBase* of the set assigned to this segment.
        MdtHitCit mdtHOTEnd() const;    //!< retrieve const_iterator to the last MdtCalibHitBase* of the set assigned to this segment.
        MdtHitCit mdtCloseHitsBegin() const;  //!< retrieve const_iterator to the first mdt hit of the set nearby the segment.
        MdtHitCit mdtCloseHitsEnd() const;    //!< retrieve const_iterator to the last mdt hit of the set nearby this segment.

        CscHitCit cscHOTBegin() const;  //!< retrieve const_iterator to the first CscCalibHitBase* of the set assigned to this segment.
        CscHitCit cscHOTEnd() const;    //!< retrieve const_iterator to the last CscCalibHitBase* of the set assigned to this segment.
        CscHitCit cscCloseHitsBegin() const;  //!< retrieve const_iterator to the first csc hit of the set nearby the segment.
        CscHitCit cscCloseHitsEnd() const;    //!< retrieve const_iterator to the last csc hit of the set nearby this segment.

        RpcHitCit rpcHOTBegin() const;  //!< retrieve const_iterator to the first RpcCalibHitBase* of the set assigned to this segment.
        RpcHitCit rpcHOTEnd() const;    //!< retrieve const_iterator to the last RpcCalibHitBase* of the set assigned to this segment.
        RpcHitCit rpcCloseHitsBegin() const;  //!< retrieve const_iterator to the first rpc hit of the set nearby the segment.
        RpcHitCit rpcCloseHitsEnd() const;    //!< retrieve const_iterator to the last rpc hit of the set nearby this segment.

        TgcHitCit tgcHOTBegin() const;  //!< retrieve const_iterator to the first TgcCalibHitBase* of the set assigned to this segment.
        TgcHitCit tgcHOTEnd() const;    //!< retrieve const_iterator to the last TgcCalibHitBase* of the set assigned to this segment.
        TgcHitCit tgcCloseHitsBegin() const;  //!< retrieve const_iterator to the first tgc hit of the set nearby the segment.
        TgcHitCit tgcCloseHitsEnd() const;    //!< retrieve const_iterator to the last tgc hit of the set nearby this segment.

        /* member access */
        double chi2() const;                                                //!< retrieve chi2
        double error_dy0() const;                                           //!< retrieve error position
        double error_dtheta() const;                                        //!< retrieve error angle
        bool hasFittedT0() const { return m_fittedT0 != -99999.; }          //!< check whether t0 was fitted
        double fittedT0() const { return hasFittedT0() ? m_fittedT0 : 0; }  //!< retrieve fitted T0, return -99999 if no fit was performed
        const Amg::Vector3D& position() const;                              //!< retrieve local position of segment (on station level)
        const Amg::Vector3D& direction() const;                             //!< retrieve local direction of segment (on station level)
        const Amg::Transform3D& localToGlobal()
            const;  //!< retrieve the transformation from local chamber coordinates into global coordinates

        Amg::Vector3D globalPosition() const;   //!< retrieve global position
        Amg::Vector3D globalDirection() const;  //!< retrieve global direction

        int author() const { return m_author; }                     //!< retrieve the segment author
        unsigned int qualityFlag() const { return m_qualityFlag; }  //!< retrieve the Fit quality flag

        /* methodes to add XxxCalibHitBase's */
        void addHitOnTrack(MdtCalibHitBase* hit);  //!< add the MdtCalibHitBase to the set assigned to the segment
        void addCloseHit(MdtCalibHitBase* hit);    //!< add the mdt hit to the set nearby the segment

        void addHitOnTrack(CscCalibHitBase* hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(CscCalibHitBase* hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(RpcCalibHitBase* hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(RpcCalibHitBase* hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(TgcCalibHitBase* hit);  //!< add the TgcCalibHitBase to the set assigned to the segment
        void addCloseHit(TgcCalibHitBase* hit);    //!< add the tgc hit to the set nearby the segment

        void refineMdtSelection(const std::vector<unsigned int>& new_selection);  //!< move trck hits to close hits
        std::ostream& dump(std::ostream& stream) const;

    private:
        /** hits  */
        MdtHitVec m_mdtHitsOnTrack;  //!< set of MdtCalibHitBase s assigned to the segment
        MdtHitVec m_mdtCloseHits;    //!< set of mdt hits nearby the segment

        CscHitVec m_cscHitsOnTrack;  //!< set of CscCalibHitBase s assigned to the segment
        CscHitVec m_cscCloseHits;    //!< set of csc hits nearby the segment

        RpcHitVec m_rpcHitsOnTrack;  //!< set of RpcCalibHitBase s assigned to the segment
        RpcHitVec m_rpcCloseHits;    //!< set of rpc hits nearby the segment

        TgcHitVec m_tgcHitsOnTrack;  //!< set of TgcCalibHitBase s assigned to the segment
        TgcHitVec m_tgcCloseHits;    //!< set of tgc hits nearby the segment

        /* local track parameters in chamber coordinates */
        double m_chi2;                     //!< goodness of (DC)fit
        double m_dy0;                      //!< error on local position
        double m_dtheta;                   //!< error on local direction
        Amg::Vector3D m_localPosition;     //!< position in station (local) coordinates
        Amg::Vector3D m_localDirection;    //!< direction in station (local) coordinates
        Amg::Transform3D m_localToGlobal;  //!< transformation local->global
        double m_fittedT0;                 //!< fitted t0
        unsigned int m_qualityFlag;        //!< flag describing the fit quality
        int m_author;                      //!< author flag
    };

    // set routines
    inline void MuonCalibSegment::set(double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir) {
        m_chi2 = chi2;
        m_localPosition = pos;
        m_localDirection = dir;
    }

    // member access
    inline unsigned int MuonCalibSegment::hitsOnTrack() const {
        return m_mdtHitsOnTrack.size() + m_cscHitsOnTrack.size();  // + m_rpcHitsOnTrack.size() + m_tgcHitsOnTrack.size();
    }

    inline unsigned int MuonCalibSegment::closeHits() const {
        return m_mdtCloseHits.size();  // + m_cscCloseHits.size() + m_rpcCloseHits.size() + m_tgcCloseHits.size();
    }

    //  number of hits in segment
    inline unsigned int MuonCalibSegment::missedHits() const { return outOfTimeHits() + emptyTubes(); }
    inline unsigned int MuonCalibSegment::deltaHits() const { return m_qualityFlag % 10; }
    inline unsigned int MuonCalibSegment::emptyTubes() const { return (m_qualityFlag % 1000) / 100; }
    inline unsigned int MuonCalibSegment::outOfTimeHits() const { return (m_qualityFlag % 100) / 10; }

    // MDT specific
    inline unsigned int MuonCalibSegment::mdtHitsOnTrack() const { return m_mdtHitsOnTrack.size(); }
    inline const MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtHOT() const { return m_mdtHitsOnTrack; }
    inline MuonCalibSegment::MdtHitIt MuonCalibSegment::mdtHOTBegin() { return m_mdtHitsOnTrack.begin(); }
    inline MuonCalibSegment::MdtHitIt MuonCalibSegment::mdtHOTEnd() { return m_mdtHitsOnTrack.end(); }
    inline MuonCalibSegment::MdtHitCit MuonCalibSegment::mdtHOTBegin() const { return m_mdtHitsOnTrack.begin(); }
    inline MuonCalibSegment::MdtHitCit MuonCalibSegment::mdtHOTEnd() const { return m_mdtHitsOnTrack.end(); }

    inline unsigned int MuonCalibSegment::mdtCloseHits() const { return m_mdtCloseHits.size(); }
    inline const MuonCalibSegment::MdtHitVec& MuonCalibSegment::mdtClose() const { return m_mdtCloseHits; }
    inline MuonCalibSegment::MdtHitIt MuonCalibSegment::mdtCloseHitsBegin() { return m_mdtCloseHits.begin(); }
    inline MuonCalibSegment::MdtHitIt MuonCalibSegment::mdtCloseHitsEnd() { return m_mdtCloseHits.end(); }
    inline MuonCalibSegment::MdtHitCit MuonCalibSegment::mdtCloseHitsBegin() const { return m_mdtCloseHits.begin(); }
    inline MuonCalibSegment::MdtHitCit MuonCalibSegment::mdtCloseHitsEnd() const { return m_mdtCloseHits.end(); }

    // CSC specific
    inline unsigned int MuonCalibSegment::cscHitsOnTrack() const { return m_cscHitsOnTrack.size(); }
    inline const MuonCalibSegment::CscHitVec& MuonCalibSegment::cscHOT() const { return m_cscHitsOnTrack; }
    inline MuonCalibSegment::CscHitIt MuonCalibSegment::cscHOTBegin() { return m_cscHitsOnTrack.begin(); }
    inline MuonCalibSegment::CscHitIt MuonCalibSegment::cscHOTEnd() { return m_cscHitsOnTrack.end(); }
    inline MuonCalibSegment::CscHitCit MuonCalibSegment::cscHOTBegin() const { return m_cscHitsOnTrack.begin(); }
    inline MuonCalibSegment::CscHitCit MuonCalibSegment::cscHOTEnd() const { return m_cscHitsOnTrack.end(); }

    inline unsigned int MuonCalibSegment::cscCloseHits() const { return m_cscCloseHits.size(); }
    inline const MuonCalibSegment::CscHitVec& MuonCalibSegment::cscClose() const { return m_cscCloseHits; }
    inline MuonCalibSegment::CscHitIt MuonCalibSegment::cscCloseHitsBegin() { return m_cscCloseHits.begin(); }
    inline MuonCalibSegment::CscHitIt MuonCalibSegment::cscCloseHitsEnd() { return m_cscCloseHits.end(); }
    inline MuonCalibSegment::CscHitCit MuonCalibSegment::cscCloseHitsBegin() const { return m_cscCloseHits.begin(); }
    inline MuonCalibSegment::CscHitCit MuonCalibSegment::cscCloseHitsEnd() const { return m_cscCloseHits.end(); }

    // RPC specific
    inline unsigned int MuonCalibSegment::rpcHitsOnTrack() const { return m_rpcHitsOnTrack.size(); }
    inline const MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcHOT() const { return m_rpcHitsOnTrack; }
    inline MuonCalibSegment::RpcHitIt MuonCalibSegment::rpcHOTBegin() { return m_rpcHitsOnTrack.begin(); }
    inline MuonCalibSegment::RpcHitIt MuonCalibSegment::rpcHOTEnd() { return m_rpcHitsOnTrack.end(); }
    inline MuonCalibSegment::RpcHitCit MuonCalibSegment::rpcHOTBegin() const { return m_rpcHitsOnTrack.begin(); }
    inline MuonCalibSegment::RpcHitCit MuonCalibSegment::rpcHOTEnd() const { return m_rpcHitsOnTrack.end(); }

    inline unsigned int MuonCalibSegment::rpcCloseHits() const { return m_rpcCloseHits.size(); }
    inline const MuonCalibSegment::RpcHitVec& MuonCalibSegment::rpcClose() const { return m_rpcCloseHits; }
    inline MuonCalibSegment::RpcHitIt MuonCalibSegment::rpcCloseHitsBegin() { return m_rpcCloseHits.begin(); }
    inline MuonCalibSegment::RpcHitIt MuonCalibSegment::rpcCloseHitsEnd() { return m_rpcCloseHits.end(); }
    inline MuonCalibSegment::RpcHitCit MuonCalibSegment::rpcCloseHitsBegin() const { return m_rpcCloseHits.begin(); }
    inline MuonCalibSegment::RpcHitCit MuonCalibSegment::rpcCloseHitsEnd() const { return m_rpcCloseHits.end(); }

    // TGC specific
    inline unsigned int MuonCalibSegment::tgcHitsOnTrack() const { return m_tgcHitsOnTrack.size(); }
    inline const MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcHOT() const { return m_tgcHitsOnTrack; }
    inline MuonCalibSegment::TgcHitIt MuonCalibSegment::tgcHOTBegin() { return m_tgcHitsOnTrack.begin(); }
    inline MuonCalibSegment::TgcHitIt MuonCalibSegment::tgcHOTEnd() { return m_tgcHitsOnTrack.end(); }
    inline MuonCalibSegment::TgcHitCit MuonCalibSegment::tgcHOTBegin() const { return m_tgcHitsOnTrack.begin(); }
    inline MuonCalibSegment::TgcHitCit MuonCalibSegment::tgcHOTEnd() const { return m_tgcHitsOnTrack.end(); }

    inline unsigned int MuonCalibSegment::tgcCloseHits() const { return m_tgcCloseHits.size(); }
    inline const MuonCalibSegment::TgcHitVec& MuonCalibSegment::tgcClose() const { return m_tgcCloseHits; }
    inline MuonCalibSegment::TgcHitIt MuonCalibSegment::tgcCloseHitsBegin() { return m_tgcCloseHits.begin(); }
    inline MuonCalibSegment::TgcHitIt MuonCalibSegment::tgcCloseHitsEnd() { return m_tgcCloseHits.end(); }
    inline MuonCalibSegment::TgcHitCit MuonCalibSegment::tgcCloseHitsBegin() const { return m_tgcCloseHits.begin(); }
    inline MuonCalibSegment::TgcHitCit MuonCalibSegment::tgcCloseHitsEnd() const { return m_tgcCloseHits.end(); }

    // local track parameters
    inline double MuonCalibSegment::chi2() const { return m_chi2; }
    inline double MuonCalibSegment::error_dy0() const { return m_dy0; }
    inline double MuonCalibSegment::error_dtheta() const { return m_dtheta; }
    inline const Amg::Vector3D& MuonCalibSegment::position() const { return m_localPosition; }
    inline const Amg::Vector3D& MuonCalibSegment::direction() const { return m_localDirection; }
    inline const Amg::Transform3D& MuonCalibSegment::localToGlobal() const { return m_localToGlobal; }

    inline Amg::Vector3D MuonCalibSegment::globalPosition() const { return Amg::Vector3D(localToGlobal() * position()); }
    inline Amg::Vector3D MuonCalibSegment::globalDirection() const { return Amg::Vector3D(localToGlobal().linear() * direction()); }

    // methodes to add CalibHitBase's
    inline void MuonCalibSegment::addHitOnTrack(MdtCalibHitBase* hit) { m_mdtHitsOnTrack.push_back(hit); }
    inline void MuonCalibSegment::addCloseHit(MdtCalibHitBase* hit) { m_mdtCloseHits.push_back(hit); }

    inline void MuonCalibSegment::addHitOnTrack(CscCalibHitBase* hit) { m_cscHitsOnTrack.push_back(hit); }
    inline void MuonCalibSegment::addCloseHit(CscCalibHitBase* hit) { m_cscCloseHits.push_back(hit); }

    inline void MuonCalibSegment::addHitOnTrack(RpcCalibHitBase* hit) { m_rpcHitsOnTrack.push_back(hit); }
    inline void MuonCalibSegment::addCloseHit(RpcCalibHitBase* hit) { m_rpcCloseHits.push_back(hit); }

    inline void MuonCalibSegment::addHitOnTrack(TgcCalibHitBase* hit) { m_tgcHitsOnTrack.push_back(hit); }
    inline void MuonCalibSegment::addCloseHit(TgcCalibHitBase* hit) { m_tgcCloseHits.push_back(hit); }
}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibSegment& seg);

#endif
