/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ChargeCollProbSvc_H
#define ChargeCollProbSvc_H
 
#include "IChargeCollProbSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
 
#include "AthenaBaseComps/AthService.h"
 
#include <vector>
#include <map>
 
class ISvcLocator;
class IToolSvc;
class ITagInfoMgr;
 
template <class TYPE> class SvcFactory;
 
class ChargeCollProbSvc : public AthService, virtual public IChargeCollProbSvc {
 
public:
 
    virtual StatusCode initialize();
    virtual StatusCode finalize();
 
    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
 
 
    virtual StatusCode readProbMap( std::string );
    virtual StatusCode printProbMap( std::string );
    virtual double getProbMapEntry( std::string, int, int);
 
    //MsgStream GetMsgStream() const {return log;}
 
 
 
    friend class SvcFactory<ChargeCollProbSvc>;
 
    // Standard Constructor
    ChargeCollProbSvc(const std::string& name, ISvcLocator* svc);
 
    // Standard Destructor
    virtual ~ChargeCollProbSvc();
       
    //MsgStream log;
       
private:
 
    std::string m_cc_prob_file_fei3;
    std::string m_cc_prob_file_fei4;
    std::multimap< std::pair< int , int >, double >  m_probMapFEI4;
    std::multimap< std::pair< int , int >, double >  m_probMapFEI3;
       
};
 
#endif
