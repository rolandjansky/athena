/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_PUNCHTHROUGHTOOLS_SRC_PUNCHTHROUGHTOOL_H
#define ISF_PUNCHTHROUGHTOOLS_SRC_PUNCHTHROUGHTOOL_H 1

#include "ISF_FastCaloSimInterfaces/IPunchThroughTool.h"
#include <string>

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"

#include "BarcodeEvent/Barcode.h"
#include "BarcodeEvent/PhysicsProcessCode.h"
#include "GeoPrimitives/GeoPrimitives.h"

/*-------------------------------------------------------------------------
 *  Forward declarations
 *-------------------------------------------------------------------------*/

class IPartPropSvc;
class IAtRndmGenSvc;
class TFile;
class IEnvelopeDefSvc;

namespace CLHEP {
  class HepRandomEngine;
}
	
namespace HepPDT {
  class ParticleDataTable;
}
	
namespace HepMC {
  class GenEvent;
}

namespace Barcode {
  class IBarcodeSvc;
}
	
namespace ISF {

  class PunchThroughParticle;
  class PDFcreator;
  class IGeoIDSvc;
}

namespace ISF {
	 
  class PunchThroughTool : public extends<AthAlgTool, IPunchThroughTool>
  {
    public:
    /** Constructor */
    PunchThroughTool(const std::string&,const std::string&,const IInterface*);
	   
    /** Destructor */
    virtual ~PunchThroughTool ();
	   
    /** AlgTool initialize method */
    virtual StatusCode initialize();
    /** AlgTool finalize method */
    virtual StatusCode finalize  ();
    /** interface function: fill a vector with the punch-through particles */
    const ISF::ISFParticleContainer* computePunchThroughParticles(const ISF::ISFParticle &isfp) const;
	
      private:
   /*---------------------------------------------------------------------
    *  Private member functions
    *---------------------------------------------------------------------*/
    /** registers a type of punch-through particles which will be simulated */
        StatusCode registerParticle(int pdgID, bool doAntiparticle = false, 
                                double minEnergy = 0., int maxNumParticles = -1,
                                double numParticlesFactor = 1., double energyFactor = 1.,
                                double posAngleFactor = 1.,
                                double momAngleFactor = 1.);
    /** register a correlation for the two given types of punch-through particles
	with a given energy threshold above which we will have full correlation */
	StatusCode registerCorrelation(int pdgID1, int pdgID2,double minCorrEnergy = 0., double fullCorrEnergy = 0.);
	
    /** reads out the lookuptable for the given type of particle */
	PDFcreator *readLookuptablePDF(int pdgID, std::string folderName);
	
    /** create the right number of punch-through particles for the given pdg
	     *  and return the number of particles which was created. also create these
    *  particles with the right distributions (energy, theta, phi).
	     *  if a second argument is given, create exactly this number of particles
	     *  (also with the right energy,theta,phi distributions */
	int getAllParticles(int pdg, int numParticles = -1) const;
	
    /** get the right number of particles for the given pdg while considering
     *  the correlation to an other particle type, which has already created
     *  'corrParticles' number of particles */
	int getCorrelatedParticles(int doPdg, int corrParticles ) const;
	
    /** create exactly one punch-through particle with the given pdg and the given max energy */
	ISF::ISFParticle *getOneParticle(int pdg, double maxEnergy) const;
	
    /** create a ISF Particle state at the MS entrace containing a particle with the given properties */
	ISF::ISFParticle *createExitPs(int PDGcode, double energy, double theta, double phi, double momTheta, double momPhi) const;

    /** get the floating point number in a string, after the given pattern */
        double getFloatAfterPatternInStr(const char *str, const char *pattern);
    /** get particle through the calorimeter */
        Amg::Vector3D propagator(double theta, double phi) const;

    /*---------------------------------------------------------------------
     *  Private members
     *---------------------------------------------------------------------*/
	   	
