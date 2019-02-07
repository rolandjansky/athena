/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/TrackFitter711.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/ftkdefs.h"

#include <TSystem.h>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cassert>
using namespace std;

static int print_guessed=0; //1000;

TrackFitter711::TrackFitter711() :
  TrackFitter(),
  m_use_guessing(true),
  m_ncoords_incomplete(0),
  m_incomplete_coordsmask(0), m_incomplete_planesmask(0),
  m_extrafits(1), m_max_missing_extraplanes(1), m_nneighbours(3),
  m_passedExtrapolation(false),
  m_nplanes_ignored(0),
  m_idplanes(0), m_idcoords(0), m_idplanes_or(0),
  m_endcap_inversion(0),
  m_idplanes_eff(0), m_idcoords_eff(0),
  m_CImap_eff(0), m_CIlayermap_eff(0x0),
  m_ext_parmask(1<<3), m_idpars(0),
  m_histores_hitcoord_PXL(0),
  m_histores_hitcoord_SCT(0),
  m_use_SectorDB(false), m_use_multiple_conn(false), m_use_n_conn(999), m_saveIncompleteTracks(false),
  m_resolution_mode(false), m_super_extrapolate(true),
  m_CImap(0), m_CIlayermap(0x0),
  m_resfile(0),
  m_histores_SS_BRL(0),
  m_histores_hits_ECP(0),
  m_histores_SS_ECP(0),
  m_treeres(0),
  m_tv_nmiss(0),
  m_tv_npars(0),
  m_tv_ncombs(0),
  m_tv_chi2I(0),
  m_tv_chi2C(0),
  m_tv_parsI(0),
  m_tv_parsC(0),
  m_tv_orhits(0),
  m_tv_exhits(0),
  m_tv_orSS(0),
  m_tv_exSS(0),
  m_tv_isEC(0),
  m_ncombsI(0),
  m_nfitsI(0),
  m_nfits_majI(0),
  m_nfits_majI_pix(0),
  m_nfits_majI_SCT(0),
  m_nfits_recI(0),
  m_nfits_addrecI(0),
  m_nfits_badI(0),
  m_nfits_rejI(0),
  m_nfits_badmajI(0),
  m_nfits_rejmajI(0),
  m_nconn(0),
  m_nextrapolatedTracks(0),
  m_comb_idI(0),
  m_debug_super_extrapolate(0),
  m_section_of_exp_layers(0),
  m_baseHWflag(0),
  m_region_for_superexp(0),
  m_subreg_for_superexp(0),
  m_sector_for_superexp(0),
  m_ssid(0),
  m_bounds(0),
  cur_iconn(0),
  m_cbitmask(0),
  m_cbitmask_real(0),
  m_norecovery_mask(0),
  m_hits_more(0),
  m_check_TR_by(0),
  m_transition_region_max_eta(1.4),
  m_transition_region_min_eta(1.0),
  m_super_exp_allow_extra_miss(false),
  m_diagnostic_mode(false),
  m_diagnostic_plots_tfile(0),
  m_histo_nhitsfound_per_track_each_layer(0),
  m_histo_nhitsfound_per_track_each_layer_each_conn(0),
  m_histo_ncomb_per_track_each_conn(0),
  m_histo_ncomb_each_conn(0),
  m_histo_nfits_per_track(0),
  m_histo_ntrackI_ntrack(0),
  m_histo_nconn_per_track(0),
  m_histo_bitmask(0),
  m_histo_module_value_diff(0),
  m_ncomb_event_conn(0),
  m_IBL_res(0),
  IBLcoordPhi(0),
  IBLcoordEta(0),
  m_histo_delta_guessed_hit_found_hit(0),
  ibl_module_with_hit(0)
{
   // change the current default behaviors when the constants
   m_noconstants_errlevel = ftk::warn;
}


TrackFitter711::~TrackFitter711()
{
   //for (int i=0;i<m_nregions;++i) {
  //  for (int j=0;j!=m_nsubregions;++j) {
  //    if (m_constant_incomplete[i][j])
  //      delete m_constant_incomplete[i][j];
  //  }
  //  delete [] m_constant_incomplete[i];
  //}
  for (int i=0;i<m_nregions;++i) {
    for (int j=0;j!=m_nsubregions;++j) {
      if (m_constant_incomplete[i][j])
        delete m_constant_incomplete[i][j];
    }
    delete [] m_constant_incomplete[i];
  }
  if (m_nregions>0) delete [] m_constant_incomplete;
  if (m_CImap) {
    delete [] m_CImap;
    delete [] m_CIlayermap;
    delete [] m_idplanes;
    delete [] m_idcoords;
    delete [] m_idplanes_or;
    delete [] m_idpars;
    delete [] m_endcap_inversion;

    delete [] m_idplanes_eff;
    delete [] m_idcoords_eff;
    delete [] m_CImap_eff;
    delete [] m_CIlayermap_eff;
  }

  if (m_resolution_mode) {
    m_resfile->Write();
    m_resfile->Close();
  }
  if (m_super_extrapolate) {
    delete [] m_section_of_exp_layers;
    delete [] m_ssid;
    delete [] m_hits_more;
    delete [] m_bounds;
  }
  if (getDiagnosticMode()) {
    saveDiagnosticPlots();
    deleteDiagnosticVariables();
  }

   if (m_histores_hitcoord_PXL) delete [] m_histores_hitcoord_PXL;
   if (m_histores_hitcoord_SCT) delete [] m_histores_hitcoord_SCT;
}

void TrackFitter711::saveDiagnosticPlots() {
  m_diagnostic_plots_tfile->cd();
  for (int i=0;i<m_nplanes_ignored;++i) {
    m_histo_nhitsfound_per_track_each_layer[i]->Write();
    m_histo_nhitsfound_per_track_each_layer_each_conn[i]->Write();
  }
  m_IBL_res->Write();
  for (int i = 0; i < m_use_n_conn; ++ i) {
    m_histo_ncomb_per_track_each_conn[i]->Write();
  }
  for (int i = 0; i < m_use_n_conn; ++ i) {
    m_histo_ncomb_each_conn[i]->Write();
  }
  m_histo_nfits_per_track->Write();
  m_histo_nconn_per_track->Write();
  m_histo_bitmask->Write();
  m_histo_module_value_diff->Write();
  m_histo_ntrackI_ntrack->Write();
  m_histo_delta_guessed_hit_found_hit->Write();
  m_diagnostic_plots_tfile->Close();
}

void TrackFitter711::deleteDiagnosticVariables() {
  // Some global variables
  delete [] m_ncomb_event_conn;
}


void TrackFitter711::setNCoordsPlanes(int , int ) {
  FTKSetup::PrintMessage(ftk::sevr, "The setNCoordsPlane method for TrackFitter711 needs 4 arguments");
}

void TrackFitter711::setNCoordsPlanes(int ncoordsI, int nplanesI,
                                      int ncoordsC, int nplanesC)
{
  TrackFitter::setNCoordsPlanes(ncoordsC,nplanesC);
  m_ncoords_incomplete = ncoordsI;
  m_nplanes_incomplete = nplanesI;

  // size the Complete->Incomplete convertion map
  if (m_CImap) {
    delete [] m_CImap;
    delete [] m_CImap_eff;
    delete [] m_CIlayermap;
    delete [] m_CIlayermap_eff;
  }

  m_CImap = new int[m_ncoords_incomplete];
  m_CImap_eff = new int[m_ncoords_incomplete];
  m_CIlayermap = new int[m_nplanes_incomplete];
  m_CIlayermap_eff = new int[m_nplanes_incomplete];

}


void TrackFitter711::setBank(int ir, int isr,
                             FTKConstantBank *bank_complete,
                             FTKConstantBank *bank_incomplete,
                             FTKSector711DB *reldb)
{
  if (ir>=m_nregions)
    FTKSetup::PrintMessageFmt(ftk::sevr,"Region ID %d too large. Wrong number of region (%d)",ir,m_nregions);
  else if (isr>=m_nsubregions)
    FTKSetup::PrintMessageFmt(ftk::sevr,"Sub-region ID %d too large. Wrong number of sub-region (%d)",isr,m_nsubregions);

  m_constant[ir][isr] = bank_complete;
  m_constant_incomplete[ir][isr] = bank_incomplete;
  m_FC_relations[ir][isr] = reldb;
}

/** initialize the track-fitter */
void TrackFitter711::init()
{
  TrackFitter::init();

  // set the dimension of the temporary tracks
  newtrkI.setNCoords(m_ncoords_incomplete);
  newtrkI.setNPlanes(m_nplanes_incomplete);

  // allocate once the array of recovered tracks
  combtrackI = new FTKTrack[m_ncoords_incomplete];
  for (int ic=0;ic!=m_ncoords_incomplete;++ic) {
    combtrackI[ic].setNCoords(m_ncoords_incomplete);
    combtrackI[ic].setNPlanes(m_nplanes_incomplete);
  }
  m_complete_maskI = ~(~0<<m_ncoords_incomplete);

  m_nplanes_ignored = m_nplanes-m_nplanes_incomplete;
  m_ncoords_ignored = m_ncoords-m_ncoords_incomplete;
  FTKSetup::PrintMessageFmt(ftk::info,"m_nplanes_ignored : %d", m_nplanes_ignored);

  m_idplanes = new int[m_nplanes_ignored];
  m_idcoords = new int[m_nplanes_ignored];
  m_idCImap = new int[m_nplanes_ignored];
  m_idplanes_or = new int[m_nplanes_ignored];
  m_idpars = new int[m_nplanes_ignored];
  m_endcap_inversion = new bool[m_nplanes_ignored];
  m_idplanes_eff = new int[m_nplanes_ignored];
  m_idcoords_eff = new int[m_nplanes_ignored];

  int ipar(0);
  for (int ip=0;ip!=m_npars;++ip) {
    if (m_ext_parmask&(1<<ip)) continue;
    if (ip==4) continue;
    m_idpars[ipar++] = ip;
  }
  // if it fails there were a problem in extmask
  //assert(ipar==m_nplanes_ignored);

  //
  // Coordinate masking and plane masking map building algorithm
  //

  // These 6 counters will evolve in each step of map building
  // to generalize to 1st/2nd stage extrapolation.
  int plane_index_1st_stage(0);
  int plane_index_2nd_stage(0);
  int coord_index_1st_stage(0);
  int coord_index_2nd_stage(0);
  int index_missing_plane(0);
  int index_missing_coord(0);

  //int debugcounter(0);
  do { // maps building

    // DEBUG
    //FTKSetup::PrintMessageFmt(ftk::debg, "DEBUG iteration # %d\n", debugcounter);
    //FTKSetup::PrintMessageFmt(ftk::debg, "----------------------------------------------\n");
    //FTKSetup::PrintMessageFmt(ftk::debg, "plane_index_1st_stage: %d\n", plane_index_1st_stage);
    //FTKSetup::PrintMessageFmt(ftk::debg, "plane_index_2nd_stage: %d\n", plane_index_2nd_stage);
    //FTKSetup::PrintMessageFmt(ftk::debg, "coord_index_1st_stage: %d\n", coord_index_1st_stage);
    //FTKSetup::PrintMessageFmt(ftk::debg, "coord_index_2nd_stage: %d\n", coord_index_2nd_stage);
    //FTKSetup::PrintMessageFmt(ftk::debg, "index_missing_plane  : %d\n", index_missing_plane);
    //FTKSetup::PrintMessageFmt(ftk::debg, "index_missing_coord  : %d\n", index_missing_coord);


    //
    // variables initialization
    //
    // dimension of this layer (2 for pixel, 1 for SCT). Reading from .pmap file.
    const int ndim = m_pmap->getPlane(plane_index_2nd_stage,0).getNDimension();
    bool doRelation(false); // if true, this layer/coords has/have missing layer/coords

    int nplanes_1st_stage = m_pmap_incomplete->getNPlanes();

    //FTKSetup::PrintMessageFmt(ftk::debg, "ndim: %d\n", ndim);
    //FTKSetup::PrintMessageFmt(ftk::debg, "nplanes_1st_stage: %d\n", nplanes_1st_stage);

    //
    // Determine whether in this iteration the layers/coords to be extrapolated
    //
    if (plane_index_1st_stage >= nplanes_1st_stage) {
      doRelation = true;
    } else if (m_pmap_incomplete->getPlane(plane_index_1st_stage, 0).getLayer() !=
               m_pmap           ->getPlane(plane_index_2nd_stage, 0).getLayer()
              ) {
      doRelation = true;
      // debug
      //int det_layer_1st_stage = m_pmap_incomplete->getPlane(plane_index_1st_stage, 0).getLayer();
      //int det_layer_2nd_stage = m_pmap->getPlane(plane_index_2nd_stage, 0).getLayer();
      //FTKSetup::PrintMessageFmt(ftk::debg, "det_layer_1st_stage: %d\n", det_layer_1st_stage);
      //FTKSetup::PrintMessageFmt(ftk::debg, "det_layer_2nd_stage: %d\n", det_layer_2nd_stage);
    }

    //
    // Perfrom map building
    //
    if (doRelation) {

      //
      // When there is a missing hit
      //

      // index of missing planes in 2nd_stage reference frame
      m_idplanes[index_missing_plane] = plane_index_2nd_stage;

      // index of missing coords in 2nd_stage reference frame
      m_idcoords[index_missing_plane] = coord_index_2nd_stage;

      // index to the coordinates right before the missing coords in 1st stage reference rame
      m_idCImap[index_missing_plane] = coord_index_1st_stage-1;

      // index to the planes right before the missing plane in 1st stage reference rame
      m_idplanes_or[index_missing_plane] = plane_index_1st_stage-1;

      // protecting certain index values to be never below 0 in order to protect against reading false memory position
      if (m_idCImap[index_missing_plane] < 0)
        m_idCImap[index_missing_plane] = 0;
      if (m_idplanes_or[index_missing_plane] < 0)
        m_idplanes_or[index_missing_plane] = 0;

      //for (int ii = 0; ii < m_nplanes; ++ii) {
      //  std::cout << "getPlane(0)" << ii << " " << m_pmap->getPlane(ii,0).getNSte() << std::endl;
      //  std::cout << "getPlane(1)" << ii << " " << m_pmap->getPlane(ii,1).getNSte() << std::endl;
      //}

      // do Invert in this plane?
      bool doInvert = m_pmap->getPlane(plane_index_2nd_stage,0).getNSte() != m_pmap->getPlane(plane_index_2nd_stage,1).getNSte();
      if (m_pmap->getPlane(plane_index_2nd_stage,0).getNDimension()!=1)
        doInvert = false;
      m_endcap_inversion[index_missing_plane] = doInvert;


      //std::cout << "m_endcap_inversion: {";
      //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
      //  std::cout << m_endcap_inversion[ii] << ", ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_idplanes: {";
      //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
      //  std::cout << m_idplanes[ii] << ", ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_idcoords: {";
      //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
      //  std::cout << m_idcoords[ii] << ", ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_idCImap: {";
      //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
      //  std::cout << m_idCImap[ii] << ", ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_idplanes_or: {";
      //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
      //  std::cout << m_idplanes_or[ii] << ", ";
      //}
      //std::cout << "}" << std::endl;

      plane_index_2nd_stage += 1;
      coord_index_2nd_stage += ndim;
      index_missing_plane += 1;
      index_missing_coord += ndim;

      continue;
    }
    else {

      //
      // When there is no missing hits
      //

      m_incomplete_planesmask |= 1<<plane_index_2nd_stage;
      for (int ix = 0; ix != ndim; ++ix) {
        m_CImap[coord_index_1st_stage + ix] = coord_index_2nd_stage + ix;
        m_CImap_eff[coord_index_1st_stage + ix] = coord_index_2nd_stage + ix;
        m_incomplete_coordsmask |= 1<<(coord_index_2nd_stage + ix);
      }

      // fill the relation for the logical layers
      m_CIlayermap[plane_index_1st_stage] = plane_index_2nd_stage;
      m_CIlayermap_eff[plane_index_1st_stage] = plane_index_2nd_stage;

      //std::cout << "m_CImap: {";
      //for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
      //  std::cout << m_CImap[ii] << " , ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_CImap_eff: {";
      //for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
      //  std::cout << m_CImap_eff[ii] << " , ";
      //}
      //std::cout << "}" << std::endl;

      //std::cout << "m_incomplete_coordsmask " << m_incomplete_coordsmask << std::endl;
      //int number = m_incomplete_coordsmask;
      //do {
      //  std::cout << number%2 << " , ";
      //  number = number >> 1;
      //} while(number != 0);
    }

    // move the indexes
    plane_index_1st_stage += 1;
    coord_index_1st_stage += ndim;
    plane_index_2nd_stage += 1;
    coord_index_2nd_stage += ndim;
  }   while (plane_index_2nd_stage!=m_pmap->getNPlanes());// end maps building loop


  // check the reach of the end of both maps
  assert(coord_index_1st_stage==m_ncoords_incomplete && coord_index_2nd_stage==m_ncoords);

  if (m_super_extrapolate) {
    m_section_of_exp_layers = new int[m_nplanes_ignored];
    m_ssid = new int[m_nplanes_ignored];
    m_bounds = new int*[m_nplanes_ignored];
    m_hits_more = new std::vector<FTKHit>[m_nplanes_ignored];
  }


  // if it is working in the resolution mode few histograms are prepared
  if (m_resolution_mode) {
    std::cout << "Create study711.root" << std::endl;
    std::cout << gSystem->Getenv("PWD") << std::endl;
    m_resfile = TFile::Open("study711.root","recreate");
    m_nmiss_PXL = 2;
    m_nmiss_SCT = 3;
    m_n_tot_PXL = 4;
    m_resmode_bitmask = 30204;

    m_histores_hitcoord_PXL = new TH2F*[m_nmiss_PXL];
    for (int i=0;i<m_nmiss_PXL;++i) {
      TString histname = TString::Format("m_histores_hitcoord_PXL%d",i);
      m_histores_hitcoord_PXL[i] = new TH2F(histname,histname,20,-20,20,20,-20,20);
    }

    m_histores_hitcoord_SCT = new TH1F*[m_nmiss_SCT];
    for (int i=0;i<m_nmiss_SCT;++i) {
      TString histname = TString::Format("m_histores_hitcoord_SCT%d",i);
      m_histores_hitcoord_SCT[i] = new TH1F(histname,histname,20,-5,5);
    }
  }

  // If diagnostic mode is true, create some additional plots that would normally not be included track containers
  if (getDiagnosticMode()) {
    prepareDiagonsticPlots();
    createDiagnosticVariables();
  }
}

