/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKFITTER711_H
#define TRACKFITTER711_H

#include "TrackFitter.h"
#include "FTKSSMap.h"
#include "FTKSector711DB.h"

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>

#include <list>
#include <utility>
#include <vector>

class TrackFitter711 : public TrackFitter {
protected:
  bool m_use_guessing; // true (default) if the MJ algorithm is used for the estrapolation
  int m_ncoords_incomplete; // number of coordinates in the incomplete fits
  int m_nplanes_incomplete; // number of layers used for the incomplete tracks

  unsigned int m_incomplete_coordsmask; // mask with a 0 in the ingored coordinates
  unsigned int m_incomplete_planesmask; // mask with a 0 in the ingored planes

  unsigned int m_extrafits; // if >0 [default]the complete fits are executed

  int m_max_missing_extraplanes; // maximum number of missing layers

  int m_nneighbours; // number of strips to retrieve, 1 just the central
  FTKTrack *combtrackI; // pre-allocated array for MJ-recovery

  bool m_passedExtrapolation; // temp variable to count if AUX track passed extrapolation

  int m_nplanes_ignored; // number of planes to extrapolate the 7L tracks
  int m_ncoords_ignored; // number of coords to extrapolate the 7L tracks
  int *m_idplanes; // array of the index of additional layers
  int *m_idcoords; // ID of the additional coordinates
  int *m_idCImap; // ID of the CI map..?
  int *m_idplanes_or; // origianl ID of the layered paired with the additional
  bool *m_endcap_inversion; // true if endcap invert stereo and axial layer IDs
  int *m_idplanes_eff; // effective coordinates
  int *m_idcoords_eff; // effective coordinates
  int *m_CImap_eff; // effective coordinates
  int *m_CIlayermap_eff; // effective mapping to map the complete in an incomplete track
  unsigned int m_incomplete_coordsmask_eff; // eff coordinates mask
  unsigned int m_incomplete_planesmask_eff; // eff coordinates mask
  unsigned int m_ext_parmask; // mask with the parameter veto
  int *m_idpars; // ID of the parameters used to extrapolate the tracks
  int m_nmiss_PXL; // used in ResolutionMode ONLY. To be used as a hardcoded value
  int m_nmiss_SCT; // used in ResolutionMode ONLY. To be used as a hardcoded value
  int m_n_tot_PXL; // used in ResolutionMode ONLY. To be used as a hardcoded value
  int m_resmode_bitmask; // used in ResolutionMode ONLY. To be used as a hardcoded value
  TH2F** m_histores_hitcoord_PXL;
  TH1F** m_histores_hitcoord_SCT;

  FTKSSMap *m_ssmap_incomplete; // ssmap to decompose original SS information
  FTKPlaneMap *m_pmap_incomplete; // the pmap definition for the incomplete roads

  FTKSSMap *m_ssmap_complete; // ssmap to evaluate the new SS

  FTKConstantBank ***m_constant_incomplete; // constant banks

  bool m_use_SectorDB; // true the fitter use the connection from 7L to 11L

	bool m_use_multiple_conn; // multiple connection
	int m_use_n_conn; // only use first N connections

  FTKSector711DB ***m_FC_relations; // list with sector relations

  bool m_saveIncompleteTracks; // true if you want to save incompelte tracks

  bool m_resolution_mode; // if !=0 the mode used to evaluate resolution on themissing planes is on

  bool m_super_extrapolate; // if !=0 this will try the extrapolation AND retrieve hits to make judgement about which connection to use

  int *m_CImap; // this array, if m_ncoords_incomplete size, has the relation to transform a complete track in an incomplete one
  int *m_CIlayermap; // this array maps the position of the layer for the complete tracks in the incomplete version

  FTKTrack newtrkI; // incomplete track used during the fitting
  unsigned int m_complete_maskI; // set 1 in all the coordinates

  TFile *m_resfile; // output file for resolution studies
  TH1F **m_histores_parsIo; // histogram for parameters resolution or 11L vs or 7L
  TH1F *m_histores_chi2Io; // histogram for parameters resolution or 11L vs or 7L
  TH1F ***m_histores_parsCe; // histogram for parameters resolution or 11L vs extrapolated 11L
  TH1F ***m_histores_parsIe; // histogram for parameters resolution or 7L vs extrapolated 11L
  TH1F ***m_histores_hits_BRL; // histogram for hits resolution, barrel modules
  TH1F ***m_histores_SS_BRL; // histogram with the distance in SS, barrel modules
  TH1F ***m_histores_hits_ECP; // histogram for hits resolution, endcap modules
  TH1F ***m_histores_SS_ECP; // histogram with the distance in SS, endcap modules
  TTree *m_treeres; // tree to keep more refined statistic information
  int m_tv_nmiss; // tree variable with the number of missing variables
  int m_tv_npars; // tree variable with the number of parameters
  int m_tv_ncombs; // tree variable with the number of combinations
  double m_tv_chi2I; // incomplate chi2
  double m_tv_chi2C; // complete chi2
  double *m_tv_parsI; // incomplate chi2
  double *m_tv_parsC; // complete chi2
  double *m_tv_orhits; // original hit positions
  double *m_tv_exhits; // extrapolated hits position
  int *m_tv_orSS; // original SS with the given SSMap
  int *m_tv_exSS; // extrapolate SS with the given SSMap
  int *m_tv_isEC; // store if the module is Barrel (0) or Endcap (1)

