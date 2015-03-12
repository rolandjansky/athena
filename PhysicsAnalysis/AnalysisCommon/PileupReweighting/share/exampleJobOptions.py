
#by default (i.e. with no config files) the tool will be created in config file making mode

ToolSvc += CfgMgr.CP__PileupReweightingTool("myTool", 
            ConfigFiles=["PileupReweighting/mc12ab_defaults.prw.root"],
            DataScaleFactor=1./1.09,
            UnrepresentedDataAction=2,
            LumiCalcFiles=["GRL_v61.None.lumicalc.root"])

#in your code, create a ToolHandle<IPileupReweightingTool>
#the ToolHandle constructor should be given "CP::PileupReweightingTool/myTool" as its string argument

