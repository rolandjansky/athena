/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_BASETAGINFO_P1_H
#define JETTAGINFOTPCNV_BASETAGINFO_P1_H

#include <vector>
#include <string>

namespace Analysis {

  /// Forward declare the converter.
  class BaseTagInfoCnv_p1;

  /// Hold onto the base tagging info.
  class BaseTagInfo_p1 {
    friend class BaseTagInfoCnv_p1;

  public:
    inline BaseTagInfo_p1()
      : m_isValid(false),
      m_weight(0)
      {}

  private:
    bool m_isValid;                      
    std::vector<float> m_tagLikelihood;
    float m_weight; 
    std::string m_tagJetInfoType;
  };

} // End namespace
#endif
