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
#include <vector>
#include <ostream>

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

   @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
*/

  class MuonCalibPattern {
  public:
    typedef std::vector<MuonCalibSegment*> MuonSegmentVec; //!< typedef for a set of (pointers to) MuonCalibSegments
    typedef MuonSegmentVec::iterator       MuonSegIt;      //!< typedef for the iterator of the set of MuonCalibSegments
    typedef MuonSegmentVec::const_iterator MuonSegCit;     //!< typedef for the const_iterator of the set of MuonCalibSegments 


  public:
    MuonCalibPattern(); //!< Default constructor
    MuonCalibPattern(double chi2, double z0, double r0,
		     double invP, double phi, double theta); //!< constructor initializing the trackparameters + chi2
    MuonCalibPattern(double chi2, double z0, double r0,
		      double invP, double phi, double theta, 
		     unsigned int nmdt, unsigned int nrpc, unsigned int ntgc, unsigned int ncsc); //!< constructor initializing the pattern with trackparameters, chi2 and number of hits in the different technologies.
    MuonCalibPattern(const MuonCalibPattern &pat);//!< Copy constructor. Not sure wether we need it...
    MuonCalibPattern& operator=(const MuonCalibPattern &pat ); //!< assignment operator. Not sure whether we need it...
    ~MuonCalibPattern(); //!< destructor

    //accessors
    /* Number of muonSegments */
    unsigned int          muonSegments() const;  //!< retrieve number of segments
    /* access the (const) iterators*/
    MuonSegIt             muonSegBegin();        //!< retrieve iterator: first MuonCalibSegment* of the set 
    MuonSegIt             muonSegEnd();          //!< retrieve end iterator: last MuonCalibSegment* of the set
    MuonSegCit            muonSegBegin() const;  //!< retrieve begin const_iterator: first MuonCalibSegment* of the set  
    MuonSegCit            muonSegEnd() const;    //!< retrieve end const_iterator: last MuonCalibSegment* of the set
    /* retrieve a reference to the MuonCalibSegments-vector*/
    const MuonSegmentVec& muonSegs() const;      //!< retrieve the whole set of MuonCalibSegments: reference to the segment vector

    /* access to track parameters */
    double chi2() const;    //!< retrieve chi2 of the pattern
    double z0() const;      //!< retrieve the z0 (track)parameter of the pattern
    double r0() const;      //!< retrieve the r0 (track)parameter of the pattern
    double invP() const;    //!< retrieve the invP (track)parameter of the pattern
    double phi() const;     //!< retrieve the phi (track)parameter of the pattern
    double theta() const;   //!< retrieve the theta (track)parameter of the pattern

    /* access to number of hits */
    unsigned int nmdtHits() const;    //!< retrieve number of MDT hits. Counting convention: 1000*eta hits + phi hits. Note that MDTs only measure eta hits.
    unsigned int nrpcHits() const;    //!< retrieve number of RPC hits. Counting convention: 1000*eta hits + phi hits.
    unsigned int ntgcHits() const;    //!< retrieve number of TGC hits. Counting convention: 1000*eta hits + phi hits.
    unsigned int ncscHits() const;    //!< retrieve number of CSC hits. Counting convention: 1000*eta hits + phi hits.

    /* methods for adding segments */
    void addMuonSegment(MuonCalibSegment *seg); //!< Adds the MuonCalibSegment to the set MuonCalibSegments of the MuonCalibPattern.

    std::ostream& dump( std::ostream& os) const;

  private:
    MuonSegmentVec m_muonSegments; //!< set of MuonCalibSegments assigned to this MuonCalibPattern

    /* pattern parameters */
    double m_chi2;  //!< Chi2 of the MuonCalibPattern
    double m_z0;    //!< z0 (track)parameter of the MuonCalibPattern
    double m_dist0; //!< d0 (track)parameter of the MuonCalibPattern
    double m_invP;  //!< invP (track)parameter of the MuonCalibPattern
    double m_phi;   //!< phi (track)parameter of the MuonCalibPattern
    double m_theta; //!< theta (track)parameter of the MuonCalibPattern

    unsigned int m_nmdt;  //!< number of MDT hits. Counting convention: 1000*eta hits + phi hits. Note that MDTs only measure eta hits.
    unsigned int m_nrpc;  //!< number of RPC hits. Counting convention: 1000*eta hits + phi hits. 
    unsigned int m_ntgc;  //!< number of TGC hits. Counting convention: 1000*eta hits + phi hits. 
    unsigned int m_ncsc;  //!< number of CSC hits. Counting convention: 1000*eta hits + phi hits. 
  };

  inline unsigned int MuonCalibPattern::muonSegments() const { return m_muonSegments.size(); }
  inline MuonCalibPattern::MuonSegIt MuonCalibPattern::muonSegBegin() { return m_muonSegments.begin(); }
  inline MuonCalibPattern::MuonSegIt MuonCalibPattern::muonSegEnd()   { return m_muonSegments.end(); }  
  inline MuonCalibPattern::MuonSegCit MuonCalibPattern::muonSegBegin() const { return m_muonSegments.begin(); }
  inline MuonCalibPattern::MuonSegCit MuonCalibPattern::muonSegEnd() const { return m_muonSegments.end(); }  

  // track parameters
  inline double MuonCalibPattern::chi2() const { return m_chi2; }
  inline double MuonCalibPattern::z0() const { return m_z0; }
  inline double MuonCalibPattern::r0() const { return m_dist0; }
  inline double MuonCalibPattern::invP() const { return m_invP; }
  inline double MuonCalibPattern::phi() const { return m_phi; }
  inline double MuonCalibPattern::theta() const { return m_theta; }

  inline unsigned int MuonCalibPattern::nmdtHits()const { return m_nmdt; }
  inline unsigned int MuonCalibPattern::nrpcHits()const { return m_nrpc; }
  inline unsigned int MuonCalibPattern::ntgcHits()const { return m_ntgc; }
  inline unsigned int MuonCalibPattern::ncscHits()const { return m_ncsc; }

  // methodes for adding segments
  inline void MuonCalibPattern::addMuonSegment(MuonCalibSegment* seg) { 
    m_muonSegments.push_back( seg); 
  }

}  //namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MuonCalibPattern &pat);

#endif
