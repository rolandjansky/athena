/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/DiphotonVertexIDTool.h"

#include "TLorentzVector.h"
#include "TSystem.h"

// #if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
// #endif

#include <iostream>
#include <algorithm>
#include <math.h>

#define DEBUG(x) cout << "-- <DEBUG> DiphotonVertexIDTool: " << x << endl
double pi = acos(-1.);

DiphotonVertexIDTool::DiphotonVertexIDTool(
  bool debug,
  TString path_file_1,
  TString path_file_2, 
  unsigned int maxVertices,
  float classifier_cut_1, 
  float classifier_cut_2,
  float cut_pt_track, 
  float cut_deltaphi_track_cluster ) : 
    m_debug( debug ),
    m_maxVertices( maxVertices ), 
    m_classifierCut1( classifier_cut_1 ), 
    m_classifierCut2( classifier_cut_2 ),
    m_convPtCut( cut_pt_track ),
    m_deltaphiCut( cut_deltaphi_track_cluster )
{
  gSystem->ExpandPathName(path_file_1);
  gSystem->ExpandPathName(path_file_2);
  
  m_reader1 = new TMVA::Reader( "!Color:!Silent" );
  m_reader1->AddVariable( "log_PV_pt_scal2 := log10(PV_pt_scal2)", &log_PV_pt_scal2 );
  m_reader1->AddVariable( "PV_deltaphi", &PV_deltaphi );
  m_reader1->AddVariable( "PV_deltaz", &PV_deltaz );
  m_reader1->BookMVA( "MLP method" , path_file_1);
  
  m_reader2 = new TMVA::Reader( "!Color:!Silent" );
  m_reader2->AddVariable( "log_PV_pt_scal2 := log10(PV_pt_scal2)", &log_PV_pt_scal2 );
  m_reader2->AddVariable( "log_PV_pt_scal := log10(PV_pt_scal)", &log_PV_pt_scal );
  m_reader2->AddVariable( "PV_deltaphi", &PV_deltaphi );
  m_reader2->AddVariable( "PV_deltaz", &PV_deltaz );
  m_reader2->BookMVA( "MLP method", path_file_2);
}

DiphotonVertexIDTool::~DiphotonVertexIDTool()
{
  delete m_reader1;
  delete m_reader2;
}

/** Determine the case to treat:
  * Case 0 = at least one photon with a track associated to a PV (return -vertex)
  * Case 1 = at least one converted photon with Si track(s) but not in case 0
  * Case 2 = all the others (unconverted photons or TRT conversions)
  **/
