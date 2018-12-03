/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4ParticlePropertyTool.h, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLSG4_G4PARTICLEPROPERTYTOOL_H
#define ISF_FATRASTOOLSG4_G4PARTICLEPROPERTYTOOL_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Fatras
#include "ISF_FatrasInterfaces/IParticlePropertyTool.h"

namespace iFatras
{

  class PDGToG4Particle;
  
  /** 
      @class G4ParticlePropertyTool
      
      This tool retrieves basic particle properties from PartPropSvc /
      HepPDT
      
      @author Joerg.Mechnich@cern.ch
  */
  
  class G4ParticlePropertyTool : public extends<AthAlgTool, IParticlePropertyTool>
  {
  public:
    /**Constructor */
    G4ParticlePropertyTool( const std::string&,
                            const std::string&,
                            const IInterface*);

    /**Destructor*/
    ~G4ParticlePropertyTool();

    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();
    
    /** get list of particles */
    std::vector<std::pair<int,std::string> >
    listOfParticles() const;
    
    /** retrieve basic information of the particle */
    StatusCode
    basicInfo( int pdgCode, int* charge=0, double* mass=0) const;

  private:
    /*---------------------------------------------------------------------
     *  Private members
     *---------------------------------------------------------------------*/

    /*---------------------------------------------------------------------
     *  ToolHandles
     *---------------------------------------------------------------------*/
    ToolHandle<PDGToG4Particle> m_pdgToG4Conv;
  };
}
#endif // FATRASINTERFACES_PARTICLEPROPERTYTOOL_H
