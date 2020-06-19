#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "Convert XGBoost model to TTree to be used with MVAUtils."
__author__ = "Yuan-Tang Chou"


try:
    import xgboost as xgb
except ImportError:
    print("""cannot load xgboost. Try to install it with
   pip install xgboost
   """)
import ROOT
import time
import json
import logging
logging.basicConfig(level=logging.DEBUG)


class XBGoostTextNode(dict):
    """
    Adaptor from XGBoost dictionary to tree
       * XGboost Yes is < and No is >=
    """

    def get_split_feature(self):
        if 'split' in self:
            return self['split']
        else:
            return -1

    def get_value(self):
        if 'split_condition' in self:
            return self['split_condition']
        else:
            return self['leaf']

    def get_left(self):
        if 'children' not in self:
            return None
        else:  # XGBoost "YES" is left branch in MVAUtils
            return XBGoostTextNode(self['children'][self.get_nodeid('yes')])

    def get_right(self):
        if 'children' not in self:
            return None
        else:  # XGBoost "NO" is right branch in MVAUtils
            return XBGoostTextNode(self['children'][self.get_nodeid('no')])

    def get_nodeid(self, type):
        for idx, children in enumerate(self['children']):
            if children['nodeid'] == self[type]:
                return idx

    def get_default_left(self):
        if 'children' not in self:
            return True
        elif self.get_nodeid('yes') == self.get_nodeid('missing'):
            return True
        else:
            return False


def dump_tree(tree_structure):
    """
    dump a single decision tree to arrays to be written into the TTree
    """

    split_values = []
    split_features = []
    default_left = []
    top = XBGoostTextNode(tree_structure)

    def preorder(node):
        # visit root
        split_features.append(node.get_split_feature())
        split_values.append(node.get_value())
        default_left.append(node.get_default_left())

        # visit (yes)left
        if node.get_left() is not None:
            preorder(node.get_left())
        # visit (no)right
        if node.get_right() is not None:
            preorder(node.get_right())

    preorder(top)
    return split_features, split_values, default_left

def dump2ROOT(model, output_filename, output_treename='xgboost'):
    model.dump_model('dump_model.json', dump_format='json')
    with open('dump_model.json', 'r') as dump_json:
        model_dump = dump_json.read()
    trees = json.loads(model_dump)
    fout = ROOT.TFile.Open(output_filename, 'recreate')

    features_array = ROOT.std.vector('int')()
    values_array = ROOT.std.vector('float')()
    default_lefts_array = ROOT.std.vector('bool')()

    title = 'creator=xgboost'
    root_tree = ROOT.TTree(output_treename, title)
    root_tree.Branch('vars', 'vector<int>', ROOT.AddressOf(features_array))
    root_tree.Branch('values', 'vector<float>', ROOT.AddressOf(values_array))
    root_tree.Branch('default_left', 'vector<bool>', ROOT.AddressOf(default_lefts_array))

    logging.info("tree support nan: using XGBoost implementation")

    for tree in trees:
        tree_structure = tree
        features, values, default_lefts = dump_tree(tree_structure)

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

def convertXGBoostToRootTree(model, output_filename, tree_name='xgboost'):
    """
    Model: - a string, in this case, it is the name of the input file containing the xgboost model
             you can get this model with xgboost with `bst.save_model('my_model.model')
           - directly a xgboost booster object
    """
    if type(model) is str:
        bst = xgb.Booster()
        bst.load_model(model)
        return dump2ROOT(bst, output_filename, tree_name)
    else:
        return dump2ROOT(model, output_filename, tree_name)


def test(model_file, tree_file, objective, tree_name='xgboost', ntests=10000, test_file=None):
    bst = xgb.Booster()
    bst.load_model(model_file)
    f = ROOT.TFile.Open(tree_file)
    tree = f.Get(tree_name)
    try:
        _ = ROOT.MVAUtils.BDT
    except:
        print("cannot import MVAUtils")
        return None

    mva_utils = ROOT.MVAUtils.BDT(tree)

    if 'binary' in objective:
        logging.info("testing binary")
        return test_binary(bst, mva_utils, objective, ntests, test_file)
    elif 'multi' in objective:
        logging.info("testing multi-class")
        return test_multiclass(bst,mva_utils, objective, ntests, test_file)
    else:
        logging.info("testing regression")
        return test_regression(bst, mva_utils, objective, ntests, test_file)

def test_regression(booster, mva_utils, objective, ntests=10000, test_file=None):
    import numpy as np
    logging.info("Tesing input features with regression")

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.error("Please provide an input test file for testing")

    start = time.time()
    dTest = xgb.DMatrix(data_input)
    results_xgboost = booster.predict(dTest)
    logging.info("xgboost (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

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

    for input_values, output_xgb, output_MVAUtils in zip(data_input, results_xgboost, results_MVAUtils):
        if not np.allclose(output_xgb, output_MVAUtils, rtol=1E-4):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "xgboost: %s\n"
                         "inputs: %s", output_MVAUtils, output_xgb, input_values)
            return False
    return True


