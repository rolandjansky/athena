/*

 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

Objects.h: Contains definitions of objects necessary for MET algorithms

Author: Ava Myers (amyers@cern.ch)

Objects: Object: template four momentum object holding pt, eta, phi, m for definitions of more complex objects

         Block: g(j)Block required for Jets without Jets algorithm. Designed to be 3x3 block around a seed tower

         TowerGrid: Used by Block object. Defines coordinates/indices of towers and neighbors to the seed tower

	 Patch: Tower "patch" required for PUfit algorithm. A simpler implementation of Block/TowerGrid for quick use. Called a tower in PUfit algorithm, but renaming as Patch to avoid confusion
 */

#include "Math/Vector4D.h"
#include "TVector2.h"
#include "math.h"
#include "TMath.h"
#include <vector>
#include <algorithm>
#include "TrigT1CaloFexSim/JGTower.h"

namespace TowerObject{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float>> FourMom_t;
  
  //primarily to make the interface consistent
  class Object: public FourMom_t{
  private:
    float m_area = 0.0;
    
  public:
  Object(): FourMom_t(0.0, 0.0, 0.0, 0.0){};
  Object(float pt, float eta, float phi, float m):
    FourMom_t(pt, eta, phi, m){};
    ~Object(){};
    
    float pt() const {return this->Pt();};
    void pt(float pt) {this->SetPt(pt);};
    float eta() const {return this->Eta();};
    void eta(float eta) {this->SetEta(eta);};
    float phi() const {return this->Phi();};
    void phi(float phi) {this->SetPhi(phi);};
    float m() const {return this->M();};
    void m(float m) {this->SetM(m);};
    float area() const {return this->M();};
    void area(float a) {m_area = a;};
    
    // comparison operators for easy sorting                                                                                    
    friend bool operator <  (const Object &o1, const Object &o2){ return o1.pt() <  o2.pt(); };
    friend bool operator >  (const Object &o1, const Object &o2){ return o1.pt() >  o2.pt(); };
    friend bool operator<= (const Object &o1, const Object &o2){ return o1.pt() <= o2.pt(); };
    friend bool operator>= (const Object &o1, const Object &o2){ return o1.pt() >= o2.pt(); };
    
    float deltaR(const Object& other) const{
      float deta = this->deltaEta(other);
      float dphi = this->deltaPhi(other);
      return sqrt(deta*deta + dphi*dphi);
    };
    
    float deltaEta(const Object& other) const{
      return this->eta() - other.eta();
    };
    float deltaPhi(const Object& other) const{
      return TVector2::Phi_mpi_pi(this->phi() - other.phi());
    };
  
  };

  class Block : public Object{
  private: 
    int m_seedIndex = -1;
    int m_numEta;
    int m_numPhi;
    int m_numConstituents = 0;
    
  public:
    //Block: Object(){};
  Block(float pt, float eta, float phi, float m): Object(pt, eta, phi, m){};
    
    float pt_cal() const {return 2700 + (1./0.8189)*this->Object::pt();}; //values have been previously derived for gFEX calibrations
    //    float pt(bool calib = false) const {return (calibration)?pt_cal():this->Object::pt();}
    int seedIndex() const {return m_seedIndex;};
    void seedIndex(int index) {m_seedIndex = index;};
    int numEta() const {return m_numEta;};
    void numEta(int n) {m_numEta = n;};
    int numPhi() const {return m_numPhi;};
    void numPhi(int n) {m_numPhi = n;};
     int numConstituents() const {return m_numConstituents;};
    void numConstituents(int n) {m_numConstituents = n;};
  };
  
  class TowerGrid{
  public:
    //build the grid
    TowerGrid(const xAOD::JGTowerContainer& towers){
      grid = std::vector<int>(grid_eta*grid_phi, -1);
      for(const xAOD::JGTower* tower: towers){
	int towerIndex = std::find(towers.begin(), towers.end(), tower) - towers.begin();
	this->add_tower(*tower, towerIndex);
      }
    };
    
