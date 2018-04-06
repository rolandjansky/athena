/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/FTKSplitEncoder.h"
#include "TrigFTKSim/FTKSetup.h"
// needed to read SCTtrk tracks (Constantinos case)
#include "TrigFTKSim/FTKTrackInput.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTKRoadInput.h"
#include "TrigFTKSim/FTKTrack.h"

#include <fstream>
#include <cmath> // needed for floor()
using namespace std;

FTKRawHit::FTKRawHit()
  : TObject(), m_truth(nullptr), m_channels()
{
  // nothing to do
  reset();
}

/* Special constructor for SCTtrk hits*/
FTKRawHit::FTKRawHit(const FTKTrack* trk, int) :
  TObject(), m_truth(nullptr), m_channels()
{
  reset();
  m_x = trk->getPhi();
  m_y = trk->getCotTheta();
  m_z = trk->getHalfInvPt();
  m_hitType = ftk::SCTtrk;
  m_moduleType = ftk::MODULETYPE_INVALID; // set to invalid until it is correctly set
  m_layer_disk = 0; // for the scttrk case
  // store information about 8L SCT road and track into unused variables:
  setBankID(trk->getBankID());
  setRoadID(trk->getRoadID());
  setTrackID(trk->getTrackID());
  // dummy truth, will be replaced by 8L truth
  m_truth = new MultiTruth();
}

FTKRawHit::FTKRawHit(const FTKRawHit &cpy) :
  TObject(cpy),
  m_x(cpy.m_x), m_y(cpy.m_y), m_z(cpy.m_z),
  m_hitType(cpy.m_hitType),
  m_moduleType(cpy.m_moduleType),
  m_IdentifierHash(cpy.m_IdentifierHash),
  m_barrel_ec(cpy.m_barrel_ec),
  m_layer_disk(cpy.m_layer_disk),
  m_phi_module(cpy.m_phi_module),
  m_eta_module(cpy.m_eta_module),
  m_pi_side(cpy.m_pi_side),
  m_ei_strip(cpy.m_ei_strip),
  m_n_strips(cpy.m_n_strips),
  m_etaWidth(cpy.m_etaWidth),
  m_phiWidth(cpy.m_phiWidth),
  m_dPhi(cpy.m_dPhi),
  m_dEta(cpy.m_dEta),
  m_hw_word(cpy.m_hw_word),
  m_includesGangedHits(cpy.m_includesGangedHits),
  m_eventindex(cpy.m_eventindex),
  m_barcode(cpy.m_barcode),
  m_barcode_pt(cpy.m_barcode_pt),
  m_parentage_mask(cpy.m_parentage_mask),
  m_channels(cpy.m_channels)
{
  if (cpy.m_truth)
    m_truth = new MultiTruth(*(cpy.m_truth));
  else
    m_truth = nullptr;
}

FTKRawHit::~FTKRawHit()
{
  if (m_truth) delete m_truth;
}

// shallow copy - MultiTruth did not used to be copied, but is now!
FTKRawHit& FTKRawHit::operator=(const FTKRawHit &cpy)
{
#ifdef PROTECT_SA // speedup
  if (this != &cpy)
#endif
    {
      m_x = cpy.m_x; m_y = cpy.m_y; m_z = cpy.m_z;
      m_hitType = cpy.m_hitType;
      m_moduleType = cpy.m_moduleType;
      m_IdentifierHash = cpy.m_IdentifierHash;
      m_barrel_ec = cpy.m_barrel_ec;
      m_layer_disk = cpy.m_layer_disk;
      m_phi_module = cpy.m_phi_module;
      m_eta_module = cpy.m_eta_module;
      m_pi_side = cpy.m_pi_side;
      m_ei_strip = cpy.m_ei_strip;
      m_n_strips = cpy.m_n_strips;
      m_etaWidth = cpy.m_etaWidth;
      m_phiWidth = cpy.m_phiWidth;
      m_dPhi = cpy.m_dPhi;
      m_dEta = cpy.m_dEta;
      m_includesGangedHits = cpy.m_includesGangedHits;
      m_eventindex = cpy.m_eventindex;
      m_barcode = cpy.m_barcode;
      m_barcode_pt = cpy.m_barcode_pt;
      m_parentage_mask = cpy.m_parentage_mask;
      m_hw_word = cpy.m_hw_word;
      m_truth = nullptr;
      m_channels.assign(cpy.m_channels.begin(),cpy.m_channels.end());
      if (cpy.m_truth)
	m_truth = new MultiTruth(*(cpy.m_truth));
      ///
    }
  return *this;
}

