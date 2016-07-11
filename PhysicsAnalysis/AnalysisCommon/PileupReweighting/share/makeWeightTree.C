
///***************************************************************************************
///PRW Weight Tree Making and Usage:
///---------------------------------------------------------------------------------------
///This is a small script you can run to generate a 'prw weight tree'
///Modify the properties below to set up an instance of a prw tool as you would if you
///processing all your MC in a usual event loop.
///Then in the 'MakeWeightTree' line, specify all the channel numbers you want to generate
///pileup weights for. Specify them as a comma delimited list in a string.
///
///Once you run this script (root makeWeightTree.C) you will get a prwTree.root file
///To use it, you need to have made a branch in your main ntuple called 'PRWHash'
///which contains the result of the GetPRWHash() method of the tool.
///Then you just friend this tree to your main tree/chain, like this:
///TChain c("MainTreeChain"); //add files to this
///TChain* prwChain = new TChain("prwTree"); prwChain->Add("prwTree.root"); prwChain->BuildIndex("PRWHash");
///c.AddFriend(prwChain);
///Your main chain will then appear to have a 'PileupWeight' branch in it!!



{
   CP::PileupReweightingTool tool("tool");

   std::vector<std::string> configFiles = {"/usera/will/testareas/ZdZd13TeV/ZdZdAnalysis/share/302073.prw.root","/usera/will/testareas/ZdZd13TeV/ZdZdAnalysis/share/361106_361108.r6630.prw.root"};
   std::vector<std::string> lumiFiles = {"/usera/will/testareas/ZdZd13TeV/ZdZdAnalysis/share/data15_13TeV.periodA.None.lumicalc.gerl.root"};

   tool.setProperty("ConfigFiles",configFiles);
   tool.setProperty("LumiCalcFiles",lumiFiles);
   tool.setProperty("DefaultChannel",361106);
   tool.setProperty("OutputLevel",2);//DEBUG

   ///this part is optional ... if we wanted to filter our lumicalc file based on a GRL
   //what is happening here is we grab the JobOptionsSvc, and put a property in the catalogue
   //then we tell our prw tool that we will want a GRL tool: when it gets initialized, it will
   //use the property in the joboptions catalogue to configure itself
   //This is how Gaudi works!
   ServiceHandle<IJobOptionsSvc> josvc("JobOptionsSvc","");
   std::vector<std::string> testXML = {"test.xml"};
   josvc->addPropertyToCatalogue("ToolSvc.grlTool",StringArrayProperty("GoodRunsListVec",testXML));
   tool.setProperty("GRLTool","GoodRunsListSelectionTool/grlTool");
   ///end of optional part!

   tool.initialize();
   //tool.expert()->MakeWeightTree("361106,361107,361108,123456","prwTree.root","PRWHash" /*change to name of branch in main tree*/ ,"PileupWeight" /*optional change to name of branch you want to be the weight*/);
tool.expert()->MakeWeightTree("361106","prwTree.root","PRWHash" /*change to name of branch in main tree*/ ,"PileupWeight" /*optional change to name of branch you want to be the weight*/);


}
