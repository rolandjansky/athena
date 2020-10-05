/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_TGCRECROISVC_H
#define TRIGT1_TGCRECROISVC_H

#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

class ITGCcablingSvc;

namespace MuonGM
{
  class MuonDetectorManager;
}

namespace LVL1TGC {

  /**
   *  $Date: 2009-04-30 03:58:46 $
   *  @short Dump LVL1TGC::TGCRecRoiSvc for main algorithm
   *  
   *  The package provides functions to get (eta, phi) of RoI center and
   *  also the maximum and minimum eta/phi for a given RoI word.
   *
   *  @author Hisaya Kurashige <Hisaya.Kurashige@cern.ch>
   *  @author Chihiro Omachi <omati@phys.sci.kobe-u.ac.jp>
   *  @author Yoji Hasegawa <Yoji.Hasegawa@cern.ch>
   *  @author Naoko Kanaya <Naoko.Kanaya@cern.ch>
   *
   *  @version $Revision: 1.15 $
   * 
   */
     
class TGCRecRoiSvc : public LVL1::RecMuonRoiSvc
{
public:
  // standard constructor
  TGCRecRoiSvc (const std::string& name, ISvcLocator* svc);
  
  virtual ~TGCRecRoiSvc()=default;

  StatusCode initialize();
  
  void reconstruct (const unsigned int & roIWord) const;  //!< calculate eta and phi of RoI center @link LVL1TGC::TGCRecRoiSvc::reconstruct @endlink

  void RoIsize(const unsigned int & roIWord,
	       double & etaMin, double & etaMax, double & phiMin, double & phiMax) const; //!< calculate RoI edges @link LVL1TGC::TGCRecRoiSvc::RoIsize @endlink

  void print(const unsigned int & roIWord);

  double phi (void) const {return m_phi;} //! get eta of RoI center @link LVL1TGC::TGCRecRoiSvc::eta @endlink
  double eta (void) const {return m_eta;} //! get phi of RoI center @link LVL1TGC::TGCRecRoiSvc::phi @endlink

  Identifier channelId_phi (void) const {return m_channelId_phi;}
  Identifier channelId_eta (void) const {return m_channelId_eta;}

private:
  enum EdgeType {NonEdge=-1,
                 LowerREdge,
                 UpperREdge,
                 LowerPhiEdge,
                 UpperPhiEdge};
  enum DummyOffset {DummyOffset=-9999};

  bool getSLBparameters(const unsigned int & roIWord,
			TGCIdBase::SideType & sideType,
			TGCIdBase::RegionType & regionType,
			unsigned int & sectorID,
			unsigned int & roiNumber,
			unsigned int & r, 
			unsigned int & phi,
			int & wireSLBId, 
			int & block ) const;
  
  void getWireASDOut(TGCIdBase* & w_asdout,
		     TGCIdBase::SideType sideType,
		     TGCIdBase::RegionType regionType,
		     unsigned int sectorID,
		     int wireSLBId,
		     int block,
		     int phi ,
		     int offset) const;
  
  void getStripASDOut(TGCIdBase* & s_asdout,
		      TGCIdBase::SideType sideType,
		      TGCIdBase::RegionType regionType,
		      unsigned int sectorID,
		      int wireSLBId,
		      int block,
		      int phi,
		      int wireOffset,
		      int stripOffset=DummyOffset) const;

  void getWireInfo(Amg::Vector3D & w_pos,
		   Identifier & wireId,
		   TGCIdBase * w_asdout,
		   EdgeType edge=NonEdge) const;

  void getStripInfo(Amg::Vector3D & s_pos,
		    Identifier & stripId,
		    TGCIdBase * s_asdout,
		    EdgeType edge=NonEdge) const;

  StatusCode getCabling() const;

  //


  //
  mutable double m_phi, m_eta;

  mutable Identifier m_channelId_phi, m_channelId_eta;

  // Properties
  BooleanProperty m_patchForM5; 
  BooleanProperty m_patchForP4; 
  BooleanProperty m_patchForP5; 
  BooleanProperty m_patchForRoIWord;

  mutable const ITGCcablingSvc* m_cabling;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  const MuonGM::MuonDetectorManager* m_muonMgr;

  mutable bool m_isAtlas;
};

} // end of namespace

#endif 
