///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAsgSelectionTool.h 
// Header file for class IAsgSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASGANALYSISINTERFACES_IASGMTSELECTIONTOOL_H
#define ASGANALYSISINTERFACES_IASGMTSELECTIONTOOL_H

// STL includes


// FrameWork includes
#include <AsgTools/IAsgTool.h>

namespace asg {
  class AcceptInfo;
  class AcceptData;
}

// Forward declaration
namespace xAOD{
  class IParticle;
}

namespace CP {

  class ISelectionTool
    : virtual public asg::IAsgTool
  { 
    /// Declare the interface ID for this pure-virtual interface class to the Athena framework
    ASG_TOOL_INTERFACE(ISelectionTool)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /** Method to get the plain AcceptInfo.
        This is needed so that one can already get the AcceptInfo 
        and query what cuts are defined before the first object 
        is passed to the tool. */
    virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

    /** Backward compatibility method */
    [[deprecated("please use getAcceptInfo() instead")]]
    inline const asg::AcceptInfo& getTAccept () const {
      return getAcceptInfo();};

    /** The main accept method: the actual cuts are applied here */
    virtual asg::AcceptData accept( const xAOD::IParticle* /*part*/ ) const = 0;


  }; 
}

#endif //> !PATCORE_IASGSELECTIONTOOL_H
