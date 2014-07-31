# Taken from CMT setup script https://trac.lal.in2p3.fr/CMT/browser/CMT/HEAD/src/setup.sh
# Define completion for interactive shell only
if [ -n "$-" ] && [ "${-#*i}" != "$-" ]; then
if [ -n "${ZSH_NAME-}" ]; then
  . ${SETUPCOMPLETIONROOT}/cmt/completion/setup_completion.zsh
elif [ -n "${BASH-}" ]; then
  . ${SETUPCOMPLETIONROOT}/cmt/completion/setup_completion.bash
fi
fi
unset SETUPCOMPLETIONROOT
