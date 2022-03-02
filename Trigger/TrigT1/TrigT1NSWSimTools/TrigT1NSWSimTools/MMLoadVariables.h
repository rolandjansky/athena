/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMLOADVARIABLES_H
#define MMLOADVARIABLES_H

#include "MMT_struct.h" //for digitWrapper, hitData_key, hitData_entry, evInf_entry
#include "AthenaBaseComps/AthMessaging.h"

#include <map>
#include <vector>
#include <string>

class MsgStream;
class MmIdHelper;
class MmDigit;
class StoreGateSvc;
class MMT_Parameters;
class MmDigitContainer;

namespace MuonGM {
  class MuonDetectorManager;
}

 class MMLoadVariables : public AthMessaging {

  public:

    MMLoadVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MmIdHelper* idhelper);

    StatusCode getMMDigitsInfo(std::map<std::pair<int,unsigned int>,std::vector<digitWrapper> >& entries,
                         std::map<std::pair<int,unsigned int>,std::map<hitData_key,hitData_entry> >& Hits_Data_Set_Time,
                         std::map<std::pair<int,unsigned int>,evInf_entry>& Event_Info,
                         std::map<std::string,std::shared_ptr<MMT_Parameters> > &pars);
    //Import_Athena..._.m stuff
    double phi_shift(double athena_phi,const std::string& wedgeType, int stationPhi) const;
    int Get_VMM_chip(int strip) const;  //*** Not Finished... Rough
    int strip_number(int station, int plane, int spos, std::shared_ptr<MMT_Parameters> par)const;
    int Get_Strip_ID(double X,double Y,int plane) const;
    bool Mimic_VMM_Chip_Deadtime(hitData_entry& candy);
    void xxuv_to_uvxx(ROOT::Math::XYZVector& hit, int plane, std::shared_ptr<MMT_Parameters> par)const;
    void hit_rot_stereo_fwd(ROOT::Math::XYZVector& hit, std::shared_ptr<MMT_Parameters> par)const;//x to v, u to x
    void hit_rot_stereo_bck(ROOT::Math::XYZVector& hit, std::shared_ptr<MMT_Parameters> par)const;//x to u, v to x

    struct histogramVariables{
        std::vector<std::string> *m_NSWMM_dig_stationName;
        std::vector<int> NSWMM_dig_stationEta;
        std::vector<int> NSWMM_dig_stationPhi;
        std::vector<int> NSWMM_dig_multiplet;
        std::vector<int> NSWMM_dig_gas_gap;
        std::vector<int> NSWMM_dig_channel;

        std::vector< std::vector<float> >  NSWMM_dig_time;
        std::vector< std::vector<float> >  NSWMM_dig_charge;
        std::vector< std::vector<int> >    NSWMM_dig_stripPosition;
        std::vector< std::vector<double> > NSWMM_dig_stripLposX;
        std::vector< std::vector<double> > NSWMM_dig_stripLposY;
        std::vector< std::vector<double> > NSWMM_dig_stripGposX;
        std::vector< std::vector<double> > NSWMM_dig_stripGposY;
        std::vector< std::vector<double> > NSWMM_dig_stripGposZ;
        std::vector< std::vector<float> >  NSWMM_dig_sr_time;
        std::vector< std::vector<float> >  NSWMM_dig_sr_charge;
        std::vector< std::vector<int> >    NSWMM_dig_sr_stripPosition;
        std::vector< std::vector<double> > NSWMM_dig_sr_stripLposX;
        std::vector< std::vector<double> > NSWMM_dig_sr_stripLposY;
        std::vector< std::vector<double> > NSWMM_dig_sr_stripGposX;
        std::vector< std::vector<double> > NSWMM_dig_sr_stripGposY;
        std::vector< std::vector<double> > NSWMM_dig_sr_stripGposZ;

        std::vector< int    > NSWMM_dig_truth_barcode;
        std::vector< double > NSWMM_dig_truth_localPosX;
        std::vector< double > NSWMM_dig_truth_localPosY;
        std::vector< double > NSWMM_dig_truth_globalPosX;
        std::vector< double > NSWMM_dig_truth_globalPosY;
        std::vector< double > NSWMM_dig_truth_globalPosZ;
        std::vector< float  > NSWMM_dig_truth_XZ_angle;

        std::vector<int>   NSWMM_dig_stripForTrigger;
        std::vector<float> NSWMM_dig_stripTimeForTrigger;
    };
    histogramVariables histVars;

  private:
  
  //moved here
     //VMM info
    std::vector<std::vector<bool> > m_VMM_ChipStatus{};
    std::vector<std::vector<int> > m_VMM_ChipLastHitTime{};

    double m_VMM_Deadtime{};
    int m_numVMM_PerPlane{};
    //x <---> u/v switches
    bool m_uvxxmod{};
    
    //

    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const MmIdHelper* m_MmIdHelper;        //!< MM offline Id helper
    StoreGateSvc* m_evtStore;
    bool m_striphack{};


  };


#endif
