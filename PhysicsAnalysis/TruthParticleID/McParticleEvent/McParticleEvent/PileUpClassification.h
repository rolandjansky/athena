///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// \class PileUpClassification
/// 
/// Utilities to retrieve GenEvents according to their pile-up type from 
/// the McEventCollection.
/// The pile-up type is defined in 
///  https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PileupDigitization#Arrangement_of_Truth_Information
/// All utilities provided here rely on the above organisation.
///
/// \author P.A. Delsart
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_PileUpClassification_H 
#define MCPARTICLEEVENT_PileUpClassification_H 

// GeneratorObjects includes
#include "GeneratorObjects/McEventCollection.h"

class PileUpClassification {
public:

  /// define some Pile-up classification
  /// Important : this classification is copied in McParticleAlgs/python/JobOptCfg.py
  ///    report any modification here to this jobOption !
  typedef enum PileuType_t { SIGNAL = 0,
                             ALL,
                             INTIME, OUTOFTIME, RESTOFMB, CAVERN, ALLMINBIAS, UNKONW = 9999  } PileuType_t;
  
  


  /// Find interval [fEvt,lEvt] containing all GenEvents of type putype from the McEventCollection.
  /// IMPORTANT : fEvt and lEvt are expected to be first and last iterators on the McEventCollection !
  static void findEventIterators(PileuType_t putype, McEventCollection::const_iterator &fEvt, McEventCollection::const_iterator &lEvt );

  /// Find interval [firstIndex,lastIndex] containing all GenEvents of type putype from the McEventCollection.
  static void findEventIterators(PileuType_t putype, const McEventCollection* evts, size_t &firstIndex, size_t &lastIndex );

  /// Return the type of pile-up event of the GenEvent number evtIndex in the McEventCollection
  static PileuType_t pileUpType(const McEventCollection* evts, size_t evtIndex);

  /// Fill evtToPUType so that entry i is the pile-up type of GenEvent number i inside the McEventCollection
  static void  fillPileUpType(const McEventCollection* evts, std::vector<size_t> &evtToPUType);

protected:

  
};


#endif
