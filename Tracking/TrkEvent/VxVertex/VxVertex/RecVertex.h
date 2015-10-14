/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef VXVERTEX_RECVERTEX_H
#define VXVERTEX_RECVERTEX_H

#include "VxVertex/Vertex.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

/**
 * @class Trk::RecVertex
 *                            
 *   begin   : Autumn 2003
 *   <br>
 *   changes : 11.02.04 added docu
 *   <br>
 *   changes : 9/2/06 Giacinto Piacquadio - added support for non integer ndf < #Tracks * 5 - 3
 *   <br>
 *   changes : 05/12/06 Kirill.Prokofiev@cern.ch - EDN cleanup and change to the FitQuality
 *   <br>
 *   @authors  Andreas Wildauer (CERN PH-ATC) andreas.wildauer@cern.ch, 
 *   Kirill.Prokofiev@cern.ch, Giacinto Piacquadio 
 */


class MsgStream;
class RecVertexCnv_p1;
 
namespace Trk 
{

  /** 
   *  Trk::RecVertex inherits from Trk::Vertex.
   *  In addition to the vertex position, defined in Vertex class,
   *  it contains its error matrix, which can be retrieved as covariance
   *  or  weight matrix, and well as fit quality (in for of chi2 and ndf).
   */
   
  class RecVertex : public Vertex 
  {
   public:
   
  /**
   * Default constructor, if called initializes a RecVertex
   * with all data members set to 0.
   */
    RecVertex();
  
  /**
   * Constructor from a Vertex class. ErrorMatrix will be 0,0,0.
   */ 
    RecVertex(const Vertex& pos);
     
  /**
   * Constructs a RecVertex with position, covariance, ndf, dhi2 and ntrk
   */                               
    RecVertex(const Amg::Vector3D &pos, const AmgSymMatrix(3) &cov,
                    const double ndf = 0., const double chi2 = 0.);
		    	     
  /**
   *Constructs a RecVertex with position, default covariance, ndf, dhi2 and ntrk
   */  
    RecVertex(const Amg::Vector3D &pos, const double ndf = 0., const double chi2 =0.);
  
  /**
   * Copy constructor and assignement operator
   */  
    RecVertex(const RecVertex& ); 
    RecVertex &operator= (const RecVertex &);  

  /** 
   * Output Method for MsgStream, to be overloaded by child classes 
   */
    virtual MsgStream& dump(MsgStream& sl) const;
    
  /** 
   * Output Method for std::ostream, to be overloaded by child classes 
   */
    virtual std::ostream& dump(std::ostream& sl) const;

  /**
   * Destructor 
   */
    virtual ~RecVertex(); 

  /**
   * Weight and covariance matrix access method
   */
    const AmgSymMatrix(3)& covariancePosition() const; //!< return the covDeltaV matrix of the vertex fit

  /**
   * Fit quality access method
   */
    const Trk::FitQuality& fitQuality() const;

  /**
   * Error scaling method
   */      
  void scaleError(float factor); 

  private:
    friend class ::RecVertexCnv_p1;
    AmgSymMatrix(3) m_positionCovariance; //!< covariance matrix on vertex position
    Trk::FitQuality m_fitQuality; //chi2 and number of degrees of freedom of the fit
    
  };
} // end of namespace
#endif

