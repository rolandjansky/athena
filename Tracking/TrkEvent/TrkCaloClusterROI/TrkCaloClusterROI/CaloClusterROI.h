/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCALOCLUSTERROI_H
#define TRKCALOCLUSTERROI_H


// Amg 
#include "EventPrimitives/EventPrimitives.h" 
#include "GeoPrimitives/GeoPrimitives.h" 
//#include "TrkEventPrimitives/GlobalPosition.h"

namespace Trk {
  class Surface;
  class LocalParameters;

  /** @class CaloClusterROI
    Class to handle ROI for CaloClusters,
    
  */

  class CaloClusterROI{
    public:
      /** Default Constructor for POOL */
      CaloClusterROI();
      /** Copy Constructor */
      CaloClusterROI(const CaloClusterROI& ccRIO);
 
      /** Assignment operator */
      CaloClusterROI& operator=(const CaloClusterROI& ccRIO);

      /** Constructor with parameters  LocalParameters*,  Surface&
      This class owns the LocalParameters,  A copy of the  
			surface will be made if it is not owned by a detector element.    */
      CaloClusterROI( const LocalParameters* locpars,
                      const Surface& surf,
                      const double  energy= 0,
                      const double  widthPhi = 0.1,
                      const double  widthEta = 0.1);
    
      /** Destructor */
      virtual ~CaloClusterROI();

      /** Pseudo-constructor, needed to avoid excessive RTTI*/
      virtual CaloClusterROI* clone() const;
            
      /** returns the LocalParameters */
      virtual const LocalParameters& localParameters() const;
        
      /** returns the surface for the local to global transformation */
      virtual const Surface& associatedSurface() const;
     
      /** method to get the global Position */
      virtual const Amg::Vector3D& globalPosition() const;
 
 
      virtual double energy() const;
      
      virtual double phiWidth() const;
      
      virtual double etaWidth() const;
      

    protected:
      /** local position of the Cluster, in the coordinate frame of the Trk::Surface it exists on*/
      mutable const LocalParameters* m_localParams;
      
      /** Surface associated to the measurement*/
      mutable const Surface* m_surface;
      
      /** global position of the cluster hit*/
      mutable const Amg::Vector3D* m_globalpos;
      
      /** Energy*/
      mutable double m_energy;
      /** Width phi*/
      mutable double m_phiWidth;
      /** Width eta*/
      mutable double m_etaWidth;
      

  };//End of Class

}//Namespace



inline const Trk::LocalParameters& Trk::CaloClusterROI::localParameters() const
{ return *m_localParams; }

inline const Trk::Surface& Trk::CaloClusterROI::associatedSurface() const
{ return *m_surface; }

inline double Trk::CaloClusterROI::energy() const
{ return m_energy; }

inline double Trk::CaloClusterROI::phiWidth() const
{ return m_phiWidth; }

inline double Trk::CaloClusterROI::etaWidth() const
{ return m_etaWidth; }



#endif //TRKCaloClusterROI_H
