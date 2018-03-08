# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl FileParser
#  PowhegControl Text alteration operations for files
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import glob
import os
import re
import shutil

# Utility class to perform simple operations on files


class FileParser(object):

    # Initialise with an file pattern
    #  @param input_file_pattern Pattern of files to perform actions on
    def __init__(self, input_file_pattern):
        self.__input_file_names = glob.glob(input_file_pattern)

    # Replace regex_find by regex_replace in input_file_name
    #  @param regex_find Regular expression to search for
    #  @param string_replace String to replace by
    #  @param count If non-zero then only replace first count occurences in each line
    def text_replace(self, regex_find, string_replace, count=0):
        for input_file_name in self.__input_file_names:
            shutil.move(input_file_name, "{}.text_replace_backup".format(input_file_name))
            with open("{}.text_replace_backup".format(input_file_name), "rb") as f_input:
                with open(input_file_name, "wb") as f_output:
                    for line in f_input:
                        new_line = re.sub(regex_find, string_replace, line.rstrip(), count)
                        f_output.write(new_line + "\n")
            os.remove("{}.text_replace_backup".format(input_file_name))

    # Remove lines matching regex_find in input_file_name
    #  @param regex_find Regular expression to search for
    def text_remove(self, regex_find):
        for input_file_name in self.__input_file_names:
            shutil.move(input_file_name, "{}.text_replace_backup".format(input_file_name))
            with open("{}.text_replace_backup".format(input_file_name), "rb") as f_input:
                with open(input_file_name, "wb") as f_output:
                    for line in f_input:
                        if re.search(regex_find, line.rstrip()) is None:
                            f_output.write(line)
            os.remove("{}.text_replace_backup".format(input_file_name))
