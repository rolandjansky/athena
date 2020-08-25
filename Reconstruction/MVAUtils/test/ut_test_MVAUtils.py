#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "Test for MVAUtils, check simple hardcoded cases"
__author__ = "Ruggero Turra"


import unittest
import ROOT
from array import array
from math import exp


ROOT.PyConfig.IgnoreCommandLineOptions = True


def create_tree(variables, values, offset, default_lefts=None, tree_title='tree'):
    """
    convenient function to create a TTree from list of inputs, to be used as
    input of MVAUtils/BDT
    variables: a list of variable indexes. Each element is a list relative to a
          different decision tree, for each node
    values: a list of threshoulds or leaf values. Each element is a list
            relative to a different decision tree, for each node
    offset: a list of offset. Each tree has one offset
    """
    tree = ROOT.TTree('tree', tree_title)

    variables_array = ROOT.std.vector('int')()
    values_array = ROOT.std.vector('float')()
    offset_array = array('f', [0.])
    default_left_array = ROOT.std.vector('bool')()

    tree.Branch('vars', 'vector<int>', ROOT.AddressOf(variables_array))
    tree.Branch('values', 'vector<float>', ROOT.AddressOf(values_array))

    if offset is not None:
        tree.Branch('offset', offset_array, 'offset/F')
    else:
        offset = [0.] * len(values)

    if default_lefts is not None:
        tree.Branch('default_left', 'vector<bool>', default_left_array)
    else:
        default_lefts = [True] * len(variables)

    for var, val, off, dl in zip(variables, values, offset, default_lefts):
        variables_array.clear()
        values_array.clear()
        default_left_array.clear()

        for v in val:
            values_array.push_back(v)
        for v in var:
            variables_array.push_back(v)
        if dl is not True:
            for v in dl:
                default_left_array.push_back(v)

        offset_array[0] = off

        tree.Fill()
    return tree


def list2stdvector(values, dtype='float'):
    result = ROOT.std.vector(dtype)()
    for v in values:
        result.push_back(v)
    return result


def lgbm_rawresponse_each_tree(model, my_input):
    import numpy as np
    nclasses = model.num_model_per_iteration()
    output_values = np.array([np.array([[0] * nclasses])] + [model.predict(my_input, raw_score=True, num_iteration=itree) for itree in range(1, (model.num_trees() / nclasses + 1))])
    output_trees = np.diff(output_values, axis=0)
    return output_trees


