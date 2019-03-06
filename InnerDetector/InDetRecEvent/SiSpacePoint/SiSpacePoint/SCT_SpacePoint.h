/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_SpacePoint.h
//   Header file for class SCT_SpacePoint
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to handle SPs for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0   12/12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////
#ifndef TRKSPACEPOINT_SCTSPACEPOINT_H
#define TRKSPACEPOINT_SCTSPACEPOINT_H

#include "TrkSpacePoint/SpacePoint.h"
#include "Identifier/IdentifierHash.h"
#include "TrkSpacePoint/SpacePoint.h"

namespace Trk
{
 class PrepRawData;
}



namespace InDet
{
  
  class SCT_SpacePoint : public Trk::SpacePoint 
    {
      
      /////////////////////////////`//////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:
      
      /** default constructor */
      SCT_SpacePoint() ;

      /** parametrised constructors 
       In order to ensure initialisation, the global Position has to be 
      on the surface associated to the FIRST member of the PRD-pair clusList. */

      SCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
		     const Amg::Vector3D* position, 
		     const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>* clusList) ;
      
      SCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
		     const Amg::Vector3D* position,
		     const Amg::MatrixX* loccov,//assumes ownership of loccov
		     const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>* clusList) ;
      
      SCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
		     const Amg::Vector3D& position, 
		     const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>* clusList) ;
      
      SCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList, 
		     const Amg::Vector3D& position,
		     const Amg::MatrixX& loccov,//assumes ownership of loccov
		     const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>* clusList) ;
      
      /** Copy Constructor */
      SCT_SpacePoint(const SCT_SpacePoint &) ;

      /** Destructor */
      virtual ~SCT_SpacePoint() = default;

      /** Overloading Assignment Operator */
      SCT_SpacePoint &operator=(const SCT_SpacePoint &);
      
      /** Clones */
      virtual Trk::SpacePoint* clone() const ;       
      
      /**Interface method for output, to be overloaded by child classes* */
      virtual MsgStream&    dump( MsgStream& out ) const ;  
      /**Interface method for output, to be overloaded by child classes* */
      virtual std::ostream& dump( std::ostream& out ) const ;
      
    private:

      /** method to set up the local Covariance Matrix. */
      void setupLocalCovarianceSCT() ;

      /** common method used in constructors. */
      void setup(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
		 const Amg::Vector3D& position,
		 const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>* clusList);
    };
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////

  inline Trk::SpacePoint* SCT_SpacePoint::clone() const
    {  return new SCT_SpacePoint(*this) ;  }
  
} // end of namespace Trk

#endif // TRKSPACEPOINT_PIXELSPACEPOINT_H
