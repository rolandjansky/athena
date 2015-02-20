## -*- python -*-

## waf imports
import waflib.Logs as msg

PACKAGE = {
    "name":    "External/ExternalPolicy",
    "authors": ["atlas collaboration"],


}

### ---------------------------------------------------------------------------
def pkg_deps(ctx):
    
    ## public dependencies
    #ctx.use_pkg("External/ExternalComponents", version="", public=True)
    ctx.use_pkg("External/PlatformPolicy", version="PlatformPolicy-*", public=True)
    #ctx.use_pkg("$(use_cmtgrapher)", version="", public=True)
    
    ## no private dependencies
    ## no runtime dependencies
    return # pkg_deps


### ---------------------------------------------------------------------------
def options(ctx):
    
    return # options


### ---------------------------------------------------------------------------
def configure(ctx):

    if not ctx.env.CMTCONFIG:
        ctx.env.CMTCONFIG = ctx.env.HWAF_VARIANT
        pass
    
    ## FIXME:
    if 1:
        return
    
    ## FIXME:
    ctx.hwaf_declare_tag("notProject", content=[])
    
    #### macro &{{isProject [{default [${CMTPROJECTPATH}notProject]}]}}
    ctx.hwaf_declare_macro("isProject", (
      {"default": "${CMTPROJECTPATH}notProject"},
    ))
    #### apply_tag &{{$(isProject) []}}
    ctx.hwaf_apply_tag("${isProject}")
    #### macro &{{useExternalComponents [{default []} {notProject [ExternalComponents ExternalComponents-00-* External]}]}}
    ctx.hwaf_declare_macro("useExternalComponents", (
      {"default": ""},
      {"notProject": ["ExternalComponents", "ExternalComponents-00-*", "External"]},
    ))
    #### apply_tag &{{LCGCMT_INSTALLED []}}
    ctx.hwaf_apply_tag("LCGCMT_INSTALLED")
    #### apply_tag &{{block-tdaqc []}}
    ctx.hwaf_apply_tag("block-tdaqc")
    ##### **** statement *hlib.PatternStmt (&{empty_include_path -global include_path none})
    #### macro &{{shlibsuffix [{default [so]}]}}
    ctx.hwaf_declare_macro("shlibsuffix", (
      {"default": "so"},
    ))
    #### macro &{{ar [{default [${ar}]} {Darwin [ar -cr]}]}}
    ctx.hwaf_declare_macro("ar", (
      {"default": "${ar}"},
      {"Darwin": ["ar", "-cr"]},
    ))
    #### macro &{{ranlib [{default [${ranlib}]} {Darwin&mac106 [touch]} {Darwin [ranlib]}]}}
    ctx.hwaf_declare_macro("ranlib", (
      {"default": "${ranlib}"},
      {("Darwin", "mac106"): "touch"},
      {"Darwin": "ranlib"},
    ))
    #### macro &{{symlink [{default [symlink.sh]}]}}
    ctx.hwaf_declare_macro("symlink", (
      {"default": "symlink.sh"},
    ))
    #### macro &{{symlink_command [{default [symlink.sh]}]}}
    ctx.hwaf_declare_macro("symlink_command", (
      {"default": "symlink.sh"},
    ))
    #### macro &{{symlinknew [{default [${symlink_command}]}]}}
    ctx.hwaf_declare_macro("symlinknew", (
      {"default": "${symlink_command}"},
    ))
    #### macro &{{copy_command [{default [cp -R -H]}]}}
    ctx.hwaf_declare_macro("copy_command", (
      {"default": ["cp", "-R", "-H"]},
    ))
    #### macro &{{symlink [{default [${ExternalPolicy_root}/src/symlink.sh]}]}}
    ctx.hwaf_declare_macro("symlink", (
      {"default": "${ExternalPolicy_root}/src/symlink.sh"},
    ))
    #### macro &{{symlink_command [{default [${ExternalPolicy_root}/src/symlink.sh]}]}}
    ctx.hwaf_declare_macro("symlink_command", (
      {"default": "${ExternalPolicy_root}/src/symlink.sh"},
    ))
    #### macro &{{atlas_install_command [{default [cp -R -H]}]}}
    ctx.hwaf_declare_macro("atlas_install_command", (
      {"default": ["cp", "-R", "-H"]},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{atlas_data_installer})
    ##### **** statement *hlib.PatternStmt (&{generic_declare macro_append <package>_<name><kind> <files> ; document atlas_data_installer install_<name><kind> prefix=<prefix> $(<package>_<name><kind>) cmt_install_area_command="'<install_command>' ; private ; action check_install_<name><kind> $(EXTERNALPOLICYROOT)/cmt/atlas_check_installations.sh -files='$(<package>_<name><kind>) -installdir=${CMTINSTALLAREA}/<prefix> -level=$(<package>_pedantic_level) ; macro install_<name><kind>_dependencies check_install_<name><kind> ; end_private})
    ##### **** statement *hlib.PatternStmt (&{generic_declare_for_link apply_pattern generic_declare kind=<kind> files='<files> prefix=<prefix> name=<name> install_command="'$(symlink_command)'})
    ##### **** statement *hlib.PatternStmt (&{generic_declare_for_copy apply_pattern generic_declare kind=<kind> files='<files> prefix=<prefix> name=<name> install_command="'$(copy_command)'})
    ##### **** statement *hlib.PatternStmt (&{install_docs apply_pattern generic_declare_for_link files='-s=../doc *.* <extras> <files> prefix=doc/<package>})
    ##### **** statement *hlib.PatternStmt (&{install_external_home apply_pattern generic_declare_for_link kind=<package>_home files=$(<package>_home) prefix=external/<package> ; macro <package>_installed_home $(<package>_cmtpath)/$(cmt_installarea_prefix)/external/<package>/`basename $(<package>_home)`})
    ##### **** statement *hlib.PatternStmt (&{install_external_named_home apply_pattern generic_declare_for_link kind=<name>_home files=$(<name>_home) prefix=external/<package>-<name> ; macro <package>-<name>_installed_home $(<package>_cmtpath)/$(cmt_installarea_prefix)/external/<package>-<name>/`basename $(<name>_home)`})
    ##### **** statement *hlib.PatternStmt (&{install_external_named_location apply_pattern generic_declare_for_link kind=<name>_loc files=<location> prefix=external/<package>-<name> ; macro <package>-<name>_installed_location $(<package>_cmtpath)/$(cmt_installarea_prefix)/external/<package>-<name>/`basename <location>`})
    ##### **** statement *hlib.PatternStmt (&{declare_external_applications macro_append use_applications <applications>})
    ##### **** statement *hlib.PatternStmt (&{declare_external_java macro_append use_java <java>})
    ##### **** statement *hlib.MakeFragmentStmt (&{readme_installer_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{readme_installer})
    #### macro &{{cmtgrapher_use [{default []} {CERN&Linux [cmtgrapher v1 CMT -no_auto_imports]}]}}
    ctx.hwaf_declare_macro("cmtgrapher_use", (
      {"default": ""},
      {("CERN", "Linux"): ["cmtgrapher", "v1", "CMT", "-no_auto_imports"]},
    ))
    #### macro &{{use_cmtgrapher [{default []} {target_cmtgrapher [${cmtgrapher_use}]}]}}
    ctx.hwaf_declare_macro("use_cmtgrapher", (
      {"default": ""},
      {"target_cmtgrapher": "${cmtgrapher_use}"},
    ))
    #### macro &{{base_URL [{default [http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/OO/dist/]}]}}
    ctx.hwaf_declare_macro("base_URL", (
      {"default": "http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/OO/dist/"},
    ))
    ##### **** statement *hlib.PatternStmt (&{readme_installer document readme_installer install_readme -group=install DOCPATH=$(<package>_doc_path)})
    ##### **** statement *hlib.MakeFragmentStmt (&{readme_trailer})
    #### apply_tag &{{ATLAS []}}
    ctx.hwaf_apply_tag("ATLAS")
    #### macro &{{SRT_TARGET [{default []} {egcs-1.1.2 [i586-pc-linux-gnu/egcs-1.1]} {gcc-2.95.2 [i586-pc-linux-gnu/gcc-2.95.2]} {gcc32x [i586-pc-linux-gnu/gcc-3.2]} {slc4&64&gcc34x [x86_64-pc-linux-gnu/gcc-3.4]} {gcc34x [i586-pc-linux-gnu/gcc-3.4]} {Solaris [sparc-sun-solaris2.7/CC-5.2]}]}}
    ctx.hwaf_declare_macro("SRT_TARGET", (
      {"default": ""},
      {"egcs-1.1.2": "i586-pc-linux-gnu/egcs-1.1"},
      {"gcc-2.95.2": "i586-pc-linux-gnu/gcc-2.95.2"},
      {"gcc32x": "i586-pc-linux-gnu/gcc-3.2"},
      {("slc4", "64", "gcc34x"): "x86_64-pc-linux-gnu/gcc-3.4"},
      {"gcc34x": "i586-pc-linux-gnu/gcc-3.4"},
      {"Solaris": "sparc-sun-solaris2.7/CC-5.2"},
    ))
    ##### **** statement *hlib.MakeFragmentStmt (&{zipper_header})
    ##### **** statement *hlib.MakeFragmentStmt (&{zipper})
    #### macro &{{kits_dir [{default [cmt/kits]}]}}
    ctx.hwaf_declare_macro("kits_dir", (
      {"default": "cmt/kits"},
    ))
    ##### **** statement *hlib.PatternStmt (&{kits document zipper RE -group=kits $(<package>_RE_files) ; document zipper DEV -group=kits $(<package>_DEV_files) ; document zipper SRC -group=kits $(<package>_SRC_files) ; document zipper FULL -group=kits $(<package>_FULL_files)})
    #### macro &{{zip_command [{default [tar]}]}}
    ctx.hwaf_declare_macro("zip_command", (
      {"default": "tar"},
    ))
    ##### **** statement *hlib.PatternStmt (&{RE_files macro <package>_RE_files -s=$(bin) lib<package>.$(shlibsuffix)})
    #### macro &{{symlink [{default [${ExternalPolicy_root}/scripts/symlink.sh]}]}}
    ctx.hwaf_declare_macro("symlink", (
      {"default": "${ExternalPolicy_root}/scripts/symlink.sh"},
    ))
    #### macro &{{CMT_PATH_remove_regexp [{default [/[^s/][^/]*/]} {WIN32 [\\[^s/][^\\]*\]}]}}
    ctx.hwaf_declare_macro("CMT_PATH_remove_regexp", (
      {"default": "/[^s/][^/]*/"},
      {"WIN32": "\\\\[^s/][^\\\\]*\\"},
    ))
    #### tag &{gcc&32 [force32]}
    ctx.hwaf_declare_tag(
        "gcc&32",
        content=["force32"]
    )
    
    return # configure


### ---------------------------------------------------------------------------
def build(ctx):
    
    
    ##### **** statement *hlib.ApplyPatternStmt (&{declare_scripts [files="-s=$(EXTERNALPOLICYROOT)/src symlink.sh inverse_symlink.sh]})
    
    
    return # build

## EOF ##
