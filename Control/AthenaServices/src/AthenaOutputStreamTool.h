/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaOutputStreamTool.h 
 * 
 * @brief Tool to stream out objects
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: AthenaOutputStreamTool.h,v 1.5 2008-09-23 22:00:47 binet Exp $
 */

#ifndef ATHENASERVICES_ATHENAOUTPUTSTREAMTOOL_H
#define ATHENASERVICES_ATHENAOUTPUTSTREAMTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IAthenaOutputStreamTool.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"

// AthenaBaseComps
#include "AthenaBaseComps/AthAlgTool.h"


//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class StoreGateSvc;
class IConversionSvc;
class IClassIDSvc;
namespace SG {
  class IFolder;
}

/** 
 ** @class AthenaOutputStreamTool
 ** 
 ** @brief This is a tool that allows streaming out of
 ** DataObjects. This has been factorized out from AthenaOutputStream
 ** to allow output streaming to occur from anywhere, for example
 ** from the finalized method of an algorithm.
 **/

class AthenaOutputStreamTool : public AthAlgTool,
			       virtual public IAthenaOutputStreamTool
{

public:    

    /// Constructor
    AthenaOutputStreamTool(const std::string& type, 
			   const std::string& name, 
			   const IInterface* parent); 
    /// Destructor
    virtual ~AthenaOutputStreamTool();  
 
    /// Initialize 
    virtual StatusCode initialize();

    /// Finalize 
    virtual StatusCode finalize();

    /// Specify which data store and conversion service to use
    /// and whether to extend provenence 
    ///   Only use if one wants to override jobOptions
    virtual StatusCode connectServices(const std::string& dataStore, 
				       const std::string& cnvSvc,
				       bool extendProvenenceRecord);

    /// Connect to the output stream
    ///   Must connectOutput BEFORE streaming
    ///   Only specify "outputName" if one wants to override jobOptions
    virtual StatusCode connectOutput(const std::string& outputName);
    
    /// Commit the output stream after having streamed out objects
    ///   Must commitOutput AFTER streaming
    virtual StatusCode commitOutput();

    /// Finalize the output stream after the last commit, e.g. in
    /// finalize
    virtual StatusCode finalizeOutput();

    /// Stream out objects. Provide vector of typeName/key pairs. 
    ///   If key is empty, assumes only one object and this 
    ///   will fail if there is more than one
    typedef std::pair<std::string, std::string>   TypeKeyPair;
    typedef std::vector< TypeKeyPair >            TypeKeyPairs;
    virtual StatusCode streamObjects(const TypeKeyPairs& typeKeys);

    /// Stream out a vector of objects
    ///   Must convert to DataObject, e.g.    
    ///   \#include "SGTools/StorableConversions.h"
    ///     T* obj = xxx;
    ///     DataObject* dataObject = SG::asStorable(obj);
    typedef std::vector< DataObject* >            DataObjectVec;
    virtual StatusCode streamObjects (const DataObjectVec& dataObjects);

    /// Fill refs of an object - done as second iteration over
    /// objects, after streamObject
    virtual StatusCode fillObjectRefs(const DataObjectVec& dataObjects);

    virtual StatusCode getInputItemList(SG::IFolder* m_p2BWrittenFromTool);
private:

    /// Do the real connection to services
    virtual StatusCode connectServices();

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to the data store
    StoreGateSvc_t     m_store;

    typedef ServiceHandle<IConversionSvc> ICnvSvc_t;
    /// Keep reference to the data conversion service
    ICnvSvc_t    m_cnvSvc;

    typedef ServiceHandle<IClassIDSvc> IClassIDSvc_t;
    /// Ref to ClassIDSvc to convert type name to clid
    IClassIDSvc_t       m_clidSvc;   

    /// Name of the output db name
    std::string        m_outputName;

    /// Flag to tell whether connectOutput has been called
    bool               m_connectionOpen;

    /// Flag as to whether to extend provenance via the DataHeader
    bool               m_extendProvenanceRecord;
    
    /// tag of processing stage:
    std::string        m_processTag;
};

#endif // ATHENASERVICES_ATHENAOUTPUTSTREAMTOOL_H
