/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSTRIPTDSOFFLINETOOL_H
#define MMSTRIPTDSOFFLINETOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"

//local includes
#include "TrigT1NSWSimTools/IMMStripTdsTool.h"


//forward declarations
class IIncidentSvc;
class IAtRndmGenSvc;
class MmIdHelper;
class MmDigit;
class TTree;

//MMT_Loader includes

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
//#include "/afs/cern.ch/user/m/mkhader/mm_trig/MM_Trigger/plots/mmt_plot/atlasstyle-00-03-05/AtlasStyle.h"




namespace CLHEP {
  class HepRandomEngine;
}

namespace MuonGM {
  class MuonDetectorManager;
}


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short interface for the StripTDS tools
   *
   * This class implements the Strip TDS offline simulation. It loops over the input digits, 
   * determines the BC tag and applies the additional processing of the VMM chip which is
   * not yet implemented in the digitization. The loop is executed over the full digit pool
   * once upon the first data request for an event and the PAD data are internally cached
   * and collected per trigger sectors. The run ID and event ID are cached; the processing
   * status is also cached to be propagated via a StatusCode at each data request. 
   *
   * Supported processing:
   *  Time delay simulation by subtracting the time_of_flight and the electronic random jitter;
   *  VMM Dead Time simulation;
   *
   * It returns a vector of MMStripData to input the MMStripTrigger simulation.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *
   *
   */

  class MMStripHits;

  class MMStripTdsOfflineTool: virtual public IMMStripTdsTool, 
    public AthAlgTool, 
    public IIncidentListener {

  public:

    //MMTrigger stuff
    int eta_bin(double theta) const;
    string eta_str(int eta) const;
    int n_etabins;
    int correct_bcid;
    vector<double> m_etabins;
    double etalo,etahi;
    
    string nom;

    //MMT_Loader stuff
    //debug stuff
    vector<vector<vector<double> > >strip_poss,yvals;
    map<int,double> strip500;
    map<int,int> zplanes;
    vector<TH1D*> m_diff_xuv;
    MMT_Parameters *m_par;
    //MMT_Finder m_find;

    //load event stuff
    vector<hitData_entry> event_hitDatas(int find_event) const;
    vector<hitData_key> event_hitData_keys(int find_event) const;

    map<int,evFit_entry> Event_Fit;//key is event no.
    map<int,evInf_entry> Event_Info;//key is event no.
    map<int,evAna_entry> Event_Analysis;//key is event no.
    map<hitData_key,hitData_entry> Hits_Data_Set_Time;//key is hit_index? <BC_time,time>?

    //VMM info
    vector<vector<bool> > VMM_chip_status;
    vector<vector<int> > VMM__chip_last_hit_time;
    bool Mimic_VMM_Chip_Deadtime(hitData_entry& candy);
    double VMM_deadtime;
    int num_VMM_per_plane;

    //Import_Athena..._.m stuff
    double phi_shift(double athena_phi) const;
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

    


    //MMT_Loader stuff end

    enum cStatus {OK, FILL_ERROR, CLEARED};

    MMStripTdsOfflineTool(const std::string& type, 
			  const std::string& name,
			  const IInterface* parent);

    virtual ~MMStripTdsOfflineTool();

    virtual StatusCode initialize();

    virtual void handle (const Incident& inc);

    StatusCode gather_mmstrip_data(std::vector<MMStripData*>& mmstrips, int side=-1, int sector=-1);

    
  private:

    
    // methods implementing the internal data processing
    cStatus fill_mmstrip_cache();                           //!< loop over the digit container, apply the additional processing then fill the cache
    void clear_cache();                                     //!< clear the MM strip hit cache deleting the MMStripData pointers

    StatusCode book_branches();                             //!< book the branches to analyze the MMStripTds behavior
    void reset_ntuple_variables();                          //!< reset the variables used in the analysis ntuple
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple
    void fill_mmstrip_validation_id();                      //!< fill the ntuple branch for the MMStripTdsOffline
 

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
    ServiceHandle< IAtRndmGenSvc >     m_rndmSvc;           //!< Athena random number service
    CLHEP::HepRandomEngine*            m_rndmEngine;        //!< Random number engine
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const MmIdHelper*                  m_MmIdHelper;        //!< MM offline Id helper

    // hidden variables
    std::vector< std::vector<MMStripData*> > m_mmstrip_cache; //!< cache for the MM Strip hit data in the event
    int     m_mmstrip_cache_runNumber;                            //!< run number associated to the current MM Strip cache
    int     m_mmstrip_cache_eventNumber;                          //!< event number associated to the current MM Strip cache
    cStatus m_mmstrip_cache_status;                               //!< status of the current cache
    

    // properties: container and service names
    StringProperty   m_rndmEngineName;                      //!< property, see @link MMStripTdsOfflineTool::MMStripTdsOfflineTool @endlink
    StringProperty   m_MmDigitContainer;                    //!< property, see @link MMStripTdsOfflineTool::MMStripTdsOfflineTool @endlink
    StringProperty   m_MmSdoContainer;                      //!< property, see @link MMStripTdsOfflineTool::MMStripTdsOfflineTool @endlink
    StringProperty   m_MmHitContainer;
    StringProperty   m_Truth_ContainerName;
    StringProperty   m_MuEntry_ContainerName;

    BooleanProperty  m_doNtuple;                            //!< property, see @link MMStripTdsOfflineTool::MMStripTdsOfflineTool @endlink


    // analysis ntuple
    TTree* m_tree;                                          //!< ntuple for analysis

    // analysis variable to be put into the ntuple 

    std::vector<double>* m_fitThe;
    std::vector<double>* m_fitPhi;
    std::vector<double>* m_fitDth;
    std::vector<double>* tru_etarange;
    std::vector<double>* tru_ptrange;
    std::vector<double>* fit_etarange;
    std::vector<double>* fit_ptrange;
    std::vector<double>* res_the;
    std::vector<double>* res_phi;
    std::vector<double>* res_dth;


    /*
    int m_nMMStripHits;                                         //!< number of Strip hit delivered
    std::vector<int>* m_MMhitPDGId;                             //!< PDG id of particles creating the hits
    std::vector<float>* m_MMhitDepositEnergy;                   //!< energy deposited by particles
    std::vector<float>* m_MMhitKineticEnergy;                   //!< kinetic energy of particles
    std::vector<float>* m_mmstripGlobalX;                       //!< global position X of the Strip hit
    std::vector<float>* m_mmstripGlobalY;                       //!< global position Y of the Strip hit
    std::vector<float>* m_mmstripGlobalZ;                       //!< global position Z of the Strip hit
    std::vector<float>* m_mmstripTruthHitGlobalX;               //!< global position X of the truth hit associated to the Strip hit
    std::vector<float>* m_mmstripTruthHitGlobalY;               //!< global position Y of the truth hit associated to the Strip hit
    std::vector<float>* m_mmstripTruthHitGlobalZ;               //!< global position Z of the truth hit associated to the Strip hit
    */

    //std::vector<float>* m_mmstripGlobalX;                       //!< global position X of the Strip hit
    //std::vector<float>* m_mmstripGlobalY;                       //!< global position Y of the Strip hit
    //std::vector<float>* m_mmstripGlobalZ;
    //These are not in MMDigitvariables, need to find what they correspond to in previous ntuple

    // Variables from MMDigitVariables for ntuple (maybe keep)


    //int m_NSWMM_nDigits;
    std::vector<std::string> *m_NSWMM_dig_stationName;
    std::vector<int> *m_NSWMM_dig_stationEta;
    std::vector<int> *m_NSWMM_dig_stationPhi;
    std::vector<int> *m_NSWMM_dig_multiplet;
    std::vector<int> *m_NSWMM_dig_gas_gap;
    std::vector<int> *m_NSWMM_dig_channel;

    std::vector< std::vector<float> >  *m_NSWMM_dig_time;
    std::vector< std::vector<float> >  *m_NSWMM_dig_charge;
    std::vector< std::vector<int> >    *m_NSWMM_dig_stripPosition;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposZ;
    std::vector< std::vector<float> >  *m_NSWMM_dig_sr_time;
    std::vector< std::vector<float> >  *m_NSWMM_dig_sr_charge;
    std::vector< std::vector<int> >    *m_NSWMM_dig_sr_stripPosition;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposZ;

    std::vector< int    > *m_NSWMM_dig_truth_barcode;
    std::vector< double > *m_NSWMM_dig_truth_localPosX;
    std::vector< double > *m_NSWMM_dig_truth_localPosY;
    std::vector< double > *m_NSWMM_dig_truth_globalPosX;
    std::vector< double > *m_NSWMM_dig_truth_globalPosY;
    std::vector< double > *m_NSWMM_dig_truth_globalPosZ;
    std::vector< float  > *m_NSWMM_dig_truth_XZ_angle;

    std::vector<int>   *m_NSWMM_dig_stripForTrigger;
    std::vector<float> *m_NSWMM_dig_stripTimeForTrigger;


    std::vector<int>    *m_NSWMM_trackId;
    std::vector<int>    *m_NSWMM_truthEl;
    std::vector<double> *m_NSWMM_globalTime;
    std::vector<double> *m_NSWMM_hitGlobalPositionX;
    std::vector<double> *m_NSWMM_hitGlobalPositionY;
    std::vector<double> *m_NSWMM_hitGlobalPositionZ;
    std::vector<double> *m_NSWMM_hitGlobalPositionR;
    std::vector<double> *m_NSWMM_hitGlobalPositionP;
    std::vector<double> *m_NSWMM_hitGlobalDirectionX;
    std::vector<double> *m_NSWMM_hitGlobalDirectionY;
    std::vector<double> *m_NSWMM_hitGlobalDirectionZ;
  
    std::vector<double> *m_NSWMM_hitLocalPositionX;
    std::vector<double> *m_NSWMM_hitLocalPositionY;
    std::vector<double> *m_NSWMM_hitLocalPositionZ;
 
    std::vector<double> *m_NSWMM_detector_globalPositionX;
    std::vector<double> *m_NSWMM_detector_globalPositionY;
    std::vector<double> *m_NSWMM_detector_globalPositionZ;
    std::vector<double> *m_NSWMM_detector_globalPositionR;
    std::vector<double> *m_NSWMM_detector_globalPositionP;
 
    std::vector<double> *m_NSWMM_hitToDsurfacePositionX;
    std::vector<double> *m_NSWMM_hitToDsurfacePositionY;
    std::vector<double> *m_NSWMM_hitToDsurfacePositionZ;
  
    std::vector<double> *m_NSWMM_hitToRsurfacePositionX;
    std::vector<double> *m_NSWMM_hitToRsurfacePositionY;
    std::vector<double> *m_NSWMM_hitToRsurfacePositionZ;
 

    std::vector<int> *m_NSWMM_particleEncoding;
    std::vector<double> *m_NSWMM_kineticEnergy;
    std::vector<double> *m_NSWMM_depositEnergy;
    std::vector<double> *m_NSWMM_StepLength;
 
    std::vector<std::string> *m_NSWMM_sim_stationName;
    std::vector<int> *m_NSWMM_sim_stationEta;
    std::vector<int> *m_NSWMM_sim_stationPhi;
    std::vector<int> *m_NSWMM_sim_multilayer;
    std::vector<int> *m_NSWMM_sim_layer; 
    std::vector<int> *m_NSWMM_sim_side;

    std::vector<std::string> *m_NSWMM_off_stationName;
    std::vector<int> *m_NSWMM_off_stationEta;
    std::vector<int> *m_NSWMM_off_stationPhi;
    std::vector<int> *m_NSWMM_off_multiplet;
    std::vector<int> *m_NSWMM_off_gas_gap;
    std::vector<int> *m_NSWMM_off_channel;


  };  // end of MMStripTdsOfflineTool class

} // namespace NSWL1

#endif
