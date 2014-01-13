/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_READTINY_H
#define ANP_READTINY_H

/**********************************************************************************
 * @Package: TrigCostAthena
 * @Class  : ReadTiny
 * @Author : Aidan Randle-Conde, Stephen Sekula, Rustem Ospanov
 *
 * @Brief  : Tool for recording reconstructed vertex informartion and trigger
 *           decision information
 *
 *  -- check run number, read eb files for each new run number
 *  -- find online entry for current ntuple event 
 *
 *
 *
 *
 **********************************************************************************/

// C++
#include <time.h>

// ROOT
#include "Rtypes.h"
#include "TChain.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/ConfigABC.h"

namespace Anp
{   
  class ReadTiny : public virtual AlgEvent
  {
  public:

    ReadTiny();
    virtual ~ReadTiny();
       
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Done();

    void Exec();

  private:

    struct VertexData
    {
      VertexData();

      Float_t  dVtx_x;
      Float_t  dVtx_y;
      Float_t  dVtx_z;
      Float_t  dVtx_chi2;
      Float_t  dSumPt;
      Int_t    dVtx_ndof;
      Int_t    dNTrack;
    };

    struct EventData 
    {
      EventData();

      Int_t   dRunNumber ;
      Int_t   dLumiblock ;
      Int_t   dEventNumber ;
      Int_t   dBunchId ;
      Int_t   dTimestamp;
      Int_t   dTimestamp_ns;
      Int_t   dNVtx;
      Float_t dMu ;
      Int_t   dBCIDStatus ;

      std::vector<uint16_t>   dEventLV1;
      std::vector<uint32_t>   dEventHLT;      
      std::vector<VertexData> dVertex;
    };
    
    struct BCIDData
    { 
      // Bunch structure information
      std::vector<unsigned> dFirstBunches ;
      std::vector<unsigned> dOtherBunches ;
    };
    
    typedef std::pair<uint32_t, uint32_t>      EventKey;
    typedef std::map<uint32_t, TrigMonConfig> ConfigMap;
    typedef std::map<EventKey, EventData>      EventMap;
    typedef std::map<Int_t, Float_t>              MuKey;    
    typedef std::map<unsigned, unsigned>      NEventKey;    
    typedef std::map<Int_t, BCIDData>           BCIDKey;
    
    struct RunData
    {
      RunData();

      EventMap                  events;

      bool                      isConfig;
      Int_t                     dHLTPrescaleKey ;
      Int_t                     dLV1PrescaleKey ;
      Int_t                     dMasterKey ;
      
      std::vector<std::string>  dTrigNames ;
      std::vector<uint32_t>     dTrigIds   ;
      
      // Removed to prevent std::bad_alloc error
      //BCIDData                  dBCID ;
    };
    
    typedef std::map<unsigned, RunData>                     RunMap;
    typedef std::map<unsigned, std::vector<std::string> >  FileMap;
    typedef std::map<unsigned, unsigned >                 NFileMap;
    typedef std::map<unsigned, MuKey >                       MuMap;
    typedef std::map<unsigned, NEventKey >               NEventMap;
    typedef std::map<unsigned, BCIDData>                   BCIDMap;
    unsigned NTotalEvent ;

  private:
  
    RunMap    fRuns     ;
    ConfigMap fConfigs  ;
    FileMap   fFileMap  ;
    NFileMap  fNFileMap ;
    MuMap     fMuMap    ;
    BCIDMap   fBCIDMap  ;
    NEventMap fNEventMap;

    bool UpdateEvent(TrigMonEvent &event);

    bool UpdateVertex(TrigMonEvent &event, const EventData &data) const;

    bool UpdateOnline(TrigMonEvent &event, const EventData &data) const;

    bool ReadFiles(unsigned run, RunData &data);       

    //void Read(TFile &file, RunData &data);

    void ReadChains(TChain &chainEvent, TChain &chainConfig, RunData &data);

    bool ReadXML();

    bool CheckConfig(const TrigMonConfig &config, const RunData &data) const;

  private:

    Branch<TrigMonEvent> fNtupleEntry; // Current trigger decions
    Branch<TrigMonEvent> fOnlineEntry; // Online  trigger decisions
    Handle<ConfigABC>    fConf;        // Configuration algorithm

    //
    // Properties:
    //
    bool fDebug;

    std::string fTreeNameEvent  ; // event  tree name
    std::string fTreeNameConfig ; // config tree name
    std::string fEosXmlPath     ; // XML for eos input files
    std::string fMuXmlPath      ; // XML for mu
    std::string fBCIDXmlPath    ; // XML for BCID

    //
    // Branches per event
    //
    Int_t fRunNumber ;
    Int_t fLumiblock ;
    Int_t fEventNumber ;
    Int_t fBunchId ;
    Int_t fTimestamp;
    Int_t fTimestamp_ns;
    Int_t fNVtx;

    std::vector<Float_t>  *fVtx_x;
    std::vector<Float_t>  *fVtx_y;
    std::vector<Float_t>  *fVtx_z;
    std::vector<Float_t>  *fVtx_chi2;
    std::vector<Int_t>    *fVtx_ndof;

    std::vector<uint16_t> *fEventLV1;
    std::vector<uint32_t> *fEventHLT;

    //
    // Branches per config
    //
    Int_t fHLTPrescaleKey ;
    Int_t fLV1PrescaleKey ;
    Int_t fMasterKey ;

    std::vector<std::string>  *fTrigNames ;
    std::vector<uint32_t>     *fTrigIds   ;

    // Count events
    int fNFailedUpdate      ;
    int fNFailedBCIDUpdate  ;
    int fNNonoverlappingVtx ;
    int fNFailedMu ;
    int fCounter ;
    
    time_t fT0 ;
    time_t fT1 ;
  };
}

#endif
