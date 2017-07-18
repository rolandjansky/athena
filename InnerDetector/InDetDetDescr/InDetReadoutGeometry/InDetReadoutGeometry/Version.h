/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////
// Version.h
////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_VERSION 
#define INDETREADOUTGEOMETRY_VERSION 

#include <string>

namespace InDetDD {

  /** @class Version

    Class to hold version information consisting of tag, name layout and description as strings,
    such as their integer regpresentation in the major-minor-tag scheme 
    */

  class Version {

    public:

      /** Constructor.  */
      Version(const std::string & tag, 
	  const std::string & name, 
	  const std::string & layout, 
	  const std::string & description, 
	  int major,
	  int minor,
	  int patch);

     /** Constructor. DEPRECATED */
      Version(const std::string & name, 
	  const std::string & layout, 
	  const std::string & description, 
	  int major,
	  int minor,
	  int patch);

      /** Empty Constructor  */
      Version();

      /** Version tag */
      const std::string & tag() const;

      /** Version label */
      const std::string & name() const;
  
      /** Layout (eg Initial, Final, TestBeam) */
      const std::string & layout() const;

      /** Description or comment. */
      const std::string & description() const;

      /** Major version number */
      int majorNum() const;
  
      /** Minor version number */
      int minorNum() const;

      /** Patch version number  */
      int patchNum() const;

      /** Print out version number (eg. 2.00.00) */
      std::string versionNumber() const;
  
      /** Full Description 
          For example,
          Version: SCT-DC1-00, Name: DC1, Layout: Final, Code Version: 2.00.00, Description: DC1 Geometry */
      std::string fullDescription() const;

  
   private:
 
      std::string m_tag;
      std::string m_name;
      std::string m_layout;
      std::string m_description;
      int m_major;
      int m_minor;
      int m_patch;
  };

} //  namespace  InDetDD

#endif // INDETREADOUTGEOMETRY_VERSION 
