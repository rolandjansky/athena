// emacs: this is -*- c++ -*-
//
//   @file    RoiDescriptor.h        
//
//             RoiDescriptor class - includes constructors in preparation
//             for enforcement of correct roi sizes       
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptor.h, v0.0   Fri 08 Jun 2013 23:52:09 CEST sutt $


#ifndef ROIDESCRIPTOR_ROIDESCRIPTOR_H
#define ROIDESCRIPTOR_ROIDESCRIPTOR_H

#include <iostream>

#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"


/**
 * @brief Describes the Region of Ineterest geometry
 *  It has basically 9 parameters
 *
 * -# zed          : central z position of RoI
 * -# zedMinus     : most negative z position of the roi
 * -# zedPlus      : most positive z position of the roi
 * -# phi          : azimuthal angle (radians) of "centre" of RoI at origin in range from [-pi, pi]
 * -# phiMinus     : the most      clockwise (negative phi direction) phi value for the RoI
 * -# phiPlus      : the most anti-clockwise (positive phi direction) phi value for the RoI
 * -# eta          : pseudo-rapidity of "centre" of RoI 
 * -# etaMinus     : pseudo-rapidity at zedMinus
 * -# etaPlus      : pseudo-rapidity at zedPlus
 */

class RoiDescriptor : public IRoiDescriptor {

public:

  /**
   * @brief default constructor
   * @param fullscan      if fullscan is true, this RoI will span the entire detector
   */
  RoiDescriptor(bool fullscan=false);

  /**
   * @brief constructor
   * @param eta      eta of RoI
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phi      phi of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   * @param zed      zed of RoI
   * @param zedMinus zed at rear  of RoI
   * @param zedPlus  zed at front of RoI
   */
  RoiDescriptor(double eta_,   double etaMinus_,   double etaPlus_, 
		double phi_,   double phiMinus_,   double phiPlus_, 
		double zed_=0, double zedMinus_=-zedWidthDefault, double zedPlus_=zedWidthDefault   );
  /**
   * @brief constructor
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   */
  RoiDescriptor(double etaMinus_, double etaPlus_, 
		double phiMinus_, double phiPlus_ ); 

  /**
   * @brief copy constructor
   */
  RoiDescriptor( const IRoiDescriptor& roi );

  RoiDescriptor& operator=( const IRoiDescriptor& r );  
  
  // Destructor
  virtual ~RoiDescriptor();


  // Methods to retrieve data members

  double phi() const { return m_phi; }
  double eta() const { return m_eta; }
  double zed() const { return m_zed; }

  /// these quantities probably don't need to be used any more 
  /// - they are implemented here only because we had them in 
  ///   the original legacy interface
 
  virtual unsigned int roiId()   const { return 0; }
  virtual unsigned int l1Id()    const { return 0; }
  virtual unsigned int roiWord() const { return 0; }

  double zedPlus()  const { return m_zedPlus; } //!< z at the most forward end of the RoI
  double zedMinus() const { return m_zedMinus; } //!< z at the most backward end of the RoI

  double etaPlus()  const { return m_etaPlus; }   //!< gets eta at zedPlus
  double etaMinus() const { return m_etaMinus; }   //!< gets eta at zMinus

  double phiPlus()  const  { return m_phiPlus; }    //!< gets phiPlus
  double phiMinus() const  { return m_phiMinus; }   //!< gets phiMinus


  /// versioning 
  int   version() const   { return m_version; } 
  void  version(int v)    { m_version = v; } 


  /// output
  virtual operator std::string() const;


  /// is this a full scan RoI?
  bool  isFullscan() const { return m_fullscan; }
  
  /// SuperRoI compatability methods

  /// am I a SuperRoi?
  virtual bool composite() const          { return m_composite; }
  virtual void setComposite(bool b=true)  { m_composite=b; } 

  /// always manage constituents ???
  virtual bool manageConstituents() const  { return m_manageConstituents; }
  virtual void manageConstituents(bool b)  { m_manageConstituents=b; }

  /// number of constituents
  virtual unsigned size() const { return m_roiDescriptors.size(); }