def test_binary(booster, mva_utils, objective, ntests=10000, test_file=None):
    import numpy as np
    logging.info("Testing input features with binary classification")
    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.error("Please provide an input test file for testing")

    start = time.time()
    dTest = xgb.DMatrix(data_input)
    results_xgboost = booster.predict(dTest)
    logging.info("xgboost (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

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

    for input_values, output_xgb, output_MVAUtils in zip(data_input, results_xgboost, results_MVAUtils):
        if not np.allclose(output_xgb, output_MVAUtils):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "xgboost: %s\n"
                         "inputs: %s", output_MVAUtils, output_xgb, input_values)
            return False
    return True

def test_multiclass(booster, mva_utils, objective, ntests=10000, test_file=None):
    import numpy as np
    logging.info("using multiclass model")

    if test_file is not None:
        data_input = np.load(test_file)
        logging.info("using as input %s inputs from file %s", len(data_input), test_file)
    else:
        logging.error("Please provide an input test file for testing")

    start = time.time()
    dTest = xgb.DMatrix(data_input)
    results_xgboost = booster.predict(dTest)
    ##Extract the number of classes from the python prediction
    nclasses = results_xgboost.shape[1]
    logging.info("xgboost (vectorized) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    input_values_vector = ROOT.std.vector("float")()
    results_MVAUtils = []
    start = time.time()
    for input_values in data_input:
        input_values_vector.clear()
        for v in input_values:
            input_values_vector.push_back(v)
        output_MVAUtils = mva_utils.GetMultiResponse(input_values_vector, nclasses)
        results_MVAUtils.append(output_MVAUtils)

    logging.info("mvautils (not vectorized+overhead) timing = %s ms/input", (time.time() - start) * 1000 / len(data_input))

    for input_values, output_xgb, output_MVAUtils in zip(data_input, results_xgboost, results_MVAUtils):
        if not np.allclose(output_xgb, output_MVAUtils):
            logging.info("output are different:"
                         "mvautils: %s\n"
                         "xgboost: %s\n"
                         "inputs: %s", output_MVAUtils, output_xgb, input_values)
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
    parser.add_argument('input', help='input xgboost model')
    parser.add_argument('output', type=str, default ='xgboost_model.root', help='Output file name, it must end with .root')
    parser.add_argument('--tree-name', type=str, default = 'xgboost', help="tree name in Output root file")
    parser.add_argument('--no-test', action='store_true', help="don't run test (not suggested)")
    parser.add_argument('--ntests', type=int, default=1000, help="number of random test, default=1000")
    parser.add_argument('--test-file', type=str, help='numpy table')
    parser.add_argument('--objective', type=str, help='Specify the learning task and the corresponding learning objective, currently support options: binary:logistic, reg:linear(squarederror), multi:softprob')

    args = parser.parse_args()
    logging.info("converting input file %s to root file %s", args.input, args.output)

    #  'reg:linear'is been named as 'reg:squarederror' in newer version of xgboost (> 0.90)
    supported_objective = ['binary:logistic', 'reg:linear', 'reg:squarederror','multi:softprob']

    if args.objective not in supported_objective:
        parser.error('''
                     Current version does NOT support this objective!!
                     Only the following objectives are supported and tested:
                     - binary:logistic
                     - reg:linear(or squarederror)
                     - multi:softprob
                     ''')

    if not args.input:
        parser.error('Model file name not given!')

    if "root" not in args.output:
        parser.error("The outputfile name must end with .root!!")

    output_treename = convertXGBoostToRootTree(args.input, args.output, args.tree_name)

    if args.no_test:
        print("model has not been tested. Do not use it production!")
    else:
        logging.info("testing model")
        if not args.test_file:
            parser.error("Attempting to do test but no test file was provided, pass this with '--test-file <test_file> or use option '--no_test' ")
        if not check_file(args.output):
            print("problem when checking file")
        result = test(args.input, args.output, args.objective, args.tree_name, args.ntests, args.test_file)
        if not result:
            print("some problems during test. Have you setup athena? Do not use this in production!")
        else:
            print(u"::: everything fine: XGBoost output == MVAUtils output :::")
            objective = args.objective
            import numpy as np
            data = np.load(args.test_file)
            if 'binary' in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig
// ...
float output = my_bdt.GetClassification(input_values);
                ''' % (args.output, output_treename, len(data[0])))
            elif 'reg' in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig
// ...
float output = my_bdt.Predict(input_values);
                ''' % (args.output, output_treename, len(data[0])))
            elif "multi" in objective:
                print('''In c++ use your BDT as:
#include "MVAUtils/BDT.h"

TFile* f = TFile::Open("%s");
TTree* tree = nullptr;
f->GetObject("%s", tree);
MVAUtils::BDT my_bdt(tree);
// ...
// std::vector<float> input_values(%d, 0.);
// fill the vector using the order as in the trainig
// ...
float output = my_bdt.GetMultiResponse(input_values, nclasses);
''' % (args.output, output_treename, len(data[0])))
