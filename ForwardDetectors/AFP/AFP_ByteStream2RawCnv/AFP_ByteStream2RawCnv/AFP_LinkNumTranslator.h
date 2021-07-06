/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_BYTESTREAM2RAWCNV_AFP_LINKNUMTRANSLATOR_H
#define AFP_BYTESTREAM2RAWCNV_AFP_LINKNUMTRANSLATOR_H

// Package includes
#include "AFP_ByteStream2RawCnv/IAFP_LinkNumTranslator.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// STL includes
#include <string>

/**
 * @class AFP_LinkNumTranslator
 * @brief
 **/
class AFP_LinkNumTranslator : public extends<AthAlgTool, IAFP_LinkNumTranslator> {
public:
  AFP_LinkNumTranslator(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~AFP_LinkNumTranslator() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // same enumeration as for Run2
  enum LinkNums : unsigned int { kNS0=0, kNS1=1, kNS2=2, kNS3=3, kFS0=8, kFS1=9, kFS2=10, kFS3=11, kFS5=12, kFS4=13 };  

  unsigned int translate(unsigned int origlink) const override;

private:
  Gaudi::Property<bool> m_useDB {this, "UseDB", false, "whether to use COOL DB (true) or hard-coded values (false) to obtain link numbers"};

  // Using DB is not implemented yet
  // TODO: change based on reality
  // SG::ReadCondHandleKey<CondAttrListCollection> m_readDBKey_AF { this, "ReadDbKey_AF", "/AFP/Config/SideA/Far/Planes", "DB folder from which to read link numbers, side A, Far" };
  // SG::ReadCondHandleKey<CondAttrListCollection> m_readDBKey_AN { this, "ReadDbKey_AN", "/AFP/Config/SideA/Near/Planes", "DB folder from which to read link numbers, side A, Near" };
  // SG::ReadCondHandleKey<CondAttrListCollection> m_readDBKey_CF { this, "ReadDbKey_CF", "/AFP/Config/SideC/Far/Planes", "DB folder from which to read link numbers, side C, Far" };
  // SG::ReadCondHandleKey<CondAttrListCollection> m_readDBKey_CN { this, "ReadDbKey_CN", "/AFP/Config/SideC/Near/Planes", "DB folder from which to read link numbers, side C, Near" };
};

#endif // AFP_BYTESTREAM2RAWCNV_AFP_LINKNUMTRANSLATOR_H
