#!/bin/sh

echo '-->' $cc $cppflags $cppdebugflags -frtti -c $CheckerGccPlugins_args  -fplugin-arg-libchecker_gccplugins-checkers=all  $1
$cc $cppflags $cppdebugflags -frtti -c $CheckerGccPlugins_args  -fplugin-arg-libchecker_gccplugins-checkers=all  $1

