/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"
#include "PixelCoralClientUtils/PixelCalibData.hh"

#include<string>
#include<cstring>
#include<map>
#include<set>
#include<fstream>
using namespace std;

int main ATLAS_NOT_THREAD_SAFE (int argc, char **argv){ // Thread unsafe const_iterator end is used.

  int iarg = 1;
  bool verbose = false;
  if (argc>iarg && strcmp(argv[iarg],"-v")==0) {
    verbose = true;
    ++iarg;
  }

  enum OPTION { THRESHOLDS, TIMEWALK, TOT };

  OPTION option = TOT;
  if (argc>iarg && strcmp(argv[iarg],"-t")==0) {
    option = THRESHOLDS;
    ++iarg;
  }
  if (argc>iarg && strcmp(argv[iarg],"-w")==0) {
    option = TIMEWALK;
    ++iarg;
  }

  if (iarg>=argc) {
    cout << "Usage: extractAnalysis [-v] [-t|-w] anal_id" << endl;
    return 1;
  }

  CAN::SerialNumber_t anal_id = atoi(argv[iarg]);

  cout << "--> Summary for anal_id " << anal_id << endl;

  //
  // load pcd from text file
  //

  map<unsigned int, PixelCoralClientUtils::PixelCalibData> pcdmap;
  ifstream in("pcd.dat");
  if (!in.is_open()) return 1;

  while (in) {
    unsigned int idmod; in >> idmod;
    if (in.eof()) break;

    PixelCoralClientUtils::PixelCalibData pcd;

    for (int i = 0; i<16; ++i) {
      char dummy; in >> dummy;
      if (dummy!='I') break;
      int idchip; in >> idchip;
      if (idchip!=i) break;
      PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData pcsd;
      pcsd.setChipID(idchip);
      int thres, sigma, noise, timewalk;
      in >> thres >> sigma >> noise >> timewalk;
      pcsd.setThreshold(0, thres, sigma, noise, timewalk);
      in >> thres >> sigma >> noise >> timewalk;
      pcsd.setThreshold(1, thres, sigma, noise, timewalk);
      in >> thres >> sigma >> noise >> timewalk;
      pcsd.setThreshold(2, thres, sigma, noise, timewalk);
      float A, E, C;
      in >> A >> E >> C;
      pcsd.setQ2Tot(0, A, E, C);
      in >> A >> E >> C;
      pcsd.setQ2Tot(2, A, E, C);
      float p1, p2;
      in >> p1 >> p2;
      pcsd.setTotRes(p1, p2);
      pcd.add(pcsd);
    }

    pcdmap.insert(make_pair(idmod,pcd));
  }
  cout << "pix cal data size: " << pcdmap.size() << endl;

  //
  // load module->hashid lookup table
  //

  map<string, unsigned int> lookup;
  ifstream in_lookup("aliases.dat");
  if (!in_lookup.is_open()) return 1;

  string s;
  while (getline(in_lookup, s)) {
    if (s.find("HASHID") != string::npos) {
      istringstream ss(s);
      unsigned int idmod;
      string s1, s2;
      ss >> idmod >> s1 >> s2;
      lookup.insert(make_pair(s2,idmod));
    }
  }
  cout << "lookup table size: " << lookup.size() << endl;

  map<string, unsigned int> lookup_errors;

  //
  // connect to analysis data base
  //

  try {

    // Start CORAL client
    const char* tableName = "CALIB_ANAL";
    PixCoralClient coralClient(verbose, coral::ReadOnly, tableName);

    // Show the structure of the master table
    if (verbose) {
      coralClient.printTableDesc(tableName);
      coralClient.printTableContent(tableName);
    }

    // Return a CAN::AnalysisResultList_t object
    CAN::AnalysisResultList_t a = coralClient.getAnalysisResultsFromDB(anal_id);

    if (option!=TOT) {
      //
      // threshold/timewalk calibration
      //

      // load bool parameters
      set<unsigned int> passed_idmod;
      set<unsigned int> failed_idmod;
      if (verbose) cout << "looking at bools" << endl;
      for (map<string, map<string, bool> >::const_iterator iter = a.begin<bool>();
	   iter != a.end<bool>(); ++iter) {
	const string& s = iter->first;
	if (verbose) cout << s << endl;
	// figure out if we need it
	if (s != "Status") continue;
	for (map<string, bool>::const_iterator val_iter = iter->second.begin();
	     val_iter != iter->second.end(); ++val_iter) {
	  const string& ss = val_iter->first;
	  bool value = val_iter->second;
	  // convert connectivity to module id
	  unsigned int idmod = lookup[ss];
	  if (value) passed_idmod.insert(idmod);
	  else failed_idmod.insert(idmod);
	}
      }
      cout << "good " << passed_idmod.size() << ", bad " << failed_idmod.size() << endl;

      // load float parameters
      // verify if the scan is good
      for (map<string, map<string, float> >::const_iterator iter = a.begin<float>();
	   iter != a.end<float>(); ++iter) {
	const string& s = iter->first;
	if (s != "number of bad pixels") continue;
	for (map<string, float>::const_iterator val_iter = iter->second.begin();
	     val_iter != iter->second.end(); ++val_iter) {
	  const string& ss = val_iter->first;
	  // convert connectivity to module id
	  unsigned int idmod = lookup[ss];
	  float value = val_iter->second;
	  const float maxBadPixels = 20000;
	  if (value>=maxBadPixels) {
	    if (verbose) cout << s << "-> " << ss << " (" << idmod << "): too many bad pixels = " << value << endl;
	    failed_idmod.insert(idmod);
	  }
	}
      }
      // proceed with loading
      set<unsigned int> used_idmod;
      set<unsigned int> rejected_idmod;
      if (verbose) cout << "looking at floats" << endl;
      for (map<string, map<string, float> >::const_iterator iter = a.begin<float>();
	   iter != a.end<float>(); ++iter) {
	const string& s = iter->first;
	if (verbose) cout << s << endl;
	// figure out if we need it
	const int nvar = 5;
	string svar[nvar] = {
	  "average threshold",
	  "threshold RMS",
	  "average noise for normal pixel",
	  "average noise for long pixel",
	  "average noise for ganged pixel"
	};
	if (option==TIMEWALK) svar[0] = "Average_threshold";
	int jvar = -1;
	for (int ivar = 0; ivar<nvar; ++ivar) {
	  if (s == svar[ivar]) {
	    jvar = ivar;
	    break;
	  }
	}
	if (jvar<0) continue;
	if (option==TIMEWALK && jvar>0) continue;
	for (map<string, float>::const_iterator val_iter = iter->second.begin();
	     val_iter != iter->second.end(); ++val_iter) {
	  const string& ss = val_iter->first;
	  // convert connectivity to module id
	  unsigned int idmod = lookup[ss];
	  if (verbose) cout << s << "-> " << ss << " (" << idmod << ")";
	  // check whether the scans are good
	  if (failed_idmod.find(idmod)!=failed_idmod.end()) {
	    rejected_idmod.insert(idmod);
	    if (verbose) cout << ": bad scan" << endl;
	    continue;
	  }
	  used_idmod.insert(idmod);
	  // find corresponding PixelCalibData
	  map<unsigned int, PixelCoralClientUtils::PixelCalibData>::iterator ipcd = pcdmap.find(idmod);
	  //if (ipcd == pcdmap.end()) throw std::runtime_error("Invalid module ID");
	  if (ipcd == pcdmap.end()) {
	    lookup_errors.insert(make_pair(ss, idmod));
	    if (verbose) cout << ": lookup error" << endl;
	    continue;
	  }
	  PixelCoralClientUtils::PixelCalibData& pcd = ipcd->second;
	  float value = val_iter->second;
	  // use the same value for all FE
	  for (int idchip = 0; idchip<16; ++idchip) {
	    PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd = *pcd.findChipID(idchip);
	    // replace the parameter in the table
	    int v[12];
	    for (int type = 0; type<3; ++type) {
	      v[type*4  ] = pcsd.getThreshold(type);
	      v[type*4+1] = pcsd.getThresholdSigma(type);
	      v[type*4+2] = pcsd.getNoise(type);
	      v[type*4+3] = pcsd.getTimeWalk(type);
	    }
	    bool verbose1 = verbose && idchip==0;
	    if (verbose1) cout << svar[jvar] << "=";
	    if (option==THRESHOLDS) {
	      if (jvar==0) { // average threshold
		if (verbose1) cout << v[0] << "," << v[4] << "," << v[8];
		v[0] = v[4] = v[8] = (int)value;
	      } else if (jvar==1) { // threshold RMS
		if (verbose1) cout << v[1] << "," << v[5] << "," << v[9];
		v[1] = v[5] = v[9] = (int)value;
	      } else { // average noise
		int type = jvar-2;
		if (verbose1) cout << v[type*4+2];
		v[type*4+2] = (int)value;
	      }
	    } else { // timewalk, jvar is always 0
	      if (verbose1) cout << v[3] << "," << v[7] << "," << v[11];
	      v[3] = v[7] = v[11] = (int)value;
	    }
	    if (verbose1) cout << " replaced by " << value << endl;
	    for (int type = 0; type<3; ++type) {
	      pcsd.setThreshold(type, v[type*4], v[type*4+1], v[type*4+2], v[type*4+3]);
	    }
	  }
	}
      }
      cout << "modified modules: " << used_idmod.size() << endl;
      cout << "rejected modules: " << rejected_idmod.size() << endl;

    } else {

      //
      // ToT calibration
      //

      // load bool parameters
      set<unsigned int> passed_n_id;
      set<unsigned int> passed_g_id;
      set<unsigned int> failed_n_id;
      set<unsigned int> failed_g_id;
      if (verbose) cout << "looking at bools" << endl;
      for (map<string, map<string, bool> >::const_iterator iter = a.begin<bool>();
	   iter != a.end<bool>(); ++iter) {
	const string& s = iter->first;
	if (verbose) cout << s << endl;
	// figure out if we need it
	if (s.compare(0, 6, "passed")) continue;
	// determine chip id
	int ix = s.find("FE") + 2;
	int idchip = atoi(s.substr(ix).c_str());
	// determine pixel type
	int itype = -1;
	if (s.find("non_ganged_pix") != string::npos) itype = 0;
	else if (s.find("ganged_pix") != string::npos) itype = 2;
	if (itype<0) continue;
	for (map<string, bool>::const_iterator val_iter = iter->second.begin();
	     val_iter != iter->second.end(); ++val_iter) {
	  const string& ss = val_iter->first;
	  bool value = val_iter->second;
	  // convert connectivity to module id
	  unsigned int idmod = lookup[ss];
	  unsigned int id = (idmod<<4) | idchip;
	  if (itype==0) {
	    if (value) passed_n_id.insert(id);
	    else failed_n_id.insert(id);
	  }
	  else if (itype==2) {
	    if (value) passed_g_id.insert(id);
	    else failed_g_id.insert(id);
	  }
	}
      }
      cout << "non_ganged_pixels: good " << passed_n_id.size() << ", bad " << failed_n_id.size() << endl;
      cout << "ganged_pixels:     good " << passed_g_id.size() << ", bad " << failed_g_id.size() << endl;

      // load float parameters
      set<unsigned int> used_id;
      set<unsigned int> rejected_id;
      if (verbose) cout << "looking at floats" << endl;
      for (map<string, map<string, float> >::const_iterator iter = a.begin<float>();
	   iter != a.end<float>(); ++iter) {
	const string& s = iter->first;
	if (verbose) cout << s << endl;
	// figure out if we need it
	const int nvar = 5;
	const string svar[nvar] = {
	  "tot_p0_FE", "tot_p1_FE", "tot_p2_FE",
	  "disp_p0_FE", "disp_p1_FE"
	};
	int jvar = -1;
	for (int ivar = 0; ivar<nvar; ++ivar) {
	  if (s.compare(0, svar[ivar].length(), svar[ivar])==0) {
	    jvar = ivar;
	    break;
	  }
	}
	if (jvar<0) continue;
	// determine chip id
	int ix = s.find("FE") + 2;
	int idchip = atoi(s.substr(ix).c_str());
	// determine pixel type
	int itype = -1;
	if (s.find("non_ganged_pix") != string::npos) itype = 0;
	else if (s.find("ganged_pix") != string::npos) itype = 2;
	// obsolete names, used for A5082
	//if (s.find("normal_pix") != string::npos) itype = 0;
	//else if (s.find("special-non-ganged_pix") != string::npos) itype = 1;
	//else if (s.find("ganged_pix") != string::npos) itype = 2;
	if (itype<0) continue;
	for (map<string, float>::const_iterator val_iter = iter->second.begin();
	     val_iter != iter->second.end(); ++val_iter) {
	  const string& ss = val_iter->first;
	  // convert connectivity to module id
	  unsigned int idmod = lookup[ss];
	  if (verbose) cout << s << "-> " << ss << ":" << idchip << " (" << idmod << ")";
	  // check whether the scans are good
	  unsigned int id = (idmod<<4) | idchip;
	  if (failed_n_id.find(id)!=failed_n_id.end() ||
	      failed_g_id.find(id)!=failed_g_id.end()) {
	    rejected_id.insert(id);
	    if (verbose) cout << ": bad scan" << endl;
	    continue;
	  }
	  used_id.insert(id);
	  // find corresponding PixelCalibData
	  map<unsigned int, PixelCoralClientUtils::PixelCalibData>::iterator ipcd = pcdmap.find(idmod);
	  //if (ipcd == pcdmap.end()) throw std::runtime_error("Invalid module ID");
	  if (ipcd == pcdmap.end()) {
	    lookup_errors.insert(make_pair(ss, idmod));
	    if (verbose) cout << ": lookup error" << endl;
	    continue;
	  }
	  PixelCoralClientUtils::PixelCalibData& pcd = ipcd->second;
	  PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd = *pcd.findChipID(idchip);
	  float value = val_iter->second;
	  // replace the parameter in the table
	  if (s.compare(0,5,"tot_p")==0) {
	    float v[3];
	    v[0] = pcsd.getQ2TotA(itype);
	    v[1] = pcsd.getQ2TotE(itype);
	    v[2] = pcsd.getQ2TotC(itype);
	    int j = s[5]-'0'; if (j<0 || j>2) continue;
	    if (verbose) {
	      cout << ", ";
	      if (j==0) cout << "A"; else if (j==1) cout << "E"; else cout << "C";
	      cout << "=" << v[j] << " replaced by " << value << endl;
	    }
	    v[j] = value;
	    pcsd.setQ2Tot(itype, v[0], v[1], v[2]);
	  } else if (s.compare(0,6,"disp_p")==0) {
	    float v[2];
	    v[0] = pcsd.getTotP1();
	    v[1] = pcsd.getTotP2();
	    int j = s[6]-'0'; if (j<0 || j>1) continue;
	    if (verbose) {
	      cout << ", ";
	      cout << "p" << j+1 << "=" << v[j] << " replaced by " << value << endl;
	    }
	    v[j] = value;
	    pcsd.setTotRes(v[0], v[1]);
	  } else {
	    if (verbose) cout << endl;
	  }
	}
      }
      cout << "modified chips: " << used_id.size() << endl;
      cout << "rejected chips: " << rejected_id.size() << endl;
      cout << "total modules:  " << (used_id.size()+rejected_id.size())/16 << endl;
    }

    if (!lookup_errors.empty()) {
      cout << "module ids not found in the lookup table:" << endl;
      for (map<string, unsigned int>::const_iterator il = lookup_errors.begin();
	   il != lookup_errors.end(); ++il) {
	cout << il->first << " " << il->second << endl;
      }
    }

  } catch ( exception& e ) {
    cout << "exception caught: " << e.what() << endl;
    return -1;
  }

  catch (...) {
    cout << "Unknown exception caught!" << endl;
    return -1;
  }

  //
  // save pcd back to text file
  //

  ofstream out("pcd.sav");
  for (map<unsigned int, PixelCoralClientUtils::PixelCalibData>::const_iterator ipcd = pcdmap.begin();
       ipcd != pcdmap.end(); ++ipcd) {
    out << ipcd->first << endl;
    const PixelCoralClientUtils::PixelCalibData& pcd = ipcd->second;
    for (int i = 0; i<16; ++i) {
      const PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd = *pcd.getPixelChipSummaryData(i);
      out << "I" << pcsd.getChipID()
          << " " << pcsd.getThreshold(0)
          << " " << pcsd.getThresholdSigma(0)
          << " " << pcsd.getNoise(0)
          << " " << pcsd.getTimeWalk(0)
          << " " << pcsd.getThreshold(1)
          << " " << pcsd.getThresholdSigma(1)
          << " " << pcsd.getNoise(1)
          << " " << pcsd.getTimeWalk(1)
          << " " << pcsd.getThreshold(2)
          << " " << pcsd.getThresholdSigma(2)
          << " " << pcsd.getNoise(2)
          << " " << pcsd.getTimeWalk(2)
          << " " << pcsd.getQ2TotA(0)
          << " " << pcsd.getQ2TotE(0)
          << " " << pcsd.getQ2TotC(0)
          << " " << pcsd.getQ2TotA(2)
          << " " << pcsd.getQ2TotE(2)
          << " " << pcsd.getQ2TotC(2)
          << " " << pcsd.getTotP1()
          << " " << pcsd.getTotP2() << endl;
    }
  }
  out.close();

  return 0;

}
