/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to obtain barcode(s) for charged tracks produced or
 reconstructable inside the Inner Detector acceptance.
 ***************************************************************************/

#ifndef TRUTHTOOLS_TRUTHSELECTOR_H
#define TRUTHTOOLS_TRUTHSELECTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepMC/GenParticle.h"
#include "GenInterfaces/ITruthSelector.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TruthSelector: public AthAlgTool,
		     virtual public ITruthSelector
{

public:
    TruthSelector		(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~TruthSelector		(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    // map to provide user code-independence between old G3 and G4 kine/barcode labelling schemes
    const std::map<int,int>&	indetKineMap(void);

    // select muons with production vertex inside InDet envelope
    // developement needed to provide equivalent electron functionality
    const std::vector<int>&	indetMuons(double minPt);

    // access McEventCollection (auto-switch between G3, DC2 and current naming)
    const McEventCollection*	mcEventCollection(void);

    // jobOptions control the primary/secondary definitions below
    // vector of barcodes for primary and close-in secondary tracks that should be reconstructed
    const std::vector<int>&	reconstructablePrimaries(double minPt);
    // vector of barcodes for secondary tracks that should be reconstructed
    const std::vector<int>&	reconstructableSecondaries(double minPt);

private:
    bool			selectParticle (const HepMC::GenParticle& particle, double minPt);
    
    std::vector<int>			m_barcodes;
    std::map<int,int>			m_indetKineMap;
    double				m_maxEta;
    double				m_maxRIndet;
    double				m_maxRStartPrimary;
    double				m_maxRStartSecondary;
    double			       	m_maxZIndet;
    double				m_maxZStartPrimary;
    double				m_maxZStartSecondary;
    std::string				m_mcEventsName;	// name of McEventCollection (only for G4)
    double				m_minPt;
    double				m_minREndPrimary;
    double				m_minREndSecondary;
    double				m_minZEndPrimary;
    double				m_minZEndSecondary;
    const HepPDT::ParticleDataTable*	m_particleDataTable;

};
   
#endif // TRUTHTOOLS_TRUTHSELECTOR_H

