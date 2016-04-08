/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonByteStreamErrors package
 TgcByteStreamErrorContainer class
 -----------------------------------------
***************************************************************************/


#ifndef MUONBYTESTREAMERRORS_TGCBYTESTREAMERRORCONTAINER_H
#define MUONBYTESTREAMERRORS_TGCBYTESTREAMERRORCONTAINER_H


#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


namespace Muon {
  //temporary implementation of errors enumeration
  enum TgcBSerrorTypes {    
    TGC_CSMHeaderErrors,
    TCG_CSMSubHeaderErrors,
    Tgc_NUM_ERROR_TYPES  // always have this one last, so we can use it as a loop index
  };

  class TgcByteStreamErrorContainer 
  {
  public: 
    TgcByteStreamErrorContainer();
    TgcByteStreamErrorContainer( const DataVector<std::pair<IdentifierHash, int> > & errorVector );
    ~TgcByteStreamErrorContainer();

    void addError(IdentifierHash, int); 
    const DataVector<std::pair<IdentifierHash, int> >& getAllErrors() const;
  private:
    DataVector<std::pair<IdentifierHash, int> > m_errorVector; 
  };

}

CLASS_DEF( Muon::TgcByteStreamErrorContainer ,1299911504 , 1 )
#endif // MUONBYTESTREAMERRORS_TGCBYTESTREAMERRORCONTAINER_H

