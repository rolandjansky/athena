/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKFITTER_H
#define TRACKFITTER_H

#include "FTKRoadInput.h"
#include "FTKTrackOutput.h"
#include "FTKConstantBank.h"
#include "FTKTrack.h"
#include "FTKPMap.h"
// output class for firmware tests
//#include "FTKFirmwareOutput.h"

#include <list>
#include <vector>

class TrackFitter {
protected:
  int m_ncoords; // number of coordinates
  int m_nplanes; // number of used layers

  int m_npars; // number of parameters

  float m_Chi2Cut; // Chi2 cut for complete tracks

  float m_Chi2Cut_maj; // Chi2 cut for tracks using majority

  float m_Chi2Cut_vetomaj; // Chi2 for full fits above which majority
			   // is not attempted; not used by default

  float m_Chi2DofCutAux; // Cut on the chi2 per degree of freedom for Aux; not
		      // used by default

  float m_Chi2DofCutSSB; // Cut on the chi2 per degree of freedom for SSB; not
          // used by default

  int m_HitWarrior; // 1 means in-road HW enabled, 2 mean global HW (default)

  int m_HW_ndiff; // maximum number of different points

  float *m_HW_dev; //[m_ncoords] tolerances for the HW

  bool m_AuxDoctor; // true means enabling Aux Doctor, false means disabling it

  int m_keep_rejected; // >0 keep rejected roads (1 HW rej, 2 bad quality)

  int m_fit_removed; /* if >0 fit the hit combinations in the removed road.
			1 - fit roads removed by the in-sector RW
			2 - fit roads removed by the cross-sector RW
			3 - fit roads removed by the overlap RW
		     */

  int m_max_ncomb; // maximum number of combinations to process per road
  int m_max_nhitsperplane; // maximum number of hits per plane to per road (value < 0 => no cut)
  int m_max_trkout; // maximum number of tracks to save per region per event
  int m_norecovery_nhits; // maximum number of hits per superstrip beyond which majority is not attempted

  bool m_one_per_road; // Output only one track per road
  bool m_require_first; // Require a hit in the first layer (usually pixel b-layer)
  unsigned int m_do_majority; // When to do majority fits (explained in trackfitter.tpl)

  FTKRoadInput *m_roadinput; // input module, pointer to use abstract class in the future

  FTKTrackOutput *m_trackoutput; // track output module
  FTKTrackOutput *m_trackoutput_pre_hw; // track output module for tracks before the HW filter

  // output for firmware tests
  //FTKFirmwareOutput *m_fwoutput; // generic firmware output module
  //int m_maxsectors_fwo;
  //int m_nsectors_fwo;

  int m_nregions; // number of regions

  int m_nsubregions; // number of sub-regions

  FTKConstantBank ***m_constant; // constant banks

  bool *m_goodRegion; // an array of flag to report which regions can really be simulated

  ftk::message_level m_noconstants_errlevel; // error level when not valid fit constants are found

  FTKPlaneMap *m_pmap; // plane-map, needed to know hits dimension

  int m_ntracks; // counter of the stored tracks
  int m_ntracks_pre_hw; // counter of the stored tracks before HW filter

  std::list<FTKTrack> m_tracks; // list of output tracks
  std::list<FTKTrack> m_tracks_pre_hw; // list of output tracks before HW filter

  std::list<FTKTrack> m_tracks_hits; // list of stored tracks that pass hit requirements
  std::list<FTKTrack> m_tracks_pattern; // list of stored tracks with patterns

  int m_ncombs; // number of combinations
  int m_nfits; // number of fits tryied in a road
  int m_nfits_maj; // number of majority fits
  int m_nfits_maj_pix; // number of majority fits with missing in PIX layer
  int m_nfits_maj_SCT; // number of majority fits with missing in SCT layer
  int m_nfits_rec; // number of recoverd fits
  int m_nfits_addrec; // number of recoverd fits
  int m_nfits_bad; // number of fits with a bad chi2
  int m_nfits_rej; // number of fits rejected by HW
  int m_nfits_badmaj; // number of fits with a bad chi2
  int m_nfits_rejmaj; // number of fits rejected by HW

  int m_comb_id; // current combination id, has to be unique in each event, for each bank
  FTKTrack newtrk; // fit attempt used in processor() method
  FTKTrack *combtrack; // array of tracks temporary used during track recovery
  unsigned int m_complete_mask;

  // block of iterators used to compose the track fit combinations
  std::vector<FTKHit>::const_iterator *m_startlist;
  std::vector<FTKHit>::const_iterator *m_position;
  std::vector<FTKHit>::const_iterator *m_endlist;
  int *m_hitcnt; // counter that enumerates current hit in FTKHit array

  bool m_identify_badhit; // the flag enables the identification of the bad hits for the recovery

  bool m_saveStepByStepTracks; // true if you want to save incompelte tracks

