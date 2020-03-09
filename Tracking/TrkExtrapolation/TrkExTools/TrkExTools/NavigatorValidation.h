/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Navigator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_NAVIGATORVALIDATION_H
#define TRKEXTOOLS_NAVIGATORVALIDATION_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "TrkExInterfaces/INavigator.h"
#include "TrkExTools/Navigator.h"

//For marking not thread safety                                                                                         
#include "CxxUtils/checker_macros.h" 
#define TRKEXTOOLS_MAXNAVSTEPS 100
class TTree;
 /** 
     @class NavigatorValidation
     It implements the validation methods of the standard
     Navigator adding Validation output.
     Currently no thread safe, perhaps a better solution
     should be used for the navigator section.
     Split away to allow thread safe use of the main tool
 */
namespace Trk{

  class  ATLAS_NOT_THREAD_SAFE  NavigatorValidation : public Navigator,
                    virtual public INavigator {
    public:
      /** Constructor */
      NavigatorValidation(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~NavigatorValidation();
      /** Validation Action:
        Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const override final;
                                                            
    private:
   
      virtual void validationInitialize() override final;
      /* no-op here */
      virtual void validationFill(const Trk::TrackParameters* trackPar) const override final;
    
      //------VALIDATION MODE SECTION ----------------------------------//
      std::string                               m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                               m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                               m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out                                       
      TTree*                                    m_validationTree;            //!< Root Validation Tree                                             
      mutable int                               m_boundariesCounter;         //!< counter for boundary surfaces hit                                                   
      mutable int                               m_boundaries;                           //!< associated Ntuple variable
      mutable float                             m_boundaryHitX[TRKEXTOOLS_MAXNAVSTEPS]; //!< x Position of interseciton with BoundarySurface
      mutable float                             m_boundaryHitY[TRKEXTOOLS_MAXNAVSTEPS]; //!< y Position of interseciton with BoundarySurface
      mutable float                             m_boundaryHitR[TRKEXTOOLS_MAXNAVSTEPS]; //!< Radius of interseciton with BoundarySurface
      mutable float                             m_boundaryHitZ[TRKEXTOOLS_MAXNAVSTEPS]; //!< z Position of interseciton with BoundarySurface
     };

} // end of namespace


#endif // TRKEXTOOLS_NAVIGATORVALIDATION_H

