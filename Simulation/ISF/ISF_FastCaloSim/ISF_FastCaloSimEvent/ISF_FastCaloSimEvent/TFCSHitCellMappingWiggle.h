/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMappingWiggle_h
#define TFCSHitCellMappingWiggle_h

#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"

#ifdef USE_GPU
#include "ISF_FastCaloGpu/LoadGpuFuncHist.h"
#include "ISF_FastCaloGpu/FH_structs.h"
#endif


class TFCS1DFunction;
class TH1;

class TFCSHitCellMappingWiggle:public TFCSHitCellMapping {
public:
  TFCSHitCellMappingWiggle(const char* name=nullptr, const char* title=nullptr, ICaloGeometry* geo=nullptr);
  ~TFCSHitCellMappingWiggle();
  
  void initialize(TFCS1DFunction* func);
  void initialize(const std::vector< const TFCS1DFunction* >& functions, const std::vector< float >& bin_low_edges);

  void initialize(TH1* histogram,float xscale=1);
  void initialize(const std::vector< const TH1* > histograms, std::vector< float > bin_low_edges, float xscale=1);

  inline unsigned int get_number_of_bins() const {return m_functions.size();};
  
  inline double get_bin_low_edge(int bin) const {return m_bin_low_edge[bin];};
  inline double get_bin_up_edge(int bin) const {return m_bin_low_edge[bin+1];};
  
  inline const TFCS1DFunction* get_function(int bin) const {return m_functions[bin];};
  const std::vector< const TFCS1DFunction* > get_functions() {return m_functions;};
  const std::vector< float > get_bin_low_edges() {return m_bin_low_edge;};

  /// modify one hit position to emulate the LAr accordeon shape
  /// and then fills all hits into calorimeter cells
  virtual FCSReturnCode simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;

  virtual bool operator==(const TFCSParametrizationBase& ref) const override;

  void Print(Option_t *option="") const override;

  static void unit_test(TFCSSimulationState* simulstate=nullptr,TFCSTruthState* truth=nullptr, TFCSExtrapolationState* extrapol=nullptr);

#ifdef USE_GPU
  //construct the hist function and copy to GPU
  //will not compile by default
  void    set_d_HistFuncs( FHs* hf_ptr ) { m_d_HistFuncs = hf_ptr; };
  const FHs*       d_HistFuncs() { return m_d_HistFuncs; };
  void             LoadHistFuncs();
  LoadGpuFuncHist* LdFH() { return m_LdFH; };
#endif

protected:  
  bool compare(const TFCSParametrizationBase& ref) const;

private:

#ifdef USE_GPU
  //hist functions in GPU
  FHs*             m_d_HistFuncs = nullptr;
  LoadGpuFuncHist* m_LdFH        = nullptr;
#endif

  //** Function for the hit-to-cell assignment accordion structure fix (wiggle)  **//
  //** To be moved to the conditions database at some point **//
  std::vector< const TFCS1DFunction* > m_functions = {nullptr};
  std::vector< float > m_bin_low_edge = {0,static_cast<float>(init_eta_max)};

  ClassDefOverride(TFCSHitCellMappingWiggle,1)  //TFCSHitCellMappingWiggle
};

#endif
