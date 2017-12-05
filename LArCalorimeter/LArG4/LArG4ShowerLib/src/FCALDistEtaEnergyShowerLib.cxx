/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "LArG4ShowerLib/FCALDistEtaEnergyShowerLib.h"

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

#define LIB_VERSION 5
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
#define LIB_VERSION_OLD 3
#endif

namespace ShowerLib {

  inline int calcKey(float eta, float dist) {
	  return (int)(eta*1000000)+(int)(dist*1000);
  }

  float FCALDistEtaEnergyShowerLib::distance (double x, double y) const
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

  IShowerLib* FCALDistEtaEnergyShowerLib::readFromROOTFile(TFile* source)
  {

      
      TParameter<int>* ver;
      ver = (TParameter<int>*)source->Get("version");

      if ((ver == NULL) || (ver->GetVal() != LIB_VERSION)) 
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
           if ((ver == NULL) || (ver->GetVal() != LIB_VERSION_OLD))
#endif
             return NULL;

      TTree* TTreeMeta = (TTree*)source->Get("meta");
      TTree* TTreeLib  = (TTree*)source->Get("library");

      if ((TTreeMeta == NULL) || (TTreeLib == NULL)) return NULL;

      std::cout << "FCALDistEtaEnergyShowerLib header found." << std::endl;

	  FCALDistEtaEnergyShowerLib* newlib = new FCALDistEtaEnergyShowerLib();
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
      if (ver->GetVal() == LIB_VERSION_OLD)
          newlib->m_compat=true;
      else
	  newlib->m_compat=false;
#endif

	  if (!(newlib->readMeta(TTreeMeta)) || !(newlib->read(TTreeLib))) {
		  delete newlib;
	      std::cout << "FCALDistEtaEnergyShowerLib read unsuccessful." << std::endl;
		  return NULL;
	  }

	  return newlib;

  }

  IShowerLib* FCALDistEtaEnergyShowerLib::createEmptyLib(const std::string& inputFile)
  {
	  /*
	   * Eta Energy Structure format:
	   *
	   * VER PART DET
	   * XRODCENT YRODCENT STEP
	   * ETA1 DIST1 DIST2 ...
	   * ETA2 DIST1 DIST2 ...
	   * ...
	   * END
	   * COMMENT
	   *
	   * where
	   *
	   * VER == 5
	   * DIST(N+1) > DIST(N)
	   * DIST(N) >= 0
	   * First DIST is the beginning of the lib. If not zero, zero will be added
	   * Last DIST is the end of the lib. If less then 4.5, 4.5 will be added
	   */
	  std::ifstream filestr(inputFile.c_str(),std::ios::in);


	  if (!filestr.is_open()) {
		  std::cout << "FCALDistEtaEnergyShowerLib    " << inputFile << ": bad file!" << std::endl;
		  return NULL;
	  }

	  std::string instr;
	  std::getline(filestr,instr);

	  int ver;
	  int part;
	  std::string det;

	  {
		  std::stringstream ss(instr);

		  ss >> ver;
		  
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
		  if (ver == LIB_VERSION_OLD) {
			  std::cout << "FCALDistEtaEnergyShowerLib: you are trying to create the old version of this library. Use the new one." << std::endl;
			  return NULL;
		  }
#endif
		  if (ver != LIB_VERSION) {
			  return NULL;
		  }

		  ss >> part >> det;
	  }

	  std::getline(filestr,instr);
	  double xrodcent, yrodcent,step;
	  {
	  std::stringstream ss(instr);
	  ss >> xrodcent >> yrodcent >> step;
	  }

	  std::map<float,std::vector<float> > libstruct;

	  float etaold = 0;
	  std::getline(filestr,instr);
	  while (instr != "END") {

		  std::stringstream ss(instr);

		  float eta;
		  ss >> eta;

		  if (etaold == 0) {
			  if (eta > 3.0001) {
				  std::cout << "First eta should be 3.0 (have " << eta << ")" << std::endl;
				  return NULL;
			  }
			  eta = 3.0;
		  } else if (eta <= etaold) {
			  std::cout << "eta is not correct: " << eta << "<=" << etaold << std::endl;
			  return NULL;
		  }
		  etaold = eta;

		  std::vector<float> * distlist = &(libstruct[eta]);

		  float dist;
		  float distold;

		  ss >> dist;

		  if (ss.fail()) {
			  std::cout << "file reading failed! (not enough data)" << std::endl;
			  return NULL;
		  }

		  if (dist != 0.) {
			  if (dist < 0) {
				  std::cout << "no negative dists allowed (" << eta << ")" << std::endl;
				  return NULL;
			  }
			  distlist->push_back(0.);
		  }

		  distlist->push_back(dist);
		  distold = dist;
		  
		  float maxdist = step/sqrt(3);

		  while (!ss.eof()) {
			  ss >> dist;
			  if ((ss.fail()) || (dist <= distold)) {
				  std::cout << "screwed dists! (" << dist << "<=" << distold << ")" << std::endl;
				  return NULL;
			  }
			  if (dist < maxdist) {
				  distlist->push_back(dist);
			  } else if (dist >= maxdist) {
				  std::cout << "dist can't be bigger than " << maxdist << " (" << dist << "). ignored" << std::endl;
			  }
			  distold = dist;
		  }
		  std::getline(filestr,instr);
		  if (filestr.fail()) {
			  std::cout << "file reading failed! (not enough data)" << std::endl;
			  return NULL;
		  }
	  }

	  //if (etalist.back() < 4.5) etalist.push_back(4.5);

	  FCALDistEtaEnergyShowerLib* newlib = new FCALDistEtaEnergyShowerLib();
	  if (!newlib->readStructure(libstruct)) {
		  std::cout << "this structure is not valid" << std::endl;
		  delete newlib;
		  return NULL;
	  }

	  newlib->m_detector = det;
	  newlib->m_particle = part;
	  
	  newlib->m_xrodcent = xrodcent;
	  newlib->m_yrodcent = yrodcent;
	  newlib->m_step = step;

	  newlib->m_filled = false;

	  std::getline(filestr,instr);
	  newlib->m_comment = instr;

	  return newlib;
  }

