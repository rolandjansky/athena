/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROAD_H
#define FTKROAD_H

#include "../TrigFTKSim/PatternBank.h"

#include <TClonesArray.h>
#include <TObject.h>
#include <TBits.h>

#include <vector>
#include <string>
#include <ostream>

#include "FTKHit.h"


class FTKRoad : public TObject {
private:
  int m_DCMatchMode; // comunicate the DC match mode, used by the FTKRoadStream to retrieve the hits

  int m_roadID; // global road ID for this bank

  int m_bankID; // bank ID, should be unique, formed as #SUBREG*100+#REG
   
  int m_patternID; // pattern ID, for the BankID pattern bank

  int m_patternDBID; // if the pattern is from a DB bank this is the pattern ID to select in the DB Bank

  int m_sectorID; // sector ID of this pattern

  int m_DCmask; // describe the "don't care" bitmask

  int m_HLmask; // describe the map of used internal strips where DC is not set

  int m_HF_status; // additional hitfilter data: flags matched roads by setting it to nmatch

  int m_HF_rejected; // flag >0 if ss-shifted hit filter rejected this road

  int m_RW_rejected; // flag >0 if a RW killed this road

  int m_RW_roadID; // ID of the road killed this road
  
  int m_RW_bankID; // bank unique identifier

  int m_nhits; // number of fired hits

  unsigned int m_bitmask; // bit mask

  int m_nplanes; // number of planes

  int *m_ssid; //[m_nplanes] id of the SS in the pattern related to this road

  int *m_hlid; //[m_nplanes] the number of bits used in each layer

  int *m_hlidoff; //! offset for each layer;
  
  int *m_SubSSMask; //[m_nplanes] for each layer the mask notified with are the active sub-SS, the number of position is the double of the number of bits for the given layer

  int m_nsubroads; // number of sub-roads
  TClonesArray *m_subroads; //-> list of sub-roads

  FTKRoad *m_AMroad; //! main AM road, is the same of this for AM roads

  // A pointer to m_ssmaps list of hits (located in FTKRoadStream)
  // NOTE: this pointer is set dynamically via attachHits(), and is not saved in root file!
  std::vector< std::vector<FTKHit> > m_hits; //! hits, divided by plane
  
  // for each layer, add a bitmap containing result of hit filter for each hit
  // NOTE: this is stored in the root file and used at track fitter stage
  std::vector<TBits> m_hits_mask; // HF bitmask, divided by plane
  
  // vector that will contain the result

  long int *m_evtidx; //!
  long int *m_barcode; //!
  float *m_frac; //!

  static int m_verbose; //! verbosity level when road information is printed
public:
  FTKRoad();
  FTKRoad(int,int,int,int,int,unsigned int);
  FTKRoad(const FTKRoad &);
  virtual ~FTKRoad();
  
  virtual Bool_t IsSortable() const
    { return kTRUE; }
  virtual Int_t Compare(const TObject*) const;

  void addHits(int, const std::vector<FTKHit>&);

  /** check if hit # "idx" in the superstrip on plane "pl" failed hit filter */
  bool hit_failed_HF(int pl,int idx) const {
    if(m_hits_mask.empty()) return false; // backward compatibility
    return m_hits_mask[pl].TestBitNumber(idx);
  };
  void clearHitsMask() { m_hits_mask.clear(); }
  void reserveHitsMask(int newsize) { m_hits_mask.resize(newsize); }
  void setHitsMask(int pl,TBits v) { m_hits_mask[pl] = v; }
  void setHitsMask(const std::vector<TBits>& v) { m_hits_mask = v; }
  std::vector<TBits> getHitsMask() const {return m_hits_mask;}
  TBits getHitsMaskByPlane(int pl) const {return m_hits_mask[pl];}
  
