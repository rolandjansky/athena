/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGS_CSCGEOMETRYSVC_H
#define MUONCABLINGS_CSCGEOMETRYSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdHelper.h"

namespace MuonGM
{
    class MuonDetectorManager;
}
class CSCcablingSvc;
class CscIdHelper;
class MsgStream;

static const InterfaceID IID_ICSCgeometrySvc("CSCgeometrySvc", 1, 0);

class CSCgeometrySvc : public Service, public virtual IInterface
{

 public:

  // Structure used for returning ROB IDs and hash IDs
  struct CSCRobHash {
    uint32_t robID;
    IdentifierHash hashID;
  };

  // Structure used for passing strip information to MuFast.
  struct CSCStripInfo  {
    float eta;
    float phi;
    float r;
    float z;
    float width;
    int measPhi;
  };

  // Definition used for passing ROB & hash IDs.
  typedef std::vector <CSCRobHash> CSCIDInfo;

  // read/write flags for rwLUTFile.
  enum {LUT_WRITE, LUT_READ};

public:

  static const InterfaceID& interfaceID() { return IID_ICSCgeometrySvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  CSCgeometrySvc(const std::string& name, ISvcLocator* sl);
  StatusCode initialize();
  StatusCode finalize();

  /** Initialize CSC Chamber information from MuonGeoModel and CSCCablingSvc**/
  StatusCode InitCscChamberInfo();

  /** Write CSC geometry LUT to a file **/
  StatusCode rwLUTFile(const int);

  /** return ROB ids for given eta, phi region **/
  void CSCIdList(float eta, float phi, CSCIDInfo *IDs) const;

  //  /** return CSC geometry info for a given strip **/
  const CSCStripInfo& CSCGetInfo( IdentifierHash id ) const;

private:
  // structure used for generating ROB id list.
  struct CSCGeoLookup {
    float phimin;
    float phimax;
    uint16_t subdet;
    uint16_t rodID;
    IdentifierHash hashID;
  };
  std::vector <CSCGeoLookup> m_CSCChamber_neg;
  std::vector <CSCGeoLookup> m_CSCChamber_pos;

  // Maximum number of strip hash ids.
  int m_strips;

  // Lookup table stuff.
  CSCStripInfo* m_CSCStripInfo;
  static CSCStripInfo m_NULL_CSCStripInfo;

  // Variables used for writing geometry LUT to file.
    StringProperty m_fileName;
    bool m_writeLUTFile;

};

#endif //MUONCABLINGS_CSCGEOMETRYSVC_H

