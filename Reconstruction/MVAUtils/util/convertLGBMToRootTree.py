#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
            raise ValueError('do not support categorical input BDT (decision_type = %s)' % node['decision_type'])

        if 'missing_type' in node:
            if node['missing_type'] not in ('NaN', 'None'):
                raise ValueError('do not support missing values different from NaN or None')

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
        features, values, default_lefts, simple_tree = dump_tree(tree_structure)
        if not simple_tree:
            simple = False
            node_type = 'node_type=lgbm'

    infos = ';'.join(['%s=%s' % (k, str(v)) for k, v in model.items() if type(v) != list])
    title = ';'.join(('creator=lgbm', node_type, infos))
    root_tree = ROOT.TTree(output_treename, title)
    root_tree.Branch('vars', 'vector<int>', ROOT.AddressOf(features_array))
    root_tree.Branch('values', 'vector<float>', ROOT.AddressOf(values_array))

    if not simple:
        logging.info("tree support nan: using full implementation (LGBMNode)")
        root_tree.Branch('default_left', 'vector<bool>', ROOT.AddressOf(default_lefts_array))
    if simple:
        logging.info("tree do not support nan: using simple implementation (LGBMNodeSimple)")

    for tree in model['tree_info']:
        tree_structure = tree['tree_structure']
        features, values, default_lefts, simple_tree = dump_tree(tree_structure)

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
    Model: - a string, in this case, it is the name of the input file containing the lgbm model
             you can get this model with lgbm with `boosted.save_model('my_model.txt')
           - directly a lgbm booster object
    """
    if type(model) is str:
        model = lgb.Booster(model_file=model)
        return dump2ROOT(model, output_filename, tree_name)
    else:
        return dump2ROOT(model, output_filename, tree_name)


def test(model_file, tree_file, tree_name='lgbm', ntests=10000, test_file=None):
    booster = lgb.Booster(model_file=model_file)
    f = ROOT.TFile.Open(tree_file)
    tree = f.Get(tree_name)
    try:
        _ = ROOT.MVAUtils.BDT
    except:
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
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.info("using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning("using random uniform input as test: this is not safe, provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = mva_utils.GetResponse(input_values_vector)
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    for input_values, output_lgbm, output_MVAUtils in zip(data_input, results_lgbm, results_MVAUtils):
        if not np.allclose(output_lgbm, output_MVAUtils, rtol=1E-4):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "lgbm: %s\n"
                         "inputs: %s", output_MVAUtils, output_lgbm, input_values)
            return False
    return True


def test_binary(booster, mva_utils, ntests=10000, test_file=None):
    import numpy as np
    nvars = booster.num_feature()
    logging.info("using %d input features with binary classification", nvars)

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.info("using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning("using random uniform input as test: this is not safe, provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = mva_utils.GetClassification(input_values_vector)
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    for input_values, output_lgbm, output_MVAUtils in zip(data_input, results_lgbm, results_MVAUtils):
        if not np.allclose(output_lgbm, output_MVAUtils):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "lgbm: %s\n"
                         "inputs: %s", output_MVAUtils, output_lgbm, input_values)
            return False
    return True


def test_multiclass(booster, mva_utils, ntests=10000, test_file=None):
    import numpy as np
    nvars = booster.num_feature()
    nclasses = booster.num_model_per_iteration()
    logging.info("using %d input features with %d classes", nvars, nclasses)

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.info("using as input %s random uniform inputs (-100,100)", ntests)
        logging.warning("using random uniform input as test: this is not safe, provide an input test file")
        data_input = np.random.uniform(-100, 100, size=(ntests, nvars))

    start = time.time()
    results_lgbm = booster.predict(data_input)
    logging.info("lgbm (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = list(mva_utils.GetMultiResponse(input_values_vector, nclasses))
        results_MVAUtils.append(output_MVAUtils)
    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    for input_values, output_lgbm, output_MVAUtils in zip(data_input, results_lgbm, results_MVAUtils):
        if not np.allclose(output_lgbm, output_MVAUtils):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "lgbm: %s\n"
                         "inputs: %s", output_MVAUtils, output_lgbm, input_values)
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
    parser.add_argument('output', help='output ROOT filename')
    parser.add_argument('--tree-name', default='lgbm')
    parser.add_argument('--no-test', action='store_true', help="don't run test (not suggested)")
    parser.add_argument('--ntests', type=int, default=1000, help="number of random test, default=1000")
    parser.add_argument('--test-file', help='numpy table')

    args = parser.parse_args()
    logging.info("converting input file %s to root file %s", args.input, args.output)
    output_treename = convertLGBMToRootTree(args.input, args.output, args.tree_name)
    if args.no_test:
        print("model has not been tested. Do not use it production!")
    else:
        logging.info("testing model")
        if not check_file(args.output):
            print("problem when checking file")
        result = test(args.input, args.output, args.tree_name, args.ntests, args.test_file)
        if not result:
            print("some problems during test. Have you setup athena? Do not use this in production!")
        else:
            print(u":::😀😀😀 everything fine: LGBM output == MVAUtils output 😀😀😀:::")
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
                ''' % (args.output, output_treename, booster.num_feature(), ','.join(booster.feature_name()), booster.num_model_per_iteration()))
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
                ''' % (args.output, output_treename, booster.num_feature(), ','.join(booster.feature_name())))
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
                ''' % (args.output, output_treename, booster.num_feature(), ','.join(booster.feature_name())))
