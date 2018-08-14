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
 * -# zed      : z position at beamline of RoI
 * -# zedMinus : z position at beamline as rear of RoI 
 * -# zedPlus  : z position at beamline as front of RoI 
 * -# phi      : azimuthal angle (radians) of centre of RoI at origin in range from [-pi, pi]
 * -# phiMinus : most negative phi angle of RoI
 * -# phiPlus  : most positive phi angle of RoI
 * -# eta      : pseudo-rapidity of centre of RoI at origin
 * -# etaPlus  : pseudo-rapidity at zed + zed half-width
 * -# etaMinus : pseudo-rapidity at zed - zed half-width
 * \warning An attempt to cnstruct the objects of this calss with phi0 out of allowed range reasults in throwing exception
 */


#ifndef __TIDAROIDESCRIPTOR_H
#define __TIDAROIDESCRIPTOR_H

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>

#include "TObject.h"

// #include "TrigInDetAnalysis/IRoiDescriptor.h"
#include "TrigInDetAnalysis/TIDARoiParameters.h"


class TIDARoiDescriptor : public TObject { //, public IRoiDescriptor {

public:

  /**
   * @brief default constructor
   */
  TIDARoiDescriptor(bool fullscan=false);


  /**
   * @brief constructor where phi,eta and zed are specified
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI
   * and limits fir each
   */

  TIDARoiDescriptor( double eta,  double etaMinus,   double etaPlus,
		     double phi,  double phiMinus,   double phiPlus,
		     double zed,  double zedMinus,   double zedPlus );


 /**
   * @brief constructor
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI
   * and limits fir each
   */
  TIDARoiDescriptor(unsigned l1id, int id, 
		    double eta,  double etaMinus,   double etaPlus,
		    double phi,  double phiMinus,   double phiPlus,
		    double zed,  double zedMinus,   double zedPlus );
  
 /**
   * @brief constructor
   * @param roiWord LVL1 trigger decision word
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta eta of RoI
   * @param phi phi of RoI
   * @param zed zed of RoI
   * and limits fir each
   */
  TIDARoiDescriptor(unsigned roiWord, unsigned l1id, int id, 
		    double eta,  double etaMinus,   double etaPlus,
		    double phi,  double phiMinus,   double phiPlus,
		    double zed,  double zedMinus,   double zedPlus );
  
  /**
   * @brief copy constructor
   * @param TIDARoiDescriptor
   */
  TIDARoiDescriptor( const TIDARoiDescriptor& r);


  // Destructor
  virtual ~TIDARoiDescriptor();
  
  // Methods to set data members
  //  void phi(const double phi) { m_phi = phi; } //!< set phi of RoI
  //  void eta(const double eta) { m_eta = eta; } //!< set eta of RoI
  //  void zed(const double zed) { m_zed = zed; } //!< set eta of RoI

  void set_roiId(const unsigned int id) { m_roiId=id; }     //!< set roiId
  void set_l1Id(const unsigned int l1id) { m_l1Id=l1id; }             //!< set event number
  void set_roiWord(const unsigned int roiWord) { m_roiWord=roiWord; } //!< sets RoI word

  //  void phiHalfWidth(double x) { m_phiHalfWidth=x; }       //!< sets phi half-width
  //  void etaHalfWidth(double x) { m_etaHalfWidth=x; }       //!< sets eta half-width
  //  void zedHalfWidth(double x) { m_zedHalfWidth=x; }       //!< sets zed half-width

  //  void  etaPlus(double x)  { m_etaPlus  = x; }       //!< sets eta at z+zwidth
  //  void  etaMinus(double x) { m_etaMinus = x; }     //!< sets eta at z-zwidth

  // Methods to retrieve data members

  //  double phi0() const { return m_params.phi(); }
  //  double eta0() const { return m_params.eta(); }
  //  double zed0() const { return m_params.zed(); }

  double phi()  const { return m_params.phi(); }
  double eta()  const { return m_params.eta(); }
  double zed()  const { return m_params.zed(); }

  double etaMinus() const { return m_params.etaMinus(); }
  double etaPlus()  const { return m_params.etaPlus(); }

  double phiMinus() const { return m_params.phiMinus(); }
  double phiPlus()  const { return m_params.phiPlus(); }

  double zedMinus() const { return m_params.zedMinus(); }
  double zedPlus()  const { return m_params.zedPlus(); }


  /// full containment methods
 
  double zedPlusR()  const { return m_zedPlusR; }
  double zedMinusR() const { return m_zedMinusR; }

  double rPlusZed()  const { return m_rPlusZed; }
  double rMinusZed() const { return m_rMinusZed; }

  double maxR()   const { return maxRadius; }
  double maxZ()   const { return maxZed; }

  static double  exitpoint( double tz0, double teta, double& zexit, double& rexit );

  /// identifier methods

  unsigned roiId()   const { return m_roiId; }
  unsigned l1Id()    const { return m_l1Id; }
  unsigned roiWord() const { return m_roiWord; }

  int      version() const { return 4; }  

  operator std::string() const;

  bool isFullscan() const { return m_fullscan; } 

  /// composite RoI methods  

  bool composite() const { return !m_rois.empty(); }

