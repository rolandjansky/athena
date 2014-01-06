# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- python -*-

## stdlib imports
import os
import os.path as osp
import re
import sys

## waf imports
import waflib.Errors
import waflib.Logs as msg
import waflib.Utils
import waflib.Configure
import waflib.Build
import waflib.Task
import waflib.Tools.ccroot
from waflib.Configure import conf
from waflib.TaskGen import feature, before_method, after_method, extension, after

### ---------------------------------------------------------------------------
@conf
def detcommon_library(ctx, **kw):
    kw['features'] = ['atlas_library'] + waflib.Utils.to_list(kw.get('features', []))
    return ctx(**kw)

### ---------------------------------------------------------------------------
@conf
def trigconf_application(ctx, **kw):
    kw = dict(kw)

    name = kw['name']
    source = kw["source"]
    del kw['name']
    del kw['source']

    kw['features'] = waflib.Utils.to_list(
        kw.get('features', '')) + [
        'cxx', 'cxxprogram', 'symlink_tsk',
        ]
    
    kw['use'] = waflib.Utils.to_list(kw.get('use', []))

    pkg_node = ctx.path.get_src()
    src_node = ctx.path.find_dir('src')

    srcs = ctx._cmt_get_srcs_lst(source)
    linkflags = waflib.Utils.to_list(kw.get('linkflags', []))
    linkflags = ctx.env.SHLINKFLAGS + linkflags
    kw['linkflags'] = linkflags

    defines = waflib.Utils.to_list(kw.get('defines', []))
    kw['defines'] = defines + ctx._get_pkg_version_defines()
    
    includes = waflib.Utils.to_list(kw.get('includes', []))
    includes.insert(0, ctx.path.abspath())
    #includes.insert(1, ctx.path.abspath()+'/'+PACKAGE_NAME)
    kw['includes'] = includes + [src_node]

    # extract package name
    PACKAGE_NAME = ctx._get_pkg_name()

    exe = ctx(
        name=name,
        source=srcs,
        **kw)
    exe.install_path = '${INSTALL_AREA}/bin'
    exe.libpath = ctx.env.LD_LIBRARY_PATH + [ctx.path.get_bld().abspath()]
    exe.target = name+'.exe'
    
    return exe

### ---------------------------------------------------------------------------
@conf
def detcommon_install_headers(ctx, **kw):
    kw['features'] = ['atlas_install_headers'] + waflib.Utils.to_list(kw.get('features', []))
    return ctx(**kw)

### ---------------------------------------------------------------------------
@conf
def detcommon_generic_install(ctx, **kw):
    kw['features'] = ['atlas_generic_install'] + waflib.Utils.to_list(kw.get('features', []))
    return ctx(**kw)

### ---------------------------------------------------------------------------
@conf
def detcommon_ant(ctx, **kw):
    kw = dict(kw)

    def callback(*k, **kw):
        return

    ## FIXME: dummy

    kw['features'] = ['atlas_generic_install'] + waflib.Utils.to_list(kw.get('features', []))
    o = ctx(**kw)
    
    return o

## EOF ##

