/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_LOCALPARAMETERS_H
#define TRKEVENTPRIMITIVES_LOCALPARAMETERS_H

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// TRK
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
// maths
#include <cmath>

class MsgStream;

class LocalParametersCnv_p1;

/** standard namespace for Tracking*/
namespace Trk {

  /**@class LocalParameters 
    
     Class inheriting from HepVector for 1-5 dimensional parameter vectors.
     It holds a  static const projection matrices set
     for expanding the actual dimension to 5 dimension, 
     respectively reducing a 5 dimensional object to the actual dimension of
     this object.
     
     The identification is done by a parameter key (integer) that represents a
     binary number. For technical reasons this binary number has to be read from 
     back to the front to associate it with the five parameters.
     
     For the binary number, the following identification is chosen:
            -  1 ... parameter is defined 
            -  0 ... parameter is not defined
            
     Examples:
     <table cellspacing=0 cellpadding=0 border=1>
     <tr> <td><b>loc1</b></td>
          <td><b>loc2</b></td>
          <td><b>@f$ \phi @f$</b></td>
          <td><b>@f$ \theta @f$</b></td>
          <td><b>@f$ \frac{q}{p} @f$</b></td>
          <td><b>binary number (mirrored) </b></td>
          <td><b>Parameter - Key </b></td>
          <td><b>Comments </b></td>
     </tr>
     <tr> <td>1</td>
          <td>1</td>
          <td>1</td>
          <td>1</td>
          <td>1</td>
          <td>11111</td>
          <td>31</td>
          <td>The 'old' track parameters vector</td>
     </tr>
     <tr> <td>1</td>
          <td>0</td>
          <td>0</td>
          <td>0</td>
          <td>0</td>
          <td>00001</td>
          <td>1</td>
          <td>'local position'(measurement) with only one dimension (TRT, MDT) </td>
     </tr>
     <tr> <td>1</td>
          <td>1</td>
          <td>0</td>
          <td>0</td>
          <td>0</td>
          <td>00011</td>
          <td>3</td>
          <td>'local position'(measurement) with only one dimension</td>
     </tr>
     <tr> <td>1</td>
          <td>1</td>
          <td>0</td>
          <td>1</td>
          <td>0</td>
          <td>01011</td>
          <td>11</td>
          <td>hypothetical track segment having 2 local coordinated and @f$ \theta @f$ information</td>
     </tr>
     </table>
     
    The accessors are optimized for full track parameters, 1 dim  and 2 dim local parameters.
    @author Andreas.Salzburger@cern.ch
    */
     
  class LocalParameters : public Amg::VectorX {

    public:
      /**Default constructor used for POOL */
      LocalParameters();

      //default other operations 
      LocalParameters(const LocalParameters&) = default;
      LocalParameters(LocalParameters&&) = default;
      LocalParameters& operator=(const LocalParameters&) = default;
      LocalParameters& operator=(LocalParameters&&) = default;
      ~LocalParameters() = default;

      /**Default constructor with dimension */
      LocalParameters(int dim);
      
      /**Explicit constructor for 1-dimensional vector */
      LocalParameters(const DefinedParameter& one);
      
      /**Explicit constructor for 2-dimensional vector */
      LocalParameters(const DefinedParameter& one,
                      const DefinedParameter& two);
      
      /**Explicit constructor from LocalPosition*/
      LocalParameters(const Amg::Vector2D& lp);                
                      
      /**Explicit constructor for full parameter vector - respect the order*/
      LocalParameters(double locone, double loctwo, double tphi, double ttheta, double tqOverp);
      
      /**Dynamical constructor */                
      LocalParameters(const std::vector<DefinedParameter>& );

      /**Pseudo-constructor */
      LocalParameters* clone() const;
      
      /** Reduction matrix from 5x5 to the [dimension()]x[dimension()] */
      const Amg::MatrixX& reductionMatrix() const;
      
      /** Expansion matrix from 5x5 to the [dimension()]x[dimension()] */
      const Amg::MatrixX& expansionMatrix() const;
      
      /** Dimension of this localParameters() vector */
      int dimension() const;
      
      /** Identifier key for matrix expansion/reduction */
      int parameterKey() const;
      
      /** The simple check for the clients wheter the parameter is contained */
      bool contains(ParamDefs par) const;

      /**Read from data members */
      const double & operator[](ParamDefs par) const;
      
      /**Write data members */      
      double & operator[](ParamDefs par);
      
        /**Retrieve specified parameter (const version). There is NO check to see if a parameter is contained, 
        so consider using contains(ParamDefs par) to make sure it is actually defined for this object.
        @param par Parameters requested to be return
        @return Value of stored parameter (or undefined, if the parameter is not used in this object i.e. be SURE it is!) */
      double get(ParamDefs par) const;

        /**Retrieve specified parameter. There is NO check to see if a parameter is contained, 
        so consider using contains(ParamDefs par) to make sure it is actually defined for this object.
        @param par Parameters requested to be return
        @return Value of stored parameter (or undefined, if the parameter is not used in this object i.e. be SURE it is!) */
      double get(ParamDefs par);
      
      /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
      friend MsgStream& operator << ( MsgStream& sl, const LocalParameters& lp);
      friend std::ostream& operator << ( std::ostream& sl, const LocalParameters& lp); 

  
    protected:     
     friend class ::LocalParametersCnv_p1;
     int    m_parameterkey;
     
    private: 
     static const ProjectionMatricesSet s_projectionMatrices;
  };    
       
} // end of namespace

#include "TrkEventPrimitives/LocalParameters.icc"
#endif // TRKEVENTPRIMITIVES_LOCALPARAMETERS_H
