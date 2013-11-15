## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/AtlasPython",
    "authors": ["David Quarrie <David.Quarrie@cern.ch>"],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("External/ExternalPolicy", version="ExternalPolicy-00-*", public=True)
    ctx.use_pkg("LCG_Interfaces/Python", version="v*", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    macro = ctx.hwaf_declare_macro
    
    ##### **** statement *hlib.SetStmt (&{{EXTRA_CFLAGS [{default []} {target-i686 [-m32]}]}})
    macro("EXTRA_CFLAGS", (
        {"default": ""},
        {"target-i686": "-m32"},
    ))
    ctx.hwaf_declare_runtime_env("EXTRA_CFLAGS")

    ## FIXME: migrate to a hwaf-feature
    
    ##### **** statement *hlib.PatternStmt (&{distutils_build macro_append constituents distutils_build_<name> ; private ; use PyCmt PyCmt-* Tools -no_auto_imports ; end_private ; macro distutils_build_<name>_dependencies  ; action distutils_build_<name> $(AtlasPython_root)/cmt/distutils-build.py --name=<name> --src-dir='<src_dir> --install-dir='$(CMTINSTALLAREA)/<install_dir>' ;})
    #### macro &{{py_distutils_ldflags [{default [${Python_linkopts} -shared ${cpplinkflags} ${cmt_installarea_linkopts}]}]}}
    macro("py_distutils_ldflags", (
      {"default": ["${Python_linkopts}", "-shared", "${cpplinkflags}", "${cmt_installarea_linkopts}"]},
    ))
    #### macro &{{py_distutils_cc [{default [${cc} -fno-strict-aliasing]}]}}
    macro("py_distutils_cc", (
      {"default": ["${cc}", "-fno-strict-aliasing"]},
    ))
    #### macro &{{py_distutils_cflags [{default [${cflags} -fno-strict-aliasing]}]}}
    macro("py_distutils_cflags", (
      {"default": ["${cflags}", "-fno-strict-aliasing"]},
    ))
    #### macro_append &{{py_distutils_cflags [{default [${cppflags} ${cppdebugflags}]}]}}
    ctx.hwaf_macro_append("py_distutils_cflags", (
      {"default": ["${cppflags}", "${cppdebugflags}"]},
    ))
    #### macro_remove &{{py_distutils_cflags [{default [-Woverloaded-virtual]}]}}
    ctx.hwaf_macro_remove("py_distutils_cflags", (
      {"default": "-Woverloaded-virtual"},
    ))
    #### macro &{{py_distutils_cxx [{default [${cpp} -fno-strict-aliasing]}]}}
    macro("py_distutils_cxx", (
      {"default": ["${cpp}", "-fno-strict-aliasing"]},
    ))
    #### macro &{{py_distutils_cxxflags [{default [${cppflags} ${cppdebugflags}]}]}}
    macro("py_distutils_cxxflags", (
      {"default": ["${cppflags}", "${cppdebugflags}"]},
    ))
    #### macro &{{py_distutils_fcompiler [{default [${for}]}]}}
    macro("py_distutils_fcompiler", (
      {"default": "${for}"},
    ))
    #### macro &{{py_distutils_fflags [{default [${fflags}]}]}}
    macro("py_distutils_fflags", (
      {"default": "${fflags}"},
    ))
    #### macro &{{py_distutils_cppflags [{default [${includes}]}]}}
    macro("py_distutils_cppflags", (
      {"default": "${includes}"},
    ))

    ctx.hwaf_copy_uselib_defs("AtlasPython", "python")
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    
    
    return # build

## EOF ##
