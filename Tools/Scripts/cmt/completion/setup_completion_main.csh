# Taken from CMT setup script https://trac.lal.in2p3.fr/CMT/browser/CMT/HEAD/src/setup.csh
# Define completion for interactive shell only
if ( $?0 == 0 ) then
if ( $?tcsh == 1 ) then
  source ${SETUPCOMPLETIONROOT}/cmt/completion/setup_completion.tcsh
else if ( "${shell:t}" == "tcsh" ) then
  source ${SETUPCOMPLETIONROOT}/cmt/completion/setup_completion.tcsh
endif
endif
unsetenv SETUPCOMPLETIONROOT
