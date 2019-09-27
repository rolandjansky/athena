/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "PromptUtils.h"

// C/C++
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <stdint.h>
#include <stdexcept>


using namespace std;

//=============================================================================
double Prompt::GetVertexFitProb(const xAOD::Vertex *vtx)
{
  //
  // Get xAOD::Vertex fit probability
  //
  double fit_prob = -1;
   
  if(!vtx) {
    return fit_prob;
  }

  if(vtx->numberDoF() > 0 && vtx->chiSquared() > 0) {
    fit_prob = TMath::Prob(vtx->chiSquared(), vtx->numberDoF()); 
  }

  return fit_prob;
}


//=============================================================================
double Prompt::GetDistance(const xAOD::Vertex *vtx1, const xAOD::Vertex *vtx2)
{
  if((!vtx1) || (!vtx2)) {
    return 9999.0;
  }

  return Prompt::GetDistance(vtx1->position(), vtx2->position());
}

//=============================================================================
double Prompt::GetDistance(const Amg::Vector3D &vtx1, const Amg::Vector3D &vtx2)
{
  const double dx = vtx2.x()-vtx1.x();
  const double dy = vtx2.y()-vtx1.y();
  const double dz = vtx2.z()-vtx1.z();

  return std::sqrt(dx*dx + dy*dy + dz*dz);
}

//=============================================================================
double Prompt::GetNormDist(const Amg::Vector3D &vtx1, const Amg::Vector3D &vtx2, const std::vector<float> &errorMatrix, MsgStream &msg)
{
  double significance = -10;

  double distx =  vtx1.x()- vtx2.x();
  double disty =  vtx1.y()- vtx2.y();
  double distz =  vtx1.z()- vtx2.z();

  Eigen::Matrix<double, 3, 3,0, 3, 3> PrimCovMtx;  //Create matrix
  //mutable InDet::AmgSymMatrix(3) PrimCovMtx;  //Create matrix

  if(errorMatrix.empty()) {
    msg << "GetNormDist - Error matrix is empty" << endreq;
    return significance; 
  }

  PrimCovMtx(0,0) = static_cast<double> (errorMatrix[0]);
  PrimCovMtx(0,1) = static_cast<double> (errorMatrix[1]);
  PrimCovMtx(1,0) = static_cast<double> (errorMatrix[1]);
  PrimCovMtx(1,1) = static_cast<double> (errorMatrix[2]);
  PrimCovMtx(0,2) = static_cast<double> (errorMatrix[3]);
  PrimCovMtx(2,0) = static_cast<double> (errorMatrix[3]);
  PrimCovMtx(1,2) = static_cast<double> (errorMatrix[4]);
  PrimCovMtx(2,1) = static_cast<double> (errorMatrix[4]);
  PrimCovMtx(2,2) = static_cast<double> (errorMatrix[5]);

  if(PrimCovMtx.determinant() == 0) {
    msg << "GetNormDist - Matrix can not be inversed" << endreq;
    return significance;
  }

  Eigen::Matrix<double, 3, 3,0, 3, 3> WgtMtx = PrimCovMtx.inverse();

  //InDet::AmgSymMatrix(3) WgtMtx = PrimCovMtx.inverse();

  significance = distx*WgtMtx(0,0)*distx
               + disty*WgtMtx(1,1)*disty
               + distz*WgtMtx(2,2)*distz
               + 2.*distx*WgtMtx(0,1)*disty
               + 2.*distx*WgtMtx(0,2)*distz
               + 2.*disty*WgtMtx(1,2)*distz;

  if(!(significance > 0)) {
    msg << "GetNormDist - significance is negative" << endreq;
    return significance;
  }

  significance = std::sqrt(significance); 

  return significance;
} 

//=============================================================================
void Prompt::FillTH1(TH1 *h, double val, double weight) 
{
  //
  // Read new event entry
  //
  if(!h) {
    return;
  }
  
  const double xmax = h->GetXaxis()->GetXmax();
  const double xmin = h->GetXaxis()->GetXmin();
  
  if(!(val < xmax)) {
    h->Fill(h->GetXaxis()->GetBinCenter(h->GetNbinsX()), weight);
  }
  else if (!(val > xmin)) {
    h->Fill(h->GetXaxis()->GetBinCenter(1), weight);
  }
  else {
    h->Fill(val, weight);
  }
}

