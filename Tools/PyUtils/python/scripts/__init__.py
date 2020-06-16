# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# hook for PyUtils.scripts package

import PyUtils.acmdlib as acmdlib
acmdlib.register('chk-file', 'PyUtils.scripts.check_file')
acmdlib.register('chk-sg', 'PyUtils.scripts.check_sg')
acmdlib.register('chk-rflx', 'PyUtils.scripts.check_reflex')
acmdlib.register('diff-pool', 'PyUtils.scripts.diff_pool_files')
acmdlib.register('diff-root', 'PyUtils.scripts.diff_root_files')
acmdlib.register('dump-root', 'PyUtils.scripts.dump_root_file')
acmdlib.register('ath-dump', 'PyUtils.scripts.ath_dump')
acmdlib.register('gen-klass', 'PyUtils.scripts.gen_klass')

acmdlib.register('merge-files', 'PyUtils.scripts.merge_files')
acmdlib.register('filter-files', 'PyUtils.scripts.filter_files')

acmdlib.register('cmake.depends', 'PyUtils.scripts.cmake_depends')

acmdlib.register('jira.issues', 'PyUtils.scripts.jira_issues')
##
