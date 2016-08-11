/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_MVFFITINFO_H
#define VXMULTIVERTEX_MVFFITINFO_H

/**
 *  @class Trk::MvfFitInfo.h 
 *
 *  A helper storage class providing
 *  the information  required during the 
 *  multiple vertex fit. This object is 
 * thus meant to be transient.
 *             
 *  begin December 2006
 *  @authors: Kirill Prokofiev (University of Sheffield) Kirill.Prokofiev@cern.ch
 *
 * -----------------------------------------------------
 *  changes:
 *
 *  David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 *  constraintVertex     now uses xAOD::Vertex  instead of Trk::RecVertex
 *  seedVertex           now uses Amg::Vector3D instead of Trk::Vertex
 *  linearizationVertex  now uses Amg::Vector3D instead of Trk::Vertex
 */

#include "VxVertex/Vertex.h"
#include <vector>

#include "xAODTracking/Vertex.h"
// for use of Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

 class MvfFitInfo
 {
   public:
    
/**
 * Default constructor and destructor
 */    
    MvfFitInfo();
        
/**
 * Constructor taking pointers to constraint vertex,
 * seed vertex and linearization point 
 */   
     MvfFitInfo(xAOD::Vertex* constraintVertex,
                Amg::Vector3D* seedVertex,
                Amg::Vector3D* linearizationVertex);
    
     virtual ~MvfFitInfo();
/**
 * Copy-constructor and assignement operator
 */
     MvfFitInfo(const MvfFitInfo& rhs);
     MvfFitInfo &operator= (const MvfFitInfo & rhs); 

/**
 * Const access to the constraint vertex
 */   
     const xAOD::Vertex* constraintVertex(void) const;

/**
 * Unconst access to the constraint vertex
 */      
     xAOD::Vertex* constraintVertex(void);
    
/**
 * Constraint vertex set method
 */       
     void setConstraintVertex(xAOD::Vertex*);
      
/**
 * Const access to the seed vertex
 */       
     const Amg::Vector3D* seedVertex(void) const;

/**
 * Unconst access to the seed vertex
 */     
     Amg::Vector3D* seedVertex(void);
          
/**
 * Seed vertex set method
 */       
     void setSeedVertex(Amg::Vector3D*);

/**
 * Const access to the linearization point
 */
     const Amg::Vector3D* linearizationVertex(void) const;
  
/**
 * Unconst access to the linearization point
 */         
     Amg::Vector3D* linearizationVertex(void);
     
/**
 * Linearization point set method
 */       
     void setLinearizationVertex(Amg::Vector3D*);
    
   private:
   
     xAOD::Vertex* m_constraintVertex;
     Amg::Vector3D* m_seedVertex;
     Amg::Vector3D* m_linearizationVertex;
 
 }; //end of class definitions


  inline void MvfFitInfo::setConstraintVertex(xAOD::Vertex* constraintVertex)
  {
    m_constraintVertex=constraintVertex;
  }


  inline const xAOD::Vertex* MvfFitInfo::constraintVertex(void) const
  {
    return m_constraintVertex;
  }

  inline xAOD::Vertex* MvfFitInfo::constraintVertex(void)
  {
    return m_constraintVertex;
  }

  inline const Amg::Vector3D* MvfFitInfo::seedVertex(void) const
  {
    return m_seedVertex;
  }

  inline Amg::Vector3D* MvfFitInfo::seedVertex(void)
  {
    return m_seedVertex;
  }

  inline const Amg::Vector3D* MvfFitInfo::linearizationVertex(void) const
  {
    return m_linearizationVertex;
  }

  inline Amg::Vector3D* MvfFitInfo::linearizationVertex(void)
  {
    return m_linearizationVertex;
  }



}//end of namespace definitions

#endif