/** initiating diagnostic plots **/
void TrackFitter711::prepareDiagonsticPlots() {

  std::cout << "Diagnostic mode is turned on" << std::endl;
  std::cout << "This will create additional histograms in ";
  std::cout << gSystem->Getenv("PWD");
  std::cout << "/tf711_diagnostic_plots.root" << std::endl;

  m_diagnostic_plots_tfile = TFile::Open("tf711_diagnostic_plots.root", "recreate");

  m_histo_nhitsfound_per_track_each_layer = new TH1F*[m_nplanes_ignored];
  for (int i=0;i<m_nplanes_ignored;++i) {
    TString histname = TString::Format("m_histo_nhitsfound_per_track_layer%d", i);
    m_histo_nhitsfound_per_track_each_layer[i] = new TH1F(histname, histname, 20, 0, 20);
  }
  m_histo_nhitsfound_per_track_each_layer_each_conn = new TH2F*[m_nplanes_ignored];
  m_IBL_res = new TH2F("IBL_res","IBL_res",40,-20,20,40,-20,20);
  for (int i=0;i<m_nplanes_ignored;++i) {
    TString histname = TString::Format("m_histo_nhitsfound_per_track_per_conn_layer%d", i);
    m_histo_nhitsfound_per_track_each_layer_each_conn[i] = new TH2F(histname, histname, 20, 0, 20, 20, 0, 20);
  }
  m_histo_ncomb_per_track_each_conn = new TH1F*[m_use_n_conn];
  for (int i=0;i<m_use_n_conn;++i) {
    TString histname = TString::Format("m_histo_ncomb_per_track_each_conn%d", i);
    m_histo_ncomb_per_track_each_conn[i] = new TH1F(histname, histname, 100, 0, 100);
  }
  m_histo_ncomb_each_conn = new TH1F*[m_use_n_conn];
  for (int i=0;i<m_use_n_conn;++i) {
    TString histname = TString::Format("m_histo_ncomb_each_conn%d", i);
    m_histo_ncomb_each_conn[i] = new TH1F(histname, histname, 300, 0, 300);
  }
  m_histo_nfits_per_track = new TH1F("m_histo_nfits_per_track", "m_histo_nfits_per_track", 100, 0, 100);
  m_histo_nconn_per_track = new TH1F("m_histo_nconn_per_track", "m_histo_nconn_per_track", m_use_n_conn+1, 0, m_use_n_conn+1);
  m_histo_bitmask = new TH1F("m_histo_bitmask", "m_histo_bitmask", m_ncoords, 0, m_ncoords);
  m_histo_module_value_diff = new TH1F("module_value_diff", "module_value_diff", 10000, 0, 10000);
  m_histo_ntrackI_ntrack = new TH2F("m_histo_ntrackI_ntrack", "m_histo_ntrackI_ntrack", 100, 0, 100, 20, 0, 20);
  //m_histo_delta_guessed_hit_found_hit = new TH2F("m_histo_delta_guessed_hit_found_hit", "m_histo_delta_guessed_hit_found_hit", 100, -336, 336, 100, -160, 160);
  m_histo_delta_guessed_hit_found_hit = new TH2F("m_histo_delta_guessed_hit_found_hit", "m_histo_delta_guessed_hit_found_hit", 100, -36, 36, 100, -36, 36);
}

void TrackFitter711::createDiagnosticVariables() {
  // Some global variables
  m_ncomb_event_conn = new int[m_use_n_conn];
}

void TrackFitter711::initializeDiagnosticVariables() {
  for (int iconn = 0; iconn < m_use_n_conn; ++ iconn) {
    m_ncomb_event_conn[iconn] = 0;
  }
}

/** prepare */
void TrackFitter711::prepareBanks(int nreg, int nsub)
{
  TrackFitter::prepareBanks(nreg,nsub);

  m_constant_incomplete = new FTKConstantBank**[m_nregions];
  m_FC_relations = new FTKSector711DB**[m_nregions];

  for (int i=0;i<m_nregions;++i) { // regions loop
    m_constant_incomplete[i] = new FTKConstantBank*[m_nsubregions];
    m_FC_relations[i] = new FTKSector711DB*[m_nsubregions];

    for (int j=0;j<m_nsubregions;++j) {
      m_constant_incomplete[i][j] = 0;
      m_FC_relations[i][j] = 0x0;
    }
  } // end regions loop

}


/** resets the usual TF variable and more the specific TF711
    variables */
void TrackFitter711::processor_init(int ibank)
{

  TrackFitter::processor_init(ibank);

  // reset counters
  m_ncombsI = 0;
  m_nfitsI = 0;
  m_nfits_majI = 0;
  m_nfits_majI_pix = 0;
  m_nfits_majI_SCT = 0;
  m_nfits_recI = 0;
  m_nfits_addrecI = 0;
  m_nfits_badI = 0;
  m_nfits_rejI = 0;
  m_nfits_badmajI = 0;
  m_nfits_rejmajI = 0;
  m_nconn = 0;
  m_nextrapolatedTracks = 0;
  // diagnostic variables
  if (getDiagnosticMode()) {
    initializeDiagnosticVariables();
  }
}

/** call the base ::processor_end() to store the base variables,
    then call specific functions to store the additional variable
    used to store the load of the TF in the first step */
void TrackFitter711::processor_end(int ibank)
{
  if (getDiagnosticMode())
    fillDiagnosticPlotsPerEvent();

  TrackFitter::processor_end(ibank);

  m_trackoutput->addNCombsI(ibank,m_ncombsI);
  m_trackoutput->addNFitsI(ibank,m_nfitsI);
  m_trackoutput->addNFitsMajorityI(ibank,m_nfits_majI);
  m_trackoutput->addNFitsMajorityI_pix(ibank,m_nfits_majI_pix);
  m_trackoutput->addNFitsMajorityI_SCT(ibank,m_nfits_majI_SCT);
  m_trackoutput->addNFitsRecoveryI(ibank,m_nfits_recI);
  m_trackoutput->addNAddFitsRecoveryI(ibank,m_nfits_addrecI);
  m_trackoutput->addNFitsBadI(ibank,m_nfits_badI);
  m_trackoutput->addNFitsHWRejectedI(ibank,m_nfits_rejI);
  m_trackoutput->addNFitsBadMajorityI(ibank,m_nfits_badmajI);
  m_trackoutput->addNFitsHWRejectedMajorityI(ibank,m_nfits_rejmajI);
  m_trackoutput->addNConnections(ibank,m_nconn);
  m_trackoutput->addNExtrapolatedTracks(ibank,m_nextrapolatedTracks);
  
  // m_trackoutput_pre_hw->addNCombsI(ibank,m_ncombsI);
  // m_trackoutput_pre_hw->addNFitsI(ibank,m_nfitsI);
  // m_trackoutput_pre_hw->addNFitsMajorityI(ibank,m_nfits_majI);
  // m_trackoutput_pre_hw->addNFitsMajorityI_pix(ibank,m_nfits_majI_pix);
  // m_trackoutput_pre_hw->addNFitsMajorityI_SCT(ibank,m_nfits_majI_SCT);
  // m_trackoutput_pre_hw->addNFitsRecoveryI(ibank,m_nfits_recI);
  // m_trackoutput_pre_hw->addNAddFitsRecoveryI(ibank,m_nfits_addrecI);
  // m_trackoutput_pre_hw->addNFitsBadI(ibank,m_nfits_badI);
  // m_trackoutput_pre_hw->addNFitsHWRejectedI(ibank,m_nfits_rejI);
  // m_trackoutput_pre_hw->addNFitsBadMajorityI(ibank,m_nfits_badmajI);
  // m_trackoutput_pre_hw->addNFitsHWRejectedMajorityI(ibank,m_nfits_rejmajI);
  // m_trackoutput_pre_hw->addNConnections(ibank,m_nconn);
  // m_trackoutput_pre_hw->addNExtrapolatedTracks(ibank,m_nextrapolatedTracks);

}


/*******************************************
 * Function:  processor()
 * Purpose:   Finds all tracks within a given road
 * Arguments: FTKRoad
 * Comment:   Called once per road (aka pattern) from a loop in nextEvent()
*******************************************/
void TrackFitter711::processor(const FTKRoad &road) {
  if (m_resolution_mode) {

    list<FTKTrack> road_tracks; // list 7L tracks for this road
    list<FTKTrack> road_tracks_pre_hw; // list 7L tracks before the HW filter

    processor_Incomplete(road,road_tracks, road_tracks_pre_hw);

    // JAAA not used anymore
    //    processor_ResolutionMode(road,road_tracks);
  }
  else {     // performe the fits of the incomplete set of constants
    list<FTKTrack> road_tracks; // list 7L tracks for this road
    list<FTKTrack> road_tracks_pre_hw; // list 7L tracks before the HW filter

    // perfom the incomplete fit
    processor_Incomplete(road,road_tracks, road_tracks_pre_hw);

    if (road_tracks.empty()) return;

    if (m_saveIncompleteTracks) {
      int region = road.getRegion();
      list<FTKTrack>::iterator itrack = road_tracks.begin();
      for (;itrack!=road_tracks.end();++itrack) {
        m_trackoutput->addTrackI(region,*itrack);
      }
      if (m_saveStepByStepTracks) {
        itrack = road_tracks_pre_hw.begin();
        for (;itrack!=road_tracks_pre_hw.end();++itrack) {
          m_trackoutput->addTrackI_pre_hw(region,*itrack);
        }
      }
      // itrack = road_tracks_pre_hw.begin();
      // for (;itrack!=road_tracks_pre_hw.end();++itrack) {
      //   m_trackoutput_pre_hw->addTrackI(region,*itrack);
      //      }
    }
    // extrapolate and complete the fit
    if (!m_super_extrapolate){
      processor_Extrapolate(road,road_tracks);
    }
    else{
      processor_SuperExtrapolate(road,road_tracks);
  }
}
}


/** this method is derived from the standard processor method in the
    TrackFitter class. Basically changes the reference to the incomplete
    constant set and add some details related in the method how the
    incomplete tracks are managed in the current case */
void TrackFitter711::processor_Incomplete(const FTKRoad &road,
                                          list<FTKTrack> &road_tracks,
                                          list<FTKTrack> &road_tracks_pre_hw)
{
  // check if the road was marked as rejected by the RW or HF
  if (road.getRWRejected()!=0 &&
      (m_fit_removed==0 ||
       (m_fit_removed>0 && road.getRWRejected()<m_fit_removed)) ) {
    // if asked rejected roads can be fitted for debugging reason
    return;
  }
  if (road.getHFRejected())
    return;

  // if it's required ask for hits in the first layer
  if (m_require_first && !road.hasHitOnLayer(0)) return;

   // retrieve all the informations for the fits
  int region = road.getRegion(); // a.k.a. bank
  int subreg = road.getSubRegion();
  int sector = road.getSectorID();
  int nplanes = road.getNPlanes();
  if (nplanes!=m_nplanes_incomplete) {
      FTKSetup::PrintMessageFmt(ftk::sevr,"Mismatch between number of layer in the road: %d!=%d",
                                nplanes,m_nplanes_incomplete);
  }

  // get the bank related to this road
  FTKConstantBank *current_bank_incomplete = m_constant_incomplete[region][subreg];
  if (!current_bank_incomplete) { // no valid constants
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                              "*** Incomplete constants for region %d - %d don't exists\n",
                              region,subreg);

    return;
  }
  // check if the number of sector is compatible with this bank
  else if (sector>=current_bank_incomplete->getNSectors()) {
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                              "*** Incomplete constants for Sector %d in region %d - %d don't exists\n",
                              sector,region,subreg);

    return;
  }
  // check if the sector has a valid constants set
  else if (!current_bank_incomplete->getIsGood(sector)) {
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                              "*** Incomplete constants for Sector %d in region %d - %d is not valid\n",
                              sector,region,subreg);

    return;
  }

  // mask to prevent majority in planes with multiple hits
  unsigned int norecovery_mask(0);

   // number of combinations
   int ncomb(1);
   int nmissing(m_ncoords_incomplete);
   unsigned int bitmask(0);
   bool missPix(false);
   bool missSCT(false);

   for (int p=0;p<nplanes;++p) {
     int nhits = road.getNHits(p);
     if (nhits==0) {
       // check if the planes has hits or not
       m_startlist[p] = m_endlist[p] = m_position[p] = vector<FTKHit>::const_iterator(); // empty iterator, this will flag empty lists
       m_hitcnt[p]=-1;
       // set the fake hit in the empty layer
       newtrkI.setFTKHit(p,FTKHit());
       int iy = m_pmap->getDim(p,1); // use to determine if plane with no hits is PIX or SCT
       if (iy == -1) missSCT = true; // SCT
       else missPix = true; // Pix
     }
     else {
       // set the list iterators
       m_position[p] = m_startlist[p] = road.getHits(p).begin();
       m_endlist[p] = road.getHits(p).end();
       ncomb  *= nhits;
       m_hitcnt[p] = 0;
       // set the bitmask and nmissing, the same for all the combs
       // in this road

       // retrieve the coordinate in the final array
       int ix = m_pmap_incomplete->getDim(p,0);
       int iy = m_pmap_incomplete->getDim(p,1);
       // decide if a 1D or 2D hit
       int ndim = iy==-1 ?  1 : 2;
       if (ndim==1) {
         bitmask |= 1 << ix;
         nmissing -= 1;
       }
       else {
         bitmask |= ( (1<<ix) | (1<<iy));
         nmissing -= 2;
       }
       // set the mask to avoid recovery in crowded planes
       if (nhits>m_norecovery_nhits && m_norecovery_nhits!=-1) norecovery_mask |= (1<<p);
     }
   }

   m_ncombsI += ncomb;

   // newtrkI and newtrk are "global", the road related values can be set here
   // for all the combinations
   newtrkI.setBankID(road.getBankID());
   newtrkI.setRoadID(road.getRoadID());
   newtrkI.setSectorID(road.getSectorID());
   // bitmask variables for the complete tracks will change
   newtrkI.setBitmask(bitmask);
   newtrkI.setNMissing(nmissing);

   // the base HW flag depends if the road is accepted by the RW
   int HWbase = road.getRWRejected()*10;

   // FlagJT: Vector to hold all of the combinations in order to pick
   // the best-chi2 fit with the most number of hits. If we're not
   // just doing one track/road, then this just holds all of the
   // combinations.
   std::vector<FTKTrack> theCombos;
   //int min_nmissing = nplanes;

   // reset the FTK track type flag
   newtrkI.setTypeMask(0);

   // FlagAK: don't process more than m_max_ncomb. Relevant for 10^34 events
   for (int comb=0;comb<(ncomb>m_max_ncomb ? m_max_ncomb : ncomb);++comb) {

     /* FlagAK:
        Decide if this hit combination survived hit filter (optional)
        If hit filter had not been run, this doesn't do anything
      */
     int hf_nmatches(0);
     for (int ip=0;ip<nplanes;++ip) {
       if(m_hitcnt[ip]==-1) { // this plane doesn't have any hits
         hf_nmatches++;
       }
       else { // this plane has some hits. Test if current hit passed hit filter
         if(!road.hit_failed_HF(ip,m_hitcnt[ip])) hf_nmatches++;
       }
     }
     int hf_rejected = (hf_nmatches != nplanes);

     /* Try to fit a track */
     if (m_ntracks>=m_max_trkout) {
       FTKSetup::PrintMessage(ftk::warn,"processor:too many tracks");
       break;
     } else if (!hf_rejected) {
       // this block uses the "newtrk" variable that is a field of the object

       for (int ip=0;ip<nplanes;++ip) {
         // retrieve the coordinate in the final array
         int ix = m_pmap_incomplete->getDim(ip,0);
         int iy = m_pmap_incomplete->getDim(ip,1);
         // decide if a 1D or 2D hit
         int ndim = iy==-1 ?  1 : 2;
         if (m_hitcnt[ip]!=-1) {
           // set the cluster for this layer
           newtrkI.setFTKHit(ip,*m_position[ip]);

           if (ndim==1) {
             newtrkI.setCoord(ix,(*m_position[ip])[0]);
           }
           else {
             newtrkI.setCoord(ix,(*m_position[ip])[0]);
             newtrkI.setCoord(iy,(*m_position[ip])[1]);
           }
         }
       }
       // set the combination counter and increment it by 1
       newtrkI.setTrackID(m_comb_idI++);
       newtrkI.setCombinationID(comb);
       newtrkI.setHWRejected(HWbase);
       newtrkI.setHWTrackID(-1);

       if (m_saveStepByStepTracks) m_tracks_hits.push_back(newtrkI);

       // add one fit in the counters
       if (nmissing==0) m_nfitsI += 1;
       if (nmissing>0) m_nfits_majI += 1;
       if (missPix) m_nfits_majI_pix += 1;
       if (missSCT) m_nfits_majI_SCT += 1;

       /* Do the actual fit - see code in FTKConstantBank::linfit  */
       current_bank_incomplete->linfit(sector,newtrkI);
       newtrkI.setOrigChi2(newtrkI.getChi2());

       bool toAdd(true);

       // exact 0 means no valid constants, skipped
       if( newtrkI.getChi2() == 0.) toAdd = false;
       else if (m_saveStepByStepTracks) m_tracks_pattern.push_back(newtrkI);

       // majority track with bad chisq have no reason to be kept, recovery is not possible
       if (newtrkI.getNMissing() > 0) {
           float dof = m_ncoords_incomplete - m_npars - newtrkI.getNMissing();
           if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof
           float chisqcut = m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux : m_Chi2Cut_maj;
           if (newtrkI.getChi2()>chisqcut)
             toAdd = false;
       }

       if (toAdd)
         theCombos.push_back(newtrkI);

     } // end if !hf_rejected


     /* compose the next combination */
     for (int curpl=0;curpl<nplanes;++curpl) {
       if (m_startlist[curpl]==m_endlist[curpl]) // empty hit
         continue;

       // move to the next
       m_position[curpl]++;  m_hitcnt[curpl]++;
       if (m_position[curpl]!=m_endlist[curpl] && m_hitcnt[curpl]!=m_max_nhitsperplane) {
         // possible combination
         break;
       }
       else {
         // restart on this plane, move the next plane
         m_position[curpl] = m_startlist[curpl];
         m_hitcnt[curpl] = 0;
       }
     }

   } // loop over combinations

   if( m_one_per_road ) {
     if( theCombos.size() != 0 ) {

       // Find the best chi2 track
       float bestfullchi2(theCombos[0].getChi2());
       int idbestfull(0);

       for( unsigned int idx = 1; idx < theCombos.size(); idx++) {
         if( theCombos[idx].getChi2() < bestfullchi2 ) {
           bestfullchi2 =  theCombos[idx].getChi2();
           idbestfull = idx;
         }
       }
       newtrkI = theCombos[idbestfull];
       theCombos.clear();
       theCombos.push_back(newtrkI);
     }
   }

   // In the case of m_do_majority > 1, we only do majority fits if
   // ALL full fits fail the chi2 cut
   bool fullpassed = false;
   if( m_do_majority > 1 ) {
     for( unsigned int idx = 0; idx < theCombos.size(); idx++ ) {
       newtrkI = theCombos[idx];
       if( newtrkI.getNMissing() != 0 ) continue;

       float dof = m_ncoords_incomplete - m_npars - newtrkI.getNMissing(); //Rui's change to match fw
       if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

       if( newtrkI.getChi2() < ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux : m_Chi2Cut ) ) {
         fullpassed = true;
         break;
       }
     }
   }

   for( unsigned int idx = 0; idx < theCombos.size(); idx++ ) {
     newtrkI = theCombos[idx];

     // float dof = m_ncoords_incomplete - m_npars - m_newtrkI.getNMissing();
     float dof = m_ncoords_incomplete - m_npars - newtrkI.getNMissing();
     if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

     // Try to recover majority if chi2 no good
     if (newtrkI.getNMissing()==0 &&
         (m_do_majority==1 || (m_do_majority>1 && !fullpassed)) &&
         // Do recovery if chi2 or chi2/dof is above threshold
         (newtrkI.getChi2() > ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux : m_Chi2Cut )) &&
         // Or veto majority if chi2 is too high
         (newtrkI.getChi2() < m_Chi2Cut_vetomaj || m_Chi2Cut_vetomaj < 0) ) { // recover majority
       /* if the N/N track have a bad chi2 we try to evaluate
          the track using the "best" track within the N combination
          of N-1/N tracks */

       // in this block the used array of track combtrackI[] is created
       // in the init block to save some time

       float bestchi2(newtrkI.getChi2());
       int idbest(-1);

       m_nfits_recI += 1; // ONLY recovery fits

       int firstCombo = m_require_first ? 1:0;
       for (int ip=firstCombo;ip<nplanes;++ip) { // loop over the combinations

         // skip planes with multiple hits
         if (norecovery_mask&(1<<ip)) continue;

         // start for the complete track
         combtrackI[ip] = newtrkI;

         // put an empty cluster in place pf the removed hit
         combtrackI[ip].setFTKHit(ip,FTKHit());

         // remove the hits related with this plane
         int ix = m_pmap_incomplete->getDim(ip,0);
         int iy = m_pmap_incomplete->getDim(ip,1);
         int ndim = iy==-1? 1 : 2;

         // set the number of missing points and the related bitmask
         combtrackI[ip].setNMissing(ndim);
         unsigned int newbitmask = (1<<ix);
         if (iy!=-1) newbitmask |= (1<<iy);
         newbitmask = newtrkI.getBitmask() & ~newbitmask;

         combtrackI[ip].setBitmask(newbitmask);

         m_nfits_addrecI += 1;
         current_bank_incomplete->linfit(sector,combtrackI[ip]);

         // check if the chi2 is better
         if (combtrackI[ip].getChi2()<bestchi2) {
           bestchi2 = combtrackI[ip].getChi2();
           idbest = ip;
         }
       } // end loop over the combinations

       if (idbest>=firstCombo) {
         // a track, excluding the 1st layer, was found
         newtrkI = combtrackI[idbest];
         newtrkI.setTypeMask(1); // set as recovered
       }

     } // end block to recover complete tracks with bad chi2

     dof = m_ncoords_incomplete - m_npars - newtrkI.getNMissing();
     if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

     // check if the track pass the quality requirements
     if (newtrkI.getChi2()< ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux :
                             (newtrkI.getNMissing() > 0 ? dof*m_Chi2DofCutAux : m_Chi2Cut) )  &&
         newtrkI.getChi2() != 0 ) {

       // appending pre-HW tracks to dump
       road_tracks_pre_hw.push_back(newtrkI);

       // to append the found track go trought the HW filter
       // add this track to track list only if
       // don't have common hits with another track,
       // in this case keep the best

       // remains 0 if the track has to be added
       // -1 means is worse than an old track (duplicated)

       int accepted(0);
       // Disable hitwarrior, auto-accept every track
       if (m_HitWarrior!=0)
         accepted = doHitWarriorFilter(newtrkI,road_tracks);

       if (accepted>=0) { // track accepted, no hits shared with already fitted tracks
         // copy newtrkI after truth assignment.
         compute_truth_incomplete(region,road,newtrkI);
         road_tracks.push_back(newtrkI);
         if (accepted==1) { // removed an existing track
           m_nfits_rejI += 1;
           if (nmissing>0) m_nfits_rejmajI += 1;
         }
         else {// is a really new track
           m_ntracks += 1;
         }
       }
       else if (accepted==-1) {
         // the track is candidate as ghost and is not the best
         // clean the list of the hits

         if (m_keep_rejected) {
           // copy newtrk after truth assignment.
           compute_truth_incomplete(region,road,newtrkI);
           road_tracks.push_back(newtrkI);
         }
         m_nfits_rejI += 1;
         if (nmissing>0) m_nfits_rejmajI += 1;
       }
     }
     else { // if the track fail the cut on chi-square clean the hit list
       if (m_keep_rejected>1) {
         newtrkI.setHWRejected( newtrkI.getHWRejected()+100 );
         // copy newtrk after truth assignment.
         compute_truth_incomplete(region,road,newtrkI);
         road_tracks.push_back(newtrkI);
       }
       m_nfits_badI += 1;
       if (nmissing>0) m_nfits_badmajI += 1;
     }
   } // end insertion block
}

