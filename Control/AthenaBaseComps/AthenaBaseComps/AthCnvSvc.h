///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthCnvSvc.h 
// Header file for class AthCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHCNVSVC_H
#define ATHENABASECOMPS_ATHCNVSVC_H 1

// STL includes
#include <string>
#include <unordered_map>
#include <mutex>

// GaudiKernel
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IAddressCreator.h"

// AthenaBaseComps
#include "AthenaBaseComps/AthService.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;


/** @class AthCnvSvc AthCnvSvc.h AthenaBaseComps/AthCnvSvc.h

    Base class for all conversion services. It manages a set of Converters that
    are specialized for conversions of specific objects.

    This supports self learning converters:
    User hooks include the following (overridable) entries:
    - findCnvFactory:      returns a suitable converter factory
    - createConverter:     the actual converter creation
    - configureConverter:  configure converter before initialize
    - initializeConverter: initializes the converter
    - activateConverter:   any additional configuration to be done after 
                           initialize.

    configureConverter and activateConverter are user hooks, where
    the converter can be manipulated by the hosting service
    and knowledge can be supplied, which a "generic" converter cannot 
    acquire itself.

    These hooks allow any sub-classed conversion service to
    override the calls and create converters, which acquire
    the knowledge about their persistency type and the
    object type they convert during the initialization.

    Only AFTER these three steps the converter must satisfy the
    storage type of the hosting service and the class type of
    the required object type.

    @author Markus Frank
    @version 1.0
*/

class AthCnvSvc
  : virtual public ::IConversionSvc,
    virtual public ::IAddressCreator,
            public ::AthService
{ 
  friend class SvcFactory<AthCnvSvc>;

public:
  class WorkerEntry {
    friend class AthCnvSvc;
  private:
    CLID        m_class;
    IConverter* m_converter;
  public:
    WorkerEntry(const CLID& cl, IConverter* cnv) 
      : m_class(cl), m_converter(cnv)       {
    }
    WorkerEntry(const WorkerEntry& copy)
      : m_class(copy.m_class), m_converter(copy.m_converter)    {
    }
    WorkerEntry& operator = (const WorkerEntry& copy)   {
      m_class     = copy.m_class;
      m_converter = copy.m_converter;
      return *this;
    }
    virtual ~WorkerEntry()  {
    }
    IConverter*     converter()  {
      return m_converter;
    }
    const CLID&     clID()  const {
      return m_class;
    }
    // comparison operators
    bool operator == (const WorkerEntry& rhs)   
    {
      return rhs.clID() == clID();
    }
    bool operator < (const WorkerEntry& rhs)
    {
      return this->clID() < rhs.clID();
    }
  };
  typedef std::unordered_map<CLID,WorkerEntry> Workers;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthCnvSvc( const std::string& name, ISvcLocator* pSvcLocator, long type );

  // Assignment operator: 
  //AthCnvSvc &operator=(const AthCnvSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface (const InterfaceID& riid,
                                     void** ppvInterface);
  //@}

  ///@name @c IConversionSvc implementation
  //@{
  /// Retrieve the class type of the data store the converter uses.
  virtual long repSvcType() const;

  /// Implementation of IConverter: dummy call
  const CLID& objType() const;

  /** Implementation of IConverter: Set Data provider service
      @return    Status code indicating success or failure
      @param     pService   Pointer to data provider service
  */
  virtual 
  StatusCode setDataProvider(IDataProviderSvc* pService);

  /** Implementation of IConverter: Get Data provider service
      @return    Pointer to data provider service
  */
  virtual SmartIF<IDataProviderSvc>& dataProvider()  const;

  /// Implementation of IConverter: Set conversion service the converter is connected to
  virtual
  StatusCode setConversionSvc(IConversionSvc* svc);

  /// Implementation of IConverter: Get conversion service the converter is connected to
  virtual 
  SmartIF<IConversionSvc>& conversionSvc()    const;

  /// Set address creator facility
  virtual
  StatusCode setAddressCreator(IAddressCreator* creator);

  /// Retrieve address creator facility
  virtual
  SmartIF<IAddressCreator>& addressCreator()   const;
  
  /// Implementation of IConverter: Create the transient representation of an object.
  virtual
  StatusCode createObj(IOpaqueAddress* pAddress,DataObject*& refpObject);

  /// Implementation of IConverter: Resolve the references of the created transient object.
  virtual
  StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Update the transient object from the other representation.
  virtual
  StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);

  /// Implementation of IConverter: Update the references of an updated transient object.
  virtual
  StatusCode updateObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Convert the transient object to the requested representation.
  virtual
  StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);

  /// Implementation of IConverter: Resolve the references of the converted object. 
  virtual
  StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);

  /// Implementation of IConverter: Update the converted representation of a transient object.
  virtual
  StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Update the references of an already converted object.
  virtual
  StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Add converter object to conversion service.
  virtual
  StatusCode addConverter(const CLID& clid);

  /// Add converter object to conversion service.
  virtual
  StatusCode addConverter(IConverter* pConverter);

  /// Remove converter object from conversion service (if present).
  virtual
  StatusCode removeConverter(const CLID& clid);

  /// Retrieve converter from list
  virtual
  IConverter* converter(const CLID& wanted);

  /// Connect the output file to the service with open mode.
  virtual
  StatusCode connectOutput(const std::string& outputFile,
                                   const std::string& openMode);

  /// Connect the output file to the service.
  virtual
  StatusCode connectOutput(const std::string& output);

  /// Commit pending output.
  virtual
  StatusCode commitOutput(const std::string& output, bool do_commit);

  //@}

  /// Disconnect output files from the service.
  virtual
  StatusCode disconnectOutput();

  /// @name @c IAddressCreator interface implementation
  //@{
  /// Create a Generic address using explicit arguments to identify a single object.
  virtual
  StatusCode createAddress (long svc_type,
                            const CLID& clid,
                            const std::string* par, 
                            const unsigned long* ip,
                            IOpaqueAddress*& refpAddress);

  /// Convert an address to string form
  virtual 
  StatusCode convertAddress (const IOpaqueAddress* pAddress,
                             std::string& refAddress);

  /// Convert an address in string form to object form
  virtual
  StatusCode createAddress (long svc_type, 
                            const CLID& clid,
                            const std::string& refAddress,
                            IOpaqueAddress*& refpAddress);

  /// Update state of the service
  virtual
  StatusCode updateServiceState(IOpaqueAddress* pAddress);

  //@}

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 
  /// Default constructor: 
  AthCnvSvc(); //< not implemented
  AthCnvSvc (const AthCnvSvc&); //< not implemented
  AthCnvSvc& operator= (const AthCnvSvc&); //< not implemented

  /////////////////////////////////////////////////////////////////// 
  // Protected data and methods: 
  /////////////////////////////////////////////////////////////////// 
