#!/usr/bin/env python

# info

__all__ = ['metadata', 'metadata_all_files', 'convert_itemList', 'convert_metadata_items']

metadata = {}
metadata_all_files = {}


def _setup():

	from PyUtils.MetaReader import read_metadata

	import logging
	msg = logging.getLogger('MetaReader')

	global metadata
	global metadata_all_files

	# get input file name
	from RecExConfig.RecoFunctions import InputFileNames

	inFiles = InputFileNames()
	if len(inFiles) < 1:
		msg.error("No input files specified yet! Cannot do anything.")

	metadata_all_files = read_metadata(inFiles, mode='full')

	first_filename = inFiles[-1]  # take only the last input file from the infiles

	metadata = metadata_all_files[first_filename]
	metadata['file_name'] = first_filename


_setup()