/** this method iterates over the tracks found in the incomplete step,
    convert the tracks into complete, extrapolating them, and perform
    the fits using all the existing information */
void TrackFitter711::processor_Extrapolate(const FTKRoad &road,
                                          list<FTKTrack> &road_tracks)
{

  // ---------------------------------------------------------------------------
  // Initialization :
  //
  //
  // get the pointer to the AM road, needed to have information on the SS ids
  const FTKRoad *AMroad = road.getAMRoad();

  // retrieve all the informations for the fits
  int region = road.getRegion(); // a.k.a. bank
  int subreg = road.getSubRegion();
  int sector = road.getSectorID();


  // m_use_SectorDB is true for TRACKFITTER_MODE 1  if true it uses, .conn file.
  // So the following loads the similar sectors if necessary
  if (m_use_SectorDB) {
    if (m_FC_relations[region][subreg]->getNSimilarSectors(sector)==0) {
      // If there are not a single similar sector in this region and subregion print a message and exit
      FTKSetup::PrintMessageFmt(m_noconstants_errlevel, "*** There are no valid extrapolation constants for Sector %d in region %d - %d\n", sector, region, subreg);
      return;
    }
  }

  // set parameters that will remain the same for all the tracks
  newtrk.setBankID(road.getBankID());
  newtrk.setRoadID(road.getRoadID());
  newtrk.setSectorID(road.getSectorID());

  // the base HW flag depends if the road is accepted by the RW
  int HWbase = road.getRWRejected()*10;

  // vector used as buffer to temporary store the found tracks
  std::vector<FTKTrack> theCombos;
  // vector used to retrieve the correspondent simlar sector
  std::vector<pair<int,int> > theCombosInfo;
  // vector used to store the number of missing points int the extension
  std::vector<int> theCombosNMiss;


  //
  // Layer Inversion
  //

  /** This part fixes the issue due to the layer inversion
      in the Endcap between stereo and axial relative position.
      This code loops the check if a SS is in the endcap, vrifies
      if the fix is needed, and fill the conversion maps for this roads.
      It is important to notice that the inversion disappears in the
      unused layers map, used in the road_finder.
  */

  m_incomplete_coordsmask_eff = m_incomplete_coordsmask;
  m_incomplete_planesmask_eff = m_incomplete_planesmask;

  int *missing_section = new int[m_nplanes_ignored];

  for (int i=0;i!=m_nplanes_ignored;++i) {
    // get the logical layer ID of the connected SCT layers
    const int &layerpos = m_idplanes_or[i];

    // this is special hack for DC match, the first N bits should be ignored
    int realSS = AMroad->getDCMatchMode()==1 ? (AMroad->getSSID(layerpos)>>AMroad->getHLID(layerpos)) : AMroad->getSSID(layerpos);
    // isEndcap? : condition.
    bool isEndcap = (realSS%FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode()))>=20;

    if (isEndcap)
      missing_section[i] = realSS%10; // endcap section
    else
      missing_section[i] = 0; // barrel

    if (m_pmap->getPlane(m_idplanes[i],0).getNDimension()!=1)
      missing_section[i] = 0;

    //std::cout << "i " << i << std::endl;
    //std::cout << "layerpos " << layerpos << std::endl;
    //std::cout << "m_idplanes " << m_idplanes[i] << std::endl;
    //std::cout << "isEndcap " << isEndcap << std::endl;
    //std::cout << "m_endcap_inversion " << m_endcap_inversion[i] << std::endl;

    const int &pos = m_idCImap[i];

    if (isEndcap && m_endcap_inversion[i]) {
      // the stereo layer, to extrapolate, is the previous
      m_idplanes_eff[i] = m_idplanes[i]-1;
      m_idcoords_eff[i] = m_idcoords[i]-1;

      // the axial layer is the next
      m_CImap_eff[pos] = m_CImap[pos]+1;
      m_CIlayermap_eff[layerpos] = m_CIlayermap[layerpos]+1;

      // invert 10 in 01 in the coordinates' bitmask
      int coords_to_zero = m_idcoords_eff[i];
      int coords_to_set  = m_idcoords_eff[i] + 1;
      m_incomplete_coordsmask_eff &= ~(1<<(coords_to_zero)); // zeroes the first bit
      m_incomplete_coordsmask_eff |= 1<<(coords_to_set); // set on the next bit

      int planes_to_zero = m_idplanes_eff[i];
      int planes_to_set  = m_idplanes_eff[i] + 1;
      m_incomplete_planesmask_eff &= ~(1<<(planes_to_zero)); // zeroes the first bit
      m_incomplete_planesmask_eff |= 1<<(planes_to_set); // set on the next bit
    } else {
      // barrel or endcap good layers don't need to change the relation
      m_idplanes_eff[i] =  m_idplanes[i];
      m_idcoords_eff[i] =  m_idcoords[i];
      m_CImap_eff[pos] =  m_CImap[pos];
      m_CIlayermap_eff[layerpos] =  m_CIlayermap[layerpos];
    }
  }

  //std::cout << "m_idplanes: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << m_idplanes[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "m_idcoords: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << m_idcoords[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "m_idCImap: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << m_idCImap[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "m_idplanes_or: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << m_idplanes_or[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;

  //std::cout << "m_CImap: {";
  //for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
  //  std::cout << m_CImap[ii] << " , ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "m_CImap_eff: {";
  //for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
  //  std::cout << m_CImap_eff[ii] << " , ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "m_incomplete_coordsmask " << m_incomplete_coordsmask << std::endl;
  //int number = m_incomplete_coordsmask;
  //do {
  //  std::cout << number%2 << " , ";
  //  number = number >> 1;
  //} while(number != 0);

  //std::cout << "m_idplanes_eff: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << m_idplanes_eff[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;
  //
  //std::cout << "missing_section: {";
  //for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
  //  std::cout << missing_section[ii] << ", ";
  //}
  //std::cout << "}" << std::endl;

  // mask to prevent majority in planes with multiple hits or previous hits
  unsigned int norecovery_mask(m_incomplete_planesmask_eff);


  //
  // Start Extrapolation
  //

  // iterator over the incomplete tracks list
  list<FTKTrack>::iterator itrack = road_tracks.begin();

  for (;itrack!=road_tracks.end(); ++itrack) { // incomplete tracks loop
    // reference the incomplete track
    FTKTrack &curtrackI = *itrack;

    // reset the flags
    newtrk.setHWRejected(HWbase);
    newtrk.setHWTrackID(-1);

    //------------------------------------------------------------
    // INCOMPLETE TRACKS PREPRATION
    //------------------------------------------------------------

    // complete track bitmask
    unsigned int cbitmask_real(0);

    /* conventional complete bitmask, this assumes
       all hits for previous steps as real*/
    unsigned int cbitmask(m_incomplete_coordsmask_eff);
    newtrk.setHalfInvPt(curtrackI.getHalfInvPt());
    newtrk.setInvPtFW(round(curtrackI.getInvPtFW()*1e5)/1e5);

    /* Fix from corrgen: in the constant generation, tracks
       in regions 3, 4, and 5 has phi defined in the range [0:2 PI].
       For the extrapolation only checks if the angle is in those
       regions and fix the definition */
    if (m_ssmap_complete->getRegionMap()->getNumRegions()==8) { // 8 Regions, constant generation step
      if (region>=3 && region<=5) { // Regions 3, 4, and 5
        if (curtrackI.getPhi()<=0) { // The angle should be defined in the range [0:2 PI]. Otherwise we fix it.
          newtrk.setPhi(curtrackI.getPhi()+2*M_PI,false);
          newtrk.setPhiFW(round(curtrackI.getPhiFW()+2*M_PI),false);
        }
      }
    }
    else if (m_ssmap_complete->getRegionMap()->getNumRegions()==64) { // 64 Regions, extrapolation step
      if (region==9 || region==10 || region==25 || region==26 || region==41 || region==42 || region==57 || region==58) { // Regions 9, 10, 25, 26, 41, 42, 57, and 58
        if (curtrackI.getPhi()<=0) {
          newtrk.setPhi(curtrackI.getPhi()+2*M_PI,false);
          newtrk.setPhiFW(round(curtrackI.getPhiFW()+2*M_PI),false);
        }
      }
    }
    else { // In other cases we don't have to force phi to be in the range [0:2 PI].
      newtrk.setPhi(curtrackI.getPhi());
      newtrk.setPhiFW(round(curtrackI.getPhiFW()));
    }

    // set the incomplete track with some original info
    newtrk.setIP(curtrackI.getIP());
    newtrk.setIPFW(round(curtrackI.getIPFW()));
    newtrk.setCotTheta(curtrackI.getCotTheta());
    newtrk.setCTheta(round(curtrackI.getCTheta()));
    newtrk.setZ0(curtrackI.getZ0());
    newtrk.setZ0FW(round(curtrackI.getZ0FW()));
    newtrk.setChi2(curtrackI.getChi2());
    newtrk.setChi2FW(round(curtrackI.getChi2FW()));
    newtrk.setOrigChi2(curtrackI.getOrigChi2());
    newtrk.setCombinationID(curtrackI.getCombinationID());

    // zero out coords
    for (int ix=0; ix!=m_ncoords; ++ix) {
      newtrk.setCoord(ix,0);
    }
    //std::cout << "before exp" << std::endl;
    //for ( int i = 0; i < 16; i++)
    //  std::cout << "newtrk.getCoord(" << i << ") = " << newtrk.getCoord(i) << std::endl;

    // set the incomplete track coordinates
    for (int ix=0; ix!=m_ncoords_incomplete; ++ix) {
      int cix = m_CImap_eff[ix];
      newtrk.setCoord(cix,curtrackI.getCoord(ix));

      /* the real mask has 1 only in layers with hits */
      cbitmask_real |= ((curtrackI.getBitmask()&(1<<ix))>>ix)<<cix;
    }

    //std::cout << "before exp" << std::endl;
    //for ( int i = 0; i < 16; i++)
    //  std::cout << "newtrk.getCoord(" << i << ") = " << newtrk.getCoord(i) << std::endl;
        // set the incomplete track hits
    for (int ip=0; ip!=m_nplanes_incomplete; ++ip) {
      int cip = m_CIlayermap_eff[ip];
      newtrk.setFTKHit(cip,curtrackI.getFTKHit(ip));
    }

    // complete the track using the extrapolation method
    // the bitmask and number of missing point is artificial
    newtrk.setBitmask(m_incomplete_coordsmask_eff);
    newtrk.setNMissing(m_ncoords_ignored);

    // set other track flags
    newtrk.setTypeMask(curtrackI.getTypeMask());


    // ===== Incomplete tracks are ready to be extrapolated!


    //------------------------------------------------------------
    // EXTRAPOLATION
    //------------------------------------------------------------

    // simids.first represents the subregion ID, simids.second the sector ID within the bank
    pair<int,int> simids(subreg,sector);

    int conn_subreg(-1);
    int conn_sector(-1);
    int best_connection(-1); // ID of the best connection

    FTKConstantBank *current_bank_complete = 0x0;

    // scheme where the relation between 7L --> 11L is ensured by the synchronization
    if (!m_use_SectorDB) {   // TRACKFITTER_MODE 0

      // "connected" subreg and sector are basically identical : synchronized.
      conn_subreg = subreg;
      conn_sector = sector;
      newtrk.setSectorID(conn_subreg);

      // debugging
      FTKSetup::PrintMessageFmt(m_noconstants_errlevel,"TRACKFITTER_MODE0 reg %d: sector %d: subreg %d: \n",region, sector, subreg);

      // get the bank related to this road
      current_bank_complete = m_constant[region][conn_subreg];

      // Do a sanity check on the retrieved constant bank.
      if (!current_bank_complete) {
         FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                                   "*** Complete constants for region %d - %d don't exists\n",
                                   region,
                                   conn_subreg
            );
         delete [] missing_section;
         return;
      } else if (conn_sector>=current_bank_complete->getNSectors()) {
         // check if the number of sector is compatible with this bank
         FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                                   "*** Complete constants for Sector %d in region %d - %d don't exists\n",
                                   conn_sector,
                                   region,
                                   conn_subreg);
         delete [] missing_section;
         return;
      } else if (!current_bank_complete->getIsGood(conn_sector)) {
      // check if the sector has a valid constants set
        FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
                                  "*** Incomplete constants for Sector %d in region %d - %d is not valid\n",
                                  conn_sector,
                                  region,
                                  conn_subreg
                                  );
    delete [] missing_section;
        return;
      }
    } else {
      // new skim, where the DB with the relation between 7L --> 11L exists
      // nsims : how many similar sectors do we have?
      int nsims = m_FC_relations[region][subreg]->getNSimilarSectors(sector);

      int maxIN(-1); // maximum number of good X coord.

                        if (nsims > m_use_n_conn) // restrict nsims to be equal or below m_use_n_conn
        nsims = m_use_n_conn;

      // loop over similar sectors
      for (int isim=0; isim!=nsims&&maxIN!=m_nplanes_ignored; ++isim) {

        //const int &tmpconn_subreg = m_FC_relations[region][subreg]->getSimilarSubID(sector,isim); // TODO: Athena doesn't like several files for subregion constant sets.
        const int &tmpconn_subreg = subreg;
        int tmpconn_sector = sector;
        if (m_use_multiple_conn)
                tmpconn_sector = m_FC_relations[region][subreg]->getSimilarSecID(sector,isim);

        // get the bank related to this road
        current_bank_complete = m_constant[region][tmpconn_subreg];

        // Sanity check on retrieved bank for this specific sector
        if (!current_bank_complete) {
          FTKSetup::PrintMessageFmt(ftk::info,
                                    "*** Complete constants for region %d - %d don't exists\n",
                                    region,
                                    tmpconn_subreg
                                    );

          continue;
        } else if (tmpconn_sector>=current_bank_complete->getNSectors()) {
        // check if the number of sector is compatible with this bank
          FTKSetup::PrintMessageFmt(ftk::info,
                                    "*** Complete constants for Sector %d in region %d - %d don't exists\n",
                                    tmpconn_sector,
                                    region,
                                    tmpconn_subreg
                                    );

          continue;
        } else if (!current_bank_complete->getIsGood(tmpconn_sector)) {
        // check if the sector has a valid constants set
          FTKSetup::PrintMessageFmt(ftk::info,
                                    "*** Incomplete constants for Sector %d in region %d - %d is not valid\n",
                                    tmpconn_sector,
                                    region,
                                    tmpconn_subreg
                                    );

          continue;
        }

        // ---------------------------------------
        // Do the extrapolation for this similar sector and see if this is any better than the other.
        // The first method is deprecated. No more geometrical extrapolation
        //
        if (!m_use_guessing)
          current_bank_complete->extrapolate_coords(newtrk, tmpconn_sector, m_idcoords_eff, m_idpars);
        else
          current_bank_complete->missing_point_guess(newtrk, tmpconn_sector);

        // Counter that counts how many good extrapolation
        int nIN(0);

        // Count how many good X
        for (int ip=0; ip<m_nplanes_ignored; ++ip) {
          const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
          if (ndim==1) {
            const float &localX = newtrk.getCoord(m_idcoords_eff[ip]);

            // If the following condition is met count as good local X
            if (localX>=0 && localX <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth)
              nIN += 1;
          } else if (ndim==2) {
            const float &localX = newtrk.getCoord(m_idcoords_eff[ip]);
            const float &localY = newtrk.getCoord(m_idcoords_eff[ip]+1);

            // If the following condition is met count as good local X
            //std::cout << "missing_section["<< ip <<"]: " << missing_section[ip] << std::endl;
            //std::cout << "m_idplanes_eff ["<< ip <<"]: " << m_idplanes_eff[ip] << std::endl;
            if (localX>=0 && localX <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth) {
              if (localY>=0 && localY <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 1).m_etawidth)
                nIN += 1;
            }
          }
        }

        // here counts the number of out-boundary X and decide if it is the best
        if (nIN>maxIN) {
          simids.first  = tmpconn_subreg;
          simids.second = tmpconn_sector;
          best_connection = isim;
          maxIN = nIN;
        }
      } // Looping over similar sector is done.

      // **************** Note: First Exit Point !!! ********************
      if (maxIN==-1) // no valid sectors
      {
        //printf("Exit point 1\n");
         delete [] missing_section;
        return;
      }
      // **************** Note: First Exit Point !!! ********************

      if (maxIN<m_nplanes_ignored)
      {
        FTKSetup::PrintMessageFmt(ftk::info,"Extrapolation with out boundary X (%d)\n",m_nplanes_ignored-maxIN);
      }

      // set the variables to extrapolate the variables
      conn_subreg = simids.first;
      conn_sector = simids.second;
      newtrk.setSectorID(conn_subreg);

      current_bank_complete = m_constant[region][conn_subreg];
    }




    //------------------------------------------------------------
    // The extrapolation is done right here.
    // Two types of method
    // the missing_point_guess
    //------------------------------------------------------------
    if (!m_use_guessing)  // Geometrical extrapolation
      current_bank_complete->extrapolate_coords(newtrk, conn_sector, m_idcoords_eff, m_idpars);
    else
      current_bank_complete->missing_point_guess(newtrk,conn_sector);


    //------------------------------------------------------------
    // SSMAP COORDINATE -> SSID
    //------------------------------------------------------------
    //

    int *ssid = new int[m_nplanes_ignored];
    int **bounds = new int*[m_nplanes_ignored];

    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {

      const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();

      bounds[ip] = new int[2*ndim];

      /* use the module id to extract the phi id and the eta id,
         in particular for modules the ID is phiid*1000+etaid */
      const int &moduleID = m_FC_relations[region][subreg]->getSimilarStereoIDs(sector,best_connection)[ip];
      float phiwindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_phiwidth - 0.001;
      float etawindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_etawidth - 0.001;

      // Alternative way
      FTKHit* tmpHit = new FTKHit(ndim);
      tmpHit->setSector(moduleID);
      for (int i=0;i<ndim;++i)
        tmpHit->setCoord(i, newtrk.getCoord(m_idcoords_eff[ip]+i));
      tmpHit->setSector(moduleID);
      tmpHit->setEtaWidth(etawindow);
      tmpHit->setPhiWidth(phiwindow);
      tmpHit->setBankID(region);
      tmpHit->setPlane(m_idplanes_eff[ip]);

      ssid[ip] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);

      //std::cout << ssid[ip] << std::endl;

      if (ndim == 2) {
        // modify coordinates and get the bounds
        tmpHit->setCoord(0,0);         bounds[ip][0] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
        tmpHit->setCoord(0,phiwindow); bounds[ip][1] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
        // restore the x-coord
        tmpHit->setCoord(0,m_idcoords_eff[ip]);
        // now vary y-cord
        tmpHit->setCoord(1,0);         bounds[ip][2] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
        tmpHit->setCoord(1,etawindow); bounds[ip][3] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
      } else if (ndim == 1) {
        // modify coordinates and get the bounds
        tmpHit->setCoord(0,0);         bounds[ip][0] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
        tmpHit->setCoord(0,phiwindow); bounds[ip][1] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
      }

      delete tmpHit;

      //int phimod;
      //int etaoff;
      //int phioff;
      //int oldLocalSS;

      //phimod = moduleID/1000;
      //etaoff = moduleID%1000;


      //// Use the module parameters to extract the new SS
      //const float& localX = newtrk.getCoord(m_idcoords_eff[ip]);

      //// Print error message
      //if (localX<0 || localX>m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth)
      //  FTKSetup::PrintMessageFmt(ftk::warn, "Warning: coordinate %d = %f\n", ip, localX);

      ///* get central SS ID and the module boundaries, in SS gage.
      //   the boundaries are used to avoid search on SS near modules */

      //float phiwindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_phiwidth;
      //float etawindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_etawidth;
      //// The localX area
      //if (ndim == 2) {
      //  // if ndim == 2 also load y axis coord.
      //  std::cout << "2nd coord:" << m_idcoords_eff[ip]+1 << std::endl;
      //  const float& localY = newtrk.getCoord(m_idcoords_eff[ip]+1);

      //  if (localY<0 || localY>m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 1).m_etawidth)
      //    FTKSetup::PrintMessageFmt(ftk::warn, "Warning: coordinate %d = %f\n", ip+1, localY);

      //  ssid[ip] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, localX, localY);
      //  bounds[ip][0] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, 0        , localY);
      //  bounds[ip][1] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, phiwindow, localY);
      //  bounds[ip][2] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, localX   , 0);
      //  bounds[ip][3] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, localX   , etawindow);
      //}
      //else if (ndim == 1) {
      //  ssid[ip] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, localX);
      //  bounds[ip][0] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, 0);
      //  bounds[ip][1] = m_ssmap_complete->getSS(m_idplanes_eff[ip], missing_section[ip], phimod, etaoff, phiwindow);
      //}
    }

    //std::cout <<"Checking NHits in unused SS: m_nregions:"<<m_nregions<<std::endl;
    //for (int ibank=0; ibank<m_nregions; ++ibank) {
    //  FTKRoadStream* roadstream = m_roadinput->getStream(ibank);
    //  if (!roadstream)
    //    continue;
    //  const std::map<int, std::map<int,FTKSS> > unused_ssmap = roadstream->getUnusedSSMap();
    //  std::map<int, std::map<int,FTKSS> >::const_iterator unused_ssmap_iter;
    //  unused_ssmap_iter = unused_ssmap.begin();
    //  for (;unused_ssmap_iter != unused_ssmap.end(); ++unused_ssmap_iter) {
    //    std::map<int,FTKSS>::const_iterator unused_ss_iter = ((*unused_ssmap_iter).second).begin();
    //    for (;unused_ss_iter != ((*unused_ssmap_iter).second).end(); ++unused_ss_iter) {
    //      std::cout << " in : " << ((*unused_ss_iter).first) << " There are:  " << ((*unused_ss_iter).second).getNHits() << std::endl;
    //      std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getEtaCode() <<std::endl;
    //      std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getPhiModule() <<std::endl;
    //      std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getSector() <<std::endl;
    //    }
    //  }
    //}


    //------------------------------------------------------------
    // SSIDs -> Obtain Hit IDs
    //------------------------------------------------------------

    //std::cout << "Searching hits ...." << std::endl;

    // get the list of hits
    vector<FTKHit> *hits_more = new vector<FTKHit>[m_nplanes_ignored];

    // loop to retrieve the hit lists
    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
      hits_more[ip].clear();

      // the hits are obtained from the extrapolated SS and the neighbor
      for (int ss_shift=-m_nneighbours/2; ss_shift!=m_nneighbours/2+1; ++ss_shift) {
        const int tmpSS = ssid[ip]+ss_shift*FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode());
  
        // skip SS out of the current module boundaries
        if (/*(ndim==1)&&*/(tmpSS<bounds[ip][0]||tmpSS>bounds[ip][1])) // TODO:
          continue;
        //else if ((ndim==2)&&(tmpSS<bounds[ip][2]||tmpSS>bounds[ip][3])) // TODO: implement proper eta window check
        //  continue;

        /* refers to the unused layers with the general relation
           used in the barrel, doesn't take into account the possible
           inversion in some endcap layer */
        const FTKSS &ss = m_roadinput->getUnusedSS(region, m_idplanes[ip], tmpSS);

        std::cout << " in : " << tmpSS << " is there a hit?: " << !(ss.getHits().empty()) << std::endl; // TODO: debug print output

        if (!ss.getHits().empty())
          hits_more[ip].insert(hits_more[ip].end(), ss.getHits().begin(), ss.getHits().end());
      }

    } // end loop to retrieve the hit lists

    // Delete unused objects
    delete [] ssid;
    for (int ip=0;ip!=m_nplanes_ignored;++ip)
      delete [] bounds[ip];
    delete [] bounds;

    //---------------------------------------------------------
    // Linear fit
    //---------------------------------------------------------

    // compute the additional combinations to complete the fits
    int ncombs_more(1);

    // number of hits and the stereo layers
    int nplanes_more(0);
    int ncoords_more(0);

    // reduced bitmask for the extapolated layers
    unsigned int bitmask_more(0);

    vector<FTKHit>::iterator *curhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];
    vector<FTKHit>::iterator *endhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];
    bool missPix(false), missSCT(false);

    int ic(0);
    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
      //std::cout << " --- # of hits in each layers --- " << std::endl; //TODO: debug print output remove
      const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
      for (int i=0;i!=ndim;++i,++ic) {
        //std::cout << ic << " " << hits_more[ip].size(); //TODO: debug print output remove
        //for (unsigned ihits=0;ihits<hits_more[ip].size();++ihits) {
        //  std::cout << " " << (*hits_more)[ip];
        //}
        //std::cout << std::endl;
        // move the iterator at the begin of the list
        curhit_more[ip] = hits_more[ip].begin();
        endhit_more[ip] = hits_more[ip].end();


        if (curhit_more[ip]!=endhit_more[ip]) {
          // icrement the number of combinations
          ncombs_more *= hits_more[ip].size();

          // incremente the number of not empty layers
          if (i!=1)
            nplanes_more += 1;
          ncoords_more += 1;

          // set the reduced bitmask
          bitmask_more |= 1<<ic;

          // set the complete track bitmask, real and conventional
          cbitmask |= (1<<(m_idcoords_eff[ip]+i));
          cbitmask_real |= (1<<(m_idcoords_eff[ip]+i));
        } else {
    if (ndim == 1) missSCT = true;
    else missPix = true;
          // in this case the extrapolation failed, some empty value is placed
          newtrk.setFTKHit(m_idplanes_eff[ip],FTKHit()); // set an empty hit
        }
      }
    }

    // quality check to verify a sufficient number of hits
    int nmissing_more(m_nplanes_ignored-nplanes_more);
    int nmissing_more_coord(m_ncoords_ignored-ncoords_more);
    //std::cout << "nmissing_more:"  <<nmissing_more << std::endl;

    if (!m_extrafits && nmissing_more<=m_max_missing_extraplanes) {
      /*
       * In this case the track will be accepted assuming the layers with
       * hits can be succefully linked to this track, quality check are done
       * only the number of layers with real hits but no further fits are
       * performed
       */

      // set the correct number of missing points and
      // and the real bitmask
      newtrk.setTrackID(m_comb_id++);
      newtrk.setNMissing(curtrackI.getNMissing()+nmissing_more_coord);
      //FTKSetup::PrintMessageFmt(ftk::info,"Track accepted");
      // match the truth only using the partial information in the layers within the road
      TrackFitter::compute_truth(region,road,curtrackI);
      newtrk.setBarcode(curtrackI.getBarcode());
      newtrk.setBarcodeFrac(curtrackI.getBarcodeFrac());
      newtrk.setBitmask(cbitmask_real);

      m_nfits += 1;
      theCombos.push_back(newtrk);
      theCombosInfo.push_back(simids);
      theCombosNMiss.push_back(nmissing_more_coord);
    } else if (nmissing_more<=m_max_missing_extraplanes) {
      /*
       * Do additional fits using the hits in the SCT layers not used
       * in the first step
       */

      // the number of missing points depends only by the last step,
      // the MJ hits in the first step are used as real hits
      newtrk.setNMissing(nmissing_more_coord);

      // set the conventional bitmask for the fit
      newtrk.setBitmask(cbitmask);

      // increment the number of addional fits
      m_ncombs += ncombs_more;

      // loop over the additional combination, choosing only the tracks sutisfying the quality requirements
      for (int icomb_more=0; icomb_more!=ncombs_more; ++icomb_more) {
        // complete fit loop
        int ic(0);
        for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
          const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
          for (int i=0;i!=ndim;++i,++ic) {
            if (!(bitmask_more&(1<<ic)))
              continue; // skip empty planes

            // the missing layers are assumed to be 1-D
            const int &cid = m_idcoords_eff[ip]+i;
            newtrk.setFTKHit(m_idplanes_eff[ip],*curhit_more[ip]);
            newtrk.setCoord(cid,(*curhit_more[ip])[i]);


          }
        }

        //for ( int i = 0; i < 16; i++)
        //  std::cout << "newtrk.getCoord(" << i << ") = " << newtrk.getCoord(i) << std::endl;

        if (nmissing_more==0)m_nfits += 1;
        //std::cout << "m_nfits: " << m_nfits << std::endl;

        if (nmissing_more>0) {
          m_nfits_maj += 1;
    if (missPix) m_nfits_maj_pix += 1;
    if (missSCT) m_nfits_maj_SCT += 1;
  }

        // perform the fit
        //newtrk.setTrackID(m_comb_id++);
        newtrk.setTrackID(curtrackI.getTrackID());
        newtrk.setExtrapolationID(icomb_more);
        current_bank_complete->linfit(conn_sector,newtrk);
        //std::cout << " Chi2 " <<newtrk.getChi2() << std::endl;
        newtrk.setOrigChi2(newtrk.getChi2());

        // set the correct number of missing points and
        // and the real bitmask
        newtrk.setNMissing(curtrackI.getNMissing()+nmissing_more_coord);
        newtrk.setBitmask(cbitmask_real);

        // match the track to a geant particle using the channel-level
        // geant info in the superstrip data.
        compute_truth(region,road,curtrackI,hits_more,newtrk);
        theCombos.push_back(newtrk);
        theCombosInfo.push_back(simids);
        theCombosNMiss.push_back(nmissing_more_coord);
        //FTKSetup::PrintMessageFmt(ftk::info,"Track accepted");
        // go to the next combination
        for (int ip=0; ip!=m_nplanes_ignored; ++ip)
        {
          if (!(bitmask_more&(1<<ip)))
            continue; // skip empty planes

          // move the hit in this plane
          curhit_more[ip]++;

          if (curhit_more[ip]!=endhit_more[ip])
            break;
          else // rewind this hits and move the next plane
            curhit_more[ip] = hits_more[ip].begin();
        }

      } // end complete fit loop
    } // end block to complete the fits
    else if (m_keep_rejected)
    {
      /*
       * Keep the track only if debug informations are required
       */

      // set the correct number of missing points and
      // and the real bitmask
      newtrk.setTrackID(m_comb_id++);
      newtrk.setNMissing(curtrackI.getNMissing()+nmissing_more_coord);

      // match the truth only using the partial information in the layers within the road
      TrackFitter::compute_truth(region,road,curtrackI);
      newtrk.setBarcode(curtrackI.getBarcode());
      newtrk.setBarcodeFrac(curtrackI.getBarcodeFrac());
      newtrk.setBitmask(cbitmask_real);
      newtrk.setHWRejected(newtrk.getHWRejected()+1000);
      theCombos.push_back(newtrk);
      theCombosInfo.push_back(simids);
      theCombosNMiss.push_back(nmissing_more_coord);
    }

    delete [] hits_more;
    delete [] curhit_more;
    delete [] endhit_more;

  } // end incomplete tracks loop

  // section information unused after this point
  delete [] missing_section;

  //std::cout << "one_per_road " << m_one_per_road << std::endl;

  // If it requres only one track per road then choose the best chi2 track
  if( m_one_per_road && theCombos.size() != 0 )
  {
    // Find the best chi2 track
    float bestfullchi2(theCombos[0].getChi2());
    int idbestfull(0);

    for(unsigned int idx=1; idx < theCombos.size(); idx++)
    {
      if(theCombos[idx].getChi2() < bestfullchi2)
      {
        bestfullchi2 =  theCombos[idx].getChi2();
        idbestfull = idx;
      }
    }

    newtrk = theCombos[idbestfull];
    theCombos.clear();
    theCombos.push_back(newtrk);
  }
  // In the case of m_do_majority > 1, we only do majority fits if
  // ALL full fits fail the chi2 cut
  bool fullpassed = false;

  if( m_do_majority > 1 )
  {
    for( unsigned int idx = 0; idx < theCombos.size(); idx++ )
    {
      newtrk = theCombos[idx];
      if( newtrk.getNMissing() != 0 )
        continue;

      float dof = m_ncoords - m_npars - newtrk.getNMissing();
      if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

      if( newtrk.getChi2() < ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ) )
      {
        fullpassed = true;
        break;
      }
    }
  }


  // -------------------------------------------------------------------------
  // N-TRACKS -> SAVE GOOD ONES
  // -------------------------------------------------------------------------
  //
  for( unsigned int idx = 0; idx < theCombos.size(); idx++ )
  {

    // retrive the information on the stored track
    newtrk = theCombos[idx];
    pair<int,int> simids = theCombosInfo[idx];
    int nmissing_more = theCombosNMiss[idx];

    float dof = m_ncoords - m_npars - newtrk.getNMissing();
    if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof


    //if (m_extrafits && nmissing_more==0 &&
    //    (m_do_majority==1 || (m_do_majority>1 && !fullpassed)) &&
    //    // Do recovery if chi2 or chi2/dof is above threshold
    //    (newtrk.getChi2() > ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut )) &&
    //    // Or veto majority if chi2 is too high
    //    (newtrk.getChi2() < m_Chi2Cut_vetomaj || m_Chi2Cut_vetomaj < 0) )

    // -------------------------------------------------------
    // Boolean question
    // Do majority recover??
    // -------------------------------------------------------
    // Try to recover majority if chi2 no good
    bool do_majority_recover = m_extrafits && nmissing_more==0;
    do_majority_recover = do_majority_recover && (m_do_majority==1 || (m_do_majority>1 && !fullpassed));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() > ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() > ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() < m_Chi2Cut_vetomaj || m_Chi2Cut_vetomaj < 0);


    if (do_majority_recover)
    { // recover majority
      /* if the N/N track have a bad chi2 we try to evaluate
         the track using the "best" track within the N combination
         of N-1/N tracks */
      //      FTKSetup::PrintMessageFmt(ftk::info,"pass majority cut");
      // in this block the used array of track combtrack[] is created
      // in the init block to save some time

      float bestchi2(newtrk.getChi2());
      int idbest(-1);
      m_nfits_rec += 1;
      const int recbegin = m_require_first ? 1 : 0;

      // loop over the combinations
      for (int ip=recbegin;ip<m_pmap->getNPlanes();++ip)
      {

        // skip planes with multiple hits
        if (norecovery_mask&(1<<ip))
          continue;

        // start for the complete track
        combtrack[ip] = newtrk;

        // reset the correspondent hit
        combtrack[ip].setFTKHit(ip,FTKHit());

        // remove the hits related with this plane
        int ix = m_pmap->getDim(ip,0);
        int iy = m_pmap->getDim(ip,1);
        int ndim = iy==-1? 1 : 2;

        // set the number of missing points and the related bitmask
        combtrack[ip].setNMissing(ndim);
        unsigned int newbitmask = (1<<ix);

        if (iy!=-1) newbitmask |= (1<<iy);

        newbitmask = m_complete_mask & ~newbitmask;

        combtrack[ip].setBitmask(newbitmask);

        FTKConstantBank *current_bank_complete = m_constant[region][simids.first];
        m_nfits_addrec += 1;
        current_bank_complete->linfit(simids.second,combtrack[ip]);

        // check if the chi2 is better
        if (combtrack[ip].getChi2()<bestchi2)
        {
          bestchi2 = combtrack[ip].getChi2();
          idbest = ip;
        }
      } // end loop over the combinations

      if (idbest>=recbegin) {
        // a track, excluding the 1st layer, was found
        // set the correct flags
        combtrack[idbest].setTypeMask(newtrk.getTypeMask()|(1<<1));
        combtrack[idbest].setNMissing(newtrk.getNMissing()+combtrack[idbest].getNMissing());
        combtrack[idbest].setBitmask(newtrk.getBitmask() & combtrack[idbest].getBitmask());
        newtrk = combtrack[idbest];
      }

    } // end block to recover complete tracks with bad chi2

    dof = m_ncoords - m_npars - newtrk.getNMissing();
    if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

    // check if the track pass the quality requirements
    if (newtrk.getChi2()< ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB :
          (newtrk.getNMissing() > 0 ? m_Chi2Cut_maj : m_Chi2Cut) )  &&
        newtrk.getChi2() != 0 )
    {

      // to append the found track go trought the HW filter
      // add this track to track list only if
      // don't have common hits with another track,
      // in this case keep the best

      // remains 0 if the track has to be added
      // -1 means is worse than an old track (duplicated)

      int accepted(0);

      // Disable hitwarrior, auto-accept every track
      if (m_HitWarrior!=0)
        accepted = doHitWarriorFilter(newtrk,m_tracks);

      //std::cout<<"TH: I accepted = "<<accepted<<std::endl;
      if (accepted>=0) // track accepted, no hits shared with already fitted tracks
      {
        // copy newtrkI after truth assignment.
        m_tracks.push_back(newtrk);

        if (accepted==1) // removed an existing track
        {
          m_nfits_rej += 1;
          if (nmissing_more>0)
            m_nfits_rejmaj += 1;
        }
      }
      else if (accepted==-1)
      {
        // the track is candidate as ghost and is not the best
        // clean the list of the hits

        if (m_keep_rejected)
        {
          // copy newtrk after truth assignment.
          m_tracks.push_back(newtrk);
        }

        m_nfits_rej += 1;

        if (nmissing_more>0)
          m_nfits_rejmaj += 1;
      }
    }
    else // if the track fail the cut on chi-square clean the hit list
    {
      if (m_keep_rejected>1)
      {
        newtrk.setHWRejected( newtrk.getHWRejected()+100 );

        // copy newtrk after truth assignment.
        m_tracks.push_back(newtrk);
      }

      m_nfits_bad += 1;

      if (nmissing_more>0)
        m_nfits_badmaj += 1;
    }
  } // end insertion block
}


