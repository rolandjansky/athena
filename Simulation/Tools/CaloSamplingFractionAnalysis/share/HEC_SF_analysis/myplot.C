/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
void myplot (Float_t x1=0, Float_t x2=1, Float_t y1=0, Float_t y2=1,
            Char_t const *titx="X axis", Char_t const *tity="Y axis", 
            Char_t const *titp=" ")
{
   TH1F *frame = gPad->DrawFrame(x1,y1,x2,y2,titp);
//           
   frame->GetXaxis()->SetTitleSize(0.05);
   frame->GetXaxis()->SetTitleOffset(1.2);
   frame->GetXaxis()->SetTitle(titx);
//
   frame->GetYaxis()->SetTitleSize(0.05);
   frame->GetYaxis()->SetTitleOffset(1.8);
   frame->GetYaxis()->SetTitle(tity);
//
//
//
   TPaveText *title = 0;
   TObject *obj;
   TIter next(gPad->GetListOfPrimitives());
   while ((obj = next())) {
      if (!obj->InheritsFrom(TPaveText::Class())) continue;
      title = (TPaveText*)obj;
      if (strcmp(title->GetName(),"title")) {title = 0; continue;}
      break;
   }
//
//
   if (title) {
      title->SetBorderSize(1);
      title->SetFillColor(10);
      title->SetTextAlign(22);
      title->SetTextFont(62);
//
      title->SetX1NDC(0.18);      
      title->SetX2NDC(0.98);      
//
      TText *t0 = (TText*)title->GetLine(0);
//
      t0->SetTextColor( 1 );
//      
      title->Draw();
   }
   else {
      cerr << " *** myplot:  title does not exist !!! *** " << endl;
      return;
   }
//
//
}
////////////////////////////////////////////////////////////////////////
