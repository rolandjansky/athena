void makeclass_ClCaloHits ( )
{
   TFile *f;
   TTree *t;
//   
   Char_t const *TreeName  = "calohitD3PD"; //  name of the tree
   Char_t const *ClassName = "ClCaloHits";  //  class name
//
//
   Char_t const *Dir = "/eos/user/k/kiryunin/HEC_SF";
   Char_t rootfile[222];
   sprintf (rootfile, 
            "%s/10.1-FTFP_BERT_ATL/DumpHits/ele_e0100_sc1/01.root", Dir);
//
//   
   f = TFile::Open(rootfile);
   if ( !f->IsOpen() ) {
      printf (" *** File - %s - is not openned *** \n", rootfile);
      exit (-2);
   }
//
//
   t = (TTree *) f->Get(TreeName);
   if ( !t ) {
      printf (" *** Tree - %s - is not found *** \n", TreeName);
      exit (-2);
   }
//  
// 
   t->MakeClass(ClassName);
//
//   
}
