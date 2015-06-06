## -*- python -*-
## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/AtlasBoost",
    "authors": ["Paolo Calafiura <Paolo.Calafiura@cern.ch>"],
}


### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("External/ExternalPolicy")
    ctx.use_pkg("LCG_Interfaces/Boost")
    return # pkg_deps

### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):

    ## define an AtlasBoost (default) use-rule from the boost one
    ## somewhat mimicking the CMT-AtlasBoost behaviour
    for d in ("INCLUDES", "CXXFLAGS", "DEFINES", "LIBPATH"):
        ctx.env["%s_AtlasBoost" % d] = ctx.env["%s_boost" % d]
    
    boost_system = ctx.env["LIB_boost-system"]
    boost_filesystem = ctx.env["LIB_boost-filesystem"]
    boost_thread = ctx.env["LIB_boost-thread"]
    ctx.env["LIB_AtlasBoost"] = boost_system + boost_filesystem + boost_thread
    
    return # configure

### ---------------------------------------------------------------------------
def build(ctx):
    return # build

## EOF ##