    /** initial particle properties */
	mutable const ISF::ISFParticle*	     m_initPs;         //!< the incoming particle
	mutable double                       m_initEnergy;     //!< the incoming particle's energy
    	mutable double                       m_initEta;        //!< the incoming particle's eta
	mutable double                       m_initTheta;      //!< the incoming particle's theta
	mutable double                       m_initPhi;        //!< the incoming particle's phi

    /** calo-MS borders */
	double 				     R1, R2, z1, z2;

    /** the returned vector of ISFParticles */
	mutable ISF::ISFParticleContainer  *m_isfpCont;

    /** parent event */
	mutable HepMC::GenEvent*            m_parentGenEvt;    //!< all newly created particles/vertices will have this common parent

    /** ParticleDataTable needed to get connection pdg_code <-> charge */
	const HepPDT::ParticleDataTable*    m_particleDataTable;
	
    /** random generator service */
	ServiceHandle<IAtRndmGenSvc>        m_randomSvc;                //!< Random Svc
	std::string                         m_randomEngineName;         //!< Name of the random number stream
	CLHEP::HepRandomEngine*             m_randomEngine;             //!< Random Engine 
	
    /** ROOT objects */
	TFile*                              m_fileLookupTable;          //!< the punch-through lookup table file
    /** general information of the punch-through particles which will be created */
	mutable std::map<int, bool>         m_doAntiParticleMap;        /*!< stores information, if anti-particles are
	                                                                         created for any given PDG id */
    /** needed to create punch-through particles with the right distributions */
	mutable std::map<int, PunchThroughParticle*> m_particles;       //!< store all punch-through information for each particle id
	
    /** barcode steering */

	mutable Barcode::PhysicsProcessCode m_processCode;
	mutable Barcode::ParticleBarcode    m_primBC;
	mutable Barcode::ParticleBarcode    m_secBC;

	/*---------------------------------------------------------------------
	     *  Properties
	 *---------------------------------------------------------------------*/
	
    /** Properties */
	std::string                          m_filenameLookupTable;     //!< holds the filename of the lookup table (property)
	std::vector<int>                     m_pdgInitiators;           //!< vector of punch-through initiator pgds
	std::vector<int>                     m_punchThroughParticles;   //!< vector of pdgs of the particles produced in punch-throughs
	std::vector<bool>                    m_doAntiParticles;         //!< vector of bools to determine if anti-particles are created for each punch-through particle type
    	std::vector<int>                     m_correlatedParticle;      //!< holds the pdg of the correlated particle for each given pdg
    	std::vector<double>                  m_minCorrEnergy;           //!< holds the energy threshold below which no particle correlation is computed
    	std::vector<double>                  m_fullCorrEnergy;          //!< holds the energy threshold above which a particle correlation is fully developed
    	std::vector<double>                  m_posAngleFactor;          //!< tuning parameter to scale the position deflection angles
    	std::vector<double>                  m_momAngleFactor;          //!< tuning parameter to scale the momentum deflection angles
    	std::vector<double>                  m_minEnergy;               //!< punch-through particles minimum energies
    	std::vector<int>                     m_maxNumParticles;         //!< maximum number of punch-through particles for each particle type
    	std::vector<double>                  m_numParticlesFactor;      //!< scale the number of punch-through particles
    	std::vector<double>                  m_energyFactor;            //!< scale the energy of the punch-through particles
	
    /*---------------------------------------------------------------------
     *  ServiceHandles
     *---------------------------------------------------------------------*/
	ServiceHandle<IPartPropSvc>          m_particlePropSvc;         //!< particle properties svc
	ServiceHandle<IGeoIDSvc>	     m_geoIDSvc;
	ServiceHandle<Barcode::IBarcodeSvc>  m_barcodeSvc;
        ServiceHandle<IEnvelopeDefSvc>       m_envDefSvc;

    /** beam pipe radius */
	double				    m_beamPipe;
  }; 
}
	
#endif
