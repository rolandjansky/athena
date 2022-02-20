/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/LArTemperatureCorrectionTool.h>
#include <TParameter.h>
#include <exception>

template<typename T>
T
get_value_parameter(TFile& f, const std::string& name)
{
  TParameter<T>* p = dynamic_cast<TParameter<T>*>(f.Get(name.c_str()));
  if (not p) {
    return 0;
  }
  return p->GetVal();
}

LArTemperatureCorrectionTool::LArTemperatureCorrectionTool(const std::string& filename)
  : asg::AsgMessaging("LArTemperatureCorrectionTool")
  , m_file(TFile::Open(filename.c_str()))
{
  if (!m_file or m_file->IsZombie()) {
    throw std::runtime_error("LArTemperatureCorrectionTool: Cannot open file");
  }

  m_tree = dynamic_cast<TTree*>(m_file->Get("temperature"));
  if (!m_tree) {
    throw std::runtime_error("LArTemperatureCorrectionTool: Cannot find tree");
  }

  Int_t t_run = 0;
  m_tree->SetBranchAddress("run", &t_run);
  m_tree->GetEntry(0);
  m_first_run = t_run;
  m_tree->GetEntry(m_tree->GetEntries() - 1);
  m_last_run = t_run;

  base_temperature.barrel = get_value_parameter<double>(*m_file, "base_temperature_barrel");
  base_temperature.endcapA = get_value_parameter<double>(*m_file, "base_temperature_endcapA");
  base_temperature.endcapC = get_value_parameter<double>(*m_file, "base_temperature_endcapC");

  sensitivity_temperature.barrel = get_value_parameter<double>(*m_file, "sensitivity_temperature_barrel");
  sensitivity_temperature.endcapA = get_value_parameter<double>(*m_file, "sensitivity_temperature_endcapA");
  sensitivity_temperature.endcapC = get_value_parameter<double>(*m_file, "sensitivity_temperature_endcapC");

  ATH_MSG_INFO("LArTemperatureCorrectionTool initialized for runs " << m_first_run << ".." << m_last_run);
  ATH_MSG_INFO("base temperatures (barrel/endcapA/endcapC) = " << base_temperature.barrel << "/"
                                                               << base_temperature.endcapA << "/"
                                                               << base_temperature.endcapC);
  ATH_MSG_INFO("sensitivity relE/K (barrel/endcapA/endcapC) = " << sensitivity_temperature.barrel << "/"
                                                                << sensitivity_temperature.endcapA << "/"
                                                                << sensitivity_temperature.endcapC);
}

LArTemperatureCorrectionTool::AllValues
LArTemperatureCorrectionTool::search_correction(int run) const
{
  AllValues temp = search_temperature(run);
  temp.barrel = 1. - (temp.barrel - base_temperature.barrel) * sensitivity_temperature.barrel;
  temp.endcapA = 1. - (temp.endcapA - base_temperature.endcapA) * sensitivity_temperature.endcapA;
  temp.endcapC = 1. - (temp.endcapC - base_temperature.endcapC) * sensitivity_temperature.endcapC;
  return temp;
}

LArTemperatureCorrectionTool::AllValues
LArTemperatureCorrectionTool::search_temperature(int run) const
{
  Float_t t_barrel = base_temperature.barrel;
  Float_t t_endcapA = base_temperature.endcapA;
  Float_t t_endcapC = base_temperature.endcapC;
  Int_t t_run = 0;
  m_tree->SetBranchAddress("run", &t_run);
  m_tree->SetBranchAddress("average_temperature_barrel", &t_barrel);
  m_tree->SetBranchAddress("average_temperature_endcapA", &t_endcapA);
  m_tree->SetBranchAddress("average_temperature_endcapC", &t_endcapC);

  // use bisection, tree is ordered by run number
  int low = 0;
  int high = m_tree->GetEntries() - 1;
  int mid = 0;
  while (low <= high) {

    if (high - low < 50) { // prefer sequential scan
      for (int i = low; i <= high; ++i) {
        m_tree->GetEntry(i);
        if (run == t_run) {
          return AllValues{ t_barrel, t_endcapA, t_endcapC };
        }
      }
      break;
    }

    mid = low + (high - low) / 2; // scared of overflow?
    m_tree->GetEntry(mid);
    if (run == t_run) {
      return AllValues{ t_barrel, t_endcapA, t_endcapC };
    } else if (run < t_run) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }

  ATH_MSG_WARNING("run " << run << " not found - no temperature correction");
  return base_temperature;
}

LArTemperatureCorrectionTool::AllValues
LArTemperatureCorrectionTool::get_corrections(int run) const
{
  const auto it = m_cache.find(run);
  if (it != m_cache.end()) {
    return it->second;
  } else {
    AllValues corrections{};
    if (run < m_first_run) {
      ATH_MSG_WARNING("run " << run << " is before the first run - using the first run");
      corrections = search_correction(m_first_run);
    } else if (run > m_last_run) {
      ATH_MSG_WARNING("run " << run << " is after the last run - using the last run");
      corrections = search_correction(m_last_run);
    } else {
      corrections = search_correction(run);
    }
    m_cache[run] = corrections;
    return corrections;
  }
}
