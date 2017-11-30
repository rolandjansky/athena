/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "LArG4ShowerLib/FCALDistEnergyShowerLib.h"

#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>

#include <sstream>
#include <fstream>

#include <iostream>
#include <iomanip>

// G4 includes
#include "G4Track.hh"

#include "LArG4Code/EnergySpot.h"
#include "LArG4ShowerLib/ShowerEnergySpot.h"

#include "TTree.h"
#include "TFile.h"
#include "TParameter.h"

#define LIB_VERSION 4

namespace ShowerLib {

  inline int calcKey(float dist) {
	  return (int)(dist*1000);
  }

  float FCALDistEnergyShowerLib::distance (double x, double y) const
  {
	  double stepy = m_step * sqrt(3.)/2.;
	  double dy =  fmod(y-m_yrodcent,stepy);
	  int ny = int( (y-m_yrodcent)/stepy );
	  //additional half shift:
	  double dx0 = (ny % 2 )*m_step/2.;
	  double dx = fmod(x-m_xrodcent-dx0,m_step);
	  double d1 = sqrt(dx*dx+dy*dy);
	  double d2 = sqrt((m_step-dx)*(m_step-dx)+dy*dy);
	  double d3 = sqrt((m_step/2.-dx)*(m_step/2.-dx)+(stepy-dy)*(stepy-dy));
	  float dd = d1;
	  if (dd > d2) dd = d2;
	  if (dd > d3) dd = d3;
	  return dd;
  }

  IShowerLib* FCALDistEnergyShowerLib::readFromROOTFile(TFile* source)
  {
      TParameter<int>* ver;
      ver = (TParameter<int>*)source->Get("version");

      if ((ver == NULL) || (ver->GetVal() != LIB_VERSION)) return NULL;

      TTree* TTreeMeta = (TTree*)source->Get("meta");
      TTree* TTreeLib  = (TTree*)source->Get("library");

      if ((TTreeMeta == NULL) || (TTreeLib == NULL)) return NULL;

      std::cout << "FCALDistEnergyShowerLib header found." << std::endl;

	  FCALDistEnergyShowerLib* newlib = new FCALDistEnergyShowerLib();

	  if (!(newlib->readMeta(TTreeMeta)) || !(newlib->read(TTreeLib))) {
		  delete newlib;
	      std::cout << "FCALDistEnergyShowerLib read unsuccessful." << std::endl;
		  return NULL;
	  }

	  return newlib;

  }

  IShowerLib* FCALDistEnergyShowerLib::createEmptyLib(const std::string& inputFile)
  {
	  /*
	   * Eta Energy Structure format:
	   *
	   * VER PART DET DIST1 DIST2 ...
	   * XRODCENT YRODCENT STEP
	   * COMMENT
	   *
	   * where
	   *
	   * VER == 4
	   * DIST(N+1) > DIST(N)
	   * DIST(N) >= 0
	   * First DIST is the beginning of the lib. If not zero, zero will be added
	   * Last DIST is the end of the lib. If less then 4.5, 4.5 will be added
	   */
	  std::ifstream filestr(inputFile.c_str(),std::ios::in);


	  if (!filestr.is_open()) {
		  std::cout << "FCALDistEnergyShowerLib    " << inputFile << ": bad file!" << std::endl;
		  return NULL;
	  }

	  std::string instr;
	  std::getline(filestr,instr);
	  std::stringstream ss(instr);

	  int ver;

	  ss >> ver;

	  if (ver != LIB_VERSION) {
		  return NULL;
	  }


	  int part;
	  std::string det;

	  ss >> part >> det;

	  std::vector<float> etalist;

	  float eta;
	  float etaold;

	  ss >> eta;

	  if (ss.fail()) {
		  std::cout << "file reading failed! (not enough data)" << std::endl;
		  return NULL;
	  }

	  if (eta != 0.) {
		  if (eta < 0) {
			  std::cout << "no negative dists allowed (" << eta << ")" << std::endl;
			  return NULL;
		  }
		  etalist.push_back(0.);
	  }

	  etalist.push_back(eta);
	  etaold = eta;

	  while (!ss.eof()) {
		  ss >> eta;
		  if ((ss.fail()) || (eta <= etaold)) {
			  std::cout << "screwed dists! (" << eta << "<=" << etaold << ")" << std::endl;
			  return NULL;
		  }
		  if (eta < 4.5) {
			  etalist.push_back(eta);
		  } else if (eta > 4.5) {
			  std::cout << "dist can't be bigger than 4.5 (" << eta << "). ignored" << std::endl;
		  }
		  etaold = eta;
	  }

	  //if (etalist.back() < 4.5) etalist.push_back(4.5);

	  FCALDistEnergyShowerLib* newlib = new FCALDistEnergyShowerLib();
	  if (!newlib->readStructure(etalist)) {
		  std::cout << "this structure is not valid" << std::endl;
		  delete newlib;
		  return NULL;
	  }

	  newlib->m_detector = det;
	  newlib->m_particle = part;

	  newlib->m_filled = false;

	  std::getline(filestr,instr);
          std::stringstream ss1(instr);
	  
	  ss1 >> (newlib->m_xrodcent) >> (newlib->m_yrodcent) >> (newlib->m_step);

	  std::getline(filestr,instr);
	  newlib->m_comment = instr;

	  return newlib;
  }

