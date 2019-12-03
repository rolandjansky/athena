# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Start from the base CERN CentOS 7 image.
FROM cern/cc7-base:latest

# Perform the image setup as root.
USER root
WORKDIR /root

# Install some additional packages.
RUN yum -y install which sclo-git25 wget tar atlas-devel libuuid-devel texinfo       \
           redhat-lsb-core libX11-devel libXpm-devel libXft-devel             \
           libXext-devel openssl-devel glibc-devel rpm-build libcurl-devel && \
    yum clean all

# Set up the ATLAS user, and give it super user rights.
RUN echo '%wheel	ALL=(ALL)	NOPASSWD: ALL' >> /etc/sudoers && \
    adduser atlas && chmod 755 /home/atlas && \
    usermod -aG wheel atlas

# Copy the interactive environment setup scripts in place.
COPY atlas_prompt.sh setup_atlas.sh enable_sclo_git25.sh /etc/profile.d/
# Add the message of the day instructions for the image.
COPY motd /etc/

# Switch to the ATLAS account.
USER atlas
WORKDIR /home/atlas

# Set up the command to run when starting the image.
CMD cat /etc/motd && /bin/bash
