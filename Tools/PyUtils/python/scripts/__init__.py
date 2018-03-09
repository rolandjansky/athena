# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# hook for PyUtils.scripts package

# FIXME: waiting for a proper declarative file
import PyUtils.acmdlib as acmdlib
acmdlib.register('chk-file', 'PyUtils.scripts.check_file:main')
acmdlib.register('diff-pool', 'PyUtils.scripts.diff_pool_files:main')
acmdlib.register('diff-root', 'PyUtils.scripts.diff_root_files:main')
acmdlib.register('dump-root', 'PyUtils.scripts.dump_root_file:main')
acmdlib.register('chk-sg', 'PyUtils.scripts.check_sg:main')
acmdlib.register('ath-dump', 'PyUtils.scripts.ath_dump:main')
acmdlib.register('chk-rflx', 'PyUtils.scripts.check_reflex:main')
acmdlib.register('gen-klass', 'PyUtils.scripts.gen_klass:main')

acmdlib.register('tc.find-pkg', 'PyUtils.scripts.tc_find_pkg:main')
acmdlib.register('tc.find-tag', 'PyUtils.scripts.tc_find_tag:main')
acmdlib.register('tc.submit-tag', 'PyUtils.scripts.tc_submit_tag:main')

acmdlib.register('get-tag-diff', 'PyUtils.scripts.get_tag_diff:main')
acmdlib.register('get-tagsvn-diff', 'PyUtils.scripts.get_tagsvn_diff:main')

acmdlib.register('merge-files', 'PyUtils.scripts.merge_files:main')
acmdlib.register('filter-files', 'PyUtils.scripts.filter_files:main')

acmdlib.register('cmt.new-pkg', 'PyUtils.scripts.cmt_newpkg:main')
acmdlib.register('cmt.new-alg', 'PyUtils.scripts.cmt_newalg:main')
acmdlib.register('cmt.new-analysisalg', 'PyUtils.scripts.cmt_newanalysisalg:main')
acmdlib.register('cmt.new-asgtool', 'PyUtils.scripts.cmt_newasgtool:main')
acmdlib.register('cmt.new-pyalg', 'PyUtils.scripts.cmt_newpyalg:main')
acmdlib.register('cmt.new-jobo', 'PyUtils.scripts.cmt_newjobo:main')
acmdlib.register('cmt.new-analysisapp', 'PyUtils.scripts.cmt_newanalysisapp:main')

acmdlib.register('cmake.new-skeleton', 'PyUtils.scripts.cmake_newskeleton:main')
acmdlib.register('cmake.new-pkg', 'PyUtils.scripts.cmake_newpkg:main')
acmdlib.register('cmake.new-analysisalg', 'PyUtils.scripts.cmake_newanalysisalg:main')

acmdlib.register('jira.issues', 'PyUtils.scripts.jira_issues:main')
# acmdlib.register('jira.issue', 'PyUtils.scripts.jira_issue:main')
##

