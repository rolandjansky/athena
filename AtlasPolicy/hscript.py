# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "AtlasPolicy",
    "authors": ["Christian Arnault <arnault@lal.in2p3.fr>"],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("External/ExternalPolicy", version="ExternalPolicy-*", public=True)
    ctx.use_pkg("AtlasCxxPolicy", version="AtlasCxxPolicy-*", public=True)
    ctx.use_pkg("AtlasCommonPolicy", version="AtlasCommonPolicy-*", public=True)
    if not ctx.hwaf_enabled_tag("xaod-standalone"):
        ctx.use_pkg("GaudiPolicy", version="v*", public=True)
        pass
    #ctx.use_pkg("Tools/CodeCheck", version="CodeCheck-*", public=True)
    #ctx.use_pkg("Tools/AtlasDoxygen", version="AtlasDoxygen-*", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):

    ## special support for XAOD standalone mode.
    if ctx.hwaf_enabled_tag("xaod-standalone"):
        ctx.hwaf_macro_append("DEFINES", "XAOD_STANDALONE")
        pass
    
    ## FIXME:
    ctx.hwaf_declare_tag("do_genconf", content=[])
    ctx.hwaf_declare_tag("HasAthenaRunTime", content=[])

    ## FIXME: review mapping CMT -> hwaf

    tag = ctx.hwaf_declare_tag
    macro = ctx.hwaf_declare_macro
    
    #### tag &{dbg [debug]}
    tag(
        "dbg",
        content=["debug"]
    )
    #### tag &{debug [dbg]}
    tag(
        "debug",
        content=["dbg"]
    )
    #### tag &{opt [optimized]}
    tag(
        "opt",
        content=["optimized"]
    )
    #### tag &{optimized [opt]}
    tag(
        "optimized",
        content=["opt"]
    )
    #### tag &{prof [profiled]}
    tag(
        "prof",
        content=["profiled"]
    )
    #### tag &{profiled [prof]}
    tag(
        "profiled",
        content=["prof"]
    )
    #### tag &{pro [profiled]}
    tag(
        "pro",
        content=["profiled"]
    )
    #### tag &{profiled [pro]}
    tag(
        "profiled",
        content=["pro"]
    )
    #### tag &{Linux [Unix]}
    tag(
        "Linux",
        content=["Unix"]
    )
    #### tag &{Solaris [Unix]}
    tag(
        "Solaris",
        content=["Unix"]
    )
    #### tag &{Darwin [Unix]}
    tag(
        "Darwin",
        content=["Unix"]
    )
    #### tag &{Unix&static [UnixStatic]}
    tag(
        "Unix&static",
        content=["UnixStatic"]
    )
    #### apply_tag &{{do_genconf []}}
    ctx.hwaf_apply_tag("do_genconf")
    #### macro_remove &{{componentshr_linkopts [{default [-Wl,-Bsymbolic]}]}}
    ctx.hwaf_macro_remove("componentshr_linkopts", (
      {"default": ["-Wl,-Bsymbolic"]},
    ))
    #### macro_remove &{{componentshr_linkopts [{default [-Wl,-s]}]}}
    ctx.hwaf_macro_remove("componentshr_linkopts", (
      {"default": ["-Wl,-s"]},
    ))
    #### macro_remove &{{componentshr_linkopts [{default [-Wl,--no-undefined]} {notAsNeeded [-Wl,--no-undefined]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("componentshr_linkopts", (
      {"default": ["-Wl,--no-undefined"]},
      {"notAsNeeded": ["-Wl,--no-undefined"]},
      {"asNeeded": ""},
    ))
    #### macro_remove &{{componentshr_linkopts [{default [-Wl,--as-needed]} {notAsNeeded [-Wl,--as-needed]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("componentshr_linkopts", (
      {"default": ["-Wl,--as-needed"]},
      {"notAsNeeded": ["-Wl,--as-needed"]},
      {"asNeeded": ""},
    ))
    #### macro_remove &{{libraryshr_linkopts [{default [-Wl,--no-undefined]} {notAsNeeded [-Wl,--no-undefined]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("libraryshr_linkopts", (
      {"default": ["-Wl,--no-undefined"]},
      {"notAsNeeded": ["-Wl,--no-undefined"]},
      {"asNeeded": ""},
    ))
    #### macro_remove &{{libraryshr_linkopts [{default [-Wl,--as-needed]} {notAsNeeded [-Wl,--as-needed]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("libraryshr_linkopts", (
      {"default": ["-Wl,--as-needed"]},
      {"notAsNeeded": ["-Wl,--as-needed"]},
      {"asNeeded": ""},
    ))
    #### macro_remove &{{cpplinkflags [{default [-Wl,--no-undefined]} {notAsNeeded [-Wl,--no-undefined]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("cpplinkflags", (
      {"default": ["-Wl,--no-undefined"]},
      {"notAsNeeded": ["-Wl,--no-undefined"]},
      {"asNeeded": ""},
    ))
    #### macro_remove &{{cpplinkflags [{default [-Wl,--as-needed]} {notAsNeeded [-Wl,--as-needed]} {asNeeded []}]}}
    ctx.hwaf_macro_remove("cpplinkflags", (
      {"default": ["-Wl,--as-needed"]},
      {"notAsNeeded": ["-Wl,--as-needed"]},
      {"asNeeded": ""},
    ))
    #### macro_append &{{cppflags [{default []} {HIVE [-DATHENAHIVE]}]}}
    ctx.hwaf_macro_append("cppflags", (
      {"default": ""},
      {"HIVE": "-DATHENAHIVE"},
    ))
    #### macro_append &{{reflex_dict_options_cppflags [{default []} {HIVE [-DATHENAHIVE]}]}}
    ctx.hwaf_macro_append("reflex_dict_options_cppflags", (
      {"default": ""},
      {"HIVE": "-DATHENAHIVE"},
    ))
    #### macro_remove &{{componentshr_linkopts [{default []} {Darwin&allowUndefined []} {allowUndefined [-Wl,--no-undefined]}]}}
    ctx.hwaf_macro_remove("componentshr_linkopts", (
      {"default": ""},
      {("Darwin", "allowUndefined"): ""},
      {"allowUndefined": ["-Wl,--no-undefined"]},
    ))
    #### macro_remove &{{libraryshr_linkopts [{default []} {Darwin&allowUndefined []} {allowUndefined [-Wl,--no-undefined]}]}}
    ctx.hwaf_macro_remove("libraryshr_linkopts", (
      {"default": ""},
      {("Darwin", "allowUndefined"): ""},
      {"allowUndefined": ["-Wl,--no-undefined"]},
    ))
    #### macro_remove &{{cpplinkflags [{default []} {Darwin&allowUndefined []} {allowUndefined [-Wl,--no-undefined]}]}}
    ctx.hwaf_macro_remove("cpplinkflags", (
      {"default": ""},
      {("Darwin", "allowUndefined"): ""},
      {"allowUndefined": ["-Wl,--no-undefined"]},
    ))
    #### macro_remove &{{shlibflags [{default []} {Darwin&allowUndefined []} {Darwin [-undefined suppress]}]}}
    ctx.hwaf_macro_remove("shlibflags", (
      {"default": ""},
      {("Darwin", "allowUndefined"): ""},
      {"Darwin": ["-undefined", "suppress"]},
    ))
    #### macro_append &{{shlibflags [{default []} {Darwin&allowUndefined []} {Darwin [-undefined error]}]}}
    ctx.hwaf_macro_append("shlibflags", (
      {"default": ""},
      {("Darwin", "allowUndefined"): ""},
      {"Darwin": ["-undefined", "error"]},
    ))
    #### macro &{{asNeeded_linkopt [{default []} {Darwin []} {notAsNeeded []} {asNeeded [-Wl,--as-needed]}]}}
    macro("asNeeded_linkopt", (
      {"default": ""},
      {"Darwin": ""},
      {"notAsNeeded": ""},
      {"asNeeded": ["-Wl,--as-needed"]},
    ))
    #### macro &{{notAsNeeded_linkopt [{default []} {Darwin []} {notAsNeeded []} {asNeeded [-Wl,--no-as-needed]}]}}
    macro("notAsNeeded_linkopt", (
      {"default": ""},
      {"Darwin": ""},
      {"notAsNeeded": ""},
      {"asNeeded": ["-Wl,--no-as-needed"]},
    ))
    #### macro_append &{{componentshr_linkopts [{default []} {debug []} {separate-debug&target-opt []} {Linux [-Wl,-s]}]}}
    ctx.hwaf_macro_append("componentshr_linkopts", (
      {"default": ""},
      {"debug": ""},
      {("separate-debug", "target-opt"): ""},
      {"Linux": ["-Wl,-s"]},
    ))
    #### macro_append &{{componentshr_linkopts [{default []} {slc5&64 [-Wl,-z,max-page-size=0x1000]}]}}
    ctx.hwaf_macro_append("componentshr_linkopts", (
      {"default": ""},
      {("slc5", "64"): ["-Wl,-z,max-page-size=0x1000"]},
    ))
    #### macro_append &{{libraryshr_linkopts [{default []} {slc5&64 [-Wl,-z,max-page-size=0x1000]}]}}
    ctx.hwaf_macro_append("libraryshr_linkopts", (
      {"default": ""},
      {("slc5", "64"): ["-Wl,-z,max-page-size=0x1000"]},
    ))
    #### macro_append &{{cpplinkflags [{default []} {Linux [-Wl,--export-dynamic]}]}}
    ctx.hwaf_macro_append("cpplinkflags", (
      {"default": ""},
      {"Linux": ["-Wl,--export-dynamic"]},
    ))
    #### macro_append &{{cpplinkflags [{default []} {Linux [-Wl,--allow-shlib-undefined]}]}}
    ctx.hwaf_macro_append("cpplinkflags", (
      {"default": ""},
      {"Linux": ["-Wl,--allow-shlib-undefined"]},
    ))
    #### macro_append &{{cpplinkflags [{default []} {host-mac106&target-mac105 [-lstdc++ -lgcc_s.10.4]}]}}
    ctx.hwaf_macro_append("cpplinkflags", (
      {"default": ""},
      {("host-mac106", "target-mac105"): ["-lstdc++", "-lgcc_s.10.4"]},
    ))
    #### macro_append &{{componentshr_linkopts [{default []} {Linux [-Wl,--hash-style=both]}]}}
    ctx.hwaf_macro_append("componentshr_linkopts", (
      {"default": ""},
      {"Linux": ["-Wl,--hash-style=both"]},
    ))
    #### macro_append &{{libraryshr_linkopts [{default []} {Linux [-Wl,--hash-style=both]}]}}
    ctx.hwaf_macro_append("libraryshr_linkopts", (
      {"default": ""},
      {"Linux": ["-Wl,--hash-style=both"]},
    ))
    #### macro_append &{{cpplinkflags [{default []} {Linux [-Wl,--hash-style=both]}]}}
    ctx.hwaf_macro_append("cpplinkflags", (
      {"default": ""},
      {"Linux": ["-Wl,--hash-style=both"]},
    ))
    ##### **** statement *hlib.PatternStmt (&{default_include_path -global macro <package>_include_dirs $(<package>_cmtpath)/$(<project>_installarea_prefix)/include/<package> <project>_without_installarea $(<PACKAGE>ROOT) ; private ; macro <package>_include_dirs $(<PACKAGE>ROOT) ; end_private ; include_dirs ${<package>_include_dirs} ; document install_includes install_includes})
    ##### **** statement *hlib.PatternStmt (&{default_rpath path_remove LD_LIBRARY_PATH /<package>/})
    ##### **** statement *hlib.PatternStmt (&{default_library_path path_remove LD_LIBRARY_PATH /<package>/})
    ##### **** statement *hlib.PatternStmt (&{default_linkopts macro <package>_linkopts -L$(<PACKAGE>ROOT)/$(<package>_tag) -l<package> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a <extras> ; apply_pattern default_rpath})
    ##### **** statement *hlib.PatternStmt (&{default_no_share_linkopts macro <package>_linkopts -L$(<PACKAGE>ROOT)/$(<package>_tag) -l<package> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a <extras>})
    ##### **** statement *hlib.PatternStmt (&{installed_linkopts macro_append <package>_library_dependencies <package> ; private ; macro <package>_shlibflags  notAsNeeded  asNeeded $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<package>_use_linkopts) $(<package>_extra_shlibflags) ; macro_remove <package>_use_linkopts  notAsNeeded  asNeeded $(<package>_linkopts) ; end_private ; macro <package>_linkopts -l<package> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a <extras>})
    ##### **** statement *hlib.PatternStmt (&{default_library library <package> *.cxx ; apply_pattern default_linkopts ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{default_installed_library library <package> *.cxx ; apply_pattern installed_library ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{no_include_path ignore_pattern default_include_path ; include_path none})
    ##### **** statement *hlib.PatternStmt (&{include_path ignore_pattern default_include_path ; include_dirs <extras>})
    ##### **** statement *hlib.PatternStmt (&{declare_joboptions apply_pattern generic_declare_for_link kind=joboptions files='-s=../share <files> prefix=jobOptions/<package> name=<name>})
    ##### **** statement *hlib.PatternStmt (&{declare_python_modules apply_pattern generic_declare_for_link kind=python_modules files='-s=../python <files> prefix=python/<package> name=<name> ; private ; macro_append <package>_python_init_dependencies install_<name>python_modules ; end_private})
    ##### **** statement *hlib.PatternStmt (&{declare_runtime apply_pattern generic_declare_for_link kind=runtime files='-s=../share <files> <extras> prefix=share name=<name>})
    ##### **** statement *hlib.PatternStmt (&{declare_runtime_extras apply_pattern declare_runtime files="<files> extras="<extras> name="<name>})
    ##### **** statement *hlib.PatternStmt (&{install_runtime macro data $(use_runtime) ; document get_files <name>get_files install_dir=../run/ copymethod=<method>})
    ##### **** statement *hlib.PatternStmt (&{declare_scripts apply_pattern generic_declare_for_link kind=scripts files='-s=../share <files> prefix=share/bin name=<name>})
    ##### **** statement *hlib.PatternStmt (&{declare_xmls apply_pattern generic_declare_for_link kind=xmls files='-s=../data *.dtd *.xml <extras> prefix=XML/<package> name=<name>})
    ##### **** statement *hlib.PatternStmt (&{install_xmls })
    ##### **** statement *hlib.PatternStmt (&{declare_java macro_append <package>_java $(<package>_root)/java/*.jar ; macro_append use_java $(<package>_java)})
    ##### **** statement *hlib.PatternStmt (&{install_java path_remove CLASSPATH /<package>/java ; path_prepend CLASSPATH ${<PACKAGE>ROOT}/java ; apply_pattern generic_declare_for_link kind=java files='-s=../java *.jar prefix=java name=<name>})
    ##### **** statement *hlib.PatternStmt (&{declare_docs apply_pattern generic_declare_for_link kind=docs files='-s=../doc <files> prefix=doc/<package> name=<name>})
    ##### **** statement *hlib.MakeFragmentStmt (&{get_files})
    ##### **** statement *hlib.PatternStmt (&{get_files macro <name>jo <jo> ; macro <name>scripts <scripts> ; macro <name>data <data> ; macro <name>xmls <xmls> ; document get_files <name>get_files install_dir=../run/ copymethod=<method> name=<name>})
    ##### **** statement *hlib.PatternStmt (&{shared_library macro <package>_linkopts -L$(<PACKAGE>ROOT)/$(<package>_tag) -l<package> <extras> ; path_remove LD_LIBRARY_PATH /<package>/ ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{static_library macro <package>_linkopts -L$(<PACKAGE>ROOT)/$(<package>_tag) -l<package> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a <extras> ; macro_append <package>_stamps $(<package>_root)/$(<package>_tag)/<package>.stamp ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{installed_library macro_append <package>_installed_dependencies <package> ; macro_append <package>_library_dependencies <package> ; macro_append <package>_installed_shlibflags  notAsNeeded  asNeeded -l<package> ; private ; macro <package>_shlibflags  notAsNeeded  asNeeded $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<package>_use_linkopts) $(<package>_extra_shlibflags) ; macro_remove <package>_use_linkopts  notAsNeeded  asNeeded $(<package>_linkopts) ; end_private ; macro_append <package>_linkopts -l<package> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a <extras> ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{named_installed_library macro_append <package>_library_dependencies <library> ; macro_append <library>_dependencies $(<package>_installed_dependencies) ; private ; macro <library>_installed_shlibflags  notAsNeeded  asNeeded $(<package>_installed_shlibflags) ; macro <library>_shlibflags  notAsNeeded  asNeeded $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<library>_installed_shlibflags) $(<library>_use_linkopts) ; macro_remove <library>_use_linkopts  notAsNeeded  asNeeded $(<package>_linkopts) ; end_private ; macro_append <package>_linkopts -l<library> <extras> UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<library>.a <extras> ; apply_pattern optdebug_library name=<library>})
    ##### **** statement *hlib.PatternStmt (&{component_interface })
    ##### **** statement *hlib.PatternStmt (&{component_library_no_genCLIDDB macro_append <package>_library_dependencies <package> ; macro_append <package>_linkopts  UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a -u <package>_loadRef ; macro <package>_shlibflags $(componentshr_linkopts) $(<package>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<package> ; private ; use GaudiKernel v* -no_auto_imports ; macro dummy_for_library <library> ; macro <package>_genconfig_cmd generate_configurables no_genconf do_null_genconfig ; apply_pattern $(<package>_genconfig_cmd) library=<package> ; apply_pattern generate_rootmap library=<package> group=<group> ; end_private ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{component_library apply_pattern component_library_no_genCLIDDB library=<library> group=<group> ; private ; apply_pattern do_genCLIDDB library=<package> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{named_component_library macro_append <package>_library_dependencies <library> ; macro_append <package>_linkopts  UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<library>.a -u <library>_loadRef ; macro <library>_shlibflags $(componentshr_linkopts) $(<library>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<library> ; macro_append <package>_named_libraries <library> ; private ; use GaudiKernel v* -no_auto_imports ; apply_pattern generate_configurables library=<library> ; apply_pattern generate_rootmap library=<library> group=<group> ; apply_pattern do_genCLIDDB library=<library> ; end_private ; apply_pattern optdebug_library name=<library>})
    ##### **** statement *hlib.PatternStmt (&{linked_library macro_append <package>_library_dependencies <package> ; macro_append <package>_linkopts  UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a -u <package>_loadRef ; macro <package>_shlibflags $(componentshr_linkopts) $(<package>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<package> ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{named_linked_library macro_append <package>_library_dependencies <library> ; macro_append <package>_linkopts  UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<library>.a -u <library>_loadRef ; macro <library>_shlibflags $(componentshr_linkopts) $(<library>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<library> ; macro_append <package>_named_libraries <library> ; apply_pattern optdebug_library name=<library>})
    ##### **** statement *hlib.PatternStmt (&{dual_use_library macro <package>Lib_files <files> ; library <package>Lib -import=<imports> $(<package>Lib_files) ; library <package> -import=<imports> components/*.cxx ; macro_append <package>_library_dependencies <package> <package>Lib ; macro_append <package>_dependencies <package>Lib ; macro_append <package>stamps $(<PACKAGE>ROOT)/$(<package>_tag)/<package>Lib.stamp ; macro_append <package>_linkopts -l<package>Lib UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>.a -u <package>_loadRef $(<PACKAGE>ROOT)/$(<package>_tag)/lib<package>Lib.a ; private ; macro <package>Lib_shlibflags  notAsNeeded  asNeeded $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<package>Lib_use_linkopts) ; macro_remove <package>Lib_use_linkopts  notAsNeeded  asNeeded $(<package>_linkopts) ; end_private ; macro <package>_shlibflags $(componentshr_linkopts) $(<package>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<package> ; private ; use GaudiKernel v* -no_auto_imports ; apply_pattern generate_configurables library=<package> ; apply_pattern generate_rootmap library=<package> group=<group> ; apply_pattern do_genCLIDDB library=<package> ; end_private ; apply_pattern optdebug_library name=<package> ; apply_pattern optdebug_library name=<package>Lib})
    ##### **** statement *hlib.PatternStmt (&{named_dual_use_library macro <library>Lib_files <files> ; library <library>Lib -import=<imports> $(<library>Lib_files) ; library <library> -import=<imports> components/*.cxx ; macro_append <package>_library_dependencies <library> <library>Lib ; macro_append <library>_dependencies <library>Lib ; macro_append <library>stamps $(<PACKAGE>ROOT)/$(<package>_tag)/<library>Lib.stamp ; macro_append <package>_linkopts -l<library>Lib UnixStatic $(<PACKAGE>ROOT)/$(<package>_tag)/lib<library>.a -u <library>_loadRef $(<PACKAGE>ROOT)/$(<package>_tag)/lib<library>Lib.a ; private ; macro <library>Lib_shlibflags  notAsNeeded  asNeeded $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<library>Lib_use_linkopts) ; macro_remove <library>Lib_use_linkopts  notAsNeeded  asNeeded $(<package>_linkopts) ; end_private ; macro <library>_shlibflags $(componentshr_linkopts) $(<library>_use_linkopts) ; macro_append <package>_extra_shlibflags -l<library> ; private ; use GaudiKernel v* -no_auto_imports ; macro_append <package>_named_libraries <library> apply_pattern generate_configurables library=<library> ; apply_pattern generate_rootmap library=<library> group=<group> ; apply_pattern do_genCLIDDB library=<library> ; end_private ; apply_pattern optdebug_library name=<library> ; apply_pattern optdebug_library name=<library>Lib})
    #### macro &{{abuild_gen_tpcnv_cmd [{default [${AtlasPolicy_root}/scripts/abuild-gen-tpcnvdb.py]}]}}
    macro("abuild_gen_tpcnv_cmd", (
      {"default": "${AtlasPolicy_root}/scripts/abuild-gen-tpcnvdb.py"},
    ))
    ##### **** statement *hlib.PatternStmt (&{named_tpcnv_library apply_pattern named_installed_library library=<library> ; private ; apply_pattern generate_rootmap library=<library> group=<group> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{tpcnv_library apply_pattern installed_library ; private ; apply_pattern generate_rootmap library=<package> group=<group> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{application_path path_remove PATH /<package>/})
    ##### **** statement *hlib.PatternStmt (&{application_alias alias <application> <application>.exe})
    ##### **** statement *hlib.PatternStmt (&{declare_applications })
    ##### **** statement *hlib.PatternStmt (&{install_applications })
    ##### **** statement *hlib.PatternStmt (&{static_app application <name>Static -group=static <sources> <imports>})
    ##### **** statement *hlib.PatternStmt (&{static_athena_app macro static_Athena  static AthenaCommon AthenaCommon-* Control ; use $(static_Athena) ; application <package>Static -group=static ${ATHENACOMMONROOT}/src/AthenaMain.cxx <imports>})
    ##### **** statement *hlib.PatternStmt (&{static_use_version macro use_<use>  static <use> <vers> <container> ; use $(use_<use>)})
    ##### **** statement *hlib.PatternStmt (&{static_use macro use_<use>  static <use> * <container> ; use $(use_<use>)})
    #### macro_append &{{constituents [{default []} {static [static]}]}}
    ctx.hwaf_macro_append("constituents", (
      {"default": ""},
      {"static": "static"},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{empty_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{ps2pdf})
    ##### **** statement *hlib.MakeFragmentStmt (&{tex2html})
    #### macro_append &{{MAKEFLAGS [{default [--no-print-directory]}]}}
    ctx.hwaf_macro_append("MAKEFLAGS", (
      {"default": "--no-print-directory"},
    ))
    #### tag &{insure [Linux]}
    tag(
        "insure",
        content=["Linux"]
    )
    #### tag &{insure [i386_linux22]}
    tag(
        "insure",
        content=["i386_linux22"]
    )
    #### macro &{{genCLIDDB_cmd [{default [${AtlasPolicy_root}/cmt/fragments/gencliddb.py]}]}}
    macro("genCLIDDB_cmd", (
      {"default": "${AtlasPolicy_root}/cmt/fragments/gencliddb.py"},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{genCLIDDB})
    #### macro &{{use_CLIDComps [{default [CLIDComps * Control -no_auto_imports]} {no_genCLIDDB []}]}}
    macro("use_CLIDComps", (
      {"default": ["CLIDComps", "*", "Control", "-no_auto_imports"]},
      {"no_genCLIDDB": ""},
    ))
    ##### **** statement *hlib.PatternStmt (&{do_genCLIDDB private ; use $(use_CLIDComps) ; macro run_genCLIDDB do_real_genCLIDDB no_genCLIDDB do_null_genCLIDDB ; apply_pattern $(run_genCLIDDB) library=<library> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{do_real_genCLIDDB private ; document genCLIDDB <library>CLIDDB library=<library> ; macro_append <library>CLIDDB_dependencies <library> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{do_null_genCLIDDB private ; macro dummy_for_<library>CLIDDB <library> ; end_private})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_load})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_open_entries})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_entries})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_close_entries})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_algorithm})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_service})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_converter})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_object})
    ##### **** statement *hlib.MakeFragmentStmt (&{gaudi_generator})
    ##### **** statement *hlib.PatternStmt (&{gaudi_dll_generator macro gaudi_load_model '<gaudi_load module=$(package)/>' ; macro gaudi_entries_model '<gaudi_entries module=$(package)/><gaudi_open_entries/>$(extra_entries)<gaudi_close_entries/>' ; document gaudi_generator gaudi_load model="$(gaudi_load_model) out=$(src)$(package)_load.cxx ; document gaudi_generator gaudi_entries model="$(gaudi_entries_model) out=$(src)$(package)_entries.cxx ; macro <package>_dll_dependencies gaudi_load gaudi_entries ; library <package>_dll <package>_load.cxx <package>_entries.cxx})
    #### macro &{{remote_launcher [{default [ssh -f]}]}}
    macro("remote_launcher", (
      {"default": ["ssh", "-f"]},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{launcher_sync_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{launcher_sync})
    ##### **** statement *hlib.MakeFragmentStmt (&{launcher_async_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{launcher_async})
    ##### **** statement *hlib.MakeFragmentStmt (&{libdeps})
    ##### **** statement *hlib.PatternStmt (&{libdeps document libdeps <library>deps library=<library> ; macro_append <library>_dependencies <library>deps})
    ##### **** statement *hlib.MakeFragmentStmt (&{athena_launcher_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{athena_launcher})
    ##### **** statement *hlib.PatternStmt (&{athena_test document athena_launcher <name>_test -group=check athena_exe="'../${CMTCONFIG}/athena.exe' athena_opt="<options> athena_pre="'if test -f <name>.log ; then /bin/rm -f <name>.log; fi' athena_rem="<remove> athena_out=" >& <name>.log' athena_post="'if ! test -f ../baseline/<name>.log ; then mv <name>.log ../baseline ; else diff <name>.log ../baseline/<name>.log; fi'})
    #### macro &{{cmt_install_area_command [{default [${symlink_command}]}]}}
    macro("cmt_install_area_command", (
      {"default": "${symlink_command}"},
    ))
    #### macro &{{cmt_uninstall_area_command [{default [${uninstall_command}]}]}}
    macro("cmt_uninstall_area_command", (
      {"default": "${uninstall_command}"},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{cmt_action_runner})
    ##### **** statement *hlib.MakeFragmentStmt (&{cmt_action_runner_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{cleanup_header})
    #### macro &{{dependencies_fragment [{default [dependencies]} {CMTr23 []} {CMTr22 []} {CMTr21 []} {CMTr20 []}]}}
    macro("dependencies_fragment", (
      {"default": "dependencies"},
      {"CMTr23": ""},
      {"CMTr22": ""},
      {"CMTr21": ""},
      {"CMTr20": ""},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{$(dependencies_fragment)})
    #### macro &{{make_header_fragment [{default []} {CMTr18 []} {CMTr16 [make_header]}]}}
    macro("make_header_fragment", (
      {"default": ""},
      {"CMTr18": ""},
      {"CMTr16": "make_header"},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{$(make_header_fragment)})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_includes})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_includes_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_named_includes})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_named_includes_header})
    #### macro &{{install_include_filter [{default [*.h]}]}}
    macro("install_include_filter", (
      {"default": "*.h"},
    ))
    ##### **** statement *hlib.PatternStmt (&{install_non_standard_includes document install_named_includes install_<name>_includes name=<name> ; macro_prepend constituents install_<name>_includes ; macro_append <package>_include_dirs <name>})
    #### apply_tag &{{HasAthenaRunTime []}}
    ctx.hwaf_apply_tag("HasAthenaRunTime")
    #### macro &{{AtlasPolicy_pacman_hints [{default [AtlasPolicy.pacman]}]}}
    macro("AtlasPolicy_pacman_hints", (
      {"default": "AtlasPolicy.pacman"},
    ))

    macro("GENCLIDDB_CMD", "gencliddb.py")
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    ##### **** statement *hlib.ApplyPatternStmt (&{declare_scripts [files="checkreq.py]})
    ctx(
        name     = "AtlasPolicy-install-scripts",
        features = "atlas_install_scripts",
        source   = ["share/checkreq.py",
                    "cmt/fragments/gencliddb.py",
                    ],
        )
    
    return # build

## EOF ##
