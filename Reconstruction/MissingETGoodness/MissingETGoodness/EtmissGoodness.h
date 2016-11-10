/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : EtmissGoodness                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef MISSINGETGOODNESS_ETMISSGOODNESS_H
#define MISSINGETGOODNESS_ETMISSGOODNESS_H

#include "TObject.h"
#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/Goodies.h"

namespace MET {

  class EtmissGoodness : public TObject {
     
  public:

    EtmissGoodness();      
    virtual ~EtmissGoodness();

    EtmissGoodness(const MET::EtmissGoodness& other) ;
    EtmissGoodness& operator=(const EtmissGoodness& other) ;

    Bool_t   IsGood( const TString& qualifier, const Quality& quality = Medium, Bool_t doLookupQualifier = kTRUE ) const;
    Bool_t   IsGood( const Quality& quality = Medium, const TString& qualifier = "All", Bool_t doLookupQualifier = kTRUE ) const;
    Long64_t GetBitMask( const Quality& quality = Medium ) const;

    // retrieve kinematics
    double etx()    const;
    double ety()    const;
    double sumet()  const;
    double et()     const;
    double phi()    const;

    void Summary()  const;

  protected:

    EtmissGoodnessManager& m_manager;
    Goodies& m_goodies;

    double m_dummyd;

    ClassDef(EtmissGoodness,0)
  };
}

#endif // MISSINGETGOODNESS_ETMISSGOODNESS_H

