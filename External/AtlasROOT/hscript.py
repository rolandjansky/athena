## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/AtlasROOT",
    "authors": ["Alexander Undrus <undrus@bnl.gov>"],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("External/ExternalPolicy", version="ExternalPolicy-*", public=True)
    ctx.use_pkg("LCG_Interfaces/ROOT", version="v*", public=True)
    # long time deprecated
    #ctx.use_pkg("External/AtlasShift", version="", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):
    
    ## FIXME:
    ctx.hwaf_declare_tag("ROOTBasicLibs", content=[])
    
    #### macro &{{use_shift [{default []} {ROOTRFIOLibs [AtlasShift AtlasShift-00-* External]}]}}
    ctx.hwaf_declare_macro("use_shift", (
      {"default": ""},
      {"ROOTRFIOLibs": ["AtlasShift", "AtlasShift-00-*", "External"]},
    ))
    #### macro &{{rootBasicLibs [{default []} {ROOTBasicLibs [-L${ROOT_home}/lib -lCore -lCint -lTree -lpthread -lMathCore -lHist]}]}}
    ctx.hwaf_declare_macro("rootBasicLibs", (
      {"default": ""},
      {"ROOTBasicLibs": ["-L${ROOT_home}/lib", "-lCore", "-lCint", "-lTree", "-lpthread", "-lMathCore", "-lHist"]},
    ))
    #### macro &{{rootRFIOLibs [{default []} {ROOTRFIOLibs [-L${ROOT_home}/lib -lRFIO]}]}}
    ctx.hwaf_declare_macro("rootRFIOLibs", (
      {"default": ""},
      {"ROOTRFIOLibs": ["-L${ROOT_home}/lib", "-lRFIO"]},
    ))
    #### macro &{{rootGenVectorLibs [{default []} {ROOTGenVectorLibs [-L${ROOT_home}/lib -lGenVector]}]}}
    ctx.hwaf_declare_macro("rootGenVectorLibs", (
      {"default": ""},
      {"ROOTGenVectorLibs": ["-L${ROOT_home}/lib", "-lGenVector"]},
    ))
    #### macro &{{rootMathLibs [{default []} {ROOTMathLibs [-L${ROOT_home}/lib -lMathMore -lMinuit -lMinuit2 -lMatrix -lPhysics -lHistPainter -lHist -lRint]}]}}
    ctx.hwaf_declare_macro("rootMathLibs", (
      {"default": ""},
      {"ROOTMathLibs": ["-L${ROOT_home}/lib", "-lMathMore", "-lMinuit", "-lMinuit2", "-lMatrix", "-lPhysics", "-lHistPainter", "-lHist", "-lRint"]},
    ))
    #### macro &{{rootGraphicsLibs [{default []} {ROOTGraphicsLibs [-L${ROOT_home}/lib -lGraf -lGraf3d -lGpad -lHtml -lPostscript -lGui -lGX11TTF -lGX11]}]}}
    ctx.hwaf_declare_macro("rootGraphicsLibs", (
      {"default": ""},
      {"ROOTGraphicsLibs": ["-L${ROOT_home}/lib", "-lGraf", "-lGraf3d", "-lGpad", "-lHtml", "-lPostscript", "-lGui", "-lGX11TTF", "-lGX11"]},
    ))
    #### macro &{{rootTableLibs [{default []} {ROOTTableLibs [-L${ROOT_home}/lib -lTable]}]}}
    ctx.hwaf_declare_macro("rootTableLibs", (
      {"default": ""},
      {"ROOTTableLibs": ["-L${ROOT_home}/lib", "-lTable"]},
    ))
    #### macro &{{rootRooFitLibs [{default []} {ROOTRooFitLibs [-L${ROOT_home}/lib -lRooFitCore -lRooFit]}]}}
    ctx.hwaf_declare_macro("rootRooFitLibs", (
      {"default": ""},
      {"ROOTRooFitLibs": ["-L${ROOT_home}/lib", "-lRooFitCore", "-lRooFit"]},
    ))
    #### macro &{{rootSTLDictLibs [{default []} {ROOTSTLDictLibs []}]}}
    ctx.hwaf_declare_macro("rootSTLDictLibs", (
      {"default": ""},
      {"ROOTSTLDictLibs": ""},
    ))
    #### apply_tag &{{ROOTBasicLibs []}}
    ctx.hwaf_apply_tag("ROOTBasicLibs")
    #### macro &{{ROOT_linkopts [{default [${rootRooFitLibs} ${rootMathLibs} ${rootBasicLibs} ${rootRFIOLibs} ${rootGraphicsLibs} ${rootTableLibs} ${rootSTLDictLibs}]}]}}
    ctx.hwaf_declare_macro("ROOT_linkopts", (
      {"default": ["${rootRooFitLibs}", "${rootMathLibs}", "${rootGenVectorLibs}", "${rootBasicLibs}", "${rootRFIOLibs}", "${rootGraphicsLibs}", "${rootTableLibs}", "${rootSTLDictLibs}"]},
    ))
    ##### **** statement *hlib.PatternStmt (&{have_root_headers macro root_headers -s=$(<package>_root)/<package> <root_headers> ; macro dict $(bin)dict/ ; macro extra_root_cint_includes <extra_includes> ; macro_append extra_root_cint_includes  Darwin -I${ROOTSYS}/cint/cint/include -I${ROOTSYS}/cint/cint/lib -I${ROOTSYS}/cint/cint/stl ; macro root_cint_includes -I.. -I${ROOTSYS}/include ${extra_root_cint_includes} ; document root_cint <headers_lib>Dict ${root_headers} ; include_dirs . ; library <headers_lib> $(dict)<headers_lib>Dict.cxx ; macro_append <headers_lib>_dependencies <headers_lib>Dict ;})
    ##### **** statement *hlib.MakeFragmentStmt (&{root_cint})
    #### macro &{{rootcint [{default [rootcint]}]}}
    ctx.hwaf_declare_macro("rootcint", (
      {"default": "rootcint"},
    ))
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    
    
    return # build

## EOF ##
