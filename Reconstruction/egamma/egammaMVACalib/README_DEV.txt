This document describes how the tool has been implemented. This is quite intricated, since we want to be able to
   * apply different MVA tuning with one tool, just changing the initialization properties of the tool. Different MVAs comes from different trainings, the weight can be different and also the input variables can be different.
   * the tool must take as input xAOD objects (actually it takes the xAOD::Egamma object and the cluster separately since they are available in this way at reconstruction level)
   * the tool must know the definition of the input quantities. Input quantities can be complicated expressions defined from xAOD objects (as the ratio between layer1 and layer2 energy), presently only from xAOD::Egamma object and relative links, but in principle we could also add information non related to the particles, i.e. pileup. The same definitions of the variable (same code) must be used during the training.

At the end of the training process you have one trained algorithm for every bin (bins are defined by raw-pT and eta). The tuning of one algorithm is stored in an xml file, which is the output of TMVA, which is compressed in a ROOT TTree containing all the weights. In addition small shifts are computed to shift the peak of the distribution E/Etrue to 1 (which is not the case after the TMVA training since the loss function is the average squared residual). In every xml file you have:

   * definition of input variables as "E12 := el_rawcl_Es1 / el_rawcl_Es2" or simply "el_cl_eta"
   * definition of the algorithm tune, as the splitting for every BDT node for every tree in the forest
   * definition of the shifts (this is added by our code during the training to the end of the xml)

The ROOT file is derived from the xml and contains the TTree with all the weights and all the needed information and it is equivalent to the xml file.
The tool was designed in 2013, when everything was based on ntuples like D3PD. Moving to xAOD we just wrote a wrapper:

   * egammaMVACalib: apply the MVA calibration (old tool)
   * egammaMVATool: understand xAOD
   * egammaMVATree: link xAOD to TTree representation of the inputs

The main implementation constrain is from the TMVA design. Here just a summary (copy and paste from the documentation [1]): once you have trained your algorithm (so you have an xml file) it is applied using the TMVA::Reader:

    TMVA::Reader* reader = new TMVA::Reader( "<options>" );
    Int_t localDescreteVar;
    Float_t localFloatingVar, locaSum, localVar3;
    reader->AddVariable( "<YourDescreteVar>", &localDescreteVar );
    reader->AddVariable( "log(<YourFloatingVar>)", &localFloatingVar );
    reader->AddVariable( "SumLabel := <YourVar1>+<YourVar2>", &locaSum );
    reader->AddVariable( "<YourVar3>", &localVar3 );

    reader->BookMVA( "<YourMethodName>", "<path/JobName_MethodName.weights.xml>" );

    Double_t mvaValue = reader->EvaluateMVA( "<YourMethodName>" );

Note that: `EvaluateMVA` method takes no argument, except for the method name, which is constant in our case. The input values are not explicitely passed to the method, but readed trhohught the references intantiated before. This means that the user need to change the local variables event by event (particle by particle). With this design you can easily:

   * have an arbitrary number of variables
   * you avoid the copy of the inputs every time you call `EvaluateMVA`

 In particular:

   1. the local variables are primitive built-in c++ types
   2. the local variables can be stored outside the original data (xAOD::Egamma object), and they must be in case the input variable is defined as an expression
   3. input variables of the reader cannot be expressions: if they refer to an expression (as SumLabel := <YourVar1>+<YourVar2>) the input variable should be the value of the expression already evaluated. This is very different from the training step, where you can define a variable to be:

       factory->AddRegressionTree(regTree);
       factory->AddVariable("SumLabel := <YourVar1>+<YourVar2>", ’F’);

  (in that case YourVar1 and YourVar2 are branches of regTree).

For these reasons we decided to use as input of the internal tool a TTree which is set by the method `void egammaMVACalibegammaMVACalib::InitTree(TTree* tree, bool doNotify = true)` before using the tool. There are several advantages using such a structure:

    * easy to use if you already have a `TTree` as input (as in the D3PD era and when running the training where we use flat ntuples)
    * TTree can store heterogeneous data easily, the number of quantity is arbitrary
    * TTree can automatically compute expressions with `TTreeFormula` [2] from strings like `el_rawcl_E1 / el_rawclE2`. Remember that the definition of the input variables is stored as a string like that.

