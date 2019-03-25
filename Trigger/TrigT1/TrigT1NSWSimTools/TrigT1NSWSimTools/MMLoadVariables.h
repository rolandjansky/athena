/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMLOADVARIABLES_H
#define MMLOADVARIABLES_H


//C++ language libraries
#include <map>
#include <vector>
#include <string>

#include "MMT_struct.h" //for digitWrapper, hitData_key, hitData_entry, evInf_entry
#include "AthenaBaseComps/AthMsgStreamMacros.h"

//forward declarations
class MsgStream;
class MmIdHelper;
class MmDigit;
class StoreGateSvc;
class MMT_Parameters;
class MmDigitContainer;
class TVector3;

namespace MuonGM {
  class MuonDetectorManager;
}


  class MMLoadVariables {

  public:

    MMLoadVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MmIdHelper* idhelper, MMT_Parameters *par);
   ~MMLoadVariables();

    void getMMDigitsInfo(std::vector<digitWrapper>& entries, std::map<hitData_key,hitData_entry>& Hits_Data_Set_Time, std::map<int,evInf_entry>& Event_Info);
    //Import_Athena..._.m stuff
    double phi_shift(double athena_phi,std::string wedgeType, int stationPhi) const;
    int Get_VMM_chip(int strip) const;  //*** Not Finished... Rough
    int strip_number(int station,int plane,int spos)const;
    int Get_Strip_ID(double X,double Y,int plane) const;
    bool Mimic_VMM_Chip_Deadtime(hitData_entry& candy);
    void xxuv_to_uvxx(TVector3& hit,int plane)const;
    void hit_rot_stereo_fwd(TVector3& hit)const;//x to v, u to x
    void hit_rot_stereo_bck(TVector3& hit)const;//x to u, v to x

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


        std::vector<int>    NSWMM_trackId;
        std::vector<int>    NSWMM_truthEl;
        std::vector<double> NSWMM_globalTime;
        std::vector<double> NSWMM_hitGlobalPositionX;
        std::vector<double> NSWMM_hitGlobalPositionY;
        std::vector<double> NSWMM_hitGlobalPositionZ;
        std::vector<double> NSWMM_hitGlobalPositionR;
        std::vector<double> NSWMM_hitGlobalPositionP;
        std::vector<double> NSWMM_hitGlobalDirectionX;
        std::vector<double> NSWMM_hitGlobalDirectionY;
        std::vector<double> NSWMM_hitGlobalDirectionZ;

        std::vector<double> NSWMM_hitLocalPositionX;
        std::vector<double> NSWMM_hitLocalPositionY;
        std::vector<double> NSWMM_hitLocalPositionZ;

        std::vector<double> NSWMM_detector_globalPositionX;
        std::vector<double> NSWMM_detector_globalPositionY;
        std::vector<double> NSWMM_detector_globalPositionZ;
        std::vector<double> NSWMM_detector_globalPositionR;
        std::vector<double> NSWMM_detector_globalPositionP;

        std::vector<double> NSWMM_hitToDsurfacePositionX;
        std::vector<double> NSWMM_hitToDsurfacePositionY;
        std::vector<double> NSWMM_hitToDsurfacePositionZ;

        std::vector<double> NSWMM_hitToRsurfacePositionX;
        std::vector<double> NSWMM_hitToRsurfacePositionY;
        std::vector<double> NSWMM_hitToRsurfacePositionZ;


        std::vector<int> NSWMM_particleEncoding;
        std::vector<double> NSWMM_kineticEnergy;
        std::vector<double> NSWMM_depositEnergy;
        std::vector<double> NSWMM_StepLength;

        std::vector<std::string> NSWMM_sim_stationName;
        std::vector<int> NSWMM_sim_stationEta;
        std::vector<int> NSWMM_sim_stationPhi;
        std::vector<int> NSWMM_sim_multilayer;
        std::vector<int> NSWMM_sim_layer;
        std::vector<int> NSWMM_sim_side;

        std::vector<std::string> NSWMM_off_stationName;
        std::vector<int> NSWMM_off_stationEta;
        std::vector<int> NSWMM_off_stationPhi;
        std::vector<int> NSWMM_off_multiplet;
        std::vector<int> NSWMM_off_gas_gap;
        std::vector<int> NSWMM_off_channel;
    };
    histogramVariables histVars;
    
    protected:
    /// Log a message using the Athena controlled logging system
      MsgStream&
      msg(MSG::Level lvl) const {
        return m_msg.get() << lvl;
      }
   
      /// Check whether the logging system is active at the provided verbosity level
      bool
      msgLvl(MSG::Level lvl) {
        return m_msg.get().level() <= lvl;
      }
   
      /// Private message stream member
      mutable Athena::MsgStreamMember m_msg;



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

    const MuonGM::MuonDetectorManager* m_detManager{};        //!< MuonDetectorManager
    const MmIdHelper*                  m_MmIdHelper{};        //!< MM offline Id helper
    StoreGateSvc*                      m_evtStore{};
    MMT_Parameters* m_par{};
    bool m_striphack{};
    std::string getWedgeType(const MmDigitContainer *nsw_MmDigitContainer);


  };


#endif
