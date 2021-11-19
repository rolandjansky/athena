#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file TileInputFiles.py
@brief Python configuration of Tile input files
'''

from AthenaCommon.Logging import logging
from subprocess import check_output
from subprocess import CalledProcessError
import six

def getInputDirectory(run, stream=None, project=None, suffix=None, year=None):
    """
    Function to find input directory with Tile Calorimeter input data files: calibrations, ...
    Arguments:
        run          -- run number
        stream       -- run stream
        project      -- data project
        suffix       -- directory suffix
        skipBadFiles -- skip known bad files
        year         -- year of data
    """

    log = logging.getLogger( 'TileInputFiles.getInputDirectory' )

    if run < 10:
        directory = '.'
    elif not year:
        if run < 142682:
            year = 2009
        elif run < 171194:
            year = 2010
        elif run < 194688:
            year = 2011
        elif run < 216816:
            year = 2012
        elif run < 224305:
            year = 2013
        elif run < 248584:
            year = 2014
        elif run < 287952:
            year = 2015
        elif run < 314450:
            year = 2016
        elif run < 342540:
            year = 2017
        elif run < 367980:
            year = 2018
        elif run < 374260:
            year = 2019
        elif run < 386367:
            year = 2020
        else:
            year = 2021

    if stream:
        if not project:
            if stream.startswith('calibration_L1Calo'):
                project = 'data21_calib'
                log.warning('Data project is not set up and will be used: %s', project)
            else:
                project = 'data21_cos'
                log.warning('Data project is not set up and will be used: %s', project)
        if not suffix:
            suffix = 'daq.RAW' if stream.startswith('calibration') else 'merge.RAW'
            log.warning('Directory suffix is not set up and will be used: %s', suffix)

        directory = f'/eos/atlas/atlastier0/rucio/{project}/{stream}/00{run}/{project}.00{run}.{stream}.{suffix}'

    else:
        directory = f'/eos/atlas/atlascerngroupdisk/det-tile/online/{year}/daq'

    return directory


def findFiles(run, path=None, filter='.', stream=None, project=None, suffix=None, year=None, skipBadFiles=True):
    """
    Function to find Tile Calorimeter input data files: calibrations, ...
    Arguments:
        run          -- run number
        path         -- input directory
        filter       -- data file filter
        stream       -- run stream
        project      -- data project
        suffix       -- directory suffix
        year         -- year, data taken in
        skipBadFiles - skip known bad files
    """

    log = logging.getLogger( 'TileInputFiles.findFiles' )

    if not path:
        path = getInputDirectory(run, stream, project, suffix, year)

    if not path:
        log.warning('There is no input directory')
        return []

    log.info('Input directory: %s', path)

    if (path.startswith('/eos/')):
        listRunFiles = f'xrdfs eosatlas ls -l {path} | grep {run} | grep -v "#" '
        listRunFiles += f'| grep -v -e "         [ 0-9][ 0-9][0-9] " | grep {filter} | sed "s|^.*/||" '

    else:
        listRunFiles = f'ls {path} | grep {run} | grep {filter}'

    files = []
    try:
        files = check_output(listRunFiles, shell = True).splitlines()
    except CalledProcessError:
        log.warning('It seems that there are no such directory: %s', path)

    badFiles = ""
    if skipBadFiles:
        for badDataFiles in ['/afs/cern.ch/user/t/tilebeam/ARR/bad_data_files', '/afs/cern.ch/user/t/tiledaq/public/bad_data_files']:
            try:
                badFiles += open(badDataFiles).read()
            except Exception:
                log.warning('Can not read file with bad data files: %s => It is ignored', badDataFiles)

    fullNames = []
    files = [six.ensure_str(f) for f in files]
    for file_name in (files):
        good = (file_name not in badFiles)
        if good:
            if (path.startswith('/eos/')):
                fullNames.append(f'root://eosatlas.cern.ch/{path}/{file_name}')
            else:
                fullNames.append(f'{path}/{file_name}')
        else:
            log.warning('Excluding known bad data file: %s', file_name)

    return fullNames


if __name__=='__main__':

    log = logging.getLogger( 'TileInputFiles' )

    import argparse
    parser= argparse.ArgumentParser('Script to find Tile Calorimeter input data files: calibrations, ...')
    parser.add_argument("-r", "--run", type=int, default=None, help="Run number")
    parser.add_argument("-d", "--directory", type=str, default=None, help="Input directory")
    parser.add_argument("-s", "--stream", type=str, default=None, help="Run stream")
    parser.add_argument("-p", "--project", type=str, default=None, help="Data project")
    parser.add_argument("-f", "--filter", type=str, default=".", help="Data file filter")
    parser.add_argument("--suffix", type=str, default=None, help="Directory suffix")
    parser.add_argument("--skipBadFiles", type=bool, default=True, help="Skip bad data files?")
    parser.add_argument("-y", "--year", type=int, default=None, help="Year, data taken in")
    args = parser.parse_args()

    files = findFiles(run=args.run, path=args.directory, filter=args.filter, stream=args.stream,
                      project=args.project, suffix=args.suffix, year=args.year, skipBadFiles=args.skipBadFiles)

    log.info('Input files: %s', files)
    if not files:
        log.warning("No run data files are found")
