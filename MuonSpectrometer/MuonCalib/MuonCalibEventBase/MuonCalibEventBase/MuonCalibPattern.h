/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibPattern.h
//   Header file for class MuonCalibPattern
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONCALIBPATTERN_H
#define MUONCALIB_MUONCALIBPATTERN_H

// stl includes
#include <float.h>

#include <memory>
#include <ostream>
#include <vector>
namespace MuonCalib {

    class MuonCalibSegment;

    /**
       @class MuonCalibPattern

       Class to store the information belonging to a global pattern in the
       muon system.  It contains all information needed to calibrate the
       different technologies in the muon system.

       The pattern recognition provides the MuonCalibPattern, optionally
       one can provide the number of hits associated to the
       pattern. MuonCalibSegment s that are identified to belong to the
       same trajectory are assigned to the MuonCalibPattern. Furthermore,
       the MuonCalibPattern contains a goodness of fit and an estimate of
       the track parameters.

    */

    class MuonCalibPattern {
    public:
        using MuonCalibSegPtr = std::shared_ptr<MuonCalibSegment>;
        //!< typedef for a set of (pointers to) MuonCalibSegments
        using MuonSegmentVec = std::vector<MuonCalibSegPtr>;
        //!< typedef for the iterator of the set of MuonCalibSegments

    public:
        MuonCalibPattern() = default;                              //!< Default constructor
        MuonCalibPattern(const MuonCalibPattern& pat);             //!< Copy constructor. Not sure wether we need it...
        MuonCalibPattern& operator=(const MuonCalibPattern& pat);  //!< assignment operator. Not sure whether we need it...
        ~MuonCalibPattern() = default;                             //!< destructor

        struct defineParams {
            double chi2{FLT_MAX};   //!< Chi2 of the MuonCalibPattern
            double z0{FLT_MAX};     //!< z0 (track)parameter of the MuonCalibPattern
            double dist0{FLT_MAX};  //!< d0 (track)parameter of the MuonCalibPattern
            double invP{FLT_MAX};   //!< invP (track)parameter of the MuonCalibPattern
            double phi{FLT_MAX};    //!< phi (track)parameter of the MuonCalibPattern
            double theta{FLT_MAX};  //!< theta (track)parameter of the MuonCalibPattern

            //!< number of MDT hits. Counting convention: 1000*eta hits + phi hits. Note that MDTs only measure eta hits.
            unsigned int nmdt{0};
            //!< number of RPC hits. Counting convention: 1000*eta hits + phi hits
            unsigned int nrpc{0};
            //!< number of TGC hits. Counting convention: 1000*eta hits + phi hits.

            unsigned int ntgc{0};
            //!< number of CSC hits. Counting convention: 1000*eta hits + phi hits.

            unsigned int ncsc{0};
        };

        MuonCalibPattern(defineParams pars);  //!< constructor initializing the pattern with trackparameters, chi2 and
                                              //!< number of hits in the different technologies.

        // accessors
        /* Number of muonSegments */
        unsigned int muonSegments() const;  //!< retrieve number of segments
        /* access the (const) iterators*/
        /* retrieve a reference to the MuonCalibSegments-vector*/
        const MuonSegmentVec& muonSegs() const;  //!< retrieve the whole set of MuonCalibSegments: reference to the segment vector
        MuonSegmentVec& muonSegs();              //!< retrieve the whole set of MuonCalibSegments: reference to the segment vector

        /* access to track parameters */
        double chi2() const;   //!< retrieve chi2 of the pattern
        double z0() const;     //!< retrieve the z0 (track)parameter of the pattern
        double r0() const;     //!< retrieve the r0 (track)parameter of the pattern
        double invP() const;   //!< retrieve the invP (track)parameter of the pattern
        double phi() const;    //!< retrieve the phi (track)parameter of the pattern
        double theta() const;  //!< retrieve the theta (track)parameter of the pattern

        /* access to number of hits */
        //!< retrieve number of MDT hits. Counting convention: 1000*eta hits + phi hits. Note that MDTs only measure eta hits

        unsigned int nmdtHits() const;
        //!< retrieve number of RPC hits. Counting convention: 1000*eta hits + phi hits.
        unsigned int nrpcHits() const;
        //!< retrieve number of TGC hits. Counting convention: 1000*eta hits + phi hits.
        unsigned int ntgcHits() const;
        //!< retrieve number of CSC hits. Counting convention: 1000*eta hits + phi hits.
        unsigned int ncscHits() const;

        /* methods for adding segments */
        // void addMuonSegment(MuonCalibSegment* seg);  //!< Adds the MuonCalibSegment to the set MuonCalibSegments of the MuonCalibPattern.
        void addMuonSegment(MuonCalibSegPtr seg);
        std::ostream& dump(std::ostream& os) const;

    private:
        MuonSegmentVec m_muonSegments;  //!< set of MuonCalibSegments assigned to this MuonCalibPattern
        void copy(const MuonCalibPattern& other);

        defineParams m_pars;
        /* pattern parameters */
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibPattern& pat);

#endif
