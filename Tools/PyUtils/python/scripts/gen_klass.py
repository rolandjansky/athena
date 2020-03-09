# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.gen_klass.py
# @purpose helper script to generate header and cxx files of various
#          athena components (svc/tool/alg/isvc/itool/object)
# @author Sebastien Binet
# @date   April 2008
#
# Use PyUtils/test/test_genklass.sh to check compilation of generated code

from __future__ import print_function

__author__ = "Sebastien Binet"
__doc__ = """\
helper script to generate header and cxx files of various athena
components (svc/tool/alg/isvc/itool/object)
"""

import os
from datetime import datetime

import PyUtils.acmdlib as acmdlib

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
    copyright_template = "Copyright (C) 2002-%d CERN for the benefit of the ATLAS collaboration" % datetime.now().year
    isvc_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

#include "GaudiKernel/IService.h"
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s : virtual public IService {
public:
  DeclareInterfaceID(%(klass)s, 1, 0);

  virtual ~%(klass)s() override {}
};
%(namespace_end)s
#endif // %(guard)s
"""

    isvc_cxx_template = ""

    itool_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

#include "GaudiKernel/IAlgTool.h"
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s : virtual public IAlgTool {
public: 
  DeclareInterfaceID(%(klass)s, 1, 0);

  virtual ~%(klass)s() override {}
}; 
%(namespace_end)s
#endif // %(guard)s
"""

    itool_cxx_template = ""

    object_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

#include <iosfwd>
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s {
public:
  %(klass)s();
  %(klass)s(const %(klass)s& rhs);
  %(klass)s& operator=(const %(klass)s& rhs);
  virtual ~%(klass)s();
};

//std::ostream& operator<<(std::ostream& out, const %(klass)s& o);
%(namespace_end)s
#endif // %(guard)s
"""

    object_cxx_template = """\
/*
  %(copyright)s
*/

