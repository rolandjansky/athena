#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
 
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logging

logging_file = 'xAODBrowser.run.log'

def clean_file(filename):
    with open(filename, 'w'):
        pass

def setup():
    logger = logging.getLogger('mainLogger')
    logger.setLevel(logging.DEBUG)
    clean_file(logging_file)
    handler = logging.FileHandler(logging_file)
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)


def get_logger():
    return logging.getLogger('mainLogger')


def print_logs():
    with open(logging_file, 'r') as file:
        for line in file:
            print(line.rstrip())
