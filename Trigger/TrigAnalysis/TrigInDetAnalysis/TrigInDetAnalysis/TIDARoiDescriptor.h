// emacs: this is -*- c++ -*-
//
//   @file    TIDARoiDescriptor.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TIDARoiDescriptor.h, v0.0   Tue 10 Nov 2009 10:05:35 GMT sutt $

/**
 * @brief Describes the Region of Ineterest geometry
 *  It has basically 8 parameters
 *
 * -# zed0 : z position of RoI
 * -# zed half-width
 * -# phi0 : azimuthal angle (radians) of centre of RoI at origin in range from [-pi, pi]
 * -# phi half-width
 * -# eta0 : pseudo-rapidity of centre of RoI at origin
 * -# eta half-width
 * -# etaPlus  : pseudo-rapidity at zed0 + zed half-width
 * -# etaMinus : pseudo-rapidity at zed0 - zed half-width
 * \warning An attempt to cnstruct the objects of this calss with phi0 out of allowed range reasults in throwing exception
 */


#ifndef __TIDAROIDESCRIPTOR_H
#define __TIDAROIDESCRIPTOR_H

#include <iostream>
#include <cmath>

#include "TObject.h"



class TIDARoiDescriptor : public TObject {

public:

  /**
   * @brief default constructor
   */
  TIDARoiDescriptor();


  /**
   * @brief constructor where phi,eta and zed are specified
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI

  */
  TIDARoiDescriptor(double eta,      double phi,      double zed,
		double etaw=0.1, double phiw=0.1, double zedw=168);

 /**
   * @brief constructor
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI
   */
  TIDARoiDescriptor(unsigned int l1id, int id, 
		double eta, double phi, double zed,
		double etaw=0.1, double phiw=0.1, double zedw=168);


 /**
   * @brief constructor
   * @param roiWord LVL1 trigger decision word
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI
   */
  TIDARoiDescriptor(unsigned int roiWord, unsigned int l1id, int id, 
		double eta,      double phi,      double zed, 
		double etaw=0.1, double phiw=0.1, double zedw=168);

  /**
   * @brief copy constructor
   * @param TIDARoiDescriptor
   */
  TIDARoiDescriptor( const TIDARoiDescriptor& r);


  // Destructor
  virtual ~TIDARoiDescriptor();
  
  // Methods to set data members
  void phi0(const double phi) { m_phi0 = phi; } //!< set phi0 of RoI
  void eta0(const double eta) { m_eta0 = eta; } //!< set eta0 of RoI
  void zed0(const double zed) { m_zed0 = zed; } //!< set eta0 of RoI
  void set_roiId(const unsigned int id) { m_roiId=id; }     //!< set roiId
  void set_l1Id(const unsigned int l1id) { m_l1Id=l1id; }             //!< set event number
  void set_roiWord(const unsigned int roiWord) { m_roiWord=roiWord; } //!< sets RoI word

  void phiHalfWidth(double x) { m_phiHalfWidth=x; }       //!< sets phi half-width
  void etaHalfWidth(double x) { m_etaHalfWidth=x; }       //!< sets eta half-width
  void zedHalfWidth(double x) { m_zedHalfWidth=x; }       //!< sets zed half-width

  void  etaPlus(double x)  { m_etaPlus  = x; }       //!< sets eta at z+zwidth
  void  etaMinus(double x) { m_etaMinus = x; }     //!< sets eta at z-zwidth

  // Methods to retrieve data members

  double phi0() const { return m_phi0; }
  double phi()  const { return m_phi0; }
  double eta0() const { return m_eta0; }
  double eta()  const { return m_eta0; }
  double zed0() const { return m_zed0; }
  double zed()  const { return m_zed0; }

  unsigned int roiId() const { return m_roiId; }
  unsigned int l1Id() const { return m_l1Id; }
  unsigned int roiWord() const { return m_roiWord; }

  double phiHalfWidth() const { return m_phiHalfWidth; } //!< gets phi half-width
  double etaHalfWidth() const { return m_etaHalfWidth; } //!< gets eta half-width
  double zedHalfWidth() const { return m_zedHalfWidth; } //!< gets zed half-width

