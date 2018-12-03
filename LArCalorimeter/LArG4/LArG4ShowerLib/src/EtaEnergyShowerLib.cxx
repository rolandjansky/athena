/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "LArG4ShowerLib/EtaEnergyShowerLib.h"

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

#define LIB_VERSION 1

namespace ShowerLib {

  inline int calcKey(float eta) {
	  return (int)(eta*1000);
  }

  IShowerLib* EtaEnergyShowerLib::readFromROOTFile(TFile* source)
  {
      TParameter<int>* ver;
      ver = (TParameter<int>*)source->Get("version");

      if ((ver == NULL) || (ver->GetVal() != LIB_VERSION)) return NULL; //Eta Energy library header = 1

      TTree* TTreeMeta = (TTree*)source->Get("meta");
      TTree* TTreeLib  = (TTree*)source->Get("library");

      if ((TTreeMeta == NULL) || (TTreeLib == NULL)) return NULL;

      std::cout << "EtaEnergyShowerLib header found." << std::endl;

	  EtaEnergyShowerLib* newlib = new EtaEnergyShowerLib();

	  if (!(newlib->readMeta(TTreeMeta)) || !(newlib->read(TTreeLib))) {
		  delete newlib;
	      std::cout << "EtaEnergyShowerLib read unsuccessful." << std::endl;
		  return NULL;
	  }

	  return newlib;

  }

