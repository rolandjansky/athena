# MVAUtils

MVAUtils is a lightweight implementation of Boosted Decision Trees (BDT). It is able to compute the output of BDT optimized with several tools (presently TMVA, lgbm, and XGBoost). It has been originally designed to redude the memory usage by the TMVA::Reader. It is suggested to use this tool instead of the native api provided by the tool used in the training.

`MVAUtils::BDT` is the class the user has to instantiate. The constructor get a `TTree*` which holds the weights. Several tools are provided to convert the original format to a ROOT file containing the needed TTree.

## Usage
To predict the output of the BDT you should use one of these method:

   * Regression: `float GetResponse(const std::vector<float>& values)`
   * Classification: `float GetClassification(const std::vector<float>& values)`
   * Multiclassification `std::vector<float> GetMultiResponse(const std::vector<float>& values, unsigned int numClasses)`

The input is a `std::vector<float>` but other interfaces are provided, in particular one without any inputs (e.g. `float GetResponse()`). In this case, before calling the method, `void SetPointers(std::vector<float*>& pointers)` should be called. For example:

   
    float my_var1, my_var2;
    std::vector<float*> my_vars({&my_var1, &my_var2});
    my_bdt.SetPointers(my_vars);
    for (...) {
      // update my_var1, my_var2 values
      float response = my_bdt.GetResponse();
    }

By the way in most of the cases the first interface (passing `std::vector<float>` each time) should be preferred. The interface with pointers is not thread-safe.

## Convert weights to TTree
If you have optimized a BDT with TMVA, lgbm, or XGBoost you need to convert the weights to a TTree. The script after converting to a ROOT file checks if the output computed with MVAUtils is the same as the one predicted by the original tool (pay attention to the output of the script).

### TMVA
Save your optimization in an xml using TMVA. Then simply run:

    convertXmlToRootTree myxml.xml

### lgbm
Save your optimization (the booster) to a txt file, with

    booster.save_model('my_model.txt')

Then call the script:

    convertLGBMToRooTree.py my_model.txt my_output.root

it is also suggested to provide a table of input values (e.g. the training or testing sample) to run the test (otherwise the test will use random values as input). See `convertLGBMToRooTree.py -h`.

### XGBoost 
Save your optimization (the booster) to a model file, with

    booster.save_model('my_model.model')

Then call the script:

    convertXGBoostToRootTree.py my_model.model my_output.root --objective binary:logistic(or reg:linear)

it is also suggested to provide a numpy table of input values (e.g. the training or testing sample) to run the test. See `convertXGBoostToRootTree.py -h`.

## Details about the implementation
This tool was originally written by Jovan Mitrevski, supporting only TMVA BDT, and then updated by Ruggero Turra so that it is easy to support different kind of BDT, presently lgbm, XGBoost.

The tool uses a ROOT TTree to store the weights. Each entry of the TTree is a tree of the forest. Usually the branches are vectors with the variable used to cut (-1 for leafs), for each node, and the value to be used to cut (or the output value for leafs). The trees are serialized in vectors using preorder traversal. See previous section to see how to create these TTree.

The BDT class is now splitted in three layers:

 * BDT: this is the only class the user needs and its interface. It is reading the input weights and deciding which implementation of the forest to instantiate. BDT class holds a pointer to IForest which point to a concret Forest class (ForestLGBM / ForestLGBMSimple / ForestTMVA / ForestXGBoost).
 * Forest: this implements the forest of decision trees, but the implementation of the node of the BDT is not here. Different Forest classes are provided, since for example the way the trees are summed is different between TMVA/lgbm/XGBoost (for example for TMVA classification output is a normalized weighted sum of all the response of all the trees, while lgbm is a simple sum followed by a sigmoid activation). The Forest class holds a vector of nodes. All the nodes of all the trees are in the same vector. The Forest knows the index of the top node for each tree.
 * Node: this implements the logic at each node of the tree which is different for TMVA/lgbm/XGBoost (e.g. TMVA is using >= as decision, lgbm is using <=, lgbm support nan as input, ...)

Forest is implemented with polymorphism with virtual classes but the Node class is not (static polymorphism) to avoid to query a virtual table for each node evaluation.

Everything else is unchanged: the weights are from TTree and old TTree works in the new implementation. The memory usage and cpu speed is identical.

lgbm/XGBoost has many options (much more than tmva). Only the most common or default options are supported. The new lgbm implementation support only regression/multiclassification/binary classification. It supports continuous inputs, including nan as missing values (but not other kinds of missing values as in lgbm). It does not support categorical input variables. It supports only default activation functions (e.g. standard sigmoid for binary classification).

Only regression/binary classification are currently supported for XGBoost.

A python utility to convert the weights saved by lgbm/XGBoost training to our TTree format is provided. It needs lgbm/XGBoost installed (writing a parser is too complicated, error prone, do not guaranteed compatibility with future lgbm formats, ...).

### Performances

Take care about the size of the node classes, in particular using the best possible alignment. There is an utility (check_timing_mvautils) which can be used.

### Tests

It is import to be 100% sure that the output of MVAUtils is identical to the one of the original tool used for training. There are several pitfalls: logic of the node (<= vs <, ...), how trees are summed (weighted, non-weighed, with biases, ...) and how the response of the forest is normalized (sigmoid, ...)

In the utility to convert lgbm weights file to our TTree a check is done, depending on the task (binary classification / multiclassification / regression). It tests if the output of lgbm (computed using lgbm library) is the same as the one from MVAUtils using random inputs (it is possible, and suggested, to use inputs provided by the user, an option allows to do that).

Many automatic checks are in Reconstruction/MVAUtils/test/ut_test_MVAUtils.py. All the output functions are checked. Two kind of checks are done. The first just create a very simple forest (with hard-coded weights) and check if the output is the desidered one using hard-coded inputs. The second load lgbm library (if possible), train some BDTs (from iris dataset) and then performs similar checks. If it is not possible to load lgbm library it tries to install it. In case of failure it skips the test without failures. By default only the simple part of the test is run automatically, to run the whole test run:

    test/ut_test_MVAUtils.py 
