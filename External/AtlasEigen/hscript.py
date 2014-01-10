## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/AtlasEigen",
    "authors": [ATLAS collaboration],
}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("AtlasPolicy", version="AtlasPolicy-*", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    ctx.load("find_eigen")
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):

    if ctx.hwaf_enabled_tag("STANDALONE"):
        ctx.load("find_eigen")
        ctx.find_eigen()
        return
    
    macro = ctx.hwaf_declare_macro
    
    macro("AtlasEigen_native_version", "3.1.4")
    macro("AtlasEigen_home", "${ATLAS_EXTERNAL}/eigen/${AtlasEigen_native_version}/${LCG_platform}")
    macro("AtlasEigen_incdir", "${AtlasEigen_home}/include")
    
    macro("AtlasEigen_export_paths", "${AtlasEigen_home}")
    ctx.lcg_declare_external_package(
        'AtlasEigen',
        path='${AtlasEigen_home}',
        incdir='${AtlasEigen_incdir}',
        )

    path = ctx.hwaf_subst_vars("${AtlasEigen_home}")
    ctx.find_eigen(path_list=[path], extra_paths=[path])

    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    return # build

## EOF ##