  IShowerLib* EtaEnergyShowerLib::createEmptyLib(const std::string& inputFile)
  {
	  /*
	   * Eta Energy Structure format:
	   *
	   * VER PART DET ETA1 ETA2 ...
	   * COMMENT
	   *
	   * where
	   *
	   * VER == 1
	   * ETA(N+1) > ETA(N)
	   * First ETA is the beginning of the lib
	   * Last ETA is the end of the lib
	   */
	  std::ifstream filestr(inputFile.c_str(),std::ios::in);


	  if (!filestr.is_open()) {
		  std::cout << "EtaEnergyShowerLib    " << inputFile << ": bad file!" << std::endl;
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
	  float etaold = -9999999.9; //hopefully will be enough

	  ss >> eta;

	  if (ss.fail()) {
		  std::cout << "file reading failed! (not enough data)" << std::endl;
		  return NULL;
	  }

	  float mineta = eta;
	  float maxeta = eta;

	  while (!ss.eof()) {
		  if ((ss.fail()) || (eta <= etaold)) {
			  std::cout << "screwed etas! (" << eta << "<=" << etaold << ")" << std::endl;
			  return NULL;
		  }
		  etalist.push_back(eta);
		  etaold = eta;
		  ss >> eta;
		  maxeta = eta;
	  }

	  EtaEnergyShowerLib* newlib = new EtaEnergyShowerLib();
	  if (!newlib->readStructure(etalist)) {
		  std::cout << "this structure is not valid" << std::endl;
		  delete newlib;
		  return NULL;
	  }

	  newlib->m_detector = det;
	  newlib->m_particle = part;
	  newlib->m_mineta = mineta;
	  newlib->m_maxeta = maxeta;

	  newlib->m_filled = false;
	  if (mineta < 0) newlib->m_onlyPositive = false;
	  else newlib->m_onlyPositive = true;

	  std::getline(filestr,instr);
	  newlib->m_comment = instr;

	  return newlib;
  }

  std::vector<EnergySpot>* EtaEnergyShowerLib::getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const
  {
	  if (!checkEtaAndStuff(track)) return NULL;
	  
	  double eta = track->GetPosition().eta();

	  if ((m_onlyPositive) && (eta < 0.0)) eta = -eta;

	  library::const_iterator libit = m_libData.upper_bound(eta);
	  
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: mineta=" << m_mineta << " eta=" << eta << std::endl;
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
	  etabin::const_iterator etait = (*libit).second.lower_bound(trenergy);
	  if (etait == (*libit).second.end()) --etait; //if it points to the end, repoint it to the last shower in bin
	  else if (etait != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  etabin::const_iterator etaitch = etait;
		  --etaitch;
		  if (((*etait).first - trenergy) > (trenergy - (*etaitch).first)) { // if the closest is the previous
			  --etait;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*etait).first << std::endl;
	  if (randomShift > 0) {
		  double upperEnergy = (*etait).first * 1.01; //we allow 1% off
		  for (int i = 0; i < randomShift; i++) {
			  etait++;
			  if (etait == (*libit).second.end()) {
				  etait--; // oops! we reached the end of the bin, going back
				  break;
			  }
			  if ((*etait).first > upperEnergy) break; //energy diff too high
		  }
	  }
	  if ((randomShift < 0)&&(etait != (*libit).second.begin())) {
		  double lowerEnergy = (*etait).first * 0.99; //we allow 1% off
		  for (int i = 0; i > randomShift; i--) {
			  etait--;
			  if (etait == (*libit).second.begin()) { // oops! we reached the beginning of the bin
				  //etait++; iterator.begin() is a perfectly normal shower, in contrary to the iterator.end(), which is not
				  break;
			  }
			  if (lowerEnergy > (*etait).first) break; //energy diff too high
		  }
	  }
	  //std::cout << "Found out energy" << std::endl;
	  //std::cout << "Shower with num hits:" << (*etait).second.size() << std::endl;
	  std::vector<EnergySpot>* outshower = new std::vector<EnergySpot>();//((*etait).second);
	  Shower::const_iterator iter;
	  //std::cout << "Created out shower" << std::endl;

	  float energyScale = track->GetKineticEnergy() / (*etait).first;
	  //std::cout << "Scale: " << energyScale << std::endl;

	  for (iter = (*etait).second.begin() /*outshower->begin()*/; iter != (*etait).second.end() /*outshower->end()*/; iter++) {
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

  double EtaEnergyShowerLib::getContainmentZ(const G4Track* track) const
  {
	  if (!checkEtaAndStuff(track)) return 0.0;

	  double eta = track->GetPosition().eta();

	  if ((m_onlyPositive) && (eta < 0.0)) eta = -eta;

	  library::const_iterator libit = m_libData.upper_bound(eta);

	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: mineta=" << m_mineta << " eta=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  if ((*libit).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0.0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  etabin::const_iterator etait = (*libit).second.lower_bound(trenergy);
	  if (etait == (*libit).second.end()) --etait; //if it points to the end, repoint it to the last shower in bin
	  else if (etait != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  etabin::const_iterator etaitch = etait;
		  --etaitch;
		  if (((*etait).first - trenergy) > (trenergy - (*etaitch).first)) { // the closest is the previous
			  --etait;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*etait).first << std::endl;
	  double rezZ = (*etait).second.getZSize();
	  etabin::const_iterator etaiter = etait;
	  int actualNumFS = 1;
	  int spread = 2; //will calculate average Z for 5 showers max ( -2 .. 0 .. +2 )
	  double upperEnergy = (*etait).first * 1.01; //we allow 1% off
	  for (int i = 0; i < spread; i++) {
		  etaiter++;
		  if (etaiter == (*libit).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*etaiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezZ += (*etaiter).second.getZSize();
		  actualNumFS++;
	  }
	  etaiter = etait;
	  if (etaiter != (*libit).second.begin()) {
		  double lowerEnergy = (*etait).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  etaiter--;
			  if (lowerEnergy > (*etaiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezZ += (*etaiter).second.getZSize();
			  actualNumFS++;
			  if (etaiter == (*libit).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezZ/actualNumFS; //average Z size
  }

  double EtaEnergyShowerLib::getContainmentR(const G4Track* track) const
  {
	  if (!checkEtaAndStuff(track)) return 0.0;

	  double eta = track->GetPosition().eta();

	  if ((m_onlyPositive) && (eta < 0.0)) eta = -eta;

	  library::const_iterator libit = m_libData.upper_bound(eta);

	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: mineta=" << m_mineta << " eta=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  if ((*libit).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0.0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  etabin::const_iterator etait = (*libit).second.lower_bound(trenergy);
	  if (etait == (*libit).second.end()) --etait; //if it points to the end, repoint it to the last shower in bin
	  else if (etait != (*libit).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
		  etabin::const_iterator etaitch = etait;
		  --etaitch;
		  if (((*etait).first - trenergy) > (trenergy - (*etaitch).first)) { // the closest is the previous
			  --etait;
		  }
	  }
	  //std::cout << "Found closest energy:" << (*etait).first << std::endl;
	  double rezR = (*etait).second.getRSize();
	  etabin::const_iterator etaiter = etait;
	  int actualNumFS = 1;
	  int spread = 2; //will calculate average Z for 5 showers max ( -2 .. 0 .. +2 )
	  double upperEnergy = (*etait).first * 1.01; //we allow 1% off
	  for (int i = 0; i < spread; i++) {
		  etaiter++;
		  if (etaiter == (*libit).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*etaiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezR += (*etaiter).second.getRSize();
		  actualNumFS++;
	  }
	  etaiter = etait;
	  if (etaiter != (*libit).second.begin()) {
		  double lowerEnergy = (*etait).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  etaiter--;
			  if (lowerEnergy > (*etaiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezR += (*etaiter).second.getRSize();
			  actualNumFS++;
			  if (etaiter == (*libit).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezR/actualNumFS; //average Z size
  }

  bool EtaEnergyShowerLib::storeShower(const HepMC::GenParticle* genParticle, const Shower* shower)
  {
	  if (m_filled) {
		  std::cout << "ERROR: filled" << std::endl;
		  return false;
	  }

	  double eta = genParticle->production_vertex()->position().eta();//momentum().eta();

	  if ((m_onlyPositive) && (eta < 0.0)) eta = -eta;

	  if ( (eta < m_mineta) || (eta > m_maxeta) ) {
		  std::cout << "ERROR: eta is outside: " << m_mineta << " << " << m_maxeta << " : " << eta << std::endl;
		  return false;
	  }
	  if ( genParticle->pdg_id() != m_particle ) {
		  std::cout << "ERROR: wrong pdgcode: " << m_particle << " != " << genParticle->pdg_id() << std::endl;
		  return false;
	  }

	  library::iterator libit = m_libData.upper_bound(eta);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: mineta=" << m_mineta << " eta=" << eta << std::endl;
	  } else {
		  --libit;
	  }
	  (*libit).second.insert(etabin::value_type(genParticle->momentum().e(),(*shower)));
	  return true;
  }

  bool EtaEnergyShowerLib::writeToROOT(TFile* dest)
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


  bool EtaEnergyShowerLib::read(TTree* source)
  {
	  /*
	   * Eta Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |num of showers| min eta for  |    global    |   global |  not   |  - eta bin header
	   * |  in eta bin  | cur eta bin  |   eta min    |  eta max |  used  |
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

	  do {
		  //read eta bin header
		  source->GetEntry(entr++); //x - nshowers, y - min eta in the current eta bin
		  int nsh = (int)(x+0.1); // +0.1 just in case - c++ has low round
		  float curEta = y;
		  etabin * curbin = &(m_libData[curEta]); //creating a new eta bin
		  m_mineta = z;
		  m_maxeta = e;
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
	  if (m_mineta < 0) m_onlyPositive = false;
	  else m_onlyPositive = true;
	  return true;
  }

  bool EtaEnergyShowerLib::write(TTree* dest) const
  {
	  /*
	   * Eta Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |num of showers| min eta for  |    global    |   global |  not   |  - eta bin header
	   * |  in eta bin  | cur eta bin  |   eta min    |  eta max |  used  |
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
	  library::const_iterator libit;
	  for (libit = m_libData.begin(); libit != m_libData.end(); libit ++) {
		  x = (*libit).second.size();
		  y = (*libit).first;
		  z = m_mineta;
		  e = m_maxeta;
		  dest->Fill(); //eta bin header
		  etabin::const_iterator etait;
		  for (etait = (*libit).second.begin(); etait != (*libit).second.end(); etait++) {
			  x = (*etait).second.size();
			  y = (*etait).second.getRSize();
			  z = (*etait).second.getZSize();
			  e = (*etait).first;
			  dest->Fill(); //shower header
			  Shower::const_iterator iter;
			  for (iter = (*etait).second.begin(); iter != (*etait).second.end(); iter++) {
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

  bool EtaEnergyShowerLib::readStructure(std::vector<float>& structure)
  {
	  std::vector<float>::const_iterator iter;

	  for (iter = structure.begin(); iter != structure.end(); iter++) {
		  m_libData[(*iter)];
	  }

	  return true;
  }

  ShowerLibStatistics* EtaEnergyShowerLib::createStatistics() const
  {
	  std::map<int, std::string> names;
	  std::map<int, int> sizes;
	  for(library::const_iterator it = m_libData.begin(); it != m_libData.end(); ++it) {
	    sizes[calcKey(it->first)]=it->second.size();
	    float etalow = it->first;
	    float etahigh;
	    library::const_iterator it_copy = it;
	    ++it_copy;
	    if (it_copy == m_libData.end()) {
	      etahigh = m_maxeta;
	    } else {
	      etahigh = it_copy->first;
	    }
	    std::stringstream ss;
	    ss << std::showpos << std::fixed << std::setprecision(2);
	    ss << "ETA: " << etalow << " .. " << etahigh;
	    names[calcKey(it->first)] = ss.str();
	  }
	  return new ShowerLibStatistics(names, sizes);
  }

  bool EtaEnergyShowerLib::checkEtaAndStuff(const G4Track* track) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return false;
	  }

	  double eta = track->GetPosition().eta();

	  if ((m_onlyPositive) && (eta < 0.0)) eta = -eta;

	  if ( (eta < m_mineta) || (eta > m_maxeta)) {
		  std::cout << "eta is outside library eta range: mineta=" << m_mineta << " maxeta: " << m_maxeta <<" eta=" << eta << std::endl;
		  return false;
	  }

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return false;
	  }
	  return true;
  }

} // namespace ShowerLib