  std::vector<EnergySpot>* FCALDistEnergyShowerLib::getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return NULL;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return NULL;
	  }

	  //std::cout << "Extracted particle parameters: " << eta << std::endl;

	  library::const_iterator libit = m_libData.upper_bound(dist);

	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --libit;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*libit).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return NULL;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*libit).second.lower_bound(trenergy);
	  if (distit == (*libit).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  distbin::const_iterator distitch = distit;
		  --distitch;
		  if (((*distit).first - trenergy) > (trenergy - (*distitch).first)) { // the closest is the previous
			  --distit;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*distit).first << std::endl;
	  if (randomShift > 0) {
		  double upperEnergy = (*distit).first * 1.01; //we allow 1% off
		  for (int i = 0; i < randomShift; i++) {
			  distit++;
			  if (distit == (*libit).second.end()) {
				  distit--;
				  break;
			  }
			  if ((*distit).first > upperEnergy) break; //energy diff too high
		  }
	  }
	  if ((randomShift < 0)&&(distit != (*libit).second.begin())) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i > randomShift; i--) {
			  distit--;
			  if (distit == (*libit).second.begin()) {
				  //distit++;
				  break;
			  }
			  if (lowerEnergy > (*distit).first) break; //energy diff too high
		  }
	  }
	  //std::cout << "Found out energy" << std::endl;
	  //std::cout << "Shower with num hits:" << (*etait).second.size() << std::endl;
	  std::vector<EnergySpot>* outshower = new std::vector<EnergySpot>();//((*etait).second);
	  Shower::const_iterator iter;
	  //std::cout << "Created out shower" << std::endl;

	  float energyScale = track->GetKineticEnergy() / (*distit).first;
	  //std::cout << "Scale: " << energyScale << std::endl;

	  for (iter = (*distit).second.begin() /*outshower->begin()*/; iter != (*distit).second.end() /*outshower->end()*/; iter++) {
		  EnergySpot tmp( (*iter)->GetPosition(), (*iter)->GetEnergy(), (*iter)->GetTime() );
		  tmp.SetEnergy(tmp.GetEnergy() * energyScale);
		  outshower->push_back(tmp);
		  //(*iter).SetEnergy((*iter).GetEnergy() * energyScale);
	  }
	  //std::cout << "Scaled" << std::endl;
	  if (stats != NULL) {
		  stats->recordShowerLibUse(calcKey((*libit).first));
	  }
	  //std::cout << "Done" << std::endl;
	  return outshower;
  }

  double FCALDistEnergyShowerLib::getContainmentZ(const G4Track* track) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return 0;
	  }

	  library::const_iterator libit = m_libData.upper_bound(dist);

	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --libit;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*libit).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*libit).second.lower_bound(trenergy);
	  if (distit == (*libit).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  distbin::const_iterator distitch = distit;
		  --distitch;
		  if (((*distit).first - trenergy) > (trenergy - (*distitch).first)) { // the closest is the previous
			  --distit;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*etait).first << std::endl;
	  double rezZ = (*distit).second.getZSize();
	  distbin::const_iterator distiter = distit;
	  int actualNumFS = 1;
	  int spread = 2; //will calculate average Z for 5 showers max ( -2 .. 0 .. +2 )
	  double upperEnergy = (*distit).first * 1.01; //we allow 1% off
	  for (int i = 0; i < spread; i++) {
		  distiter++;
		  if (distiter == (*libit).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*distiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezZ += (*distiter).second.getZSize();
		  actualNumFS++;
	  }
	  distiter = distit;
	  if (distiter != (*libit).second.begin()) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  distiter--;
			  if (lowerEnergy > (*distiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezZ += (*distiter).second.getZSize();
			  actualNumFS++;
			  if (distiter == (*libit).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezZ/actualNumFS; //average Z size
  }

  double FCALDistEnergyShowerLib::getContainmentR(const G4Track* track) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return 0;
	  }

	  library::const_iterator libit = m_libData.upper_bound(dist);

	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --libit;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*libit).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*libit).second.lower_bound(trenergy);
	  if (distit == (*libit).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  distbin::const_iterator distitch = distit;
		  --distitch;
		  if (((*distit).first - trenergy) > (trenergy - (*distitch).first)) { // the closest is the previous
			  --distit;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*etait).first << std::endl;
	  double rezR = (*distit).second.getRSize();
	  distbin::const_iterator distiter = distit;
	  int actualNumFS = 1;
	  int spread = 2; //will calculate average Z for 5 showers max ( -2 .. 0 .. +2 )
	  double upperEnergy = (*distit).first * 1.01; //we allow 1% off
	  for (int i = 0; i < spread; i++) {
		  distiter++;
		  if (distiter == (*libit).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*distiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezR += (*distiter).second.getRSize();
		  actualNumFS++;
	  }
	  distiter = distit;
	  if (distiter != (*libit).second.begin()) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  distiter--;
			  if (lowerEnergy > (*distiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezR += (*distiter).second.getRSize();
			  actualNumFS++;
			  if (distiter == (*libit).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezR/actualNumFS; //average Z size
  }

  bool FCALDistEnergyShowerLib::storeShower(const HepMC::GenParticle* genParticle, const Shower* shower)
  {
	  if (m_filled) {
		  std::cout << "ERROR: filled" << std::endl;
		  return false;
	  }

	  double x = genParticle->production_vertex()->position().x();
	  double y = genParticle->production_vertex()->position().y();

	  float dist = this->distance(x,y);

	  if ( genParticle->pdg_id() != m_particle ) {
		  std::cout << "ERROR: wrong pdgcode: " << m_particle << " != " << genParticle->pdg_id() << std::endl;
		  return false;
	  }

	  library::iterator libit = m_libData.upper_bound(dist);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --libit;
	  }
	  (*libit).second.insert(distbin::value_type(genParticle->momentum().e(),(*shower)));
	  return true;
  }

  bool FCALDistEnergyShowerLib::writeToROOT(TFile* dest)
  {
	  if (m_libData.empty()) return false;
	  TParameter<int> ver("version",LIB_VERSION);

      dest->WriteObject(&ver,"version");

      TTree TTreeMeta;
      TTree TTreeLib;

      write(&TTreeLib);
      writeMeta(&TTreeMeta);

      dest->WriteObject(&TTreeLib,"library");
      dest->WriteObject(&TTreeMeta,"meta");

	  return true;
  }


  bool FCALDistEnergyShowerLib::read(TTree* source)
  {
	  /*
	   * Dist Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |  xrod cent   |  yrod cent   |step (roddist)|   not    |  not   |  - library header
	   * | (parameter)  | (parameter)  | (parameter)  |   used   |  used  |
	   * ------------------------------------------------------------------
	   * |num of showers| min dist for |     not      |   not    |  not   |  - dist bin header
	   * | in dist bin  | cur dist bin |     used     |   used   |  used  |
	   * ------------------------------------------------------------------
	   * | num of hits  |shower r-size |shower z-size |  truth   |  not   |  - shower header
	   * |  in shower   |for cont.check|for cont.check|  energy  |  used  |
	   * ------------------------------------------------------------------
	   * |x-coord of hit|y-coord of hit|z-coord of hit|dep.energy|hit time|  - hit
	   */
	  int nentr = source->GetEntriesFast();
	  if (nentr < 3) return false;
	  Float_t x,y,z,e,time;
	  source->SetBranchAddress("x",&x);
	  source->SetBranchAddress("y",&y);
	  source->SetBranchAddress("z",&z);
	  source->SetBranchAddress("e",&e);
	  source->SetBranchAddress("time",&time);
	  int entr = 0;

          source->GetEntry(entr++);
	  m_xrodcent = x;
	  m_yrodcent = y;
	  m_step = z;

	  do {
		  //read eta bin header
		  source->GetEntry(entr++); //x - nshowers, y - min dist in the current dist bin
		  int nsh = (int)(x+0.1); // +0.1 just in case - c++ has low round
		  float curDist = y;
		  distbin * curbin = &(m_libData[curDist]); //creating a new dist bin
		  for(int i = 0; i < nsh; i++) {
			  //read shower header
			  source->GetEntry(entr++); //x - nhits, y - r size, z - z size, e - gen energy
			  int nhits = (int)(x+0.1);
			  float curEnergy = e;
			  Shower * shower = &((*curbin)[curEnergy]);
			  shower->setRSize(y);
			  shower->setZSize(z);
			  for(int j = 0; j < nhits; j++) {
				  source->GetEntry(entr++); //variables mean what the name suggests
				  shower->push_back(new ShowerEnergySpot(G4ThreeVector(x,y,z),e,time));
			  }
		  }
	  } while (entr < nentr);

	  if (entr != nentr) {
	      return false;
	  }

	  m_filled = true;
	  return true;
  }

  bool FCALDistEnergyShowerLib::write(TTree* dest) const
  {
	  /*
	   * Dist Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |  xrod cent   |  yrod cent   |step (roddist)|   not    |  not   |  - library header
	   * | (parameter)  | (parameter)  | (parameter)  |   used   |  used  |
	   * ------------------------------------------------------------------
	   * |num of showers| min dist for |     not      |   not    |  not   |  - dist bin header
	   * | in dist bin  | cur dist bin |     used     |   used   |  used  |
	   * ------------------------------------------------------------------
	   * | num of hits  |shower r-size |shower z-size |  truth   |  not   |  - shower header
	   * |  in shower   |for cont.check|for cont.check|  energy  |  used  |
	   * ------------------------------------------------------------------
	   * |x-coord of hit|y-coord of hit|z-coord of hit|dep.energy|hit time|  - hit
	   */
	  Float_t x,y,z,e,time;
	  dest->Branch("x",&x);
	  dest->Branch("y",&y);
	  dest->Branch("z",&z);
	  dest->Branch("e",&e);
	  dest->Branch("time",&time);

	  x = m_xrodcent;
	  y = m_yrodcent;
	  z = m_step;
	  e = 0;
	  time = 0;
	  dest->Fill();
	  
	  library::const_iterator libit;
	  for (libit = m_libData.begin(); libit != m_libData.end(); libit ++) {
		  x = (*libit).second.size();
		  y = (*libit).first;
		  z = 0;
		  e = 0;
		  time = 0;
		  dest->Fill(); //eta bin header
		  distbin::const_iterator distit;
		  for (distit = (*libit).second.begin(); distit != (*libit).second.end(); distit++) {
			  x = (*distit).second.size();
			  y = (*distit).second.getRSize();
			  z = (*distit).second.getZSize();
			  e = (*distit).first;
			  time = 0;
			  dest->Fill(); //shower header
			  Shower::const_iterator iter;
			  for (iter = (*distit).second.begin(); iter != (*distit).second.end(); iter++) {
				  x = (*iter)->GetPosition().x();
				  y = (*iter)->GetPosition().y();
				  z = (*iter)->GetPosition().z();
				  e = (*iter)->GetEnergy();
				  time = (*iter)->GetTime();
				  dest->Fill();
			  }
		  }
	  }
	  //dest->Write();
	  return true;
  }

  const std::string FCALDistEnergyShowerLib::printParameters() const
  {
	  std::stringstream ss;
	  ss << std::fixed << std::setprecision(3);
	  ss << "Distance calculator parameters: xrod_cent=" << m_xrodcent << " yrod_cent=" << m_yrodcent << " step=" << m_step;
	  return ss.str();
  }

  bool FCALDistEnergyShowerLib::readStructure(std::vector<float>& structure)
  {
	  std::vector<float>::const_iterator iter;

	  for (iter = structure.begin(); iter != structure.end(); iter++) {
		  m_libData[(*iter)];
	  }

	  return true;
  }

  ShowerLibStatistics* FCALDistEnergyShowerLib::createStatistics() const
  {
	  std::map<int, std::string> names;
	  std::map<int, int> sizes;
	  for(library::const_iterator it = m_libData.begin(); it != m_libData.end(); ++it) {
	    sizes[calcKey(it->first)]=it->second.size();
	    float distlow = it->first;
	    float disthigh;
	    library::const_iterator it_copy = it;
	    ++it_copy;
	    if (it_copy == m_libData.end()) {
	      disthigh = 4.5;
	    } else {
	      disthigh = it_copy->first;
	    }
	    std::stringstream ss;
	    ss << std::showpos << std::fixed << std::setprecision(2);
	    ss << "DIST: " << distlow << " .. " << disthigh;
	    names[calcKey(it->first)] = ss.str();
	  }
	  return new ShowerLibStatistics(names, sizes);
  }

} // namespace ShowerLib
