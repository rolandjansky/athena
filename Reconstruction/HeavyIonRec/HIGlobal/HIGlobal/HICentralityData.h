/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HiCentralityData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HICENTRALITYDATA_H
#define HICENTRALITYDATA_H

#include <string>
#include <vector>
#include <map>

// using namespace std;

/** @class HICentralityData

    Stores heavy ion event centrality bin calculated from several quantities 
    stored in a 100x1% bin schema with possible interpretation in a coarser
    percentage schema saved also as a member of the object
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/

class Identifier;
class HICentralityData 
{
public:
  
  friend class HICentrality;

  HICentralityData(char *name);
  HICentralityData(std::vector<unsigned short> centralityPercSchema);
  HICentralityData() : m_name(0) {};
  ~HICentralityData() {};

  //Functions getting centrality bin in raw 100x1% equal bin schema
  //=====================================================================
  unsigned short GetImpactParameterBin() { 
    return m_CentralityBin["b"]; } 
  unsigned short GetNwoundedBin() { 
    return m_CentralityBin["nw"]; } 
  unsigned short GetNcollBin() { 
    return m_CentralityBin["ncoll"]; } 
  unsigned short GetCaloCellEnergyBin() { 
    return m_CentralityBin["calocell_energy"]; } 
  unsigned short GetNumberOfSiClustersBin() { 
    return m_CentralityBin["number_of_siclusters"]; }

  float GetImpactParameterVar() { 
    return m_CentralityVar["b"]; } 
  float GetNwoundedVar() { 
    return m_CentralityVar["nw"]; } 
  float GetNcollVar() { 
    return m_CentralityVar["ncoll"]; } 
  float GetCaloCellEnergyVar() { 
    return m_CentralityVar["calocell_energy"]; } 
  float GetNumberOfSiClustersVar() { 
    return m_CentralityVar["number_of_siclusters"]; }
  
  //Functions getting centrality bin in redefined percentage schema
  //If not set differently (SetPerc) standard is 10 equal bin schema
  //=====================================================================
  unsigned short GetImpactParameterBinBySchema() const;
  unsigned short GetNwoundedBinBySchema() const;
  unsigned short GetNcollBinBySchema() const;
  unsigned short GetCaloCellEnergyBinBySchema() const;
  unsigned short GetNumberOfSiClustersBinBySchema() const;

  void Print() const;
  
  //Get/Set functions for manipulating percentage schema
  void SetPerc(std::vector<unsigned short> perc) { m_CentralityPercSchema = perc; }
  std::vector<unsigned short> GetPerc() { return m_CentralityPercSchema; }
  
protected:
  
  void SetBin(char *qphys, unsigned short bin) 
                                      { m_CentralityBin[std::string(qphys)] = bin; }
  void SetVar(char *qphys, float value) 
                                      { m_CentralityVar[std::string(qphys)] = value; }
  unsigned short  GetBin(char *qphys) { return m_CentralityBin[std::string(qphys)]; }
  unsigned short  GetBinBySchema(const char *qphys) const; 
  
private:

  char *m_name;
  std::vector<unsigned short> m_CentralityPercSchema;
  std::map<std::string, unsigned short> m_CentralityBin;
  std::map<std::string, float> m_CentralityVar;
  
}; 


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(HICentralityData, 154564108, 1)

#endif 
