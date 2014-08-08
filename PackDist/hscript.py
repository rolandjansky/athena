# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-
## automatically generated from a hscript
## do NOT edit.

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "PackDist",
    "authors": ["Grigori Rybkine <Grigori.Rybkine@cern.ch>", "Christian Arnault <arnault@lal.in2p3.fr>"],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## no public dependencies
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    
    
    #### set &{{PACKCFG [{default [../config/pack-nightly-bugfix.cfg]}]}}
    ctx.hwaf_declare_macro("PACKCFG", (
      {"default": "../config/pack-nightly-bugfix.cfg"},
    ))
    ctx.hwaf_declare_runtime_env("PACKCFG")
    #### set &{{PACKOPTS [{default [${PACKOPTS}]}]}}
    ctx.hwaf_declare_macro("PACKOPTS", (
      {"default": "${PACKOPTS}"},
    ))
    ctx.hwaf_declare_runtime_env("PACKOPTS")
    #### set_remove &{{PACKOPTS [{default [-w]}]}}
    ctx.hwaf_macro_remove("PACKOPTS", (
      {"default": "-w"},
    ))
    ##### **** statement *hlib.SetAppendStmt (&{{PACKOPTS [{default [-w]}]}})
    #### macro &{{PACKREPO [{default [../run]}]}}
    ctx.hwaf_declare_macro("PACKREPO", (
      {"default": "../run"},
    ))
    #### macro &{{PACKINSTALL [{default [../installation]}]}}
    ctx.hwaf_declare_macro("PACKINSTALL", (
      {"default": "../installation"},
    ))
    #### macro &{{urlget [{default [wget]} {Darwin [curl -L -O]}]}}
    ctx.hwaf_declare_macro("urlget", (
      {"default": "wget"},
      {"Darwin": ["curl", "-L", "-O"]},
    ))
    #### macro &{{main [{default [${PACKAGE_ROOT}/scripts/proj-run.sh]}]}}
    ctx.hwaf_declare_macro("main", (
      {"default": "${PACKAGE_ROOT}/scripts/proj-run.sh"},
    ))
    #### macro &{{version_info [{default [`${main} -v`]}]}}
    ctx.hwaf_declare_macro("version_info", (
      {"default": ["`${main}", "-v`"]},
    ))
    #### macro &{{doc_proj-run_sh [{default [pack project(s}, external package(s}]}]}}
    ctx.hwaf_declare_macro("doc_proj-run_sh", (
      {"default": ["pack", "project(s},", "external", "package(s}"]},
    ))
    #### macro &{{doc_pack-relocate_sh [{default [relocate or transform distribution kit]}]}}
    ctx.hwaf_declare_macro("doc_pack-relocate_sh", (
      {"default": ["relocate", "or", "transform", "distribution", "kit"]},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{pack_run})
    ##### **** statement *hlib.MakeFragmentStmt (&{pack_install})
    ##### **** statement *hlib.MakeFragmentStmt (&{help2doc})
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    ##### **** statement *hlib.DocumentStmt (&{pack_run [Run -group=run $(main)]})
    ##### **** statement *hlib.DocumentStmt (&{pack_install [Installation -group=installation -s=. $(PACKREPO)]})
    ##### **** statement *hlib.DocumentStmt (&{help2doc [Doc -group=doc $(main) ../share/pack-relocate.sh]})
    
    
    return # build

## EOF ##