  /// find an RoiDescriptor constituent
  virtual const IRoiDescriptor* at(int i) const { return m_roiDescriptors.at(i); }

  /// clear the vector                                                                                                                               
  void clear()  { m_roiDescriptors.clear(); }  // setComposite(false); }

  /// add and RoiDescriptor                                                                                                                           
  void push_back(const IRoiDescriptor* roi) { m_roiDescriptors.push_back(roi); setComposite(true); }

  /// iterators                                                                                                                                       
  //  std::vector<const IRoiDescriptor*>::const_iterator begin() const { return m_roiDescriptors.begin(); }
  //  std::vector<const IRoiDescriptor*>::const_iterator end()   const { return m_roiDescriptors.end(); }
  roi_iterator  begin() const { return m_roiDescriptors.begin(); }
  roi_iterator  end()   const { return m_roiDescriptors.end(); }



  /// methods to determine whether coordinates or stubs are within the RoI 

  /// return the gradients 
  double dzdrMinus() const { return m_dzdrMinus; }       //!<  dz/dr at the rear of the RoI
  double dzdrPlus()  const { return m_dzdrPlus; }        //!<  dz/dr at the front of the RoI

  double drdzMinus() const { return m_drdzMinus; }       //!<  dr/dz at the rear of the RoI
  double drdzPlus()  const { return m_drdzPlus; }        //!<  dr/dz at the front of the RoI

  /// methods to calculate z position at the RoI boundary 
  /// at a given radius
  double zedMin(const double r) const;
  double zedMax(const double r) const; 

  double zedOuterPlus()  const { return m_zedOuterPlus; } //!< z at the most forward end of the RoI
  double zedOuterMinus() const { return m_zedOuterMinus; } //!< z at the most backward end of the RoI

  double rhoMin(const double z) const; 
  double rhoMax(const double z) const; 

protected:

  friend class TrigRoiDescriptorCnv_p2;
  friend class TrigRoiDescriptorCnv_p3;

  /**
   * @brief construct RoiDescriptor internals -  similar to constructors
   * @see  RoiDescriptor( .... )
   */
  void construct(double eta, double etaMinus, double etaPlus, 
		 double phi, double phiMinus, double phiPlus, 
		 double zed, double zedMinus, double zedPlus); 

  void construct( const IRoiDescriptor& _roi );

protected:

  /// default parameters - there may be better ways, but this will do
  static const double zedWidthDefault;

protected:

  float m_phi;                 //!< phi of RoI center
  float m_eta;                 //!< eta of RoI center
  float m_zed;                 //!< zed of RoI center

  float m_phiMinus;            //!< most negative RoI in azimuthal
  float m_phiPlus;             //!< most positive RoI in azimuthal 
  float m_etaMinus;             //!< eta of RoI at zedMinus
  float m_etaPlus;              //!< eta of RoI at zedPlus
  float m_zedMinus;             //!< z position at most negative position along the beamline
  float m_zedPlus;              //!< z position at most positive position along the beamline

  float m_dzdrMinus;       //!<  dz/dr at the rear of the RoI
  float m_dzdrPlus;        //!<  dz/dr at the front of the RoI

  float m_drdzMinus;    //!<  dr/dz at the rear of the RoI
  float m_drdzPlus;     //!<  dr/dz at the front of the RoI

  float m_zedOuterMinus;  //!< z at rear of RoI at the outer radius ( = 1100 mm) 
  float m_zedOuterPlus;   //!< z at front of RoI at the outer radius ( = 1100 mm) 

  bool m_fullscan;             //!< flag this as a full detector RoI
  bool m_composite;            //!< flag this as a composite RoI
  bool m_manageConstituents;   //!< flag to determine whether consituents should be managed

  int  m_version;              //!< transient version identifier

  std::vector<const IRoiDescriptor*>  m_roiDescriptors;  //!< roi constituents
  
};


std::string str( const RoiDescriptor& d );                           //<! printing helper
std::ostream& operator<<( std::ostream& m, const RoiDescriptor& d ); //<! printing helper (wraps above)

#endif // ROIDESCRIPTOR_H

