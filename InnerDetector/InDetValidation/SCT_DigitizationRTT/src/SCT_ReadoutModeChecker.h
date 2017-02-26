// Multiple inclusion protection
#ifndef SCT_DIGITIZATIONRTT_SCT_READOUTCHECKER_H
#define SCT_DIGITIZATIONRTT_SCT_READOUTCHECKER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h" 

class TH1F;
class TH2F;
class TProfile;

class ITHistSvc;
//class INTupleSvc;

class SCT_RDORawData;
class SCT_ID;
namespace InDetDD {
  class SCT_DetectorManager;
}
class ISiliconConditionsSvc;
class ISiPropertiesSvc;

/** This class ..... */
class SCT_ReadoutModeChecker:public AthAlgorithm {

public:

  SCT_ReadoutModeChecker (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  ServiceHandle<ITHistSvc> m_tHistSvc; //!< Histogram service handle
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;
  //ServiceHandle<INTupleSvc> m_nTupleSvc; //!< Ntuple service handle
  const InDetDD::SCT_DetectorManager* m_sctmgr; //!< Pointer to the SCT detector manager
  const SCT_ID * m_sctID;       //!<  Pointer to the SCT ID helper
  std::string m_rootRoot;   //!< Name of the ITHistSvc (default: /SCT_DigiRTT)

  /** General Histogram booking methods */
  StatusCode BookRawDataHists();
  TH1F* BookTH1F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh);
  TH2F* BookTH2F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, int nbinsy, double ylow, double yhigh);
  TProfile* BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh);
  TProfile* BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, double ymin, double ymax);
  std::string m_rootName;
  //std::string m_treeName;

  
  int m_runNumber;                    //!< Run number
  int m_eventNumber;                  //!< Event number 
  int m_numberOfEventsInput;          //!< Total number of events seen by SCT_ReadoutModeChecker so far
  int m_numberOfEventsSelected;       //!< Total number of events selected so far

  StatusCode ExamineRDOs(); 
  TH1F* m_RDO_GroupSize;
  TH1F* m_RDO_TimeBin;
  TH2F* m_RDO_TimeBinVsGroupSize;

  BooleanProperty m_RDOsWrittenOutInCompressedMode; //!< Were the RDOs written out in compressed mode? (or in expanded mode)
  StringProperty m_ExpectedReadoutModeString;
  typedef enum SCTReadoutModes
    {
      LEVEL_MODE   = 1,
      EDGE_MODE    = 2,
      ANY_HIT_MODE = 3,
      UNKNOWN_MODE = 4
    } SCTReadoutModes;
  SCTReadoutModes m_ExpectedReadoutMode;

};
#endif
