/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetTrackTruthMatching.h
//   Header file for class IJetTrackTruthMatching
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Andreas.Wildauer@cern.ch
///////////////////////////////////////////////////////////////////


#ifndef PARTICLEJETTOOLS_JETTRACKTRUTHMATCHING_H
#define PARTICLEJETTOOLS_JETTRACKTRUTHMATCHING_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "ParticleJetTools/IJetTruthMatching.h"

#include <vector>
#include <string>

class StoreGateSvc;
class TrackParticleTruthCollection;
class McEventCollection;
class Jet;

namespace Rec
{
  class TrackParticleContainer;
}

namespace Analysis {

    /** \class JetTrackTruthMatching
      
       @author Andreas.Wildauer@cern.ch
      */

  
  class JetTrackTruthMatching : public AlgTool,
                       virtual public IJetTruthMatching {
     public:
       JetTrackTruthMatching(const std::string&,const std::string&,const IInterface*);
       virtual ~JetTrackTruthMatching();
       
       /** AlgTool initailize method */
       StatusCode initialize();
       /** AlgTool finalize method */
       StatusCode finalize();
         
       /** Method to truth tag a jet */
       virtual bool matchJet(const Jet& myJet);
       
       /** print parameter settings of the truth match tool */
       virtual void m_printParameterSettings();

       /** Return the predefined name to label the jets passing the matching: */
       inline const int& jetLabel() const { return m_jetLabel; }                       

	private:
      StoreGateSvc* m_storeGate; //!< Pointer to StoreGate
      
      /** The names of different containers to be read or written */
      std::string m_trackParticlesName;        //!< name of TrackParticleContainer
      std::string m_trackParticleTruthMapName; //!< Name of truth map
      std::string m_mcEventCollection;         //!< name of the McEventCollection to use (GEN_EVENT or TruthEvent)
      
      bool m_jetContainsBMeson(const Jet& myJet, 
              const Rec::TrackParticleContainer* origTrackPC,
              const TrackParticleTruthCollection* tpTruthColl,
              const McEventCollection* myMcEventCollection );

      int m_jetLabel;
	
//	 IToolSvc*                   m_toolsvc;                          //!<  Pointer to tool service
  };
} // end of namespace

#endif // TRUTHMATCHTOOLS_JETTRACKTRUTHMATCHING_H