void
TrackFitter711::compute_truth(const unsigned int& /*ibank*/,const FTKRoad& ,
                              FTKTrack &) const {
  FTKSetup::PrintMessage(ftk::sevr, "The compute_truth method for TrackFitter711 needs 5 arguments");
}

void
TrackFitter711::compute_truth(const unsigned int& /*ibank*/,const FTKRoad& road,
                              FTKTrack &track_incomplete,
                              vector<FTKHit> *hits_more,
                              FTKTrack &track_complete) const
{
  const bool debug = false;
  // retrieve the superstrips for this road and recover the truth
  // data for the hit used on the superstrip.
  static vector<MultiTruth> mtv;
  mtv.clear();
  mtv.reserve( road.getNPlanes() );
  if( debug ) {
    // dump all ssid coordinates and all track coordinates.
    cout << " FINAL ROAD SS DUMP: pl ssid c0" << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ssid = road.getDCMatchMode()==1 ? road.getSSID(i)>>road.getHLID(i) : road.getSSID(i);
      const std::vector<FTKHit>& hits(road.getHits(i));
      for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
        const FTKHit& hit = *ihit;
        cout << i << " " << ssid << " " << hit[0] << endl;
        unsigned int index = std::distance( hits.begin() , ihit );
        road.getTruth(i,index).display();
      }
    }
    cout << " FINAL TRACK COORD DUMP: pl bitmask_bit c0" << endl;
    cout << " bitmask: " << track_complete.getBitmask() << endl;
    cout << " nmissing: " << track_complete.getNMissing() << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ix = m_pmap->getDim(i,0);
      const float xcoord = track_incomplete.getCoord(ix);
      const bool has_hit = (track_incomplete.getBitmask() & (1 << ix));
      cout << i << " " << has_hit << " " << xcoord << endl;
    }
  } // end if dump debugging info to stdout
  /*
   * The loop to match the hits is divided in two steps:
   * 1. loop over the incomplete track's hits, trying the match with the road's hits
   * 2. loop over the hits in the extrapolated SS with the complete track
   */
  for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) { // road hits matching loop
    const std::vector<FTKHit>& hits(road.getHits(i));
    // retrieve the coordinate in the final array
    const int ix = m_pmap_incomplete->getDim(i,0);
    const float xcoord = track_incomplete.getCoord(ix);
    // find the hit index into the superstrip for this
    // coordinate. this is tricky---only the coordinate is
    // saved, so it is the only way we have of identifying
    // which geant truth data to use.
    std::vector<FTKHit>::const_iterator match_hit=hits.end();
    for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
      const FTKHit& hit = *ihit;
      assert( hit.getDim()>=1 );
      if( true && (hit.getDim()==2) && (hit[0]==xcoord) && (hit[1]==track_incomplete.getCoord(m_pmap_incomplete->getDim(i,1))) ) {
        match_hit = ihit;
        break;
      } else if( hit[0] == xcoord ) {
        match_hit = ihit;
        break;
      }
    }
    // by construction, superstrip must contain the hit used on this track, or
    // the track must not have a hit in this plane.
    if( (track_incomplete.getBitmask() & (1 << ix)) == 0 ) { continue; } // no hit in this plane
    assert( match_hit != hits.end() );
    unsigned int index = std::distance( hits.begin() , match_hit );
    assert( index<=static_cast<unsigned int>(road.getNHits(i)) );
    // adjust weight for hits without (and also with) a truth match, so that each is counted with the same weight.
    //MultiTruth this_mt( ss.getTruth(index) );
    MultiTruth this_mt( (*match_hit).getTruth() );
    this_mt.assign_equal_normalization();
    mtv.push_back( this_mt );
    if( debug ) {
      cout << "added MultiTruth for " << index << " ";
      this_mt.display();
    }
  } // end road hits matching loop

  for( int i=0; i!=m_nplanes_ignored; ++i ) { // additional hits match
    // retrieve the coordinate in the final array
    //const int ix = m_pmap->getDim(m_idplanes[i],0);
    const int ndim = m_pmap->getPlane(m_idplanes[i],0).getNDimension();
    const int ix = m_idcoords_eff[i];
    const float xcoord = track_complete.getCoord(ix);
    float ycoord(0);
    if (ndim==2)
      ycoord = track_complete.getCoord(ix+1);
    // find the hit index into the superstrip for this
    // coordinate. this is tricky---only the coordinate is
    // saved, so it is the only way we have of identifying
    // which geant truth data to use.
    std::vector<FTKHit>::iterator match_hit=hits_more[i].end();
    for( std::vector<FTKHit>::iterator ihit=hits_more[i].begin(), fhit=hits_more[i].end(); ihit!=fhit; ++ihit ) {
      const FTKHit& hit = *ihit;
      assert( hit.getDim()>=1 );
      if( true && (hit.getDim()==2) && (hit[0]==xcoord) && (hit[1]==ycoord) ) {
        match_hit = ihit;
        break;
      } else if( hit[0] == xcoord ) {
        match_hit = ihit;
        break;
      }
      //if( true && (hit.getDim()==2) && (hit[0]==xcoord) && (hit[1]==track_incomplete.getCoord(m_pmap_incomplete->getDim(m_idplanes[i],1))) ) {
      //  match_hit = ihit;
      //  break;
      //} else if( hit[0] == xcoord ) {
      //  match_hit = ihit;
      //  break;
      //}
    }
    // by construction, superstrip must contain the hit used on this track, or
    // the track must not have a hit in this plane.
    if( (track_complete.getBitmask() & (1 << ix)) == 0 ) { continue; } // no hit in this plane
    assert( match_hit != hits_more[i].end() );
    unsigned int index = std::distance( hits_more[i].begin() , match_hit );
    assert(index <= hits_more[i].size() );
    // adjust weight for hits without (and also with) a truth match, so that each is counted with the same weight.
    MultiTruth this_mt( hits_more[i][index].getTruth() );
    this_mt.assign_equal_normalization();
    mtv.push_back( this_mt );
    if( debug ) {
      cout << "added MultiTruth for " << index << " ";
      this_mt.display();
    }
  } // end additional hits match

  // compute the best geant match, the barcode with the largest number of hits contributing to the track.
  // frac is then the fraction of the total number of hits on the track attributed to the barcode.
  MultiTruth mt( std::accumulate(mtv.begin(),mtv.end(),MultiTruth(),MultiTruth::AddAccumulator()) );
  // retrieve the best barcode and frac and store in the track.
  MultiTruth::Barcode tbarcode;
  MultiTruth::Weight tfrac;
  const bool ok = mt.best(tbarcode,tfrac);
  if( ok ) {
    track_complete.setEventIndex(tbarcode.first);
    track_complete.setBarcode(tbarcode.second);
    track_complete.setBarcodeFrac(tfrac);
  }
  // dump matching info to stdout
  if( debug ) {
    mt.display();
    cout << " TRACK MATCHED TO " << tbarcode.first << "," << tbarcode.second << " " << tfrac << " " << mtv.size() << endl;
    std::for_each( mtv.begin() , mtv.end() , mem_fun_ref( &MultiTruth::display ) );
  }
}