#include "%(pkg)s/%(klass)s.h"
%(namespace_begin)s
%(namespace_end)s
"""

    svc_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

// Package includes
#include "%(ipkg)s/%(iklass)s.h"

// Framework includes
#include "AthenaBaseComps/AthService.h"

// STL includes
#include <string>
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s : public extends<AthService, %(iklass)s> {
public:
  %(klass)s(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~%(klass)s() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

private:
  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};
%(namespace_end)s
#endif // %(guard)s
"""

    svc_cxx_template = """\
/*
  %(copyright)s
*/
#include "%(klass)s.h"
%(namespace_begin)s
%(klass)s::%(klass)s(const std::string& name, ISvcLocator* pSvcLocator) :
  base_class(name, pSvcLocator)
{
}

%(klass)s::~%(klass)s()
{
}

StatusCode %(klass)s::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  return StatusCode::SUCCESS;
}
%(namespace_end)s
"""

    alg_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s : public AthAlgorithm {
public:
  %(klass)s(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~%(klass)s() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};
%(namespace_end)s
#endif // %(guard)s
"""

    alg_cxx_template = """\
/*
  %(copyright)s
*/

#include "%(klass)s.h"
%(namespace_begin)s
%(klass)s::%(klass)s(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

%(klass)s::~%(klass)s()
{
}

StatusCode %(klass)s::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::execute()
{
  return StatusCode::SUCCESS;
}
%(namespace_end)s
"""

    reentrant_alg_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// STL includes
#include <string>
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief 
 **/
class %(klass)s : public AthReentrantAlgorithm {
public:
  %(klass)s(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~%(klass)s() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};
%(namespace_end)s
#endif // %(guard)s
"""

    reentrant_alg_cxx_template = """\
/*
  %(copyright)s
*/

#include "%(klass)s.h"
%(namespace_begin)s
%(klass)s::%(klass)s(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

%(klass)s::~%(klass)s()
{
}

StatusCode %(klass)s::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::execute(const EventContext& context) const
{
  return StatusCode::SUCCESS;
}
%(namespace_end)s
"""

    tool_hdr_template = """\
/*
  %(copyright)s
*/
#ifndef %(guard)s
#define %(guard)s

// Package includes
#include "%(ipkg)s/%(iklass)s.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

// STL includes
#include <string>
%(namespace_begin)s
/**
 * @class %(klass)s
 * @brief
 **/
class %(klass)s : public extends<AthAlgTool, %(iklass)s> {
public:
  %(klass)s(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~%(klass)s() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

private:
  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};
%(namespace_end)s
#endif // %(guard)s
"""

    tool_cxx_template = """\
/*
  %(copyright)s
*/

#include "%(klass)s.h"
%(namespace_begin)s
%(klass)s::%(klass)s(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

%(klass)s::~%(klass)s()
{
}

StatusCode %(klass)s::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize()
{
  return StatusCode::SUCCESS;
}
%(namespace_end)s
"""

    pyalg_template = """\
#
# %(copyright)s
#

# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
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
# %(copyright)s
#

# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
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
# %(copyright)s
#

# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
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
# %(copyright)s
#

# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
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
    except AttributeError as err:
        print ("::: UNKNOWN klass_type [%s] !" % klass_type)
        raise err

    namespace_klass = klass.replace('::','_')
    namespace_begin,namespace_end = "",""
    if klass.count("::")>0:
        nm    = klass.split("::")[0]
        klass = klass.split("::")[1]
        namespace_begin = os.linesep + "namespace %s {" % nm + os.linesep
        namespace_end   = os.linesep + "} // namespace %s" % nm + os.linesep
        pass

    guard = "%s_%s_H" % (pkg.upper(), namespace_klass.upper())

    d = dict( pkg=pkg,
              klass=klass,
              ipkg=ipkg,
              iklass=iklass,
              guard=guard,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end,
              copyright=Templates.copyright_template
              )
    fname = os.path.splitext(fname)[0]

    o_hdr = open(fname+'.h', 'w')
    o_hdr.writelines(hdr%d)
    o_hdr.flush()
    o_hdr.close()

    if len(cxx)>0:
        o_cxx = open(fname+'.cxx', 'w')
        o_cxx.writelines(cxx%d)
        o_cxx.flush()
        o_cxx.close()

    return 0


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
    except AttributeError as err:
        print ("::: UNKNOWN klass_type [%s] !" % klass_type)
        raise err

    invalid_py_chars = ( ':', '.', '>', '<', ' ' )

    if any([c for c in invalid_py_chars if c in klass]):
        err = "::: INVALID class name ! (%s) !\n"%klass
        err += "::: python class names can *NOT* contain any character of %s"%\
               repr(invalid_py_chars)
        print (err)
        raise RuntimeError(err)

    fname=''.join([fname,'.py'])
    d = dict( pkg=pkg,
              klass=klass,
              fname=fname,
              copyright=Templates.copyright_template
              )
    o = open(fname, 'w')
    o.writelines(py_template%d)
    o.flush()
    o.close()
    return 0

@acmdlib.command(name='gen-klass')
@acmdlib.argument(
    "--klass",
    required=True,
    help = "The (fully qualified) name of the python or C++ class to create (ex: ElectronContainer, Analysis::Electron, MyAlgTool, PyTestAlg)")
@acmdlib.argument(
    "--pkg",
    required=True,
    help = "The name of the package holding the C++ class to create (ex: MyAnalysis, JetEvent)")
@acmdlib.argument(
    "--type",
    dest = "klass_type",
    required=True,
    choices = GenTypes.values,
    help = "The type of class to create")
@acmdlib.argument(
    "--ipkg",
    default = None,
    help = "The name of the package holding the interface of the C++ class (mandatory for 'svc' and 'tool' types)")
@acmdlib.argument(
    "--iklass",
    default = None,
    help = "The name of the interface the C++ class is implementing (mandatory for 'svc' and 'tool' types)")
@acmdlib.argument(
    "-o",
    "--output-file",
    required=True,
    dest = "fname",
    help = "The name of the file(s) which will hold header and implementation of the class (ex: 'Foo' --> ('Foo.h','Foo.cxx'))")
def main(args):
    """helper script to generate header and cxx files
    of various athena components (svc/tool/alg/isvc/itool/object)
    """

    exitcode = 0

    if args.klass_type in GenTypes.needing_iface and \
       ( args.ipkg is None or args.iklass is None ) :
        print (":: You have to give 'ipkg' and 'iklass' options to properly ",)
        print ("generate an implementation for '%s'"%args.klass_type)
        return 3


    if args.ipkg is None:
        args.ipkg = ""

    if args.iklass is None:
        args.iklass = ""

    if args.klass_type.startswith('py'):
        exitcode = gen_pyfiles(
            klass=args.klass,
            klass_type=args.klass_type,
            pkg=args.pkg,
            fname=args.fname
            )
    else:
        exitcode = gen_files(
            klass=args.klass,
            klass_type=args.klass_type,
            pkg=args.pkg,
            iklass=args.iklass,
            ipkg=args.ipkg,
            fname=args.fname
            )
    return exitcode
