/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena basic includes
#include "GaudiKernel/MsgStream.h"

// local includes
#include "sTGCenumeration.h"
#include "sTGCgeometry.h"
#include "PadOfflineData.h"

#include "TMath.h"
#include "TRotation.h"
#include "TVector3.h"

#include <utility>
#include <cassert>

#include "PadUtil.h"

namespace NSWL1 {

  //-------------------------------------
  bool wedgeidIsInvalid(int wedgeid, MsgStream& msg){
    bool invalid(wedgeid!=LARGE && wedgeid!=SMALL);
    if(invalid && msg.level()<=MSG::VERBOSE )
      msg << MSG::VERBOSE << "PadUtil::wedgeIsInvalid: invalid wedgeid " << wedgeid << endmsg;
    return invalid;
  }
  //-------------------------------------
  bool wedgetypeIsInvalid(int wedgetype, MsgStream& msg){
    bool invalid(wedgetype!=PIVOT && wedgetype!=CONFIRM);
    if(invalid && msg.level()<=MSG::VERBOSE)
      msg << MSG::VERBOSE << "PadUtil::wedgeTypeIsInvalid: invalid wedgetype " << wedgetype << endmsg;
    return invalid;
  }
  //-------------------------------------
  bool layerIsInvalid(int layer, MsgStream& msg){
    bool invalid(layer<1 || layer>4);
    if(invalid && msg.level()<=MSG::VERBOSE)
      msg << MSG::VERBOSE << "PadUtil::layerIsInvalid: invalid layer " << layer << endmsg;
    return invalid;
  }
  //-------------------------------------
  bool sectorIsInvalid(int sector, MsgStream& msg){
    bool invalid(sector<1 || sector>16);
    if(invalid && msg.level()<=MSG::VERBOSE)
      msg << MSG::VERBOSE << "PadUtil::sectorIsInvalid: invalid sector " << sector << endmsg;
    return invalid;
  }
  //-------------------------------------
  bool detectorNumberIsInvalid(int dn, MsgStream& msg){
    bool invalid(dn<0 || dn >STGC_DETECTOR_3);
    if(invalid && msg.level()<=MSG::VERBOSE)
      msg << MSG::VERBOSE << "PadUtil::detectorNumberIsInvalid: invalid detectorNumber " << dn << endmsg;
    return invalid;
  }
  //-------------------------------------
  int sideFromZ(float z) { return (z>0.0 ? 0:1); }
  //-------------------------------------
  float midSectorPhi(int sector, MsgStream& msg){
    float nSectors=16.0;
    float sectorDphi(TMath::TwoPi()/nSectors);
    if(sector<1 && msg.level()<=MSG::DEBUG) msg << MSG::DEBUG << "sector " << sector << std::endl;
    assert(sector>0); // we are assuming that the sector N starts from 1
    return (sector-1)*sectorDphi;
  }
  //-------------------------------------
  float adjustHforZdifference(const float &h, const STGCType &s, const int &l)
  {
    return h * Z_CURRENT_LAYOUT[s][l] / Z_DANIEL[s][l];
  }