vector<int> DiphotonVertexIDTool::get_vertex(unsigned int indexPhoton1,
  unsigned int indexPhoton2,
  float beamSpot_z,
  float beamSpot_sigma_z,
  vector<float> *PV_px,
  vector<float> *PV_py,
  vector<float> *PV_z,
  vector<float> *PV_sumPt,
  vector<float> *Photon_PV_sumpt2,
  vector<vector<int> > *PV_trk_index,
  vector<float> *ph_cl_E,
  vector<float> *ph_etas2,
  vector<float> *ph_cl_phi,
  vector<float> *ph_CaloPointing_zvertex,
  vector<float> *ph_CaloPointing_sigma_zvertex,
  vector<float> *ph_HPV_zvertex,
  vector<float> *ph_HPV_sigma_zvertex,
  vector<int> *ph_convtrk1nPixHits,
  vector<int> *ph_convtrk1nSCTHits,
  vector<int> *ph_convtrk2nPixHits,
  vector<int> *ph_convtrk2nSCTHits,
  vector<float> *ph_pt1conv,
  vector<float> *ph_pt2conv,
  vector<vector<vector<int> > > *ph_vx_convTrk_index,
  vector<vector<vector<float> > > *ph_vx_paramsAtConvVtx_phi,
  vector<int> *GSF_trk_trk_index)
{
  vector<int> photonIndices;
  photonIndices.push_back(indexPhoton1);
  photonIndices.push_back(indexPhoton2);
  
  mlp_values.clear();
  mlp_ranking.clear();
  m_convertedPhotons.clear();
  m_zCommon = m_zError = -9999.;
  vector<int> vIndex;
  
  m_case = 2;
  if (ph_HPV_sigma_zvertex != 0)
    m_case = get_case(&photonIndices, ph_vx_convTrk_index, ph_vx_paramsAtConvVtx_phi,
       ph_pt1conv, ph_pt2conv, ph_cl_phi, GSF_trk_trk_index, PV_trk_index,
       ph_convtrk1nPixHits,ph_convtrk2nPixHits,ph_convtrk1nSCTHits,ph_convtrk2nSCTHits );
  
  if (m_debug) DEBUG("Treating case " << m_case);
  if (m_case <= 0) // case 0, vertex already defined (and sign flipped)
    vIndex.push_back( -m_case );
  else if (m_case == 1 || m_case == 2) // case 1 or case 2, will get the TMVA response
  {
    // Calculate m_zCommon and error 
    // (weighted mean of beamspot and pointing/HPV for each photon)
    m_zCommon = beamSpot_z/square(beamSpot_sigma_z);
    m_zError = 1./square(beamSpot_sigma_z);

    vector<int>::iterator it = photonIndices.begin();
    for ( ; it != photonIndices.end(); ++it)
    {
      // For Si conversion use HPV, otherwise use CaloPonting
      // They should be equivalent for unconv except in p1032 where there was a bug...
      if (m_case == 1 && isConvertedPhoton(*it))
      {
        m_zCommon += ph_HPV_zvertex->at(*it)/square(ph_HPV_sigma_zvertex->at(*it));
        m_zError += 1./square(ph_HPV_sigma_zvertex->at(*it));
      }
      else
      {
        m_zCommon += ph_CaloPointing_zvertex->at(*it)/square(ph_CaloPointing_sigma_zvertex->at(*it));
        m_zError += 1./square(ph_CaloPointing_sigma_zvertex->at(*it));
      }
    }
    m_zCommon /= m_zError;
    m_zError = 1./sqrt( m_zError );
    
    // phi of the diphoton system
    float diphotonPhi = getDiphotonPhi(&photonIndices, ph_cl_E, ph_etas2, ph_cl_phi);
    if (m_debug) DEBUG("zCommon = " << m_zCommon << " zError = " << m_zError
                       << " diphotonPhi = " << diphotonPhi);
        
    // ****** Calculate the MVA response for each vertex ****
    TMVA::Reader *reader = (m_case == 1) ? m_reader1 : m_reader2;
        
    for(unsigned int i_PV =0; i_PV<(*PV_trk_index).size(); ++i_PV)
    {
      if (PV_trk_index->at(i_PV).size()==0) continue; // skip vertices with no tracks
      
      // skip vertices above 10 sigma from pointing
      PV_deltaz = (m_zCommon - PV_z->at(i_PV))/m_zError;
      if (m_case == 2 && fabs(PV_deltaz) > 10) 
      {
        if (m_debug) DEBUG("MLP = -9999. deltaZ/sigma = " << PV_deltaz);
        continue; 
      }
      // Set the inputs for the MLP and calculate the response
      // log sumPt, log sumPt2, deltaPhi(diphoton,PV), deltaZ/sigma      
      TLorentzVector v_PV;
      v_PV.SetPxPyPzE(PV_px->at(i_PV),PV_py->at(i_PV),0,0);
      PV_deltaphi = fabs(deltaPhi(v_PV.Phi(), diphotonPhi));
      log_PV_pt_scal = log10(PV_sumPt->at(i_PV));
      log_PV_pt_scal2 = log10(Photon_PV_sumpt2->at(i_PV));
      
      float mlp = reader->EvaluateMVA("MLP method");
      if (m_debug) DEBUG("MLP = " << mlp << 
                " deltaZ/sigma = " << PV_deltaz << 
                " deltaPhi = " << PV_deltaphi <<
                " sumpt = " << PV_sumPt->at(i_PV) <<
                " sumpt2 = " << Photon_PV_sumpt2->at(i_PV));
      
      // Insert the mlp result in mlp_values and the PV index in mlp_ranking,
      // keeping mlp_values sorted in decreasing order
      vector<float>::iterator itMLP = mlp_values.begin();
      vector<int>::iterator itRank = mlp_ranking.begin();
      for ( ; itMLP != mlp_values.end(); ++itMLP, ++itRank)
        if (mlp > (*itMLP) ) break;
      
      mlp_values.insert(itMLP, mlp);
      mlp_ranking.insert(itRank, i_PV);
    }
    
    // Keep only the maximal number of allowed vertices 
    // and remove those that have mlp below the threshold
    if (mlp_ranking.size()) // always get at least 1 vertex if available
      vIndex.push_back( mlp_ranking.front() ); 
    for (unsigned int iV = 1; iV < mlp_ranking.size(); ++iV)
    {
      if (iV >= m_maxVertices) break; // already got the maximum number of vertices
      float mlpV = mlp_values[iV];
      if ( (m_case == 1 && mlpV < m_classifierCut1) || 
           (m_case == 2 && mlpV < m_classifierCut2) )
        break; // remaining vertices do not pass mlp cut
      vIndex.push_back( mlp_ranking[iV] );
    }
  
  } // if (m_case == 1 || m_case == 2)
  else
  {
    cout << "Invalid case: " << m_case << endl;
  }
  
  if (vIndex.empty())
  {
    if (m_debug) DEBUG("No vertex selected, returning first primary vertex");
    vIndex.push_back(0);
  }
  return vIndex;
}


