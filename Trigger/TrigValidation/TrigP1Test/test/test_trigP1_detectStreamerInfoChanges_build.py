#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Detect TStreamerInfo changes of classes in the given build
# art-type: build
# art-include: master/Athena
# art-include: 22.0/Athena
# Skipping art-output which has no effect for build tests.
#
# The test performs the following:
# 1) get the bs-streamerinfos.root from the nightly and check it
# 2) detect whether there are TStreamerInfo changes for classes that are packed to bytestream (Run 3 EDM targets BS, *DS)
# 3) if there are class changes, the test is failed, asking to update bs-streamerinfos.root
# 4) it generates a new test bs-streamerinfos.root and checks is

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex_get_BS_file = ExecStep.ExecStep('GetBSFile')
ex_get_BS_file.type = 'other'
ex_get_BS_file.input = ''
ex_get_BS_file.executable = 'get_files'
ex_get_BS_file.args = '-data bs-streamerinfos.root'

ex_check_file_pre = ExecStep.ExecStep('CheckBSFilePre')
ex_check_file_pre.type = 'other'
ex_check_file_pre.input = ''
ex_check_file_pre.executable = 'python'
ex_check_file_pre.args = '-c "import sys; from TrigValTools.TrigRootUtils import check_file_subproc; sys.exit(check_file_subproc(\'bs-streamerinfos.root\'))"'

ex_detect_new_sinfo = ExecStep.ExecStep('NewStreamerInfo')
ex_detect_new_sinfo.type = 'other'
ex_detect_new_sinfo.input = ''
ex_detect_new_sinfo.executable = 'python'
ex_detect_new_sinfo.args = '-m TrigP1Test.DetectStreamerInfoChanges'

ex_check_file_post = ExecStep.ExecStep('CheckBSFilePost')
ex_check_file_post.type = 'other'
ex_check_file_post.input = ''
ex_check_file_post.depends_on_previous = False
ex_check_file_post.executable = 'python'
ex_check_file_post.args = '-c "import sys; from TrigValTools.TrigRootUtils import check_file_subproc; sys.exit(check_file_subproc(\'bs-streamerinfos.root\'))"'

ex_diff_root_msg = ExecStep.ExecStep('CompRootMsg')
ex_diff_root_msg.type = 'other'
ex_diff_root_msg.input = ''
ex_diff_root_msg.depends_on_previous = False
ex_diff_root_msg.executable = 'diff'
ex_diff_root_msg.args = 'CheckBSFilePre.log CheckBSFilePost.log'


test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex_get_BS_file, ex_check_file_pre, ex_detect_new_sinfo, ex_check_file_post, ex_diff_root_msg]
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep)]

import sys
sys.exit(test.run())
