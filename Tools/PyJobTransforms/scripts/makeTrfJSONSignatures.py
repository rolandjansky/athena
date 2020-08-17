#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##############################################################################

import os, os.path, sys, json, argparse

from PyJobTransforms.trfLogger import msg

##############################################################################

def _getTransformsFromPATH():
    ##########################################################################

    done_list = set([
    ])

    skip_list = set([
        'Athena_tf.py',
        'beamSpotT0_Vertex_tf.py',
        'Cat_tf.py',
        'Echo_tf.py',
        'ESDtoAOD_tf.py',
        'ExeWrap_tf.py',
        'Sleep_tf.py',
        'RAWtoESD_tf.py',
    ])

    ##########################################################################

    result = []

    for path in os.environ['PATH'].split(":"):

        try: 

            for name in [entry for entry in os.listdir(path) if entry.endswith("_tf.py")]:

                if name not in done_list\
                   and                  \
                   name not in skip_list:

                    done_list.add(name)

                    result.append(os.path.join(path, name))

        except OSError:
            pass

    ##########################################################################

    return result

##############################################################################

def ___patchParams(d, target1, target2):

    if 'listtype' in d:
        listtype = d['listtype']
        del d['listtype']

        d[target1] = ('list')
        d[target2] = listtype

##############################################################################

def __patchParams(d):
    ##########################################################################

    if 'type' in d and d['type'].lower() == 'substep':

        if 'substeptype' in d:
            substeptype = d['substeptype']
            del d['substeptype']

            if substeptype.lower() != (('list'))\
               and                              \
               substeptype.lower() != 'steering':

                d['subtype'] = substeptype

            else:
                ___patchParams(d, 'subtype', 'subsubtype')

    else:
        ___patchParams(d, 'type', 'subtype')

    ##########################################################################

    if 'type' in d and (not d['type'] or d['type'].lower() == 'none'):
        del d['type']

    if 'subtype' in d and (not d['subtype'] or d['subtype'].lower() == 'none'):
        del d['subtype']

    if 'subsubtype' in d and (not d['subsubtype'] or d['subsubtype'].lower() == 'none'):
        del d['subsubtype']

##############################################################################

def _patchParams(d):

    for transform in d:

        __patchParams(d[transform])

    return d

##############################################################################

def main():
    ##########################################################################

    parser = argparse.ArgumentParser(description = "Generate signature files for substeps, dumped in JSON format." )

    parser.add_argument('--output', help = 'JSON output file',
                        required = True)

    parser.add_argument('--mode', help = 'mode (default = params)',
                        choices = ['params', 'params-alt', 'substeps'], default = 'params')

    parser.add_argument('--transforms', help = 'List of transforms to process'
                        ' (any path given is added to PYTHONPATH automatically).'
                        ' If not specified then all executable *_tf.py files'
                        ' found in PATH are added.', 
                        nargs = '+', default = None)

    cliargs = vars(parser.parse_args())

    ##########################################################################

    transforms_path_list = _getTransformsFromPATH() if cliargs['transforms'] is None else cliargs['transforms']

    ##########################################################################

    for transform_path in transforms_path_list:

        trfpath = os.path.dirname(transform_path)

        if len(trfpath) > 1 and trfpath not in sys.path:

            sys.path.append(trfpath)

    ##########################################################################

    result = {}
    treated = []

    for transform_path in transforms_path_list:
        ######################################################################

        if not transform_path.endswith('_tf.py'):
            continue

        ######################################################################

        transform_name = os.path.basename(transform_path)

        transform_module = os.path.splitext(transform_name)[0]

        ######################################################################

        msg.info('Processing transform {0}:'.format(transform_path))

        try:
            trfModule = __import__(transform_module, globals(), locals(), ['getTransform'], 0)

        except Exception as e:
            msg.warning('Failed to import transform {0} ({1}) - ignored'.format(transform_module, e))
            continue

        ######################################################################

        try:

            if 'getTransform' in dir(trfModule):
                ##############################################################

                transform = trfModule.getTransform()

                ##############################################################
                # MODE PARAMS                                                #
                ##############################################################
                if cliargs['mode'] == 'params':
                ##############################################################

                    desc = transform.parser.getProdsysDesc

                    if not isinstance(desc, dict):
                       desc = transform.parser.getProdsysDesc()

                    result[transform_module] = (((((((desc)))))))

                ##############################################################
                # MODE PARAMS-ALT                                            #
                ##############################################################
                if cliargs['mode'] == 'params-alt':
                ##############################################################

                    desc = transform.parser.getProdsysDesc

                    if not isinstance(desc, dict):
                        desc = transform.parser.getProdsysDesc()

                    result[transform_module] = _patchParams(desc)

                ##############################################################
                # MODE SUBSTEPS                                              #
                ##############################################################
                if cliargs['mode'] == 'substeps':
                ##############################################################

                    result[transform_module] = [{
                        'name': executor.name,
                        'alias': executor.substep,
                    } for executor in transform._executors]

            ##################################################################

            treated.append(transform_module)

            ##################################################################

        except Exception as e:
            msg.warning('Failed to treate transform {0} ({1}) - ignored'.format(transform_module, e))
            continue

    ##########################################################################

    with open(cliargs['output'], 'w') as fp:
        json.dump(result, fp, indent = 2)

    ##########################################################################

    msg.info('Successfully generated signature file {0} for transforms {1}'.format(cliargs['output'], json.dumps(treated)))

    ##########################################################################

    sys.exit(0)

##############################################################################

if __name__ == '__main__':
    main()

##############################################################################
