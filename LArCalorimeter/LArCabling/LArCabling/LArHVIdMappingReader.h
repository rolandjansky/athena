// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArCabling/LArHVIdMappingReader.h
 * @author scott snyder <snyder@bnl.gov>, from code originally in LArHVCablingTool.
 * @date Jun, 2021
 * @brief Helper to read HV ID mapping data from a file or conditions.
 */


#ifndef LARCABLING_LARHVIDMAPPINGREADER_H
#define LARCABLING_LARHVIDMAPPINGREADER_H


#include "AthenaBaseComps/AthMessaging.h"
#include "Identifier/HWIdentifier.h"
#include "GaudiKernel/StatusCode.h"
#include <map>
#include <vector>
#include <string>
class AthenaAttributeList;
class LArHVLineID;
class LArElectrodeID;


/**
 * @brief Helper to read HV ID mapping data from a file or conditions.
 */
class LArHVIdMappingReader
  : public AthMessaging
{
public:
  using hvlineMap_t = std::map<HWIdentifier,HWIdentifier>;
  using electrodeMap_t = std::map<HWIdentifier,std::vector<HWIdentifier> >;

  LArHVIdMappingReader();

  StatusCode read (const AthenaAttributeList* attrList,
                   const LArHVLineID& hvlineHelper,
                   const LArElectrodeID& electrodeHelper,
                   hvlineMap_t& hvlineMap,
                   electrodeMap_t& electrodeMap) const;


private:
  std::string fromAttrList (const AthenaAttributeList& attrList) const;
  std::string fromFile() const;

  StatusCode fillHV (const std::string& content,
                     const LArHVLineID& hvlineHelper,
                     const LArElectrodeID& electrodeHelper,
                     hvlineMap_t& hvlineMap,
                     electrodeMap_t& electrodeMap) const;
};


#endif // not LARCABLING_LARHVIDMAPPINGREADER_H
