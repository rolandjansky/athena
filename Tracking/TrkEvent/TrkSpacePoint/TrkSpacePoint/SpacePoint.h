/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePoint.h
//   Header file for ABC SpacePoint
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to handle SPs for InDet and Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
// Version 2.0 12/12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////
#ifndef TRKSPACEPOINT_SPACEPOINT_H
#define TRKSPACEPOINT_SPACEPOINT_H


#include "Identifier/IdentifierHash.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include <cassert>
#include <iostream>

class MsgStream;

namespace Trk{

  class LocalParameters;
  class Surface;
  class PrepRawData;

  class SpacePoint : public MeasurementBase {
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    // public because of DataPool
    SpacePoint();
    SpacePoint(const SpacePoint &) = default;
    SpacePoint &operator=(const SpacePoint &) = default;
    // Destructor:
    virtual ~SpacePoint() = default;
	
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    /** return the pair of Ids of the element by reference*/
    const std::pair<IdentifierHash, IdentifierHash>& elementIdList() const;
    
    /** return the pair of cluster pointers by reference */
    const std::pair<const PrepRawData*, const PrepRawData*>& clusterList() const;
	
    /** return the error matrix by reference 
     The Matrix is calculated from the local Covariance Matrix when demanded and the cached */
    const Amg::MatrixX& globCovariance() const;
	
    /** calculate eta (not cached), needs z0 info */
    double eta(double z0=0) const;
    
    /**returns the r value of the SpacePoint's position (in cylindrical coordinates). 
       This is calculated once in the ctor and then cached*/
    double r() const ;
	
    /**returns the phi value of the SpacePoint's position (in cylindrical coordinates).
       This is calculated once in the ctor and then cached*/
    double phi() const ;
	
    /**Interface method to get the associated Surface*/
    virtual const Surface& associatedSurface() const override final;
       
    /**Interface method to get the global Position*/
    virtual const Amg::Vector3D& globalPosition() const override final; 
       
    /** Clone */
    virtual SpacePoint* clone() const override = 0;       

    /** Extended method checking the type*/
    virtual bool type(MeasurementBaseType::Type type) const override final
    {
      return (type==MeasurementBaseType::SpacePoint);
    }

    /**Interface method for output, to be overloaded by child classes* */
    virtual MsgStream&    dump( MsgStream& out ) const override =0 ;  
    /**Interface method for output, to be overloaded by child classes* */
    virtual std::ostream& dump( std::ostream& out ) const override=0 ;

  protected:
    std::pair<const PrepRawData*, const PrepRawData*> m_clusList;
    std::pair<IdentifierHash, IdentifierHash> m_elemIdList;
    Amg::Vector3D m_position; 
    Amg::MatrixX  m_globalCovariance;

    void setupGlobalFromLocalCovariance();
    /** might not be performant enough, evaluate */

  private:
        
  };

  /**Overload of << operator for MsgStream for debug output*/ 
  MsgStream& operator << ( MsgStream& sl, const SpacePoint& spacePoint);

  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const SpacePoint& spacePoint);

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////

  inline const std::pair<IdentifierHash, IdentifierHash>& SpacePoint::elementIdList() const
    { return m_elemIdList; }


  inline const std::pair<const PrepRawData*, const PrepRawData*>& SpacePoint::clusterList() const
    {
      return m_clusList;
    }

  inline double SpacePoint::eta(double z0) const
    {
      //  double zr = (m_z-z0)/m_r; 
      double zr = (m_position.z() - z0)/m_position.perp();
      return log(zr+std::sqrt(1.+zr*zr));
    }

  inline double SpacePoint::r() const
    { return m_position.perp(); }
  
  inline double SpacePoint::phi() const 
    {  return m_position.phi(); }

  // The methods required by MeasurementBase
  inline const Amg::Vector3D& SpacePoint::globalPosition() const
    {
      return m_position;
    }
  inline const Amg::MatrixX& SpacePoint::globCovariance() const {
    return  m_globalCovariance;
  }

  
}//end of ns

#endif // TRKSPACEPOINT_SPACEPOINT_H
