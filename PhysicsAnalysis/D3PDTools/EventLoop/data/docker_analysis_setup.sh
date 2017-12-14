#
# Environment configuration file setting up the installed GitLabCITests project.
#

# Set up the base environment using the base image's setup script:
source /home/atlas/analysis_release_setup.sh

# Set up the analysis base installation:
source /home/atlas/install/*/*/InstallArea/*/setup.sh
echo "Configured analysis setup from from: /home/atlas/install"

# Set up the prompt:
# export PS1='\[\033[01;35m\][bash]\[\033[01;31m\][\u GitLabCITests-$GitLabCITests_VERSION]\[\033[01;34m\]:\W >\[\033[00m\] ';
