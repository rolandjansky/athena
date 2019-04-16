
printf '\n\e[48;5;021m-->Installing all VP1Light dependences...\e[0m\n';

sudo apt-get -y install git \
                        cmake \
                        python-dev \
                        freeglut3-dev \
                        zlib1g-dev \
                        libssl-dev \
                        libgd-dev \
                        libxft-dev \
                        libtbb-dev \
                        rpm \
                        uuid-dev

printf '\n\e[48;5;112m-->DONE! - All VP1Light dependecies installed!\e[0m\n';