  int m_ncombsI; // number of combinations
  int m_nfitsI; // number of fits tryied in a road
  int m_nfits_majI; // number of majority fits
  int m_nfits_majI_pix; // number of majority fits with missing pixel hit
  int m_nfits_majI_SCT; // number of majority fits with missing SCT hit
  int m_nfits_recI; // number of recoverd fits
  int m_nfits_addrecI;
  int m_nfits_badI; // number of fits with a bad chi2
  int m_nfits_rejI; // number of fits rejected by HW
  int m_nfits_badmajI; // number of fits with a bad chi2
  int m_nfits_rejmajI; // number of fits rejected by HW
  int m_nconn; // number of connections
  int m_nextrapolatedTracks; // number of extrapolated tracks

  int m_comb_idI; // current combination id, has to be unique in each event, for each bank

  virtual void processor_init(int);
  virtual void processor(const FTKRoad &);
  virtual void processor_end(int);

  /* void processor_ResolutionMode(const FTKRoad &); */
  /* void processor_ResolutionMode(const FTKRoad &, std::list<FTKTrack> &); */
  void processor_Incomplete(const FTKRoad &, std::list<FTKTrack> &, std::list<FTKTrack> &);
  void processor_Extrapolate(const FTKRoad &, std::list<FTKTrack> &);
  void processor_SuperExtrapolate(const FTKRoad &, std::list<FTKTrack> &);

  virtual void compute_truth(const unsigned int&,
			     const FTKRoad &,FTKTrack&) const;
  virtual void compute_truth(const unsigned int&,
			     const FTKRoad &,FTKTrack&,
			     std::vector<FTKHit> *, FTKTrack&) const;
  virtual void compute_truth_incomplete(const unsigned int&,
			     const FTKRoad &,FTKTrack&) const;
public:
  TrackFitter711();
  void setUseGuessing(bool flag) { m_use_guessing = flag; }
  bool getUseGuessing() { return m_use_guessing; }

  virtual void setNCoordsPlanes(int,int, int, int);
  virtual void setNCoordsPlanes(int,int);

  int getNCoordsComplete() const { return m_ncoords; }
  int getNCoordsIncomplete() const { return m_ncoords_incomplete; }

  void setSaveIncompleteTracks(bool flag) { m_saveIncompleteTracks = flag; }
  bool getSaveIncompeteTracks() const { return m_saveIncompleteTracks; }

  void setResolutionMode(bool flag) { m_resolution_mode = flag; }
  bool getResolutionMode() const { return m_resolution_mode; }

  void setUseSectorDB(bool flag) { m_use_SectorDB = flag; }
  bool getUseSectorDB() const { return m_use_SectorDB; }

  void setUseMultipleConn(bool flag) { m_use_multiple_conn = flag; }
  bool getUseMultipleConn() const { return m_use_multiple_conn; }

  void setUseNConn(int nconn) { m_use_n_conn = nconn; }
  bool getUseNConn() const { return m_use_n_conn; }

  void setSSMapIncomplete(FTKSSMap *map) { m_ssmap_incomplete = map; }
  void setSSMapComplete(FTKSSMap *map) { m_ssmap_complete = map; }

  void setPlaneMapIncomplete(FTKPlaneMap *map) { m_pmap_incomplete = map; }

  void setExtraFits(int lvl) { m_extrafits = lvl; }
  int getExtraFits() const { return m_extrafits; }

  void setMaxMissingExtraPlanes(int max) { m_max_missing_extraplanes = max; }
  const int& getMaxMissingExtraPlanes() const { return m_max_missing_extraplanes; }

  void setNNeighbours(int val) { m_nneighbours = val; }
  int getNNeighbours() const { return m_nneighbours; }

  void setSuperExtrapolateMode(bool mode) { m_super_extrapolate = mode; }
  bool getSuperExtrapolateMode() { return m_super_extrapolate; }

  virtual ~TrackFitter711();

  virtual void init();

  virtual void prepareBanks(int, int); // set number r regions and sub-regions

  void setBank(int, int, FTKConstantBank*, FTKConstantBank*, FTKSector711DB*);

