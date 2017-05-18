/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Options.h"

SCT_Options::SCT_Options()
  : m_g3Compatible(false),
    m_dc1Geometry(false),
    m_dc2Geometry(true),
    m_alignable(true),
    m_alignModule(true),
    m_dynAlignFolders(false)
{}

void 
SCT_Options::setG3Compatible(bool flag)
{
  m_g3Compatible = flag;
  if (flag) setDC1Geometry();
}

bool 
SCT_Options::g3Compatible() const 
{
  return  m_g3Compatible;
}

void 
SCT_Options::setDC1Geometry()
{
  m_dc1Geometry = true;
  m_dc2Geometry = false;
}

bool 
SCT_Options::versionDC1() const 
{
  return m_dc1Geometry;
}

void 
SCT_Options::setDC2Geometry()
{
  m_dc1Geometry = false;
  m_dc2Geometry = true;
  m_g3Compatible = false;
}

bool 
SCT_Options::versionDC2() const 
{
  return m_dc2Geometry;
}

void 
SCT_Options::setAlignable(bool flag)
{
  m_alignable = flag;
}

bool 
SCT_Options::alignable() const
{
  return m_alignable;
}

void 
SCT_Options::setAlignAtModuleLevel(bool flag)
{
  m_alignModule = flag;
}

bool 
SCT_Options::alignAtModuleLevel() const
{
  return m_alignModule;
}

void SCT_Options::setDynamicAlignFolders(const bool flag)
{
  m_dynAlignFolders = flag;
}

bool SCT_Options::dynamicAlignFolders() const 
{  
  return m_dynAlignFolders;
}
