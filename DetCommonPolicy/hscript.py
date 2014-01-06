# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "DetCommonPolicy",
    "authors": ["David.Berge@cern.ch"],
    "managers": ["David.Berge@cern.ch", "Joerg.Stelzer@cern.ch"],

}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    ctx.use_pkg("LCG_Policy", version="*", public=True)
    ctx.use_pkg("LCG_Settings", version="*", public=True)
    ctx.use_pkg("AtlasCommonPolicy", version="*", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):


    ## load+export detcommon policies
    ctx.hwaf_export_module("waftools/detcommon-policy.py")
    
    macro = ctx.hwaf_declare_macro

    ##### **** statement *hlib.PatternStmt (&{detcommon_ignores ignore_pattern include_dir_policy ; ignore_pattern install_includes ; ignore_pattern install_pythonmods ; ignore_pattern library})
    ##### **** statement *hlib.PatternStmt (&{detcommon_shared_library_settings macro <package>_linkopts -l<package> ; private ; macro <package>_shlibflags $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<package>_use_linkopts) ; macro_remove <package>_use_linkopts $(<package>_linkopts) ; end_private})
    ##### **** statement *hlib.PatternStmt (&{detcommon_shared_library library <package> -no_prototypes *.cxx ; apply_pattern detcommon_shared_library_settings ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{detcommon_shared_generic_library library <package> -no_prototypes <files> ; apply_pattern detcommon_shared_library_settings ; apply_pattern optdebug_library name=<package>})
    ##### **** statement *hlib.PatternStmt (&{detcommon_shared_named_library_settings macro <package>_linkopts -l<library> ; private ; macro <library>_shlibflags $(libraryshr_linkopts) $(cmt_installarea_linkopts) $(<library>_use_linkopts) ; macro_remove <library>_use_linkopts $(<package>_linkopts) ; end_private})
    ##### **** statement *hlib.PatternStmt (&{detcommon_shared_named_generic_library library <library> -no_prototypes <files> ; apply_pattern detcommon_shared_named_library_settings library=<library> ; apply_pattern optdebug_library name=<library>})
    ##### **** statement *hlib.PatternStmt (&{trigconf_example_application application <name> <file> bindirname="bin macro_append <name>linkopts $(<package>_linkopts) -L$(CMTINSTALLAREA)/$(CMTCONFIG)/lib $(Boost_linkopts_thread)})
    ##### **** statement *hlib.PatternStmt (&{trigconf_application apply_pattern trigconf_example_application name=TrigConf<name> file=../src/test/<name>.cxx macro_append TrigConf<name>_dependencies <package>})
    ##### **** statement *hlib.PatternStmt (&{trigconf_generic_application apply_pattern trigconf_example_application name=TrigConf<name> file=../src/test/<name>.cxx macro_append TrigConf<name>_dependencies <package>})

    ctx.hwaf_macro_append("CFLAGS", (
      {"default": ["-Wwrite-strings", "-Wpointer-arith", "-Wno-long-long"]},
    ))

    ctx.hwaf_macro_append("CXXFLAGS", (
      {"default": ["-Wwrite-strings", "-Wpointer-arith", "-Woverloaded-virtual", "-Wno-long-long"]},
    ))

    ctx.hwaf_macro_append("DEFINES", (
      {"default": ""},
      {("host-slc6", "target-slc5"): "__USE_XOPEN2K8"},
    ))

    macro("JAVA_SUBDIR", (
      {"default": "sw/lcg/external/Java"},
      {"BNL": "cernsw/java"},
    ))
    ctx.hwaf_declare_runtime_env("JAVA_SUBDIR")

    macro("JAVA_BASE", (
      {"default": "${SITEROOT}/${JAVA_SUBDIR}"},
    ))
    ctx.hwaf_declare_runtime_env("JAVA_BASE")

    macro("JDK_VENDOR", (
      {"default": "SUN_"},
      {"Darwin": ""},
    ))

    macro("JDK_ARCH", (
      {"default": "ia32"},
      {"64":      "amd64"},
      {"Darwin":  "Darwin"},
    ))
    
    macro("JDK_FULL_VERSION", (
      {"default": "1.5.0_11"},
      {"Darwin":  "1.5.0"},
    ))

    macro("JDK_HOME", (
      {"default": "${JAVA_BASE}/JDK/${JDK_VENDOR}${JDK_FULL_VERSION}/${JDK_ARCH}"},
    ),override=True)
    ctx.hwaf_declare_runtime_env("JDK_HOME")

    macro("JAVA_HOME", (
      {"default": "${JDK_HOME}"},
      {"target-mac106": ""},
    ),override=True)
    ctx.hwaf_declare_runtime_env("JAVA_HOME")

    macro("_JAVA_OPTIONS", (
      {"default": ["-Xms64m", "-Xmx128m"]},
    ))
    ctx.hwaf_declare_runtime_env("_JAVA_OPTIONS")

    ctx.hwaf_path_remove("PATH", (
      {"default": "/JDK"},
    ))

    ctx.hwaf_path_append("PATH", (
      {"default": "${JDK_HOME}/jre/bin"},
      {"noJava": ""},
    ))

    ctx.hwaf_path_append("PATH", (
      {"default": "${JDK_HOME}/bin"},
      {"noJava": ""},
    ))

    macro("JavaSDK_export_paths", (
      {"default": "${JDK_HOME}"},
    ),override=True)

    macro("JavaSDK_native_version", (
      {"default": "${JDK_VENDOR}${JDK_FULL_VERSION}"},
    ), override=True)

    macro("JavaSDK_platform", (
      {"default": ""},
      {"PACK": "${JDK_ARCH}"},
    ))

    macro("ANT_VERSION", (
      {"default": "1.7.0"},
    ))

    macro("ANT_HOME", (
      {"default": "${JAVA_BASE}/Ant/${ANT_VERSION}/share"},
    ))
    ctx.hwaf_declare_runtime_env("ANT_HOME")

    ctx.hwaf_path_remove("PATH", (
      {"default": "Ant"},
    ))

    ctx.hwaf_path_prepend("PATH", (
      {"default": "${ANT_HOME}/bin"},
      {"noJava": ""},
    ))

    macro("ANT_SITE", (
      {"default": "CMT"},
    ))
    ctx.hwaf_declare_runtime_env("ANT_SITE")
    ##### **** statement *hlib.MakeFragmentStmt (&{ant})

    macro("Ant_export_paths", (
      {"default": "${ANT_HOME}"},
    ))

    macro("Ant_native_version", (
      {"default": "${ANT_VERSION}"},
    ))

    macro("cmt_installarea_prefix", (
      {"default": "InstallArea"},
    ), override=True)

    macro("application_suffix", (
      {"default": ""},
      {"block-tdaqc": ".exe"},
    ), override=True)

    macro("cmt_installarea_command", (
      {"default": ["ln", "-fs"]},
    ), override=True)
    ##### **** statement *hlib.MakeFragmentStmt (&{install_header_files})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_header_files_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{install_includes_header})
    ##### **** statement *hlib.PatternStmt (&{detcommon_header_installer ; macro <package>_include_dirs $(<package>_cmtpath)/$(<project>_installarea_prefix)/include/<package> <project>_without_installarea $(<PACKAGE>ROOT) ; private ; macro <package>_include_dirs $(<PACKAGE>ROOT) ; end_private ; include_dirs ${<package>_include_dirs} ; document install_header_files install_header_files ; include_path none})

    macro("copy_command", (
      {"default": ["cp", "-R", "-H"]},
    ))

    macro("symlink_command", (
      {"default": ["ln", "-sf"]},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{atlas_data_installer})
    ##### **** statement *hlib.PatternStmt (&{detcommon_generic_install macro_append <package>_<kind> <files> ; document atlas_data_installer install_<name><kind> prefix=<prefix> $(<package>_<kind>) cmt_install_area_command="'<install_command>'})
    ##### **** statement *hlib.PatternStmt (&{detcommon_link_files apply_pattern detcommon_generic_install kind=<kind> files='<files> prefix=<prefix> name=<name> install_command="'$(symlink_command)'})
    ##### **** statement *hlib.PatternStmt (&{detcommon_copy_files apply_pattern detcommon_generic_install kind=<kind> files='<files> prefix=<prefix> name=<name> install_command="'$(copy_command)'})
    ##### **** statement *hlib.PatternStmt (&{detcommon_install_docs apply_pattern detcommon_link_files files='-s=../doc *.* <extras> prefix=doc/<package>})
    #### tag &{static [WithStatic]}
    ctx.hwaf_declare_tag(
        "static",
        content=["WithStatic"]
    )

    ctx.hwaf_declare_tag(
        "UnixStatic",
        content=["WithStatic"]
    )

    macro("library_fragment", (
      {"default": "library"},
      {"WithStatic": ""},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{$(library_fragment)})
    ##### **** statement *hlib.MakeFragmentStmt (&{application})
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    return # build

## EOF ##
