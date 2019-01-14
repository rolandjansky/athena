/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbStatus.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Storage Service DbStatus definition file
//--------------------------------------------------------------------
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBSTATUS_H
#define POOL_DBSTATUS_H 1

// Framework customization file
#include "StorageSvc/DbConfig.h"

/*
 *   POOL namespace declaration
 */
namespace pool {

  /** @class DbStatus DbStatus.h, StorageSvc/DbStatus.h
    *
    * This class is highly inspired by the error code distribution
    * system as it is used in DEC/VMS or MS/WNT.
    *
    * Values are 32 bit values layed out as follows:
    *
    *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
    *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    * +---+-+-+-----------------------+-------------------------------+
    * |Sev|C|R|     Facility          |               Code            |
    * +---+-+-+-----------------------+-------------------------------+
    *
    * where
    *
    *     Sev - is the severity code
    *
    *         00 - Success
    *         01 - Informational
    *         10 - Warning
    *         11 - Error
    *
    *     C - is a customer bit  (currently not used)
    *     R - is a reserved bit
    *
    *     Facility - is the facility code
    *
    *     Code - is the facility's DbStatus code
    *
    * Additionally Success message codes  **must** have the low bit set.
    * e.g.
    * 0xC0000001 is Success
    * 0xD0000000 is Informational
    * 0xE0000000 is Warning
    * 0xF0000000 is Error
    *
    * however, 0xF0000001, 0xE000001, 0xD0000001 or 0xC0000001 
    * are not allowed.
    *
    *
    * @author   Markus Frank
    * @version  1.0
    * @date     15/07/2002
    */
  class DbStatus  {
  private:
    unsigned int m_code;

  public:
    enum  ErrorLevel    {
      Success    = 1,
      Info       = (1<<30),
      Warning    = (1<<31),
      Error      = (1<<31) + (1<<30)
    };
    /// Standard Constructor. Defaults to Error
    DbStatus() : m_code(Error)                          {                     }
    /// Default copy constructor
    DbStatus(const DbStatus&) = default;
    DbStatus& operator=(const DbStatus&) = default;
    /// Standard constructor with initialization from long  value.
    DbStatus(unsigned int code) : m_code(code)  {                        }
    /// Constructor with initialization from Error level, facility and code.
    DbStatus(ErrorLevel lvl, unsigned int fac, unsigned int code);
    /// Default destructor. Non virtual for performance reasons. Do NOT inherit.
    ~DbStatus() = default;
    /// Cast operator.
    operator unsigned int() const              {  return m_code;              }
    /// Assignment operator.
    DbStatus& operator=(unsigned int value)    { m_code = value; return *this;}
    /// Access severity code
    unsigned int   severity()  const  { return  m_code>>30;                   }
    /// Access facility code
    unsigned int   facility()  const  { return (m_code&0x0FFF0000)>>16;       }
    /// Access error code value
    unsigned int   code()      const  { return  m_code&0x0000FFFF;            }
    /// Access the reserved bits
    unsigned int   reserved()  const  { return (m_code&0x3FFFFFFF)>>28;       }
    /// Acces fully encoded value
    unsigned int   value()     const  { return  m_code;                       }
    /// Check if the return code indicated success
    bool            isSuccess() const { return (m_code>>30)==0 && (m_code&1); }
    /// Check if the return code indicated  an informational message
    bool            isInfo()    const { return (m_code>>30)==1;               }
    /// Check if the return code indicated a warning
    bool            isWarning() const { return (m_code>>30)==2;               }
    /// Check if the return code indicated an error
    bool            isError()   const { return (m_code>>30)==3;               }
    /// Access  bit mask field  for Success messages.
    static unsigned int success()     { return  Success;                      }
    /// Access  bit mask field  for Informational messages.
    static unsigned int info()        { return  Info;                         }        
    /// Access  bit mask field  for Warning messages.
    static unsigned int warning()     { return  Warning;                      }
    /// Access  bit mask field  for Error messages.
    static unsigned int error()       { return  Error;                        }

    /// Comparison operator lesser.
    friend bool operator< ( const DbStatus& a, const DbStatus& b );
    /// Comparison operator greater.
    friend bool operator> ( const DbStatus& a, const DbStatus& b );
  };


  inline DbStatus::DbStatus(DbStatus::ErrorLevel lvl, unsigned int fac, unsigned int cod)
  { m_code = (lvl&Error)+((fac&0x0FFF)<<16)+(cod&0xFFFF);                     }
  inline bool operator< ( const DbStatus& a, const DbStatus& b ) 
  { return a.m_code < b.m_code;                                               }
  inline bool operator> ( const DbStatus& a, const DbStatus& b ) 
  { return a.m_code < b.m_code;                                               }
}

#endif  // POOL_DBSTATUS_H