protected:

  /// Destructor: 
  virtual ~AthCnvSvc(); 

  /// Create new Converter using factory
  virtual 
  IConverter* createConverter (long typ, const CLID& clid,
                               const ICnvFactory* fac);

  /// Configure the new converter before initialize is called
  virtual 
  StatusCode configureConverter (long typ, const CLID& clid, IConverter* cnv);

  /// Initialize the new converter
  virtual 
  StatusCode initializeConverter (long typ, const CLID& clid, IConverter* cnv);

  /// Activate the new converter after initialization
  virtual 
  StatusCode activateConverter (long typ, const CLID& clid, IConverter* cnv);
  
  /// Load converter or dictionary needed by the converter
  virtual 
  void loadConverter(DataObject* pObject);

  /// Retrieve address creation interface
  virtual
  SmartIF<IAddressCreator>& addressCreator()
  {
    return m_addressCreator;
  }

  StatusCode makeCall (int typ,
                       bool ignore_add,
                       bool ignore_obj,
                       bool update,
                       IOpaqueAddress*& pAddress, 
                       DataObject*& pObject);

  /// Pointer to data provider service
  SmartIF<IDataProviderSvc>   m_dataSvc;
  /// Pointer to the address creation service interface
  SmartIF<IAddressCreator>    m_addressCreator;
  /// Pointer to the IConversionSvc interface of this
  SmartIF<IConversionSvc>     m_cnvSvc;
  /// Conversion service type
  long                m_type;
  /// List of conversion workers
  Workers             m_workers;
  /// Mutex to protect Converter creation
  typedef std::recursive_mutex CallMutex;
  CallMutex           m_conv_mut;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> !ATHENABASECOMPS_ATHCNVSVC_H
