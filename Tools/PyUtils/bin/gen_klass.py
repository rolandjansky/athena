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
              'alg', 'reentrant_alg',
              ## the python ones
              'pyalg', 'pysvc', 'pytool', 'pyaud'
              )
    needing_iface = ('svc', 'tool')
    pass

class Templates:
    isvc_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s 
#define %(guard)s 1 

#include <string>

#include "GaudiKernel/IService.h"

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : virtual public ::IService
{ 
 public: 
    
  virtual ~%(klass)s();

  static const InterfaceID& interfaceID();

}; 

inline const InterfaceID& %(klass)s::interfaceID() 
{ 
  static const InterfaceID IID_%(klass)s("%(klass)s", 1, 0);
  return IID_%(klass)s; 
}

%(namespace_end)s
#endif //> !%(guard)s
"""

    isvc_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

%(namespace_begin)s

%(klass)s::~%(klass)s()
{}

%(namespace_end)s
"""

    itool_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1

#include "GaudiKernel/IAlgTool.h"

%(namespace_begin)s

static const InterfaceID IID_%(klass)s("%(klass)s", 1, 0);

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : virtual public ::IAlgTool
{ 

 public: 

  virtual ~%(klass)s();

  static const InterfaceID& interfaceID();

 protected: 

 private:

}; 

inline const InterfaceID& %(klass)s::interfaceID() 
{ 
   return IID_%(klass)s; 
}

%(namespace_end)s
#endif //> !%(guard)s
"""

    itool_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// Framework includes
//#include "GaudiKernel/MsgStream.h"

// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

%(namespace_begin)s

%(klass)s::~%(klass)s()
{}

%(namespace_end)s
"""
    object_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1


#include <iosfwd>

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
{ 

 public: 

  %(klass)s();

  %(klass)s( const %(klass)s& rhs );

  %(klass)s& operator=( const %(klass)s& rhs ); 

  virtual ~%(klass)s(); 

 private: 

}; 

//std::ostream& operator<<( std::ostream& out, const %(klass)s& o );

%(namespace_end)s

#endif //> !%(guard)s
"""

    object_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// %(pkg)s includes
#include "%(pkg)s/%(klass)s.h"

%(namespace_begin)s

%(namespace_end)s
"""

    svc_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1

#include <string>

#include "AthenaBaseComps/AthService.h"

#include "%(ipkg)s/%(iklass)s.h"

class ISvcLocator;
template <class TYPE> class SvcFactory;

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : virtual public ::%(iklass)s,
            public ::AthService
{ 
  friend class SvcFactory<%(klass)s>;
 public: 

  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~%(klass)s(); 

  //%(klass)s &operator=(const %(klass)s &alg); 

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );

  static const InterfaceID& interfaceID();
 private: 
  %(klass)s();

  

}; 

inline const InterfaceID& %(klass)s::interfaceID() 
{ 
  return %(iklass)s::interfaceID(); 
}

%(namespace_end)s

#endif //> !%(guard)s
"""

    svc_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// %(pkg)s includes
#include "%(klass)s.h"


#include "GaudiKernel/Property.h"

%(namespace_begin)s


%(klass)s::%(klass)s( const std::string& name, 
		      ISvcLocator* pSvcLocator ) : 
  ::AthService( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );

}

%(klass)s::~%(klass)s()
{}

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

%(namespace_end)s
"""

    alg_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1


#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : public ::AthAlgorithm
{ 
 public: 
  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~%(klass)s(); 

  //%(klass)s &operator=(const %(klass)s &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;

 private: 
  %(klass)s();

}; 

%(namespace_end)s
#endif //> !%(guard)s
"""


    
    alg_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// %(pkg)s includes
#include "%(klass)s.h"

#include "GaudiKernel/Property.h"

%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );
}

%(klass)s::~%(klass)s()
{}

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

%(namespace_end)s
"""

    reentrant_alg_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : public ::AthReentrantAlgorithm
{ 
 public: 

  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~%(klass)s(); 


  //%(klass)s &operator=(const %(klass)s &alg); 

  StatusCode  initialize() override;
  StatusCode  execute_r(const EventContext& context) const override;
  StatusCode  finalize() override;
 
 private: 
  %(klass)s();
 
}; 

%(namespace_end)s
#endif //> !%(guard)s
"""


    
    reentrant_alg_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "%(klass)s.h"

%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );

}

%(klass)s::~%(klass)s()
{}

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

StatusCode %(klass)s::execute_r(const EventContext& context) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  return StatusCode::SUCCESS;
}

%(namespace_end)s
"""

    
    tool_hdr_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef %(guard)s
#define %(guard)s 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// %(ipkg)s includes
#include "%(ipkg)s/%(iklass)s.h"

%(namespace_begin)s

/**
 * @class $(klass)s
 * @brief 
 **/

class %(klass)s
  : virtual public ::%(iklass)s,
            public ::AthAlgTool
{ 

 public: 

  %(klass)s( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~%(klass)s(); 

  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

 private: 

  %(klass)s();

}; 

%(namespace_end)s
#endif //> !%(guard)s
"""

    tool_cxx_template = """\
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// %(pkg)s includes
#include "%(klass)s.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"


%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
{
  //declareProperty( "Property", m_nProperty );
}

%(klass)s::~%(klass)s()
{}

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

%(namespace_end)s
"""

    pyalg_template = """\
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
