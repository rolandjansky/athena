/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  HistogramDefinitionSvc.h
//
//  Created by sroe on 07/07/2015.
//

#ifndef HistogramDefinitionSvc_h
#define HistogramDefinitionSvc_h

#include "InDetPhysValMonitoring/IHistogramDefinitionSvc.h"
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <map>
#include "AthenaBaseComps/AthService.h"
#include <memory>
template <class TYPE> class SvcFactory;

class IReadHistoDef;
class ISvcLocator;
class StatusCode;
class InterfaceID;


class HistogramDefinitionSvc :virtual public IHistogramDefinitionSvc, public AthService{
friend class SvcFactory<HistogramDefinitionSvc>;
public:
    HistogramDefinitionSvc(const std::string &name, ISvcLocator * svc);
    virtual ~HistogramDefinitionSvc();
    //@name Service methods, reimplemented
    //@{
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    //interfaceID() implementation is in the baseclass
    virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
    //@}
    SingleHistogramDefinition definition(const std::string &name, const std::string & dirName="") const final;
    std::string histoType(const std::string &name, const std::string & dirName="") const final;
    std::string title(const std::string &name, const std::string &dirName="") const final;
    unsigned int nBinsX(const std::string &name, const std::string &dirName="") const final;
    unsigned int nBinsY(const std::string &name, const std::string &dirName="") const final;
    unsigned int nBinsZ(const std::string &name, const std::string &dirName="") const final;
    IHistogramDefinitionSvc::axesLimits_t xLimits(const std::string &name, const std::string &dirName="") const final;
    IHistogramDefinitionSvc::axesLimits_t yLimits(const std::string &name, const std::string &dirName="") const final;
    IHistogramDefinitionSvc::axesLimits_t zLimits(const std::string &name, const std::string &dirName="") const final;
    std::string xTitle(const std::string &name, const std::string &dirName="") const final;
    std::string yTitle(const std::string &name, const std::string &dirName="") const final;
    std::string zTitle(const std::string &name, const std::string &dirName="") const final;
    
private:
    StringProperty m_source;
    StringProperty m_formatString;
    IHistogramDefinitionSvc::Formats m_format;
    std::map<std::string, SingleHistogramDefinition> m_histoDefMap;
    std::unique_ptr<IReadHistoDef> m_reader;
    bool sourceExists();
    bool formatOk();
    
};
#endif /* defined(HistogramDefinitionSvc_h) */
