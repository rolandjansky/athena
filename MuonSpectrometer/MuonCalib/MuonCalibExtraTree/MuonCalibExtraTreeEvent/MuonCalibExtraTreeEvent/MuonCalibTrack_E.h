/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTRATREEEVENT_MUONCALIBTRACK_E_H
#define MUONCALIBEXTRATREEEVENT_MUONCALIBTRACK_E_H

#include <EventPrimitives/EventPrimitives.h>
#include <MuonCalibExtraTreeEvent/MuonCalibHit_E.h>
#include <MuonCalibExtraTreeEvent/MuonCalibHole_E.h>

#include <cmath>
#include <memory>
#include <vector>
namespace MuonCalib {
    class MuonCalibSegment;
    /**
       @class MuonCalibTrack_E

       Simplified class designed to store information of a Track. It has trackparameters and a set of MuonCalib::MuonCalibHit_E s and
       MuonCalib::MuonCalibHole_E s.

    */
    class MuonCalibTrack_E {
    public:
        using CalibHitPtr = std::shared_ptr<const MuonCalibHit_E>;
        using CalibHolePtr = std::shared_ptr<MuonCalibHole_E>;
        using CalibSegPtr = std::shared_ptr<MuonCalibSegment>;
        using HitVector = std::vector<CalibHitPtr>;    //!< typedef of a set of MuonCalib::MuonCalibHit_E s
        using HoleVector = std::vector<CalibHolePtr>;  //!< typedef of a set of MuonCalib::MuonCalibHole_E s
        using SegmentVector = std::vector<CalibSegPtr>;
        MuonCalibTrack_E() = default;  //!< default constructor

        static constexpr float dummy_chi2{9999.};
        struct defineParams {
            float x0{0.};
            float y0{0.};
            float z0{0.};
            float phi{5. * M_PI};
            float theta{0.};
            float qOverP{0.};

            int author{0};
            float chi2{dummy_chi2};
            int ndof{1};
            AmgSymMatrix(5) cov{};
        };

        //!< constructor setting all trackparameters and position
        MuonCalibTrack_E(defineParams pars);
        //!< copyconstructor

        MuonCalibTrack_E(const MuonCalibTrack_E& pat) = default;
        //!< destructor
        virtual ~MuonCalibTrack_E() = default;

        const HitVector& hits() const;
        HitVector& hits();

        const HoleVector& holes() const;
        HoleVector& holes();

        const SegmentVector& segmetnsOnTrack() const;
        SegmentVector& segmetnsOnTrack();

        MuonCalibTrack_E& operator=(const MuonCalibTrack_E& rhs) = default;  //!< assignment operator
        unsigned int nrHits() const { return m_hits.size(); }                //!< returns number of MuonCalib::MuonCalibHit_E s on track
        unsigned int nrHoles() const { return m_holes.size(); }              //!< returns number of MuonCalib::MuonCalibHole_E s on track

        virtual float d0() const;    //!< returns trackparameter d0
        virtual float z0ip() const;  //!< returns trackparameter z0 at IP
        float x0() const;            //!< returns trackparameter x0
        float y0() const;            //!< returns trackparameter y0
        float z0() const;            //!< returns trackparameter z0
        float phi() const;           //!< returns trackparameter phi
        float theta() const;         //!< returns trackparameter theta
        float qOverP() const;        //!< returns trackparameter q/p
        float p() const;             //!< returns momentum
        float pt() const;            //!< returns pt
        float errInvP() const;       //!< returns the error**2 on trackparameter q/p
        int author() const;          //!< returns the author
        float chi2() const;          //!< returns track chi2
        int ndof() const;            //!< returns the number of degrees of freedom

