/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_ActiveFractionSvc_H
#define TRT_ActiveFractionSvc_H

/*----------------------------------------------------------
 *  @file TRT_ActiveFractionSvc.h
 *  @Service to provide active straw fraction in an eta-phi bin
 *  @author Denver Whittington <Denver.Whittington@cern.ch>
 *  @author Sasa Fratina <Sasa.Fratina@cern.ch>
 *///-------------------------------------------------------

// Header Includes
#include "InDetRecToolInterfaces/ITRT_ActiveFractionSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class ITRT_StrawStatusSummarySvc;
class IIncidentSvc;

/// Service to provide active straw fraction in an eta-phi bin
class TRT_ActiveFractionSvc : public AthService,
  virtual public ITRT_ActiveFractionSvc,
  virtual public IIncidentListener
{

 public:

  TRT_ActiveFractionSvc( const std::string&, ISvcLocator* );
  virtual ~TRT_ActiveFractionSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID&, void** );

  /// Returns the fraction of active straws in an eta-phi bin
  double getActiveFraction( const Trk::TrackParameters* );

  /// Handle IncidentSvc callbacks
  void handle( const Incident& );

 private:

  int findEtaBin( double );
  int findPhiBin( double );

  /// Incident service
  ServiceHandle<IIncidentSvc> m_incSvc;

  /// Dead/noisy straw service
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_deadStrawSvc;

  int m_nBinsPhi;
  std::vector<std::pair<double,double> > m_etaBins;
  std::vector<std::pair<double,double> > m_phiBins;
  std::vector<std::vector<double> > m_activeFracTable; // [etaBin,phiBin]

};

//////
/// Query Interface
inline StatusCode TRT_ActiveFractionSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( ITRT_ActiveFractionSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_ActiveFractionSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvIf );
  }
  return StatusCode::SUCCESS;
}

//////
/// Get Active Fraction
inline double TRT_ActiveFractionSvc::getActiveFraction( const Trk::TrackParameters* trkPar ) {

  double eta = trkPar->momentum().eta();
  double phi = trkPar->momentum().phi();

  int etaBin = findEtaBin(eta);
  if ( etaBin < 0 ) return 1.;
  int phiBin = findPhiBin(phi);
  if ( phiBin < 0 ) return 1.;

  // Get the fraction
  return m_activeFracTable[etaBin][phiBin];
}

//////
/// Find eta bin number
inline int TRT_ActiveFractionSvc::findEtaBin(double eta) {
  // Get the eta bin number
  int etaBin = 0;
  for ( ; etaBin < (int)m_etaBins.size(); ++etaBin ) {
    std::pair<double,double> theBin = m_etaBins.at(etaBin);
    if ( eta > theBin.first && eta <= theBin.second ) break;
  }
  if ( etaBin == (int)m_etaBins.size() ) {
    ATH_MSG_DEBUG( "No eta bin defined contiaining this eta." );
    return -1;
  }
  return etaBin;
}

//////
/// Find phi bin number
inline int TRT_ActiveFractionSvc::findPhiBin(double phi) {
  // Get the phi bin number
  int phiBin = 0;
  for ( ; phiBin < (int)m_phiBins.size(); ++phiBin ) {
    std::pair<double,double> theBin = m_phiBins.at(phiBin);
    if ( phi > theBin.first && phi <= theBin.second ) break;
  }
  if ( phiBin == (int)m_phiBins.size() ) {
    ATH_MSG_DEBUG( "No phi bin defined containing this eta." );
    return -1;
  }
  return phiBin;
}

#endif // TRT_ActiveFractionSvc_H