float DiphotonVertexIDTool::deltaPhi(float phi1, float phi2)
{
  float PHI = phi1-phi2;
  if (PHI<=pi) {
    if (PHI >= -pi){
      return PHI;
    }
    else {
      return PHI + 2*pi;
    }
  }
  return 2*pi - PHI;
}


int DiphotonVertexIDTool::get_case(vector <int> *photonIndices,
                                   vector<vector<vector<int> > > *ph_vx_convTrk_index,
                                   vector<vector<vector<float> > > *ph_vx_Trk_phi,
                                   vector<float> *ph_pt1conv,
                                   vector<float> *ph_pt2conv,
                                   vector<float> *ph_cl_phi,
                                   vector<int> *GSF_trk_trk_index,
                                   vector<vector<int> > *PV_trk_index,
                                   vector <int> *ph_convtrk1nPixHits,
                                   vector <int> *ph_convtrk2nPixHits,
                                   vector <int> *ph_convtrk1nSCTHits,
                                   vector <int> *ph_convtrk2nSCTHits)
{
  int iCase = 2;
  int vertex = -1;
  
  // Loop over the photons to see if they are Si conversions, matched to a vertex
  vector<int>::iterator itPh = photonIndices->begin();
  for( ; itPh != photonIndices->end(); itPh++)
  {
    vector<int> isSi = isSiConversion(*itPh, ph_convtrk1nPixHits, ph_convtrk2nPixHits,
                                      ph_convtrk1nSCTHits, ph_convtrk2nSCTHits);
    if (m_debug) 
      DEBUG("Photon " << (*itPh) << " has " << isSi.size() << " track(s) with Si hits");

    // Loop over Si tracks to find if they are valid and associated to a vertex
    for (vector<int>::iterator it = isSi.begin(); it != isSi.end(); ++it)
    {
      float trkPt = (*it == 0) ? ph_pt1conv->at(*itPh) : ph_pt2conv->at(*itPh);
      float trkPhi = -9999.;
      if (!ph_vx_Trk_phi)
        trkPhi = ph_cl_phi->at(*itPh); // to avoid testing deltaPhi (2011)
      else if (ph_vx_Trk_phi->size() >= (unsigned int) (*itPh) && 
          ph_vx_Trk_phi->at(*itPh).size() &&
          ph_vx_Trk_phi->at(*itPh)[0].size() >= (unsigned int) (*it) )
        trkPhi = ph_vx_Trk_phi->at(*itPh)[0][*it];
      else
      {
        if (m_debug) DEBUG("Could not get trkPhi");
        continue;
      }
      
      if (!acceptSiTrack(ph_cl_phi->at(*itPh), trkPt, trkPhi)) 
        continue; // Si track rejected, still case 2
      iCase = 1;
      m_convertedPhotons.push_back(*itPh); // flag photon as Si conversion
      
      // Get the index of track in GSF block and check if track belongs to a vertex
      int iGSF = getGSFTrkIndex(*itPh, *it, ph_vx_convTrk_index);
      if (m_debug) DEBUG("Checking photon " << (*itPh) << " GSFtrack " << iGSF);
      
      int new_vertex = getVertexFromTrack(iGSF, GSF_trk_trk_index, PV_trk_index);
      if (vertex != -1 && m_debug)
        DEBUG("Photon is associated to a primary (or pileup) vertex");
      if (vertex == -1)
        vertex = new_vertex;
      else if (vertex != new_vertex && new_vertex != -1 && m_debug)
      {
        DEBUG("Warning: tracks matched to different vertices --> " << vertex <<
          " and " << new_vertex);
      }
    }
  }
  if (vertex >= 0) return -vertex;
  return iCase;
}
    	