        float cov00() const { return m_params.cov(0, 0); }  //!< returns the covariance matrix elements
        float cov01() const { return m_params.cov(0, 1); }  //!< returns the covariance matrix elements
        float cov02() const { return m_params.cov(0, 2); }  //!< returns the covariance matrix elements
        float cov03() const { return m_params.cov(0, 3); }  //!< returns the covariance matrix elements
        float cov04() const { return m_params.cov(0, 4); }  //!< returns the covariance matrix elements
        float cov11() const { return m_params.cov(1, 1); }  //!< returns the covariance matrix elements
        float cov12() const { return m_params.cov(1, 2); }  //!< returns the covariance matrix elements
        float cov13() const { return m_params.cov(1, 3); }  //!< returns the covariance matrix elements
        float cov14() const { return m_params.cov(1, 4); }  //!< returns the covariance matrix elements
        float cov22() const { return m_params.cov(2, 2); }  //!< returns the covariance matrix elements
        float cov23() const { return m_params.cov(2, 3); }  //!< returns the covariance matrix elements
        float cov24() const { return m_params.cov(2, 4); }  //!< returns the covariance matrix elements
        float cov33() const { return m_params.cov(3, 3); }  //!< returns the covariance matrix elements
        float cov34() const { return m_params.cov(3, 4); }  //!< returns the covariance matrix elements
        float cov44() const { return m_params.cov(4, 4); }  //!< returns the covariance matrix elements

        void setX0(float x0);          //!< sets trackparameter x0
        void setY0(float y0);          //!< sets trackparameter y0
        void setZ0(float z0);          //!< sets trackparameter z0
        void setPhi(float phi);        //!< sets trackparameter phi
        void setTheta(float theta);    //!< sets trackparameter theta
        void setQOverP(float qOverP);  //!< sets trackparameter q/p
        void setAuthor(int author);    //!< sets author

        void setChi2(float chi2);  //!< sets track chi2
        void setNdof(int ndof);    //!< sets ndof

        /** position of perigee of track */
        const Amg::Vector3D& position() const;
        /** direction of perigee of track */
        const Amg::Vector3D& direction() const;

        void setCov00(float cov00) { m_params.cov(0, 0) = cov00; }  //!< sets covariance matrix
        void setCov01(float cov01) { m_params.cov(0, 1) = cov01; }  //!< sets covariance matrix
        void setCov02(float cov02) { m_params.cov(0, 2) = cov02; }  //!< sets covariance matrix
        void setCov03(float cov03) { m_params.cov(0, 3) = cov03; }  //!< sets covariance matrix
        void setCov04(float cov04) { m_params.cov(0, 4) = cov04; }  //!< sets covariance matrix
        void setCov11(float cov11) { m_params.cov(1, 1) = cov11; }  //!< sets covariance matrix
        void setCov12(float cov12) { m_params.cov(1, 2) = cov12; }  //!< sets covariance matrix
        void setCov13(float cov13) { m_params.cov(1, 3) = cov13; }  //!< sets covariance matrix
        void setCov14(float cov14) { m_params.cov(1, 4) = cov14; }  //!< sets covariance matrix
        void setCov22(float cov22) { m_params.cov(2, 2) = cov22; }  //!< sets covariance matrix
        void setCov23(float cov23) { m_params.cov(2, 3) = cov23; }  //!< sets covariance matrix
        void setCov24(float cov24) { m_params.cov(2, 4) = cov24; }  //!< sets covariance matrix
        void setCov33(float cov33) { m_params.cov(3, 3) = cov33; }  //!< sets covariance matrix
        void setCov34(float cov34) { m_params.cov(3, 4) = cov34; }  //!< sets covariance matrix
        void setCov44(float cov44) { m_params.cov(4, 4) = cov44; }  //!< sets covariance matrix

        void addHit(const CalibHitPtr& hit);           //!< Add a MuonCalib::MuonCalibHit_E to the track
        void addHole(const CalibHolePtr& hole);        //!< Add a MuonCalib::MuonCalibHole_E to the track
        void addSegmentOnTrack(const CalibSegPtr& s);  //! Add a Segment on Track

    private:
        /** frees memory of current object */
        void clear();

        HitVector m_hits;    //!< set of MuonCalib::MuonCalibHit_E s on track
        HoleVector m_holes;  //!< set of MuonCalib::MuonCalibHole_E s on track
        SegmentVector m_segments_on_track;

        defineParams m_params{};
        Amg::Vector3D m_pos{0., 0., 0.};
        Amg::Vector3D m_dir{0., 0., 1.};
    };

}  // namespace MuonCalib

#endif  // MUONCALIBTRACK_E_H
