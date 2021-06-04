/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasurementBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKMEASUREMENTBASE_MEASUREMENTBASE_H
#define TRKMEASUREMENTBASE_MEASUREMENTBASE_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
//Trk
#include "TrkEventPrimitives/LocalParameters.h"

// I/O
#include <iosfwd>
#include <memory>
class MsgStream;

namespace Trk {

  class LocalParameters;
  class ErrorMatrix;
  class Surface;

  /**@class MeasurementBase
  
    This class is the pure abstract base class for all fittable tracking measurements.
    It holds the minimal information, such as LocalParameters, ErrorMatrix, 
    a Surface and a GlobalPosition.
    
    The associatedSurface and the globalPosition are returned by const reference
    
    @author Andreas.Salzburger@cern.ch
  
  */

namespace MeasurementBaseType{
/* The various kind of MeasurementBase 
 * to avoid dynamic_cast via using the type method
 */
enum Type{
  Segment                  =0,
  SpacePoint               =1,
  RIO_OnTrack              =2,
  CompetingRIOsOnTrack     =3,
  PseudoMeasurementOnTrack =4,
  VertexOnTrack            =5,
  CaloCluster_OnTrack      =6
}; 
}


  class MeasurementBase {
    
    public:
       /** Default constructor - needed for POOL/SEAL */
       MeasurementBase()= default;
       
       /** constructor  */
       MeasurementBase( const LocalParameters& pars, const Amg::MatrixX& cov ) : 
         m_localParams(pars), 
         m_localCovariance(cov)
      {}
       
       /** Virtual destructor */
       virtual ~MeasurementBase()= default;
    
       /**Pseudo-Constructor */
       virtual MeasurementBase* clone() const = 0;
       
       /**NVI Clone giving up unique pointer */
       std::unique_ptr<MeasurementBase> uniqueClone() const{
         return std::unique_ptr<MeasurementBase>(clone());
       }
    
       /**Interface method to get the LocalParameters*/
       const LocalParameters& localParameters() const;
       
       /**Interface method to get the localError*/
       const Amg::MatrixX& localCovariance() const;
  
       /**Interface method to get the associated Surface*/
       virtual const Surface& associatedSurface() const = 0;
       
       /**Interface method to get the global Position*/
       virtual const Amg::Vector3D& globalPosition() const = 0;

       /** Interface method checking the type*/
       virtual bool type(MeasurementBaseType::Type type) const =0;  
       
       /**Interface method for output, to be overloaded by child classes* */
       virtual MsgStream& dump( MsgStream& out ) const = 0;  

       /**Interface method for output, to be overloaded by child classes* */
       virtual std::ostream& dump( std::ostream& out ) const = 0;
              
  protected:
       LocalParameters  m_localParams;
       Amg::MatrixX     m_localCovariance;
  };

/**Overload of << operator for MsgStream for debug output*/ 
inline MsgStream& operator << ( MsgStream& sl, const Trk::MeasurementBase& mbase)
  { return mbase.dump(sl); }


/**Overload of << operator for std::ostream for debug output*/ 
inline std::ostream& operator << ( std::ostream& sl, const Trk::MeasurementBase& mbase)
  { return mbase.dump(sl); }
 
}

inline const Trk::LocalParameters& Trk::MeasurementBase::localParameters() const {
  return m_localParams;
}
       
inline const Amg::MatrixX& Trk::MeasurementBase::localCovariance() const {
  return m_localCovariance;
}

#endif


