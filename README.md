Athena - 21.9 Branch for Upgrade Phase-2
==========================

This branch is currently used for:
- Upgrade Phase-2 developments, such as ITk and HGTD

A note on Merge Review/Continuous Integration policy for this branch:

- We have adopted a policy of fixing compilation warnings resulting from changes in MRs, or in code packages that have been touched in the MR, but not to fix pre-existing compilation warnings.
- The reason for this is to avoid introducing conflicts with code being swept from ~21.3 (which also applies a similar policy) and to make the changeset that needs to be moved to master clearer (since we should not try and apply compilation warning fixes on code that may have diverged significantly, and should already be warning-free according to the policy in ~master).
