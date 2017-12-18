#
# Environment configuration file setting up the installed GitLabCITests project.
#

# Set up the base environment using the base image's setup script:
source /home/atlas/analysis_release_setup.sh

# Set up the analysis base installation:
source /usr/${PROJECT_NAME}/*/InstallArea/*/setup.sh
echo "Configured analysis setup from from: /usr/${PROJECT_NAME}"

# Set up the prompt:
# export PS1='\[\033[01;35m\][bash]\[\033[01;31m\][\u GitLabCITests-$GitLabCITests_VERSION]\[\033[01;34m\]:\W >\[\033[00m\] ';
