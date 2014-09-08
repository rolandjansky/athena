/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ASGFUDGEMCTOOL_H
#define _ASGFUDGEMCTOOL_H

/**
   @class AsgFudgeMCTool
   @brief Tool to fudge photon MC shower shapes.

   @author Rob Roy Fletcher (University of Pennsylvania) rob.fletcher@cern.ch
   @date   August 2014


*/

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "ElectronPhotonSelectorTools/FudgeMCTool.h"
#include "ElectronPhotonSelectorTools/IAsgFudgeMCTool.h"

#include "xAODEgamma/Photon.h"

class AsgFudgeMCTool : virtual public asg::AsgTool, virtual public  IAsgFudgeMCTool
{
   /// Declare the interface that the class provides
   ASG_TOOL_CLASS(AsgFudgeMCTool, IAsgFudgeMCTool)

public: 
  /** Standard constructor */
  AsgFudgeMCTool( const std::string myname);


  /** Standard destructor */
  virtual ~AsgFudgeMCTool();

public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

public:
   /// Apply the correction on a modifyable object
   const CP::CorrectionCode applyCorrection( xAOD::Photon* gamma, int isConv, int preselection ) const;
   //
   const CP::CorrectionCode applyCorrection( xAOD::IParticle* part, int isConv, int preselection ) const;
   //
   /// Create a corrected copy from a constant muon
//   virtual CP::CorrectionCode correctedCopy( const xAOD::Photon* input,
  //                                       xAOD::Photon* output ) = 0;

private:
   
   FudgeMCTool* m_rootTool;

}; // class AsgFudgeMCTool

// Inline methods
inline const CP::CorrectionCode AsgFudgeMCTool::applyCorrection( xAOD::IParticle* part, int isConv, int preselection) const
{
  xAOD::Photon* gamma = dynamic_cast<xAOD::Photon*>(part);
  if (gamma)
    {
      return applyCorrection(gamma, isConv, preselection);
    }
  else
    {
      ATH_MSG_ERROR ( " Could not cast to photon" );
      return CP::CorrectionCode::Error; 
    }
}



#endif // _IFUDGEMCTOOL_H
