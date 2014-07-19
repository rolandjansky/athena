/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PixelCalibData.hh"

#include<string>
#include<map>
#include<fstream>
using namespace std;

int load_pcd(map<unsigned int, PixelCoralClientUtils::PixelCalibData>& pcdmap,
	     const char* filename)
{
  ifstream in(filename);
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

  return 0;
}

//int main(int argc, char **argv){
int main(int, char **){

  //
  // load pcd from text file
  //

  map<unsigned int, PixelCoralClientUtils::PixelCalibData> pcdmap_old;
  if (load_pcd(pcdmap_old, "pcd_old.dat")) return 1;
  map<unsigned int, PixelCoralClientUtils::PixelCalibData> pcdmap;
  if (load_pcd(pcdmap, "pcd.dat")) return 1;

  cout << "pix cal data size: " << pcdmap_old.size() << "/" << pcdmap.size() << endl;

  //
  // recalculate timewalk according to the formula
  // new_timewalk = old_timewalk - old_threshold + new_threshold
  //

  for (map<unsigned int, PixelCoralClientUtils::PixelCalibData>::iterator ipcd = pcdmap.begin();
       ipcd != pcdmap.end(); ++ipcd) {
    PixelCoralClientUtils::PixelCalibData& pcd = ipcd->second;
    PixelCoralClientUtils::PixelCalibData& pcd_old = pcdmap_old[ipcd->first];
    for (int i = 0; i<16; ++i) {
      PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd = *pcd.findChipID(i);
      PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd_old = *pcd_old.findChipID(i);
      // replace the parameter in the table
      for (int type = 0; type<3; ++type) {
	int old_threshold = pcsd_old.getThreshold(type);
	int new_threshold = pcsd.getThreshold(type);
	int sigma = pcsd.getThresholdSigma(type);
	int noise = pcsd.getNoise(type);
	int old_timewalk = pcsd_old.getTimeWalk(type);
	int new_timewalk = old_timewalk - old_threshold + new_threshold;
	pcsd.setThreshold(type, new_threshold, sigma, noise, new_timewalk);
      }
    }
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
