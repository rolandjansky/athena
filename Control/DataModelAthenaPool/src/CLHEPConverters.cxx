/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/src/CLHEPConverters.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from previous code.
 * @brief Define converters for CLHEP types needed to read old data files.
 */


#include "DataModelAthenaPool/CLHEPConverters.h"
#include "RootConversions/TConverterRegistry.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "TMemberStreamer.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TROOT.h"
#include "Rtypes.h"
#include <cassert>


namespace DataModelAthenaPool {


/**
 * @brief Converter for @c Hep3Vector to @c Point3D<double>.
 */
class PointConverter
  : public TMemberStreamer
{
public:
  /// Constructor.
  PointConverter ();


  /**
   * @brief Run the streamer.
   * @param b Buffer from which to read.
   * @param pmember Pointer to the object into which to read.
   * @param size Number of instances to read.
   */
  virtual void operator() (TBuffer& b, void* pmember, Int_t size=0);


private:
  /// Hold the class for @c Hep3Vector.
  TClass* m_cl;
};


/**
 * @brief Constructor.
 */
PointConverter::PointConverter()
{
  m_cl = gROOT->GetClass ("Hep3Vector");
  m_cl = gROOT->GetClass ("CLHEP::Hep3Vector");
}


/**
 * @brief Run the streamer.
 * @param b Buffer from which to read.
 * @param pmember Pointer to the object into which to read.
 * @param size Number of instances to read.
 */
void PointConverter::operator() (TBuffer& b,
                                 void* pmember, 
                                 Int_t size /*=0*/)
{
  // This only works for reading!
  assert (b.IsReading());

  // The transient object.
  HepGeom::Point3D<double>* obj =
    reinterpret_cast<HepGeom::Point3D<double>*> (pmember);

  // We'll read into this object.
  CLHEP::Hep3Vector tmp;

  if (size == 0) size = 1;
  while (size-- > 0) {
    // Read into tmp and copy data to *obj.
    m_cl->Streamer (&tmp, b);
    obj->setX (tmp.x());
    obj->setY (tmp.y());
    obj->setZ (tmp.z());
    ++obj;
  }
}


/**
 * @brief Register all CLHEP converters.
 */
void CLHEPConverters::initialize()
{
  TConverterRegistry::Instance()->AddStreamerConverter
    ("CLHEP::Hep3Vector", "HepGeomPoint3D<double>", new PointConverter);
}


} // namespace DataModelAthenaPool
