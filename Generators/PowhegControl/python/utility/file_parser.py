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


class FileParser(object):
    """! Utility class to perform simple operations on files."""

    def __init__(self, input_file_pattern):
        """! Initialise with an file pattern.

        @param input_file_pattern Pattern of files to perform actions on.
        """
        self.__input_file_names = glob.glob(input_file_pattern)

    def text_append(self, string_append):
        """! Replace regex_find by regex_replace in input_file_name.

        @param string_append String to append.
        """
        for input_file_name in self.__input_file_names:
            with open(input_file_name, "ab") as f_input:
                f_input.write(string_append + "\n")

    def text_replace(self, regex_find, string_replace, count=0, regex_line_match=None):
        """! Replace regex_find by regex_replace in input_file_name.

        @param regex_find        Regular expression to search for.
        @param string_replace    String to replace by.
        @param count             If non-zero then only replace first count occurences in each line.
        @param regex_line_match  If not 'None' then only do replacement on lines matching this
        """
        for input_file_name in self.__input_file_names:
            shutil.move(input_file_name, "{}.text_replace_backup".format(input_file_name))
            with open("{}.text_replace_backup".format(input_file_name), "rb") as f_input:
                with open(input_file_name, "wb") as f_output:
                    for line in f_input:
                        if regex_line_match is not None and not re.search(regex_line_match, line):
                            f_output.write(line)
                            continue
                        new_line = re.sub(regex_find, string_replace, line.rstrip(), count)
                        f_output.write(new_line + "\n")
            os.remove("{}.text_replace_backup".format(input_file_name))

    def text_remove(self, regex_find):
        """! Remove lines matching regex_find in input_file_name.

        @param regex_find Regular expression to search for.
        """
        for input_file_name in self.__input_file_names:
            shutil.move(input_file_name, "{}.text_replace_backup".format(input_file_name))
            with open("{}.text_replace_backup".format(input_file_name), "rb") as f_input:
                with open(input_file_name, "wb") as f_output:
                    for line in f_input:
                        if re.search(regex_find, line.rstrip()) is None:
                            f_output.write(line)
            os.remove("{}.text_replace_backup".format(input_file_name))
