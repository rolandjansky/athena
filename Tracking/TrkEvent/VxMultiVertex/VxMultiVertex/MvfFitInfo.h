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
 */

#include "VxVertex/Vertex.h"
#include <vector>

namespace Trk
{

 class Vertex;
 class RecVertex;
 
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
     MvfFitInfo(Trk::RecVertex* constraintVertex,
                Trk::Vertex* seedVertex,
                Trk::Vertex* linearizationVertex);
    
     virtual ~MvfFitInfo();
/**
 * Copy-constructor and assignement operator
 */
     MvfFitInfo(const MvfFitInfo& rhs);
     MvfFitInfo &operator= (const MvfFitInfo & rhs); 

/**
 * Const access to the constraint vertex
 */   
     const Trk::RecVertex* constraintVertex(void) const;

/**
 * Unconst access to the constraint vertex
 */      
     Trk::RecVertex* constraintVertex(void);
    
/**
 * Constraint vertex set method
 */       
     void setConstraintVertex(Trk::RecVertex*);
      
/**
 * Const access to the seed vertex
 */       
     const Trk::Vertex* seedVertex(void) const;

/**
 * Unconst access to the seed vertex
 */     
     Trk::Vertex* seedVertex(void);
          
/**
 * Seed vertex set method
 */       
     void setSeedVertex(Trk::Vertex*);

/**
 * Const access to the linearization point
 */
     const Trk::Vertex* linearizationVertex(void) const;
  
/**
 * Unconst access to the linearization point
 */         
     Trk::Vertex* linearizationVertex(void);
     
/**
 * Linearization point set method
 */       
     void setLinearizationVertex(Trk::Vertex*);
    
   private:
   
     Trk::RecVertex* m_constraintVertex;
     Trk::Vertex* m_seedVertex;
     Trk::Vertex* m_linearizationVertex;
 
 }; //end of class definitions


  inline void MvfFitInfo::setConstraintVertex(Trk::RecVertex* constraintVertex)
  {
    m_constraintVertex=constraintVertex;
  }


  inline const Trk::RecVertex* MvfFitInfo::constraintVertex(void) const
  {
    return m_constraintVertex;
  }

  inline Trk::RecVertex* MvfFitInfo::constraintVertex(void)
  {
    return m_constraintVertex;
  }

  inline const Trk::Vertex* MvfFitInfo::seedVertex(void) const
  {
    return m_seedVertex;
  }

  inline Trk::Vertex* MvfFitInfo::seedVertex(void)
  {
    return m_seedVertex;
  }

  inline const Trk::Vertex* MvfFitInfo::linearizationVertex(void) const
  {
    return m_linearizationVertex;
  }

  inline Trk::Vertex* MvfFitInfo::linearizationVertex(void)
  {
    return m_linearizationVertex;
  }



}//end of namespace definitions

#endif
