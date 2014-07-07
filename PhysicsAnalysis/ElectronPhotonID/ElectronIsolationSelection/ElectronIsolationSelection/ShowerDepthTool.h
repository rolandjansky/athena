/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONISOLATIONSELECTION_SHOWERDEPTHTOOL_H
#define ELECTRONISOLATIONSELECTION_SHOWERDEPTHTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "ElectronIsolationSelection/IShowerDepthTool.h"

#include <TString.h>
#include <string>
#include <cmath>
#include <stdlib.h>

class TH1;

namespace CP {

  class ShowerDepthTool : public virtual IShowerDepthTool,
                          public asg::AsgTool {
                            
                            
      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( ShowerDepthTool, CP::IShowerDepthTool )                         
  
    public :
       /// Constructor for standalone usage
      ShowerDepthTool( const std::string& name );     
      
      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();      
      
      /** Shower depth (in mm) on EM1 vs. eta, considering misalignments **/
      float getCorrectedShowerDepthEM1(const float& etas1,const float& phi,const bool& isData = true) const;

      /** Shower depth (in mm) on EM2 vs. eta, considering misalignments **/
      float getCorrectedShowerDepthEM2(const float& etas2,const float& phi,const bool& isData = true) const;

      /** Return the shower depth in R,Z considering misalignments **/
      std::pair<float, float> getCorrectedRZ(const float& eta,const float& phi,const bool& isData = true,const int& sampling = 1) const;
      
       /** Return the calorimeter displacement in R(Z) for barrel (endcap) **/
      float getRZCorrection(const float& eta,const float& phi,const bool& isData = true) const;     
      
      /** Eta direction from zvertex to the shower in the given sampling **/
      float getCorrectedEtaDirection(const float& zvertex,const float& eta,const float& phi,const bool& isData=true,const int& sampling = 1) const;
      
      /** Eta direction from samplings 1 and 2 (pointing) **/
      float getCaloPointingEta(const float& etas1,const float& etas2,const float& phi,const bool& isData=true) const;

      /** Shower depth (in mm) vs. eta on EM1 **/
      float getShowerDepthEM1(const float& etas1) const;
      
      /** Shower depth (in mm) vs. eta on EM2 **/
      float getShowerDepthEM2(const float& etas2) const;

      /** Shower depth in R,Z for the given sampling **/
      std::pair<float, float> getRZ(const float& eta,const int& sampling) const;

      float getEtaDirection(const float& zvertex,const float& R,const float& z) const;
      
      
    protected:
      TString findInputFile_ExpectBetterSolutionFromASG();
      
      /** Return TH1* from file given fileName, histoName **/
      TH1* getHistoFromFile(const TString& fileName,const TString& histoName);      
      
    private:
      TH1 *m_hData;
      TH1 *m_hMC;    
      std::string m_inputFile;
      TString m_dataHistoName;
      TString m_mcHistoName;
  };

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_SHOWERDEPTHTOOL_H