  void setBankID(int v);
  void setPatternID(int v) { m_patternID = v; }
  void setPatternDBID(int v) { m_patternDBID = v; }
  void setSectorID(int v) { m_sectorID = v; }
  void setRoadID(int v) { m_roadID = v; }
  void setNPlanes(int v);
  void setNHits(int v) { m_nhits = v;}
  void setHFStatus(int v) { m_HF_status = v; }  
  void setHFRejected(int v) { m_HF_rejected = v; }
  void setRWRejected(int v) { m_RW_rejected = v; }
  void setKillerRoad(const FTKRoad *road);
  void setKillerRoad(int rid, int bid) 
    { m_RW_roadID = rid; m_RW_bankID = bid; }
  void setSSID(int pl, int id) { m_ssid[pl] = id; }
  void setHLID(int pl, int id) { m_hlid[pl] = id; }
  void setSubSSMask(int pl, int id) { m_SubSSMask[pl] = id; }
  void setBitMask(unsigned int v){ m_bitmask = v; }
  void setDCBitmask(int v) { m_DCmask = v; }
  void setHLBitmask(int v) { m_HLmask = v; }
  void setAMRoad(FTKRoad *amroad) { m_AMroad = amroad; }

  int getBankID() const { return m_bankID; }
  int getRegion() const { return m_bankID%100; }
  int getSubRegion() const { return m_bankID/100; }
  int getPatternID() const { return m_patternID; }
  int getPatternDBID() const { return m_patternDBID; }
  int getSectorID() const { return m_sectorID; }
  int getRoadID() const { return m_roadID; }
  std::string getBitmaskASCII() const;
  int getHFRejected() const { return m_HF_rejected; }
  int getHFStatus() const { return m_HF_status; }
  int getRWRejected() const { return m_RW_rejected; }
  int getRWRoad() const { return m_RW_roadID; }
  int getRWBank() const { return m_RW_bankID; }
  int getNPlanes() const { return m_nplanes; }
  const int& getSSID(int pl) const { return m_ssid[pl]; }
  int getHLID(int pl) const { return !m_hlid ? 0 :m_hlid[pl]; }
  const int& getSubSSMask(int pl) const { return m_SubSSMask[pl]; }
  const int& getHLIDOffset(int pl);
  const FTKRoad *getAMRoad() const { return m_AMroad; }

  int getNHits() const { return m_nhits; }
  unsigned int getBitMask() const { return m_bitmask; }
  bool hasHitOnLayer(int pl) const { return m_bitmask & (1 << pl) ? true : false;} 
  const int& getDCBitmask() const { return m_DCmask; }
  bool hasDCOnLayer(int pl) const { return m_DCmask & (1<<pl); }
  const int& getHLBitmask() const { return m_HLmask; }

  FTKRoad* addSubRoad(const FTKRoad &);
  FTKRoad* getSubRoad(int iroad) { return dynamic_cast<FTKRoad*>(m_subroads->At(iroad)); }
  const FTKRoad* getSubRoad(int iroad) const { return dynamic_cast<const FTKRoad*>(m_subroads->At(iroad)); }
  int getNSubRoads() const { return m_nsubroads; }

  unsigned int getNHits(int pl) const;
  const FTKHit& getHit(int pl, int i) const { return m_hits[pl][i]; }
  const std::vector<FTKHit>& getHits(int pl) const { return m_hits[pl]; }
  const MultiTruth& getTruth(int pl, int id) const { return m_hits[pl][id].getTruth(); }
  void clearHits();

  void prepareHits();

  int getNCombs(); // return the number of combinations

  void setDCMatchMode(int v) { m_DCMatchMode = v; }
  int getDCMatchMode() const { return m_DCMatchMode; }

  int computeTruthList();
  long int getEventIndex(int idx) const { return m_evtidx[idx];}
  long int getBarcode(int idx ) const { return m_barcode[idx]; }
  float getBarcodeFrac(int idx) const { return m_frac[idx]; }

  static bool doStaticRoadWarrior(const FTKRoad&, 
				  const PatternDefinition&,
				  const FTKRoad&,
				  const PatternDefinition&);
  
  bool operator<(const FTKRoad&) const;
  bool operator>(const FTKRoad&) const;


  static void setVerbose(int val) { m_verbose = val; }
  static int getVerbose() { return m_verbose; }

  friend std::ostream& operator<<(std::ostream&, const FTKRoad&);

  void Print(Option_t* option = "") const;

  ClassDef(FTKRoad,12)
};

#endif // FTKROAD_H
