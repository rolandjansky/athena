///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IIoSvc.h 
// Header file for class IIoSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_IIOSVC_H 
#define ATHENAKERNEL_IIOSVC_H 1 

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IService.h"

// AthenaKernel includes


/** @class IIoSvc is a registry of file handles, storing their name
 *  and openmode.
 */
class IIoSvc
  : virtual public ::IService
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public typedef: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// unix-y file descriptor
  typedef int Fd;
  
  /// I/O Connection types
  enum IoType   { 
    INVALID  = 0,
    READ     = 1<<1,
    UPDATE   = 1<<2,
    CREATE   = 1<<3,
    RECREATE = (1<<4)+(1<<3) 
  };

  static
  const std::string& IoTypeName(IoType mode) {
    static std::map<IoType, std::string> s_names;
    if (s_names.empty()) {
      s_names[INVALID] = "";
      s_names[READ] = "READ";
      s_names[UPDATE] = "UPDATE";
      s_names[CREATE] = "CREATE";
      s_names[RECREATE] = "RECREATE";
    }
    return s_names[mode];
  }

  static
  IoType IoTypeFromName(const std::string& name) {
    static std::map<std::string, IoType> s_names;
    if (s_names.empty()) {
      s_names[""] = INVALID;
      s_names["READ"] = READ;
      s_names["UPDATE"] = UPDATE;
      s_names["CREATE"] = CREATE;
      s_names["RECREATE"] = RECREATE;
    }
    return s_names[name];
  }

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Destructor: 
   */
  virtual ~IIoSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /// open file `fname` with open mode `mode`
  /// @returns -1 if not successful
  virtual 
  Fd open(const std::string& fname, IoType mode) = 0;

  /// close file `fd`
  virtual
  StatusCode close(Fd fd) = 0;

  /// test if a given file descriptor `fd` is known to us
  virtual
  bool has_fd(Fd fd) const = 0;

  /// retrieve the file descriptor associated with file `fname`
  /// @returns -1 if no such `fname` is known
  virtual
  Fd fd(const std::string& fname) const = 0;

  /// retrieve the file `fname` associated with file descriptor `fd`
  /// @returns empty string if no such `fd` is known
  virtual
  const std::string& fname(Fd fd) const = 0;

  /// retrieve the open mode associated with file descriptor `fd`
  virtual
  IoType mode(Fd fd) const = 0;
  
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& IIoSvc::interfaceID() 
{ 
  static const InterfaceID IID_IIoSvc("IIoSvc", 1, 0);
  return IID_IIoSvc; 
}


#endif //> !ATHENAKERNEL_IIOSVC_H
