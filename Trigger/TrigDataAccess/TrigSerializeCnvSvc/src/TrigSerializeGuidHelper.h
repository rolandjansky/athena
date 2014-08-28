/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class TrigSerializeGuidHelper
 * collect GUID related operations and provide caching
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 */


#ifndef TRIGSERGUIDHELPER_H
#define TRIGSERGUIDHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSerializeCnvSvc/ITrigSerGuidHelper.h"

class TrigSerializeGuidHelper : virtual public ITrigSerializeGuidHelper,
				public AthAlgTool {

public:
  TrigSerializeGuidHelper(const std::string& name, const std::string& type,
			  const IInterface* parent);
  virtual ~TrigSerializeGuidHelper();

  StatusCode ClassNameToInts(const std::string &clname, uint32_t *iarr);
  StatusCode IntsToClassName(const uint32_t *iarr, std::string &clname);

private:
  //  std::map<std::string,>

};

#endif
