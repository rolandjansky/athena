/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBCALOCONDITIONS_TBCALOCOOLPOSTOOL
# define TBCALOCONDITIONS_TBCALOCOOLPOSTOOL

#include "TBCaloConditions/ITBCaloPosTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "StoreGate/DataHandle.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

class StoreGateSvc; 

/** 
 ** Class TBCaloCoolPosTool
 ** 
 ** Implementation of ITBCaloPosTool using COOL

   Feb 6, 2006   Richard Hawkings
	
 **/
 
class TBCaloCoolPosTool : public AthAlgTool,
      virtual public ITBCaloPosTool, public IIncidentListener
{
public:

    TBCaloCoolPosTool(const std::string& type, const std::string& name,
                const IInterface* parent);   
  
    virtual ~TBCaloCoolPosTool() ;


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

    const DataHandle<AthenaAttributeList> m_etaTable; 
    const DataHandle<AthenaAttributeList> m_thetaTable; 
    const DataHandle<AthenaAttributeList> m_zTable; 
    const DataHandle<AthenaAttributeList> m_deltaTable; 
    
    bool m_init;	
};



#endif
