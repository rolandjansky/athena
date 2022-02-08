/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "AthContainers/DataVector.h"
#include "InDetAlignTrkInfo/AlignSiModule.h"
#include "InDetAlignTrkInfo/AlignTrk.h"
#include "InDetAlignTrkInfo/AlignTrkContainer.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetAlignTrkInfo/AlignSiHit.h"
#include "InDetAlignGenTools/InDetAlignOverlapTool.h"
#include "InDetAlignGenTools/AlignSiModuleList.h"


using namespace std;


InDetAlignOverlapTool::InDetAlignOverlapTool(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_Overlaphits{}
  , p_modlist{} {
  declareInterface<IInDetAlignOverlapTool>(this);
}

InDetAlignOverlapTool::~InDetAlignOverlapTool() {}

StatusCode InDetAlignOverlapTool::initialize() {
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;

  // get DetectorStore service
  ATH_CHECK(detStore().retrieve());
  // get pointer to AlignSiModuleList; after setup has created it
  ATH_CHECK(detStore()->retrieve(p_modlist, "InDetAlignNt"));
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode InDetAlignOverlapTool::finalize() {
  ATH_MSG_DEBUG("finalize() successful in " << name());
  return AlgTool::finalize();
}

//** This function give us the number of overlaps in pixel detector

int InDetAlignOverlapTool::getNumberOverlapPIX(const AlignTrk& trk) const {
  ATH_MSG_DEBUG(" InDetAlignOverlapTool::get getNumberOverlapPIX inizialized");

  int nPixelBarrelOverlapEta = 0;
  int nPixelBarrelOverlapPhi = 0;
  int nPixelECOverlapPhi = 0;
  int PixelSector[3] = {
    21, 37, 51
  };
  int ECPixelSector = 48;
  int nOverlapPIX = 0;
  int nhits2 = 0;

  //--------------------
  // first loop:
  //--------------------

  nhits2 = trk.nhits();
  ATH_MSG_DEBUG(" Num de Hits " << nhits2);
  ATH_MSG_DEBUG(" Anem a entrar en el loop");

  for (std::vector<AlignSiHit>::const_iterator hit = trk.hitlist_cbegin();
       hit != trk.hitlist_cend(); ++hit) {
    int index = hit->index() - 1; // note index starts at 1 in ntuple!

    if (msgLvl(MSG::VERBOSE)) {
      if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[index]).dettype() == 1) {
        msg().setColor(MSG::GREEN);
        msg(MSG::VERBOSE) << "Index " << index << " Detector: " << (p_modlist->vec[index]).dettype() << " layer : " <<
        (p_modlist->vec[index]).layer() << " ring : "
                          << (p_modlist->vec[index]).ring() << " sector : " << (p_modlist->vec[index]).sector() <<
        endmsg;
      }
    }

    //--------------------
    // second loop:
    //--------------------
    for (std::vector<AlignSiHit>::const_iterator Allhit = hit;
         Allhit != trk.hitlist_cend(); ++Allhit) {
      int Allindex = Allhit->index() - 1; // note index starts at 1 in ntuple!

      if (msgLvl(MSG::VERBOSE)) {
        if ((p_modlist->vec[Allindex]).bec() == 0 && (p_modlist->vec[Allindex]).dettype() == 1) {
          msg().setColor(MSG::BLUE);
          msg(MSG::VERBOSE) << "Index " << Allindex << endmsg;
        }
      }

      //------------------------//
      //      PIXEL SYSTEM      //
      //------------------------//

      if ((p_modlist->vec[index]).dettype() == 1 && (p_modlist->vec[Allindex]).dettype() == 1) {
        if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[Allindex]).bec() == 0) { // BARREL
          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[Allindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[Allindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[Allindex]).sector() -
                    ((p_modlist->vec[index]).sector() + PixelSector[(p_modlist->vec[index]).layer()])) == 1.)) {
            // msg()
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;//<<"rPhi Residuals: " << hit->rphi_resid() <<"
                                                                         // eta Residuals: " << hit->z_resid());
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;//<<" rPhi Residuals: " <<
                                                                            // Allhit->rphi_resid() <<"  eta Residuals:
                                                                            // " << Allhit->z_resid());
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << "Pixel Overlap in Phi found" << endmsg;
            } //msg() end

            nPixelBarrelOverlapPhi++;
          }

          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[Allindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[Allindex]).sector() == (p_modlist->vec[index]).sector()) {
            //msg()
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;//<<"rPhi Residuals: " << hit->rphi_resid() <<"
                                                                         // eta Residuals: " << hit->z_resid());
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;// <<" rPhi Residuals: " <<
                                                                            // Allhit->rphi_resid() <<"  eta Residuals:
                                                                            // " << Allhit->z_resid());
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << "Pixel Overlap in Eta found" << endmsg;
            } //msg() end

            nPixelBarrelOverlapEta++;
          }
        }  // END OF BARREL

        if ((p_modlist->vec[index]).bec() != 0 && (p_modlist->vec[index]).bec() == (p_modlist->vec[Allindex]).bec()) { // ENDCAP
          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[Allindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[Allindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[Allindex]).sector() - ((p_modlist->vec[index]).sector() + ECPixelSector)) == 1.)) {
            //msg()
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;
              msg(MSG::DEBUG) << "EC Pixel Overlap in Phi found" << endmsg;
            }// msg() end

            nPixelECOverlapPhi++;
          }
        } // END OF ENDCAP
      }
    }
  }

  nOverlapPIX = nPixelBarrelOverlapPhi + nPixelBarrelOverlapEta + nPixelECOverlapPhi;
  ATH_MSG_DEBUG(" Total number of Pixel overlaps in eta " << nPixelBarrelOverlapEta);
  ATH_MSG_DEBUG(" Total number of Pixel overlaps in phi " << nPixelBarrelOverlapPhi);
  ATH_MSG_DEBUG(" Total number of Pixel EC overlaps in phi " << nPixelECOverlapPhi);

  return nOverlapPIX;
}