So what egammaMVACalib does is:

   1. read the xml or (default) the equivalent ROOT file
   2. create a TTreeFormula for every input variables (also for the ones that are just equal to a branch of the TTree, without a non-trivial expression), and for other variables that are always used (as the variables used to define the bins). Instantiate a float for every input of the MVA (both this float and the TTreeFormula are stored inside a simple `egammaMVACalib::VarFormula` struct)
   3. instantiate a reader for every bin. By default the reader is not TMVA::Reader, but the lightweight class BDT
   4. connect each reader to the input variables, using the method `AddVariable`
   5. For every particle: updates the value evaluating all the formulas (using TTreeFormula::EvalInstance), find the right bin and the relative BDT and call the `TMVA::Reader::EvaluateMVA` method (or the equivalent for the BDT class), multiply this number by the raw energy and by the shift

Since we have xAOD::Egamma particle we need something to translate xAOD::Egamma object to a TTree. This is done by the `egammaMVATree` class. This inherit from a TTree, but it guarantees to have only 1 entry. Calling the constructor a list of string are passed which define the variables as `el_rawcl_Es0`. For every variable a branch is created. The main method is `egammaMVATree::update(const xAOD::Egamma*)` which update the TTree to represent the xAOD::Egamma object. This class is used also during the training, when dumping the xAOD to a flat ntuple which is needed by TMVA during the training. In this way the same definition of variables are using during the training and the application of the MVA.

The main class the user should use is egammaMVATool. This tool has a property to identify the tuning of the calibration. This property is mapped to a calibration area where the tuning for electron, converted and unconverted photons are. Then the tool instantes the egammaMVATree and the egammaMVACalib tool. Every time an xAOD::Egamma object needs to be calibrate with the egammaMVATool::execute(const xAOD::CaloCluster* cluster, const xAOD::Electron* el) method first the tool calls egammaMVATree::update(const xAOD::CaloCluster* cluster, const xAOD::Electron* el) and then egammaMVACalib::getMVAEnergy().

Just to give an example, suppose that the input of the MVA are `el_cl_eta` and `el_rawcl_Es1 / el_rawcl_Es2` and that there are no bins (just one bin). During the initialization of egammaMVATool parse the two string and split it in three variables `el_cl_eta, el_rawcl_Es1, el_rawcl_Es2`; initialize the egammaMVATree with these three variables definition; initialize the egammaMVACalib passing as TTree the intance of the egammaMVATree which contains three branches. At this stage there are already some duplications, different memory location store the same quantity, for example: the TTree is based on the three floats that are defined inside TMVATree. We never call the TTree::Fill method. Inside the egammaMVACalib we have two floats, one for every input: `el_cl_eta` and `el_rawcl_Es1 / el_rawcl_Es2`. Since `el_cl_eta` is a trivial expression it is represented equally in egammaMVATree and in egammaMVACalib. Two TTreeFormulas are instantiated by egammaMVACalib. One BDT (since there is only one bin) is intantiated by egammaMVACalib. When calibrating, egammaMVATool calls the egammaMVATree::update function and then it calls egammaMVACalib::getMVAEnergy.

Why egammaMVATool::execute is non-const
=======================================

The user should expect the method egammaMVATool::execute to be const, since it should not change the status of the tool. In reality this is not true. The root of the problem is in the way TMVA::Reader is design. You are not passing something like:

reader->EvaluateMVA("method", my_input)

where my_input can be an std::ntuple or a templated-class (or a std::vector<float> if we decide to cast everything to float) which represent the input of a specific tuning. The TMVA::Reader force you to have a side effect in your class every time you call it:


    localFloatingVar = 10.; locaSum = 20; localVar3 = 30;
    reader->EvaluateMVA("method")

Of course we can add `mutable` to `egammaMVATree` but probably this is not what you want in a multi-thread environment, or we can simply put the call to `egammaMVATree::update` and to `egammaMVACalib::getMVAEnergy` in the same lock (even if I have no idea if we can have locks inside athena).

[1] http://tmva.sourceforge.net/docu/TMVAUsersGuide.pdf
[2] https://root.cern.ch/doc/master/classTTreeFormula.html
