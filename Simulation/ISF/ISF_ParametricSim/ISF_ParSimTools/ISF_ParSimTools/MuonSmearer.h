/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_MUONSMEARER_H
#define ISF_PARSIMTOOLS_MUONSMEARER_H

// ISF
#include "ISF_ParSimInterfaces/IChargedSmearer.h"

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// ROOT
#include "TFile.h"
#include "TLorentzVector.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "TVector3.h"

// CLHEP
#include "CLHEP/Random/RandomEngine.h"

class TTree;

namespace ISF {
    class ISFParticle;
}

namespace iParSim {
      
  /** 
   @class MuonSmearer

   Creates xAOD::TrackParticles from ISFParticles
   this is a smearer for muons

   @author Miha.Muskinja -at- cern.ch 
   */
      
  class MuonSmearer : public AthAlgTool, 
        virtual public iParSim::IChargedSmearer
  {
    public:

      /**Constructor */
      MuonSmearer(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      ~MuonSmearer();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();

      /** Smear the existing xAOD::TrackParticle */
      bool smear(xAOD::TrackParticle* xaodTP) const override;
      
      /** Return the pdg code of the smearer */
      unsigned int pdg() const override;

    private:

      /**MC12 Muon Smearer files */
      std::map<unsigned int, std::string>  m_filenamesMC12MuonSmearer; //!< reordered in a map for look-up
      std::string m_filenameMC12MuonPtBins;
      std::string m_filenameMC12MuonEtaBins;

      bool getBinsFromFile(std::string ptFile, std::string etaFile);
      
      int getEtaBin(const double) const;
      
      int getPtBin(const double) const;

      std::vector<double>  m_pt_min;
      std::vector<double>  m_pt_max;
      std::vector<double>  m_eta_min;
      std::vector<double>  m_eta_max;
      
      /** Mode of the muon smearer: 1 for ID only and 2 for MS only */
      int m_para_code;
      
      /** Root file with the muon smearing parameters */
      TFile* m_file_para;

      ServiceHandle<IAtRndmGenSvc>         m_randomSvc;                //!< Random Svc
      CLHEP::HepRandomEngine*              m_randomEngine;             //!< Random Engine
      std::string                          m_randomEngineName;         //!< Name of the random number stream
  };

} // end of namespace

#endif 