//** This function give us the number of overlaps in pixel detector.
//** The structure is the same that in the pixel case ( differs in dettype and number of sectors, etc...

int InDetAlignOverlapTool::getNumberOverlapSCT(const AlignTrk& trk) const {
  ATH_MSG_DEBUG(" InDetAlignOverlapTool::get getNumberOverlapSCT initialized");

  // counters for overlaps eta and overlaps Phi
  int nSCTBarrelOverlapEta = 0;
  int nSCTBarrelOverlapPhi = 0;
  int nSCTECOverlapPhi = 0;
  int nSCTECOverlapEta = 0;

  int SCTSector[4] = {
    31, 39, 47, 55
  };
  int ECSCTSector = 52;

  int nOverlapSCT = 0;
  int nhits2 = 0;

  nhits2 = trk.nhits();
  ATH_MSG_DEBUG(" Num de Hits " << nhits2);
  ATH_MSG_DEBUG(" Anem a entrar en el loop");

  //-------------
  // first loop:
  //-------------

  for (std::vector<AlignSiHit>::const_iterator hit = trk.hitlist_cbegin();
       hit != trk.hitlist_cend(); ++hit) {
    int index = hit->index() - 1; // note index starts at 1 in ntuple!

    if (msgLvl(MSG::VERBOSE)) {
      if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[index]).dettype() == 1) {
        msg().setColor(MSG::GREEN);
        msg(MSG::VERBOSE) << "Index " << index << " Detector: " << (p_modlist->vec[index]).dettype() << " layer : " <<
        (p_modlist->vec[index]).layer() << " ring : "
                          << (p_modlist->vec[index]).ring() << " sector : " << (p_modlist->vec[index]).sector() <<
        endmsg;
      }
    }

    //-------------
    // second loop:
    //-------------

    for (std::vector<AlignSiHit>::const_iterator Allhit = hit;
         Allhit != trk.hitlist_cend(); ++Allhit) {
      int Allindex = Allhit->index() - 1; // note index starts at 1 in ntuple!

      if (msgLvl(MSG::VERBOSE)) {
        if ((p_modlist->vec[Allindex]).bec() == 0 && (p_modlist->vec[Allindex]).dettype() == 1) {
          msg().setColor(MSG::BLUE);
          msg(MSG::VERBOSE) << "Index " << Allindex << endmsg;
        }
      }

      //------------------------//
      //      SCT SYSTEM      //
      //------------------------//

      if ((p_modlist->vec[index]).dettype() == 2 && (p_modlist->vec[Allindex]).dettype() == 2) {
        if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[Allindex]).bec() == 0) { // BARREL
          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[Allindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[Allindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[Allindex]).sector() -
                    ((p_modlist->vec[index]).sector() + SCTSector[(p_modlist->vec[index]).layer()])) == 1.)) {
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << "SCT Overlap in Phi found" << endmsg;
            }
            nSCTBarrelOverlapPhi++;
          }

          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[Allindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[Allindex]).sector() == (p_modlist->vec[index]).sector()) {
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << "SCT Overlap in Eta found" << endmsg;
            }
            nSCTBarrelOverlapEta++;
          }
        }

        if ((p_modlist->vec[index]).bec() != 0 && ((p_modlist->vec[index]).bec() == (p_modlist->vec[Allindex]).bec())) { // EndCap
                                                                                                                         // (
                                                                                                                         // EC
                                                                                                                         // values
                                                                                                                         // 2
                                                                                                                         // i
                                                                                                                         // -2)
          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[Allindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[Allindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[Allindex]).sector() - ((p_modlist->vec[index]).sector() + ECSCTSector)) == 1.)) {
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;
              msg().setColor(MSG::RED);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;

              msg(MSG::DEBUG) << "EC SCT Overlap in Phi found" << endmsg;
            }
            nSCTECOverlapPhi++;
          }

          if ((p_modlist->vec[Allindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[Allindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[Allindex]).sector() == (p_modlist->vec[index]).sector()) {
            if (msgLvl(MSG::DEBUG)) {
              msg().setColor(MSG::GREEN);
              msg(MSG::DEBUG) << " Index:" << index << " layer " << (p_modlist->vec[index]).layer() << " ring " <<
              (p_modlist->vec[index]).ring() <<
                " sector " << (p_modlist->vec[index]).sector() << endmsg;
              msg().setColor(MSG::GREEN);
              msg(MSG::DEBUG) << " AllIndex:" << Allindex << " layer " << (p_modlist->vec[Allindex]).layer() <<
              " ring " << (p_modlist->vec[Allindex]).ring() <<
                " sector " << (p_modlist->vec[Allindex]).sector() << endmsg;

              msg(MSG::DEBUG) << "EC SCT Overlap in Eta found" << endmsg;
            }
            nSCTECOverlapEta++;
          }
        }
      }
    }
  }

  nOverlapSCT = nSCTBarrelOverlapPhi + nSCTBarrelOverlapEta + nSCTECOverlapPhi + nSCTECOverlapEta;

  ATH_MSG_DEBUG(" Total number of STC overlaps in eta " << nSCTBarrelOverlapEta);
  ATH_MSG_DEBUG(" Total number of SCT overlaps in phi " << nSCTBarrelOverlapPhi);
  ATH_MSG_DEBUG(" Total number of STC EC overlaps in phi " << nSCTECOverlapPhi);
  ATH_MSG_DEBUG(" Total number of STC EC overlaps in Eta " << nSCTECOverlapEta);

  return nOverlapSCT;
}

