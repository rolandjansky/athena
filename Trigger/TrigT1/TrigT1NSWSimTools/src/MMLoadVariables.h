/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMLOADVARIABLES_H
#define MMLOADVARIABLES_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"


//forward declarations
//class IIncidentSvc;
//class IAtRndmGenSvc;
class MmIdHelper;
class MmDigit;
class TTree;

//C++ language libararies
#include <dirent.h>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>

//Muon Software Includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonDigitContainer/MmDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
//#include "MuonReadoutGeometry/NSWenumeration.h"
//#include "MuonReadoutGeometry/NSWgeometry.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CLHEP/Vector/ThreeVector.h"

//ROOT libraries
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TH2D.h"
#include "Rtypes.h"
#include "TTree.h"
#include "TFile.h"
#include "TObject.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "MMT_struct.h"
#include "MMT_Finder.h"
#include "MMT_Fitter.h"
namespace MuonGM {
  class MuonDetectorManager;
}


  class MMLoadVariables {

  public:

    MMLoadVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MmIdHelper* idhelper, MMT_Parameters *par);
   ~MMLoadVariables();

    void getMMDigitsInfo(vector<digitWrapper>& entries, map<hdst_key,hdst_entry>& Hits_Data_Set_Time, map<int,evInf_entry>& Event_Info);

    string nom;

    //map<hdst_key,hdst_entry> Hits_Data_Set_Time;//key is hit_index? <BC_time,time>?

    //VMM info
    vector<vector<bool> > VMM_chip_status;
    vector<vector<int> > VMM__chip_last_hit_time;
    bool Mimic_VMM_Chip_Deadtime(hdst_entry& candy);
    double VMM_deadtime;
    int num_VMM_per_plane;

    //Import_Athena..._.m stuff
    double phi_shift(double athena_phi,std::string wedgeType, int stationPhi) const;
    int Get_VMM_chip(int strip) const;  //*** Not Finished... Rough
    int strip_number(int station,int plane,int spos)const;
    int Get_Strip_ID(double X,double Y,int plane) const;

    //x <---> u/v switches
    bool uvxxmod;
    void xxuv_to_uvxx(TVector3& hit,int plane)const;
    void hit_rot_stereo_fwd(TVector3& hit)const;//x to v, u to x
    void hit_rot_stereo_bck(TVector3& hit)const;//x to u, v to x

    //Hist stuff
    //TH1::SetDefaultSumw2();
    //TH1D *m_fit_the;
    //event counter should be fixed when code is better understood
    int evtcount = 0;

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


  private:

    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const MmIdHelper*                  m_MmIdHelper;        //!< MM offline Id helper
    StoreGateSvc*                      m_evtStore;
    MMT_Parameters* m_par;
    bool striphack = false;
    std::string getWedgeType(const MmDigitContainer *nsw_MmDigitContainer);


  };


#endif
