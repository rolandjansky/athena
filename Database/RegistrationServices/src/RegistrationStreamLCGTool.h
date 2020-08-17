/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_REGSTREAMLCGTOOL_H
#define REGISTRATIONSVC_REGSTREAMLCGTOOL_H

/**
 * @file RegistrationStreamLCGTool.h
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: RegistrationStreamLCGTool.h,v 1.11 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IRegistrationStreamTool.h"
#include "PoolSvc/IPoolSvc.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IAddressCreator;
class StoreGateSvc;
namespace pool
{
    class ICollection;
}
namespace coral
{
    class Attribute;
    class AttributeListSpecification;
}
class AthenaAttributeList;

/**
 * @class RegistrationStreamLCGTool
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 */

class RegistrationStreamLCGTool : public AthAlgTool, virtual public IRegistrationStreamTool
{
public:

  /// Standard RegistrationStreamLCGTool Constructor
  RegistrationStreamLCGTool(const std::string& type, const std::string& name, const IInterface* parent);

  /// Initialize RegistrationStreamLCGTool
  virtual StatusCode initialize();

  /// Terminate RegistrationStreamLCGTool
  virtual StatusCode finalize();

  /// Working entry point
  /// NOTE that the Tools assume this list will be ordered.
  ///  begin = newest; end = oldest
  ///  duplicate elimination is left to the Tools
   /// REQUIRED INTERFACE METHOD
  virtual StatusCode fill(std::vector< std::pair<std::string, std::string> >& refs,
                          std::string key);

   /// explicit commit function rather than finalize or execute
   /// REQUIRED INTERFACE METHOD
   virtual StatusCode commit();

   /// Type of Attribute list being used
   /// REQUIRED INTERFACE METHOD
   const CLID& listID();

   /// Control collection metadata storegate keys
   /// REQUIRED INTERFACE METHOD
   void setCollMetadataKeys(const std::vector<std::string>& keys);

  /// Standard Destructor
   virtual ~RegistrationStreamLCGTool();

protected:

   /// Dump values from AttributeList for DEBUG
   virtual const CLID& attListID();
   virtual StatusCode fillAtt(std::vector< std::pair<std::string, std::string> >& refs,
                              std::string key);

   void dumpLCGList(const AthenaAttributeList* attributes) const;

   template <class ATTLIST>
   void dumpAttList(const ATTLIST* attributes) const;

   void overrideProperties();

   void addMetadata();

   StatusCode initCollection();

   /// Check type and dump value of Attribute for DEBUG
   void dumpAttribute(const coral::Attribute& attribute) const;

   /// Dump names from AttributeListSpec for DEBUG
   void dumpDesc(int fragid=0) const;

   /// name of store:
   StringProperty           m_storeName;

   /// type of the collection
   StringProperty           m_collectionType;
   /// DB connection
   StringProperty           m_connection;
   /// Name of the output collection
   StringProperty           m_outputCollection;
   /// Collection open mode
   StringProperty           m_openMode;
   /// Primary key attributes
   ///   Default behavior is that if an att in the list does not exist, 
   ///   the setting is ignored and warning issued
   //StringArrayProperty      m_primKeyAtts;
   std::vector<std::string>  m_primKeyAtts;

   /// Override tool properties with ones from stream parent
   BooleanProperty          m_slave;

   /// Pointer to StoreGateSvc
   ServiceHandle<StoreGateSvc>    m_metadataStore;
   /// Access to the PoolSvc
   ServiceHandle<IPoolSvc>        m_poolSvc;

   /// The output collection
   pool::ICollection*       m_collection;

   /// Event count
   int                      m_events;

   /// Metadata fields from properties
   //unsigned int m_passnum;
   //std::string m_passname, m_stream, m_config, m_project;

   std::vector<std::string> m_cmdKeys;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

template < class ATTLIST >
inline
void RegistrationStreamLCGTool::dumpAttList(const ATTLIST* attributes) const
{
    typename ATTLIST::const_iterator first = attributes->begin();
    typename ATTLIST::const_iterator last  = attributes->end();
    for (; first != last; ++first) {
        this->dumpAttribute( (*first) );
    }
}

#endif // REGISTRATIONSVC_REGSTREAMLCGTOOL_H
