#!/bin/env python3
"""Prepare ATLAS release notes with a list of merge requests

davide.gerbaudo@gmail.com
Jul 2017

Modified by edward.moyse@cern.ch
Dec 2020
"""
usage = """%prog target_release nightly_tag

where
  target_release is the release you are creating, e.g. release/21.1.6
  nightly_tag is the nightly tag the release will be based on, e.g. nightly/21.1/2017-06-07T2215

Example:
  git clone ssh://git@gitlab.cern.ch:7999/atlas/athena.git
  cd athena
  %prog release/21.1.6 nightly/21.1/2017-06-07T2215
"""

import argparse
import os
import re
import subprocess
from collections import defaultdict

gitlab_available=True
try:
    import gitlab                     
except ImportError:
    gitlab_available=False

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-p', '--previous', help='previous release wrt. which we diff')
    parser.add_argument('-o', '--output', default='release_notes.md', help='where the notes are written')
    parser.add_argument('-r', '--relaxed', action='store_true', help='do not stop on dubious configurations')
    parser.add_argument('-v', '--verbose', action='store_true', help='print more info')
    parser.add_argument('--group-merge-requests', action='store_true', help='Group merge requests with the same labels together.')
    parser.add_argument('-t', '--token', help='Optionally pass a gitlab token to get more information.')
    parser.add_argument('target', help='Target release')
    parser.add_argument('nightly', help='Nightly tag to use')
    args = parser.parse_args()

    if args.token and not gitlab_available:
        print('WARNING - passing a token but was not able to import gitlab. You probably need to install python-gitlab first (see https://python-gitlab.readthedocs.io)')

    target_release = args.target
    nightly_tag = args.nightly
    sanitize_args(target_release, nightly_tag, keep_going=args.relaxed)
    previous_release = guess_previous_and_check(target_release=target_release) if not args.previous else args.previous
    verbose = args.verbose
    pretty_format = '%b' # perhaps some combination of '%s%n%b' ?
    cmd = "git log "+previous_release+".."+nightly_tag+" --pretty=format:'"+pretty_format+"' --merges"
    if verbose:
        print ("Executing:")
        print (cmd)
    output_log = get_command_output(cmd)
    if (output_log['returncode']>0):
        print("Git failed with: {}".format(output_log['stderr']))
        print('Are you running this script from within the athena/ directory?')
        quit(999)
    # If possible, use Gitlab
    gl_project=None
    if args.token and gitlab_available:
        gl = gitlab.Gitlab("https://gitlab.cern.ch", args.token)
        gl_project = gl.projects.get("atlas/athena")
    merged_mrs = parse_mrs_from_log (output_log['stdout'].decode("utf-8"),
                                    pretty_format=pretty_format, verbose=verbose, gl_project=gl_project)
    fill_template(target_release, nightly_tag, previous_release,
                  merged_mrs, output_filename=args.output, verbose=verbose, gl_project=gl_project, group_mrs=args.group_merge_requests)
    
def sanitize_args(target_release, nightly_tag, keep_going=False):
    if not target_release.startswith('release/'):
        print ("The target release should start with 'release/', you are using:\n%s"%target_release)
        if not keep_going:
            raise RuntimeWarning('Fix target release')
    if not nightly_tag.startswith('nightly/'):
        print ("The nightly tag should start with 'nightly/', you are using:\n%s"%nightly_tag)
        if not keep_going:
            raise RuntimeWarning('Fix nightly tag')

    rel_match = re.search(r'release/(?P<ver>\d+)\.(?P<maj>\d+?)\.(?P<rev>\d+?)', target_release)
    nig_match = re.search(r'nightly/(?P<branch>((\d+\.\d+)|master))/(?P<date>\d{4}-\d{2}-\d{2})T(?P<time>\d{4})', nightly_tag)

    if not rel_match:
        print ("The target release is not formatted as xx.y.z (version.major.revision semantic)")
        if not keep_going:
            raise RuntimeWarning('Fix formatting target release')
    elif not nig_match:
        print ("The nightly tag is not formatted as expected, nightly/branch/yyyy-mm-ddThhmm\n%s"%nightly_tag)
        if not keep_going:
            raise RuntimeWarning('Fix formatting nightly tag')
    else:
       branch_rel = '.'.join([rel_match.group('ver'), rel_match.group('maj')])
       branch_nig = nig_match.group('branch')
       if branch_rel!=branch_nig and branch_rel!='22.0' and branch_nig!='master':
           print ("You are creating a tag for %s from a nightly from %s" % (branch_rel, branch_nig))
           if not keep_going:
               raise RuntimeWarning('Create a tag for the correct branch')