void
TrackFitter711::compute_truth_incomplete(const unsigned int& /*ibank*/,const FTKRoad& road,FTKTrack& newtrk) const
{
  const bool debug = false;
  // retrieve the superstrips for this road and recover the truth
  // data for the hit used on the superstrip.
  static vector<MultiTruth> mtv;
  mtv.clear();
  mtv.reserve( road.getNPlanes() );
  if( debug ) {
    // dump all ssid coordinates and all track coordinates.
    cout << " FINAL ROAD SS DUMP: pl ssid c0" << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ssid = road.getDCMatchMode()==1 ? road.getSSID(i)>>road.getHLID(i) : road.getSSID(i);
      const std::vector<FTKHit>& hits(road.getHits(i));
      for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
        const FTKHit& hit = *ihit;
        cout << i << " " << ssid << " " << hit[0] << endl;
        unsigned int index = std::distance( hits.begin() , ihit );
        road.getTruth(i,index).display();
      }
    }
    cout << " FINAL TRACK COORD DUMP: pl bitmask_bit c0" << endl;
    cout << " bitmask: " << newtrk.getBitmask() << endl;
    cout << " nmissing: " << newtrk.getNMissing() << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ix = m_pmap_incomplete->getDim(i,0);
      const float xcoord = newtrk.getCoord(ix);
      const bool has_hit = (newtrk.getBitmask() & (1 << ix));
      cout << i << " " << has_hit << " " << xcoord << endl;
    }
  } // end if dump debugging info to stdout
  for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
    //const int ssid = road.getSSID(i);
    //FTKSS& ss = const_cast<FTKSS&>( m_roadinput->getSS(ibank,i,ssid) );
    //const std::vector<FTKHit>& hits(ss.getHits());
    const std::vector<FTKHit>& hits = road.getHits(i);
    // retrieve the coordinate in the final array
    const int ix = m_pmap_incomplete->getDim(i,0);
    const float xcoord = newtrk.getCoord(ix);
    // find the hit index into the superstrip for this
    // coordinate. this is tricky---only the coordinate is
    // saved, so it is the only way we have of identifying
    // which geant truth data to use.
    std::vector<FTKHit>::const_iterator match_hit=hits.end();
    for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
      const FTKHit& hit = *ihit;
      assert( hit.getDim()>=1 );
      if( true && (hit.getDim()==2) && (hit[0]==xcoord) && (hit[1]==newtrk.getCoord(m_pmap_incomplete->getDim(i,1))) ) {
        match_hit = ihit;
        break;
      } else if( hit[0] == xcoord ) {
        match_hit = ihit;
        break;
      }
    }
    // by construction, superstrip must contain the hit used on this track, or
    // the track must not have a hit in this plane.
    if( (newtrk.getBitmask() & (1 << ix)) == 0 ) { continue; } // no hit in this plane
    assert( match_hit != hits.end() );
    //unsigned int index = std::distance( hits.begin() , match_hit );
    //assert( static_cast<int>(index)<=ss.getNHits() );
    // adjust weight for hits without (and also with) a truth match, so that each is counted with the same weight.
    //MultiTruth this_mt( ss.getTruth(index) );
    MultiTruth this_mt( (*match_hit).getTruth() );
    this_mt.assign_equal_normalization();
    mtv.push_back( this_mt );
    if( debug ) {
      unsigned int index = std::distance( hits.begin() , match_hit );
      cout << "added MultiTruth for " << index << " ";
      this_mt.display();
    }
  }
  // compute the best geant match, the barcode with the largest number of hits contributing to the track.
  // frac is then the fraction of the total number of hits on the track attributed to the barcode.
  MultiTruth mt( std::accumulate(mtv.begin(),mtv.end(),MultiTruth(),MultiTruth::AddAccumulator()) );
  // retrieve the best barcode and frac and store in the track.
  MultiTruth::Barcode tbarcode;
  MultiTruth::Weight tfrac;
  const bool ok = mt.best(tbarcode,tfrac);
  if( ok ) {
    newtrk.setEventIndex(tbarcode.first);
    newtrk.setBarcode(tbarcode.second);
    newtrk.setBarcodeFrac(tfrac);
  }
  // dump matching info to stdout
  if( debug ) {
    mt.display();
    cout << " TRACK MATCHED TO " << tbarcode.first << "," << tbarcode.second << " " << tfrac << " " << mtv.size() << endl;
    std::for_each( mtv.begin() , mtv.end() , mem_fun_ref( &MultiTruth::display ) );
  }
}

/** this processor method type take as input complete roads,
    than evaluate, for each 11L combination:
    - 11L fit parameters
    - 7L fit parameters
    - Using the 7L fit parameters the resolution in the extrapolation
*/

// void TrackFitter711::processor_ResolutionMode(const FTKRoad &road,
//                                               list<FTKTrack> &road_tracks) {
//   return;
// }

