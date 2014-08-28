/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// PAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////


#ifndef RECOTOOLS_PRIMARYVERTEXFINDER_H
#define RECOTOOLS_PRIMARYVERTEXFINDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUprimaryVertexFinder.h"

#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooKeysPdf.h"

namespace Rec
{
  class TrackParticle;
}

class TH1;
class TFile;

class PAUprimaryVertexFinder : public AthAlgTool, virtual public IPAUprimaryVertexFinder {
 public:
  
  /** AlgTool like constructor */
  PAUprimaryVertexFinder(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUprimaryVertexFinder();
  
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  /** Interface method for use with TrackParticle and given vertex position - AOD */
  const std::vector<double> PrimaryVxFinder(const double z,const double sigma_z) const;
  likelihoodPV RooPrimaryVxFinder(const double z, const double sigma_z);

  //    double PrimaryVxFinder(double z,double sigma_z); 
  
  /*   StatusCode initialize();
       StatusCode finalize();
       StatusCode execute();*/
  
 private:
  
  /// a handle on Store Gate 
   TH1* m_signal;
   TH1* m_bruit;    
   TFile* m_inputPDFfile ;

   TFile* m_inputPDFSigfile ;
   TFile* m_inputPDFBkgfile ;
   RooWorkspace* m_w_sig;
   RooWorkspace* m_w_bkg;

};


#endif // TRKEXTOOLS_PRIMARYVERTEXFINDER_H
