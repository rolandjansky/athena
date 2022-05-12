/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMLOADVARIABLES_H
#define MMLOADVARIABLES_H

#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AtlasHepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonDigitContainer/MmDigit.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrigT1NSWSimTools/MMT_struct.h" //for digitWrapper, hitData_key, hitData_entry, evInf_entry
#include "Math/Vector4D.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

namespace MuonGM {
  class MuonDetectorManager;
}

struct histogramDigitVariables{
    std::vector<std::string> NSWMM_dig_stationName{};
    std::vector<int> NSWMM_dig_stationEta{};
    std::vector<int> NSWMM_dig_stationPhi{};
    std::vector<int> NSWMM_dig_multiplet{};
    std::vector<int> NSWMM_dig_gas_gap{};
    std::vector<int> NSWMM_dig_channel{};

    std::vector< std::vector<float> >  NSWMM_dig_time{};
    std::vector< std::vector<float> >  NSWMM_dig_charge{};
    std::vector< std::vector<int> >    NSWMM_dig_stripPosition{};
    std::vector< std::vector<double> > NSWMM_dig_stripLposX{};
    std::vector< std::vector<double> > NSWMM_dig_stripLposY{};
    std::vector< std::vector<double> > NSWMM_dig_stripGposX{};
    std::vector< std::vector<double> > NSWMM_dig_stripGposY{};
    std::vector< std::vector<double> > NSWMM_dig_stripGposZ{};
};


 class MMLoadVariables : public AthMessaging {

  public:
    MMLoadVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MmIdHelper* idhelper);

    StatusCode getMMDigitsInfo(const McEventCollection *truthContainer,
                               const TrackRecordCollection* trackRecordCollection,
                               const MmDigitContainer *nsw_MmDigitContainer,
                               std::map<std::pair<int,unsigned int>,std::vector<digitWrapper> >& entries,
                               std::map<std::pair<int,unsigned int>,std::map<hitData_key,hitData_entry> >& Hits_Data_Set_Time,
                               std::map<std::pair<int,unsigned int>,evInf_entry>& Event_Info,
                               std::map<std::string,std::shared_ptr<MMT_Parameters> > &pars,
                               histogramDigitVariables &histDigVars) const;
    //Import_Athena..._.m stuff
    double phi_shift(double athena_phi,const std::string& wedgeType, int stationPhi) const;
    int Get_VMM_chip(int strip) const;  //*** Not Finished... Rough
    int strip_number(int station, int plane, int spos, std::shared_ptr<MMT_Parameters> par)const;
    int Get_Strip_ID(double X,double Y,int plane) const;
    void xxuv_to_uvxx(ROOT::Math::XYZVector& hit, int plane, std::shared_ptr<MMT_Parameters> par)const;
    void hit_rot_stereo_fwd(ROOT::Math::XYZVector& hit, std::shared_ptr<MMT_Parameters> par)const;//x to v, u to x
    void hit_rot_stereo_bck(ROOT::Math::XYZVector& hit, std::shared_ptr<MMT_Parameters> par)const;//x to u, v to x

  private:
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const MmIdHelper* m_MmIdHelper;        //!< MM offline Id helper
    const StoreGateSvc* m_evtStore {nullptr};
  };
#endif
