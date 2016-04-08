/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// The Athena Pool Converter for TagInfo guys
///
#ifndef JETTAGINFOTPCNV_JETTAGINFOCNV
#define JETTAGINFOTPCNV_JETTAGINFOCNV

#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"
#include "JetTagInfoTPCnv/JetTagInfo_tlp1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp1.h"
#include "JetTagInfoTPCnv/JetTagInfo_tlp2.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp2.h"
#include "JetTagInfoTPCnv/JetTagInfo_tlp3.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "JetTagInfoAthenaPool/JetTagInfo.h"


typedef T_AthenaPoolExtendingCnv<JetTagInfo, Analysis::JetTagInfo_tlp3> JetTagInfoCnvBase;

#include <iostream>

class JetTagInfoCnv : public JetTagInfoCnvBase {

  friend class CnvFactory<JetTagInfoCnv>;

 protected:

  JetTagInfoCnv (ISvcLocator *svcloc);
  StatusCode initialize(void);
  inline AthenaPoolTopLevelTPCnvBase *getTopLevelTPCnv(void)
    {
      return &m_TPConverter;
    }
  inline AthenaPoolCnvTPExtension *clone (void)
    {
      return new JetTagInfoCnv(0);
    }
  void readObjectFromPool (const std::string &obj);

 private:
  Analysis::JetTagInfoCnv_tlp3 m_TPConverter;
  Analysis::JetTagInfoCnv_tlp2 m_TPConverter_p2;
  Analysis::JetTagInfoCnv_tlp1 m_TPConverter_p1;
};
#endif