    void add_tower(const xAOD::JGTower& tower, int towerIndex){
      int gridIndex = get_index(tower);
      if(gridIndex >= 0){
	grid[gridIndex] = towerIndex;
      }
    };
    
    //normalization functions
    std::pair<int, int> normalize_coord(std::pair<int, int> coord) const{
      while(coord.second < 0) coord.second += grid_phi;
      return coord;
    };
    
    int normalize_index(int index) const{
      while(index < 0) index += grid_eta*grid_phi;
      return index;
    };
  
    //get index functions
    int get_index(const xAOD::JGTower& tower) const{
      return get_index(get_coord(tower));
    };
    int get_index(std::pair<int, int> coord) const{
      coord = normalize_coord(coord);
      if(coord.first < 0 || coord.first >= grid_eta) return -1;
      return coord.second*grid_eta + coord.first;
    };
    
    //get coordinate functions
    std::pair<int, int> get_coord(const xAOD::JGTower& tower) const{
      int ieta(-1);
      for(const float& eta: eta_bins){
	if(tower.eta() < eta) break;
	ieta++;
      }
      int iphi(-1);
      for(const float& phi : phi_bins){
	if(tower.phi() < phi) break;
	iphi++;
      }
      return std::pair<int, int>(ieta, iphi);
    };
    
    std::pair<int, int> get_coord(int index) const{
      index = normalize_index(index);
      int iphi = std::floor(index/grid_eta);
      int ieta = index - iphi*grid_eta;
      return std::pair<int, int>(ieta, iphi);
    };
    
    //neighbors functions
    std::vector<int> neighbors(const xAOD::JGTower& tower, int nrows = 3, int ncols = 3) const{
      const std::pair<int, int> coord = get_coord(tower);
      return neighbors(coord, nrows, ncols);
    };
    
    std::vector<int> neighbors(const std::pair<int, int> coord, int nrows, int ncols) const{
      std::vector<int> neighbors;
      if(nrows%2 == 0 || ncols%2 == 0) return neighbors;
      
      for(int irow = -(nrows-1)/2; irow <= (nrows-1)/2; irow++){
	for(int icol = -(ncols-1)/2; icol <= (ncols-1)/2; icol++){
	  //don't include the target tower itself
	  if(irow == 0 && icol == 0) continue;
	  std::pair<int, int> neighbor(coord.first + icol, coord.second + irow);
	  int index = this->at(normalize_coord(neighbor));
	  //only if the tower exists in the grid
	  if(index >= 0){
	    neighbors.push_back(index);
	  }
	}
      }
      return neighbors;
    };
    
    //at functions
    int at(const std::pair<int, int>& coord) const{
      return this->at(get_index(coord));
    };
    int at(int index) const{
      if(index < 0) return -1;
      if(index >= static_cast<int>(grid.size())) return -1;
      return grid.at(index);
    };
    
  private:
    const int grid_eta = 24;
    const int grid_phi = 32;
    std::vector<int> grid;
    const std::vector<float> eta_bins = {
      -2.4000,
      -2.2000,
      -2.0000,
      -1.8000,
      -1.6000,
      -1.4000,
      -1.2000,
      -1.0000,
      -0.8000,
      -0.6000,
      -0.4000,
      -0.2000,
      0.0000,
      0.2000,
      0.4000,
      0.6000,
      0.8000,
      1.0000,
      1.2000,
      1.4000,
      1.6000,
      1.8000,
      2.0000,
      2.2000,
      2.4000
    };
    
    const std::vector<float> phi_bins = {
      -3.1416,
      -2.9452,
      -2.7489,
      -2.5525,
      -2.3562,
      -2.1598,
      -1.9635,
      -1.7671,
      -1.5708,
      -1.3744,
      -1.1781,
      -0.9817,
      -0.7854,
      -0.5890,
      -0.3927,
      -0.1963,
      0.0000,
      0.1963,
      0.3927,
      0.5890,
      0.7854,
      0.9817,
      1.1781,
      1.3744,
      1.5708,
      1.7671,
      1.9635,
      2.1598,
      2.3562,
      2.5525,
      2.7489,
      2.9452,
      3.1416
    };
  }; 
}
