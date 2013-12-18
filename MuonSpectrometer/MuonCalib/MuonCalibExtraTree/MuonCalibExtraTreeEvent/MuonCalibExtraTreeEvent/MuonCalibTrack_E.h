/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBTRACK_E_H
#define MUONCALIBTRACK_E_H

#include <vector>
#include <cmath>

namespace MuonCalib {
  class MuonCalibHit_E;
  class MuonCalibHole_E;
  class MuonCalibSegment;
  /**
     @class MuonCalibTrack_E

     Simplified class designed to store information of a Track. It has trackparameters and a set of MuonCalib::MuonCalibHit_E s and MuonCalib::MuonCalibHole_E s.

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonCalibTrack_E {
  public:
    typedef std::vector<MuonCalibHit_E*> HitVector;   //!< typedef of a set of MuonCalib::MuonCalibHit_E s
    typedef std::vector<MuonCalibHole_E*> HoleVector; //!< typedef of a set of MuonCalib::MuonCalibHole_E s

    MuonCalibTrack_E();                               //!< default constructor
    MuonCalibTrack_E( double x0, double y0, double z0, double phi, double theta, double qOverP, int author, 
                      double cov00, double cov01, double cov02, double cov03, double cov04, 
                                    double cov11, double cov12, double cov13, double cov14, 
                                                  double cov22, double cov23, double cov24, 
                                                                double cov33, double cov34, 
                                                                              double cov44, 
		      double chi2, int ndof); //!< constructor setting all trackparameters and position
    MuonCalibTrack_E( const MuonCalibTrack_E& pat ) ; //!< copyconstructor
    virtual ~MuonCalibTrack_E();                           //!< destructor
  
    HitVector::iterator beginHit()             { return m_hits.begin(); } ;       //!< retrieve an iterator of the first MuonCalib::MuonCalibHit_E of the track
    HitVector::iterator endHit()               { return m_hits.end() ; } ;        //!< retrieve an iterator of the last MuonCalib::MuonCalibHit_E of the track
    HitVector::const_iterator beginHit() const { return m_hits.begin(); } ;       //!< retrieve a const_ iterator of the first MuonCalib::MuonCalibHit_E of the track
    HitVector::const_iterator endHit() const   { return m_hits.end() ; } ;        //!< retrieve a const_ iterator of the last MuonCalib::MuonCalibHit_E of the track
    HoleVector::iterator beginHole()             { return m_holes.begin(); } ;    //!< retrieve an iterator of the first MuonCalib::MuonCalibHole_E of the track
    HoleVector::iterator endHole()               { return m_holes.end() ; } ;     //!< retrieve an iterator of the last MuonCalib::MuonCalibHole_E of the track
    HoleVector::const_iterator beginHole() const { return m_holes.begin(); } ;    //!< retrieve a const_ iterator of the first MuonCalib::MuonCalibHole_E of the track
    HoleVector::const_iterator endHole() const   { return m_holes.end() ; } ;     //!< retrieve a const_ iterator of the last MuonCalib::MuonCalibHole_E of the track

    MuonCalibTrack_E& operator=( const MuonCalibTrack_E& rhs ) ;  //!< assignment operator
    unsigned int nrHits() const { return m_hits.size() ;} ;       //!< returns number of MuonCalib::MuonCalibHit_E s on track
    unsigned int nrHoles() const { return m_holes.size() ;} ;     //!< returns number of MuonCalib::MuonCalibHole_E s on track
      
    float x0() const {return m_x0;}                   //!< returns trackparameter x0
    float y0() const {return m_y0;}                   //!< returns trackparameter y0
    float z0() const {return m_z0;}                   //!< returns trackparameter z0
    virtual float d0() const;                                 //!< returns trackparameter d0
    virtual float z0ip() const;                               //!< returns trackparameter z0 at IP
    float phi() const {return m_phi;}                 //!< returns trackparameter phi
    float theta() const {return m_theta;}             //!< returns trackparameter theta
    float qOverP() const {return m_qOverP;}           //!< returns trackparameter q/p
    float p() const {return std::fabs(m_qOverP) > 1e-9 ? std::fabs(1./qOverP()) : 1e9;}           //!< returns momentum
    float pt() const { return std::sin(theta())*p(); }           //!< returns pt
    float errInvP() const {return m_cov44;}           //!< returns the error**2 on trackparameter q/p
    int author() const {return m_author;}             //!< returns the author 
    float cov00() const {return m_cov00;}             //!< returns the covariance matrix elements
    float cov01() const {return m_cov01;}             //!< returns the covariance matrix elements
    float cov02() const {return m_cov02;}             //!< returns the covariance matrix elements
    float cov03() const {return m_cov03;}             //!< returns the covariance matrix elements
    float cov04() const {return m_cov04;}             //!< returns the covariance matrix elements
    float cov11() const {return m_cov11;}             //!< returns the covariance matrix elements
    float cov12() const {return m_cov12;}             //!< returns the covariance matrix elements
    float cov13() const {return m_cov13;}             //!< returns the covariance matrix elements
    float cov14() const {return m_cov14;}             //!< returns the covariance matrix elements
    float cov22() const {return m_cov22;}             //!< returns the covariance matrix elements
    float cov23() const {return m_cov23;}             //!< returns the covariance matrix elements
    float cov24() const {return m_cov24;}             //!< returns the covariance matrix elements
    float cov33() const {return m_cov33;}             //!< returns the covariance matrix elements
    float cov34() const {return m_cov34;}             //!< returns the covariance matrix elements
    float cov44() const {return m_cov44;}             //!< returns the covariance matrix elements
    float chi2() const {return m_chi2;}               //!< returns track chi2
    int   ndof() const { return m_ndof; }              //!< returns the number of degrees of freedom
    inline const std::vector<const MuonCalibSegment *> & segmetnsOnTrack() const
    	{
	return m_segments_on_track;
	}
    void setX0(float x0) {m_x0 = x0;}                 //!< sets trackparameter x0
    void setY0(float y0) {m_y0 = y0;}                 //!< sets trackparameter y0
    void setZ0(float z0) {m_z0 = z0;}                 //!< sets trackparameter z0
    void setPhi(float phi) {m_phi = phi;}             //!< sets trackparameter phi
    void setTheta(float theta) {m_theta = theta;}     //!< sets trackparameter theta
    void setQOverP(float qOverP) {m_qOverP = qOverP;} //!< sets trackparameter q/p
//    void setErrInVP(float errInvP) {m_errinvP = errInvP;} //!< sets error trackparameter q/p
    void setAuthor(int author) {m_author = author;}   //!< sets author  
    void setCov00(float cov00) {m_cov00 = cov00;}     //!< sets covariance matrix 
    void setCov01(float cov01) {m_cov01 = cov01;}     //!< sets covariance matrix 
    void setCov02(float cov02) {m_cov02 = cov02;}     //!< sets covariance matrix 
    void setCov03(float cov03) {m_cov03 = cov03;}     //!< sets covariance matrix 
    void setCov04(float cov04) {m_cov04 = cov04;}     //!< sets covariance matrix 
    void setCov11(float cov11) {m_cov11 = cov11;}     //!< sets covariance matrix 
    void setCov12(float cov12) {m_cov12 = cov12;}     //!< sets covariance matrix 
    void setCov13(float cov13) {m_cov13 = cov13;}     //!< sets covariance matrix 
    void setCov14(float cov14) {m_cov14 = cov14;}     //!< sets covariance matrix 
    void setCov22(float cov22) {m_cov22 = cov22;}     //!< sets covariance matrix 
    void setCov23(float cov23) {m_cov23 = cov23;}     //!< sets covariance matrix 
    void setCov24(float cov24) {m_cov24 = cov24;}     //!< sets covariance matrix 
    void setCov33(float cov33) {m_cov33 = cov33;}     //!< sets covariance matrix 
    void setCov34(float cov34) {m_cov34 = cov34;}     //!< sets covariance matrix 
    void setCov44(float cov44) {m_cov44 = cov44;}     //!< sets covariance matrix 
    void setChi2(float chi2) {m_chi2 = chi2;}         //!< sets track chi2
    void setNdof(int ndof) {m_ndof = ndof;}           //!< sets ndof 

    void addHit( MuonCalibHit_E* hit ) ;               //!< Add a MuonCalib::MuonCalibHit_E to the track
    void addHole( MuonCalibHole_E* hole ) ;            //!< Add a MuonCalib::MuonCalibHole_E to the track
    inline void addSegmentOnTrack(const MuonCalibSegment *s)
    	{
	m_segments_on_track.push_back(s);
	}
  private:
    /** copy content of track into this object. DOES NOT clear! */
    void copy( const MuonCalibTrack_E& track );
    
    /** frees memory of current object */
    void clear();

    HitVector m_hits;         //!< set of MuonCalib::MuonCalibHit_E s on track
    HoleVector m_holes;       //!< set of MuonCalib::MuonCalibHole_E s on track
    std::vector<const MuonCalibSegment *> m_segments_on_track;
    float m_x0;               
    float m_y0;
    float m_z0;
    float m_phi;
    float m_theta;
    float m_qOverP;
//    float m_errinvP;
    int  m_author;
    float m_cov00;
    float m_cov01;
    float m_cov02;
    float m_cov03;
    float m_cov04;
    float m_cov11;
    float m_cov12;
    float m_cov13;
    float m_cov14;
    float m_cov22;
    float m_cov23;
    float m_cov24;
    float m_cov33;
    float m_cov34;
    float m_cov44;
    float m_chi2;
    int   m_ndof;
  };

}//namespace MuonCalib

inline float MuonCalib::MuonCalibTrack_E::d0()const{return -m_x0*std::sin(m_phi) + m_y0*std::cos(m_phi);}

#endif //MUONCALIBTRACK_E_H
