/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD8F7603CA.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD8F7603CA.cm

//## begin module%3CDD8F7603CA.cp preserve=no
//## end module%3CDD8F7603CA.cp

//## Module: GeoVFullPhysVol%3CDD8F7603CA; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVFullPhysVol.cxx

//## begin module%3CDD8F7603CA.additionalIncludes preserve=no
//## end module%3CDD8F7603CA.additionalIncludes

//## begin module%3CDD8F7603CA.includes preserve=yes
//## end module%3CDD8F7603CA.includes

// GeoVFullPhysVol
#include "GeoModelKernel/GeoVFullPhysVol.h"
//## begin module%3CDD8F7603CA.additionalDeclarations preserve=yes
#include <string>
//## end module%3CDD8F7603CA.additionalDeclarations


// Class GeoVFullPhysVol 

GeoVFullPhysVol::GeoVFullPhysVol (const GeoLogVol* LogVol)
  //## begin GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.hasinit preserve=no
  //## end GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.hasinit
  //## begin GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.initialization preserve=yes
:GeoVPhysVol (LogVol), m_id(NULL), m_absPosInfo (NULL)
  //## end GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.initialization
{
  //## begin GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.body preserve=yes
  //## end GeoVFullPhysVol::GeoVFullPhysVol%3CDDA7F301DE.body
}


GeoVFullPhysVol::~GeoVFullPhysVol()
{
  //## begin GeoVFullPhysVol::~GeoVFullPhysVol%3CDD8F7603CA_dest.body preserve=yes
  delete m_absPosInfo;
  delete m_id;
  //## end GeoVFullPhysVol::~GeoVFullPhysVol%3CDD8F7603CA_dest.body
}



//## Other Operations (implementation)
const HepGeom::Transform3D & GeoVFullPhysVol::getAbsoluteTransform () const
{
  //## begin GeoVFullPhysVol::getAbsoluteTransform%3CDE70FB0196.body preserve=yes
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage = std::string ("Full Physical Volume errroneously placed in ") +	//     
    std::string ("a shared portion of a detector description graph.\n") +	//     
    std::string ("Name of shared volume:  ");	//     
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     
  if (!m_absPosInfo)
    m_absPosInfo = new GeoAbsPositionInfo ();

  if (!m_absPosInfo->getAbsTransform ())
    {


      if (isShared ())
	throw std::runtime_error (errorMessage);

      //     
      // Check the cache.  If it is empty, compute the absolute position from the     
      // top of the tree down to here, and update the cache     
      //     
      PVConstLink child = this, parent = NULL;
      if (child->isShared ())
	{
	  throw std::runtime_error (errorMessage);
	}
      else
	{
	  parent = child->getParent ();
	}

      HepGeom::Transform3D tProd;

      while (parent)
	{
#ifdef HOSED

	  int index = parent->indexOf (child);
	  HepGeom::Transform3D transform = parent->getXToChildVol (index);

	  tProd = transform * tProd;
	  child = parent;
	  if (child->isShared ())
	    {
	      throw std::runtime_error (errorMessage);
	    }
	  else
	    {
	      parent = child->getParent ();
	    }
#else
	  HepGeom::Transform3D transform = child->getX();
	  tProd = transform * tProd;
	  child = parent;
	  if (child->isShared ())
	    {
	      throw std::runtime_error (errorMessage);
	    }
	  else
	    {
	      parent = child->getParent ();
	    }
#endif
	}
      m_absPosInfo->setAbsTransform (tProd);

    }
  return *m_absPosInfo->getAbsTransform ();

  //## end GeoVFullPhysVol::getAbsoluteTransform%3CDE70FB0196.body
}

void GeoVFullPhysVol::clearPositionInfo () const
{
  //## begin GeoVFullPhysVol::clearPositionInfo%3CDE9AB9033A.body preserve=yes

  delete m_absPosInfo;
  m_absPosInfo = NULL;
  //## end GeoVFullPhysVol::clearPositionInfo%3CDE9AB9033A.body
}

