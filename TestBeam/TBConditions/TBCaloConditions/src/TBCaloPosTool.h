/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBCALOCONDITIONS_TBCALOPOSTOOL
# define TBCALOCONDITIONS_TBCALOPOSTOOL

#include "TBCaloConditions/ITBCaloPosTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "StoreGate/DataHandle.h"
#include "CondDBObjects/GenericDbTableClassDef.h"

class StoreGateSvc; 

/** 
 ** Class TBCaloPosTool
 ** 
 ** Implementation of ITBCaloPosTool 

   Sept 28, 2004   Hong Ma 
	
 **/
 
class TBCaloPosTool : public AthAlgTool,
      virtual public ITBCaloPosTool, public IIncidentListener
{
public:

    TBCaloPosTool(const std::string& type, const std::string& name,
                const IInterface* parent);   
  
    virtual ~TBCaloPosTool() ;


    virtual StatusCode initialize() ; 

    virtual StatusCode finalize() ; 

    // IIncidentListner interface
    virtual void handle(const Incident&) ;

    ///  access eta value 
    virtual double eta ()     ;

    ///  access eta value 
    virtual double  theta ()  ;

    ///  access eta value 
    virtual double z ()       ;

    ///  access eta value 
    virtual double delta ()       ;

private: 

    bool initHandles() ; 

    const DataHandle<GenericDbTable> m_etaTable; 
    const DataHandle<GenericDbTable> m_thetaTable; 
    const DataHandle<GenericDbTable> m_zTable; 
    const DataHandle<GenericDbTable> m_deltaTable; 
    
    bool m_init;	
};



#endif
