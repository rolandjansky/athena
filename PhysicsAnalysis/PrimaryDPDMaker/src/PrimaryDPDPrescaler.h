/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRIMARYDPDPRESCALER_H
#define PRIMARYDPDPRESCALER_H

/** 
 *  @brief This file contains the class definition for the PrimaryDPDPrescaler class.
 *  @author Marcello Barisonzi <marcello.barisonzi@desy.de>
 *  $Id: PrimaryDPDPrescaler.h 519325 2012-09-28 07:47:46Z kkoeneke $
 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

// Forward declarations
class TRandom3;
//class IAtRndmGenSvc;
//namespace CLHEP { class HepRandomEngine; }


class PrimaryDPDPrescaler : public AthFilterAlgorithm, virtual public IIncidentListener {
public: // Constructor and Destructor
  /// Standard Service Constructor
  PrimaryDPDPrescaler(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrimaryDPDPrescaler();
  
public:
  /// Gaudi Service Interface method implementations:
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  /// Incident service handle listening for BeginInputFile and EndInputFile.
  void handle(const Incident& incident);


  // Private methods
private:

  StatusCode decodeAlgorithms(StringArrayProperty *theAlgNames, std::vector<Gaudi::Algorithm*> *theAlgs);
  bool       isEventAccepted();

  void ResetSelfDescription();


  // Private data members
private: 
  /// handle to the @c IAtRndmGenSvc we want to test
  //ServiceHandle<IAtRndmGenSvc> m_rndmSvc;

  /// The stream name of the random number service
  //StringProperty m_streamName;

  /// The random nunumber engine
  //CLHEP::HepRandomEngine* m_pEng;
  TRandom3* m_tRand;


  int m_evtcount;
  int m_accepted;
  int m_vetoed;
  int m_processed;
  int m_filtered;
  int m_rollover;

  unsigned int m_nameHash;

  /// The prescale value 
  double m_prescale;
  
  /// List of algorithms to combine with logical OR
  std::vector<Gaudi::Algorithm*>  m_theAcceptAlgs;

  /// List of algorithms to combine with logical AND
  std::vector<Gaudi::Algorithm*>  m_theRequireAlgs;
  

  /// List of algorithms names to combine with logical OR
  StringArrayProperty      m_theAcceptAlgNames;

  /// List of algorithms names to combine with logical AND
  StringArrayProperty      m_theRequireAlgNames;
  

};


#endif