void FTKRawHit::reset()
{
  m_x = 0.;
  m_y = 0.;
  m_z = 0.;
  m_hitType = 0;
  m_moduleType = ftk::MODULETYPE_INVALID;
  m_IdentifierHash = 0;
  m_barrel_ec = 0;
  m_layer_disk = 0;
  m_phi_module = 0;
  m_eta_module = 0;
  m_pi_side = 0;
  m_ei_strip = 0;
  m_n_strips = 0;
  m_etaWidth = 0;
  m_phiWidth = 0;
  m_dPhi = 0.;
  m_dEta = 0.;
  m_includesGangedHits = false;
  m_barcode = -1;
  m_eventindex = -1;
  m_barcode_pt = 0.;
  m_parentage_mask = 0;
  m_hw_word = 0;
  if( m_truth ) {
     delete m_truth;
     m_truth = 0;
  }
  m_channels.clear();
}

ostream &operator<<(std::ostream& out,const FTKRawHit& hit)
{
  out << "S" << "\t" << hit.m_x << "\t" << hit.m_y << "\t" << hit.m_z << "\t";
  out << hit.m_hitType << "\t" << hit.m_barrel_ec << "\t" << hit.m_layer_disk << "\t";
  out << hit.m_phi_module << "\t" <<  hit.m_eta_module << "\t";
  out << hit.m_pi_side << "\t" << hit.m_ei_strip << "\t" << hit.m_n_strips;
  return out;
}

istream& operator>>(istream &input, FTKRawHit &hit)
{
  int dummy;

  input >> hit.m_x >> hit.m_y >> hit.m_z;
  input >> hit.m_hitType >> hit.m_barrel_ec;
  input >> hit.m_layer_disk >> hit.m_phi_module >> hit.m_eta_module;
  input >> hit.m_pi_side >> hit.m_ei_strip;
  input >> hit.m_n_strips;
  if (!input) {
    // is a very old format w/o the number of consecutive strips
    hit.m_n_strips = 1;
  } else {
    // June 2009 format: remaining fields are the UniqueBarcode and the highest pt contributing to the channel, in that order.
    // August 2009 format: remaing fields are event_index, barcode, highest pt, parentage mask.
    streampos position = input.tellg();
    input >> hit.m_barcode;
    if( input ) {
      // is not an old format w/o geant truth info
      input >> hit.m_barcode_pt;
      // Check for August 2009
      input >> dummy;
      if( input ) {
	// August 2009 - need to go back and re-read
	input.seekg(position);
	input >> hit.m_eventindex;
	input >> hit.m_barcode;
	input >> hit.m_barcode_pt;
	input >> hit.m_parentage_mask;
      }
      else {
	// June 2009 -- set barcode and event index to new format.
	hit.m_eventindex = hit.m_barcode/100000;
	hit.m_barcode %= 100000;
	hit.m_parentage_mask = 0;
      }
    }
  }

    input >> hit.m_IdentifierHash;

  // change the layer id according the FTK use
  hit.normalizeLayerID();

  return input;
}


