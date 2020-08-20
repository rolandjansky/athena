#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "Convert LightGBM model to TTree to be used with MVAUtils."
__author__ = "Ruggero Turra"

try:
    import lightgbm as lgb
except ImportError:
    print("""cannot load lightgbm. Try to install it with
   pip install lightgbm
or (usually on lxplus)
   pip install numpy scipy scikit-learn
   pip install --no-binary :all: lightgbm
""")
import ROOT
import time
import logging
logging.basicConfig(level=logging.DEBUG)


def lgbm_rawresponse_each_tree(model, my_input):
    import numpy as np
    nclasses = model.num_model_per_iteration()
    output_values = np.array(
        [np.array([[0] * nclasses])]
        + [model.predict(my_input,
                         raw_score=True,
                         num_iteration=itree)
           for itree in range(1, (model.num_trees() / nclasses + 1))])
    output_trees = np.diff(output_values, axis=0)
    return output_trees


def list2stdvector(values, dtype='float'):
    result = ROOT.std.vector(dtype)()
    for v in values:
        result.push_back(v)
    return result


class LGBMTextNode(dict):
    """
    Adaptor from LGBM dictionary to tree
    """

    def __init__(self, structure, invert_as_tmva=False):
        super(LGBMTextNode, self).__init__(structure)
        self.invert_as_tmva = invert_as_tmva

    def get_split_feature(self):
        if 'split_feature' in self:
            return self['split_feature']
        else:
            return -1

    def get_value(self):
        if 'threshold' in self:
            return self['threshold']
        else:
            return self['leaf_value']

    def _get_left(self):
        if 'left_child' not in self:
            return None
        if self['decision_type'] == '<=':
            return LGBMTextNode(self['left_child'])
        else:
            return LGBMTextNode(self['right_child'])

    def _get_right(self):
        if 'right_child' not in self:
            return None
        if self['decision_type'] == '<=':
            return LGBMTextNode(self['right_child'])
        else:
            return LGBMTextNode(self['left_child'])

    def get_left(self):
        if not self.invert_as_tmva:
            return self._get_left()
        else:
            return self._get_right()

    def get_right(self):
        if not self.invert_as_tmva:
            return self._get_right()
        else:
            return self._get_left()

    def get_default_left(self):
        return self.get('default_left', True)


def dump_tree(tree_structure):
    """
    dump a single decision tree to arrays to be written into the TTree
    """

    split_values = []
    split_features = []
    default_left = []
    top = LGBMTextNode(tree_structure)
    simple = [True]  # python2 lack of nonlocal variables

    def preorder(node):
        # visit root
        split_features.append(node.get_split_feature())
        split_values.append(node.get_value())
        default_left.append(node.get_default_left())

        if not node.get_default_left():
            simple[0] = False

        if 'decision_type' in node and node['decision_type'] != '<=':
            raise ValueError(
                'do not support categorical input BDT (decision_type = %s)'
                % node['decision_type'])

        if 'missing_type' in node:
            if node['missing_type'] not in ('NaN', 'None'):
                raise ValueError(
                    'do not support missing values different from NaN or None')

        # visit left
        if node.get_left() is not None:
            preorder(node.get_left())
        # visit right
        if node.get_right() is not None:
            preorder(node.get_right())

    preorder(top)
    return split_features, split_values, default_left, simple[0]


def dump2ROOT(model, output_filename, output_treename='lgbm'):
    model = model.dump_model()
    fout = ROOT.TFile.Open(output_filename, 'recreate')

    features_array = ROOT.std.vector('int')()
    values_array = ROOT.std.vector('float')()
    default_lefts_array = ROOT.std.vector('bool')()

    simple = True
    node_type = 'node_type=lgbm_simple'
    for tree in model['tree_info']:
        tree_structure = tree['tree_structure']
        features, values, default_lefts, simple_tree = dump_tree(
            tree_structure)
        if not simple_tree:
            simple = False
            node_type = 'node_type=lgbm'

    infos = ';'.join(['%s=%s' % (k, str(v))
                      for k, v in model.items() if type(v) != list])
    title = ';'.join(('creator=lgbm', node_type, infos))
    root_tree = ROOT.TTree(output_treename, title)
    root_tree.Branch('vars', 'vector<int>', ROOT.AddressOf(features_array))
    root_tree.Branch('values', 'vector<float>', ROOT.AddressOf(values_array))

    if not simple:
        logging.info("tree support nan: using full implementation (LGBMNode)")
        root_tree.Branch('default_left', 'vector<bool>',
                         ROOT.AddressOf(default_lefts_array))
    if simple:
        logging.info(
            "tree do not support nan:"
            "using simple implementation (LGBMNodeSimple)")

    for tree in model['tree_info']:
        tree_structure = tree['tree_structure']
        features, values, default_lefts, simple_tree = dump_tree(
            tree_structure)

        features_array.clear()
        values_array.clear()
        default_lefts_array.clear()

        for value in values:
            values_array.push_back(value)
        for feature in features:
            features_array.push_back(feature)
        for default_left in default_lefts:
            default_lefts_array.push_back(default_left)

        root_tree.Fill()

    root_tree.Write()
    fout.Close()
    return output_treename