//=============================================================================
std::string Prompt::PrintPromptVertexAsStr(const xAOD::Vertex *vtx, MsgStream &msg)
{
  std::stringstream str;
  
  str << "xAOD::Vertex pointer = " << vtx<< std::endl;

  if(vtx) {
    float chisquared = -9999;
    float numberdof  = -9999;
    int   index      = -999;

    if(!GetVar(vtx, index, "SecondaryVertexIndex")) {
      msg << "PprintPromptVertexAsStr -- not valid vtx SecondaryVertexIndex!!!" << endreq;
    }

    if(!GetVar(vtx, chisquared, "chiSquared")) {
      msg << "PrintPromptVertexAsStr -- not valid vtx chiSquared!!!" << endreq;
    }
    
    if(!GetVar(vtx, numberdof, "numberDoF")) {
      msg << "PrintPromptVertexAsStr -- not valid vtx numberDoF!!!" << endreq;
    }

    str << "   index      " << index             << std::endl;
    str << "   position   " << vtx->position  () << std::endl;
    str << "   x          " << vtx->x         () << std::endl;
    str << "   y          " << vtx->y         () << std::endl;
    str << "   z          " << vtx->z         () << std::endl;
    str << "   chiSquared " << chisquared        << std::endl;
    str << "   numberDoF  " << numberdof         << std::endl;

    str << "   covariance.size() = " << vtx->covariance().size() << std::endl;
    str << "   covariance        = [";
    
    for(const float val: vtx->covariance()) {
      str << val << ", ";
    }

    str << "]";
  }

  return str.str();
}

//=============================================================================
std::string Prompt::VtxAsStr(const xAOD::Vertex *vtx, bool print_tracks)
{  
  if(!vtx) {
    return "VtxAsStr - null pointer";
  }

  stringstream str;

  float distToPV = -1;
  float sigToPV = -1;
  
  Prompt::GetAuxVar(*vtx, distToPV, "distToPriVtx");
  Prompt::GetAuxVar(*vtx, sigToPV,  "normDistToPriVtx");
  
  str << "xAOD::Vertex - " << vtx << ": ntrack=" << vtx->nTrackParticles()
      << ", chi2/ndof=" << vtx->chiSquared() << "/" << vtx->numberDoF()
      << ", prob=" << Prompt::GetVertexFitProb(vtx)
      << ", (x, y, z)=(" << vtx->x() << ", " << vtx->y() << ", " << vtx->z() << ")"
      << ", distToPV=" << distToPV
      << ", sigToPV=" << sigToPV;

  if(print_tracks) {
    str << endl;

    std::vector<const xAOD::TrackParticle *> tracks;

    for(unsigned i = 0; i < vtx->nTrackParticles(); ++i) {
      tracks.push_back(vtx->trackParticle(i));
    }

    std::sort(tracks.begin(), tracks.end(), Prompt::SortByIDTrackPt());

    for(unsigned i = 0; i < tracks.size(); ++i) {
      str << "     xAOD::Vertex track[" << i << "] " << TrkAsStr(tracks.at(i)) << endl;
    }
  }

  return str.str();
}

//=============================================================================
std::string Prompt::TruthAsStr(const xAOD::IParticle &particle)
{ 
  int truthOrigin = -1;
  int truthType   = -1;

  Prompt::GetAuxVar(particle, truthOrigin, "truthOrigin");
  Prompt::GetAuxVar(particle, truthType,   "truthType");

  std::stringstream str;
  str << "truthType=" << truthType << ", truthOrigin=" << truthOrigin;
  
  return str.str();
}

//=============================================================================
std::string Prompt::TrkAsStr(const xAOD::TrackParticle *trk)
{  
  if(!trk) {
    return "TrkAsStr - null pointer";
  }

  stringstream str;
  
  str << "xAOD::TrackParticle - " << trk << ": pT=" << trk->pt()
      << ", eta=" << trk->eta()
      << ", phi=" << trk->phi();

  return str.str();
}

//=============================================================================
std::string Prompt::PrintResetStopWatch(TStopwatch &watch)
{
  watch.Stop();
  
  double realt = watch.RealTime();
  double cput  = watch.CpuTime();
  
  watch.Reset();
  watch.Start();
  
  const int hours = static_cast<int>(realt/3600.0);
  const int  min  = static_cast<int>(realt/60.0) - 60*hours;
  
  realt -= hours * 3600;
  realt -= min * 60;
  
  if (realt < 0) realt = 0;
  if (cput  < 0) cput  = 0;
  
  const int sec = static_cast<int>(realt);
  
  std::stringstream str;
  str << "Real time " 
      << setw(2) << setfill('0') << hours 
      << ":" << setw(2) << setfill('0') << min
      << ":" << setw(2) << setfill('0') << sec
      << " CPU time " << setprecision(3) << fixed << cput;
  
  return str.str();
}