const HepGeom::Transform3D & GeoVFullPhysVol::getDefAbsoluteTransform () const
{
  //## begin GeoVFullPhysVol::getDefAbsoluteTransform%3CDE9CDF0359.body preserve=yes
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage = std::string ("Full Physical Volume errroneously placed in ") +  //     
    std::string ("a shared portion of a detector description graph.\n") +	                    //     
    std::string ("Name of shared volume:  ");	                                                    //     
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     
  if (!m_absPosInfo)
    m_absPosInfo = new GeoAbsPositionInfo ();
  if (!m_absPosInfo->getDefAbsTransform ())
    {


      if (isShared ())
	throw std::runtime_error (errorMessage);

      //     
      // Check the cache.  If it is empty, compute the absolute position from the     
      // top of the tree down to here, and update the cache     
      //     
      PVConstLink child = this, parent = NULL;
      if (child->isShared ())
	{
	  throw std::runtime_error (errorMessage);
	}
      else
	{
	  parent = child->getParent ();
	}

      HepGeom::Transform3D tProd;
      while (parent)
	{
#ifdef HOSED

	  int index = parent->indexOf (child);
	  HepGeom::Transform3D transform = parent->getDefXToChildVol (index);

	  tProd = transform * tProd;
	  child = parent;
	  if (child->isShared ())
	    {
	      throw std::runtime_error (errorMessage);
	    }
	  else
	    {
	      parent = child->getParent ();
	    }
#else
	  HepGeom::Transform3D transform = child->getDefX();
	  tProd = transform * tProd;
	  child = parent;
	  if (child->isShared ())
	    {
	      throw std::runtime_error (errorMessage);
	    }
	  else
	    {
	      parent = child->getParent ();
	    }
#endif
	}
      m_absPosInfo->setDefAbsTransform (tProd);
    }
    return *m_absPosInfo->getDefAbsTransform ();
  //## end GeoVFullPhysVol::getDefAbsoluteTransform%3CDE9CDF0359.body
}

const std::string &  GeoVFullPhysVol::getAbsoluteName ()
{
  //## begin GeoVFullPhysVol::getAbsoluteName%3CEB216903A2.body preserve=yes
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage = std::string ("Full Physical Volume errroneously placed in ") +	//     
    std::string ("a shared portion of a detector description graph.\n") +	//     
    std::string ("Name of shared volume:  ");	//     
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     

  if (m_absName == "")
    {


      if (isShared ())
	throw std::runtime_error (errorMessage);

      //     
      // Check the cache.  If it is empty, compute the absolute position from the     
      // top of the tree down to here, and update the cache     
      //     
      PVConstLink child = this, parent = NULL;
      if (child->isShared ())
	{
	  throw std::runtime_error (errorMessage);
	}
      else
	{
	  parent = child->getParent ();
	}

      std::string tProd;

      while (parent)
	{

	  int index = parent->indexOf (child);
	  std::string name = parent->getNameOfChildVol (index);

	  tProd = tProd + "/" + name;
	  child = parent;
	  if (child->isShared ())
	    {
	      throw std::runtime_error (errorMessage);
	    }
	  else
	    {
	      parent = child->getParent ();
	    }
	}
      m_absName = tProd;

    }

  return m_absName;
  //## end GeoVFullPhysVol::getAbsoluteName%3CEB216903A2.body
}

unsigned int GeoVFullPhysVol::getId () const
{
  //## begin GeoVFullPhysVol::getId%3CEB24B3033F.body preserve=yes
  //------------------------------------------------------------------------------------------------//     
  //                                                                                                //     
  // Get ready for something to go wrong:                                                           //     
  //                                                                                                //     
  static std::string errorMessage = std::string ("Full Physical Volume errroneously placed in ") +	//     
    std::string ("a shared portion of a detector description graph.\n") +	//     
    std::string ("Name of shared volume:  ");	//     
  //                                                                                                //     
  //------------------------------------------------------------------------------------------------//     

  if (m_id==NULL) {
     if (isShared ()) throw std::runtime_error (errorMessage);

     //     
     // Check the cache.  If it is empty, compute the absolute position from the     
     // top of the tree down to here, and update the cache     
     //     
     PVConstLink child = this, parent = NULL;
     if (child->isShared ())	{
	   throw std::runtime_error (errorMessage);
	 }	
     else {
	   parent = child->getParent ();
	 }

	 int index = parent->indexOf(child);
	 m_id = new Query<int>(parent->getIdOfChildVol(index));
  }
  return *m_id;
  //## end GeoVFullPhysVol::getId%3CEB24B3033F.body
}

// Additional Declarations
  //## begin GeoVFullPhysVol%3CDD8F7603CA.declarations preserve=yes
  //## end GeoVFullPhysVol%3CDD8F7603CA.declarations

//## begin module%3CDD8F7603CA.epilog preserve=yes
//## end module%3CDD8F7603CA.epilog
