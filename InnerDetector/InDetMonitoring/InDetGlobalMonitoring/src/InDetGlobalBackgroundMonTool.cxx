/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/** @file InDetGlobalBackgroundMonTool.cxx
 * Tool for Monitoring the Beam Induced Background in the SCT
 *
 * @author Edoardo Rossi <Edoardo.Rossi@cern.ch> @n
 *
 *********************************************************************************/


//Local
#include "InDetGlobalBackgroundMonTool.h"
//Framework
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "InDetRawData/InDetTimeCollection.h"
//Standard c++
#include <iostream>
#include <fstream>

//Space Points
#include "InDetIdentifier/SCT_ID.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"

//plots
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

//Definitons related to SCT geometry
#define SCT_ECC -2
#define SCT_BARREL 0
#define SCT_DISKS_N 9

InDetGlobalBackgroundMonTool::InDetGlobalBackgroundMonTool(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : InDetGlobalMotherMonTool(type, name, parent),
  m_EndCapDisks{nullptr},
  m_AsymDisks{nullptr},
  m_EndCapLB{nullptr},
  m_asymLB(nullptr),
  m_asymBCID(nullptr),
  m_hitBCID(nullptr),
  m_Mapping{nullptr},
  m_RadMap{nullptr} {
}

//---------------------------------------------------------
StatusCode InDetGlobalBackgroundMonTool::bookHistograms() {
  bool status = true;

  for (unsigned int asym_j = 0; asym_j < 2; asym_j++) {
    m_EndCapDisks[asym_j] = new TProfile(Form("EndCapDisks%s", (asym_j == 0 ? "AC" : "CA")),
                                         Form(
                                           "SPs vs. EC Disk in Events with beam %s;Disk; SP Density per Event (cm-2)",
                                           (asym_j == 0 ? "AC" : "CA")), 19, -9.5, 9.5);
    status &= regHist(m_EndCapDisks[asym_j], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();
  }

  for (unsigned int k = 0; k < int(int(SCT_DISKS_N) / 2); k++) {
    //Average over 5 Lumiblocks for increased statistics
    m_AsymDisks[k] = new TH1I(Form("AsymDisks%i%i", 2 * k + 1, 2 * k + 2),
                              Form("Asymmetry of Disks %i and %i;Z Asymmetry;Counts", 2 * k + 1,
                                   2 * k + 2), 201, -1.1, 1.1);
    status &= regHist(m_AsymDisks[k], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();
    m_EndCapLB[0][k] = new TProfile(Form("EndCapLB%i%iAC", 2 * k + 1, 2 * k + 2),
                                    Form(
                                      "SPs per Event per LB on Disks %i-%i (both ECs) with beam AC; LumiBlock;Counts per Event",
                                      2 * k + 1, 2 * k + 2), 600, 0.5, 3000.5);
    status &= regHist(m_EndCapLB[0][k], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();
    m_EndCapLB[1][k] = new TProfile(Form("EndCapLB%i%iCA", 2 * k + 1, 2 * k + 2),
                                    Form(
                                      "SPs per Event on Disks %i-%i (both ECs) with beam CA; LumiBlock;Counts per Event",
                                      2 * k + 1, 2 * k + 2), 600, 0.5, 3000.5);
    status &= regHist(m_EndCapLB[1][k], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();
  }

  //Average over 5 Lumiblocks for increased statistics
  m_asymLB = new TProfile("asymLB", "Fraction of Events with beam CA;LumiBlock;", 600, 0.5, 3000.5);
  status &= regHist(m_asymLB, "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();

  m_asymBCID =
    new TProfile("asymBCID", "Average z Asymmetry for disks 7-8 per BCID;BCID;Z asymmetry", 5000, 0.5, 5000.5);
  status &= regHist(m_asymBCID, "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();

  m_hitBCID = new TProfile("hitBCID", "SPs on Disks 7-8 per BCID;BCID;Counts", 5000, 0.5, 5000.5);
  status &= regHist(m_hitBCID, "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();

  for (unsigned int side_i = 0; side_i < 2; side_i++) {
    for (unsigned int disk_i = 0; disk_i < int(SCT_DISKS_N) * 2; disk_i++) {
      m_Mapping[side_i][disk_i] =
        new TH2I(Form("BIBmapping%s%i%s", (side_i == 0 ? "C" : "A"), (disk_i < 9 ? disk_i + 1 : disk_i - 8),
                      (disk_i < 9 ? "AC" : "CA")),
                 Form("Map EC %s disk %i with beam %s;x[mm];y[mm]",
                      (side_i == 0 ? "C" : "A"),
                      disk_i + 1, (disk_i < 9 ? "AC" : "CA")), 60, -600, 600, 60, -600, 600);
      status &= regHist(m_Mapping[side_i][disk_i], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();

      m_RadMap[side_i][disk_i] =
        new TH1F(Form("RadialMapping%s%i%s", (side_i == 0 ? "C" : "A"), (disk_i < 9 ? disk_i + 1 : disk_i - 8),
                      (disk_i < 9 ? "AC" : "CA")),
                 Form("Radial distribution EC %s disk %i with beam %s;Radius[mm];Space Point Density",
                      (side_i == 0 ? "C" : "A"), disk_i + 1, (disk_i < 9 ? "AC" : "CA")), 100, 250, 600);
      status &= regHist(m_RadMap[side_i][disk_i], "InDetGlobal/BackgroundNCB", run, ATTRIB_MANAGED).isSuccess();
    }
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode InDetGlobalBackgroundMonTool::fillHistograms() {
  const SpacePointContainer* sctSPContainer = 0;
  StatusCode sc = evtStore()->retrieve(sctSPContainer, "SCT_SpacePoints");

  if (msgLvl(MSG::DEBUG)) {
    if (sc.isFailure() || !sctSPContainer) {
      msg(MSG::DEBUG) << "No SCT SpacePoints in StoreGate " << endreq;
    } else {
      msg(MSG::DEBUG)
        << "found SCT SpacePoints in StoreGate " << endreq;
    }
  }

  //Counts in every disk in both endcaps. first parenthesis is the side: 0 is EC C, 1 is EC A
  //second parenthesis is the disk: from 1 to 9 are the disks, while 0 contains the total sum
  int nsp[2][10] = {};

  //Cycle to fill the nsp variable to calculate z asymmetry later. Necessary to identify beam direction
  for (SpacePointContainer::const_iterator it = sctSPContainer->begin(); it != sctSPContainer->end(); ++it) {
    const SpacePointCollection* colNext = &(**it);
    if (!colNext) continue;
    int thisBec = m_sctID->barrel_ec(colNext->identify());
    if (thisBec == SCT_BARREL) continue; //discards barrel space points
    int thisLayerDisk = m_sctID->layer_disk(colNext->identify());
    if (thisLayerDisk >= 0 && thisLayerDisk <= SCT_DISKS_N) {
      unsigned int side = (thisBec == SCT_ECC ? 0 : 1);
      //adds the number of sp in every module to the respective endcap/disk
      nsp[side][0] += colNext->size();
      nsp[side][thisLayerDisk + 1] += colNext->size();
    }
  }

  double asym78 = 0;

  // Calculates z asymmetry for disks 7 and 8
  if ((nsp[1][7] + nsp[1][8] + nsp[0][7] + nsp[0][8]) != 0) {
    asym78 = (nsp[1][7] + nsp[1][8] - nsp[0][7] - nsp[0][8] + 0.0) /
             (nsp[1][7] + nsp[1][8] + nsp[0][7] + nsp[0][8]);
  }

  //Extract the BCID
  unsigned int BCID = 0;
  InDetTimeCollection::const_iterator it_time, it_time_E(m_sctBCIDContainer->end());
  for (it_time = m_sctBCIDContainer->begin(); it_time != it_time_E; ++it_time) {
    if (!(*it_time)) continue;
    BCID = (*it_time)->second;
  }

  //Fills asymmetry plots independently from the asymmetry cut
  for (unsigned int k = 0; k < int(int(SCT_DISKS_N) / 2); k++) {
    m_AsymDisks[k]->Fill((nsp[1][2 * k + 1] + nsp[1][2 * k + 2] - nsp[0][2 * k + 1] - nsp[0][2 * k + 2] + 0.0) /
                         (nsp[1][2 * k + 1] + nsp[1][2 * k + 2] + nsp[0][2 * k + 1] + nsp[0][2 * k + 2]));
  }
  m_asymBCID->Fill(BCID, asym78);

  //defines 3 possibilities for the plots
  //0 is beam AC, 1 beam CA, -1 unidentifiable beam: no plots in this case
  int asymsign = -1;
  if (asym78 < -0.5) {
    asymsign = 0; //beam AC
  } else if (asym78 > 0.5) {
    asymsign = 1; //beamCA
  }

  if (asymsign != -1) {
    for (SpacePointContainer::const_iterator it2 = sctSPContainer->begin(); it2 != sctSPContainer->end(); ++it2) {
      const SpacePointCollection* colNext = &(**it2);
      if (!colNext) continue;
      int thisBec = m_sctID->barrel_ec(colNext->identify());
      int thisLayerDisk = m_sctID->layer_disk(colNext->identify());
      if (thisBec == SCT_BARREL) continue; //discards barrel space points
      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint = (*it2)->begin();
      DataVector<Trk::SpacePoint>::const_iterator nextSpacePointEnd = (*it2)->end();
      for (; nextSpacePoint != nextSpacePointEnd; ++nextSpacePoint) {
        const Trk::SpacePoint& sp = **nextSpacePoint;
        //filling the 2D maps and radial distribution
        if (thisLayerDisk >= 0 && thisLayerDisk <= int(SCT_DISKS_N) - 1) {
          unsigned int side = (thisBec == SCT_ECC ? 0 : 1);
          if (asymsign == 0) {
            m_Mapping[side][thisLayerDisk]->Fill(sp.globalPosition().x(), sp.globalPosition().y());
            // For the area density in every bin in radial coordinates, considering the binning infinitesimal,
            // one must multiply the number of hits per 1/2*pi*R*DeltaRbin. The latter being the length of one bin
            m_RadMap[side][thisLayerDisk]->Fill(sp.globalPosition().perp(),
                                                1.0 / (2 * 3.14 * sp.globalPosition().perp() * (600 - 250) / 100));
          }

          if (asymsign == 1) {
            m_Mapping[side][thisLayerDisk + SCT_DISKS_N]->Fill(sp.globalPosition().x(), sp.globalPosition().y());
            m_RadMap[side][thisLayerDisk + SCT_DISKS_N]->Fill(sp.globalPosition().perp(),
                                                1.0 / (2 * 3.14 * sp.globalPosition().perp() * (600 - 250) / 100));
          }
        }
      }
    }

    //areas of SCT endcap disks for normalization
    static const double scale[9] = {
      1 / 6268.3, 1 / 7472.4, 1 / 7472.4, 1 / 7472.4, 1 / 7472.4, 1 / 7472.4, 1 / 6268.3, 1 / 4620.1, 1 / 3801.1
    };
    //Fills the space points area density vs. disk number plots
    for (unsigned int ii = 1; ii <= int(SCT_DISKS_N); ii++) {
      m_EndCapDisks[asymsign]->Fill(int(-ii), nsp[0][ii] * scale[ii - 1]);//int needed to have a negative value from unsigned
      m_EndCapDisks[asymsign]->Fill(ii, nsp[1][ii] * scale[ii - 1]);
    }
    //Fills the space points vs. LB plots
    for (unsigned int k = 0; k < int(int(SCT_DISKS_N) / 2); k++) {
      m_EndCapLB[asymsign][k]->Fill(m_manager->lumiBlockNumber(),
                                    nsp[1][2 * k + 1] + nsp[1][2 * k + 2] + nsp[0][2 * k + 1] + nsp[0][2 * k + 2]);
    }
    //Plot of fraction of events with beam CA vs. LB and number of hits vs. BCID plots
    m_asymLB->Fill(m_manager->lumiBlockNumber(), asymsign);
    m_hitBCID->Fill(BCID, nsp[0][7] + nsp[0][8] + nsp[1][7] + nsp[1][8]);
  }


  return StatusCode::SUCCESS;
}