def convertLGBMToRootTree(model, output_filename, tree_name='lgbm'):
    """
    Model: - a string, in this case, it is the name of
    the input file containing the lgbm model you
    can get this model with lgbm with
    `boosted.save_model('my_model.txt')
    - directly a lgbm booster object
    """
    if type(model) is str:
        model = lgb.Booster(model_file=model)
        return dump2ROOT(model, output_filename, tree_name)
    else:
        return dump2ROOT(model, output_filename, tree_name)


def test(model_file, tree_file,
         tree_name='lgbm',
         ntests=10000,
         test_file=None):
    booster = lgb.Booster(model_file=model_file)
    f = ROOT.TFile.Open(tree_file)
    tree = f.Get(tree_name)
    try:
        _ = ROOT.MVAUtils.BDT
    except Exception:
        print("cannot import MVAUtils")
        return None

    mva_utils = ROOT.MVAUtils.BDT(tree)

    objective = booster.dump_model()['objective']
    if 'multiclass' in objective:
        logging.info("testing multiclass")
        return test_multiclass(booster, mva_utils, ntests, test_file)
    elif 'binary' in objective:
        logging.info("testing binary")
        return test_binary(booster, mva_utils, ntests, test_file)
    else:
        logging.info("testing regression")
        return test_regression(booster, mva_utils, ntests, test_file)


def test_regression(booster, mva_utils, ntests=10000, test_file=None):
    import numpy as np
    nvars = booster.num_feature()
    logging.info("using %d input features with regression", nvars)

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s",
                     len(data_input), test_file)
    else:
        logging.info(
            "using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning(
            "using random uniform input as test: this is not safe"
            "provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = mva_utils.GetResponse(input_values_vector)
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    for input_values, output_lgbm, output_MVAUtils in zip(data_input,
                                                          results_lgbm,
                                                          results_MVAUtils):
        if not np.allclose(output_lgbm, output_MVAUtils, rtol=1E-4):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "lgbm: %s\n"
                         "inputs: %s",
                         output_MVAUtils,
                         output_lgbm,
                         input_values)
            return False
    return True


def test_binary(booster, mva_utils, ntests=10000, test_file=None):
    import numpy as np
    nvars = booster.num_feature()
    logging.info("using %d input features with binary classification", nvars)

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s",
                     len(data_input), test_file)
    else:
        logging.info(
            "using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning(
            "using random uniform input as test:"
            "this is not safe, provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = mva_utils.GetClassification(input_values_vector)
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    for input_values, output_lgbm, output_MVAUtils in zip(data_input,
                                                          results_lgbm,
                                                          results_MVAUtils):
        if not np.allclose(output_lgbm, output_MVAUtils):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "lgbm: %s\n"
                         "inputs: %s", output_MVAUtils,
                         output_lgbm,
                         input_values)
            return False
    return True


