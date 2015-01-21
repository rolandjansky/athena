## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/PlatformPolicy",
    "authors": ["ATLAS collaboration"],
}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    if not ctx.hwaf_enabled_tag("xaod-standalone"):
        ctx.use_pkg("GaudiPolicy", version="*", public=True)
        pass
    ctx.use_pkg("AtlasCommonPolicy", version="AtlasCommonPolicy-*", public=True)
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    
    ## FIXME:
    ctx.hwaf_declare_tag("LCGCMT_INSTALLED", content=[])
    ctx.hwaf_declare_tag("block-tdaqc", content=[])
    ###
    
    ctx.hwaf_apply_tag("LCGCMT_INSTALLED")

    # Force an override of the tdaq-common policies
    ctx.hwaf_apply_tag("block-tdaqc")

    tag = ctx.hwaf_declare_tag

    # ---------- Scientific Linux CERN 5 ----------

    tag("i686-slc5-gcc34-opt", content=["Linux", "slc5", "32", "gcc-3.4", "opt", "i686-slc5-gcc34"])
    tag("i686-slc5-gcc34-dbg", content=["Linux", "slc5", "32", "gcc-3.4", "debug", "i686-slc5-gcc34"])

    tag("i686-slc5-gcc41-opt", content=["Linux", "slc5", "32", "gcc-4.1", "opt", "i686-slc5-gcc41"])
    tag("i686-slc5-gcc41-dbg", content=["Linux", "slc5", "32", "gcc-4.1", "debug", "i686-slc5-gcc41"])

    tag("x86_64-slc5-gcc41-opt", content=["Linux", "slc5", "64", "gcc-4.1", "opt", "x86_64-slc5-gcc41"])
    tag("x86_64-slc5-gcc41-dbg", content=["Linux", "slc5", "64", "gcc-4.1", "debug", "x86_64-slc5-gcc41"])

    tag("i686-slc5-gcc43-opt", content=["Linux", "slc5", "32", "gcc-4.3", "opt", "i686-slc5-gcc43"])
    tag("i686-slc5-gcc43-dbg", content=["Linux", "slc5", "32", "gcc-4.3", "debug", "i686-slc5-gcc43"])

    tag("x86_64-slc5-gcc43-opt", content=["Linux", "slc5", "64", "gcc-4.3", "opt", "x86_64-slc5-gcc43"])
    tag("x86_64-slc5-gcc43-dbg", content=["Linux", "slc5", "64", "gcc-4.3", "debug", "x86_64-slc5-gcc43"])

    tag("x86_64-slc5-gcc45-opt", content=["Linux", "slc5", "64", "gcc-4.5", "opt", "x86_64-slc5-gcc45"])
    tag("x86_64-slc5-gcc45-dbg", content=["Linux", "slc5", "64", "gcc-4.5", "debug", "x86_64-slc5-gcc45"])

    tag("x86_64-slc5-gcc46-opt", content=["Linux", "slc5", "64", "gcc-4.6", "opt", "x86_64-slc5-gcc46"])
    tag("x86_64-slc5-gcc46-dbg", content=["Linux", "slc5", "64", "gcc-4.6", "debug", "x86_64-slc5-gcc46"])

    tag("x86_64-slc5-gcc47-opt", content=["Linux", "slc5", "64", "gcc-4.7", "opt", "x86_64-slc5-gcc47"])
    tag("x86_64-slc5-gcc47-dbg", content=["Linux", "slc5", "64", "gcc-4.7", "debug", "x86_64-slc5-gcc47"])

    tag("x86_64-slc5-icc13-opt", content=["Linux", "slc5", "64", "icc13", "gcc46", "opt", "x86_64-slc5-icc13"])
    tag("x86_64-slc5-icc13-dbg", content=["Linux", "slc5", "64", "icc13", "gcc46", "debug", "x86_64-slc5-icc13"])

    tag("slc5&gcc-3.4", content=["slc5-gcc34"])
    tag("slc5&gcc-4.1", content=["slc5-gcc41"])
    tag("slc5&gcc-4.3", content=["slc5-gcc43"])
    tag("slc5&gcc-4.5", content=["slc5-gcc45"])
    tag("slc5&gcc-4.6", content=["slc5-gcc46"])

    tag("gcc-3.4", content=["gcc34x", "gcc34", "gcc"])
    tag("gcc-4.1", content=["gcc41x", "gcc41", "gcc"])
    tag("gcc-4.3", content=["gcc43x", "gcc43", "gcc"])
    tag("gcc-4.5", content=["gcc45x", "gcc45", "gcc"])
    tag("gcc-4.6", content=["gcc46x", "gcc46", "gcc"])
    tag("gcc-4.7", content=["gcc47x", "gcc47", "gcc"])

    tag("slc50", content=["host-slc5"])
    tag("slc51", content=["host-slc5"])
    tag("slc52", content=["host-slc5"])
    tag("slc53", content=["host-slc5"])
    tag("slc54", content=["host-slc5"])
    tag("slc55", content=["host-slc5"])
    tag("slc56", content=["host-slc5"])
    tag("slc57", content=["host-slc5"])
    tag("slc58", content=["host-slc5"])
    tag("slc59", content=["host-slc5"])

    tag("rhel5",  content=["host-slc5"])
    tag("rhel50", content=["host-slc5"])
    tag("rhel51", content=["host-slc5"])
    tag("rhel52", content=["host-slc5"])
    tag("rhel53", content=["host-slc5"])
    tag("rhel54", content=["host-slc5"])
    tag("rhel55", content=["host-slc5"])
    tag("rhel56", content=["host-slc5"])
    tag("rhel57", content=["host-slc5"])
    tag("rhel58", content=["host-slc5"])
    tag("rhel59", content=["host-slc5"])

    tag("sl50", content=["host-slc5"])
    tag("sl51", content=["host-slc5"])
    tag("sl52", content=["host-slc5"])
    tag("sl53", content=["host-slc5"])
    tag("sl54", content=["host-slc5"])
    tag("sl55", content=["host-slc5"])
    tag("sl56", content=["host-slc5"])
    tag("sl57", content=["host-slc5"])
    tag("sl58", content=["host-slc5"])
    tag("sl59", content=["host-slc5"])

    # ---------- Scientific Linux CERN 6 ----------
    tag("i686-slc6-gcc43-opt", content=["Linux", "slc6", "32", "gcc-4.3", "opt", "i686-slc6-gcc43"])
    tag("i686-slc6-gcc43-dbg", content=["Linux", "slc6", "32", "gcc-4.3", "debug", "i686-slc6-gcc43"])

    tag("i686-slc6-gcc47-opt", content=["Linux", "slc6", "32", "gcc-4.7", "opt", "i686-slc6-gcc47"])
    tag("i686-slc6-gcc48-opt", content=["Linux", "slc6", "32", "gcc-4.8", "opt", "i686-slc6-gcc48"])

    tag("x86_64-slc6-gcc43-opt", content=["Linux", "slc6", "64", "gcc-4.3", "opt", "x86_64-slc6-gcc43"])
    tag("x86_64-slc6-gcc43-dbg", content=["Linux", "slc6", "64", "gcc-4.3", "debug", "x86_64-slc6-gcc43"])

    tag("x86_64-slc6-gcc45-opt", content=["Linux", "slc6", "64", "gcc-4.5", "opt", "x86_64-slc6-gcc45"])
    tag("x86_64-slc6-gcc45-dbg", content=["Linux", "slc6", "64", "gcc-4.5", "debug", "x86_64-slc6-gcc45"])

    tag("x86_64-slc6-gcc46-opt", content=["Linux", "slc6", "64", "gcc-4.6", "opt", "x86_64-slc6-gcc46"])
    tag("x86_64-slc6-gcc46-dbg", content=["Linux", "slc6", "64", "gcc-4.6", "debug", "x86_64-slc6-gcc46"])

    tag("x86_64-slc6-gcc47-opt", content=["Linux", "slc6", "64", "gcc-4.7", "opt", "x86_64-slc6-gcc47"])
    tag("x86_64-slc6-gcc47-dbg", content=["Linux", "slc6", "64", "gcc-4.7", "debug", "x86_64-slc6-gcc47"])

    tag("x86_64-slc6-gcc48-opt", content=["Linux", "slc6", "64", "gcc-4.8", "opt", "x86_64-slc6-gcc48"])
    tag("x86_64-slc6-gcc48-dbg", content=["Linux", "slc6", "64", "gcc-4.8", "debug", "x86_64-slc6-gcc48"])

    tag("x86_64-slc6-clang32-opt", content=["Linux", "slc6", "64", "clang32", "gcc46", "opt", "x86_64-slc6-clang32"])
    tag("x86_64-slc6-clang32-dbg", content=["Linux", "slc6", "64", "clang32", "gcc46", "debug", "x86_64-slc6-clang32"])

    tag("x86_64-slc6-clang33-opt", content=["Linux", "slc6", "64", "clang33", "gcc48", "opt", "x86_64-slc6-clang33"])
    tag("x86_64-slc6-clang33-dbg", content=["Linux", "slc6", "64", "clang33", "gcc48", "debug", "x86_64-slc6-clang33"])

    tag("x86_64-slc6-icc13-opt", content=["Linux", "slc6", "64", "icc13", "gcc46", "opt", "x86_64-slc5-icc13"])
    tag("x86_64-slc6-icc13-dbg", content=["Linux", "slc6", "64", "icc13", "gcc46", "debug", "x86_64-slc5-icc13"])

    tag("slc6&gcc-4.3", content=["slc6-gcc43"])
    tag("slc6&gcc-4.5", content=["slc6-gcc45"])
    tag("slc6&gcc-4.6", content=["slc6-gcc46"])
    
    tag("gcc-4.3", content=["gcc43x", "gcc43", "gcc"])
    tag("gcc-4.5", content=["gcc45x", "gcc45", "gcc"])
    tag("gcc-4.6", content=["gcc46x", "gcc46", "gcc"])
    
    tag("slc60", content=["host-slc6"])
    tag("slc61", content=["host-slc6"])
    tag("slc62", content=["host-slc6"])
    tag("slc63", content=["host-slc6"])
    tag("slc64", content=["host-slc6"])
    tag("slc65", content=["host-slc6"])
    tag("slc66", content=["host-slc6"])
    tag("slc67", content=["host-slc6"])
    tag("slc68", content=["host-slc6"])
    tag("slc69", content=["host-slc6"])
    
    tag("rhel6",  content=["host-slc6"])
    tag("rhel60", content=["host-slc6"])
    tag("rhel61", content=["host-slc6"])
    tag("rhel62", content=["host-slc6"])
    tag("rhel63", content=["host-slc6"])
    tag("rhel64", content=["host-slc6"])
    tag("rhel65", content=["host-slc6"])
    tag("rhel66", content=["host-slc6"])
    tag("rhel67", content=["host-slc6"])
    tag("rhel68", content=["host-slc6"])
    tag("rhel69", content=["host-slc6"])
    
    tag("sl60", content=["host-slc6"])
    tag("sl61", content=["host-slc6"])
    tag("sl62", content=["host-slc6"])
    tag("sl63", content=["host-slc6"])
    tag("sl64", content=["host-slc6"])
    tag("sl65", content=["host-slc6"])
    tag("sl66", content=["host-slc6"])
    tag("sl67", content=["host-slc6"])
    tag("sl68", content=["host-slc6"])
    tag("sl69", content=["host-slc6"])

    # ------------------ Special Platform tags ------------
    tag("CERNDISTCC", content=["CERN", "distcc"])

    # setup overrides from SLES9
    tag("sles9", content=["SLES9"])

    # ---------- Mac OS X ----------
    
    tag("i386-mac105-gcc40-dbg", content=["Darwin", "i386-mac", "target-mac105", "gcc-4.0", "debug", "i386-osx105-gcc40"])
    tag("i386-mac105-gcc40-opt", content=["Darwin", "i386-mac", "target-mac105", "gcc-4.0", "opt", "i386-osx105-gcc40"])
    tag("i386-mac106-gcc40-dbg", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.0", "debug", "i386-osx106-gcc40"])
    tag("i386-mac106-gcc40-opt", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.0", "opt", "i386-osx106-gcc40"])
    tag("i386-mac106-gcc42-dbg", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.2", "debug", "i386-osx106-gcc42"])
    tag("i386-mac106-gcc42-opt", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.2", "opt", "i386-osx106-gcc42"])
    tag("x86_64-mac106-gcc42-dbg", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.2", "debug", "x86_64-osx106-gcc42", "64"])

    tag("x86_64-mac106-gcc42-opt", content=["Darwin", "i386-mac", "target-mac106", "gcc-4.2", "opt", "x86_64-osx106-gcc42", "64"])
    tag("mac105&gcc-4.0", content=["mac105-gcc40"])
    tag("mac105", content=["host-mac105"])
    tag("mac106", content=["host-mac106"])
    tag("mac107", content=["host-mac107"])
    
    tag("32", content=["target-32"])
    tag("64", content=["target-64"])

    # gcc compatibility tags
    tag("gcc-4.0", content=["gcc-4", "gcc"])
    tag("gcc-4.1", content=["gcc-4", "gcc"])
    tag("gcc-4.3", content=["gcc-4", "gcc"])

    tag("icc-8.0", content=["icc-8", "icc"])
    tag("icc-8.1", content=["icc-8", "icc"])

    tag("xlc-6.0", content=["xlc-6", "xlc"])

    tag("debug", content=["dbg"])


    # Setup platform-specific linker flags 

    # The notAsNeeded tag is designed to allow individual packages to override the asNeeded tag
    # and is also applied globally for MacOSX and SLES9 since they don't support these linkopts
    ctx.hwaf_declare_macro("as_needed_macro", (
      {"default": "asNeeded"},
      {"notAsNeeded": "notAsNeeded"},
      {"Darwin": "asNeeded"},
      {"SLES9": "notAsNeeded"},
    ))

    tag("asNeeded", content=[])
    tag("notAsNeeded", content=[])


    ctx.hwaf_apply_tag("${as_needed_macro}")
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    return # build

## EOF ##
