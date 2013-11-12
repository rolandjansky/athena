## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/AtlasCompilers",
    "authors": ["David Quarrie <David.Quarrie@cern.ch>",
                "Emil Obreshkov <Emil.Obreshkov@cern.ch>",
                ],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("LCG_Platforms", version="*", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    macro = ctx.hwaf_declare_macro
    
    macro("gcc_config_version", (
      {"default": "4.3.6"},
      {"target-mac": ""},
      {"target-gcc45": "4.5.2"},
      {"target-gcc46": "4.6.3"},
      {"target-gcc47": "4.7.2"},
      {"target-gcc48": "4.8.1"},
      {"target-clang32": "4.6.3"},
      {"target-clang33": "4.8.1"},
      {"target-icc13": "4.6.3"},
    ), override=True)

    macro("clang_config_version", (
      {"default": "2.8"},
      {"target-llvm29": "2.9"},
      {"target-clang30": "3.0"},
      {"target-clang32": "3.2"},
      {"target-clang33": "3.3"},
    ), override=True)

    macro("icc_config_version", (
      {"default": "11.1/072"},
      {"target-icc11": "11.1/072"},
      {"target-icc13": "13"},
    ), override=True)

    macro("compiler_versions", (
      {"default": "gcc=${gcc_config_version}:icc=${icc_config_version}:llvm=${clang_config_version}"},
    ))
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    
    
    return # build

## EOF ##
