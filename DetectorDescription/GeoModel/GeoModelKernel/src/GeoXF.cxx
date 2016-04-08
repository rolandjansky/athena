/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoXF.h"
#include <stdexcept>
namespace GeoXF
{



  //------------------------------------------------------------------//
  //                                                                  //
  //  Implementation of Function                                      //
  //                                                                  //
  //------------------------------------------------------------------//

  Function::Function ()
  {
  }

  Function::~Function ()
  {
  }

  Product operator * (const Function & a, const Function & b)
  {
    return Product (&a, &b);
  }

  PreMult operator * (const HepGeom::Transform3D & xf, const Function & b)
  {
    return PreMult (xf, &b);
  }

  PostMult operator * (const Function & a, const HepGeom::Transform3D & xf)
  {
    return PostMult (&a, xf);
  }

  unsigned int Function::dimensionality () const
  {
    return 1;
  }

  //------------------------------------------------------------------//
  //                                                                  //
  //  Implementation of Product                                       //
  //                                                                  //
  //------------------------------------------------------------------//

  Product::Product (const Function * arg1,
		    const Function * arg2):m_arg1 (arg1->clone ()),
    m_arg2 (arg2->clone ())
  {
    if (arg1->dimensionality () != arg2->dimensionality ())
      {
	throw std::
	  runtime_error ("Warning:  dimension mismatch in GeoXF::Product");
      }
  }


  // Every function must override this:
  Product *Product::clone () const
  {
    return new Product (*this);
  }

  // Copy constructor:
  Product::Product (const Product & right):Function (),
    m_arg1 (right.m_arg1->clone ()), m_arg2 (right.m_arg2->clone ())
  {
  }


  Product::~Product ()
  {
    delete m_arg1;
    delete m_arg2;
  }

  unsigned int Product::dimensionality () const
  {
    return m_arg1->dimensionality ();
  }

  HepGeom::Transform3D Product::operator        () (double x) const
  {
    return (*m_arg1) (x) * (*m_arg2) (x);
  }

  HepGeom::Transform3D Product::operator        () (const Genfun::Argument & x) const
  {
    return (*m_arg1) (x) * (*m_arg2) (x);
  }



  //------------------------------------------------------------------//
  //                                                                  //
  //  Implementation of PreMult                                       //
  //                                                                  //
  //------------------------------------------------------------------//

  PreMult::PreMult (const HepGeom::Transform3D & arg1,
		    const Function * arg2):m_arg1 (arg1),
    m_arg2 (arg2->clone ())
  {
  }


  // Every function must override this:
  PreMult *PreMult::clone () const
  {
    return new PreMult (*this);
  }

  // Copy constructor:
  PreMult::PreMult (const PreMult & right):Function (), m_arg1 (right.m_arg1),
    m_arg2 (right.m_arg2->clone ())
  {
  }


  PreMult::~PreMult ()
  {
    delete m_arg2;
  }

  unsigned int PreMult::dimensionality () const
  {
    return m_arg2->dimensionality ();
  }

  HepGeom::Transform3D PreMult::operator        () (double x) const
  {
    return m_arg1 * (*m_arg2) (x);
  }

  HepGeom::Transform3D PreMult::operator        () (const Genfun::Argument & x) const
  {
    return m_arg1 * (*m_arg2) (x);
  }


  //------------------------------------------------------------------//
  //                                                                  //
  //  Implementation of PostMult                                      //
  //                                                                  //
  //------------------------------------------------------------------//

  PostMult::PostMult (const Function * arg1,
		      const HepGeom::Transform3D & arg2):m_arg1 (arg1->clone ()),
    m_arg2 (arg2)
  {
  }


  // Every function must override this:
  PostMult *PostMult::clone () const
  {
    return new PostMult (*this);
  }

  // Copy constructor:
  PostMult::PostMult (const PostMult & right):Function (),
    m_arg1 (right.m_arg1->clone ()), m_arg2 (right.m_arg2)
  {
  }


  PostMult::~PostMult ()
  {
    delete m_arg1;
  }

  unsigned int PostMult::dimensionality () const
  {
    return m_arg1->dimensionality ();
  }

  HepGeom::Transform3D PostMult::operator        () (double x) const
  {
    return (*m_arg1) (x) * m_arg2;
  }

  HepGeom::Transform3D PostMult::operator        () (const Genfun::Argument & x) const
  {
    return (*m_arg1) (x) * m_arg2;
  }


  Pow::Pow (const HepGeom::Transform3D & xform, Genfun::GENFUNCTION f):m_xf (xform),
    m_function (f.clone ())
  {
  }

  Pow::~Pow ()
  {
    delete m_function;
  }

  HepGeom::Transform3D Pow::operator        () (double x) const
  {
    //
    // Get the translation part and the rotation part:
    //
    CLHEP::HepRotation rotate = m_xf.getRotation ();
    CLHEP::Hep3Vector translate = m_xf.getTranslation ();
    CLHEP::HepAxisAngle aa = rotate.axisAngle ();
    //
    // Evaluate the function
    //
    double nTimes = (*m_function) (x);
    //
    // Modify:
    //
      translate *= nTimes;
      aa.setDelta (aa.delta () * nTimes);
    //
    // Now compose these and return a result:
    //
      return HepGeom::Translate3D (translate) * HepGeom::Rotate3D (aa.delta (),
						       aa.axis ());
  }

  HepGeom::Transform3D Pow::operator        () (const Genfun::Argument & argument) const
  {
    return operator        () (argument[0]);
  }

  Pow *Pow::clone () const
  {
    return new Pow (*this);
  }

  Pow::Pow (const Pow & right):Function (), m_xf (right.m_xf),
    m_function (right.m_function->clone ())
  {
  }

}
