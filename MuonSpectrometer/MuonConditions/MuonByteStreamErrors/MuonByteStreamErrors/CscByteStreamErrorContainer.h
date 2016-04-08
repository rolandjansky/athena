/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonByteStreamErrors package
 CscByteStreamErrorContainer class
 -----------------------------------------
***************************************************************************/


#ifndef MUONBYTESTREAMERRORS_CSCBYTESTREAMERRORCONTAINER_H
#define MUONBYTESTREAMERRORS_CSCBYTESTREAMERRORCONTAINER_H


#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


namespace Muon {
  enum CscBSerrorTypes {    
    //temporary implementation of errors enumeration
    CSC_CSMHeaderErrors,
    CSC_CSMSubHeaderErrors,
    Csc_NUM_ERROR_TYPES  // always have this one last, so we can use it as a loop index
  };

  class CscByteStreamErrorContainer 
  {
  public: 
    CscByteStreamErrorContainer();
    CscByteStreamErrorContainer( const DataVector<std::pair<IdentifierHash, int> > & errorVector );
    ~CscByteStreamErrorContainer();

    void addError(IdentifierHash, int); 
    const DataVector<std::pair<IdentifierHash, int> >& getAllErrors() const;
  private:
    DataVector<std::pair<IdentifierHash, int> > m_errorVector; 
  };

}

CLASS_DEF( Muon::CscByteStreamErrorContainer , 1307307367  , 1 )
#endif // MUONBYTESTREAMERRORS_CSCBYTESTREAMERRORCONTAINER_H