class TestMVAUtilsBasic(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # just two trees. First tree:
        #      [var[2] > 3.15]
        #      |
        #      +--yes (or nan)-> [var[0] > 6.45]
        #      |                 |
        #      |                 +--yes         -> [leaf: -1.23]
        #      |                 +--no (or nan) -> [leaf: -1.23]
        #      |--no--> [leaf: -1.]
        cls.variables = [[2, 0, -1, -1, -1],
                         [3, 2, -1, -1, -1]]
        cls.values = [[3.15, 6.45, -1.23, -1.23, -1.],
                      [0.5, 4.95, -1.167, -0.98, -1.17]]
        cls.offset = [10., 20.]
        cls.default_lefts = [[True, False, True, True, True],
                             [False, False, True, True, True]]
        cls.basic_tree = create_tree(cls.variables, cls.values, cls.offset, tree_title='creator=TMVA')
        cls.lgbm_tree = create_tree(cls.variables, cls.values, offset=None, tree_title='creator=lgbm;node_type=lgbm_simple')
        cls.lgbm_tree_nan = create_tree(cls.variables, cls.values, None, cls.default_lefts, tree_title='creator=lgbm;node_type=lgbm')
        cls.xgb_tree = create_tree(cls.variables, cls.values, None, cls.default_lefts, tree_title='creator=xgboost')

    def test_nvars(self):
        bdt = ROOT.MVAUtils.BDT(self.basic_tree)
        self.assertEqual(bdt.GetNVars(), 4)

        bdt = ROOT.MVAUtils.BDT(self.lgbm_tree)
        self.assertEqual(bdt.GetNVars(), 4)

        bdt = ROOT.MVAUtils.BDT(self.lgbm_tree_nan)
        self.assertEqual(bdt.GetNVars(), 4)

        bdt = ROOT.MVAUtils.BDT(self.xgb_tree)
        self.assertEqual(bdt.GetNVars(), 4)

    def test_initializationTMVA(self):
        bdt = ROOT.MVAUtils.BDT(self.basic_tree)

        self.assertEqual(bdt.GetNTrees(), len(self.variables))
        # GetOffset returns only the first
        self.assertAlmostEqual(bdt.GetOffset(), self.offset[0])

    def test_GetResponseTMVA(self):
        bdt = ROOT.MVAUtils.BDT(self.basic_tree)

        my_inputs = list2stdvector([0., 2., 3., 4.])

        # first tree select -1
        # second tree select -0.98
        # GetResponse just return the sum plus the offset (10)
        # For some reason need to reduce the number of digits from the default
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), 10. - 1. - 0.98, places=5)

        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), 10. - 1.23 - 0.98, places=5)

        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), 10. - 1. - 1.17, places=5)

    def test_GetGradBoostMVA(self):
        def normalization(x):
            return 2. / (1. + exp(-2 * x)) - 1.

        bdt = ROOT.MVAUtils.BDT(self.basic_tree)

        my_inputs = list2stdvector([0., 2., 3., 4.])
        self.assertAlmostEqual(bdt.GetGradBoostMVA(my_inputs), normalization(-1. - 0.98), places=5)
        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetGradBoostMVA(my_inputs), normalization(-1.23 - 0.98), places=5)
        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetGradBoostMVA(my_inputs), normalization(-1. - 1.17), places=5)

    def test_GetClassificationTMVA(self):
        bdt = ROOT.MVAUtils.BDT(self.basic_tree)

        sumw = sum(self.offset)

        my_inputs = list2stdvector([0., 2., 3., 4.])
        self.assertAlmostEqual(bdt.GetClassification(my_inputs),
                               (-1 * self.offset[0] - 0.98 * self.offset[1]) / sumw, places=5)
        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetClassification(my_inputs),
                               (-1.23 * self.offset[0] - 0.98 * self.offset[1]) / sumw, places=5)
        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetClassification(my_inputs),
                               (-1. * self.offset[0] - 1.17 * self.offset[1]) / sumw, places=5)

    def test_GetMultiResponse(self):
        bdt = ROOT.MVAUtils.BDT(self.basic_tree)

        def normalization(x):
            den = sum([exp(xx) for xx in x])
            return [exp(xx) / den for xx in x]

        def do_test(my_input, results):
            r = list(bdt.GetMultiResponse(my_input, 2))
            # numpy has a mehod to compare lists
            for a, b in zip(r, results):
                self.assertAlmostEqual(a, b, places=5)

        my_inputs = list2stdvector([0., 2., 3., 4.])
        do_test(my_inputs, normalization([-1, -0.98]))
        my_inputs = list2stdvector([4., 0., 4., 2.])
        do_test(my_inputs, normalization([-1.23, -0.98]))
        my_inputs = list2stdvector([0., 0., 0., 0.])
        do_test(my_inputs, normalization([-1., -1.17]))

    def test_GetResponseLGBMSimple(self):
        bdt = ROOT.MVAUtils.BDT(self.lgbm_tree)
        my_inputs = list2stdvector([0., 2., 3., 4.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1 - 1.17, places=5)
        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.167, places=5)
        my_inputs = list2stdvector([6.45, 0.0, 4.95, 0.5])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1. - 1.167, places=5)

    def test_GetResponseLGBM(self):
        bdt = ROOT.MVAUtils.BDT(self.lgbm_tree_nan)
        my_inputs = list2stdvector([0., 2., 3., 4.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1 - 1.17, places=5)
        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.167, places=5)
        my_inputs = list2stdvector([6.45, 0.0, 4.95, 0.5])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1. - 1.167, places=5)

        try:
            import numpy as np
        except ImportError:
            # presently problem loading numpy in AthDerivation
            self.skipTest("cannot load numpy. Are you in AthDerivation?")

        my_inputs = list2stdvector([np.nan, 2., np.nan, 4.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([4., np.nan, 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1 - 1.17, places=5)
        my_inputs = list2stdvector([np.nan, np.nan, np.nan, np.nan])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([6.45, 0.0, np.nan, np.nan])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)

def test_GetResponseXGBoost(self):
        bdt = ROOT.MVAUtils.BDT(self.xgb_tree)
        my_inputs = list2stdvector([0., 2., 3., 4.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([4., 0., 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1 - 1.17, places=5)
        my_inputs = list2stdvector([0., 0., 0., 0.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.167, places=5)
        my_inputs = list2stdvector([6.45, 0.0, 4.95, 0.5])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1. - 1.17, places=5)

        try:
            import numpy as np
        except ImportError:
            # presently problem loading numpy in AthDerivation
            self.skipTest("cannot load numpy. Are you in AthDerivation?")

        my_inputs = list2stdvector([np.nan, 2., np.nan, 4.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([4., np.nan, 4., 2.])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1 - 1.17, places=5)
        my_inputs = list2stdvector([np.nan, np.nan, np.nan, np.nan])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)
        my_inputs = list2stdvector([6.45, 0.0, np.nan, np.nan])
        self.assertAlmostEqual(bdt.GetResponse(my_inputs), -1.23 - 1.17, places=5)

class TestMVAUtilsLGBMConversion(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # import lightgbm, if not present try to install from pip
        cls.lgb_enabled = False
        try:
            import lightgbm
            cls.lgb_enabled = True
        except ImportError:
            pass

        if not cls.lgb_enabled:
            import os
            # this works on lxplus (Successfully installed lightgbm-2.3.0 scikit-learn-0.20.4 scipy-1.2.2)
            # pip is from /usr/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/pip
            print("trying to install lgbm")
            os.system('pip install numpy scipy scikit-learn')
            os.system('pip install --no-binary :all: lightgbm')
            try:
                import lightgbm
                cls.lgb_enabled = True
            except ImportError:
                pass

        if cls.lgb_enabled:
            cls.model_multiclass = cls.create_model_multiclass()
            cls.model_multiclass_nan = cls.create_model_multiclass(use_nan=True)
            cls.model_binary = cls.create_model_binary()
            cls.model_regression = cls.create_model_regression()
            cls.model_regression.save_model('regression.txt')


    @classmethod
    def create_model_binary(cls):
        import numpy as np
        import lightgbm as lgb

        SIZE = 1000
        NFEATURES = 4
        df_dependent = np.random.uniform(-100, 100, size=(SIZE, NFEATURES))
        df_true = (df_dependent[:, 0] - 3 * df_dependent[:, 2]
                   + 4 * df_dependent[:, 3] + df_dependent[:, 1])
        df_true = np.where(df_true > 0, 0, 1)
        df_dependent[:0, 30:40] = np.nan
        df_dependent[:1, 25:30] = np.nan
        df_dependent[:2, 40:50] = np.nan
        df_dependent[:3, 0:20] = np.nan

        params = {
            'task': 'train',
            'verbose': -1,
            'boosting_type': 'gbdt',
            'objective': 'binary',
            'metric': 'binary_logloss',
            'num_leaves': 50,
            'learning_rate': 0.1,
            'feature_fraction': 0.9,
            'bagging_fraction': 0.9,
            'bagging_freq': 1,
        }

        lgb_train = lgb.Dataset(df_dependent, df_true, params={'verbose': -1})
        cls.inputs_binary = df_dependent
        gbm = lgb.train(params,
                        lgb_train,
                        num_boost_round=200,
                        valid_sets=lgb_train,  # should be validation sample
                        verbose_eval=None,
                        early_stopping_rounds=5)
        return gbm

    @classmethod
    def create_model_regression(cls):
        import numpy as np
        import lightgbm as lgb

        SIZE = 500
        NFEATURES = 4
        df_dependent = np.random.uniform(-100, 100, size=(SIZE, NFEATURES))
        df_true = (df_dependent[:, 0] - 3 * df_dependent[:, 2]
                   + 4 * df_dependent[:, 3] + df_dependent[:, 1])
        df_dependent[:0, 30:40] = np.nan
        df_dependent[:1, 25:30] = np.nan
        df_dependent[:2, 40:50] = np.nan
        df_dependent[:3, 0:20] = np.nan

        params = {
            'task': 'train',
            'verbose': -1,
            'boosting_type': 'gbdt',
            'objective': 'regression',
            'metric': 'l2',
            'num_leaves': 50,
            'learning_rate': 0.1,
            'feature_fraction': 0.9,
            'bagging_fraction': 0.9,
            'bagging_freq': 1,
        }

        lgb_train = lgb.Dataset(df_dependent, df_true, params={'verbose': -1})
        cls.inputs_regression = df_dependent
        gbm = lgb.train(params,
                        lgb_train,
                        num_boost_round=200,
                        valid_sets=lgb_train,  # should be validation sample
                        verbose_eval=None,
                        early_stopping_rounds=5)
        return gbm

    @classmethod
    def create_model_multiclass(cls, use_nan=False):
        """
        download iris data and train a fast bdt with lgbm
        """
        from sklearn.datasets import load_iris
        from sklearn.model_selection import train_test_split
        import lightgbm as lgb
        import numpy as np

        iris = load_iris()
        cls.inputs_multiclass = df_features = iris.data
        df_dependent = iris.target

        x_train, x_test, y_train, y_test = train_test_split(df_features, df_dependent, test_size=0.3, random_state=2)
        params = {
            'task': 'train',
            'verbose': -1,
            'boosting_type': 'gbdt',
            'objective': 'multiclass',
            'metric': {'multi_logloss'},
            'num_leaves': 63,
            'learning_rate': 0.1,
            'feature_fraction': 0.9,
            'bagging_fraction': 0.9,
            'bagging_freq': 0,
            'num_class': 3
        }

        if use_nan:
            for i in np.where(y_train == 1)[0]:
                x_train[i, 1] = np.nan
            for i in np.where(y_test == 1)[0]:
                x_test[i, 1] = np.nan

        lgb_train = lgb.Dataset(x_train, y_train, params={'verbose': -1})
        lgb_eval = lgb.Dataset(x_test, y_test, reference=lgb_train, params={'verbose': -1})
        gbm = lgb.train(params,
                        lgb_train,
                        num_boost_round=200,
                        valid_sets=lgb_eval,
                        verbose_eval=False,
                        early_stopping_rounds=5)
        return gbm

    def _test_multi_response(self, model, root_name):
        if not self.lgb_enabled:
            self.skipTest("lgbm is not installed")
    
        from MVAUtils.convertLGBMToRootTree import convertLGBMToRootTree

        convertLGBMToRootTree(model, root_name, 'lgbm')
        f = ROOT.TFile.Open(root_name)
        tree = f.Get('lgbm')
        self.assertTrue(tree)
        mvautils = ROOT.MVAUtils.BDT(tree)

        self.assertEqual(model.num_trees(), tree.GetEntries())

        nclasses = 3
        for input_value in self.inputs_multiclass:
            # test output for each tree

            output_trees_lgbm = lgbm_rawresponse_each_tree(model, [input_value])

            for i, output_tree in enumerate(output_trees_lgbm):
                o1 = output_tree[0]
                o2 = [mvautils.GetTreeResponse(list2stdvector(input_value), i * nclasses + c) for c in range(nclasses)]
                for iclass, (oo1, oo2) in enumerate(zip(o1, o2)):
                    self.assertAlmostEqual(oo1, oo2, places=6, msg='output values are different (lgbm) %s!=%s (mvautils) for single tree'
                                           ' using input values %s for class %d' % (oo1, oo2, input_value, iclass))

            # test output for the full BDT
            o1 = model.predict([input_value])[0]
            o2 = mvautils.GetMultiResponse(list2stdvector(input_value), nclasses)
            for oo1, oo2 in zip(o1, o2):
                self.assertAlmostEqual(oo1, oo2, places=6, msg='output values are different %s!=%s from a full'
                                       ' multiclass output = %s %s. Input = %s' % (oo1, oo2, o1, o2, input_value))

    def test_multi_response(self):
        if not self.lgb_enabled:
            self.skipTest("lgbm is not installed")

        res = self._test_multi_response(self.model_multiclass,
                                        'test_lgbm_multiclass.root')
        return res

    def test_multi_reponse_nan(self):
        if not self.lgb_enabled:
            self.skipTest("lgbm is not installed")

        res = self._test_multi_response(self.model_multiclass_nan,
                                        'test_lgbm_multiclass_nan.root')
        return res

    def test_binary(self):
        if not self.lgb_enabled:
            self.skipTest("lgbm is not installed")

        from MVAUtils.convertLGBMToRootTree import convertLGBMToRootTree

        convertLGBMToRootTree(self.model_binary, 'test_lgbm_binary.root', 'lgbm')
        f = ROOT.TFile.Open('test_lgbm_binary.root')
        tree = f.Get('lgbm')
        self.assertTrue(tree)
        mvautils = ROOT.MVAUtils.BDT(tree)
        self.assertEqual(self.model_binary.num_trees(), tree.GetEntries())

        for input_value in self.inputs_binary:
            o1 = self.model_binary.predict([input_value])[0]
            v = list2stdvector(input_value)
            o2 = mvautils.GetClassification(v)
            self.assertAlmostEqual(o1, o2, places=6, msg='output values are different in binary classification %s!=%s'
                                   '.Input = %s' % (o1, o2, input_value))

    def test_regression(self):
        if not self.lgb_enabled:
            self.skipTest("lgbm is not installed")

        from MVAUtils.convertLGBMToRootTree import convertLGBMToRootTree

        convertLGBMToRootTree(self.model_regression, 'test_lgbm_regression.root', 'lgbm')
        f = ROOT.TFile.Open('test_lgbm_regression.root')
        tree = f.Get('lgbm')
        self.assertTrue(tree)
        mvautils = ROOT.MVAUtils.BDT(tree)
        self.assertEqual(self.model_regression.num_trees(), tree.GetEntries())
        for input_value in self.inputs_regression:
            output_trees_lgbm = lgbm_rawresponse_each_tree(self.model_regression, [input_value])

            v = list2stdvector(input_value)

            for itree, output_tree_lgbm in enumerate(output_trees_lgbm):
                o1 = output_tree_lgbm[0]
                o2 = mvautils.GetTreeResponse(v, itree)
                self.assertAlmostEqual(o1, o2, places=5)

            o1 = self.model_regression.predict([input_value])[0]
            o2 = mvautils.GetResponse(v)
            self.assertAlmostEqual(o1, o2, delta=abs(1E-4 * o1), msg='output values are different in regression classification %s!=%s (diff=%s)'
                                   '. Input = %s' % (o1, o2, o1 - o2, input_value))

class TestMVAUtilsXGBoostConversion(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # import xgboost, if not present try to install from pip
        cls.xgb_enabled = False
        try:
            import xgboost as xgb
            cls.xgb_enabled = True
        except ImportError:
            pass

        if not cls.xgb_enabled:
            import os
            # this works on lxplus (Successfully installed xgboost-0.82 scipy-1.2.2)
            # XGBoost DO NOT support python 2.7 after version 0.82. This is the latest verion we can use.
            print("trying to install xgboost")
            os.system('pip install numpy scipy scikit-learn')
            os.system('pip install xgboost==0.82')
            try:
                import xgboost as xgb
                cls.xgb_enabled = True
            except ImportError:
                pass

        if cls.xgb_enabled:
            cls.model_binary = cls.create_model_binary()
            cls.model_binary.save_model('test_xgb_binary.model')
            cls.model_regression = cls.create_model_regression()
            cls.model_regression.save_model('test_xgb_regression.model')


    @classmethod
    def create_model_binary(cls):
        import numpy as np
        import xgboost as xgb

        SIZE = 1000
        NFEATURES = 4
        df_dependent = np.random.uniform(-100, 100, size=(SIZE, NFEATURES))
        df_true = (df_dependent[:, 0] - 3 * df_dependent[:, 2]
                   + 4 * df_dependent[:, 3] + df_dependent[:, 1])
        df_true = np.where(df_true > 0, 0, 1)
        df_dependent[:0, 30:40] = np.nan
        df_dependent[:1, 25:30] = np.nan
        df_dependent[:2, 40:50] = np.nan
        df_dependent[:3, 0:20] = np.nan

        params = {
            'max_depth': 5,
            'eta': 0.2,
            'objective': 'binary:logistic',
            'verbosity': 0,
            'eval_metric': ['logloss'],
        }

        xgb_train = xgb.DMatrix(df_dependent, label=df_true)
        cls.inputs_binary = df_dependent
        bst = xgb.train(params, 
                        xgb_train,
                        num_boost_round=100
                        )

        return bst

    @classmethod
    def create_model_regression(cls):
        import numpy as np
        import xgboost as xgb

        SIZE = 500
        NFEATURES = 4
        df_dependent = np.random.uniform(-100, 100, size=(SIZE, NFEATURES))
        df_true = (df_dependent[:, 0] - 3 * df_dependent[:, 2]
                   + 4 * df_dependent[:, 3] + df_dependent[:, 1])
        df_dependent[:0, 30:40] = np.nan
        df_dependent[:1, 25:30] = np.nan
        df_dependent[:2, 40:50] = np.nan
        df_dependent[:3, 0:20] = np.nan

        params = {
            'max_depth': 5,
            'eta': 0.2,
            'objective': 'reg:linear',
            'verbosity': 0,
            'eval_metric': ['mae'],
            'base_score':0
        }

        xgb_train = xgb.DMatrix(df_dependent, label=df_true)
        cls.inputs_regression = df_dependent
        bst = xgb.train(params, 
                        xgb_train,
                        num_boost_round=100,
                        )

        return bst

    def test_binary(self):
        if not self.xgb_enabled:
            self.skipTest("xgboost is not installed")
        import xgboost as xgb
        from MVAUtils.convertXGBoostToRootTree import convertXGBoostToRootTree

        convertXGBoostToRootTree('test_xgb_binary.model', 'test_xgb_binary.root', 'xgboost')
        f = ROOT.TFile.Open('test_xgb_binary.root')
        tree = f.Get('xgboost')
        self.assertTrue(tree)
        mvautils = ROOT.MVAUtils.BDT(tree)
        self.assertEqual(self.model_binary.best_ntree_limit, tree.GetEntries())

        input_values = xgb.DMatrix(self.inputs_binary)
        output_predicts = self.model_binary.predict(input_values)
        for idx, input_value in enumerate(self.inputs_binary):
            o1 = output_predicts[idx]    
            v = list2stdvector(input_value)
            o2 = mvautils.GetClassification(v)
            self.assertAlmostEqual(o1, o2, places=6, msg='output values are different in binary classification %s!=%s'
                                   '.Input = %s' % (o1, o2, input_value))

    def test_regression(self):
        if not self.xgb_enabled:
            self.skipTest("xgboost is not installed")

        import xgboost as xgb
        from MVAUtils.convertXGBoostToRootTree import convertXGBoostToRootTree

        convertXGBoostToRootTree('test_xgb_regression.model', 'test_xgb_regression.root', 'xgboost')
        f = ROOT.TFile.Open('test_xgb_regression.root')
        tree = f.Get('xgboost')
        self.assertTrue(tree)
        mvautils = ROOT.MVAUtils.BDT(tree)
        self.assertEqual(self.model_regression.best_ntree_limit, tree.GetEntries())

        input_values = xgb.DMatrix(self.inputs_regression)
        self.model_regression.predict(input_values) 
        output_predicts = self.model_regression.predict(input_values)
        for idx, input_value in enumerate(self.inputs_regression):
            o1 = output_predicts[idx]
            v = list2stdvector(input_value)
            o2 = mvautils.GetResponse(v)
            self.assertAlmostEqual(o1, o2, delta=abs(1E-4 * o1), msg='output values are different in regression classification %s!=%s (diff=%s)'
                                   '. Input = %s' % (o1, o2, o1 - o2, input_value))


if __name__ == '__main__':
    unittest.main()