/* JAA has commented this out since it has bugs and isn't used anymore. We should remove it in the future! */
/*
void TrackFitter711::processor_ResolutionMode(const FTKRoad &road,
                                              list<FTKTrack> &road_tracks)
{

  //empty____________0_
  //empty____________1_
  m_CImap_eff[0]   = 2;
  m_CImap_eff[1]   = 3;
  m_CImap_eff[2]   = 4;
  m_CImap_eff[3]   = 5;
  m_CImap_eff[4]   = 6;
  m_CImap_eff[5]   = 7;
  m_CImap_eff[6]   = 8;
  //empty____________9_
  m_CImap_eff[7]   = 10;
  //empty____________11
  m_CImap_eff[8]   = 12;
  m_CImap_eff[9]   = 13;
  m_CImap_eff[10]  = 14;
  //empty____________15

  int region  = road.getRegion(); // a.k.a. bank
  int subreg  = road.getSubRegion();
  int sector  = road.getSectorID();

  // tracks with full hits from 1st stage
  std::vector<FTKTrack> fulltracksI;

  // iterator over the incomplete tracks list
  list<FTKTrack>::iterator itrack = road_tracks.begin();

  m_incomplete_coordsmask_eff = m_incomplete_coordsmask;
  m_incomplete_planesmask_eff = m_incomplete_planesmask;

  int *missing_section = new int[m_nplanes_ignored];

  const FTKRoad *AMroad = road.getAMRoad();

  for (int i=0;i!=m_nplanes_ignored;++i) {
    // get the logical layer ID of the connected SCT layers
    const int &layerpos = m_idplanes_or[i];

    // isEndcap? : condition.
    int realSS = AMroad->getDCMatchMode()==1 ? AMroad->getSSID(layerpos)>>AMroad->getHLID(layerpos) : AMroad->getSSID(layerpos) ;
    bool isEndcap = (realSS%FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode()))>=20;

    if (isEndcap)
      missing_section[i] = realSS%10; // endcap section
    else
      missing_section[i] = 0; // barrel

    if (m_pmap->getPlane(m_idplanes[i],0).getNDimension()!=1 || FTKSetup::getFTKSetup().getHWModeSS()==2)
      missing_section[i] = 0;

    //std::cout << "i " << i << std::endl;
    //std::cout << "layerpos " << layerpos << std::endl;
    //std::cout << "m_idplanes " << m_idplanes[i] << std::endl;
    //std::cout << "isEndcap " << isEndcap << std::endl;
    //std::cout << "m_endcap_inversion " << m_endcap_inversion[i] << std::endl;

    const int &pos = m_idCImap[i];

    if (isEndcap && m_endcap_inversion[i]) {
      // the stereo layer, to extrapolate, is the previous
      m_idplanes_eff[i] = m_idplanes[i]-1;
      m_idcoords_eff[i] = m_idcoords[i]-1;

      // the axial layer is the next
      m_CImap_eff[pos] = m_CImap[pos]+1;
      m_CIlayermap_eff[layerpos] = m_CIlayermap[layerpos]+1;

      // invert 10 in 01 in the coordinates' bitmask
      int coords_to_zero = m_idcoords_eff[i];
      int coords_to_set  = m_idcoords_eff[i] + 1;
      m_incomplete_coordsmask_eff &= ~(1<<(coords_to_zero)); // zeroes the first bit
      m_incomplete_coordsmask_eff |= 1<<(coords_to_set); // set on the next bit

      int planes_to_zero = m_idplanes_eff[i];
      int planes_to_set  = m_idplanes_eff[i] + 1;
      m_incomplete_planesmask_eff &= ~(1<<(planes_to_zero)); // zeroes the first bit
      m_incomplete_planesmask_eff |= 1<<(planes_to_set); // set on the next bit
    } else {
      // barrel or endcap good layers don't need to change the relation
      m_idplanes_eff[i] =  m_idplanes[i];
      m_idcoords_eff[i] =  m_idcoords[i];
      m_CImap_eff[pos] =  m_CImap[pos];
      m_CIlayermap_eff[layerpos] =  m_CIlayermap[layerpos];
    }
  }

  for (;itrack!=road_tracks.end(); ++itrack) {

    FTKTrack &curtrackI = *itrack;

    int cbitmask(0);
    int cbitmask_real(0);

    // set the incomplete track coordinates
    for (int ix=0; ix!=m_ncoords_incomplete; ++ix) {
      int cix = m_CImap_eff[ix];
      newtrk.setCoord(cix,curtrackI.getCoord(ix));

//       the real mask has 1 only in layers with hits 
      cbitmask_real |= ((curtrackI.getBitmask()&(1<<ix))>>ix)<<cix;
    }

    //if (cbitmask_real==m_resmode_bitmask) {
    //  printBinary(cbitmask_real,m_ncoords);
    //  fulltracksI.push_back(curtrackI);
    //} else {
    //  continue; // if all hits are not found in 1st stage move on
    //}
    fulltracksI.push_back(curtrackI);

    //newtrk.setNMissing(5);
    //newtrk.setBitmask(m_resmode_bitmask);

    newtrk.setNMissing(5);
    newtrk.setBitmask(curtrackI.getBitmask());

    // simids.first represents the subregion ID, simids.second the sector ID within the bank
    pair<int,int> simids(subreg,sector);

    int conn_subreg(-1);
    int conn_sector(-1);
    int best_connection(-1); // ID of the best connection

    FTKConstantBank *current_bank_complete = 0x0;

    int nsims = m_FC_relations[region][subreg]->getNSimilarSectors(sector);

    int maxIN(-1); // maximum number of good X coord.

    if (nsims > m_use_n_conn) // restrict nsims to be equal or below m_use_n_conn
      nsims = m_use_n_conn;

    // loop over similar sectors
    for (int isim=0; isim!=nsims&&maxIN!=m_nplanes_ignored; ++isim) {

      const int &tmpconn_subreg = subreg;
      int tmpconn_sector = sector;
      if (m_use_multiple_conn)
        tmpconn_sector = m_FC_relations[region][subreg]->getSimilarSecID(sector,isim);

      // get the bank related to this road
      current_bank_complete = m_constant[region][tmpconn_subreg];

      // Sanity check on retrieved bank for this specific sector
      if (!current_bank_complete) {
        FTKSetup::PrintMessageFmt(ftk::info,
            "*** Complete constants for region %d - %d don't exists\n",
            region,
            tmpconn_subreg
            );

        continue;
      } else if (tmpconn_sector>=current_bank_complete->getNSectors()) {
        // check if the number of sector is compatible with this bank
        FTKSetup::PrintMessageFmt(ftk::info,
            "*** Complete constants for Sector %d in region %d - %d don't exists\n",
            tmpconn_sector,
            region,
            tmpconn_subreg
            );

        continue;
      } else if (!current_bank_complete->getIsGood(tmpconn_sector)) {
        // check if the sector has a valid constants set
        FTKSetup::PrintMessageFmt(ftk::info,
            "*** Incomplete constants for Sector %d in region %d - %d is not valid\n",
            tmpconn_sector,
            region,
            tmpconn_subreg
            );

        continue;
      }

      // ---------------------------------------
      // Do the extrapolation for this similar sector and see if this is any better than the other.
      // The first method is deprecated. No more geometrical extrapolation
      //
      current_bank_complete->missing_point_guess(newtrk, tmpconn_sector);

      // Counter that counts how many good extrapolation
      int nIN(0);

      // Count how many good X
      for (int ip=0; ip<m_nplanes_ignored; ++ip) {
        const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
        if (ndim==1) {
          const float &localX = newtrk.getCoord(m_idcoords_eff[ip]);

          // If the following condition is met count as good local X
          if (localX>=0 && localX <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth)
            nIN += 1;
        } else if (ndim==2) {
          const float &localX = newtrk.getCoord(m_idcoords_eff[ip]);
          const float &localY = newtrk.getCoord(m_idcoords_eff[ip]+1);

          // If the following condition is met count as good local X
          if (localX>=0 && localX <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth) {
            if (localY>=0 && localY <= m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 1).m_etawidth)
              nIN += 1;
          }
        }
      }

      // here counts the number of out-boundary X and decide if it is the best
      if (nIN>maxIN) {
        simids.first  = tmpconn_subreg;
        simids.second = tmpconn_sector;
        best_connection = isim;
        maxIN = nIN;
      }
    } // Looping over similar sector is done.

    // no valid sectors
    if (maxIN==-1) {
       delete [] missing_section;
       return;
    }

    if (maxIN<m_nplanes_ignored) {
      continue; // if all of them are not within the boundary there is no hope of finding all coordinates, so move on.
      //FTKSetup::PrintMessageFmt(ftk::info,"Extrapolation with out boundary X (%d)\n",m_nplanes_ignored-maxIN);
    }

    // set the variables to extrapolate the variables
    conn_subreg = simids.first;
    conn_sector = simids.second;
    newtrk.setSectorID(conn_subreg);

    current_bank_complete = m_constant[region][conn_subreg];

    //------------------------------------------------------------
    // The extrapolation is done right here.
    // Two types of method
    // the missing_point_guess
    //------------------------------------------------------------
    current_bank_complete->missing_point_guess(newtrk,conn_sector);
    // print the coordinates
    for (int ix=0;ix!=newtrk.getNCoords();++ix) {
      std::cout << std::setw(8) << newtrk.getCoord(ix);
    }
    std::cout << std::endl;

    //------------------------------------------------------------
    // SSMAP COORDINATE -> SSID
    //------------------------------------------------------------
    //

    int *ssid = new int[m_nplanes_ignored];
    int **bounds = new int*[m_nplanes_ignored];

    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {

      const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();

      bounds[ip] = new int[2];
      //bounds[ip] = new int[2*ndim];

      //       use the module id to extract the phi id and the eta id,
      // in particular for modules the ID is phiid*1000+etaid 
      const int &moduleID = m_FC_relations[region][subreg]->getSimilarStereoIDs(sector,best_connection)[ip];
      float phiwindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_phiwidth - 0.001;
      float etawindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], missing_section[ip], 0).m_etawidth - 0.001;

      // Alternative way
      FTKHit* tmpHit = new FTKHit(ndim);
      tmpHit->setSector(moduleID);
      for (int i=0;i<ndim;++i)
        tmpHit->setCoord(i, newtrk.getCoord(m_idcoords_eff[ip]+i));
      tmpHit->setSector(moduleID);
      tmpHit->setEtaWidth(etawindow);
      tmpHit->setPhiWidth(phiwindow);
      tmpHit->setBankID(region);
      tmpHit->setPlane(m_idplanes_eff[ip]);
      if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
	tmpHit->setIdentifierHash(moduleID);
      }
      if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
         try {
            m_ssid[ip] = m_ssmap_complete->getSSTower(*tmpHit, tmpHit->getBankID(),true);
         } catch(FTKException &e) {
            cout<<"TrackFitter711::processor_ResolutionMode (1) bug\n";
            exit(0);
         }
      }else {
	m_ssid[ip] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
      }


      //std::cout << ssid[ip] << std::endl;

      if (ndim == 2) {

        // modify coordinates and get the bounds
        tmpHit->setCoord(0,0);
        tmpHit->setCoord(1,0);
	if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
           try {
              bounds[ip][0] = m_ssmap_complete->getSSTower(*tmpHit, tmpHit->getBankID(),true);
           } catch(FTKException &e) {
            cout<<"TrackFitter711::processor_ResolutionMode (2) bug\n";
            exit(0);
         }
	} else
	  bounds[ip][0] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
        
        tmpHit->setCoord(0,phiwindow);
        tmpHit->setCoord(1,etawindow);
        if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
           try {
              bounds[ip][1] = m_ssmap_complete->getSSTower(*tmpHit, tmpHit->getBankID(),true);
           } catch(FTKException &e) {
            cout<<"TrackFitter711::processor_ResolutionMode (3) bug\n";
            exit(0);
           }
        }else
	  bounds[ip][1] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);
	
      } else if (ndim == 1) {
        // modify coordinates and get the bounds
        tmpHit->setCoord(0,0);      
	if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
           try {
              bounds[ip][0] = m_ssmap_complete->getSSTower(*tmpHit, tmpHit->getBankID(),true);
           } catch(FTKException &e) {
            cout<<"TrackFitter711::processor_ResolutionMode (4) bug\n";
            exit(0);
           }
	} else       bounds[ip][0] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);

        tmpHit->setCoord(0,phiwindow); 
	if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
           try {
              bounds[ip][1] = m_ssmap_complete->getSSTower(*tmpHit, tmpHit->getBankID(),true);
           } catch(FTKException &e) {
              cout<<"TrackFitter711::processor_ResolutionMode (4) bug\n";
              exit(0);
           }  
	} else       bounds[ip][1] = m_ssmap_complete->getSSGlobal(*tmpHit, 1);

      }

      delete tmpHit;

    }

    //------------------------------------------------------------
    // SSIDs -> Obtain Hit IDs
    //------------------------------------------------------------

    //std::cout << "Searching hits ...." << std::endl;

    // get the list of hits
    vector<FTKHit> *hits_more = new vector<FTKHit>[m_nplanes_ignored];

    // loop to retrieve the hit lists
    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
      hits_more[ip].clear();

      int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
      int m_etaneighbours = (ndim==2) ? m_nneighbours : 0;

      // the hits are obtained from the extrapolated SS and the neighbor
      for (int ss_shift=-m_nneighbours/2; ss_shift!=m_nneighbours/2+1; ++ss_shift) {
        for (int ss_shiftEta=-m_etaneighbours/2; ss_shiftEta!=m_etaneighbours/2+1; ++ss_shiftEta) {

          const int tmpSS = ssid[ip]+ss_shift*FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode())+ss_shiftEta;

          // skip SS out of the current module boundaries
          //if ((ndim==1)&&(tmpSS<bounds[ip][0]||tmpSS>bounds[ip][1])) // TODO:
          //  continue;

	  if ((tmpSS<bounds[ip][0]||tmpSS>bounds[ip][1])) // TODO:
            continue;
            
          //else if ((ndim==2)&&(tmpSS<bounds[ip][2]||tmpSS>bounds[ip][3])) // TODO: implement proper eta window check
          //  continue;

          // refers to the unused layers with the general relation
          //    used in the barrel, doesn't take into account the possible
          //    inversion in some endcap layer 
          const FTKSS &ss = m_roadinput->getUnusedSS(road.getRegion(), m_idplanes[ip], tmpSS);

          if (isDebugSuperExp())
            std::cout << " in : " << tmpSS << " is there a hit?: " << !(ss.getHits().empty()) << std::endl; // TODO: debug print output

          if (!ss.getHits().empty())
            hits_more[ip].insert(hits_more[ip].end(), ss.getHits().begin(), ss.getHits().end());
        }
      }
    } // end loop to retrieve the hit lists

    // Delete unused objects
    delete [] hits_more;
    delete [] ssid;
    for (int ip=0;ip!=m_nplanes_ignored;++ip)
      delete [] bounds[ip];
    delete [] bounds;

    //---------------------------------------------------------
    // Linear fit
    //---------------------------------------------------------

    // compute the additional combinations to complete the fits
    int ncombs_more(1);

    // number of hits and the stereo layers
    int nplanes_more(0);
    int ncoords_more(0);

    // reduced bitmask for the extapolated layers
    unsigned int bitmask_more(0);

    vector<FTKHit>::iterator *curhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];
    vector<FTKHit>::iterator *endhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];

    int ic(0);
    for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
      //std::cout << " --- # of hits in each layers --- " << std::endl; //TODO: debug print output remove
      const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
      for (int i=0;i!=ndim;++i,++ic) {
        //std::cout << ic << " " << hits_more[ip].size(); //TODO: debug print output remove
        //for (unsigned ihits=0;ihits<hits_more[ip].size();++ihits) {
        //  std::cout << " " << (*hits_more)[ip];
        //}
        //std::cout << std::endl;
        // move the iterator at the begin of the list
        curhit_more[ip] = hits_more[ip].begin();
        endhit_more[ip] = hits_more[ip].end();

        if (curhit_more[ip]!=endhit_more[ip]) {
          // icrement the number of combinations
          ncombs_more *= hits_more[ip].size();

          // incremente the number of not empty layers
          if (i!=1)
            nplanes_more += 1;
          ncoords_more += 1;

          // set the reduced bitmask
          bitmask_more |= 1<<ic;

          // set the complete track bitmask, real and conventional
          cbitmask |= (1<<(m_idcoords_eff[ip]+i));
          cbitmask_real |= (1<<(m_idcoords_eff[ip]+i));
        } else {
          // in this case the extrapolation failed, some empty value is placed
          newtrk.setFTKHit(m_idplanes_eff[ip],FTKHit()); // set an empty hit
        }
      }
    }
    // quality check to verify a sufficient number of hits
    int nmissing_more(m_nplanes_ignored-nplanes_more);
    int nmissing_more_coord(m_ncoords_ignored-ncoords_more);
    //std::cout << "nmissing_more:"  <<nmissing_more << std::endl;
    printBinary(cbitmask_real,m_ncoords);
    //std::cout << "ncomb:"  << ncombs_more<< std::endl;
    //if (ncombs_more != 1)
    //  continue;

    if (nmissing_more<=m_max_missing_extraplanes) {
      // 
      //  * Do additional fits using the hits in the SCT layers not used
      //  * in the first step
      //  

      // the number of missing points depends only by the last step,
      // the MJ hits in the first step are used as real hits
      newtrk.setNMissing(nmissing_more_coord);

      // set the conventional bitmask for the fit
      newtrk.setBitmask(cbitmask);

      // increment the number of addional fits
      m_ncombs += ncombs_more;

      // loop over the additional combination, choosing only the tracks sutisfying the quality requirements
      for (int icomb_more=0; icomb_more!=ncombs_more; ++icomb_more) {
        // complete fit loop
        int ic(0);
        float *guessed_coords = new float[7];
        int iguess(0);
        for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
          const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
          for (int i=0;i!=ndim;++i,++ic) {
            if (!(bitmask_more&(1<<ic)))
              continue; // skip empty planes

            // the missing layers are assumed to be 1-D
            const int &cid = m_idcoords_eff[ip]+i;
            newtrk.setFTKHit(m_idplanes_eff[ip],*curhit_more[ip]);

            guessed_coords[iguess] = newtrk.getCoord(cid);
            iguess ++;
            //std::cout << " cid : " << cid << ", guessed cord : " << newtrk.getCoord(cid) <<  ", coord : " << (*curhit_more[ip])[i] << std::endl;

            newtrk.setCoord(cid,(*curhit_more[ip])[i]);
          }
        }

        // At this point I have a track with full hits.
        // Now doing my study
        float *real_coords = new float[7];
        // IBL
        real_coords[0] = newtrk.getCoord(0);
        real_coords[1] = newtrk.getCoord(1);
        // BL
        real_coords[2] = newtrk.getCoord(2);
        real_coords[3] = newtrk.getCoord(3);
        // SCTs
        real_coords[4] = newtrk.getCoord(9);
        real_coords[5] = newtrk.getCoord(11);
        real_coords[6] = newtrk.getCoord(15);

        newtrkI.setCoord(0,0);
        newtrkI.setCoord(1,0);
        newtrkI.setCoord(2,newtrk.getCoord(4));
        newtrkI.setCoord(3,newtrk.getCoord(5));
        newtrkI.setCoord(4,newtrk.getCoord(6));
        newtrkI.setCoord(5,newtrk.getCoord(7));
        newtrkI.setCoord(6,newtrk.getCoord(8));
        newtrkI.setCoord(7,newtrk.getCoord(10));
        newtrkI.setCoord(8,newtrk.getCoord(12));
        newtrkI.setCoord(9,newtrk.getCoord(13));
        newtrkI.setCoord(10,newtrk.getCoord(14));

        //for (int ix=0;ix!=newtrkI.getNCoords();++ix) {
        //  std::cout << std::setw(8) << newtrkI.getCoord(ix);
        //}
        //std::cout << std::endl;

        FTKConstantBank *current_bank_incomplete = m_constant_incomplete[region][subreg];

        newtrkI.setNMissing(2);
        newtrkI.setBitmask(2044);
        //2044 = 00 11 11 1X 1X 11 1X : 8L missing BL bitmask

        current_bank_incomplete->missing_point_guess(newtrkI,sector);

        //for (int ix=0;ix!=newtrkI.getNCoords();++ix) {
        //  std::cout << std::setw(8) << newtrkI.getCoord(ix);
        //}
        //std::cout << std::endl;

        // propagating these coordinates to newtrk.
        newtrk.setCoord(0,0);
        newtrk.setCoord(1,0);
        newtrk.setCoord(2,newtrkI.getCoord(0));
        newtrk.setCoord(3,newtrkI.getCoord(1));
        newtrk.setCoord(4,newtrkI.getCoord(2));
        newtrk.setCoord(5,newtrkI.getCoord(3));
        newtrk.setCoord(6,newtrkI.getCoord(4));
        newtrk.setCoord(7,newtrkI.getCoord(5));
        newtrk.setCoord(8,newtrkI.getCoord(6));
        newtrk.setCoord(9,0);
        newtrk.setCoord(10,newtrkI.getCoord(7));
        newtrk.setCoord(11,0);
        newtrk.setCoord(12,newtrkI.getCoord(8));
        newtrk.setCoord(13,newtrkI.getCoord(9));
        newtrk.setCoord(14,newtrkI.getCoord(10));
        newtrk.setCoord(15,0);

        newtrk.setNMissing(5);
        //newtrk.setBitmask(m_resmode_bitmask);
        newtrk.setBitmask(curtrackI.getBitmask());

        current_bank_complete->missing_point_guess(newtrk,conn_sector);

        //for (int ix=0;ix!=newtrk.getNCoords();++ix) {
        //  std::cout << std::setw(8) << newtrk.getCoord(ix);
        //}
        //std::cout << std::endl;

        m_histores_hitcoord_PXL[0]->Fill(guessed_coords[0]  - real_coords[0], guessed_coords[1]  - real_coords[1]);
        m_histores_hitcoord_PXL[1]->Fill(newtrk.getCoord(0) - real_coords[0], newtrk.getCoord(1) - real_coords[1]);

        delete [] guessed_coords;
        delete [] real_coords;
      } // end complete fit loop
    } // end block to complete the fits
    delete [] curhit_more;
    delete [] endhit_more;
  }

  delete [] missing_section;

}
// END COMMENT OUT DEPRECATED FUNCTION BY JAA
*/

void TrackFitter711::printBinary(int number, int total) {
  int count = 0;
  do {
    std::cout << number%2 << " , ";
    number = number >> 1;
    count ++;
  } while(number != 0);

  while (count < total) {
    std::cout << 0 << " , ";
    count ++;
  };
  std::cout << std::endl;
}


//-----------------------------------------------------------------------

/** This method is based from processor_Extrapolate. The difference is that the
 * connection candidates will be all tested and tried. The resulting all
 * possible tracks are saved hoping the hit warrior will remove them properly
 * */
void TrackFitter711::processor_SuperExtrapolate(const FTKRoad &road, list<FTKTrack> &road_tracks) {

  // This will set all subsequent print informatin
  if (FTKSetup::getFTKSetup().getVerbosity() == -10)
    setDebugSuperExp(true);
  else
    setDebugSuperExp(false);

  initializeSuperExtrapolate(road);

  // check connection file exists
  if (!isConnFileOkay())
    return;

  prepareCompleteTrack(road);
  setLayerInversions(road);
  if (isDebugSuperExp())
    printLayerInversionResult();

  // Start Extrapolation

  // iterator over the incomplete tracks list
  list<FTKTrack>::iterator itrack = road_tracks.begin();

  for (;itrack!=road_tracks.end();++itrack) {
    newtrkI = *itrack;
    m_passedExtrapolation = false; ///reset this
    extrapolateIncompleteTrack(road);
  }

  endSuperExtrapolate();

  return;
}

void TrackFitter711::initializeSuperExtrapolate(const FTKRoad &road) {

  for (int ip = 0; ip < m_nplanes_ignored; ++ip) {
    m_section_of_exp_layers[ip] = 0;
    m_ssid[ip] = 0;
    m_bounds[ip] = 0;
    m_hits_more[ip].clear();
  }

  m_baseHWflag = road.getRWRejected()*10;

  // retrieve all the informations for the fits
  m_region_for_superexp = road.getRegion();
  m_subreg_for_superexp = road.getSubRegion();
  m_sector_for_superexp = road.getSectorID();


}

bool TrackFitter711::isConnFileOkay() {
  // check connection file exists
  if (m_FC_relations[m_region_for_superexp][m_subreg_for_superexp]->getNSimilarSectors(m_sector_for_superexp)==0) {
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel, "*** There are no valid extrapolation constants for Sector %d in region %d - %d\n", m_sector_for_superexp, m_region_for_superexp, m_subreg_for_superexp);
    return false;
  }
  return true;
}

void TrackFitter711::prepareCompleteTrack(const FTKRoad &road) {
  // set parameters that will remain the same for all the tracks
  newtrk.setBankID(road.getBankID());
  newtrk.setRoadID(road.getRoadID());
  newtrk.setSectorID(road.getSectorID());
}

void TrackFitter711::setLayerInversions(const FTKRoad &road) {
  //
  // Layer Inversion
  //

  /** This part fixes the issue due to the layer inversion
      in the Endcap between stereo and axial relative position.
      This code loops the check if a SS is in the endcap, vrifies
      if the fix is needed, and fill the conversion maps for this roads.
      It is important to notice that the inversion disappears in the
      unused layers map, used in the road_finder.
  */

  const FTKRoad * AMroad = road.getAMRoad();

  m_incomplete_coordsmask_eff = m_incomplete_coordsmask;
  m_incomplete_planesmask_eff = m_incomplete_planesmask;

  m_section_of_exp_layers = new int[m_nplanes_ignored];

  for (int i=0;i!=m_nplanes_ignored;++i) {
    // get the logical layer ID of the connected SCT layers
    const int &layerpos = m_idplanes_or[i];

    // isEndcap? : condition.
    const int realSS = AMroad->getDCMatchMode()==1 ? AMroad->getSSID(layerpos)>>AMroad->getHLID(layerpos) : AMroad->getSSID(layerpos);
    bool isEndcap = (realSS%FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode()))>=20;

    if (isEndcap)
      m_section_of_exp_layers[i] = realSS%10; // endcap section
    else
      m_section_of_exp_layers[i] = 0; // barrel

    if(FTKSetup::getFTKSetup().getHWModeSS()==2 ){
      if (m_ssmap_complete->getRegionMap()->getNumRegions()==32 && (m_region_for_superexp< 9 || m_region_for_superexp>24)) isEndcap = true;
      else isEndcap = false;
    } 

    if (m_pmap->getPlane(m_idplanes[i],0).getNDimension()!=1 || FTKSetup::getFTKSetup().getHWModeSS()==2 )
      m_section_of_exp_layers[i] = 0;

    const int &pos = m_idCImap[i];

    if (isEndcap && m_endcap_inversion[i]) {
      // the stereo layer, to extrapolate, is the previous
      m_idplanes_eff[i] = m_idplanes[i]-1;
      m_idcoords_eff[i] = m_idcoords[i]-1;

      // the axial layer is the next
      m_CImap_eff[pos] = m_CImap[pos]+1;
      m_CIlayermap_eff[layerpos] = m_CIlayermap[layerpos]+1;

      // invert 10 in 01 in the coordinates' bitmask
      int coords_to_zero = m_idcoords_eff[i];
      int coords_to_set  = m_idcoords_eff[i] + 1;
      m_incomplete_coordsmask_eff &= ~(1<<(coords_to_zero)); // zeroes the first bit
      m_incomplete_coordsmask_eff |= 1<<(coords_to_set); // set on the next bit

      int planes_to_zero = m_idplanes_eff[i];
      int planes_to_set  = m_idplanes_eff[i] + 1;
      m_incomplete_planesmask_eff &= ~(1<<(planes_to_zero)); // zeroes the first bit
      m_incomplete_planesmask_eff |= 1<<(planes_to_set); // set on the next bit
    } else {
      // barrel or endcap good layers don't need to change the relation
      m_idplanes_eff[i] =  m_idplanes[i];
      m_idcoords_eff[i] =  m_idcoords[i];
      m_CImap_eff[pos] =  m_CImap[pos];
      m_CIlayermap_eff[layerpos] =  m_CIlayermap[layerpos];
    }
  }

  // mask to prevent majority in planes with multiple hits or previous hits
  m_norecovery_mask = m_incomplete_planesmask_eff;
}


