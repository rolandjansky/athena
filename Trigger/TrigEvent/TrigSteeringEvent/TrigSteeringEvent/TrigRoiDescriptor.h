// emacs: this is -*- c++ -*-
//
//   @file    TrigRoiDescriptor.h        
//
//             RoiDescriptor class - includes constructors in preparation
//             for enforcement of correct roi sizes       
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigRoiDescriptor.h, v0.0   Fri 08 Jun 2013 23:52:09 CEST sutt $


#ifndef TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H
#define TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H
#include <stdint.h>
#include <map>
#include <ostream>


#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/MsgStream.h"


#include "IRegionSelector/IRoiDescriptor.h"



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

class TrigRoiDescriptor : public IRoiDescriptor {

public:

  typedef std::vector<const IRoiDescriptor*>::const_iterator roi_itr;

public:

  /**
   * @brief default constructor
   * @param fullscan      if fullscan is true, this RoI will span the entire detector
   */
  TrigRoiDescriptor(bool fullscan=false);

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
  TrigRoiDescriptor(double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);
  
  /**
   * @brief constructor
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
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
  TrigRoiDescriptor(unsigned int l1id, unsigned int id, 
		    double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);

  /**
   * @brief constructor
   * @param roiWord LVL1 trigger decision word
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
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
  TrigRoiDescriptor(unsigned int roiWord, unsigned int l1id, unsigned int id, 
		    double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);


  // Destructor
  ~TrigRoiDescriptor();


  // Methods to retrieve data members

  double phi() const { return m_phi0; }
  double eta() const { return m_eta0; }
  double zed() const { return m_zed0; }

  unsigned int roiId() const { return m_roiId; }
  unsigned int l1Id() const { return m_l1Id; }
  unsigned int roiWord() const { return m_roiWord; }

  /// these accessors will all be *sloooow* because they are computing 
  /// the limits dynamicall *every* time, if *only* we could change the
  /// internal representation to store the (more useful) limits rather 
  /// than these stupid "halfwidths"  

  double zedPlus()  const { return m_zed0+m_zedHalfWidth; } //!< z at the most forward end of the RoI
  double zedMinus() const { return m_zed0-m_zedHalfWidth; } //!< z at the most backward end of the RoI

  double etaPlus()  const { return m_eta0+m_etaHalfWidth; }   //!< gets eta at zedPlus
  double etaMinus() const { return m_eta0-m_etaHalfWidth; }   //!< gets eta at zMinus

  double phiPlus()  const;
  double phiMinus() const;

  /// return the gradients 
  double dzdrMinus() const { return m_dzdrMinus; }       //!<  dz/dr at the rear of the RoI
  double dzdrPlus()  const { return m_dzdrPlus; }        //!<  dz/dr at the front of the RoI

  double drdzMinus() const { return m_drdzMinus; }       //!<  dr/dz at the rear of the RoI
  double drdzPlus()  const { return m_drdzPlus; }        //!<  Dr/dz at the front of the RoI



  /// versioning and output
  int   version() const { return m_version; } 

  /// output
  operator std::string() const;

  /// is this a full scan RoI?
  bool  isFullscan() const { return m_fullscan; }



  
  /// SuperRoI compatability methods

  /// am I a SuperRoi?
  virtual bool composite() const          { return m_composite; }
  virtual void setComposite(bool b=true)  { m_composite=b; }

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
  std::vector<const IRoiDescriptor*>::const_iterator begin() const { return m_roiDescriptors.begin(); }
  std::vector<const IRoiDescriptor*>::const_iterator end()   const { return m_roiDescriptors.end(); }



  /// methods to determine whether coordinates or studs are within the RoI 

  /// methods to calculate z position at the RoI boundary 
  /// at a given radius
  double zedMin(const double r) const;
  double zedMax(const double r) const; 

  double rhoMin(const double z) const; 
  double rhoMax(const double z) const; 

  /// test whether a stub is contained within the roi
  virtual bool contains( double z0, double dzdr ) const;
  virtual bool contains_internal( double z0, double zouter ) const;

  /// test whether a point is contained within the RoI 
  virtual bool contains( double _z, double _r, double _phi ) const;
  virtual bool containsPhi( double _phi ) const;
  virtual bool containsZed( double _z, double _r ) const;




#if 0
  // Methods to set data members
  void phi(const double _phi) { m_phi0 = _phi; } //!< set phi0 of RoI
  void eta(const double _eta) { m_eta0 = _eta; } //!< set eta0 of RoI
  void zed(const double _zed) { m_zed0 = _zed; } //!< set eta0 of RoI
  void set_roiId(const unsigned int id) { m_roiId=id; }     //!< set roiId
  void set_l1Id(const unsigned int l1id) { m_l1Id=l1id; }             //!< set event number
  void set_roiWord(const unsigned int roiWord) { m_roiWord=roiWord; } //!< sets RoI word

  /**
   * @brief resets all varaibles of RoI
   */
  void reset() {
    m_phi0=0.0;
    m_eta0=0.0;
    m_zed0=0.0;
    m_phiHalfWidth=0.0;
    m_etaHalfWidth=0.0;
    m_zedHalfWidth=zedWidthDefault;
    m_etaPlus  = m_eta0;
    m_etaMinus = m_eta0;
    m_l1Id=0;
    m_roiId=0;
    m_roiWord=0;
  }
#endif



protected:

