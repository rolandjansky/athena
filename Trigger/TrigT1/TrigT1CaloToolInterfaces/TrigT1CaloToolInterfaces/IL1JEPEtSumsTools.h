/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JEPEtSumsTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JEPETSUMSTOOLS_H
#define ILVL1L1JEPETSUMSTOOLS_H

#include "GaudiKernel/IAlgTool.h"


namespace LVL1 
{
  //using namespace xAOD;

class CMMEtSums;
class JEMEtSums;
class JetElement;

/**
Interface definition for L1JEPEtSumsTools
*/
  static const InterfaceID IID_IL1JEPEtSumsTools("LVL1::IL1JEPEtSumsTools", 1, 0);

  class IL1JEPEtSumsTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formJEMEtSums(const DataVector<xAOD::JetElement>*  jetElementVec,
                               DataVector<JEMEtSums>* jemEtSumsVec) const = 0;
    virtual void formCMMEtSums(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMMEtSums>* cmmEtSumsVec) const = 0;
    virtual void formCMMEtSumsModule(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMMEtSums>* cmmEtSumsMod) const = 0;
    virtual void formCMMEtSumsCrate(const DataVector<CMMEtSums>* cmmEtSumsMod,
                               DataVector<CMMEtSums>* cmmEtSumsCrate) const = 0;
    virtual void formCMMEtSumsSystem(
                               const DataVector<CMMEtSums>* cmmEtSumsCrate,
                               DataVector<CMMEtSums>* cmmEtSumsSys) const = 0;
    virtual void formCMMEtSumsEtMaps(const DataVector<CMMEtSums>* cmmEtSumsSys,
                               DataVector<CMMEtSums>* cmmEtSumsMaps) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1JEPEtSumsTools::interfaceID()
    { 
      return IID_IL1JEPEtSumsTools;
    }

} // end of namespace

#endif 
