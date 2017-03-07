/***
 *
 *  ROOT6 macro to 1) analyze HNNLO input templates from NNLOPS
 *  and 2) correct for statistical fluctuations by using symmetry
 *  of Higgs rapidity spectrum in pp collisions (TODO?)
 *
 *  Dag Gillberg, March 4, 2017
 *
 */

typedef TString Str;
typedef std::vector<TString> StrV;
typedef std::vector<double> NumV;
template <typename T> void add(vector<T> &v, T a) { v.push_back(a); }

// splits string into string vector
StrV vectorize(Str str, Str sep=" ");
NumV vectorizeD(Str str, Str sep=" ");
StrV readFile(TString fileName);
void fatal(Str msg) {
  printf("FATAL:\n\n  %s\n\n",msg.Data()); abort();
}

void analyze(NumV bins, NumV y, NumV err, Str title);

void analyze_HNNLO_templates(Str fileName="H1250-CM13-NNPDF3-APX2-HH.top") {

  // Prepare a pdf output file
  Str pdf="plots_"+Str(fileName).ReplaceAll("top","pdf");
  TCanvas *can = new TCanvas();
  can->Print(pdf+"[");

  // Read in those lines
  StrV lines=readFile(fileName);
  
  NumV bins, y, err; Str title;
  for (Str line:lines) {
    if (line[0]=='#') {
      title=line; continue;
    }
    if (line.Sizeof()<=2) {
      if (bins.size()) {
	cout << "Done reading " << title << endl;
	analyze(bins,y,err,title);
	can->Print(pdf);
	//can->Print(pdf+"]"); fatal("stop"); // for debugging E->D
      }
      bins.clear(); y.clear(); err.clear();
      continue;
    }

    // GAH! stupid fortran code use D instead of E for exponent
    NumV nums=vectorizeD(line.ReplaceAll("D","E"));
    if (nums.size()!=4) fatal("Cannot intepret line \""+line+"\"");
    if (bins.size()==0) add(bins,nums[0]);
    add(bins,nums[1]); add(y,nums[2]); add(err,nums[3]);
  }
  can->Print(pdf+"]");
  printf("\nProduced %s\n\n",pdf.Data());
}

void analyze(NumV bins, NumV y, NumV err, Str title) {
  TH1D *h = new TH1D("",title+";"+title,-1+bins.size(),&bins[0]);
  //printf("%lu bins\n",bins.size());
  if (bins.size()!=y.size()+1||bins.size()!=err.size()+1) fatal("Bad input");
  for (size_t bin=1;bin<=y.size();++bin) {
    //printf("bin %3lu %.2f\n",bin,bins[bin-1]);
    h->SetBinContent(bin,y[bin-1]); h->SetBinError(bin,err[bin-1]);
  }
  h->Draw();
  if (title.Contains("yh index")) {
    TH1D *sym = (TH1D*)h->Clone();
    sym->SetLineColor(kRed);
    double chi2=0; int ndof=0;
    for (size_t i=0;i<50;++i) {
      double y1=y[i], y2=y[99-i], e1=err[i], e2=err[99-i];
      if (y1==0||y2==0) continue;
      double w1=1.0/e1/e1, w2=1.0/e2/e2, wsum=w1+w2, e=1/sqrt(wsum), c=(w1*y1+w2*y2)/wsum;
      printf("%6.2e +/-%.2e\n",y1,e1); printf("%6.2e +/-%.2e\n",y2,e2);
      printf("%6.2e +/-%.2e\n",c,e); printf("%2lu %6.2f\n",i,pow(y1-c,2)*w1+pow(y2-c,2)*w2);
      printf("\n");
      chi2+=pow(y1-c,2)*w1+pow(y2-c,2)*w2;
      ++ndof;
      sym->SetBinContent(i+1,c); sym->SetBinContent(100-i,c);
      sym->SetBinError(i+1,e); sym->SetBinError(100-i,e);
    }
    printf("chi2/ndof = %.2f / %i\n",chi2,ndof);
    sym->Draw("same");
    TLatex *tex = new TLatex();
    tex->SetNDC(); tex->SetTextFont(42); tex->DrawLatex(0.15,0.85,Form("#chi^{2} / #it{n}_{dof} = %.1f / %i",chi2,ndof));
  }
}


StrV vectorize(Str str, Str sep) {
  StrV result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries()==0) return result;
  TIter istr(strings);
  while (TObjString* os=(TObjString*)istr())
    if (os->GetString()[0]!='#') result.push_back(os->GetString());
    else break;
  return result;
}

NumV vectorizeD(Str str, Str sep) {
  NumV result; StrV vecS = vectorize(str,sep);
  for (auto s:vecS) {
    add(result,stod(s.Data()));
  }
  return result;
}

StrV readFile(TString fileName) {
  StrV lines;
  ifstream file(fileName.Data());
  string line;
  while (getline(file,line)) {
    lines.push_back(line);
  }
  printf("Read %lu lines from file %s\n",lines.size(),fileName.Data());
  return lines;
}