  /**
   * @brief construct RoiDescriptor internals -  similar to constructors
   * @see  TrigRoiDescriptor( .... )
   */
  void construct(double eta, double etaMinus, double etaPlus, 
		 double phi, double phiMinus, double phiPlus, 
		 double zed, double zedMinus, double zedPlus); 


private:

  double phicheck(double _phi);  //!< helper function to check if phi range was violated
  double etacheck(double _eta);  //!< helper function to check if eta range was violated
  double zedcheck(double _zed);  //!< helper function to check if zed range was violated


protected:

  /// default parameters - there may be better ways, but this will do
  static const double zedWidthDefault;

protected:

  float m_phi0;                 //!< phi of RoI center
  float m_eta0;                 //!< eta of RoI center
  float m_zed0;                 //!< zed of RoI center
  float m_phiHalfWidth;         //!< half-width of RoI in azimuthal
  float m_etaHalfWidth;         //!< half-width of RoI in pseudo-rapidity
  float m_zedHalfWidth;         //!< half-width of RoI in zed
  float m_etaPlus;              //!< eta of RoI at zed0+zedHalfWidth
  float m_etaMinus;             //!< eta of RoI at zed0-zedHalfWidth

  float m_dzdrMinus;       //!<  dz/dr at the rear of the RoI
  float m_dzdrPlus;       //!<  dz/dr at the front of the RoI

  float m_drdzMinus;    //!<  dr/dz at the rear of the RoI
  float m_drdzPlus;    //!<  dr/dz at the front of the RoI

  float m_zedOuterMinus;  //!< z at rear of RoI at the outer radius ( = 1100 mm) 
  float m_zedOuterPlus;   //!< z at front of RoI at the outer radius ( = 1100 mm) 


  unsigned int m_l1Id;          //!< lvl1 event number
  unsigned int m_roiId;         //!< RoI number
  unsigned int m_roiWord;       //!< lvl1 RoI word from which this RoI was initially constructed

  bool m_fullscan;             //!< flag this as a full detector RoI
  bool m_composite;            //!< flag this as a composite RoI
  bool m_manageConstituents;   //!< flag to determine whether consituents should be managed

  int  m_version;              //!< transient version identifier

  std::vector<const IRoiDescriptor*>  m_roiDescriptors;  //!< roi constituents
  

  std::vector<uint32_t> m_serialized;

};


std::string str( const TrigRoiDescriptor& d );                           //<! printing helper
std::ostream& operator<<( std::ostream& m, const TrigRoiDescriptor& d ); //<! printing helper (wraps above)


CLASS_DEF(TrigRoiDescriptor, 6455, 1)


#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#endif // TRIGROIDESCRIPTOR_H

