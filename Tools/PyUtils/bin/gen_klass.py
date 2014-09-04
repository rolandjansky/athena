#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @purpose: helper script to generate header and cxx files of various
#           athena components (svc/tool/alg/isvc/itool/object)
# @author Sebastien Binet
# @date   April 2008

import sys,os

class GenTypes:
    values = ('object',
              'isvc', 'svc',
              'itool', 'tool',
              'alg',
              ## the python ones
              'pyalg', 'pysvc', 'pytool', 'pyaud'
              )
    needing_iface = ('svc', 'tool')
    pass

class Templates:
    isvc_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s 
#define %(guard)s 1 

/** @class %(klass)s
 */

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IService.h"

// %(pkg)s includes

%(namespace_begin)s

class %(klass)s
  : virtual public ::IService
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~%(klass)s();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& %(klass)s::interfaceID() 
{ 
  static const InterfaceID IID_%(klass)s("%(klass)s", 1, 0);
  return IID_%(klass)s; 
}

%(namespace_end)s
#endif //> !%(guard)s
"""

    isvc_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////
%(klass)s::~%(klass)s()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
"""

    itool_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s
#define %(guard)s 1

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration

%(namespace_begin)s

static const InterfaceID IID_%(klass)s("%(klass)s", 1, 0);

class %(klass)s
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~%(klass)s();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& %(klass)s::interfaceID() 
{ 
   return IID_%(klass)s; 
}

%(namespace_end)s
#endif //> !%(guard)s
"""

    itool_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Framework includes
//#include "GaudiKernel/MsgStream.h"

// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////
%(klass)s::~%(klass)s()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
///////////////////////////////////////////////////////////////////

%(namespace_end)s
"""
    object_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s
#define %(guard)s 1

// STL includes
#include <iosfwd>

// Gaudi includes

// Forward declaration

%(namespace_begin)s

class %(klass)s
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Default constructor: 
  %(klass)s();

  /// Copy constructor: 
  %(klass)s( const %(klass)s& rhs );

  /// Assignment operator: 
  %(klass)s& operator=( const %(klass)s& rhs ); 

  /// Constructor with parameters: 

  /// Destructor: 
  virtual ~%(klass)s(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const %(klass)s& o );

%(namespace_end)s

#endif //> !%(guard)s
"""

    object_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

// STL includes

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
"""

    svc_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s
#define %(guard)s 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// %(ipkg)s
#include "%(ipkg)s/%(iklass)s.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

%(namespace_begin)s

