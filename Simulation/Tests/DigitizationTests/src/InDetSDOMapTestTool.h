#ifndef G4AT_INDETSDOMAPTESTTOOL
#define G4AT_INDETSDOMAPTESTTOOL

#include "DigiTestToolBase.h"


class InDetSDOMapTestTool final : public DigiTestToolBase {

public:

  InDetSDOMapTestTool(const std::string& name,
         const std::string& type,
         const IInterface* parent);

  virtual StatusCode initialize() override;

  virtual StatusCode processEvent() override;

  virtual StatusCode finalize() override;

 private:
  StatusCode CheckSDOs();

  unsigned int m_numberOfEventsSelected;

  std::string m_SDO_MapName;
  std::string m_addPath;

  bool m_StopTreatment;

  TH1 *m_hHMPLcollection_all;
  TH1 *m_hHMPLcollection_valid;
  TH1 *m_hHMPLcollection_ratio;
  TH1 *m_hHMPLbarcodes_all;
  TH1 *m_hHMPLbarcodes_valid;
  TH1 *m_hHMPLbarcodes_ratio;


};

#endif

