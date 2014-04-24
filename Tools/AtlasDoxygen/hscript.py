# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "Tools/AtlasDoxygen",
    "authors": ["ATLAS Collaboration"],
}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("LCG_Settings", version="*", public=True)
    ctx.use_pkg("LCG_Configuration", version="*", public=True)
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    
    
    ctx.hwaf_path_remove("DOTFONTPATH", (
      {"default": ""},
      {("Linux", "Doxygen"): "AtlasDoxygen"},
    ))

    ## FIXME: implement a real waf-feature to do doxygen documentation
    
    ## FIXME: install fonts/ttf under INSTALL_AREA and use that instead of AtlasDoxygen_root
    ctx.hwaf_path_prepend("DOTFONTPATH", (
      {"default": ""},
      {("Linux", "Doxygen"): "${AtlasDoxygen_root}/fonts/ttf"},
    ))

    ctx.hwaf_declare_macro("AtlasDoxygen_native_version", (
      {"default": ""},
      {"Doxygen": "1.5.9"},
    ))

    ctx.hwaf_path_remove("PATH", (
      {"default": ""},
      {"Doxygen": "/doxygen/"},
    ))

    ctx.hwaf_path_prepend("PATH", (
      {"default": ""},
      {"Doxygen": "${LCG_external}/doxygen/${AtlasDoxygen_native_version}/${LCG_system}/bin"},
    ))

    ctx.hwaf_declare_macro("graphviz_native_version", (
      {"default": ""},
      {"Doxygen": "2.24.0"},
    ))

    ctx.hwaf_path_remove("PATH", (
      {"default": ""},
      {"Doxygen": "/graphviz/"},
    ))

    ctx.hwaf_path_prepend("PATH", (
      {"default": ""},
      {"Doxygen": "${LCG_external}/graphviz/${graphviz_native_version}/${LCG_system}/bin"},
    ))

    ctx.hwaf_macro_append("DOXYGEN_HTML_MAPPING", (
      {"default": ""},
      {("Doxygen", "local"): "/build/atlas/software/builds/@http://cern.ch/atlas-computing/links/buildDirectory"},
      {("Doxygen", "nightly"): "/build/atnight/localbuilds/nightlies/projects/@http://cern.ch/atlas-computing/links/buildDirectory/nightlies/dev"},
      {("Doxygen", "nightlyBUG"): "/build/atnight/localbuilds/nightlies/projects/@http://cern.ch/atlas-computing/links/buildDirectory/nightlies/bugfix"},
      {("Doxygen", "nightlyAFS"): "/afs/cern.ch/atlas/software/builds/nightlies/dev/@http://cern.ch/atlas-computing/links/buildDirectory/nightlies/dev"},
      {"Doxygen": "/afs/cern.ch/atlas/software/builds/@http://cern.ch/atlas-computing/links/buildDirectory"},
    ))

    ctx.hwaf_macro_append("DOXYGEN_STRIP_FROM_PATH", (
      {"default": ""},
      {("Doxygen", "local"): "/afs/cern.ch/atlas/software/builds/"},
      {("Doxygen", "nightly"): "/afs/cern.ch/atlas/software/builds/nightlies/dev/"},
    ))

    ctx.hwaf_declare_macro("DOXYGEN_COMMON_OUTPUT_DIRECTORY", (
      {"default": ""},
      {"Doxygen": "${DOXYGEN_OUTPUT}"},
    ))

    ctx.hwaf_declare_macro("DOXYGEN_INPUT", (
      {"default": ""},
      {"Doxygen": ["../src", "../${package}", "../doc", "../share", "../python", "../cmt/fragments"]},
    ))
    #### macro &{{DOXYGEN_FILE_PATTERNS [{default []} {Doxygen [*.cxx *.h *.py *.mk *.icc]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_FILE_PATTERNS", (
      {"default": ""},
      {"Doxygen": ["*.cxx", "*.h", "*.py", "*.mk", "*.icc"]},
    ))
    #### macro &{{DOXYGEN_EXCLUDE [{default []} {Doxygen []}]}}
    ctx.hwaf_declare_macro("DOXYGEN_EXCLUDE", (
      {"default": ""},
      {"Doxygen": ""},
    ))
    #### macro &{{DOXYGEN_EXCLUDE_PATTERNS [{default []} {Doxygen []}]}}
    ctx.hwaf_declare_macro("DOXYGEN_EXCLUDE_PATTERNS", (
      {"default": ""},
      {"Doxygen": ""},
    ))
    #### macro &{{DOXYGEN_EXAMPLE_PATH [{default []} {Doxygen [../doc ../cmt ../share]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_EXAMPLE_PATH", (
      {"default": ""},
      {"Doxygen": ["../doc", "../cmt", "../share"]},
    ))
    #### macro &{{DOXYGEN_EXAMPLE_PATTERNS [{default []} {Doxygen [*.cxx *.html requirements *.py]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_EXAMPLE_PATTERNS", (
      {"default": ""},
      {"Doxygen": ["*.cxx", "*.html", "requirements", "*.py"]},
    ))
    #### macro &{{DOXYGEN_SORT_MEMBER_DOCS [{default []} {Doxygen [NO]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_SORT_MEMBER_DOCS", (
      {"default": ""},
      {"Doxygen": "NO"},
    ))
    #### macro &{{DOXYGEN_INLINE_INHERITED_MEMB [{default []} {Doxygen [YES]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_INLINE_INHERITED_MEMB", (
      {"default": ""},
      {"Doxygen": "YES"},
    ))
    #### macro &{{DOXYGEN_IMAGE_PATH [{default []}]}}
    ctx.hwaf_declare_macro("DOXYGEN_IMAGE_PATH", (
      {"default": ""},
    ))
    #### macro &{{DOXYGEN_UML_LOOK [{default []} {Doxygen [YES]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_UML_LOOK", (
      {"default": ""},
      {"Doxygen": "YES"},
    ))
    #### macro &{{DOXYGEN_QT_AUTOBRIEF [{default []} {Doxygen [YES]}]}}
    ctx.hwaf_declare_macro("DOXYGEN_QT_AUTOBRIEF", (
      {"default": ""},
      {"Doxygen": "YES"},
    ))

    ctx.hwaf_declare_macro("DOXYGEN_JAVADOC_AUTOBRIEF", (
      {"default": ""},
      {"Doxygen": "YES"},
    ))
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    return # build

## EOF ##
