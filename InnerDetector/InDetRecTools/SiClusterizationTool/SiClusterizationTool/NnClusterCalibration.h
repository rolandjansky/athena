/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef BTAGTOOL_NnClusterizationFactory_C
 #define BTAGTOOL_NnClusterizationFactory_C
 
 /******************************************************
     @class NnClusterizationFactory
     Package : JetTagTools 
     Created : January 2011
     DESCRIPTION:

 ********************************************************/
 
 #include "GaudiKernel/AlgTool.h"
 #include "GaudiKernel/ToolHandle.h"
 
 #include <vector>
 #include <string>
 #include <map>
 
 #include <TString.h>
 
 class TTrainedNetwork;
 class TH1;
 
 namespace Analysis {
 
 class JetFitterNetworkToHistoTool;
 
 class JetFitterTagInfo;
 
 class CalibrationBroker;
   
 
 static const InterfaceID IID_NnClusterizationFactory("Analysis::NnClusterizationFactory", 1, 0);
 
   class NnClusterizationFactory : public AlgTool  {
     
   public:
     
     /** AlgTool interface methods */
     static const InterfaceID& interfaceID() { return IID_NnClusterizationFactory; };
 
     NnClusterizationFactory(const std::string& name,
                     const std::string& n, const IInterface* p);
     ~NnClusterizationFactory();
     
     virtual StatusCode initialize();
     virtual StatusCode finalize();
     
     void fillLikelihoodValues(JetFitterTagInfo & myTagInfo,
                               const std::string & jetauthor,
                               double jetpT,
                               double jeteta,
                               double IP3dlike=-5000);
 
     
   private:
 
     void loadCalibration(const std::string & jetauthor);
 
     void initializeCalibrationFile();
 
     std::map<std::string,TTrainedNetwork*> m_NN;
 
     std::string m_calibrationDirectory;
 
     std::string m_calibrationSubDirectory;
 
     ToolHandle<JetFitterNetworkToHistoTool> m_networkToHistoTool;
     
     bool m_useCombinedIPNN;
     
     ToolHandle<CalibrationBroker> m_calibrationTool;
 
     int m_maximumRegisteredLayers;
 
   };
   
 }//end Analysis namespace
 
 #endif
