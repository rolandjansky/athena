/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  TFile f("bs-streamerinfos.root");


  TList *a = f.GetStreamerInfoList();
  TIter nextinfo(a);
  TStreamerInfo *inf;
  while ((inf = (TStreamerInfo *)nextinfo()) != 0){
    inf->BuildCheck();
    TClass *cl = inf->GetClass();
    if (cl)
      std::cout << "external TStreamerInfo for " << cl->GetName()
		<< " checksum: " << inf->GetCheckSum() << std::endl;
  }

}
