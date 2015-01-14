/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCS_Cell
#define FCS_Cell
#include <vector>
#include <stdint.h>
#include <Rtypes.h>
#include <TLorentzVector.h>
/******************************************
This contains structure definition
All structures are relatively simple
each matched cell remembers - cell properties + vector of g4hits in this cell + vector of FCS hits in this cell

Technicalities - needs a Linkdef.h file + makefile to create the dictionary for ROOT
then the last class could be saved in to the TTree

 ******************************************/

struct FCS_cell
{
  Long64_t cell_identifier;
  int   sampling;
  float energy;
  float center_x;
  float center_y;
  float center_z; //to be updated later                                                                                                              
};

struct FCS_hit //this is the FCS detailed hit
{
  Long64_t identifier; //hit in the same tile cell can have two identifiers (for two PMTs)
  Long64_t cell_identifier;
  int    sampling; //calorimeter layer
  float  hit_energy; //energy is already scaled for the sampling fraction
  float  hit_time;
  float  hit_x;
  float  hit_y;
  float  hit_z;
  //float  hit_sampfrac;
};

struct FCS_g4hit //this is the standard G4Hit
{
  Long64_t identifier;
  Long64_t cell_identifier;
  int    sampling;
  float  hit_energy;
  float  hit_time;
  //float  hit_sampfrac;
};

struct FCS_matchedcell //this is the matched structure for a single cell
{
  FCS_cell cell;
  std::vector<FCS_g4hit> g4hit;
  std::vector<FCS_hit> hit;
  inline void clear() {g4hit.clear(); hit.clear();};
  inline float scalingfactor(){float hitsum =0.; for (unsigned int i=0; i<hit.size(); i++){hitsum+=hit[i].hit_energy;}; return cell.energy/hitsum;}; //doesn't check for 0!
};

struct FCS_matchedcellvector //this is the matched structure for the whole event (or single layer) - vector of FCS_matchedcell 
{
  //Note that struct can have methods
  //Note the overloaded operator(s) to access the underlying vector
  std::vector<FCS_matchedcell> m_vector;
  inline std::vector<FCS_matchedcell> GetLayer(int layer){std::vector<FCS_matchedcell> ret; for (unsigned i=0; i<m_vector.size(); i++) {if (m_vector[i].cell.sampling == layer) ret.push_back(m_vector[i]);}; return ret;};
  inline FCS_matchedcell operator[](unsigned int place) { return m_vector[place];};
  inline unsigned int size() {return m_vector.size();};
  inline void push_back(FCS_matchedcell cell) { m_vector.push_back(cell);};
  inline float scalingfactor(){float cellsum=0.; float hitsum=0.; for (unsigned int i=0; i<m_vector.size(); i++){cellsum+=m_vector[i].cell.energy;for (unsigned int j=0; j<m_vector[i].hit.size(); j++){hitsum+=m_vector[i].hit[j].hit_energy;};}; return cellsum/hitsum;}; //doesn't check for 0!
};

struct FCS_truth : public TLorentzVector 
{
  std::vector<double> TTC_entrance_eta;
  std::vector<double> TTC_entrance_phi;
  std::vector<double> TTC_entrance_r;
  std::vector<double> TTC_entrance_z;
  std::vector<double> TTC_back_eta;
  std::vector<double> TTC_back_phi;
  std::vector<double> TTC_back_r;
  std::vector<double> TTC_back_z;
  double TTC_IDCaloBoundary_eta;
  double TTC_IDCaloBoundary_phi;
  double TTC_IDCaloBoundary_r;
  double TTC_IDCaloBoundary_z;
  double TTC_Angle3D;
  double TTC_AngleEta;
  int barcode;
  int vtxbarcode;
  int pdgid;
  ClassDef (FCS_truth, 1)
};


#ifdef __CINT__
#pragma link C++ struct FCS_cell+;
#pragma link C++ struct FCS_hit+;
#pragma link C++ struct FCS_g4hit+;
#pragma link C++ struct std::vector<FCS_hit>+;
#pragma link C++ struct std::vector<FCS_g4hit>+;
#pragma link C++ struct FCS_matchedcell+;
#pragma link C++ struct FCS_matchedcellvector+;
#pragma link C++ class std::vector<Float_t>+;
#pragma link C++ struct FCS_truth+;
#pragma link C++ struct std::vector<FCS_truth>+;
#endif

#endif

