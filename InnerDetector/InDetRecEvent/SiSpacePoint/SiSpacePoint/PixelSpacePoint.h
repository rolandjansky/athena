/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelSpacePoint.h
//   Header file for class PixelSpacePoint
///////////////////////////////////////////////////////////////////
// Class to handle SPs for Pixel
///////////////////////////////////////////////////////////////////
// Version 1.0  12/12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////
#ifndef TRKSPACEPOINT_PIXELSPACEPOINT_H
#define TRKSPACEPOINT_PIXELSPACEPOINT_H


#include "Identifier/IdentifierHash.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSpacePoint/SpacePoint.h"

namespace Trk
{
 class PrepRawData;
}


namespace InDet
{

  /**
   * @class PixelSpacePoint
   * A PixelSpacePoint is created from a PixelCluster.
   */

  class PixelSpacePoint : public Trk::SpacePoint {
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    
    /** Default constructor */
    PixelSpacePoint() ;

    /** Parametrised constructor */
    PixelSpacePoint( IdentifierHash elementId, 
                     const Trk::PrepRawData* clus);
    
    /** add Covariance Matrix and global position directly */
    PixelSpacePoint( IdentifierHash elementId,
                     const Trk::PrepRawData* clus,
                     const Amg::Vector3D& globpos,
                     const Amg::MatrixX& globcov);
    
    /** Copy constructor */
    PixelSpacePoint( const PixelSpacePoint & PSP);

    /** Destructor */
    ~PixelSpacePoint() = default;

    /** Overloading assignment operator */
    PixelSpacePoint &operator=(const PixelSpacePoint &);

    /** Clones */
    virtual SpacePoint* clone() const;       

    /** Interface method for output, to be overloaded by child classes */
    virtual MsgStream&    dump( MsgStream& out ) const;  

    /** Interface method for output, to be overloaded by child classes */
    virtual std::ostream& dump( std::ostream& out ) const;
      
};
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  
  inline Trk::SpacePoint* PixelSpacePoint::clone() const
    {  return new PixelSpacePoint(*this);  }
  
} // end of namespace Trk


#endif // TRKSPACEPOINT_PIXELSPACEPOINT_H