  double zedPlus()  const { return m_zed0+m_zedHalfWidth; } //!< gets phi half-width
  double zedMinus() const { return m_zed0-m_zedHalfWidth; } //!< gets phi half-width

  double etaPlus()  const { return m_etaPlus; }    //!< gets eta at z+zwidth
  double etaMinus() const { return m_etaMinus; }   //!< gets eta at z-zwidth


  /// operators
  TIDARoiDescriptor& operator=(const TIDARoiDescriptor& a) {
    if (this != &a) {
      m_phi0 = a.m_phi0;
      m_eta0 = a.m_eta0;
      m_zed0 = a.m_zed0;
      m_phiHalfWidth = a.m_phiHalfWidth;
      m_etaHalfWidth = a.m_etaHalfWidth;
      m_zedHalfWidth = a.m_zedHalfWidth;
      m_etaPlus  = a.m_etaPlus;
      m_etaMinus = a.m_etaMinus; 
      m_l1Id    = a.m_l1Id;
      m_roiId   = a.m_roiId;
      m_roiWord = a.m_roiWord;
    }
    return *this;
  }


  bool operator==( const TIDARoiDescriptor& b ) {
    if ( roiWord() != b.roiWord() ) return false;
    double epsilon=0.001; // arbitrary , but seems to be reasnable
    if ( std::fabs( phi0() - b.phi0()) > epsilon ) return false;
    if ( std::fabs( eta0() - b.eta0()) > epsilon ) return false;
    if ( std::fabs( zed0() - b.zed0()) > epsilon ) return false;
    return true;
  }

  bool operator!=( const TIDARoiDescriptor& b ) { return !((*this)==b); }


protected:

  /**
   * @brief resets all varaibles of RoI
   */
  void reset() {
    m_phi0=0.0;         m_eta0=0.0;         m_zed0=0.0;
    m_phiHalfWidth=0.1; m_etaHalfWidth=0.1; m_zedHalfWidth=168;
    m_etaPlus=m_eta0;   m_etaMinus=m_eta0;
    m_l1Id=0;           m_roiId=0;          m_roiWord=0;
  }
  
  double phicheck(double phi);  //!< helper function to check if phi range was violated
  double etacheck(double eta);  //!< helper function to check if eta range was violated
  double zedcheck(double zed);  //!< helper function to check if zed range was violated

protected:

  float m_phi0; 
  float m_eta0; 
  float m_zed0; 

  float m_phiHalfWidth;         //< half-width of RoI in azimuthal
  float m_etaHalfWidth;         //< half-width of RoI in pseudo-rapidity
  float m_zedHalfWidth;         //< half-width of RoI in zed
  float m_etaPlus;              //< eta of RoI at zed0+zedHalfWidth
  float m_etaMinus;             //< eta of RoI at zed0-zedHalfWidth

  unsigned int m_l1Id;          //< lvl1 event number
  unsigned int m_roiId;         //< RoI number
  unsigned int m_roiWord;       //< lvl1 RoI word from which this RoI was initially constructed

  ClassDef( TIDARoiDescriptor, 1 ) 

}; 


//<! printing helper
inline std::ostream& operator<<( std::ostream& s, const TIDARoiDescriptor& d ) { 
  s << " z: "   << d.zed0() << " +/- " <<  d.zedHalfWidth() 
    << " eta: " << d.eta0() << " +/- " <<  d.etaHalfWidth() << " [ eta(z+) " << d.etaPlus() << "  eta(z-) " << d.etaMinus() << " ] " 
    << " phi: " << d.phi0() << " +/- " << d.phiHalfWidth() 
    << " RoIid: " << d.roiId() << " RoIword: " << d.roiWord();
  return s;
}

#if 0
inline std::ostream& operator<<(std::ostream& s, const TIDARoiDescriptor& a) { 
  return s << "[ " 
	   << "\t" << a.phi0() 
	   << "\t" << a.eta0() 
	   << "\t" << a.zed0() 
	   << " ]";
} 
#endif

#endif // __ROIDESCRIPTOR_H
