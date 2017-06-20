///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IoSvc.h 
// Header file for class IoSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_IOSVC_H
#define ATHENASERVICES_IOSVC_H 1

// STL includes
#include <string>
#include <unordered_map>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// AthenaKernel
#include "AthenaRootKernel/IIoSvc.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;



class IoSvc
  : virtual public ::IIoSvc,
            public ::AthService
{ 
  friend class SvcFactory<IoSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  typedef IIoSvc::Fd Fd;

  // Copy constructor: 

  /// Constructor with parameters: 
  IoSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~IoSvc(); 

  // Assignment operator: 
  //IoSvc &operator=(const IoSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// test if a given file descriptor `fd` is known to us
  bool has_fd(Fd fd) const;

  /// retrieve the file descriptor associated with file `fname`
  /// @returns -1 if no such `fname` is known
  Fd fd(const std::string& fname) const;

  /// retrieve the file `fname` associated with file descriptor `fd`
  /// @returns empty string if no such `fd` is known
  const std::string& fname(Fd fd) const;

  /// retrieve the open mode associated with file descriptor `fd`
  IoType mode(Fd fd) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /// open file `fname` with open mode `mode`
  /// @returns -1 if not successful
  Fd open(const std::string& fname, IoType mode);

  /// close file `fd`
  StatusCode close(Fd fd);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  IoSvc();

  struct FdInfos {
    std::string fname;
    IIoSvc::IoType mode;
  };

  typedef std::unordered_map<Fd, FdInfos> FdMap_t;

  /// map of fd->fdinfos
  FdMap_t m_fds;

  /// last created Fd
  Fd m_last_fd;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& IoSvc::interfaceID() 
{ 
  return IIoSvc::interfaceID(); 
}



#endif //> !ATHENASERVICES_IOSVC_H
