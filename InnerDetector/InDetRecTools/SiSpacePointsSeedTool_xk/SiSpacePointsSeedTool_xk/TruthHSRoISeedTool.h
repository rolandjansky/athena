/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for Z-window RoI from truth HS position.
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedTool_xk_TruthHSRoISeedTool_h
#define SiSpacePointsSeedTool_xk_TruthHSRoISeedTool_h

#include "InDetRecToolInterfaces/IZWindowRoISeedTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>

namespace InDet {

  class TruthHSRoISeedTool : 
   virtual public IZWindowRoISeedTool, public AthAlgTool
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    TruthHSRoISeedTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TruthHSRoISeedTool();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    /** Compute RoI */
    virtual std::vector<ZWindow> getRoIs();

  protected:

    /**    @name Disallow default instantiation, copy, assignment **/
    TruthHSRoISeedTool() = delete;
    TruthHSRoISeedTool(const TruthHSRoISeedTool&) = delete;
    TruthHSRoISeedTool &operator=(const TruthHSRoISeedTool&) = delete;

    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
  
    //* Input track collection *//
    std::string m_inputTruthEvents; ///< input truth events collection name
    float m_z0Window; ///< width of z0 window
    
  }; // TruthHSRoISeedTool
} //InDet namespace

#endif // SiSpacePointsSeedMaker_TruthHSRoISeedTool