  size_t size() const { return m_rois.size(); }

  const TIDARoiDescriptor* operator[](int i) const { return &m_rois.at(i); }
  const TIDARoiDescriptor* at(int i)         const { return &m_rois.at(i); }

  //!< gets phi half-width
  double phiHalfWidth() const { 
    double dphi = 0.5*(params().phiPlus()-params().phiMinus());
    if ( params().phiMinus()>params().phiPlus() ) dphi += M_PI;
    return dphi;
  }

  double etaHalfWidth() const { return 0.5*(params().etaPlus()-params().etaMinus()); } //!< gets eta half-width
  double zedHalfWidth() const { return 0.5*(params().zedPlus()-params().zedMinus()); } //!< gets eta half-width

  //  double zedPlus()  const { return m_zed+m_zedHalfWidth; } //!< gets phi half-width
  //  double zedMinus() const { return m_zed-m_zedHalfWidth; } //!< gets phi half-width

  //  double etaPlus()  const { return m_etaPlus; }    //!< gets eta at z+zwidth
  //  double etaMinus() const { return m_etaMinus; }   //!< gets eta at z-zwidth

  /// operators


  bool operator==( const TIDARoiDescriptor& b ) {
    if ( roiWord() != b.roiWord() ) return false;
    double epsilon=0.001; // arbitrary , but seems to be reasnable
    if ( std::fabs( phi() - b.phi()) > epsilon ) return false;
    if ( std::fabs( eta() - b.eta()) > epsilon ) return false;
    if ( std::fabs( zed() - b.zed()) > epsilon ) return false;
    return true;
  }

  bool operator!=( const TIDARoiDescriptor& b ) { return !((*this)==b); }


  void push_back( const TIDARoiDescriptor& roi ) { m_rois.push_back( roi ); }
  void push_back( const TIDARoiDescriptor* roi ) { m_rois.push_back( *roi ); }



#if 0

  /// these are all set to 0 or false and not saved - will be fixed eventually

  virtual double zedMin(double r) const { return 0; } //! don't store 
  virtual double zedMax(double r) const { return 0; } //! don't store 

  virtual double rhoMin(double z) const { return 0; } //! don't store 
  virtual double rhoMax(double z) const { return 0; } //! don't store 

   /// return the gradients 
  virtual double dzdrMinus() const { return 0; } //! don't store 
  virtual double dzdrPlus()  const { return 0; } //! don't store 

  virtual double drdzMinus() const { return 0; } //! don't store 
  virtual double drdzPlus()  const { return 0; } //! don't store 

  virtual bool contains( double z0, double dzdr )            const { return false; }  //! don't store 
  virtual bool contains_internal( double z0, double zouter ) const { return false; }  //! don't store 
  
  /// see whether a point is contained within the roi (in phi and r-z)
  virtual bool contains( double _z, double _r, double _phi ) const { return false; } //! don't store 
  virtual bool containsPhi( double _phi )                    const { return false; } //! don't store 
  virtual bool containsZed( double _z, double _r )           const { return false; } //! don't store 

#endif

protected:

  /// access the internal parameter class
  const TIDARoiParameters&  params() const { return m_params; };  //!< internal parameters

  /**
   * @brief resets all varaibles of RoI
   */
  void reset() {
    m_params = TIDARoiParameters();
    //    m_phi=0.0;          m_eta=0.0;          m_zed=0.0;
    //    m_phiHalfWidth=0.1; m_etaHalfWidth=0.1; m_zedHalfWidth=168;
    //    m_etaPlus=m_eta;    m_etaMinus=m_eta;
    m_l1Id=0; 
    m_roiId=0;  
    m_roiWord=0;
    m_rois.clear();
  }

  double phicheck(double phi);  //!< helper function to check if phi range was violated
  double etacheck(double eta);  //!< helper function to check if eta range was violated
  double zedcheck(double zed);  //!< helper function to check if zed range was violated

protected: 

  static double maxRadius; //! don't save 
  static double maxZed;    //! don't save 

protected:

  TIDARoiParameters m_params;   //  actual roi values

  bool         m_fullscan;

  unsigned int m_l1Id;          //< lvl1 event number
  unsigned int m_roiId;         //< RoI number
  unsigned int m_roiWord;       //< lvl1 RoI word from which this RoI was initially constructed

  std::vector<TIDARoiDescriptor>  m_rois;   //<  actual roi descriptors - mot pointers as online

  bool    m_cached;  //! cache flag   - these should be mutable
  double  m_dphi;    //! cached value -  

  double m_zedPlusR;  //! don't save - transient zPlus at radius R
  double m_zedMinusR; //! don't save - transient zMinus at radius R

  double m_rPlusZed;  //! don't save - transient zPlus at radius R
  double m_rMinusZed; //! don't save - transient zMinus at radius R

  double m_tanthetaPlus;   //! don't save - transient roi edge gradients
  double m_tanthetaMinus; //! don't save - transient roi edge gradients
  

  ClassDef( TIDARoiDescriptor, 2 ) 

}; 


inline std::ostream& operator<<( std::ostream& s, const TIDARoiDescriptor& d ) { 
  return s << (std::string)d; 
}

#endif // __ROIDESCRIPTOR_H