  std::vector<EnergySpot>* FCALDistEtaEnergyShowerLib::getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return NULL;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);
	  double eta = track->GetPosition().eta();
	  if (eta < 0) {
		  eta = -eta;
	  }

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return NULL;
	  }
	  if (eta < 3.0) {
		  std::cout << "wrong eta: |eta|=" << eta << " is not inside FCAL" << std::endl;
		  return NULL;
	  }

	  //std::cout << "Extracted particle parameters: " << eta << std::endl;

	  library::const_iterator libit = m_libData.upper_bound(eta);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: |eta|=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  etabin::const_iterator etait = (*libit).second.upper_bound(dist);

	  if (etait == (*libit).second.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --etait;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*etait).second.size() == 0) {
		  std::cout << "The bin corresponding to the eta/dist pair is empty" << std::endl;
		  return NULL;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*etait).second.lower_bound(trenergy);
	  if (distit == (*etait).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*etait).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
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
			  if (distit == (*etait).second.end()) {
				  distit--;
				  break;
			  }
			  if ((*distit).first > upperEnergy) break; //energy diff too high
		  }
	  }
	  if ((randomShift < 0)&&(distit != (*etait).second.begin())) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i > randomShift; i--) {
			  distit--;
			  if (distit == (*etait).second.begin()) {
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
		  stats->recordShowerLibUse(calcKey((*libit).first,(*etait).first));
	  }
	  //std::cout << "Done" << std::endl;
	  return outshower;
  }

  double FCALDistEtaEnergyShowerLib::getContainmentZ(const G4Track* track) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);
	  double eta = track->GetPosition().eta();
	  if (eta < 0) {
		  eta = -eta;
	  }

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return 0;
	  }
	  if (eta < 3.0) {
		  std::cout << "wrong eta: |eta|=" << eta << " is not inside FCAL" << std::endl;
		  return 0;
	  }

	  library::const_iterator libit = m_libData.upper_bound(eta);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: |eta|=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  etabin::const_iterator etait = (*libit).second.upper_bound(dist);
	  if (etait == (*libit).second.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --etait;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*etait).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*etait).second.lower_bound(trenergy);
	  if (distit == (*etait).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*etait).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
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
		  if (distiter == (*etait).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*distiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezZ += (*distiter).second.getZSize();
		  actualNumFS++;
	  }
	  distiter = distit;
	  if (distiter != (*etait).second.begin()) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  distiter--;
			  if (lowerEnergy > (*distiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezZ += (*distiter).second.getZSize();
			  actualNumFS++;
			  if (distiter == (*etait).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezZ/actualNumFS; //average Z size
  }

  double FCALDistEtaEnergyShowerLib::getContainmentR(const G4Track* track) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0;
	  }

	  //std::cout << "Starting getShower()" << std::endl;

	  double x = track->GetPosition().getX();
	  double y = track->GetPosition().getY();

	  float dist = this->distance(x,y);
	  double eta = track->GetPosition().eta();
	  if (eta < 0) {
		  eta = -eta;
	  }

	  G4int particleCode = track->GetDefinition()->GetPDGEncoding();
	  if ( particleCode < 0 ) particleCode = -particleCode; // hack for positrons.

	  if ( particleCode != m_particle ) {
		  std::cout << "wrong particle: " << particleCode << "!=" << m_particle << std::endl;
		  return 0;
	  }
	  if (eta < 3.0) {
		  std::cout << "wrong eta: |eta|=" << eta << " is not inside FCAL" << std::endl;
		  return 0;
	  }

	  library::const_iterator libit = m_libData.upper_bound(eta);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: |eta|=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  etabin::const_iterator etait = (*libit).second.upper_bound(dist);
	  if (etait == (*libit).second.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --etait;
	  }

	  //std::cout << "Found eta bin" << std::endl;

	  if ((*etait).second.size() == 0) {
		  std::cout << "The etabin corresponding to the eta is empty" << std::endl;
		  return 0;
	  }
	  double trenergy = track->GetKineticEnergy();
	  //std::cout << "Energy: " << trenergy << std::endl;
	  distbin::const_iterator distit = (*etait).second.lower_bound(trenergy);
	  if (distit == (*etait).second.end()) --distit; //if it points to the end, repoint it to the last shower in bin
	  else if (distit != (*etait).second.begin()) { //find the closest energy. it's either found item or previous (if there is a previous)
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
		  if (distiter == (*etait).second.end()) {
			  break;
		  }
		  if (upperEnergy < (*distiter).first) break; //energy diff too high
		  //the shower is OK, including it to the average
		  rezR += (*distiter).second.getRSize();
		  actualNumFS++;
	  }
	  distiter = distit;
	  if (distiter != (*etait).second.begin()) {
		  double lowerEnergy = (*distit).first * 0.99; //we allow 1% off
		  for (int i = 0; i < spread; i++) {
			  distiter--;
			  if (lowerEnergy > (*distiter).first) break; //energy diff too high
			  //the shower is OK, including it to the average
			  rezR += (*distiter).second.getRSize();
			  actualNumFS++;
			  if (distiter == (*etait).second.begin()) {
				  break;
			  }
		  }
	  }
	  return rezR/actualNumFS; //average Z size
  }

  bool FCALDistEtaEnergyShowerLib::storeShower(const HepMC::GenParticle* genParticle, const Shower* shower)
  {
	  if (m_filled) {
		  std::cout << "ERROR: filled" << std::endl;
		  return false;
	  }

	  double x = genParticle->production_vertex()->position().x();
	  double y = genParticle->production_vertex()->position().y();

	  float dist = this->distance(x,y);
	  double eta = genParticle->production_vertex()->position().eta();//momentum().eta();
	  if (eta < 0) {
		  eta = -eta;
	  }

	  if ( genParticle->pdg_id() != m_particle ) {
		  std::cout << "ERROR: wrong pdgcode: " << m_particle << " != " << genParticle->pdg_id() << std::endl;
		  return false;
	  }
	  if (eta < 3.0) {
		  std::cout << "wrong eta: |eta|=" << eta << " is not inside FCAL" << std::endl;
		  return 0;
	  }

	  library::iterator libit = m_libData.upper_bound(eta);
	  if (libit == m_libData.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with eta: |eta|=" << eta << std::endl;
	  } else {
		  --libit;
	  }

	  etabin::iterator etait = (*libit).second.upper_bound(dist);
	  if (etait == (*libit).second.begin()) {
		  //this is really weird
		  std::cout << "Something is wrong with dist: x=" << x << " y=" << y << " dist=" << dist << std::endl;
	  } else {
		  --etait;
	  }
	  (*etait).second.insert(distbin::value_type(genParticle->momentum().e(),(*shower)));
	  return true;
  }

  bool FCALDistEtaEnergyShowerLib::writeToROOT(TFile* dest)
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


  bool FCALDistEtaEnergyShowerLib::read(TTree* source)
  {
	  /*
	   * Dist Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |  xrod cent   |  yrod cent   |step (roddist)|   not    |  not   |  - library header
	   * | (parameter)  | (parameter)  | (parameter)  |   used   |  used  |
	   * ------------------------------------------------------------------
	   * |num of distbin| min eta  for |     not      |   not    |  not   |  - eta bin header
	   * | in eta bin   | cur eta  bin |     used     |   used   |  used  |
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
	  
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
	  if (m_compat == false) {
#endif
          source->GetEntry(entr++);
	  m_xrodcent = x;
	  m_yrodcent = y;
	  m_step = z;
#ifndef __FSLIB_NO_BACKWARD_COMPAT__
	  } else {
	      m_xrodcent = -748.12;
	      m_yrodcent = -717.719;
	      m_step = 7.5;
	  }
#endif


	  do {
		  //read eta bin header
		  source->GetEntry(entr++); //x - ndistbins, y - min eta in the current eta bin
		  int ndists = (int)(x+0.1); // +0.1 just in case - c++ has low round
		  float curEta = y;
		  etabin * curetabin = &(m_libData[curEta]); //creating a new eta bin

		  for (int i = 0; i < ndists; i++) {
			  source->GetEntry(entr++); //x - nshowers, y - min dist in the current dist bin
			  int nsh = (int)(x+0.1); // +0.1 just in case - c++ has low round
			  float curDist = y;
			  distbin * curbin = &((*curetabin)[curDist]); //creating a new dist bin
			  for(int j = 0; j < nsh; j++) {
				  //read shower header
				  source->GetEntry(entr++); //x - nhits, y - r size, z - z size, e - gen energy
				  int nhits = (int)(x+0.1);
				  float curEnergy = e;
				  Shower * shower = &((*curbin)[curEnergy]);
				  shower->setRSize(y);
				  shower->setZSize(z);
				  for(int k = 0; k < nhits; k++) {
					  source->GetEntry(entr++); //variables mean what the name suggests
					  shower->push_back(new ShowerEnergySpot(G4ThreeVector(x,y,z),e,time));
				  }
			  }
		  }
	  } while (entr < nentr);
	  
	  if (entr != nentr) {
	      return false;
	  }

	  m_filled = true;
	  return true;
  }

  bool FCALDistEtaEnergyShowerLib::write(TTree* dest) const
  {
	  /*
	   * Dist Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |  xrod cent   |  yrod cent   |step (roddist)|   not    |  not   |  - library header
	   * | (parameter)  | (parameter)  | (parameter)  |   used   |  used  |
	   * ------------------------------------------------------------------
	   * |num of distbin| min eta  for |     not      |   not    |  not   |  - eta bin header
	   * | in eta bin   | cur eta  bin |     used     |   used   |  used  |
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
		  etabin::const_iterator etait;
		  for (etait = (*libit).second.begin(); etait != (*libit).second.end(); etait++) {
			  x = (*etait).second.size();
			  y = (*etait).first;
			  z = 0;
			  e = 0;
			  time = 0;
			  dest->Fill(); //eta bin header
			  distbin::const_iterator distit;
			  for (distit = (*etait).second.begin(); distit != (*etait).second.end(); distit++) {
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
	  }
	  //dest->Write();
	  return true;
  }

  const std::string FCALDistEtaEnergyShowerLib::printParameters() const
  {
	  std::stringstream ss;
	  ss << std::fixed << std::setprecision(3);
	  ss << "Distance calculator parameters: xrod_cent=" << m_xrodcent << " yrod_cent=" << m_yrodcent << " step=" << m_step;
	  return ss.str();
  }

  bool FCALDistEtaEnergyShowerLib::readStructure(std::map<float, std::vector<float> >& structure)
  {
	  std::map<float, std::vector<float> >::const_iterator iter;

	  for (iter = structure.begin(); iter != structure.end(); iter++) {
		  m_libData[(*iter).first];
		  std::vector<float>::const_iterator inneriter;
		  for (inneriter = (*iter).second.begin(); inneriter != (*iter).second.end(); inneriter++) {
			  (m_libData[(*iter).first])[(*inneriter)];
		  }
	  }

	  return true;
  }

  ShowerLibStatistics* FCALDistEtaEnergyShowerLib::createStatistics() const
  {
	  std::map<int, std::string> names;
	  std::map<int, int> sizes;
	  for(library::const_iterator it = m_libData.begin(); it != m_libData.end(); ++it) {
		  for(etabin::const_iterator etait = (*it).second.begin(); etait != (*it).second.end(); ++etait) {
			    sizes[calcKey(it->first, etait->first)]=etait->second.size();
			    float etalow = it->first;
			    float distlow = etait->first;
			    float etahigh;
			    float disthigh;
			    library::const_iterator it_copy = it;
			    ++it_copy;
			    if (it_copy == m_libData.end()) {
			      etahigh = 9.99;
			    } else {
			      etahigh = it_copy->first;
			    }
			    etabin::const_iterator etait_copy = etait;
			    ++etait_copy;
			    if (etait_copy == (*it).second.end()) {
			      disthigh = 4.5;
			    } else {
			      disthigh = etait_copy->first;
			    }
			    std::stringstream ss;
			    ss << std::showpos << std::fixed << std::setprecision(2);
			    ss << "ETA: " << etalow << " .. " << etahigh << " DIST: " << distlow << " .. " << disthigh;
			    names[calcKey(it->first, etait->first)]= ss.str();
		  }
	  }
	  return new ShowerLibStatistics(names, sizes);
  }

} // namespace ShowerLib