/** Return a vector with the indices of Si-tracks from a photon conversion **/
vector<int> DiphotonVertexIDTool::isSiConversion(unsigned int iPh, 
																				 vector<int> *ph_convtrk1nPixHits,
																				 vector<int> *ph_convtrk2nPixHits,
																				 vector<int> *ph_convtrk1nSCTHits,
																				 vector<int> *ph_convtrk2nSCTHits)
{
	vector<int> isSiSi;
	if (ph_convtrk1nPixHits->at(iPh) + ph_convtrk1nSCTHits->at(iPh))
		isSiSi.push_back(0);
	if (ph_convtrk2nPixHits->at(iPh) + ph_convtrk2nSCTHits->at(iPh))
		isSiSi.push_back(1);
	return isSiSi;
}

/** Return the index of the iPhTrk-th track of the iPh-th photon in the GSF block **/
int DiphotonVertexIDTool::getGSFTrkIndex(int iPh, int iPhTrk, 
  vector<vector<vector<int> > > *ph_vx_convTrk_index)
{
  if (!ph_vx_convTrk_index->at(iPh).size() || !ph_vx_convTrk_index->at(iPh)[0].size())
    return -1;
  return ph_vx_convTrk_index->at(iPh)[0][iPhTrk];
}


/** Return true if the track is a good conversion candidate, based on track Pt
  *  and deltaPhi between track and photon cluster
  **/  
bool DiphotonVertexIDTool::acceptSiTrack(int iPh, int iGSF, 
  vector<float> *ph_cl_phi, vector<float> *GSF_trk_pt, vector<float> *GSF_trk_phi)
{
	if (iGSF < 0 || iGSF >= (int) GSF_trk_phi->size()) return false;
	return acceptSiTrack(ph_cl_phi->at(iPh), GSF_trk_pt->at(iGSF), GSF_trk_phi->at(iGSF));
}

/** Return true if the track is a good conversion candidate, based on track Pt
  *  and deltaPhi between track and photon cluster
  **/  
bool DiphotonVertexIDTool::acceptSiTrack(float clusterPhi, float trkPt, float trkPhi)
{
	float dphi = fabs(deltaPhi(clusterPhi, trkPhi));
	bool result = (trkPt > m_convPtCut && dphi < m_deltaphiCut);
	if (m_debug) 
	{
	  if (!result)
      DEBUG("Si conversion rejected: pt = " 
	          << trkPt << " deltaPhi = " << dphi);
	  else
	    DEBUG("Si conversion found");
	}
  return result;
}


/** Return the index of the vertex to which the GSF track belongs (or -1) **/
int DiphotonVertexIDTool::getVertexFromTrack(int iGSF, 
                                             vector<int> *GSF_trk_trk_index,
                                             vector<vector<int> > *PV_trk_index)
{
  if (iGSF < 0) return -1;
  if (GSF_trk_trk_index && iGSF >= (int) GSF_trk_trk_index->size()) return -1;  
  int iTrk = iGSF;
  // If GSF_trk_trk_index was given, get the index of the track in trk block
  // (for 2011 data/MC there is no GSF so a null pointer must be passed)
  if (GSF_trk_trk_index != 0) iTrk = GSF_trk_trk_index->at(iGSF);
  if (iTrk < 0) return -1;
  
  // Loop over the tracks in each PV, looking for the track index
  for (unsigned int iPV = 0; iPV < PV_trk_index->size(); ++iPV)
  {
    vector<int>::iterator it = PV_trk_index->at(iPV).begin();
    for ( ; it != PV_trk_index->at(iPV).end(); ++it)
      if (*it == iTrk)
        return iPV;
  }
  return -1; // track does not belong to any vertex
}

/** Return phi of the diphoton system **/
float DiphotonVertexIDTool::getDiphotonPhi(vector<int> *photonIndices, 
  vector<float> *ph_cl_E, vector<float> *ph_etas2, vector<float> *ph_cl_phi)
{
  TLorentzVector v, v1;
  vector<int>::iterator it = photonIndices->begin();
  for ( ; it != photonIndices->end(); ++it)
  {
    v1.SetPtEtaPhiM(ph_cl_E->at(*it)/cosh(ph_etas2->at(*it)), 
                    ph_etas2->at(*it),
                    ph_cl_phi->at(*it),
                    0.);
    v += v1;
  }
  return v.Phi();
}

bool DiphotonVertexIDTool::isConvertedPhoton(int phIndex)
{
  vector<int>::iterator it;
  it = std::find(m_convertedPhotons.begin(), m_convertedPhotons.end(), phIndex);
  return it != m_convertedPhotons.end();
}
