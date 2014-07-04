/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronLikelihoodPIDTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ELECTRONLIKELIHOODPIDTOOL_H
#define DERIVATIONFRAMEWORK_ELECTRONLIKELIHOODPIDTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"

namespace DerivationFramework {
 
  struct LLHInputs {
            double eta; 	
            double eT; 		
            double f3; 		
            double rHad; 	
            double rHad1; 	
            double Reta; 	
            double w2; 		
            double f1; 		
            double eratio; 	
            double deltaEta; 	
            double d0; 		
            double TRratio; 	
            double d0sigma; 	
            double rphi; 	
            double deltaPoverP; 
            double deltaphires; 
            double ip; 		
            int nSi; 		
            int nSiDeadSensors; 
            int nPix; 		
            int nPixDeadSensors;
            int nBlayer; 	
            int nBlayerOutliers;
            bool expectBlayer; 	
            int convBit; 	
  }; 

  class ElectronLikelihoodPIDTool : public AthAlgTool, public IAugmentationTool {
    public: 
      ElectronLikelihoodPIDTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
      bool runLLHTool(LLHInputs) const;

    private:
      std::string m_sgName;
      std::string m_pdfRootFile;
      Root::TElectronLikelihoodTool* m_lhTool;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ELECTRONLIKELIHOODPIDTOOL_H