  int* getCIMap() { return m_CImap; }

  void printBinary(int number, int total);

  // -------------------------------------------------------------------------
  // Super extrapolation

public:
  enum CheckTRBy{
    kModuleID = 0,
    kEta = 1
  };

private:
  int m_debug_super_extrapolate;
  int *m_section_of_exp_layers;
  int m_baseHWflag;
  int m_region_for_superexp;
  int m_subreg_for_superexp;
  int m_sector_for_superexp;
  int *m_ssid;
  int **m_bounds;
  int cur_iconn;
  unsigned int m_cbitmask;
  unsigned int m_cbitmask_real;
  unsigned int m_norecovery_mask;// mask to prevent majority in planes with multiple hits or previous hits
  std::vector<FTKHit> *m_hits_more;
  std::vector<FTKTrack> completeTrackCandidates;    // vector used as buffer to temporary store the found tracks
  std::vector<std::pair<int,int> > connectedSectorPairs; // vector used to retrieve the correspondent simlar sector
  std::vector<int> nmissingOfCompleteTracks;        // vector used to store the number of missing points int the extension
  int m_check_TR_by;
  float m_transition_region_max_eta;
  float m_transition_region_min_eta;
  bool m_super_exp_allow_extra_miss;

  std::vector<int> conn_subregs;
  std::vector<int> conn_sectors;
  std::vector<FTKConstantBank *> conn_gcbanks;

  bool isDebugSuperExp() { return m_debug_super_extrapolate; }
  void initializeSuperExtrapolate(const FTKRoad &road);
  bool isConnFileOkay();
  void prepareCompleteTrack(const FTKRoad &road);
  void setLayerInversions(const FTKRoad &road);
  void printLayerInversionResult();
  void extrapolateIncompleteTrack(const FTKRoad &road);
  void prepareTrack();
  void gatherConnections();
  void printConnections();
  void performExtrapolation();
  void printCurCompleteTrack();
  void guessedHitsToSSID();
  void printRetrievedSSIDs();
  void obtainHitsFromSSIDs();
  void printFoundHits();
  void fitCompleteTracks(const FTKRoad &road);
  void saveCompleteTracks();
  void printEveryHitsInExtraLayers();
  bool isIBLhitsFound();
  bool passedNMissLayerRequirement(int nmiss);
  bool isTransitionRegion();
  bool isTransitionRegion(float eta);
  bool isTransitionRegion(int moduleid);
  void setIConn(int iconn) { cur_iconn = iconn; }
public:
  void setTransitionRegion(float min, float max) { m_transition_region_min_eta = min; m_transition_region_max_eta = max; }
  void setTransitionRegionMinEta(float min) { m_transition_region_min_eta = min; }
  void setTransitionRegionMaxEta(float max) { m_transition_region_max_eta = max; }
  void setAllowExtraMissInSuperExp(bool mode) { m_super_exp_allow_extra_miss = mode; }
  void setCheckTRby(int mode) { m_check_TR_by = mode; }
  void setDebugSuperExp(bool debug) { m_debug_super_extrapolate = debug; }
  void endSuperExtrapolate();

  // -------------------------------------------------------------------------
  // Diagnostic tools

private:
  bool m_diagnostic_mode;
  TFile* m_diagnostic_plots_tfile;
  TH1F** m_histo_nhitsfound_per_track_each_layer;
  TH2F** m_histo_nhitsfound_per_track_each_layer_each_conn;
  TH1F** m_histo_ncomb_per_track_each_conn;
  TH1F** m_histo_ncomb_each_conn;
  TH1F* m_histo_nfits_per_track;
  TH2F* m_histo_ntrackI_ntrack;
  TH1F* m_histo_nconn_per_track;
  TH1F* m_histo_bitmask;
  TH1F* m_histo_module_value_diff;
  int* m_ncomb_event_conn;
  TH2F* m_IBL_res;
  double IBLcoordPhi;
  double IBLcoordEta;
  TH2F* m_histo_delta_guessed_hit_found_hit;
  int ibl_module_with_hit;

  void fillDiagnosticPlotsPerTrack();
  void fillDiagnosticPlotsPerTrackPerConn();
  void fillDiagnosticPlotsPerEvent();
  void fillDiagnosticPlotsPerAcceptedTrack();
  void createDiagnosticVariables();
  void initializeDiagnosticVariables();
  void deleteDiagnosticVariables();
  bool isRealRegion();

public:
  void setDiagnosticMode(bool mode) { m_diagnostic_mode = mode; }
  bool getDiagnosticMode() { return m_diagnostic_mode; }
  void prepareDiagonsticPlots();
  void saveDiagnosticPlots();


};
#endif // TRACKFITTER711_H