void TrackFitter711::extrapolateIncompleteTrack(const FTKRoad &road) {
  // reset the flags
  newtrk.setHWRejected(m_baseHWflag);
  newtrk.setHWTrackID(-1);
  gatherConnections();
  printConnections();
  bool * iblhits = new bool[conn_subregs.size()];

  newtrk.setNPlanesIgnored(m_nplanes-m_nplanes_incomplete);
  for (int ic=0;ic!=m_ncoords;++ic) {
    combtrack[ic].setNPlanesIgnored(m_nplanes-m_nplanes_incomplete);
  }

  for (setIConn(0); cur_iconn < (int) conn_subregs.size(); setIConn(cur_iconn+1)) {
    prepareTrack();
    performExtrapolation();
    guessedHitsToSSID();
    printEveryHitsInExtraLayers();
    obtainHitsFromSSIDs();
    printFoundHits();
    if (getDiagnosticMode())
      fillDiagnosticPlotsPerTrackPerConn();
    iblhits[cur_iconn] = isIBLhitsFound();
    fitCompleteTracks(road);
    saveCompleteTracks();
  }

  if (m_passedExtrapolation) m_nextrapolatedTracks++;

  if (getDiagnosticMode())
    fillDiagnosticPlotsPerTrack();
  int total = 0;
  
  for (unsigned iconn = 0; iconn < conn_subregs.size(); ++iconn) {
    if (iblhits[iconn])
      total += 1;
  }
  delete [] iblhits;
  //if (total == 0)
  //  std::cout << "No hits are found in IBL" << std::endl;
}

void TrackFitter711::prepareTrack() {
  //------------------------------------------------------------
  // INCOMPLETE TRACKS PREPRATION
  //------------------------------------------------------------

  completeTrackCandidates.clear();
  connectedSectorPairs.clear();
  nmissingOfCompleteTracks.clear();

  // complete track bitmask
  m_cbitmask_real = 0;

  /* conventional complete bitmask, this assumes
     all hits for previous steps as real*/
  m_cbitmask = m_incomplete_coordsmask_eff;
  newtrk.setHalfInvPt(newtrkI.getHalfInvPt());
  newtrk.setInvPtFW(round(newtrkI.getInvPtFW()*1e5)/1e5);

  /* Fix from corrgen: in the constant generation, tracks
     in regions 3, 4, and 5 has phi defined in the range [0:2 PI].
     For the extrapolation only checks if the angle is in those
     regions and fix the definition */
    if (m_ssmap_complete->getRegionMap()->getNumRegions()==8) { // 8 Regions, constant generation step
      if (m_region_for_superexp>=3 && m_region_for_superexp<=5) { // Regions 3, 4, and 5
        if (newtrkI.getPhi()<=0) { // The angle should be defined in the range [0:2 PI]. Otherwise we fix it.
          newtrk.setPhi(newtrkI.getPhi()+2*M_PI,false);
          newtrk.setPhiFW(round(newtrkI.getPhiFW()+2*M_PI),false);
        }
      }
    }
    else if (m_ssmap_complete->getRegionMap()->getNumRegions()==64) { // 64 Regions, extrapolation step
      if (m_region_for_superexp==9 || m_region_for_superexp==10 || m_region_for_superexp==25 || m_region_for_superexp==26 || m_region_for_superexp==41 || m_region_for_superexp==42 || m_region_for_superexp==57 || m_region_for_superexp==58) { // Regions 9, 10, 25, 26, 41, 42, 57, and 58
        if (newtrkI.getPhi()<=0) {
          newtrk.setPhi(newtrkI.getPhi()+2*M_PI,false);
          newtrk.setPhiFW(round(newtrkI.getPhiFW()+2*M_PI),false);
        }
      }
    }
    else { // In other cases we don't have to force phi to be in the range [0:2 PI].
      newtrk.setPhi(newtrkI.getPhi());
      newtrk.setPhiFW(round(newtrkI.getPhiFW()));
    }

  // set the incomplete track with some original info
  newtrk.setIP(newtrkI.getIP());
  newtrk.setIPFW(round(newtrkI.getIPFW()));
  newtrk.setCotTheta(newtrkI.getCotTheta());
  newtrk.setCTheta(round(newtrkI.getCTheta()));
  newtrk.setZ0(newtrkI.getZ0());
  newtrk.setZ0FW(round(newtrkI.getZ0FW()));
  newtrk.setChi2(newtrkI.getChi2());
  newtrk.setChi2FW(round(newtrkI.getChi2FW()));
  newtrk.setOrigChi2(newtrkI.getOrigChi2());
  newtrk.setCombinationID(newtrkI.getCombinationID());

  // zero out coords
  for (int ix=0; ix!=m_ncoords; ++ix) {
    newtrk.setCoord(ix,0);
  }

  // set the incomplete track coordinates
  for (int ix=0; ix!=m_ncoords_incomplete; ++ix) {
    int cix = m_CImap_eff[ix];
    newtrk.setCoord(cix,newtrkI.getCoord(ix));

    /* the real mask has 1 only in layers with hits */
    m_cbitmask_real |= ((newtrkI.getBitmask()&(1<<ix))>>ix)<<cix;
  }

  for (int ip=0; ip!=m_nplanes_incomplete; ++ip) {
    int cip = m_CIlayermap_eff[ip];
    newtrk.setFTKHit(cip,newtrkI.getFTKHit(ip));
  }

  // complete the track using the extrapolation method
  // the bitmask and number of missing point is artificial
  newtrk.setBitmask(m_incomplete_coordsmask_eff);
  newtrk.setNMissing(m_ncoords_ignored);

  // set other track flags
  newtrk.setTypeMask(newtrkI.getTypeMask());

  // set connindex
  newtrk.setConnectionIndex(cur_iconn);

  // update the track sector ID with the 12L sector ID so it models SSB output
  newtrk.setSectorID(conn_sectors[cur_iconn]);
  
  //// ===== Incomplete tracks are ready to be extrapolated!
}

void TrackFitter711::gatherConnections() {

  // new skim, where the DB with the relation between 7L --> 11L exists
  // nsims : how many similar sectors do we have?
  int nsims = m_FC_relations[m_region_for_superexp][m_subreg_for_superexp]->getNSimilarSectors(m_sector_for_superexp);

  int maxIN(-1); // maximum number of good X coord.

        if (nsims > m_use_n_conn) // restrict nsims to be equal or below m_use_n_conn
    nsims = m_use_n_conn;

  if (!m_use_multiple_conn)
    FTKSetup::PrintMessage(ftk::sevr, "TRACKFITTER_MODE=3 Requires UseMultipleConn=1");

  conn_subregs.clear();
  conn_sectors.clear();
  conn_gcbanks.clear();

  // loop over similar sectors
  for (int isim=0; isim!=nsims&&maxIN!=m_nplanes_ignored; ++isim) {

    int tmpsub = m_subreg_for_superexp;
    int tmpsec = m_FC_relations[m_region_for_superexp][tmpsub]->getSimilarSecID(m_sector_for_superexp,isim);
    FTKConstantBank* tmpcon = m_constant[m_region_for_superexp][tmpsub];

    // Sanity check on retrieved bank for this specific sector
    if (!tmpcon) {
      FTKSetup::PrintMessageFmt(ftk::info,
                                "*** Complete constants for region %d - %d don't exists\n",
                                m_region_for_superexp,
                                tmpsub
                                );

      continue;
    } else if (tmpsec>=tmpcon->getNSectors()) {
    // check if the number of sector is compatible with this bank
      FTKSetup::PrintMessageFmt(ftk::info,
                                "*** Complete constants for Sector %d in region %d - %d don't exists\n",
                                tmpsec,
                                m_region_for_superexp,
                                tmpsub
                                );

      continue;
    } else if (!tmpcon->getIsGood(tmpsec)) {
    // check if the sector has a valid constants set
      FTKSetup::PrintMessageFmt(ftk::info,
                                "*** Incomplete constants for Sector %d in region %d - %d is not valid\n",
                                tmpsec,
                                m_region_for_superexp,
                                tmpsub
                                );

      continue;
    }

    conn_subregs.push_back(tmpsub);
    conn_sectors.push_back(tmpsec);
    conn_gcbanks.push_back(tmpcon);

  } // Looping over similar sector is done.
  m_nconn += conn_subregs.size();
}

void TrackFitter711::printCurCompleteTrack() {
  // print the coordinates
  for (int ix=0;ix!=newtrk.getNCoords();++ix) {
    std::cout << std::setw(8) << newtrk.getCoord(ix);
  }
  std::cout << std::endl;
}

void TrackFitter711::performExtrapolation() {

  if (isDebugSuperExp())
    FTKSetup::PrintMessageFmt(ftk::info, "performing extrapolation with subreg: %d, sector: %d\n", conn_subregs.at(cur_iconn), conn_sectors.at(cur_iconn));

  if (isDebugSuperExp())
    printCurCompleteTrack();

  conn_gcbanks.at(cur_iconn)->missing_point_guess(newtrk, conn_sectors.at(cur_iconn));
  IBLcoordPhi = newtrk.getCoord(0);
  IBLcoordEta = newtrk.getCoord(1);
  //newtrk.setIBLGuessedHits();

  if (isDebugSuperExp())
    printCurCompleteTrack();
  return;
}

void TrackFitter711::guessedHitsToSSID() {
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {

    const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();

    m_bounds[ip] = new int[2];
    //m_bounds[ip] = new int[ndim];

    /* use the module id to extract the phi id and the eta id,
       in particular for modules the ID is phiid*1000+etaid */
    const int &moduleID = m_FC_relations[m_region_for_superexp][m_subreg_for_superexp]->getSimilarStereoIDs(m_sector_for_superexp,cur_iconn)[ip];


    float phiwindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], m_section_of_exp_layers[ip], 0).m_phiwidth - 0.001;
    float etawindow = m_ssmap_complete->getMap(m_idplanes_eff[ip], m_section_of_exp_layers[ip], 0).m_etawidth - 0.001;



    // Alternative way
    FTKHit tmphit(ndim);
    tmphit.setSector(moduleID);
    double xMax[2];
    xMax[0]=phiwindow-0.5;
    xMax[1]=etawindow-0.5;
    for (int i=0;i<ndim;++i) {
       double x=newtrk.getCoord(m_idcoords_eff[ip]+i);
       if(x<0.) x=0.;
       if(x>xMax[i]) x=xMax[i];
       tmphit.setCoord(i,x);
    }
    tmphit.setEtaWidth(etawindow);
    tmphit.setPhiWidth(phiwindow);
    tmphit.setBankID(m_region_for_superexp);
    tmphit.setPlane(m_idplanes_eff[ip]);
    if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
      tmphit.setIdentifierHash(moduleID);
    }

    if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
       try {
          m_ssid[ip] = m_ssmap_complete->getSSTower
             (tmphit, tmphit.getBankID(),true);
       } catch(FTKException &e) {
          cout<<"TrackFitter711::guessedHitsToSSID bug\n";
          exit(0);
       }
    }else {
      m_ssid[ip] = m_ssmap_complete->getSSGlobal(tmphit, 1);
    }

    if(print_guessed && (m_idplanes_eff[ip]==0)) {
       cout<<"guessed_hit: "<< m_ssid[ip]<<":"<<moduleID
           <<","<<tmphit[0]<<","<<tmphit[1]<<"\n";
    }

    if (ndim == 2) {

      // modify coordinates and get the m_bounds
      tmphit.setCoord(0,0);
      tmphit.setCoord(1,0);
      if( FTKSetup::getFTKSetup().getHWModeSS()==2)
        m_bounds[ip][0] = m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true);
      else
        m_bounds[ip][0] = m_ssmap_complete->getSSGlobal(tmphit, 1);

      tmphit.setCoord(0,phiwindow);
      tmphit.setCoord(1,etawindow);
      if( FTKSetup::getFTKSetup().getHWModeSS()==2)
        m_bounds[ip][1] = m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true);
      else
        m_bounds[ip][1] = m_ssmap_complete->getSSGlobal(tmphit, 1);

    } else if (ndim == 1) {
      // modify coordinates and get the m_bounds
      tmphit.setCoord(0,0);     
      if( FTKSetup::getFTKSetup().getHWModeSS()==2)
        m_bounds[ip][0] = m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true);
      else       m_bounds[ip][0] = m_ssmap_complete->getSSGlobal(tmphit, 1);

      tmphit.setCoord(0,phiwindow); 
      if( FTKSetup::getFTKSetup().getHWModeSS()==2)
        m_bounds[ip][1] = m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true);
      else       m_bounds[ip][1] = m_ssmap_complete->getSSGlobal(tmphit, 1);

    }
    if (isDebugSuperExp()) {
      if (ndim == 2) {
        float x = newtrk.getCoord(m_idcoords_eff[ip]);
        float y = newtrk.getCoord(m_idcoords_eff[ip]+1);

	 if( FTKSetup::getFTKSetup().getHWModeSS()==2) {                                                                                                                         
          // modify coordinates and get the m_bounds                                                                                                                              
	    tmphit.setCoord(1,0);                                                                                                                                            
	    tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(), true) << std::endl;                                                
  	    tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(), true) << std::endl;                                
  	    tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(), true) << std::endl;                                               
	    // restore the x-coord                                                                                                                                            
	    tmphit.setCoord(1,y);                                                                                                                                             
  	    tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                         
  	    tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                               
  	    tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                                 
	    // now vary y-cord                                                                                                                                                
	    tmphit.setCoord(1,etawindow);                                                                                                                                        
  	    tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                               
  	    tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                                  
  	    tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSTower(tmphit, tmphit.getBankID(),true) << std::endl;                                                 

	 }                                                                                                                                                                   
	 else {                                                                                            

        // modify coordinates and get the m_bounds
        tmphit.setCoord(1,0);
        tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        // restore the x-coord
        tmphit.setCoord(1,y);
        tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        // now vary y-cord
        tmphit.setCoord(1,etawindow);
        tmphit.setCoord(0,0);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,x);         std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
        tmphit.setCoord(0,phiwindow); std::cout << m_ssmap_complete->getSSGlobal(tmphit, 1) << std::endl;
      }
    }
    }
  }
}


void TrackFitter711::obtainHitsFromSSIDs() {
  //------------------------------------------------------------
  // SSIDs -> Obtain Hit IDs
  //------------------------------------------------------------

  //std::cout << "Searching hits ...." << std::endl;

   int nError=0,nTry=0;
  // loop to retrieve the hit lists
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
    m_hits_more[ip].clear();

    int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
    int m_etaneighbours = (ndim==2) ? m_nneighbours : 0;
    bool isSCT = (ndim==2) ? false : true; // FTKSSMap.h (hardcoded value either 100 or 200)

    // the hits are obtained from the extrapolated SS and the neighbor
    for (int ss_shift=-m_nneighbours/2; ss_shift!=m_nneighbours/2+1; ++ss_shift) {
      for (int ss_shiftEta=-m_etaneighbours/2; ss_shiftEta!=m_etaneighbours/2+1; ++ss_shiftEta) {
	int tmpSS;
	 if( FTKSetup::getFTKSetup().getHWModeSS()==2) {   

	   int phiss =  m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiss;	   
           int phimax=m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_phiwidth;
	   int etass =  0;
           int etamax=0;
	   int section=0;
	   int localX=0,localY=0;
	   int localModuleID=-1;
           nTry++;
	   if(ndim ==2)	 {
              etass =  m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_etass;
              etamax=m_ssmap_complete->getMap(m_idplanes_eff[ip], 0, 0).m_etawidth;
	   //the "official" procedure would involve:
	   //decode the SSID into the local anc global coordinates
	   //change the local coordinates as desired
	   //re-encode the SSID
	     m_ssmap_complete->decodeSSTowerXY(m_ssid[ip],m_subreg_for_superexp,m_idplanes_eff[ip],section, localModuleID,localX,localY,true);
            localX += ss_shift*phiss;
            localY += ss_shiftEta*etass;
            if((localX<0.)||(localX>=phimax)) continue;
            if((localY<0.)||(localY>=etamax)) continue;
            try {
               tmpSS =  m_ssmap_complete->compressed_ssid_word_pixel(localModuleID,m_idplanes_eff[ip],section,localX,localY,true);
            } catch(FTKException &e) {
               // coordinates outside module boundary
               nError++;
               continue;
            }
           }else{
	     m_ssmap_complete->decodeSSTowerX(m_ssid[ip],m_subreg_for_superexp,m_idplanes_eff[ip],section, localModuleID,localX,true);
            localX += ss_shift*phiss;
            if((localX<0.)||(localX>=phimax)) continue;
  	    try {
               tmpSS =  m_ssmap_complete->compressed_ssid_word_strip(localModuleID,m_idplanes_eff[ip],section,localX,true);
            } catch(FTKException &e) {
               // coordinates outside module boundary
               nError++;
               continue;
            }
	   }
	   //	   tmpSS = m_ssid[ip]+ss_shift*shift+ss_shiftEta;
	   // std::cout << "tmpSS " << tmpSS << std::endl;
	 }
	 else tmpSS = m_ssid[ip]+ss_shift*FTKSSMap::getPhiOffset(isSCT,FTKSetup::getFTKSetup().getITkMode())+ss_shiftEta;

        if (getDiagnosticMode()) {
          if (ip==0 && ibl_module_with_hit != 0) {
            m_histo_module_value_diff->Fill(ibl_module_with_hit - tmpSS);
          }
        }

        if (isDebugSuperExp()) {
          std::cout << "    Looking for SSID : " << tmpSS << std::endl;
          std::cout << "The bounds0 for SSID : " << m_bounds[ip][0] << std::endl;
          std::cout << "The bounds1 for SSID : " << m_bounds[ip][1] << std::endl;
        }

        // skip SS out of the current module boundaries
        if (/*(ndim==1)&&*/(tmpSS<m_bounds[ip][0]||tmpSS>m_bounds[ip][1])) // TODO:
          continue;
        //else if ((ndim==2)&&(tmpSS<bounds[ip][2]||tmpSS>bounds[ip][3])) // TODO: implement proper eta window check
        //  continue;

        /* refers to the unused layers with the general relation
           used in the barrel, doesn't take into account the possible
           inversion in some endcap layer */
        const FTKSS &ss = m_roadinput->getUnusedSS(m_region_for_superexp, m_idplanes[ip], tmpSS);

	if (isDebugSuperExp())
          std::cout << " in : " << tmpSS << " is there a hit?: " << !(ss.getHits().empty()) << std::endl; // TODO: debug print output

        if (!ss.getHits().empty())
          m_hits_more[ip].insert(m_hits_more[ip].end(), ss.getHits().begin(), ss.getHits().end());
      }
    }
    //  std::cout << m_hits_more[ip].size() << std::endl;
  } // end loop to retrieve the hit lists
  if(nError>0) {
     FTKSetup::PrintMessageFmt
        (ftk::warn,"obtainHitsFromSSIDs nError=%d nTry=%d\n",
         nError,nTry);
  }
}

