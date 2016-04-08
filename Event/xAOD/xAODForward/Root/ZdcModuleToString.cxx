/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODForward/ZdcModuleToString.h"
#include <sstream>

std::string ZdcModuleToString(const xAOD::ZdcModule& zm) 
{
  std::stringstream o;
  o << " ID=" << zm.id();
  o << " S/M/T/C=" << zm.side();
  o << "/" << zm.zdcModule(); 
  o << "/" << zm.type();
  o << "/" << zm.channel();
  o << "\n";
  o << "All gain energy/amplitude/time/qual = ";
  o << zm.energy();
  o << "/" << zm.amplitude();
  o << "/" << zm.time();
  o << "/" << zm.qual();
  o << "\n";
  o << "Hi gain energy/amplitude/time/qual = ";
  o << zm.energyG1();
  o << "/" << zm.amplitudeG1();
  o << "/" << zm.timeG1();
  o << "/" << zm.qualG1();
  o << "\n";
  o << "Lo gain energy/amplitude/time/qual = ";
  o << zm.energyG0();
  o << "/" << zm.amplitudeG0();
  o << "/" << zm.timeG0();
  o << "/" << zm.qualG0();
  o << "\n";

  o << "All-gain waveform:\n";
  for (size_t i = 0;i<zm.waveformTime().size();i++)
    {
      o << zm.waveformTime().at(i) << "\t";
    }
  o<<"\n";
  for (size_t i = 0;i<zm.waveformADC().size();i++)
    {
      o << zm.waveformADC().at(i) << "\t";
    }
  o<<"\n";

  o << "Hi-gain waveform:\n";
  for (size_t i = 0;i<zm.waveformTimeG1().size();i++)
    {
      o << zm.waveformTimeG1().at(i) << "\t";
    }
  o<<"\n";
  for (size_t i = 0;i<zm.waveformADCG1().size();i++)
    {
      o << zm.waveformADCG1().at(i) << "\t";
    }
  o<<"\n";

  o << "Lo-gain waveform:\n";
  for (size_t i = 0;i<zm.waveformTimeG0().size();i++)
    {
      o << zm.waveformTimeG0().at(i) << "\t";
    }
  o<<"\n";
  for (size_t i = 0;i<zm.waveformADCG0().size();i++)
    {
      o << zm.waveformADCG0().at(i) << "\t";
    }
  o<<"\n";

  return o.str();
}

std::string ZdcModuleToString(const xAOD::ZdcModuleContainer& zc) 
{
  std::stringstream o;
  for(xAOD::ZdcModuleContainer::const_iterator iter = zc.begin();
      iter != zc.end(); ++iter){
    o << ZdcModuleToString(**iter) << std::endl;
  }
  return o.str();

}
