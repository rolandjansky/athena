/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for Z-window RoI from random position (excluding the HS).
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedTool_xk_RandomRoISeedTool_h
#define SiSpacePointsSeedTool_xk_RandomRoISeedTool_h

#include "InDetRecToolInterfaces/IZWindowRoISeedTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <vector>

class TRandom3;

namespace CLHEP {
  class RandGauss;
}

namespace InDet {

  class RandomRoISeedTool : 
   virtual public IZWindowRoISeedTool, public AthAlgTool
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
    
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    RandomRoISeedTool(const std::string&,const std::string&,const IInterface*);
    virtual ~RandomRoISeedTool();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    /** Compute RoI */
    virtual std::vector<ZWindow> getRoIs();

  private:
    TRandom3 *m_random;

  protected:

    /**    @name Disallow default instantiation, copy, assignment **/
    RandomRoISeedTool() = delete;
    RandomRoISeedTool(const RandomRoISeedTool&) = delete;
    RandomRoISeedTool &operator=(const RandomRoISeedTool&) = delete;

    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
  
    float m_z0Window; ///< width of z0 window

    ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
    StringProperty m_randomStreamName;
    CLHEP::RandGauss* m_chooseRandGauss;

    
  }; // RandomRoISeedTool
} //InDet namespace

#endif // SiSpacePointsSeedMaker_RandomRoISeedTool

