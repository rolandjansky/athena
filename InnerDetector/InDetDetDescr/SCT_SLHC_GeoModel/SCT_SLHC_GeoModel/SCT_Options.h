/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_Options_H
#define SCT_SLHC_GeoModel_SCT_Options_H

namespace InDetDDSLHC {

// Class for any run time options.
class SCT_Options
{

public:
  SCT_Options();
  bool alignable() const;
  bool alignAtModuleLevel() const;

  void setAlignable(bool flag = true);
  void setAlignAtModuleLevel(bool flag = true);
  

private:

  bool m_alignable;
  bool m_alignModule;
};


// Inlined methods
inline SCT_Options::SCT_Options()
  : m_alignable(true),
    m_alignModule(true)
{}


inline void SCT_Options::setAlignable(bool flag)
{
  m_alignable = flag;
}

inline bool SCT_Options::alignable() const
{
  return m_alignable;
}

inline void SCT_Options::setAlignAtModuleLevel(bool flag)
{
  m_alignModule = flag;
}

inline bool SCT_Options::alignAtModuleLevel() const
{
  return m_alignModule;
}
}
#endif // SCT_SLHC_GeoModel_SCT_Options_H