//** This fuctions give a DataVector<AlignSiHit> with overlap hits in aligntrk
//** Now we take into account all hits in 2 loops

std::vector<AlignSiHit> InDetAlignOverlapTool::getOverlapHit(const AlignTrk& trk) { //const {
  ATH_MSG_DEBUG(" InDetAlignOverlapTool::get getOverlapHit inizialized");

  bool isPIXOverlap = false;
  bool isSCTOverlap = false;
  //bool isOverlap=false;

  int PixelSector[3] = {
    21, 37, 51
  };
  int ECPixelSector = 48;
  int SCTSector[4] = {
    31, 39, 47, 55
  };
  int ECSCTSector = 52;

  //----------------------------------------------------
  // first loop: preparing info for next processing
  //----------------------------------------------------
  for (std::vector<AlignSiHit>::const_iterator hit = trk.hitlist_cbegin();
       hit != trk.hitlist_cend(); ++hit) {
    int index = hit->index() - 1; // note index starts at 1 in ntuple!

    isPIXOverlap = false;
    isSCTOverlap = false;
    //isOverlap=false;

    //----------------------------------------------------
    // Second loop: preparing info for next processing
    //----------------------------------------------------
    for (std::vector<AlignSiHit>::const_iterator hit2 = trk.hitlist_cbegin();
         hit2 != trk.hitlist_cend(); ++hit2) {
      int newindex = hit2->index() - 1; // note index starts at 1 in ntuple!
      //------------------------//
      //      PIXEL SYSTEM      //
      //------------------------//

      if ((p_modlist->vec[index]).dettype() == 1 && (p_modlist->vec[newindex]).dettype() == 1) {
        if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[newindex]).bec() == 0) {               // BARREL
          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[newindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[newindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[newindex]).sector() -
                    ((p_modlist->vec[index]).sector() + PixelSector[(p_modlist->vec[index]).layer()])) == 1.)) {
            isPIXOverlap = true;
          }

          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[newindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[newindex]).sector() == (p_modlist->vec[index]).sector()) {
            isPIXOverlap = true;
          }
        }

        if ((p_modlist->vec[index]).bec() != 0 && (p_modlist->vec[index]).bec() == (p_modlist->vec[newindex]).bec()) { // ENDCAP
          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[newindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[newindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[newindex]).sector() - ((p_modlist->vec[index]).sector() + ECPixelSector)) == 1.)) {
            isPIXOverlap = true;
          }
        }
      }  // End Pixel system


      //------------------------//
      //      SCT  SYSTEM      //
      //------------------------//

      if ((p_modlist->vec[index]).dettype() == 2 && (p_modlist->vec[newindex]).dettype() == 2) {
        if ((p_modlist->vec[index]).bec() == 0 && (p_modlist->vec[newindex]).bec() == 0) { // BARREL
          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[newindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[newindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[newindex]).sector() -
                    ((p_modlist->vec[index]).sector() + SCTSector[(p_modlist->vec[index]).layer()])) == 1.)) {
            isSCTOverlap = true;
          }

          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[newindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[newindex]).sector() == (p_modlist->vec[index]).sector()) {
            isSCTOverlap = true;
          }
        }

        if ((p_modlist->vec[index]).bec() != 0 && ((p_modlist->vec[index]).bec() == (p_modlist->vec[newindex]).bec())) { //  //
                                                                                                                         // ENDCAP
                                                                                                                         // (
                                                                                                                         // EC
                                                                                                                         // values
                                                                                                                         // 2
                                                                                                                         // i
                                                                                                                         // -2)
          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              (p_modlist->vec[newindex]).ring() == (p_modlist->vec[index]).ring() &&
              (std::abs((p_modlist->vec[newindex]).sector() - (p_modlist->vec[index]).sector()) == 1. ||
               std::abs((p_modlist->vec[newindex]).sector() - ((p_modlist->vec[index]).sector() + ECSCTSector)) == 1.)) {
            isSCTOverlap = true;
          }

          if ((p_modlist->vec[newindex]).layer() == (p_modlist->vec[index]).layer() &&
              std::abs((p_modlist->vec[newindex]).ring() - (p_modlist->vec[index]).ring()) == 1 &&
              (p_modlist->vec[newindex]).sector() == (p_modlist->vec[index]).sector()) {
            isSCTOverlap = true;
          }
        }  // End SCT system
      }
    }
    if (isPIXOverlap || isSCTOverlap) {
      msg().setColor(MSG::GREEN);
      ATH_MSG_DEBUG("This Hit is overlap" << index);
      m_Overlaphits.push_back(*hit);
    }
  }

  return m_Overlaphits;
}