def guess_previous_and_check(target_release='release/xx.y.z'):
    version_major_revision = target_release.split('.')
    revision = version_major_revision[-1] if len(version_major_revision)>2 else None
    missing_revision = revision is None or not str(revision).isdigit()
    if missing_revision or int(revision)==0:
        perhaps_previous = target_release[:-1]+'n-1'
        raise RuntimeWarning("Cannot guess previous release for '%s'" % target_release
                             +'\nPlease use something like'
                             +"\n --previous %s" % perhaps_previous)
    else:
        previous_revision = int(revision)-1
        version_major_previousrevision = version_major_revision[:-1] + [str(previous_revision)]
        previous_revision = '.'.join(version_major_previousrevision)
    return previous_revision

def get_command_output(command, with_current_environment=False):
     "lifted from supy (https://github.com/elaird/supy/blob/master/utils/io.py)"
     env = None if not with_current_environment else os.environ.copy()
     p = subprocess.Popen(command, shell = True, stdout = subprocess.PIPE, stderr = subprocess.PIPE, env=env)
     stdout, stderr = p.communicate()
     return {"stdout":stdout, "stderr":stderr, "returncode":p.returncode}

class MergeRequestInfo(object):
    def __init__(self, mr=None, one_liner=''):
        self.mr = mr
        self.one_liner = one_liner
    def __str__(self):
        return (" ---- : %s" % self.one_liner if self.mr is None else
                "!%d : %s" % (int(self.mr), self.one_liner))

    def init_from_gitlab_message_lines(self, mr_description_lines=[], verbose=False):
        """Try parsing the commit msg we usually get for a gitlab MR.

        This is usually a few lines that look like:

           <some description, which can be multi-line>

           See merge request [optional branch] !<some MR ID>
        """
        if verbose:
            print ("parsing\n",mr_description_lines)
        lines = mr_description_lines
        see_mr_lines = [l for l in lines if l.startswith('See merge request')]
        see_mr_line = see_mr_lines[-1] if see_mr_lines else [] # -1 in case there are multiple matches
        if len(lines)>=2 and see_mr_lines:
            self.one_liner = lines[0]
            mr_match = re.search(r'See merge request.*!(?P<mr>\d+).*', see_mr_line)
            self.mr = mr_match.group('mr') if mr_match else None
            if verbose:
                print (self.__str__())
        else:
            self.one_liner = '; '.join(lines)
            if verbose:
                print ("Cannot parse these lines:\n"+'\n'.join("[%02d] : '%s'" % (iL, l) for iL, l in enumerate(lines)))
        return self

def parse_mrs_from_log(output, pretty_format='%b', verbose=False, gl_project=None):

    if pretty_format!='%b':
        raise RuntimeWarning("parsing of pretty_format %s not implemented yet")
    mrs = []
    lines_this_mr = []

    for line in output.split('\n'):
        line = line.lstrip().strip()
        if line.startswith('See merge request'):
            lines_this_mr.append(line)
            mri = MergeRequestInfo().init_from_gitlab_message_lines(lines_this_mr, verbose)
            if gl_project:
                mrs.append(gl_project.mergerequests.get(mri.mr))
            else:
                mrs.append(mri)
            lines_this_mr = []
        elif line: # skip empty
            lines_this_mr.append(line)
    if lines_this_mr:
        mri = MergeRequestInfo().init_from_gitlab_message_lines(lines_this_mr, verbose)
        if gl_project:
            mrs.append(gl_project.mergerequests.get(mri.mr))
        else: 
            mrs.append(mri)
    return mrs


