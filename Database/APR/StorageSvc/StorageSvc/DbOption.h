/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbOption.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbOption class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBOPTION_H
#define POOL_DBOPTION_H 1

// Framework include files
#include "StorageSvc/DbColumn.h"

/*
 *  POOL namespace declaration
 */
namespace pool  {

  /** @class DbOption DbOption.h StorageSvc/DbOption.h
    *
    * Description:
    * Definition an option to be supplied to database objects.
    *
    * Note:
    * For any pointer argument, values are not copied. The values
    * must outlive the lifetime of the DbOption.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbOption    {
  public:
    /** @union Value DbOption.h StorageSvc/DbOption.h
      *
      * Description:
      * Definition an option value.
      *
      * @author  M.Frank
      * @version 1.0
      */
    union Value  {
      long long int     val_long;
      int               val_int;
      double            val_double;
      void*             val_pvoid;
      char*             val_pchar;
    };
    /// Buffer holding option value
    Value             m_value;
    /// Option data type
    DbColumn::Type    m_type;
    /// Option name identifier
    std::string       m_name;
    /// Optional identifier
    std::string       m_opt;

    /// Set the option value
    DbStatus i_setValue(const std::type_info& typ, void* value);
    /// Set the option value
    DbStatus i_getValue(const std::type_info& typ, void* value) const;

  public:
    /// Initializing constructor 
    template <class T> DbOption(const std::string& nam, 
                                const std::string& opt, 
                                T value)
    : m_type(DbColumn::UNKNOWN), m_name(nam), m_opt(opt)
    { i_setValue(typeid(T), &value);                        }
    /// Initializing constructor with type definition
    DbOption(const std::string& nam, const std::string& opt="");
    /// Default copy constructor
    /// Standard destructor
    virtual ~DbOption()                 {                   }
    /// Access to column name
    const std::string& option() const   { return m_opt;     }
    /// Access to column name
    const std::string& name() const     { return m_name;    }
    /// Integer type identifier
    DbColumn::Type type() const         { return m_type;    }
    /// Set the option value
    template<class T> DbStatus _setValue(T value)
    { return i_setValue(typeid(T), &value);                 }
    /// Set the option value
    template<class T> DbStatus _getValue(T& value) const
    { return i_getValue(typeid(T), &value);                 }
    /// Access to OS independent type name
    std::string typeName() const;
    /// Set the option value
    DbStatus setValue(DbColumn::Type typ, void* value);
    /// Set the option value
    DbStatus getValue(DbColumn::Type typ, void* value) const;
  };
}       // End namespace pool
#endif  // POOL_DbOption_H