  virtual void processor_init(int);
  virtual void processor(const FTKRoad &);
  virtual void processor_end(int);
  virtual void compute_truth(const unsigned int&,const FTKRoad &,FTKTrack&) const;
  int doHitWarriorFilter(FTKTrack&,std::list<FTKTrack>&,bool isfirst=false);
  int doAuxDoctor(FTKTrack&,std::list<FTKTrack>&);
  std::list<FTKTrack>::iterator removeTrack(std::list<FTKTrack>&, std::list<FTKTrack>::iterator, FTKTrack&, const FTKTrack&,bool isnew=false);
  std::list<FTKTrack>::iterator removeTrackStage1(std::list<FTKTrack>&, std::list<FTKTrack>::iterator, FTKTrack&, const FTKTrack&,bool isnew=false);

public:
  TrackFitter();

  virtual ~TrackFitter();

  virtual void setNCoordsPlanes(int,int);
  int getNCoords() const { return m_ncoords; }
  int getNPlanes() const { return m_nplanes; }

  void setNoConstantsErrorLevel(ftk::message_level lvl)
    { m_noconstants_errlevel = lvl; }
  ftk::message_level getNoConstatsErrorLevel() const
    { return m_noconstants_errlevel; }

  void setIdentifyBadHit(bool flag) { m_identify_badhit = flag; }
  const bool& getIdentifyBadHit() const { return m_identify_badhit; }

  virtual void prepareBanks(int, int); // set number r regions and sub-regions
  int getNumRegions() const { return m_nregions; }
  int getNumSubRegions() const { return m_nsubregions; }

  void setPlaneMap(FTKPlaneMap *map) { m_pmap = map; }
  FTKPlaneMap* getPlaneMap() const { return m_pmap; }

  void setHitWarrior(int v) { m_HitWarrior = v; }
  int getHitWarrior() const { return m_HitWarrior; }

  void setHWNDiff(int v) { m_HW_ndiff = v; }
  int getHWNDiff() const { return m_HW_ndiff; }

  void setAuxDoctor(bool v) { m_AuxDoctor = v; }
  bool getAuxDoctor() const { return m_AuxDoctor; }

  void setMaxNcomb(int v) { m_max_ncomb = v; }
  void setMaxNhitsPerPlane(int v) { m_max_nhitsperplane = v; }
  void setMaxTrkout(int v) { m_max_trkout = v; }
  void setNorecoveryNhits(int v) { m_norecovery_nhits = v; }

  bool getOnePerRoad() const { return m_one_per_road; }
  void setOnePerRoad(bool v) { m_one_per_road = v; }

  bool getRequireFirst() const { return m_require_first; }
  void setRequireFirst(bool v) { m_require_first = v; }

  unsigned int getDoMajority() const { return m_do_majority; }
  void setDoMajority(unsigned int v) { m_do_majority = v; }

  int getFitRemoved() const { return m_fit_removed; }
  void setFitRemoved(int v) { m_fit_removed = v; }

  int getKeepRejected() const { return m_keep_rejected; }
  void setKeepRejected(int v) { m_keep_rejected = v; }

  void setSaveStepByStepTracks(bool flag) { m_saveStepByStepTracks = flag; }
  bool getSaveStepByStepTracks() const { return m_saveStepByStepTracks; }

  void loadHWConf(const char*);

  void setChi2Cut(float v) { m_Chi2Cut = v; }
  float getChi2Cut() const { return m_Chi2Cut; }

  void setChi2Cut_maj(float v) { m_Chi2Cut_maj = v; }
  float getChi2Cut_maj() const { return m_Chi2Cut_maj; }

  void setChi2Cut_vetomaj(float v) { m_Chi2Cut_vetomaj = v; }
  float getChi2Cut_vetomaj() const { return m_Chi2Cut_vetomaj; }

  void setChi2DofCutAux(float v) { m_Chi2DofCutAux = v; }
  float getChi2DofCutAux() const { return m_Chi2DofCutAux; }

  void setChi2DofCutSSB(float v) { m_Chi2DofCutSSB = v; }
  float getChi2DofCutSSB() const { return m_Chi2DofCutSSB; }

  void setBank(int, int, FTKConstantBank*);

  void setRoadInputModule(FTKRoadInput *module)
    { m_roadinput = module; }
  FTKRoadInput* getRoadInputModule() const
    { return m_roadinput; }

  void setTrackOutputModule(FTKTrackOutput *module)
    { m_trackoutput = module; }
  void setTrackOutputModulePreHW(FTKTrackOutput *module)
    { m_trackoutput_pre_hw = module; }
  FTKTrackOutput* getTrackOutputModule() { return m_trackoutput; }
  FTKTrackOutput* getTrackOutputModulePreHW() { return m_trackoutput_pre_hw; }
  // output for firmware tests
  //void setFirmwareOutputModule(FTKFirmwareOutput *module) { m_fwoutput = module; }
  //void setNSectorsFWO( int v ) { m_maxsectors_fwo = v; }

  virtual void init();


  int nextEvent();

};

#endif // TRACKFITTER_H
