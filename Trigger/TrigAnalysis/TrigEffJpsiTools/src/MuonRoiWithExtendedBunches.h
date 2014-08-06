/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRoiWithExtendedBunches_h
#define MuonRoiWithExtendedBunches_h
/**
 * @author  Takashi Matsushita
 * @date    $Date: 2010-06-17 12:11:12 +0200 (Thu, 17 Jun 2010) $
 * @version $Revision: 305782 $
 *
 * This algorithm reconstructs Muon_ROI with extended bunches
 * for RPC commissioning period.
 * Based on MuCTPI_RDOToRoIBResult.h
 */

/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

namespace ROIB { class RoIBResult; }
namespace LVL1 { class RecMuonRoiSvc; }


namespace MuComm {
enum {
  NUM_THRESHOLDS = 6
};

/*--------------------------------------------------------------------*
 * classes
 *--------------------------------------------------------------------*/
class MuonRoiWithExtendedBunches : public AthAlgorithm {

  public:
    MuonRoiWithExtendedBunches(const std::string& name,
                              ISvcLocator* pSvcLocator);
    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
    ROIB::RoIBResult* getExtendedRoIBResult();
    static uint32_t RDOtoRoI(uint32_t data_word);

    std::string m_muctpiInput;
    std::string m_roibInput;
    std::string m_extendedMuonRoi;

    std::vector<int> m_allowedBcArray;
    std::set<int> m_allowedBcSet;
    std::vector<std::string> m_l1PtNames;
    std::vector<double> m_l1PtValues;

    ServiceHandle<LVL1::RecMuonRoiSvc> m_recRpcRoiSvc;
    ServiceHandle<LVL1::RecMuonRoiSvc> m_recTgcRoiSvc;

}; // class MuonRoiWithExtendedBunches

} // namespace MuComm


#endif // MuonRoiWithExtendedBunches_h
