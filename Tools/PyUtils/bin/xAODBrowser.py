#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import argparse

from PyUtils.xAODBrowserCursesWrapper import GuiLoader
from PyUtils.xAODBrowserElement import StructureGenerator, Element, get_selected
from PyUtils.xAODBrowserRoot import RootFileReader, OutputGenerator
import PyUtils.xAODBrowserLogger as logging
import PyUtils.xAODBrowserUtils as utils


def print_results(results):
    for item in results:
        name = item.name
        while item.parent:
            name = item.parent.name + '.' + name
            item = item.parent
        print(name)
    print(len(results))

def exit_app(exit_code=0):
    logging.print_logs()
    sys.exit(exit_code)



if __name__ == '__main__':
    logging.setup()
    logger = logging.get_logger()
    
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', type=str, help='xAOD input file path')
    parser.add_argument('-pi', '--pickle-input', type=str, help='pickle input file path')
    parser.add_argument('-o', '--output', type=str, help='Path to file to which results will be saved')
    parser.add_argument('-p', '--pickle', type=str, help='Path to file to which selected elements will be saved in pickle format')
    args = parser.parse_args()

    input_file_path = args.input
    pickle_input_file_path = args.pickle_input
    if not input_file_path and not pickle_input_file_path:
        logger.error('Missing input file')
        exit_app(1)
     
    data_structures = None

    if input_file_path:
        logger.info('Loading data from input file: ' + input_file_path)
        root_reader = RootFileReader(input_file_path, './container_names.pkl')
        data_dict = root_reader.data_dict
        size_dict = root_reader.size_dict
        structure_generator = StructureGenerator(size_dict)
        data_structures = structure_generator.generate(data_dict, 0)
    else:
        logger.info('Loading data from pickle file')
        try:
            data_structures = utils.load_initial_data(pickle_input_file_path)
        except TypeError as e:
            logger.error('Invalid data in pickle file: {}'.format(e.message))
            exit_app(1)

    logger.info('Data loaded. {} elements found'.format(len(data_structures)))

    summary_structure_size = Element.get_structure_size(data_structures)
    gui_loader = GuiLoader(data_structures, summary_structure_size)
    output_structure = gui_loader.load_gui()
    chosen_items_structure = get_selected(output_structure)
    output_generator = OutputGenerator(chosen_items_structure)

    output_file = args.output
    if output_file:  
        logger.info('Generating output python script: {}'.format(output_file))
        output_generator.save_to_output_file(output_file)
        logger.info('Output script {} generated'.format(output_file))
    else:
        logger.warning('Missing second argument. Results will not be saved')

    pkl_output = args.pickle
    if pkl_output:
        logger.info('Saving configuration')
        utils.save_configuration(pkl_output, output_structure)
        logger.info('Configuration saved to file: {}'.format(pkl_output))
    else:
        logger.warning('Missing third argument. Items will not be saved to pickle file')

    exit_app()
