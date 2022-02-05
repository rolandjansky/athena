/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibSegment.h
//   Header file for class MuonCalibSegment

#ifndef MUONCALIB_MUONCALIBSEGMENT_H
#define MUONCALIB_MUONCALIBSEGMENT_H

// stl includes
#include <iostream>
#include <memory>
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
 */
    class MuonCalibSegment {
    public:
        /* typedefs */

        using MdtHitPtr = std::shared_ptr<MdtCalibHitBase>;
        //!< typedef for a collection of MdtCalibHitBase s
        using MdtHitVec = std::vector<MdtHitPtr>;

        using CscHitPtr = std::shared_ptr<const CscCalibHitBase>;
        //!< typedef for a collection of CscCalibHitBase s
        using CscHitVec = std::vector<CscHitPtr>;

        using RpcHitPtr = std::shared_ptr<const RpcCalibHitBase>;
        //!< typedef for a collection of RpcCalibHitBase s
        using RpcHitVec = std::vector<RpcHitPtr>;

        using TgcHitPtr = std::shared_ptr<const TgcCalibHitBase>;
        //!< typedef for a collection of TgcCalibHitBase s
        using TgcHitVec = std::vector<TgcHitPtr>;

    public:
        MuonCalibSegment(
            double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Amg::Transform3D& locToGlo,
            unsigned int qualityFlag = 0);  //!< constructor fully initializing the segment-parameters, do we need default constructor?
        MuonCalibSegment& operator=(const MuonCalibSegment& seg);  //!< assignment operator
        MuonCalibSegment(const MuonCalibSegment& seg);             //!< copy constructor
        virtual ~MuonCalibSegment();                               //!< destructor

        /* set routines */
        void set(double chi2, const Amg::Vector3D& pos, const Amg::Vector3D& dir);
        void setSegment(const Amg::Vector3D& pos, const Amg::Vector3D& dir);
        void setErrors(double error_dy0, double error_dtheta);  //!< sets Local errors on MuonCalibSegment parameters

        void setAuthor(int author);   //!< sets author field
        void setFittedT0(double t0);  //!< sets t0 field

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

        MdtHitVec& mdtHOT();    //!< retrieve the full set of MdtCalibHitBase s assigned to this segment
        MdtHitVec& mdtClose();  //!< retrieve the full set of nearby mdt hits of this segment.

        CscHitVec& cscHOT();    //!< retrieve the full set of CscCalibHitBase s assigned to this segment
        CscHitVec& cscClose();  //!< retrieve the full set of nearby mdt hits of this segment.

        RpcHitVec& rpcHOT();    //!< retrieve the full set of RpcCalibHitBase s assigned to this segment
        RpcHitVec& rpcClose();  //!< retrieve the full set of nearby mdt hits of this segment.

        TgcHitVec& tgcHOT();    //!< retrieve the full set of TgcCalibHitBase s assigned to this segment
        TgcHitVec& tgcClose();  //!< retrieve the full set of nearby mdt hits of this segment.

        /* member access */
        double chi2() const;                     //!< retrieve chi2
        double error_dy0() const;                //!< retrieve error position
        double error_dtheta() const;             //!< retrieve error angle
        bool hasFittedT0() const;                //!< check whether t0 was fitted
        double fittedT0() const;                 //!< retrieve fitted T0, return -99999 if no fit was performed
        const Amg::Vector3D& position() const;   //!< retrieve local position of segment (on station level)
        const Amg::Vector3D& direction() const;  //!< retrieve local direction of segment (on station level)
                                                 //!< retrieve the transformation from local chamber coordinates into global coordinates

        const Amg::Transform3D& localToGlobal() const;
        Amg::Vector3D globalPosition() const;   //!< retrieve global position
        Amg::Vector3D globalDirection() const;  //!< retrieve global direction

        int author() const;                //!< retrieve the segment author
        unsigned int qualityFlag() const;  //!< retrieve the Fit quality flag

        /* methodes to add XxxCalibHitBase's */
        void addHitOnTrack(MdtCalibHitBase* hit);  //!< add the MdtCalibHitBase to the set assigned to the segment
        void addCloseHit(MdtCalibHitBase* hit);    //!< add the mdt hit to the set nearby the segment

        void addHitOnTrack(CscCalibHitBase* hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(CscCalibHitBase* hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(RpcCalibHitBase* hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(RpcCalibHitBase* hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(TgcCalibHitBase* hit);  //!< add the TgcCalibHitBase to the set assigned to the segment
        void addCloseHit(TgcCalibHitBase* hit);    //!< add the tgc hit to the set nearby the segment

        //!< Version using the smart pointers
        void addHitOnTrack(const MdtHitPtr& hit);  //!< add the MdtCalibHitBase to the set assigned to the segment
        void addCloseHit(const MdtHitPtr& hit);    //!< add the mdt hit to the set nearby the segment

        void addHitOnTrack(const CscHitPtr& hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(const CscHitPtr& hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(const RpcHitPtr& hit);  //!< add the CscCalibHitBase to the set assigned to the segment
        void addCloseHit(const RpcHitPtr& hit);    //!< add the csc hit to the set nearby the segment

        void addHitOnTrack(const TgcHitPtr& hit);  //!< add the TgcCalibHitBase to the set assigned to the segment
        void addCloseHit(const TgcHitPtr& hit);    //!< add the tgc hit to the set nearby the segment

        void refineMdtSelection(const std::vector<unsigned int>& new_selection);  //!< move trck hits to close hits
        std::ostream& dump(std::ostream& stream) const;

    private:
        static constexpr double s_dummy_ctor_dbl{-99999.};
        void copy(const MuonCalibSegment& other);
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
        double m_chi2{FLT_MAX};                      //!< goodness of (DC)fit
        double m_dy0{FLT_MAX};                       //!< error on local position
        double m_dtheta{FLT_MAX};                    //!< error on local direction
        Amg::Vector3D m_localPosition{0., 0., 0.};   //!< position in station (local) coordinates
        Amg::Vector3D m_localDirection{0., 0., 0.};  //!< direction in station (local) coordinates
        Amg::Transform3D m_localToGlobal{};          //!< transformation local->global
        double m_fittedT0{s_dummy_ctor_dbl};         //!< fitted t0
        unsigned int m_qualityFlag{0};               //!< flag describing the fit quality
        int m_author{-1};                            //!< author flag
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibSegment& seg);

#endif
