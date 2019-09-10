/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelByteStreamErrorsTool_h
#define PixelByteStreamErrorsTool_h

#ifndef SIMULATIONBASE
#include "AthenaBaseComps/AthAlgTool.h"
#include "PixelConditionsTools/IPixelByteStreamErrorsTool.h"

//STL includes
#include <string>
#include <set>
#include <map>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

//local includes
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class PixelByteStreamErrorsTool: public AthAlgTool, public IPixelByteStreamErrorsTool {
  public:
    static InterfaceID& interfaceID();

    PixelByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelByteStreamErrorsTool(){}
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual bool isGood(const IdentifierHash & elementIdHash) const override final;

    virtual unsigned int getModuleErrors(IdentifierHash hashID) const override final;

    virtual void setModuleErrors(IdentifierHash hashID, unsigned int errorcode) const override final;
    virtual void setModuleRODErrors(IdentifierHash hashID, unsigned int errorcode) const override final;

    virtual std::map<IdentifierHash, std::map<unsigned int, unsigned int> > & getAllFeErrors() const override final;
    virtual unsigned int getFeErrorCode(IdentifierHash module, unsigned int fe_number) const override final;
    virtual void setFeErrorCode(IdentifierHash module, unsigned int fe_number, unsigned int errorcode) const override final;

    virtual void setModuleFragmentSize(IdentifierHash hashID, unsigned int length) const override final;
    virtual unsigned int getModuleFragmentSize(IdentifierHash hashID) const override final;

    virtual void resetCounts() const override final; // for the counts used by HLT 
    virtual int getNumberOfErrors(int errorType) const override final; // for HLT 

    virtual void addInvalidIdentifier() const override final;
    virtual void addPreambleError() const override final;
    virtual void addTimeOutError() const override final;
    virtual void addLVL1IDError() const override final;
    virtual void addBCIDError() const override final;
    virtual void addFlaggedError() const override final;
    virtual void addTrailerError() const override final;
    virtual void addDisabledFEError() const override final;
    virtual void addDecodingError() const override final;
    virtual void addRODError() const override final;
    virtual void addRead(IdentifierHash hashID) const override final;
    virtual unsigned int getReadEvents(IdentifierHash hashID) const override final;
    virtual bool isActive(IdentifierHash hashID) const override final;
    virtual void addBadFE(IdentifierHash hashID, int badFE) const override final;
    virtual unsigned int getBadFE(IdentifierHash hashID) const override final;

    virtual StatusCode readData();
    virtual StatusCode recordData();

    // FE-I4B trailer errors
    virtual void addLinkMaskedByPPC() const override final;
    virtual void addLimitError() const override final;
    virtual void updateServiceRecords(int code, unsigned int count) const override final;
    virtual unsigned int getServiceRecordCount(int code) const override final;

  private:
    const PixelID* m_pixelID;

    SG::ReadHandleKey<InDetBSErrContainer>  m_BSErrContReadKey;
    SG::WriteHandleKey<InDetBSErrContainer> m_BSErrContWriteKey;
    SG::WriteHandle<InDetBSErrContainer>    m_BSErrContWrite;

    mutable std::vector<unsigned int> m_module_errors;
    mutable std::vector<unsigned int> m_moduleROD_errors;
    mutable std::vector<unsigned int> m_event_read;
    mutable std::vector<unsigned int> m_FE_errors;
    mutable std::vector<bool>         m_module_isread;

    //number of errors in the event
    mutable int m_numInvalidIdentifiers;
    mutable int m_numPreambleErrors;
    mutable int m_numTimeOutErrors;
    mutable int m_numLVL1IDErrors;
    mutable int m_numBCIDErrors;
    mutable int m_numFlaggedErrors;
    mutable int m_numTrailerErrors;
    mutable int m_numDisabledFEErrors;
    mutable int m_numDecodingErrors;
    mutable int m_numRODErrors;
    mutable int m_numLinkMaskedByPPC;
    mutable int m_numLimitError;

    // Collection of all FE errors in the event
    // m_all_FE_errors[moduleID][FEnumber] = errorcode
    mutable std::map<IdentifierHash, std::map<unsigned int, unsigned int> > m_all_FE_errors;

    // ROB fragment length for each module
    mutable std::map<IdentifierHash, unsigned int> m_module_fragment_size;

    // FE-I4B service record codes
    // Array of counters for each code, i.e. m_ServiceRecords[0] is the
    // count for code 0 (BCID counter error)
    mutable unsigned int m_ServiceRecords[32];

    unsigned int m_checkError;

    bool m_readESD;
    std::string m_BSErrContainerKey;
};

inline InterfaceID& PixelByteStreamErrorsTool::interfaceID() {
  static InterfaceID IID_PixelByteStreamErrorsTool("PixelByteStreamErrorsTool", 1, 0);
  return IID_PixelByteStreamErrorsTool;
}

#endif
#endif
