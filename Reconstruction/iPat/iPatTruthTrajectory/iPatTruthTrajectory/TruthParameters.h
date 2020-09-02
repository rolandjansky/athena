/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to obtain Perigee or Track Parameters corresponding to a simulated 
 particle, either from its barcode or from a HepMC GenParticle.
 ***************************************************************************/

#ifndef IPATTRUTHTRAJECTORY_TRUTHPARAMETERS_H
#define IPATTRUTHTRAJECTORY_TRUTHPARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "iPatInterfaces/ITruthParameters.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITruthSelector;
namespace Trk  { class IIntersector; }

class TruthParameters: public AthAlgTool,
		       virtual public ITruthParameters
{

public:
    TruthParameters		(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~TruthParameters		(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    const PerigeeParameters*	perigeeParameters(int barcode, const Amg::Vector3D& vertex);
    const PerigeeParameters*	perigeeParameters(const HepMC::GenParticle& particle,
						  const Amg::Vector3D& vertex);
    const TrackParameters*	trackParameters(int barcode);
    const TrackParameters*	trackParameters(const HepMC::GenParticle& particle);
  
private:
    const HepMC::GenParticle*	findParticle(int barcode);
    void			trackFromParticle(const HepMC::GenParticle& particle);


    // configuration: tools etc
    ToolHandle<Trk::IIntersector>		m_intersector;
    const HepPDT::ParticleDataTable* 		m_particleDataTable;
    ToolHandle<ITruthSelector>			m_truthSelector;

    // state
    PerigeeParameters		       		m_perigeeParameters;
    TrackParameters		       		m_trackParameters;
    
    // internal communication
    const Trk::TrackSurfaceIntersection*	m_intersection;
    double					m_qOverP;

};

#endif // IPATTRUTHTRAJECTORY_TRUTHPARAMETERS_H

