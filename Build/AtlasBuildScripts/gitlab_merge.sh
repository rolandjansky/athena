#!/usr/bin/env bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script to merge a set of MRs with given labels into the local branch.
#

# Defaults
branch='master'
remote='origin'

usage() {
    cat <<EOF
Usage: gitlab_merge.sh -l labels [-b branch] [-r remote] [-p project]
Fetch and merge MRs with the given labels into current local git branch.

  -l labels    comma separate list of labels
  -b branch    GitLab target branch of MRs [${branch}]
  -r remote    name of remote in local repository [${remote}]
  -p project   GitLab project [derived from remote]
EOF
}

set -e

while getopts "l:b:r:p:h" opt; do
    case $opt in
        l ) labels=$OPTARG;;
        b ) branch=$OPTARG;;
        r ) remote=$OPTARG;;
        p ) project=$OPTARG;;
        h ) usage; exit;;
        ? ) exit 1;;  # unknown option
        : ) exit 1;;  # missing argument
    esac
done

if [ -z "${labels}" ]; then
    usage
    exit 1
fi

# Extract GitLab project name from URL if not given
if [ -z "${project}" ]; then
    project=`git remote get-url ${remote} | rev | cut -d '/' -f 1,2 | rev | sed 's#\.git##'`
fi

# URL encode project name (mostly because of "/")
project_url=`jq -nr --arg v ${project} '$v|@uri'`

# Get MRs matching label via GitLab API (one page with 100 MRs is enough)
json=`curl --silent "https://gitlab.cern.ch/api/v4/projects/${project_url}/merge_requests?state=opened&per_page=100&target_branch=${branch}&labels=${labels}"`
merge_requests=`echo "$json" | jq '.[]|.iid'` || { echo $json; exit 1; }

if [ -z "${merge_requests}" ]; then
    echo "No MRs matching label '${labels}' for branch '${branch}' in '${project}' found."
    exit 0
fi

# Fetch all MRs and merge into current branch
# Based on https://stackoverflow.com/questions/44992512
for id in ${merge_requests}; do
    br="mr-${remote}-${id}"
    echo "Merging ${project}!${id}"
    git fetch ${remote} merge-requests/${id}/head:${br}
    git merge --commit --no-edit ${br}
    git branch -D ${br}    # delete local branch again
done
