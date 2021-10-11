/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAKEINPUTDATAHEADER_H
#define MAKEINPUTDATAHEADER_H
/** @file MakeInputDataHeader.h
 *  @brief This file contains the class definition for the MakeInputDataHeader class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/DataHeader.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include <string>

class DataHeader;

/** @class MakeInputDataHeader 
 *  @brief This class provides an algorithm to make an OutputDataHeader the InputDataHeader.
 *
 * Mark a DataHeader is input.  This could be done, for example, in order
 * to get an output DataHeader to be included as an input DataHeader for
 * another output DataHeader produced in the same job, to enable
 * back-navigation.  (Eg, a job that produces both ESD and AOD data.)
 * We used to do this by changing the status of the header in SG,
 * but we don't want to do that for MT jobs.  Instead, we mark the header
 * as input by creating an alias with the key ending with _Input.
 * This is then recognized in AthenaOutputStreamTool.
 **/
class MakeInputDataHeader : public ::AthReentrantAlgorithm {
public:
   /// Standard Algorithm Constructor
   MakeInputDataHeader(const std::string& name, ISvcLocator* pSvcLocator);
   /// AthAlgorithm Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode execute(const EventContext& ctx) const override;

private:
   /// StreamName, name of Stream to be made Input.
   SG::ReadHandleKey<DataHeader> m_streamName { this, "StreamName", "" };
   SG::WriteHandleKey<DataHeader> m_aliasName { this, "AliasName", "" };

   /// KeepCurrentInput, keep the original InputDataHeader: default = false
   BooleanProperty m_keepInput { this, "KeepCurrentInput", false, };
};

#endif