  bool determinePad(int layer,
                    int wedgeId,
                    int wedgeType,
                    int sector,
                    int detectorNumber,
                    TVector3 pos,
                    std::pair<int,int>& result,
                    MsgStream& msg)
  {
    if(layerIsInvalid         (layer,          msg)) return false;
    if(sectorIsInvalid        (sector,         msg)) return false;
    if(detectorNumberIsInvalid(detectorNumber, msg)) return false;
    int la(layer-1); // layer is in [1,4], but indices start from 0
    int wi(wedgeId), wt(wedgeType), sn(sector), dn(detectorNumber);
    // ==> used in the previos result  int side( sideFromZ(pos.Z()) );
    STGCType st(STGC_TYPES);
    // ==> used in the previos result  STGCDetectorType sdt(STGC_DETECTORS);
    LayerType lt(NO_LAYER);

    //using namespace std;
    // some conversion enum<-->int (should be implemented with the enum)
    if     (LARGE == wi && PIVOT   == wt) st = LARGE_PIVOT;
    else if(LARGE == wi && CONFIRM == wt) st = LARGE_CONFIRM;
    else if(SMALL == wi && PIVOT   == wt) st = SMALL_PIVOT;
    else if(SMALL == wi && CONFIRM == wt) st = SMALL_CONFIRM;
    else {
      if (msg.level()<=MSG::VERBOSE)
        msg << MSG::VERBOSE << "determinePad: invalid L/S (" << wi << "), P/C (" << wt << ") values " << endmsg;
      return false;
    }

    switch(la) {
    case 0: lt = STGC_LAYER_1; break;
    case 1: lt = STGC_LAYER_2; break;
    case 2: lt = STGC_LAYER_3; break;
    case 3: lt = STGC_LAYER_4; break;
    default :
      if( msg.level()<=MSG::VERBOSE) msg << MSG::VERBOSE << "determinePad: invalid layer index " << la << endmsg;
      return false;
    }
    (void) lt; // unused variable
    bool isInnerDetector(dn==STGC_DETECTOR_0 || dn==STGC_DETECTOR_1);
    float phiPadSize(PAD_PHI_DIVISION);
    if(isInnerDetector) phiPadSize /= PAD_PHI_SUBDIVISION; // inner dets have finer pads
        float phiOrigin(PAD_COL_PHI0[st][dn][la]);
        float hLowEdge(H_PAD_ROW_0[st][la]);
    float padHeight(PAD_HEIGHT[st][la]);
    //-- int leftmostCol(INDEX_LEFTMOST_COL[st][dn][la]); // see comment a few lines below
    //-- int rightmostCol(INDEX_RIGHTMOST_COL[st][dn][la]);
    float phiCenterSector(midSectorPhi(sn,msg));
    float phiSectorFive(midSectorPhi(5,msg)); // sector 5 is the one aligned with the ATLAS y axis
    pos.RotateZ(phiSectorFive - phiCenterSector);
    hLowEdge  = adjustHforZdifference(hLowEdge,  st, la);
    padHeight = adjustHforZdifference(padHeight, st, la);
    int ieta = TMath::Floor((pos.Y() - hLowEdge) / padHeight);

    int iphi = TMath::Floor((pos.Phi() - phiOrigin - phiSectorFive) / phiPadSize);

    float loHei(hLowEdge + ieta*padHeight);
    float hiHei(loHei + padHeight);
    float loPhi(phiOrigin + phiSectorFive + iphi*phiPadSize);
    float hiPhi(loPhi + phiPadSize);
    if((loHei > pos.Y()) || (pos.Y() > hiHei) || (loPhi > pos.Phi()) || (pos.Phi() > hiPhi))
      if (msg.level()<=MSG::DEBUG)
        msg << MSG::DEBUG << "determinePad: Something wrong with pad edges? " // here we should assert, but there's 2pi ambiguity
            << "height : " << loHei << " < " << pos.Y()   << " < " << hiHei << std::endl
            << "phi    : " << loPhi << " < " << pos.Phi() << " < " << hiPhi << std::endl
            << "loPhi=("   << phiOrigin << " + " << phiSectorFive << " + " << iphi << "*" << phiPadSize << ")" << std::endl
            << "phiOrigin(PAD_COL_PHI0[" << st << "][" << dn << "][" << la << "]) : " << PAD_COL_PHI0[st][dn][la] << std::endl
        ;
    // DG not sure whether we should use this adjustment...ask Shikma
    //-- if     (iphi < 0 && iphi < leftmostCol ) iphi = leftmostCol;
    //-- else if(iphi > 0 && iphi > rightmostCol) iphi = rightmostCol;


    if(msg.level()<=MSG::VERBOSE)
      msg << MSG::VERBOSE << "determine pad (" << pos.X() << ", " << pos.Y() << ", " << pos.Z() << ")"
          << " sector " << sn << " layer " << (la+1)
          << " (" << ieta << "," << iphi << ")" << endmsg;

    result.first  = ieta;
    result.second = iphi;
    //result.ieta   = ieta;
    //result.iphi   = iphi;
    //result.det    = sdt;
    //result.stgc   = st;
    //result.layer  = lt;
    //result.sector = sn;
    //result.side   = side;
    //result.setLowPhi(loPhi).setHighPhi(hiPhi).setLowH(loHei).setHighH(hiHei).fillCornerCoords(pos.Z(), phiSectorFive - phiCenterSector);
    return true;
  }

bool determine_pad_indices_with_old_algo(const PadOfflineData& pod, const Amg::Vector3D &global_pos,
                                         std::pair<int,int>& result, MsgStream& msg)

{
    TVector3 pos(global_pos.x(), global_pos.y(), global_pos.z());

    // NEW to OLD nomencleture
    int layer          = pod.gasGapId();
    int wedgeId        = pod.sectorType();
    int wedgeType      = -1;
    if ( wedgeId == 0 ) wedgeType = pod.multipletId() == 1 ? 0 : 1; // SMALL P(0):C(1)
    else                wedgeType = pod.multipletId() == 1 ? 1 : 0; // LARGE C(1):P(0)
    int sectorNumber   = pod.sectorId();
    int detectorNumber = pod.moduleId();

    return determinePad(layer,
                        wedgeId,
                        wedgeType,
                        sectorNumber,
                        detectorNumber,
                        pos,
                        result,
                        msg);
}

} // end of namesape
