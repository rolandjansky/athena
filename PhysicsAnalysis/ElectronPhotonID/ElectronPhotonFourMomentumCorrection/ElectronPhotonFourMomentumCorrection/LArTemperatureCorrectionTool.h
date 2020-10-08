/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTEMPERATURECORRECTIONTOOL_H
#define LARTEMPERATURECORRECTIONTOOL_H

/** Correction on the energy for temperature fluctuations
* authors: Ruggero Turra 2015
*
* Compute the relative correction to the energy due to temperature
* fluctuations for barrel / endcap A / C, where endcap = EMEC + HEC.
* This is not a CP tool, and it should not be used alone.
* The correction K is run-dependent and should be applied as E -> E (1 + K)
* on the raw energy (for example on the layer energies). The correction is
* relative to a reference temperature, different for every partition.
*
* If a run is in the future (with respect to the runs known by the tool) the
* correction is based on the temperature of the last run. Similarly if it is
* in the past (but it should not happen).
*
* Temperature values are from oracle, and averaged for every run.
**/

#include <string>
#include <map>
#include <memory>

#include <AsgMessaging/AsgMessaging.h>

#include <TFile.h>
#include <TTree.h>

class LArTemperatureCorrectionTool : public asg::AsgMessaging
{
public:
  struct AllValues { double barrel; double endcapA; double endcapC; };
  LArTemperatureCorrectionTool(const std::string& filename);
  /** correction should be applied on MC as a multiplication: E = E * correction **/
  AllValues get_corrections(int run) const;
  double get_correction_barrel(int run) const { return get_corrections(run).barrel; }
  double get_correction_endcapA(int run) const { return get_corrections(run).endcapA; }
  double get_correction_endcapC(int run) const { return get_corrections(run).endcapC; }
  int get_cache_size() const { return m_cache.size(); }
  int get_first_run() const { return m_first_run; }
  int get_last_run() const { return m_last_run; }
  AllValues base_temperature;
  AllValues sensitivity_temperature;
private:
  std::unique_ptr<TFile> m_file;
  TTree* m_tree;  // raw pointer, let ROOT manage it
  AllValues search_temperature(int run) const;
  AllValues search_correction(int run) const;
  mutable std::map<int, AllValues> m_cache;
  int m_first_run, m_last_run;
};

#endif