void TrackFitter711::fitCompleteTracks(const FTKRoad &road) {
  //---------------------------------------------------------
  // Linear fit
  //---------------------------------------------------------

  // compute the additional combinations to complete the fits
  int ncombs_more(1);

  // number of hits and the stereo layers
  int nplanes_more(0);
  int ncoords_more(0);

  // reduced bitmask for the extapolated layers
  unsigned int bitmask_more(0);

  vector<FTKHit>::iterator *curhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];
  vector<FTKHit>::iterator *endhit_more = new vector<FTKHit>::iterator[m_nplanes_ignored];

  bool missPix(false), missSCT(false);
  int ic(0);
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
    const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
    for (int i=0;i!=ndim;++i,++ic) {
      // move the iterator at the begin of the list
      curhit_more[ip] = m_hits_more[ip].begin();
      endhit_more[ip] = m_hits_more[ip].end();

      if (curhit_more[ip]!=endhit_more[ip]) {
        // icrement the number of combinations
        ncombs_more *= m_hits_more[ip].size();

        // incremente the number of not empty layers
        if (i!=1)
          nplanes_more += 1;
        ncoords_more += 1;

        // set the reduced bitmask
        bitmask_more |= 1<<ic;

        // set the complete track bitmask, real and conventional
        m_cbitmask |= (1<<(m_idcoords_eff[ip]+i));
        m_cbitmask_real |= (1<<(m_idcoords_eff[ip]+i));
      } else {
	if (ndim == 1) missSCT = true;
	else missPix = true;
        // in this case the extrapolation failed, some empty value is placed
        newtrk.setFTKHit(m_idplanes_eff[ip],FTKHit()); // set an empty hit
      }
    }
  }

  // quality check to verify a sufficient number of hits
  int nmissing_more(m_nplanes_ignored-nplanes_more);
  int nmissing_more_coord(m_ncoords_ignored-ncoords_more);

  if (passedNMissLayerRequirement(nmissing_more)) {
    m_passedExtrapolation = true;

    /*
     * Do additional fits using the hits in the SCT layers not used
     * in the first step
     */

    // the number of missing points depends only by the last step,
    // the MJ hits in the first step are used as real hits
    newtrk.setNMissing(nmissing_more_coord);

    // set the conventional bitmask for the fit
    newtrk.setBitmask(m_cbitmask);

    // increment the number of addional fits
    m_ncombs += ncombs_more;

    // loop over the additional combination, choosing only the tracks sutisfying the quality requirements
    for (int icomb_more=0; icomb_more!=ncombs_more; ++icomb_more) {
      // complete fit loop
      int ic(0);
      for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
        const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
        for (int i=0;i!=ndim;++i,++ic) {
          if (!(bitmask_more&(1<<ic)))
            continue; // skip empty planes

          // the missing layers are assumed to be 1-D
          const int &cid = m_idcoords_eff[ip]+i;
          newtrk.setFTKHit(m_idplanes_eff[ip],*curhit_more[ip]);
          newtrk.setCoord(cid,(*curhit_more[ip])[i]);
        }
      }

      //for ( int i = 0; i < 16; i++)
      //  std::cout << "newtrk.getCoord(" << i << ") = " << newtrk.getCoord(i) << std::endl;

      if (nmissing_more==0)m_nfits += 1;
      //std::cout << "m_nfits: " << m_nfits << std::endl;

      if (nmissing_more>0) {
        m_nfits_maj += 1;
	if (missPix) m_nfits_maj_pix += 1;
	if (missSCT) m_nfits_maj_SCT += 1;
      }


      // perform the fit
      //newtrk.setTrackID(m_comb_id++);
      newtrk.setTrackID(newtrkI.getTrackID());
      newtrk.setExtrapolationID(icomb_more);

      for (int ip=0; ip!=m_nplanes_ignored; ++ip) newtrk.setSSID(ip,m_ssid[ip]);

      conn_gcbanks.at(cur_iconn)->linfit(conn_sectors.at(cur_iconn),newtrk);
      //std::cout << " Chi2 " <<newtrk.getChi2() << std::endl;
      newtrk.setOrigChi2(newtrk.getChi2());

      // set the correct number of missing points and
      // and the real bitmask
      newtrk.setNMissing(newtrkI.getNMissing()+nmissing_more_coord);
      newtrk.setBitmask(m_cbitmask_real);

      // match the track to a geant particle using the channel-level
      // geant info in the superstrip data.
      compute_truth(m_region_for_superexp,road,newtrkI,m_hits_more,newtrk);

      // accept track
      std::pair <int, int> simids;
      simids.first = conn_subregs.at(cur_iconn);
      simids.second = conn_sectors.at(cur_iconn);
      completeTrackCandidates.push_back(newtrk);
      connectedSectorPairs.push_back(simids);
      nmissingOfCompleteTracks.push_back(nmissing_more_coord);

      //FTKSetup::PrintMessageFmt(ftk::info,"Track accepted");
      // go to the next combination
      for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
        if (!(bitmask_more&(1<<ip)))
          continue; // skip empty planes

        // move the hit in this plane
        curhit_more[ip]++;

        if (curhit_more[ip]!=endhit_more[ip])
          break;
        else // rewind this hits and move the next plane
          curhit_more[ip] = m_hits_more[ip].begin();
      }

    } // end complete fit loop
  } // end block to complete the fits

  delete [] curhit_more;
  delete [] endhit_more;
}

void TrackFitter711::saveCompleteTracks() {
  // In the case of m_do_majority > 1, we only do majority fits if
  // ALL full fits fail the chi2 cut
  bool fullpassed = false;

  if( m_do_majority > 1 ) {
    for( unsigned int idx = 0; idx < completeTrackCandidates.size(); idx++ ) {
      newtrk = completeTrackCandidates[idx];
      if( newtrk.getNMissing() != 0 )
        continue;

      float dof = m_ncoords - m_npars - newtrk.getNMissing();
      if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

      if( newtrk.getChi2() < ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ) ) {
        fullpassed = true;
        break;
      }
    }
  }

  // -------------------------------------------------------------------------
  // N-TRACKS -> SAVE GOOD ONES
  // -------------------------------------------------------------------------
  // Looping over complete track candidates.
  // 1. Check chi-square
  // 2. If it's a full track do majority recover
  // 3. Then check chi-square again
  // 4. If chi-square has passed then save. (Also performs small scale HW)
  for( unsigned int idx = 0; idx < completeTrackCandidates.size(); idx++ ) {

    // retrive the information on the stored track
    newtrk = completeTrackCandidates[idx];
    pair<int,int> simids = connectedSectorPairs[idx];
    int nmissing_more = nmissingOfCompleteTracks[idx];

    float dof = m_ncoords - m_npars - newtrk.getNMissing();
    if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

    // -------------------------------------------------------
    // Boolean question
    // Do majority recover??
    // -------------------------------------------------------
    // Try to recover majority if chi2 no good
    bool do_majority_recover = m_extrafits && nmissing_more==0;
    do_majority_recover = do_majority_recover && (m_do_majority==1 || (m_do_majority>1 && !fullpassed));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() > ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() > ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB : m_Chi2Cut ));
    do_majority_recover = do_majority_recover && (newtrk.getChi2() < m_Chi2Cut_vetomaj || m_Chi2Cut_vetomaj < 0);


    if (do_majority_recover) {
      // recover majority
      /* if the N/N track have a bad chi2 we try to evaluate
         the track using the "best" track within the N combination
         of N-1/N tracks */
      //      FTKSetup::PrintMessageFmt(ftk::info,"pass majority cut");
      // in this block the used array of track combtrack[] is created
      // in the init block to save some time

      float bestchi2(newtrk.getChi2());
      int idbest(-1);
      m_nfits_rec += 1;
      const int recbegin = m_require_first ? 1 : 0;

      // loop over the combinations
      for (int ip=recbegin;ip<m_pmap->getNPlanes();++ip) {

        // skip planes with multiple hits
        if (m_norecovery_mask&(1<<ip))
          continue;

        // start for the complete track
        combtrack[ip] = newtrk;

        // reset the correspondent hit
        combtrack[ip].setFTKHit(ip,FTKHit());

        // remove the hits related with this plane
        int ix = m_pmap->getDim(ip,0);
        int iy = m_pmap->getDim(ip,1);
        int ndim = iy==-1? 1 : 2;

        // set the number of missing points and the related bitmask
        combtrack[ip].setNMissing(ndim);
        unsigned int newbitmask = (1<<ix);

        if (iy!=-1) newbitmask |= (1<<iy);

        newbitmask = m_complete_mask & ~newbitmask;

        combtrack[ip].setBitmask(newbitmask);

        FTKConstantBank *current_bank_complete = m_constant[m_region_for_superexp][simids.first];
        m_nfits_addrec += 1;
        current_bank_complete->linfit(simids.second,combtrack[ip]);

        // check if the chi2 is better
        if (combtrack[ip].getChi2()<bestchi2) {
          bestchi2 = combtrack[ip].getChi2();
          idbest = ip;
        }
      } // end loop over the combinations

      if (idbest>=recbegin) {
        // a track, excluding the 1st layer, was found
        // set the correct flags
        combtrack[idbest].setTypeMask(newtrk.getTypeMask()|(1<<1));
        combtrack[idbest].setNMissing(newtrk.getNMissing()+combtrack[idbest].getNMissing());
        combtrack[idbest].setBitmask(newtrk.getBitmask() & combtrack[idbest].getBitmask());
        newtrk = combtrack[idbest];
      }

    } // end block to recover complete tracks with bad chi2

    dof = m_ncoords - m_npars - newtrk.getNMissing();
    if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

    //
    //
    // Track accepted - Check here if you want to do something with accepted tracks
    //
    //

    // check if the track pass the quality requirements
    if (newtrk.getChi2()< ( m_Chi2DofCutSSB > -1 ? dof*m_Chi2DofCutSSB :
          (newtrk.getNMissing() > 0 ? m_Chi2Cut_maj : m_Chi2Cut) )  &&
        newtrk.getChi2() != 0 )
    {

      // Aux Doctor (Remove extrapolated Aux tracks with the same SSIDs)
      int accepted_AuxDoctor(0);
      // Disable Aux Doctor, auto-accept every track
      if (m_AuxDoctor) accepted_AuxDoctor = doAuxDoctor(newtrk,m_tracks);

      // to append the found track go trought the HW filter
      // add this track to track list only if
      // don't have common hits with another track,
      // in this case keep the best

      // appending pre-HW tracks to dump
      m_tracks_pre_hw.push_back(newtrk);

      // remains 0 if the track has to be added
      // -1 means is worse than an old track (duplicated)

      int accepted(0);

      // Disable hitwarrior, auto-accept every track
      if (m_HitWarrior!=0)
        accepted = doHitWarriorFilter(newtrk,m_tracks);

      // track accepted, no hits or no SSIDs shared with already fitted tracks
      if (accepted>=0 && accepted_AuxDoctor==0) {
        if (getDiagnosticMode())
          fillDiagnosticPlotsPerAcceptedTrack();

        // copy newtrk after truth assignment.
        m_tracks.push_back(newtrk);

        // removed an existing track
        if (accepted==1) {
          m_nfits_rej += 1;
          if (nmissing_more>0)
            m_nfits_rejmaj += 1;
        }
      } else if (accepted==-1) {
        // the track is candidate as ghost and is not the best
        // clean the list of the hits

        if (m_keep_rejected) {
          // copy newtrk after truth assignment.
          m_tracks.push_back(newtrk);
        }

        m_nfits_rej += 1;

        if (nmissing_more>0)
          m_nfits_rejmaj += 1;
      }
    } else {
      // if the track fail the cut on chi-square clean the hit list
      if (m_keep_rejected>1) {
        newtrk.setHWRejected( newtrk.getHWRejected()+100 );

        // copy newtrk after truth assignment.
        m_tracks.push_back(newtrk);
      }

      m_nfits_bad += 1;

      if (nmissing_more>0)
        m_nfits_badmaj += 1;
    }
  } // end insertion block
}

void TrackFitter711::printLayerInversionResult() {

  std::cout << "m_idplanes: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_idplanes[ii] << ", ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_idcoords: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_idcoords[ii] << ", ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_idCImap: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_idCImap[ii] << ", ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_idplanes_or: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_idplanes_or[ii] << ", ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_CImap: {";
  for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
    std::cout << m_CImap[ii] << " , ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_CImap_eff: {";
  for (int ii = 0; ii < m_ncoords_incomplete; ++ii) {
    std::cout << m_CImap_eff[ii] << " , ";
  }
  std::cout << "}" << std::endl;

  std::cout << "m_incomplete_coordsmask " << m_incomplete_coordsmask << std::endl;
  int number = m_incomplete_coordsmask;
  do {
    std::cout << number%2 << " , ";
    number = number >> 1;
  } while(number != 0);

  std::cout << "m_idplanes_eff: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_idplanes_eff[ii] << ", ";
  }
  std::cout << "}" << std::endl;

  std::cout << "missing_section: {";
  for (int ii = 0; ii < m_nplanes_ignored; ++ii) {
    std::cout << m_section_of_exp_layers[ii] << ", ";
  }
  std::cout << "}" << std::endl;
}

void TrackFitter711::printConnections() {

  if (!isDebugSuperExp())
    return;

  for (unsigned int iconn = 0; iconn < conn_subregs.size(); ++ iconn) {
    FTKSetup::PrintMessageFmt(ftk::info, " === Found connections === ");
    FTKSetup::PrintMessageFmt(ftk::info, " subreg: %d, sector: %d\n", conn_subregs.at(iconn), conn_sectors.at(iconn));
    if (!conn_gcbanks.at(iconn))
      FTKSetup::PrintMessageFmt(ftk::sevr, " error in constants ");
  }

}

void TrackFitter711::printRetrievedSSIDs() {
}

void TrackFitter711::printFoundHits() {

  if (!isDebugSuperExp())
    return;

  int ic(0);
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
    std::cout << " --- # of hits in each layers --- " << std::endl; //TODO: debug print output remove
    const int ndim = m_pmap->getPlane(m_idplanes[ip],0).getNDimension();
    for (int i=0;i!=ndim;++i,++ic) {
      std::cout << ic << " " << m_hits_more[ip].size(); //TODO: debug print output remove
      //for (unsigned ihits=0;ihits<m_hits_more[ip].size();++ihits) {
      //  std::cout << " " << (*m_hits_more)[ip];
      //}
      std::cout << std::endl;
    }
  }
}

void TrackFitter711::fillDiagnosticPlotsPerTrack() {
  m_histo_nconn_per_track->Fill(conn_subregs.size());
}

void TrackFitter711::fillDiagnosticPlotsPerTrackPerConn() {
  int combination = 1;
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
    combination *= m_hits_more[ip].size();
  }
  //std::cout << " combination = (" << cur_iconn << " " << combination << ")" << std::endl;
  if (combination == 0)
    m_histo_ncomb_per_track_each_conn[cur_iconn]->Fill(-999);
  else
    m_histo_ncomb_per_track_each_conn[cur_iconn]->Fill(combination);

  // fill per event variables
  m_ncomb_event_conn[cur_iconn] += combination;

  for (unsigned ihit = 0; ihit < m_hits_more[0].size(); ++ihit) {
    float found_phi = m_hits_more[0][ihit][0];
    float found_eta = m_hits_more[0][ihit][1];
    m_histo_delta_guessed_hit_found_hit->Fill(found_phi - IBLcoordPhi, found_eta - IBLcoordEta);
  }
}

void TrackFitter711::printEveryHitsInExtraLayers() {
  if (!isDebugSuperExp())
    return;
  std::cout <<"Checking NHits in unused SS: m_nregions:"<<m_nregions<<std::endl;
  ibl_module_with_hit = -999;
  for (int ibank=0; ibank<m_nregions; ++ibank) {
    FTKRoadStream* roadstream = m_roadinput->getStream(ibank);
    if (!roadstream)
      continue;
    const std::map<int, std::map<int,FTKSS> > unused_ssmap = roadstream->getUnusedSSMap();
    std::map<int, std::map<int,FTKSS> >::const_iterator unused_ssmap_iter;
    unused_ssmap_iter = unused_ssmap.begin();
    for (;unused_ssmap_iter != unused_ssmap.end(); ++unused_ssmap_iter) {
      std::map<int,FTKSS>::const_iterator unused_ss_iter = ((*unused_ssmap_iter).second).begin();
      for (;unused_ss_iter != ((*unused_ssmap_iter).second).end(); ++unused_ss_iter) {
        if (ibl_module_with_hit == -999) {
          if (((*unused_ss_iter).second).getNHits() > 0)
            ibl_module_with_hit = ((*unused_ss_iter).first);
          else
            ibl_module_with_hit = 0;
        }
        std::cout << " in : " << ((*unused_ss_iter).first) << " There are:  " << ((*unused_ss_iter).second).getNHits() << std::endl;
        //std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getEtaCode() <<std::endl;
        //std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getPhiModule() <<std::endl;
        //std::cout << ((((*unused_ss_iter).second).getHits()).at(0)).getSector() <<std::endl;
      }
    }
  }
}

void TrackFitter711::fillDiagnosticPlotsPerAcceptedTrack() {
  for (int ip=0; ip!=m_nplanes_ignored; ++ip) {
    m_histo_nhitsfound_per_track_each_layer[ip]->Fill(m_hits_more[ip].size());
    m_histo_nhitsfound_per_track_each_layer_each_conn[ip]->Fill(m_hits_more[ip].size(), cur_iconn);
  }
  if (newtrk.getBitmask()&1<<0) {
    m_IBL_res->Fill(newtrk.getCoord(0) - IBLcoordPhi, newtrk.getCoord(1) - IBLcoordEta);
  }
}

bool TrackFitter711::isIBLhitsFound() {
  if (m_hits_more[0].size() > 0)
    return true;
  return false;
}

bool TrackFitter711::passedNMissLayerRequirement(int nmiss) {

  int threshold = m_max_missing_extraplanes;
  if (m_super_extrapolate && m_super_exp_allow_extra_miss && isTransitionRegion())
    threshold ++;

  bool passFlag = false;
  if (nmiss <= threshold)
    passFlag = true;

  if (isDebugSuperExp()) {
    std::cout << "   nmiss = " << nmiss << std::endl;
    std::cout << "passFlag = " << passFlag << std::endl;
  }

  return passFlag;
}

bool TrackFitter711::isTransitionRegion() {
  if (m_check_TR_by == kModuleID) {
    const int &moduleID = m_FC_relations[m_region_for_superexp][conn_subregs.at(cur_iconn)]->getSimilarStereoIDs(m_sector_for_superexp,cur_iconn)[m_nplanes_ignored-1];
    return isTransitionRegion(moduleID);
  } else if (m_check_TR_by == kEta) {
    const float &eta = newtrkI.getEta();
    return isTransitionRegion(eta);
  } else {
    assert(0);
    return false;
  }
}

bool TrackFitter711::isTransitionRegion(float eta) {
  double abseta = fabs(eta);
  bool isFlag = false;
  if (abseta < m_transition_region_max_eta)
    if (abseta > m_transition_region_min_eta)
      isFlag = true;
  if (isDebugSuperExp()) {
    std::cout << "abseta of incomplete track = " << abseta << std::endl;
    std::cout << "                    isFlag = " << isFlag << std::endl;
  }
  return isFlag;
}

bool TrackFitter711::isTransitionRegion(int moduleid) {
  bool isFlag = false;
  int mod_moduleid_100 = moduleid % 100;
  if((mod_moduleid_100==0 ) || (mod_moduleid_100==12) || (mod_moduleid_100==21) ||(mod_moduleid_100==31))
    isFlag = true;
  if (isDebugSuperExp()) {
    std::cout << "module id of last SCT = " << moduleid << std::endl;
    std::cout << "               isFlag = " << isFlag << std::endl;
  }
  return isFlag;
}

void TrackFitter711::endSuperExtrapolate() {
  if (m_bounds) {
    for (int ip=0; ip < m_nplanes_ignored; ++ip) {
      if (m_bounds[ip])
        delete [] m_bounds[ip];
    }
  }
}

void TrackFitter711::fillDiagnosticPlotsPerEvent() {
  for (int iconn = 0; iconn < m_use_n_conn; ++iconn) {
    m_histo_ncomb_each_conn[iconn]->Fill(m_ncomb_event_conn[iconn]);
  }
  list<FTKTrack>::iterator itrack2 = m_tracks.begin();
  for (;itrack2!=m_tracks.end();itrack2++) {
    for (int ic = 0; ic < m_ncoords; ++ic) {
      m_histo_bitmask->Fill(ic, ((((*itrack2).getBitmask())&(1<<ic))>>ic));
    }
  }
}

bool TrackFitter711::isRealRegion() {
  // dumb way of checking if something to be filled or not.
  bool dofill = false;
  for (int iconn = 0; iconn < m_use_n_conn; ++iconn) {
    if (m_ncomb_event_conn[iconn] != 0)
      dofill = true;
  }
  return dofill;
}

//eof