istream& clusterP( istream &input , FTKRawHit &hit )
{
  // added March, 2012

  input >> hit.m_x >> hit.m_y >> hit.m_z;
  hit.setHitType(ftk::PIXEL);
  hit.setModuleType(ftk::MODULETYPE_PIXEL);
  input >> hit.m_barrel_ec;
  input >> hit.m_layer_disk;
  input >> hit.m_phi_module >> hit.m_eta_module;
  input >> hit.m_pi_side >> hit.m_ei_strip;

  input >> hit.m_dPhi >> hit.m_dEta;
  // The wrapper is filled with m_pi_side + dPhi, for example, so correct back...
  hit.m_dPhi -= hit.m_pi_side;
  hit.m_dEta -= hit.m_ei_strip;

  input >> hit.m_phiWidth >> hit.m_etaWidth;

  input >> hit.m_n_strips;
  input >> hit.m_eventindex;
  input >> hit.m_barcode;
  input >> hit.m_barcode_pt;
  input >> hit.m_parentage_mask;

  // Since clustering is already done, there should only be 1 barcode per "hit".
  // Instead of doing anything too complicated for m_truth, we can just store the one barcode.
  //hit.m_truth = new MultiTruth( std::make_pair(hit.m_barcode,hit.m_eventindex) );
  hit.m_truth = new MultiTruth( std::make_pair(hit.m_eventindex,hit.m_barcode) );

  // change the layer id according the FTK use
  hit.normalizeLayerID();

  return input;
}

istream& clusterC( istream &input , FTKRawHit &hit ) {
  // added March, 2012

  input >> hit.m_x >> hit.m_y >> hit.m_z;
  input >> hit.m_hitType;
  input >> hit.m_barrel_ec;
  input >> hit.m_layer_disk;
  input >> hit.m_phi_module >> hit.m_eta_module;
  input >> hit.m_pi_side >> hit.m_ei_strip;

  input >> hit.m_dPhi;
  // The wrapper is filled with m_pi_side + dPhi, so correct back...
  hit.m_dPhi -= hit.m_ei_strip;

  input >> hit.m_n_strips;
  if( hit.m_n_strips < 1 ) { FTKSetup::PrintMessageFmt(ftk::sevr,"Found a C line (SCT cluster) in wrapper with n_strips < 1 (%d)\n",hit.m_n_strips); }
  input >> hit.m_eventindex;
  input >> hit.m_barcode;
  input >> hit.m_barcode_pt;
  input >> hit.m_parentage_mask;

  // Since clustering is already done, there should only be 1 barcode per "hit".
  // Instead of doing anything too complicated for m_truth, we can just store the one barcode.
  //hit.m_truth = new MultiTruth( std::make_pair(hit.m_barcode,hit.m_eventindex) );
  hit.m_truth = new MultiTruth( std::make_pair(hit.m_eventindex,hit.m_barcode) );

  // change the layer id according the FTK use
  hit.normalizeLayerID();

  return input;
}

/** This function change the layer id, how it comes from the atlas numbering
    method, from the id used in FTK pmap. This method update the
    m_layer_disk field for the SCT, doesn't affect the pixels
 */
void FTKRawHit::normalizeLayerID()
{
  if (m_hitType != ftk::SCT ) {
    return;
  }
  else if( FTKSetup::getFTKSetup().getITkMode() ) {
    m_layer_disk = (2*m_layer_disk) + m_pi_side;
  }
  else if (m_barrel_ec == 0){ // is barrel
    if     (m_layer_disk==0 && m_pi_side == 1) m_layer_disk = 0;
    else if(m_layer_disk==0 && m_pi_side == 0) m_layer_disk = 1;
    else if(m_layer_disk==1 && m_pi_side == 0) m_layer_disk = 2;
    else if(m_layer_disk==1 && m_pi_side == 1) m_layer_disk = 3;
    else if(m_layer_disk==2 && m_pi_side == 1) m_layer_disk = 4;
    else if(m_layer_disk==2 && m_pi_side == 0) m_layer_disk = 5;
    else if(m_layer_disk==3 && m_pi_side == 0) m_layer_disk = 6;
    else if(m_layer_disk==3 && m_pi_side == 1) m_layer_disk = 7;

  }
  else {
    /* this is complicated. */
    /* First, figure out if we're on the inside or outside in z, and inner or outer ring */
    int outside = 0;
    int inner_ring = 0;
    if (m_eta_module > 0) inner_ring = 1;
    if ((m_eta_module == 0 || m_eta_module == 2) && m_pi_side == 1) outside = 1;
    if (m_eta_module == 1 && m_pi_side == 0) outside = 1;

    /* disk 8 flipped: special case */
    if (m_layer_disk == 8 && m_pi_side == 1) outside = 0;
    if (m_layer_disk == 8 && m_pi_side == 0) outside = 1;

    /* split the disks up by inner/outer, and by side */
    m_layer_disk = 4*m_layer_disk + 2*inner_ring + outside;

    /* aaaaand fix eta index */
    if (inner_ring) m_eta_module = m_eta_module - 1;

    if (FTKSetup::getFTKSetup().getVerbosity() > 3) {
      printf("Converted disk %d eta %d side %d to split disk %d eta %d\n",
	     m_layer_disk, m_eta_module, m_pi_side,
	     m_layer_disk, m_eta_module);
    }
  }

}

