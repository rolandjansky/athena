/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_FILTERTOOL_H
#define D2PDMAKER_FILTERTOOL_H

//============================================================================
// Name: FilterTool.h
//
/**
   @class FilterTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Define all needed fitler methods here

*/
//=============================================================================


// STL includes
#include <string>
#include <vector>
#include <cfloat>

// DataModel
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"

// Athena Tool
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// particles includes
#include "CLHEP/Vector/LorentzVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/IParticle.h"


// System of Units
#include "CLHEP/Units/SystemOfUnits.h"





static const InterfaceID IID_FilterTool("FilterTool", 1, 0);


class FilterTool : public AthAlgTool
{


public:

  /** Default constructor */
  FilterTool( const std::string& type, 
              const std::string& name,
              const IInterface* parent );

  /** Default destructor */
  virtual ~FilterTool() ;

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_FilterTool; };

  
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();


public:

  /** Filter simple double numbers */
  bool doubleFilter( double num, double min=-DBL_MAX, double max=DBL_MAX );
  

  /** Filter on deltaEta */
  bool absDeltaEtaFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2, 
                          double deltaEtaMin=-DBL_MAX, double deltaEtaMax=DBL_MAX );

  /** Filter on deltaEta */
  bool absDeltaEtaFilter( const CLHEP::HepLorentzVector& part1, const CLHEP::HepLorentzVector& part2, 
                          double deltaEtaMin=-DBL_MAX, double deltaEtaMax=DBL_MAX );
  
  /** Filter on deltaPhi */
  bool deltaPhiFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2, 
                       double deltaPhiMin=-DBL_MAX, double deltaPhiMax=DBL_MAX );

  /** Filter on deltaPhi */
  bool deltaPhiFilter( const IParticle* part1, const IParticle* part2, 
                       double deltaPhiMin=-DBL_MAX, double deltaPhiMax=DBL_MAX );

  /** Filter on deltaPhi */
  bool deltaPhiFilter( const CLHEP::HepLorentzVector& part1, const CLHEP::HepLorentzVector& part2, 
                       double deltaPhiMin=-DBL_MAX, double deltaPhiMax=DBL_MAX );
  
  /** Filter on deltaR */
  bool deltaRFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2, 
                     double deltaRMin=-DBL_MAX, double deltaRMax=DBL_MAX );

  /** Filter on deltaR */
  bool deltaRFilter( const CLHEP::HepLorentzVector& part1, const CLHEP::HepLorentzVector& part2, 
                     double deltaRMin=-DBL_MAX, double deltaRMax=DBL_MAX );
  
  /** Charge filter */
  bool chargeFilter( const IParticle* part, int minCharge, int maxCharge,
                     bool allowChargeConj );
  

private:



}; // End class FilterTool


#endif
