/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/Version.h"

#include <sstream>
#include <string>
#include <iomanip>

namespace InDetDD {

Version::Version(const std::string & tag, 
		 const std::string & name, 
		 const std::string & layout, 
		 const std::string & description, 
		 int major,
		 int minor,
		 int patch)
  : m_tag(tag),
    m_name(name),
    m_layout(layout),
    m_description(description),
    m_major(major),
    m_minor(minor),
    m_patch(patch)
{}

// For backward compatibility
Version::Version(const std::string & name, 
		 const std::string & layout, 
		 const std::string & description, 
		 int major,
		 int minor,
		 int patch)
  : m_tag("-"),
    m_name(name),
    m_layout(layout),
    m_description(description),
    m_major(major),
    m_minor(minor),
    m_patch(patch)
{}



Version::Version()
  : m_major(0),
    m_minor(0),
    m_patch(0)
{}

const std::string & 
Version::tag() const 
{
  return m_tag;
}

const std::string & 
Version::name() const 
{
  return m_name;
}

const std::string & 
Version::layout() const 
{
  return m_layout;
}

const std::string & 
Version::description() const 
{
  return m_description;
}

int 
Version::majorNum() const 
{
  return m_major;
}

int 
Version::minorNum() const 
{
  return m_minor;
}

int 
Version::patchNum() const 
{
  return m_patch;
}

std::string 
Version::versionNumber() const
{
  std::ostringstream ostr;
  ostr << m_major 
       << "." << std::setfill('0') << std::setw(2) << m_minor 
       << "." << std::setfill('0') << std::setw(2) << m_patch;
  return ostr.str();
}

std::string  
Version::fullDescription() const 
{

  //  Output of the form
  //  Version: SCT-DC1-00, Name: DC1, Layout: Final, Code Version: 02.01.01, Description: DC1 Geometry

  std::ostringstream ostr;
  ostr << "Version: " << m_tag << ", Name: " << m_name << ", Layout: " << m_layout 
       << ", Code Version: " << versionNumber();
  if (!m_description.empty()) { 
    ostr << ", Description: " << m_description;
  }
  return ostr.str();
}

} // namespace InDetDD