/** this function convert an hit in the format that has all
    the information needed to be linked in the ATLAS geometry, to
    the simple hit codified in the mode internally used by the AM */
FTKHit FTKRawHit::getFTKHit(const FTKPlaneMap *pmap) const {
  // retrieve layer info from the pmap
  FTKPlaneSection &pinfo = pmap->getMap(m_hitType,!(m_barrel_ec==0),m_layer_disk);
  int plane = pinfo.getPlane();
  int section = pinfo.getSection();
  int sector;

  if( FTKSetup::getFTKSetup().getITkMode() ) {
    // Working at least for SCT barrel
    // Ranges from dumping detector info --
    //   phi_modules in [0,..,72]
    //   eta_modules in [-56,..,56]
    //   m_barrel_ec in [-2,0,2] (C-side,B,A-side)
    sector = (m_phi_module*100000) + ((m_eta_module+60)*100) + ((m_barrel_ec+2)*10) + section;
  }
  else if (m_barrel_ec) {
    // is in the end-caps
    int ieta = m_eta_module;
    const int NEGEC (-2);
    int code = (ieta+1)*20 + (m_barrel_ec == NEGEC)*10 + section;
    sector = m_phi_module*1000+code;
  }
  else if (FTKSetup::getFTKSetup().getIBLMode()==1 && m_layer_disk==0 && m_hitType==ftk::PIXEL && m_barrel_ec==0 ){
      // this is an IBL module, without 3d sensors, only possible case up to the TDAQ TDR
      sector = m_phi_module*1000+m_eta_module+8; // ibl was 6
  }
  else if (FTKSetup::getFTKSetup().getIBLMode()==2 && m_layer_disk==0 && m_hitType==ftk::PIXEL && m_barrel_ec==0 ){
      // this is an IBL module with 3d sensors, 20 modules in total instead of 16
      sector = m_phi_module*1000+m_eta_module+10;
  }
  else {
    // is a generic module of the barrel region
    sector = m_phi_module*1000+m_eta_module+6;
  }

  // retrieve information
  int ndim = pmap->getPlane(plane,section).getNDimension();
  FTKHit reshit(ndim);
  reshit.setITkMode( FTKSetup::getFTKSetup().getITkMode() );
  reshit.setIdentifierHash(getIdentifierHash());
  reshit.setPlane(plane);
  reshit.setSector(sector);
  reshit.setHwWord(m_hw_word);
  if (m_moduleType == ftk::MODULETYPE_SCT){
    reshit.setScalingCoordToHWCoord(0,SCT_row_scaling);
    reshit.setScalingCoordToHWCoord(1,0);
  }
  else  if (m_moduleType == ftk::MODULETYPE_PIXEL) {
    reshit.setScalingCoordToHWCoord(0,PIX_row_scaling);
    reshit.setScalingCoordToHWCoord(1,PIX_column_scaling);
  }
  else  if (m_moduleType == ftk::MODULETYPE_IBL_PLANAR) {
    reshit.setScalingCoordToHWCoord(0,IBL_planar_row_scaling);
    reshit.setScalingCoordToHWCoord(1,IBL_planar_column_scaling);
  }
  else if (m_moduleType == ftk::MODULETYPE_IBL3D) {
    reshit.setScalingCoordToHWCoord(0,IBL_3D_row_scaling);
    reshit.setScalingCoordToHWCoord(1,IBL_3D_column_scaling);
  }
  else {// don't think i should be here!
    reshit.setScalingCoordToHWCoord(0,0);
    reshit.setScalingCoordToHWCoord(1,0);
  }

  switch (m_hitType) {
  case ftk::PIXEL:
    reshit.setEtaWidth(getEtaWidth());
    reshit.setPhiWidth(getPhiWidth());
    reshit.getCoord().setIncludesGanged(getIncludesGanged());
    reshit.setNStrips(getNStrips());
    reshit[0] = m_pi_side+m_dPhi;
    reshit[1] = m_ei_strip+m_dEta;
    break;
  case ftk::SCT:
    //    reshit[0] = m_ei_strip+(m_n_strips-1.)/2.;
    reshit[0] = m_ei_strip+m_dPhi;
    reshit.setNStrips(getNStrips());
    reshit.setEtaWidth(getEtaWidth());
    reshit.setPhiWidth(getPhiWidth());
    break;
  case ftk::SCTtrk:
    // cy to adjust 8L track pars if they exceed thresholds
    double mod_x = ( m_x > ftk::PI ? ftk::PI : (m_x < -ftk::PI ? -ftk::PI : m_x) );
    double mod_y = ( m_y > ftk::COTTMAX ? ftk::COTTMAX : (m_y < -ftk::COTTMAX ? -ftk::COTTMAX : m_y) );
    double mod_z = ( m_z > ftk::CURVMAX ? ftk::CURVMAX : (m_z < -ftk::CURVMAX ? -ftk::CURVMAX : m_z) );

    // FlagAK: changed all int() conversions to floor() - this is needed for FTKSSMap to correctly guess module offset
    int phiMod=static_cast<int>(floor(mod_x/(2*ftk::PI)*(pmap->getPlane(plane,section).getNumFi())));
    if(phiMod < 0) phiMod+=pmap->getPlane(plane,section).getNumFi();

#ifdef SPLIT_OLDCTHETA
    int cottMod=static_cast<int>(floor((ftk::COTTMAX+mod_y)/(2*ftk::COTTMAX)*(pmap->getPlane(plane,section).getNumCott())));
#else
    // for cot(theta), we now use a hardooded table - see FTKSplitEncoder.{h,cxx}
    int cottMod=FTKSplitEncoder::get().cthetaIndex(mod_y);
#endif

    int curvMod=static_cast<int>(floor((ftk::CURVMAX+mod_z)/(2*ftk::CURVMAX)*(pmap->getPlane(plane,section).getNumCurv())));

    sector = phiMod*10000+cottMod*100+curvMod;
    //cout << "mod_x: " << mod_x << "\tphiMod: " << phiMod << endl; // cy debug
    //cout << "sector: " << sector << "\tplane: " << plane << endl; // cy debug
    reshit.setSector(sector);
    reshit.setBankID(getBankID());
    reshit.setRoadID(getRoadID());
    reshit.setTrackID(getTrackID());
    reshit[0] = mod_x;
    reshit[1] = mod_y;
    reshit[2] = mod_z;
    break;
  }

  // convert also the eventual channels
  vector<FTKRawHit>::const_iterator ichannel = m_channels.begin();
  vector<FTKRawHit>::const_iterator ichannel_end = m_channels.end();
  for (;ichannel!=ichannel_end;++ichannel) {
    reshit.addChannel((*ichannel).getFTKHit(pmap));
  }

  return reshit;
}

void FTKRawHit::setTruth(const MultiTruth& v)
{
  if(m_truth) {
    *m_truth = v;
  } else {
    m_truth = new MultiTruth(v);
  }
}

