/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H //To avoid re-definition
#define FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H

// EDM includes
#include "xAODJet/Jet.h"

// Other includes
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"


/// Interface class for smearing the jet energy resolution
///
/// @author Alberto Prades <alberto.prades.ibanez@cern.ch>
///



class IFFJetSmearingTool : public virtual CP::ISystematicsTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IFFJetSmearingTool )

  public:

    /// Virtual destructor
    virtual ~IFFJetSmearingTool(){};

    //Now I think that here we should deffine the functions of FFJetSmearingTool that will be used by the user. In principle I am just going to put the main one

      virtual CP::CorrectionCode applyCorrection(xAOD::Jet* jet_reco) = 0;
	//(A virtual function is a function which is declared within a base class and is re-defined (overrriden) by a derived class)

//------------------------------------------------------------ functions from other packagesi. Useless, just for guidance -----------------------------------------
    /// Apply the JER correction on a modifiable jet
   // virtual CP::CorrectionCode applyCorrection( xAOD::Jet& jet ) = 0;               //This functions will be re-definite in the FFJETSMEARINGTOOL (no en la interfaz)

    /// Create a JER-corrected copy from a const jet
  //  virtual CP::CorrectionCode correctedCopy( const xAOD::Jet& input,
  //                                            xAOD::Jet*& output ) = 0;
//--------------------------------------------------------------------------------------------------------------------------------------------


}; // class IFFJetSmearingTool


#endif /* FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H  */
