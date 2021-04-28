#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Script to compare trigger chain counts in YAML format produced by
chainDump.py --yaml and providing information and instructions for
updating counts reference in the athena repository.
'''

import sys
import os
import argparse
import logging
import yaml
import glob
import subprocess
from AthenaCommon.Utils import unixtools
from TrigValTools.TrigValSteering.Common import package_prefix_dict as packages
from TrigValTools.TrigValSteering.Common import running_in_CI


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] inputFile',
                                     description=__doc__)
    parser.add_argument('inputFile',
                        metavar='PATH',
                        help='Name of input counts file')
    parser.add_argument('-r', '--referenceFile',
                        metavar='PATH',
                        help='Name of reference counts file. If empty, try to guess from inputFile')
    parser.add_argument('-s', '--sourceRepository',
                        metavar='PATH',
                        help='Path to the git checkout of the athena repository.'
                             'If empty, try to guess from the environment')
    parser.add_argument('-p', '--patch',
                        action='store_true',
                        help='Create a git patch file with the count differences')
    parser.add_argument('-m', '--matchingOnly',
                        action='store_true',
                        help='Ignore added and removed chains, and only compare counts for chains ' + \
                             'present in both reference and input file')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def find_reference(inputFile):
    ref_path = None
    if inputFile.endswith('.new'):
        inp_name = os.path.split(inputFile)[-1]
        ref_name = inp_name[0:-4] + '.ref'
        logging.debug('Searching for file %s in DATAPATH', ref_name)
        for pkg_name in packages:
            ref_path = unixtools.find_datafile(pkg_name+'/'+ref_name)
            if ref_path:
                break
        if ref_path:
            logging.debug('Found %s', ref_path)
        else:
            logging.debug('Not found')
    return ref_path


def find_repository(ref_path):
    '''
    Try to find the athena git repository based on paths in env.
    Needs to work both in local WorkDir build and in full/incremental CI build.
    '''
    logging.debug('Searching for the source repository path')
    search_str = '/Trigger/TrigValidation'
    def try_path(p):
        logging.debug('Trying %s', p)
        if os.path.islink(p) and os.path.isfile(p) and search_str in os.path.realpath(p):
            candidate = os.path.realpath(p).split(search_str)[0]
            logging.debug('Candidate %s', candidate)
            if os.path.isdir(candidate + '/.git'):
                return candidate
        return None

    result = try_path(ref_path)
    if result:
        return result
    
    env_vars = ['DATAPATH', 'JOBOPTSEARCHPATH', 'CALIBPATH', 'XMLPATH']
    path_lists = [os.getenv(var).split(os.pathsep) for var in env_vars]
    max_len = max([len(pl) for pl in path_lists])
    max_len = min(max_len, 2)  # Limit to first two paths to avoid very deep and slow search
    for i in range(max_len):
        for pl in path_lists:
            if i >= len(pl):
                continue
            paths = glob.glob(pl[i]+'/**', recursive=True)
            for p in paths:
                result = try_path(p)
                if result:
                    return result
    return None


def load_yaml(path):
    result = None
    if not os.path.isfile(path):
        logging.error('Not a file: %s', path)
        return result
    with open(path) as f:
        result = yaml.safe_load(f)
    return result


def print_event_diff(inp_data, ref_data, key):
    if inp_data[key] != ref_data[key]:
        logging.info('    %s: %d -> %d', key, ref_data[key], inp_data[key])


def print_step_diff(inp_data, ref_data, key):
    inp_steps = inp_data.get(key, dict())
    ref_steps = ref_data.get(key, dict())
    if inp_steps != ref_steps:
        logging.info('    %s:', key)
        for step in range(max(len(inp_steps), len(ref_steps))):
            inp_count = inp_steps.get(step, 0)
            ref_count = ref_steps.get(step, 0)
            if inp_count != ref_count:
                logging.info('      %d: %d -> %d', step, ref_count, inp_count)


def print_diff(inp_dict, ref_dict, matching_only=False):
    retcode = 0
    chains_not_in_ref = [c[0] for c in inp_dict.items() if c[0] not in ref_dict]
    chains_not_in_inp = [c[0] for c in ref_dict.items() if c[0] not in inp_dict]
    chains_count_diff = [c[0] for c in inp_dict.items() if c[0] in ref_dict and c[1] != ref_dict[c[0]]]

    # New chains (missing in reference)
    n_new_chains = len(chains_not_in_ref)
    if n_new_chains > 0 and not matching_only:
        retcode = 1
        logging.info('Found %d new chain%s added:', n_new_chains, 's' if n_new_chains > 1 else '')
        for chain_name in chains_not_in_ref:
            logging.info('  %s', chain_name)

    # Removed chains (missing in test file)
    n_removed_chains = len(chains_not_in_inp)
    if n_removed_chains > 0 and not matching_only:
        retcode = 1
        logging.info('Found %d chain%s removed:', n_removed_chains, 's' if n_removed_chains > 1 else '')
        for chain_name in chains_not_in_inp:
            logging.info('  %s', chain_name)

    # Count differences
    n_diff_chains = len(chains_count_diff)
    if n_diff_chains > 0:
        retcode = 1
        logging.info('Found %d chain%s with count differences:', n_diff_chains, 's' if n_diff_chains > 1 else '')
        for chain_name in chains_count_diff:
            logging.info('  %s:', chain_name)
            inp_data = inp_dict[chain_name]
            ref_data = ref_dict[chain_name]
            print_event_diff(inp_data, ref_data, 'eventCount')
            print_step_diff(inp_data, ref_data, 'stepCounts')
            print_step_diff(inp_data, ref_data, 'stepFeatures')

    return retcode


def create_patch(inp_path, ref_path):
    def find_ref_package():
        for path in [ref_path, find_reference(inp_path), find_reference(ref_path)]:
            for pkg in packages:
                if '/data/'+pkg in ref_path:
                    return pkg, os.path.split(path)[1]
        return None, None
    package_name, ref_file_name = find_ref_package()
    if not package_name:
        logging.warning('Failed to find reference package name, cannot create git patch file')
        return

    ref_rel_path = '/Trigger/TrigValidation/' + package_name + '/share/' + ref_file_name

    diff_cmd = 'diff -U 1 -b'
    diff_cmd += ' --label a' + ref_rel_path + ' ' + ref_path
    diff_cmd += ' --label b' + ref_rel_path + ' ' + inp_path

    diff_txt = subprocess.getoutput(diff_cmd)
    patch_name = os.path.split(ref_path)[1]
    if patch_name.endswith('.ref') or patch_name.endswith('.new'):
        patch_name = patch_name[0:-4]
    patch_name += '.patch'
    with open(patch_name, 'w') as patch_file:
        patch_file.write('diff --git a{p:s} b{p:s}\n'.format(p=ref_rel_path))
        patch_file.write(diff_txt + '\n')
    logging.info('Patch file created. To apply, run in the athena source directory:')
    logging.info('git apply %s/%s', os.getcwd(), patch_name)
    logging.info('Then check with git diff and, if everything is correct, add and commit')
    if running_in_CI():
        logging.info('or see the patch below:\n')
        subprocess.call('cat ' + patch_name, shell=True)
        print('\n') # noqa: ATL901


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='chainComp %(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    inp_path = args.inputFile
    ref_path = args.referenceFile or find_reference(inp_path)
    if not ref_path:
        logging.error('No reference file specified and couldn\'t guess from '
                      'input name. Please use the -r argument (see --help).')
        return 1

    logging.info('Test file: %s', inp_path)
    logging.info('Reference file: %s', ref_path)

    # Currently not used, but ready for implementing checks against upstream
    # src_path = args.sourceRepository or find_repository(ref_path)
    # logging.info('Repository path: %s', src_path)

    inp_dict = load_yaml(inp_path)
    ref_dict = load_yaml(ref_path)
    if (not inp_dict) or (not ref_dict):
        return 1

    retcode = print_diff(inp_dict, ref_dict, args.matchingOnly)
    if retcode:
        logging.error('Trigger counts differ from the reference. If the above differences are intended, update the reference')
        if args.patch:
            create_patch(inp_path, ref_path)
    else:
        logging.info('Trigger counts match the reference')

    return retcode


if '__main__' in __name__:
    sys.exit(main())
