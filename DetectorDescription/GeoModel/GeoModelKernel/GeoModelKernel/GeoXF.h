/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//------------------------GeoTransformFunction--------------------------//
//                                                                      //
//  GeoTransformFunction, base class for function objects which         //
//  evaluate to a HepTransform.                                         //
//                                                                      //
//----------------------------------------------------------------------//
#ifndef GEOMODELKERNEL_GEOTRANSFORMFUNCTION_H
#define GEOMODELKERNEL_GEOTRANSFORMFUNCTION_H
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Argument.hh"
#include "CLHEP/Geometry/Transform3D.h"
//-----------------------------------------------------------------------//
// Exact return type of arithmentic operations.  To the user, the return //
// type is TRANsFUNCTION, or const GeoXF::Function &.                    //
//-----------------------------------------------------------------------//

namespace GeoXF
{

  class Product;
  class PreMult;
  class PostMult;
  class Pow;


  class Function
  {

  public:

    // Default Constructor
    Function ();

    // Destructor
    virtual ~ Function ();

    // Function value:  N-dimensional functions must override these:
    virtual unsigned int dimensionality () const;	//returns 1;

    // Function value
    virtual HepGeom::Transform3D operator         () (double argument) const = 0;
    virtual HepGeom::Transform3D operator         () (const Genfun::
						Argument & argument) const =
      0;

    // Every function must override this:
    virtual Function *clone () const = 0;

    // Copy constructor
      Function (const Function & right);

  private:

    // Assignment operator
    const Function & operator = (const Function & right);

  };






  class Pow:public Function
  {

  public:

    Pow (const HepGeom::Transform3D &, Genfun::GENFUNCTION f);

      virtual ~ Pow ();

    virtual HepGeom::Transform3D operator         () (double argument) const;
    virtual HepGeom::Transform3D operator         () (const Genfun::
						Argument & argument) const;

    // Every function must override this:
    Pow *clone () const;

    // Copy constructor:
      Pow (const Pow & right);

  private:

    // Assignment operator
    const Pow & operator = (const Pow & right);

    const HepGeom::Transform3D m_xf;
    const Genfun::AbsFunction * m_function;

  };





  Product operator * (const Function & op1, const Function & op2);
  PreMult operator * (const HepGeom::Transform3D & xf, const Function & op2);
  PostMult operator * (const Function & op2, const HepGeom::Transform3D & xf);


  // Internally used class:: Product:

  class Product:public Function
  {

  public:


    Product (const Function * arg1, const Function * arg2);

      virtual ~ Product ();

    virtual unsigned int dimensionality () const;

    virtual HepGeom::Transform3D operator         () (double argument) const;
    virtual HepGeom::Transform3D operator         () (const Genfun::
						Argument & argument) const;

    // Every function must override this:
    virtual Product *clone () const;

    // Copy constructor:
      Product (const Product & right);

    Product& operator= (const Product & right) = delete;

  private:

    const Function *m_arg1;
    const Function *m_arg2;

  };

  // Internally used class:: PreMult :

  class PreMult:public Function
  {

  public:


    PreMult (const HepGeom::Transform3D & arg1, const Function * arg2);

      virtual ~ PreMult ();

    virtual unsigned int dimensionality () const;

    virtual HepGeom::Transform3D operator         () (double argument) const;
    virtual HepGeom::Transform3D operator         () (const Genfun::
						Argument & argument) const;

    // Every function must override this:
    virtual PreMult *clone () const;

    // Copy constructor:
    PreMult (const PreMult & right);

    PreMult& operator= (const PreMult & right) = delete;

  private:

    const HepGeom::Transform3D m_arg1;
    const Function *m_arg2;

  };

  // Internally used class:: PostMult :

  class PostMult:public Function
  {

  public:


    PostMult (const Function * arg1, const HepGeom::Transform3D & arg2);

      virtual ~ PostMult ();

    virtual unsigned int dimensionality () const;

    virtual HepGeom::Transform3D operator         () (double argument) const;
    virtual HepGeom::Transform3D operator         () (const Genfun::
						Argument & argument) const;

    // Every function must override this:
    virtual PostMult *clone () const;

    // Copy constructor:
      PostMult (const PostMult & right);

    PostMult& operator= (const PostMult & right) = delete;

  private:

    const Function *m_arg1;
    const HepGeom::Transform3D m_arg2;

  };

  typedef const Function & TRANSFUNCTION;



}

#endif
