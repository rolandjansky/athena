/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SCALEBG_H
#define ANP_SCALEBG_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : ScaleBG
 * @Author : Monica Dunford, Rustem Ospanov
 *
 * @Brief  : Algorithm for scaling event weight by:
 *           - input and target luminosity 
 *           - bunch group pattern
 *           - trigger configuration used to record data
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/ScaleABC.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateGroup.h"

namespace Anp
{
  class ScaleBG: public virtual AlgEvent, public virtual ScaleABC {
  public:

    ScaleBG();
    virtual ~ScaleBG();

    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    long double GetScaleCorr() const { return 0.0; }
    Scale::ScaleBGInfo GetScaleBCID() const { return fScaleBCID; }
    long double GetPredLumi() const { return fPredLumi; }
    std::vector<int> GetPredBunchGroupNum() const {return fPredBGSize; }
    long double GetScalePileup() const { return 1.0;}

  private:

    struct LV1Data {
      LV1Data() : lv1_ps(0.0), lv1_weight(0.0), ctp_id(10000), hlt_chain_ps(1.0), hlt_chain_weight(0.0) {}
      
      std::string  lv1_name;
      double       lv1_ps;
      double       lv1_weight;
      unsigned     ctp_id;
      std::string  hlt_chain_name;
      double       hlt_chain_ps;
      double       hlt_chain_weight;
      
      std::set<std::string> lv1_thr;
    };

    struct HLTData {
      HLTData() : runnumber_low(0), runnumber_high(0), hlt_ps(1.0), l1_eb_weight(1.0), hlt_weight(0.0), tot_weight(0.0), hlt_counter(10000), hlt_eb_unbias_ps(1.0), hlt_eb_unbias_weight(0.0), l1_eb_unbias_counter(0), hlt_eb_unbias_counter(100000) {}
      
      void ResetTriggers(bool fromXML);

      void PrintTriggers() const;

      void UpdateHLT(const TrigMonConfig &config, std::ostream &os);
      void UpdateLV1(const TrigMonConfig &config, std::vector<LV1Data> &lv1_vec) const;
      void UpdateEBPS(const TrigMonConfig &config, std::vector<LV1Data> &lv1_vec) const;
      void GetLVL1Seeds(const TrigMonConfig &config, bool fromXML, std::ostream &os);
      void GetHypoSeeds(const TrigMonConfig &config, bool fromXML, std::ostream &os);

  
      std::string  filter_name;
      std::string  filter_type;
      std::string  ef_unbiastrig;
      unsigned     runnumber_low;
      unsigned     runnumber_high;
      std::vector<HLTData> hlt_same_seeds;  // List of EB triggers with the same L1 seed (namely RD0)
      
      double       hlt_ps;
      double       l1_eb_weight;
      double       hlt_weight;
      double       tot_weight;
      double       l1_rndweight;
      unsigned     hlt_counter;
      double       hlt_eb_unbias_ps;
      double       hlt_eb_unbias_weight;
      unsigned     l1_eb_unbias_counter;
      unsigned     hlt_eb_unbias_counter;
      
      std::vector<LV1Data> lv1_filter;  // Require that these triggers passed TAV
      std::vector<LV1Data> lv1_weight;  // Use these triggers to compute L1 weight
      std::vector<LV1Data> lv1_ebtrigs;  // Use these triggers combined EF trigger weights for the EB L1 triggers
    };

    typedef std::map<std::string, Trig::BunchGroup> BGMap;

  private:

    bool Run(const TrigMonEvent &event);

    std::vector<unsigned> UpdateFilters(const TrigMonConfig &config, std::ostream &os);
    bool FilterTriggers(const TrigMonEvent &event, double &weight, double &weight_rd0, double &weight_rd1);
    bool FilterTriggersOnline(const TrigMonEvent &event, double &weight, double &weight_rd0, double &weight_rd1);

    void ReadCurrentData(const TrigMonEvent &event);

    void ParseXML(const std::string &fname);
    
  private:    

    Branch<TrigMonEvent>        fEntry;            // Trigger events
    Branch<TrigMonEvent>        fOnlineEntry;      // Trigger events from online (tiny ntuple)
    Handle<ConfigABC>           fDataConfAlg;      // Configuration algorithm for input data
    Handle<ConfigABC>           fPredConfAlg;      // Configuration algorithm for prediction

    Handle<LumiABC>             fDataLumiAlg;      // Luminosity algorithm for input data
    Handle<LumiABC>             fPredLumiAlg;      // Luminosity algorithm for prediction

    // Properties:
    bool                        fDebug;            // Print debugging info
    bool                        fIsConfig;         // check of configuration status
    bool                        fIsCAFReprocess;   // Checks the format of the input cost_ntuple
    bool                        fReadConfigXML;    // Read EB Configuation from XML instead of tiny ntuple
    std::string                 fXMLPath;          // Path to XML file with update configuration
    std::string                 fLumiAlg;          // Name of luminosity algorithm
    
    unsigned                    fPredRun;          // Run number for rate prediction
    unsigned                    fPredLB;           // LB  number for rate prediction

    // Variables
    BGMap                       fBGData;           // BG for input data
    BGMap                       fBGPred;           // BG for rate prediction

    unsigned                    fCountPass;        // Count passed events
    unsigned                    fCountFail;        // Count failed events
    unsigned                    fCountSignal;      // Count signal callbacks

    unsigned                    fCurrLB;           // Current lumi block

    double                      fDataLiveFrac;     // Livetime fraction for input data
    double                      fDataLumi;         // Inst. luminosity for input data
    double                      fPredLumi;         // Inst. luminosity for rate prediction
    std::vector<int>            fPredBGSize;       // Bunch group size for predictions
    Scale::ScaleBGInfo          fScaleBCID;        // Scale factor calculated for this event

    std::vector<HLTData>        fHLT;              // HLT enhanced bias triggers
    std::vector<std::string>         fFilters;          // List of HLT filters
    std::vector<unsigned>       fFiltersCounters;  // List of counters for the above HLT filters

    TrigMonChain::Decision      fDecisionBitHLT;   // Decision bit used for HLT rates

  };
}

#endif