def test_multiclass(booster, mva_utils, ntests=10000, test_file=None):
    import numpy as np
    nvars = booster.num_feature()
    nclasses = booster.num_model_per_iteration()
    logging.info("using %d input features with %d classes", nvars, nclasses)

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s",
                     len(data_input), test_file)
    else:
        logging.info(
            "using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning(
            "using random uniform input as test:"
            "this is not safe, provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    # to match what mvautils is doing (using c-float)
    data_input = data_input.astype(np.float32)

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = np.asarray(
            mva_utils.GetMultiResponse(input_values_vector, nclasses))
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input",
                 (time.time() - start) * 1000 / len(data_input))

    stop_event_loop = False
    for ievent, (input_values, output_lgbm, output_MVAUtils) in enumerate(
            zip(data_input, results_lgbm, results_MVAUtils), 1):
        if not np.allclose(output_lgbm, output_MVAUtils):
            stop_event_loop = True
            logging.info("output are different on input %d/%d:\n" %
                         (ievent, len(data_input)))
            for ivar, input_value in enumerate(input_values):
                logging.info("var %d: %.15f", ivar, input_value)
            logging.info("=" * 50)
            logging.info("              mvautils       lgbm")
            for ioutput, (o1, o2) in enumerate(
                zip(output_MVAUtils,
                    output_lgbm)):
                diff_flag = "" if np.allclose(o1, o2) else "<---"
                logging.info("output %3d    %.5e    %.5e  %s",
                             ioutput, o1, o2, diff_flag)
            output_trees_lgbm = lgbm_rawresponse_each_tree(
                booster, [input_values])

            stop_tree_loop = False
            for itree, output_tree_lgbm in enumerate(output_trees_lgbm):
                output_tree_mva_utils = [mva_utils.GetTreeResponse(
                    list2stdvector(
                        input_values), itree * nclasses + c)
                    for c in range(nclasses)]
                if not np.allclose(output_tree_mva_utils, output_tree_lgbm[0]):
                    stop_tree_loop = True
                    logging.info(
                        "first tree/class with different answer (%d)" % itree)
                    for isubtree, (ol, om) in enumerate(
                            zip(output_tree_lgbm[0], output_tree_mva_utils)):
                        if not np.allclose(ol, om):
                            logging.info("different in position %d" % isubtree)
                            logging.info("lgbm:     %f" % ol)
                            logging.info("mvautils: %f" % om)
                            logging.info("=" * 50)
                            logging.info("tree %d (itree) * %d (nclasses)"
                                         "+ %d (isubtree) = %d",
                                         itree, nclasses,
                                         isubtree,
                                         itree * nclasses + isubtree)
                            mva_utils.PrintTree(itree * nclasses + isubtree)

                            node_infos = []

                            # we now which tree is failing, check if this is
                            # due to input values very close to the threshold
                            # the problem is that lgbm is using double,
                            # while mva_utils is using float
                            def ff(tree):
                                if 'left_child' in tree:
                                    node_infos.append(
                                        (tree['split_feature'],
                                         tree['threshold']))
                                    ff(tree['left_child'])
                                    ff(tree['right_child'])

                            ff(
                                booster.dump_model()[
                                    'tree_info'][
                                        itree * nclasses + isubtree][
                                            'tree_structure'])
                            for node_info in node_infos:
                                value = input_values[node_info[0]]
                                threshold = node_info[1]
                                if (not np.isnan(value)
                                    and (value <= threshold) !=
                                    (np.float32(value) <=
                                        np.float32(threshold))):
                                    logging.info(
                                        "the problem could be due to double"
                                        "(lgbm) -> float (mvautil) conversion"
                                        "for variable %d: %f and threshold %f",
                                        node_info[0], value, threshold)
                                    stop_tree_loop = False
                                    stop_event_loop = False

                            if stop_tree_loop:
                                break
            if stop_event_loop:
                return False
    return True


def check_file(fn):
    f = ROOT.TFile.Open(fn)
    keys = f.GetListOfKeys()
    keys = list(keys)
    if len(keys) != 1:
        logging.info("file %s is empty", fn)
        return False
    tree = f.Get(keys[0].GetName())
    if type(tree) != ROOT.TTree:
        logging.info("cannot find TTree in file %s", fn)
        return False
    if not tree.GetEntries():
        logging.info("tree is empty")
        return False
    return True


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', help='input text file from LGBM')
    parser.add_argument('output', help='output ROOT filename', nargs='?')
    parser.add_argument('--tree-name', default='lgbm')
    parser.add_argument('--no-test', action='store_true',
                        help="don't run test (not suggested)")
    parser.add_argument('--ntests', type=int, default=1000,
                        help="number of random test, default=1000")
    parser.add_argument('--test-file', help='numpy table')

    args = parser.parse_args()

    if args.output is None:
        import os
        args.output = os.path.splitext(
            os.path.split(args.input)[1])[0] + '.root'

    logging.info("converting input file %s to root file %s",
                 args.input, args.output)
    output_treename = convertLGBMToRootTree(
        args.input, args.output, args.tree_name)
    if args.no_test:
        print("model has not been tested. Do not use it production!")
    else:
        logging.info("testing model")
        if not check_file(args.output):
            print("problem when checking file")
        result = test(args.input, args.output, args.tree_name,
                      args.ntests, args.test_file)
        if not result:
            print(
                "some problems during test."
                "Have you setup athena? Do not use this in production!")
        else:
            try:
                print(u"::: :) :) :)  everything fine:"
                      "LGBM output == MVAUtils output :) :) :) :::")
            except UnicodeEncodeError:
                print(":::==> everything fine:"
                      "LGBM output == MVAUtils output <==:::")
            booster = lgb.Booster(model_file=args.input)
            objective = booster.dump_model()['objective']
            if 'multiclass' in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig: %s
// ...
std::vector<float> output = my_bdt.GetMultiResponse(input_values, %d);
                ''' % (args.output,
                       output_treename,
                       booster.num_feature(),
                       ','.join(booster.feature_name()),
                       booster.num_model_per_iteration()))
            elif 'binary' in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig: %s
// ...
float output = my_bdt.GetClassification(input_values);
                ''' % (args.output,
                       output_treename,
                       booster.num_feature(),
                       ','.join(booster.feature_name())))
            elif 'regression' in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig: %s
// ...
float output = my_bdt.Predict(input_values);
                ''' % (args.output, output_treename,
                       booster.num_feature(),
                       ','.join(booster.feature_name())))
