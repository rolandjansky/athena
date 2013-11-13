/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCaloCluster_OnTrack_H
#define TRKCaloCluster_OnTrack_H
 // Trk
#include "TrkMeasurementBase/MeasurementBase.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "EventPrimitives/EventPrimitives.h"  
#include "GeoPrimitives/GeoPrimitives.h" 


#include <ostream>
class MsgStream;

namespace Trk {
  class Surface;
  class EnergyLoss;

  /** @class CaloCluster_OnTrack
    Class to handle Cluster On Tracks (ROT) for CaloClusters,
    it inherits from the common MeasurementBase. 

    The Track holds a vector of TrackStateOnSurface 
    that carry the MeasurmentBase class pointers.
  */

  class CaloCluster_OnTrack : public MeasurementBase {
    public:
      /** Default Constructor for POOL */
      CaloCluster_OnTrack();
      /** Copy Constructor */
      CaloCluster_OnTrack(const CaloCluster_OnTrack& cot);
 
      /** Assignment operator */
      CaloCluster_OnTrack& operator=(const CaloCluster_OnTrack& cot);

      /** Constructor with parameters  LocalParameters*,  LocalErrorMatrix*, Surface&
      This class owns the LocalParameters, ErrorMatrix & EnergyLoss.  A copy of the  
			surface will be made if it is not owned by a detector element.    */
      CaloCluster_OnTrack( const LocalParameters& locpars,
                           const Amg::MatrixX& locerr,
                           const Surface& surf,
                           const EnergyLoss* eloss = 0);
    
      /** Destructor */
      virtual ~CaloCluster_OnTrack();

      /** Pseudo-constructor, needed to avoid excessive RTTI*/
      virtual CaloCluster_OnTrack* clone() const;
            
      /** returns the surface for the local to global transformation 
      - interface from MeasurementBase */
      virtual const Surface& associatedSurface() const;
     
      /** Interface method to get the global Position
      - interface from MeasurementBase */
      virtual const Amg::Vector3D& globalPosition() const;

      
      /** Extended method to get the EnergyLoss */
      virtual const Trk::EnergyLoss* energyLoss() const;

      /**returns the some information about this RIO_OnTrack. */
      virtual MsgStream&    dump( MsgStream& out ) const;  

      /**returns the some information about this RIO_OnTrack. */
      virtual std::ostream& dump( std::ostream& out ) const;
 

    protected:
      /** Surface associated to the measurement*/
      mutable const Surface* m_surface;
      
      /** global position of the cluster hit*/
      mutable const Amg::Vector3D* m_globalpos;
      
      /** Energy Loss */
      mutable const Trk::EnergyLoss* m_eloss;
      

  };//End of Class

}//Namespace

inline const Trk::Surface& Trk::CaloCluster_OnTrack::associatedSurface() const
{ return *m_surface; }

inline const Trk::EnergyLoss* Trk::CaloCluster_OnTrack::energyLoss() const
{ return m_eloss; }

#endif //TRKCaloCluster_OnTrack_H
