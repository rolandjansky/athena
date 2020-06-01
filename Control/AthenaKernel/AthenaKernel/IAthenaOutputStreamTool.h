/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENAOUTPUTSTREAMTOOL_H
# define ATHENAKERNEL_IATHENAOUTPUTSTREAMTOOL_H
/**
 * @file IAthenaOutputStreamTool.h 
 * 
 * @brief Interface to an output stream tool 
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: IAthenaOutputStreamTool.h,v 1.4 2007-06-23 01:12:06 calaf Exp $
 */


// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <string>
#include <vector>


class DataObject;
namespace SG {
  class IFolder;
}

/** 
 ** @class IAthenaOutputStreamTool
 ** 
 ** @brief This is a tool that allows streaming out of
 ** DataObjects. This has been factorized out from AthenaOutputStream
 ** to allow output streaming to occur from anywhere, for example
 ** from the finalized method of an algorithm.
 **
 ** How to use the IAthenaOutputStreamTool: 
 **
 **   An initial setup of services should be done either via the
 **   interface with connectServices or via jobOptions. Note that the
 **   defaults are set to Conditions data writing, since event data
 **   writing is normally done with AthenaOutputStream.:
 **
 **   To write to a POOL/ROOT file one simply needs:
 **
 **   ToolSvc = Service( "ToolSvc" )
 **   ToolSvc.CondStream1.OutputFile = "SimplePoolFile.root"
 **
 **   where one retrieves the AthenaOutputStreamTool with name
 **   CondStream1.
 **   The default job opts are:
 **
 **   ConversionSvc = "AthenaPoolCnvSvc"
 **   StoreName     = "DetectorStore"
 **
 **   After initialization of services, one writes out with a sequence
 **   of:
 **
 **   connectOutput()
 **   streamObjects(<list of objects>)
 **   commitOutput()
 **
 **   where <list of objects> may be a vector of type/key pairs, or a
 **   vector of DataObjects.
 **
 **/

class IAthenaOutputStreamTool : virtual public IAlgTool
{

public:    
  
    /// Specify which data store and conversion service to use 
    /// and whether to extend provenence 
    ///   Only use if one wants to override jobOptions
    virtual StatusCode connectServices(const std::string& dataStore, 
				       const std::string& cnvSvc,
				       bool extendProvenenceRecord = false) = 0;
    
    /// Connect to the output stream
    ///   Must connectOutput BEFORE streaming
    ///   Only specify "outputName" if one wants to override jobOptions
    virtual StatusCode connectOutput(const std::string& outputName = "") = 0;
    
    /// Commit the output stream after having streamed out objects
    ///   Must commitOutput AFTER streaming
    virtual StatusCode commitOutput(bool doCommit = false) = 0;

    /// Finalize the output stream after the last commit, e.g. in
    /// finalize
    virtual StatusCode finalizeOutput() = 0;


    /// Stream out objects. Provide vector of typeName/key pairs. 
    ///   If key is empty, assumes only one object and this 
    ///   will fail if there is more than one
    typedef std::pair<std::string, std::string>   TypeKeyPair;
    typedef std::vector< TypeKeyPair >            TypeKeyPairs;
    virtual StatusCode streamObjects(const TypeKeyPairs& typeKeys, const std::string& outputName = "") = 0;

    /// Stream out a vector of objects
    ///   Must convert to DataObject, e.g.    
    ///   #include "AthenaKernel/StorableConversions.h"
    ///     T* obj = xxx;
    ///     DataObject* dataObject = SG::asStorable(obj);
    typedef std::vector< DataObject* >            DataObjectVec;
    virtual StatusCode streamObjects(const DataObjectVec& dataObjects, const std::string& outputName = "") = 0;

    /// Get ItemList from the OutputStreamTool (e.g. all input objects)
    virtual StatusCode getInputItemList(SG::IFolder* m_p2BWrittenFromTool) = 0;

    /// Gaudi boilerplate
    static const InterfaceID& interfaceID();
};

inline
const InterfaceID& 
IAthenaOutputStreamTool::interfaceID() {
  static const InterfaceID IID("IAthenaOutputStreamTool", 1, 0);
  return IID;
}

#endif // ATHENAKERNEL_IATHENAOUTPUTSTREAMTOOL_H
