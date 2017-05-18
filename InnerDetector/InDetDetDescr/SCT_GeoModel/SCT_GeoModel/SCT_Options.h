/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_Options_H
#define SCT_GeoModel_SCT_Options_H

// Class for any run time options.


class SCT_Options
{

public:
  SCT_Options();
  bool g3Compatible() const; 
  bool versionDC1() const;
  bool versionDC2() const;
  bool alignable() const;
  bool alignAtModuleLevel() const;

  void setG3Compatible(bool flag = true);
  void setDC1Geometry();
  void setDC2Geometry();
  void setAlignable(bool flag = true);
  void setAlignAtModuleLevel(bool flag = true);

  //dynamic alignment folders
  void setDynamicAlignFolders(const bool flag = true);
  bool dynamicAlignFolders() const;

private:

  bool m_g3Compatible;
  bool m_dc1Geometry;
  bool m_dc2Geometry;
  bool m_alignable;
  bool m_alignModule;
  bool m_dynAlignFolders;   //controls which set of alignment folders is used

};


#endif // SCT_GeoModel_SCT_Options_H