class %(klass)s
  : virtual public ::%(iklass)s,
            public ::AthService
{ 
  friend class SvcFactory<%(klass)s>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~%(klass)s(); 

  // Assignment operator: 
  //%(klass)s &operator=(const %(klass)s &alg); 

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

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  %(klass)s();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& %(klass)s::interfaceID() 
{ 
  return %(iklass)s::interfaceID(); 
}

%(namespace_end)s

#endif //> !%(guard)s
"""

    svc_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// %(pkg)s includes
#include "%(klass)s.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
%(klass)s::%(klass)s( const std::string& name, 
		      ISvcLocator* pSvcLocator ) : 
  ::AthService( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
%(klass)s::~%(klass)s()
{}

// Athena Service's Hooks
////////////////////////////
StatusCode %(klass)s::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
%(klass)s::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( %(iklass)s::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<%(iklass)s*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
"""

    alg_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s
#define %(guard)s 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

%(namespace_begin)s

class %(klass)s
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~%(klass)s(); 

  // Assignment operator: 
  //%(klass)s &operator=(const %(klass)s &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  %(klass)s();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
#endif //> !%(guard)s
"""

    alg_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// %(pkg)s includes
#include "%(klass)s.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
%(klass)s::%(klass)s( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
%(klass)s::~%(klass)s()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode %(klass)s::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
"""

    tool_hdr_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.h 
// Header file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef %(guard)s
#define %(guard)s 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// %(ipkg)s includes
#include "%(ipkg)s/%(iklass)s.h"

// Forward declaration
class StoreGateSvc;

%(namespace_begin)s

class %(klass)s
  : virtual public ::%(iklass)s,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  %(klass)s( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~%(klass)s(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  %(klass)s();

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to the StoreGate service
  StoreGateSvc_t m_storeGate;

  // Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
#endif //> !%(guard)s
"""

    tool_cxx_template = """\
///////////////////////// -*- C++ -*- /////////////////////////////
// %(klass)s.cxx 
// Implementation file for class %(klass)s
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// %(pkg)s includes
#include "%(klass)s.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

%(namespace_begin)s

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
%(klass)s::%(klass)s( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_storeGate( "StoreGateSvc", name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
%(klass)s::~%(klass)s()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode %(klass)s::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // Get pointer to StoreGateSvc and cache it :
  if ( !m_storeGate.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

%(namespace_end)s
"""

    pyalg_template = """\
# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.5 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class %(klass)s (PyAthena.Alg):
    'put some documentation here'
    def __init__(self, name='%(klass)s', **kw):
        ## init base class
        kw['name'] = name
        super(%(klass)s, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        return StatusCode.Success

    def execute(self):
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class %(klass)s
"""

    pysvc_template = """\
# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.5 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class %(klass)s (PyAthena.Svc):
    'put some documentation here'
    def __init__(self, name='%(klass)s', **kw):
        ## init base class
        kw['name'] = name
        super(%(klass)s, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class %(klass)s
"""

    pytool_template = """\
# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.5 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class %(klass)s (PyAthena.AlgTool):
    'put some documentation here'
    def __init__(self, name='%(klass)s', **kw):
        ## init base class
        kw['name'] = name
        super(%(klass)s, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class %(klass)s
"""

    pyaud_template = """\
# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.5 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class %(klass)s (PyAthena.Aud):
    'put some documentation here'
    def __init__(self, name='%(klass)s', **kw):
        ## init base class
        kw['name'] = name
        super(%(klass)s, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class %(klass)s
"""

def gen_files(pkg="", klass="", klass_type='object', fname='foo',
              ipkg="", iklass=""):
    """Simple helper function to generate files based off some informations
     @param pkg the name of the package holding the class we want to generate
     @param klass the (fully qualified) name of the C++ class to generate
     @param klass_type the type of class to generate (svc/tool/alg/object)
     @param fname the filename to generate
     @param ipkg the name of the package holding the interface of the class
     @param iklass the name of the interface of the class we generate
    """
    try:
        hdr = getattr(Templates, '%s_hdr_template'%klass_type)
        cxx = getattr(Templates, '%s_cxx_template'%klass_type)
    except AttributeError,err:
        print "::: UNKNOWN klass_type [%s] !" % klass_type
        raise err

    namespace_begin,namespace_end = "",""
    if klass.count("::")>0:
        nm    = klass.split("::")[0]
        klass = klass.split("::")[1]
        namespace_begin = "namespace %s {" % nm
        namespace_end   = "} //> end namespace %s" % nm
        pass

    guard = "%s_%s_H" % (pkg.upper(), klass.upper())

    d = dict( pkg=pkg,
              klass=klass,
              ipkg=ipkg,
              iklass=iklass,
              guard=guard,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end
              )
    fname = os.path.splitext(fname)[0]
    
    o_hdr = open(fname+'.h', 'w')
    o_hdr.writelines(hdr%d)
    o_hdr.flush()
    o_hdr.close()

    o_cxx = open(fname+'.cxx', 'w')
    o_cxx.writelines(cxx%d)
    o_cxx.flush()
    o_cxx.close()

    return

   
def gen_pyfiles(pkg="", klass="", klass_type='pyalg', fname='foo'):
    """Simple helper function to generate (python) files based off some
       user informations.
     @param pkg the name of the package holding the class we want to generate
     @param klass the name of the python class to generate
     @param klass_type the type of class to generate (pysvc/pytool/pyalg/pyaud)
     @param fname the filename to generate
    """
    try:
        py_template = getattr(Templates, '%s_template'%klass_type)
    except AttributeError,err:
        print "::: UNKNOWN klass_type [%s] !" % klass_type
        raise err

    invalid_py_chars = ( ':', '.', '>', '<', ' ' )
    
    if any([c for c in invalid_py_chars if c in klass]):
        err = "::: INVALID class name ! (%s) !\n"%klass
        err += "::: python class names can *NOT* contain any character of %s"%\
               repr(invalid_py_chars)
        print err
        raise RuntimeError(err)

    fname=''.join([fname,'.py'])
    d = dict( pkg=pkg,
              klass=klass,
              fname=fname
              )
    o = open(fname, 'w')
    o.writelines(py_template%d)
    o.flush()
    o.close()
    return

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser( usage = "usage: %prog [options] filename")
    p = parser.add_option
    p("--klass",
      dest = "klass",
      default = None,
      help = "The (fully qualified) name of the python or C++ class to create (ex: ElectronContainer, Analysis::Electron, MyAlgTool, PyTestAlg)")

    p("--pkg",
      dest = "pkg",
      default = None,
      help = "The name of the package holding the C++ class to create (ex: MyAnalysis, JetEvent)")

    p("--type",
      dest = "klass_type",
      default = None,
      help = "The type of class to create (%s)"%("|".join(GenTypes.values)))

    p("--ipkg",
      dest = "ipkg",
      default = None,
      help = "The name of the package holding the interface of the C++ class (mandatory for 'svc' and 'tool' types)")

    p("--iklass",
      dest = "iklass",
      default = None,
      help = "The name of the interface the C++ class is implementing (mandatory for 'svc' and 'tool' types)")

    p("-o",
      "--output-file",
      dest = "fname",
      default = None,
      help = "The name of the file(s) which will hold header and implementation of the class (ex: 'Foo' --> ('Foo.h','Foo.cxx'))")
    
    (options, args) = parser.parse_args()

    if options.klass is None or \
       options.klass_type is None or \
       options.pkg is None or \
       options.fname is None :
        parser.print_help()
        print ":: arguments 'klass', 'type', 'pkg' and 'output-file' are ",
        print "MANDATORY"
        sys.exit(1)

    if not (options.klass_type in GenTypes.values):
        print ":: UNKNOWN klass_type [%s] !" % options.klass_type
        print ":: allowed values are (%s)" % ("|".join(GenTypes.values))
        sys.exit(2)
        
    if options.klass_type in GenTypes.needing_iface and \
       ( options.ipkg is None or options.iklass is None ) :
        parser.print_help()
        print ":: You have to give 'ipkg' and 'iklass' options to properly ",
        print "generate an implementation for '%s'"%options.klass_type
        sys.exit(3)
        

    if options.ipkg is None:
        options.ipkg = ""

    if options.iklass is None:
        options.iklass = ""

    if options.klass_type.startswith('py'):
        gen_pyfiles(klass=options.klass,
                    klass_type=options.klass_type,
                    pkg=options.pkg,
                    fname=options.fname)
    else:
        gen_files(klass=options.klass,
                  klass_type=options.klass_type,
                  pkg=options.pkg,
                  iklass=options.iklass,
                  ipkg=options.ipkg,
                  fname=options.fname)
