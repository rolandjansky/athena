/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "CaloIsolationTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  CaloIsolationTool::CaloIsolationTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_toCalo("TrackInCaloTools/extrapolMuonInCaloTool")
  {
    declareInterface<ICaloIsolationTool>(this);
    declareProperty("ExtrapolTrackToCaloTool", m_toCalo);
  }

  CaloIsolationTool::~CaloIsolationTool()
  {
  
  }

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode CaloIsolationTool::initialize() {
    ATH_CHECK(m_toCalo.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode CaloIsolationTool::finalize() {
    return StatusCode::SUCCESS;
  }


  bool CaloIsolationTool::caloIsolation( CaloIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                                         CaloIsolationTool::SubtractionStrategy strategy ) {
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(&tp);
    if( trkp ) return caloIsolation(result,*trkp,cones,strategy);
    ATH_MSG_WARNING("Isolation only supported for TrackParticles");

    return true;
  }

  bool CaloIsolationTool::caloIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
                                         CaloIsolationTool::SubtractionStrategy strategy ) {

    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    result.etcones.clear();

    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());

    if( theFlavour == Iso::etcone ) return etConeIsolation( result, tp, isoTypes, strategy );

    ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << static_cast<int>(theFlavour));
    
    return false;
  }

  bool CaloIsolationTool::etConeIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
                                           CaloIsolationTool::SubtractionStrategy strategy ) {

    // for now always use track, should move to extrapolation to calo entrance 
    const Trk::Track* track = *tp.trackLink();
    if( !track  ) {
      ATH_MSG_WARNING("Failed to access track");
      return false;
    }    
    bool onlyEM = false;
    std::vector<double> conesf;
    for( auto isoType : isoTypes ){
      Iso::IsolationFlavour flavour = Iso::isolationFlavour(isoType);
      if( flavour != Iso::etcone && flavour != Iso::etconeNoEm ) {
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << static_cast<int>(isoType));
        return false;
      }
      if( flavour == Iso::etconeNoEm ) onlyEM = true;
      float coneSize = Iso::coneSize(isoType);
      conesf.push_back(coneSize);
    }
    // sort in decending order
    std::sort(conesf.begin(),conesf.end(),[](double i, double j) { return i>j; });
    std::vector<double> energies;

    // should decode onlyEM from IsolationType
    CaloLayer lastLayer = onlyEM ? em3 : hec3;

    // get energies in cone for track
    m_toCalo->getMeasuredEnergy(track, conesf, energies, ps, lastLayer);

    if( energies.size() != conesf.size() ) {
      ATH_MSG_WARNING("Inconsistent number of energies return by: " << m_toCalo << " requested " << conesf.size() << " but got " << energies.size() );
      return false;
    }

    // calculate etcore 
    double ecore = strategy == Ecore ? m_toCalo->getEcore(track,ps,lastLayer) : 0.;
    result.ecore = ecore;

    // update result
    for( unsigned int i=0;i<conesf.size();++i ) result.etcones.push_back(energies[i]-ecore);

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << "Calculated calo isolation: ";
      if( strategy == Ecore ) msg(MSG::DEBUG) << "ecore " << ecore;
      for( unsigned int i = 0; i< result.etcones.size();++i ){
        msg(MSG::DEBUG) << " coneSize " << std::setw(3) << conesf[i] << " value " << result.etcones[i] << " energy " << energies[i];
      }
      msg(MSG::DEBUG) << endreq;
    }
    
    return true;
  }

  
  

}	// end of namespace