def default_template():
    return """
# Release notes for {target_release:s}
The release {target_release_link:s}
was built from the tag {nightly_tag_link:s}

This is the list of merge requests that were included since
the previous release {previous_release_link:s}:
{formatted_list_of_merge_requests:s}

Link to the full diff between {target_release_link:s} and
{previous_release_link:s}
is available at
https://gitlab.cern.ch/atlas/athena/compare/{previous_release:s}...{target_release:s}

"""

def format_mrs_from_gitlab(merged_mrs, group_mrs=False):
    # FIXME - we don't want to dump all labels, so have an approved list
    # However should try to think of a way to link to domain_map.py?
    # For the time being, this list was made as follows:
    # import domain_map.py
    # allowed_labels = sorted(domain_map.DOMAIN_MAP.keys())
    allowed_labels = ['ACTS', 'Analysis', 'AnalysisTop', 'BTagging', 'Build', 'CI', 'Calorimeter', 'Core', 'DQ', 'Database', 'Derivation', 'Digitization', 'EDM', 'Egamma', 'EventDisplay', 'Externals', 'ForwardDetectors', 'Generators', 'Geometry', 'HGTD', 'ITk', 'InnerDetector', 'JetEtmiss', 'LAr', 'Magnets', 'MuonSpectrometer', 'Other', 'Overlay', 'Powheg', 'QuickAna', 'Reconstruction', 'SUSYTools', 'Simulation', 'Tau', 'Test', 'TestBeam', 'Tile', 'Tools', 'Tracking', 'Trigger', 'TriggerEDM', 'TriggerID', 'TriggerJet', 'TriggerMenu', 'TriggerMinBias', 'changes-trigger-counts']
    lines = []
    if group_mrs:
        grouped_mrs=defaultdict(list)
        for mr in merged_mrs:
            labels = ['~'+label for label in mr.labels if label in allowed_labels]
            label_key = ", ".join(labels)
            grouped_mrs[label_key].append('   * {} : {}'.format(mr.id, mr.title))
        for key,value in grouped_mrs.items():
            lines.append(' * {}'.format(key))
            for title in value:
                lines.append(title)
    else:
        for mr in merged_mrs:
            labels = ['~'+label for label in mr.labels if label in allowed_labels]
            lines.append(" * {id} {title} [{labels}]".format(id=mr.id, title=mr.title, labels=", ".join(labels)))
    return '\n'.join(lines)

def fill_template(target_release, nightly_tag, previous_release,
                  merged_mrs=[], output_filename='foo.md', verbose=False, gl_project=None, group_mrs=False):
    formatted_mrs=""
    if gl_project:
        formatted_mrs = format_mrs_from_gitlab(merged_mrs, group_mrs)
    else:
        formatted_mrs = '\n'.join([" * %s" % mr for mr in merged_mrs]) if merged_mrs else '* None'
    def formatted_tag_link(tag=''):
        base_url = 'https://gitlab.cern.ch/atlas/athena/tags'
        return "[%s](%s)" % (tag, base_url+'/'+tag)
    print('formatted_mrs', formatted_mrs)
    filled_template = default_template().format(**{'target_release' : target_release,
                                                   'target_release_link' : formatted_tag_link(target_release),
                                                   'nightly_tag' : nightly_tag,
                                                   'nightly_tag_link' : formatted_tag_link(nightly_tag),
                                                   'previous_release' : previous_release,
                                                   'previous_release_link' : formatted_tag_link(previous_release),
                                                   'formatted_list_of_merge_requests' : formatted_mrs})
    out_file = open(output_filename, 'w')
    out_file.write(filled_template)
    out_file.close()
    print ("Release notes generated in '%s'" % output_filename)

if __name__=='__main__':
    main()
