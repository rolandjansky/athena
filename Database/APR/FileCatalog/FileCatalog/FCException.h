/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_FCEXCEPTION_H
#define POOL_FCEXCEPTION_H
#include "POOLCore/Exception.h"
namespace pool{
  /** @class FCduplicatePFNException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCduplicatePFNException : public pool::Exception{
  public:
    ///constructor
    FCduplicatePFNException(const std::string& from);
    ///virtual destructor
    virtual ~FCduplicatePFNException() throw(){}
  }; 
  /** @class FCduplicateLFNException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCduplicateLFNException : public pool::Exception{
  public:
    ///constructor
    FCduplicateLFNException(const std::string& from);
    ///virtual destructor
    virtual ~FCduplicateLFNException() throw(){}
  }; 
  /** @class FCnonexistentFileException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCnonexistentFileException : public pool::Exception{
  public:
    ///constructor
    FCnonexistentFileException(const std::string& from, const std::string& fileID);
    ///virtual destructor
    virtual ~FCnonexistentFileException() throw(){}
  };
  /** @class FCconnectionException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCconnectionException: public pool::Exception{
  public:
    ///constructor
    FCconnectionException(const std::string& from , const std::string& backendmessage);
    ///virtual destructor
    virtual ~FCconnectionException() throw(){}
  };
  /** @class FCbackendException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCbackendException : public pool::Exception{
  public:
    ///constructor
    FCbackendException(const std::string& from, const std::string& backendmessage);
    ///virtual destructor
    virtual ~FCbackendException() throw(){}
  };
  /** @class FCillegalContactStringException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCillegalContactStringException: public pool::Exception{
  public:
    ///constructor
    FCillegalContactStringException(const std::string& from, const std::string& message);
    ///virtual destructor
    virtual ~FCillegalContactStringException() throw(){}
  };
  /** @class FCTransactionException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCTransactionException: public pool::Exception{
  public:
    ///constructor
    FCTransactionException(const std::string& from, const std::string& message);
    ///virtual destructor
    virtual ~FCTransactionException() throw(){}
  };
  /** @class FCinconsistentSchemaException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCinconsistentSchemaException: public pool::Exception{
  public:
    ///constructor
    FCinconsistentSchemaException(const std::string& from);
    ///virtual destructor
    virtual ~FCinconsistentSchemaException() throw(){}
  };

  /** @class FCstringLimitException FCException.h FileCatalog/FCException.h
      @author Maria Girone
  */
  class FCstringLimitException: public pool::Exception{
  public:
    ///constructor
    FCstringLimitException(const std::string& from, const std::string& str);
    ///virtual destructor
    virtual ~FCstringLimitException() throw(){}
  };

  /** @class FCduplicatemetadataspecException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCduplicatemetadataspecException: public pool::Exception{
  public:
    ///constructor
    FCduplicatemetadataspecException(const std::string& from);
    ///virtual destructor
    virtual ~FCduplicatemetadataspecException() throw(){}
  };

  /** @class FCduplicatemetadataException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCduplicatemetadataException: public pool::Exception{
  public:
    ///constructor
    FCduplicatemetadataException(const std::string& from);
    ///virtual destructor
    virtual ~FCduplicatemetadataException() throw(){}
  };
  /** @class FCnonexistentMetaDataException FCException.h FileCatalog/FCException.h
      @author Zhen Xie
  */
  class FCnonexistentMetaDataException: public pool::Exception{
  public:
    ///constructor
    FCnonexistentMetaDataException(const std::string& from);
    ///virtual destructor
    virtual ~FCnonexistentMetaDataException() throw(){}
  };
}//ns pool
#endif
