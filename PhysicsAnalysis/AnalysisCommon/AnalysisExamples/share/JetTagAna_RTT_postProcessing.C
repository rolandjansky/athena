
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"

// Always leave N_HISTO_TESTS as last entry
enum histo_tests {HAS_ENTRIES, IS_EMPTY, ARE_NOT_IDENTICAL, ARE_IDENTICAL, MEAN_NEAR_ZERO, KS_TEST, ARE_ALMOST_IDENTICAL, N_HISTO_TESTS}; 

int DrawHistograms(TCanvas *, TH1F* ref, TH1F* cur, char * refstr, char *curstr);
bool CheckHistogram(TH1F *hist, TH1F* hist2, char * tests, char *hname, char *extension, char *dir);
bool PerformHistogramTest(TH1F* h1, TH1F *h2, char *test, char *extension, char *test_description, char *result_str);
int SaveHistograms(TCanvas * cx, char *filename, char *extension, char * dir);
int PrintHtmlTableEntry(char *html, char *hist_name, bool, bool ,bool );
int PrintHtmlTableEntry2(char *html, char *test, char *result, bool pass);
int PrintHtmlHead(char *, char *, char *, char *);
int PrintHtmlHead2(char *, char *);
int PrintHtmlFoot(char *);
int PrintHtmlFoot2(char *html, char *image_file);
void FormatFileName(char * h, char * h2);


// file:     root_to_html.cc
// author:   Sven Vahsen, sevahsen@lbl.gov
// version:  09/02/05
// synposis: builds web pages (.html and .gif files) for Inner Detector Tracking
// inputs:   two .root files with histograms, a testlist, and some strings
// outputs:  main web page, sub-pages for each histogram, images (.GIFs) linked from these pages
//
// revisions:
//
// 09/02/05: o error message on web page when input .root files not find
//           o new function argument for name of main html file
//             (previously hardcoded as 'test.html')
//
// to-do list:
//
//           o error message on web page in case of wrong format for testlist file --> also list testdate
//           o implement more histograms comparisons --> KS test. What else?
//           o allow appending string to all filenames ?

#include <iostream>
#include <time.h>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPaveStats.h" 

char *test_keyword[N_HISTO_TESTS]={"has_entries","is_empty","are_not_identical","are_identical", "mean_near_zero","ks_test","are_almost_identical"};
char *test_description[N_HISTO_TESTS]={"Does the histogram have entries?","Is the histogram empty?","Do the histograms differ?","Are the histograms identical?", "Is the histogram mean consistent with zero?","KS Test","Are the histograms almost identical?"};

int root_to_html(char *ref_file, char *cur_file, char *ref_version, char *cur_version, char *particle_type, char *testlist, char *tmpdir, char *pagetmp)
{
  char aline[512], hname[512], tests[512]; 
  char refstr[512], curstr [512], hname2[512], page[512], dir[512];

  sprintf(dir,"%s%s",tmpdir, (strlen(tmpdir)>0) ? "/" : "");  
  sprintf(page,"%s%s",dir,pagetmp);
  sprintf(refstr,"%s (reference)",ref_version);
  sprintf(curstr,"%s (current)",cur_version);

  // Open Root Files, initialize canvas
  //gROOT->SetStyle("Plain");
  gStyle->SetOptStat(111111);
  TCanvas *c0 = (TCanvas *) gROOT->FindObject("c0"); 
  if (c0) c0->Delete();
  c0 = new TCanvas("c0");

  // add check later: if (!OpenRootFiles()) return -1;
  TFile reference(ref_file);
  TFile current(cur_file);

  if (! reference.IsOpen()) cerr << "ERROR: Unable to open file \"" << ref_file << "\"!\n";
  if (! current.IsOpen())   cerr << "ERROR: Unable to open file \"" << cur_file << "\"!\n";

  // Open file which lists the checks to be performed
  FILE * infile = fopen(testlist,"r");
  if(!infile)
  {
    cerr << "Unable to open input file " << testlist << "\n";
    return -1;
  }

  PrintHtmlHead(page,ref_version,cur_version,particle_type, current, reference);

  while (fgets(aline,256,infile)) 
  {   
    int nvars = sscanf(aline,"Histogram %s Test %[^\n]\n", hname, tests); 

    if (nvars < 2)
    { 
      cerr << "Formatting error in txt file: Line \"" << aline << "\" will be ignored.\n";
      continue;
    }

    TH1F *ref_hist = 0;
    TH1F *cur_hist = 0;

    if (reference.IsOpen()) ref_hist = (TH1F*)reference.Get(hname);
    if (current.IsOpen())   cur_hist = (TH1F*)current.Get(hname);

    char* result = strstr(cur_hist->GetName(),"rej");
    if(result == NULL) {

      ref_hist->Scale(1./ref_hist->GetEntries());
      cur_hist->Scale(1./cur_hist->GetEntries());

    }

    FormatFileName(hname,hname2);

    DrawHistograms(c0, ref_hist, NULL, refstr, curstr );
    SaveHistograms(c0, hname2, "ref", dir);
    DrawHistograms(c0, NULL, cur_hist, refstr, curstr );
    SaveHistograms(c0, hname2, "cur", dir);
    DrawHistograms(c0, ref_hist, cur_hist, refstr, curstr );
    SaveHistograms(c0, hname2, "cmp", dir);
    
    bool pass_ref = CheckHistogram(ref_hist, NULL, tests, hname2, "ref", dir);
    bool pass_cur = CheckHistogram(cur_hist, NULL, tests, hname2, "cur", dir);
    bool pass_cmp = CheckHistogram(ref_hist, cur_hist, tests, hname2, "cmp", dir);

    PrintHtmlTableEntry(page, hname2, pass_ref, pass_cur, pass_cmp);
  }
  
  PrintHtmlFoot(page);
  return 1;
}


// substitute '/' or '\' characters in filename with '_'
void FormatFileName(char * h, char * h2)
{
  int len = strlen(h)+1;

  for (int i=0; i<len && i<511; i++)
  {
    *h2=*h;
    if (*h2=='/' || *h2=='\\') *h2='_';
    h++; h2++;
  }
}

// Check whether histogram exists
// Perform specified tests on histogram
// create html output file with gif of 
// histogram and table of tests results
bool CheckHistogram(TH1F *hist, TH1F *hist2, char * tests, char *hname, char *extension, char *dir)
{
  char image_file[512], html_file[512], one_test[512], result_str[512], tmp[512], test_descr[512]; 
  sprintf(image_file,"%s_%s.gif", hname,extension);
  sprintf(html_file, "%s%s_%s.html",dir, hname,extension);
  
  PrintHtmlHead2(html_file, hname);
  
  bool pass = true;
  if (!strcmp(extension,"cmp") && (!hist || !hist2))
  { 
    // fail if comparison test, and one of the histogram doesn't exist
    pass = false;
    if (!hist)  PrintHtmlTableEntry2 (html_file,"Reference histogram not found in root file!","",false);
    if (!hist2) PrintHtmlTableEntry2 (html_file,"Current histogram not found in root file!","",false);
  }  
  else if (!hist)
  {
    // always fail if histogram 1 doesn't exist
    pass = false;
    PrintHtmlTableEntry2(html_file,"Histogram not found in root file!","",false);
  }
  else
  {
    int nvars = 1;
    char *ptr = tests;
    int ntests=0;

    // get single words (test instructions) from string
    while (nvars > 0 && nvars != EOF && (ptr < (tests+strlen(tests))))
    {
      nvars = sscanf(ptr,"%s", one_test);
      ptr += strlen(one_test) + 1;
      if (nvars>0 && nvars !=EOF) 
      {
        strcpy(tmp,one_test); tmp[3]=0;
        //cout << "position=" << ptr-tests << ", tmp = \"" << tmp << "\" ,extension=\"" << extension << "\"\n";

        if (!strcmp(extension,tmp))
        {
          bool tmp_pass = PerformHistogramTest(hist, hist2, one_test, extension, test_descr, result_str);
          PrintHtmlTableEntry2(html_file, test_descr, result_str ,tmp_pass);
          if (!tmp_pass) pass = false;
          ntests++;
        }
      }
    }
    if (!ntests) PrintHtmlTableEntry2(html_file, "No tests defined for this histogram", "-" , 1); 
  }
  PrintHtmlFoot2(html_file, image_file);  
  return pass;
}


bool PerformHistogramTest(TH1F* h1, TH1F* h2, char *test, char *extension, char *test_descr, char *result)
{

  bool match=false;
  bool pass=false;
  int entries=0;
  double mean=0;
  double sigma=0;
  double signific=0;
  int entries_2=0;
  double mean_2=0;
  double sigma_2=0;
  char current[512];
  float cut = 0;

  // which histogram are we testing?

  sprintf(test_descr,test);

  cout << "PerformHistogramTest: tests=" << test << ", extension=" << extension << "\n";
  if (strlen(test)> 4)
  {
    cout << "Comparing " << test << endl;

    for (int i=0; i<N_HISTO_TESTS; i++)
    {

      // Truncate length of test, name it current
      int j;
      for (j=0;(j<strlen(test))&&(j<strlen(test_keyword[i])+4); j++) {
        current[j] = test[j];
      }
      current[j] = '\0';

      cout << "  To " << test_keyword[i] << ": " << current << endl;

      // check if test name matches any of the keywords
      if (!strcmp(current+4,test_keyword[i]))
      {
        match = true;
        strcpy(test_descr,test_description[i]);
        switch(i)
        {
          case HAS_ENTRIES:
            entries = (int) h1->GetEntries();
            sprintf(result,"Entries=%d",entries);
            if (entries > 0) pass=true;    
            break;
          case IS_EMPTY:
            entries = (int) h1->GetEntries();
            sprintf(result,"Entries=%d",entries);
            if (entries != 0) pass=false;          
            break;
          case MEAN_NEAR_ZERO:
            sscanf(test+4,"mean_near_zero(%f)",&cut);

            if (cut == 0) cut = 4;

            entries  = (int) h1->GetEntries();
            mean     = h1->GetMean(1);
            sigma    = h1->GetMeanError(1);
            if (sigma !=0)
            { 
              signific = fabs(mean/sigma);
              sprintf(result,"fabs(mean/sigma_of_mean)=%f (cut was %f)",signific,cut);
              if (signific < cut ) pass =true;
              else pass = false;
            }
            else
            {
               signific = 999999.0;
               sprintf(result,"sigma_mean=0,can't calculate significance!");
               pass = false;
            }
            break;
        case ARE_IDENTICAL:
        case ARE_NOT_IDENTICAL:
            entries    = (int) h1->GetEntries();
            mean       = h1->GetMean(1);
            sigma      = h1->GetMeanError(1);
            entries_2  = (int) h2->GetEntries();
            mean_2     = h2->GetMean(1);
            sigma_2    = h2->GetMeanError(1);

            if (entries != entries_2 || mean!=mean_2 || sigma !=sigma_2)
            { 
              sprintf(result,"Histogram number of entries, mean, or sigma differ");
              pass = false;
            }
            else
            {
              sprintf(result,"Histogram number of entries, mean, and sigma are identical");
              pass = true;
            }
            if (i==ARE_NOT_IDENTICAL) pass = !pass;
          break;
        case KS_TEST:
          sscanf(test+4,"ks_test(%f)",&cut);

          double ks =  h1->KolmogorovTest(h2);
          sprintf(result,"Output from KS test is %f (cut is %f)",ks,cut);
          pass = (ks > cut);
          break;

        case ARE_ALMOST_IDENTICAL:
          sscanf(test+4,"are_almost_identical(%f)",&cut);



          double dA = 0;
          double A = 0;

          for (int k=0; k <= h1->GetNbinsX()+1; k++) {
            dA += fabs(h1->GetBinContent(k) - h2->GetBinContent(k));
            A += h1->GetBinContent(k);
          }

          if (A > 0) {
            dA = dA / A;
          }

          sprintf(result,"&Sigma;(|&Delta;A|)/&Sigma;A is %f (cut is %f)",dA,cut);

          pass = (dA < cut);
          break;

          default:
            sprintf(test_descr,test+4);
            sprintf(result,"Test name recognized, but no action defined in root_to_html.cc!");
            pass=false;
        }
        break;
      }
    }
  }   
  if (!match)
  {
    sprintf(result,"Unknown test!");
    pass=false;
  }
  return pass;
}


int DrawHistograms(TCanvas *c0, TH1F* ref_hist, TH1F* cur_hist, char *refstr, char *curstr)
{
  if (!ref_hist && !cur_hist)
  {
    c0->Clear();
    return 0;
  }

  double posStatX = 0.89;
  double posLeg1X = 0.50;
  double posLeg2X = 0.89;
  gStyle->SetStatY(0.67);
  gStyle->SetOptStat(111111);
  // Modif LV: decide to put stat box on left or right side:
  TH1F* h = 0;
  if(cur_hist) h = cur_hist;
  if(ref_hist) h = ref_hist;
  if(h) {
    double activL = h->Integral(0,h->GetNbinsX()/3);
    double activR = h->Integral(h->GetNbinsX()*2/3,h->GetNbinsX());
    if(activL*1.2<activR) {
      posStatX = 0.33;
    } else {
      posStatX = 0.89;
    }
  }
  gStyle->SetStatX(posStatX);
 
  if (ref_hist)
  {
    ref_hist->Draw();
  }
  if (cur_hist)
  {
    cur_hist->SetMarkerStyle(20);
    if (ref_hist) // make sure stat boxes don't overlap
    {
      TPaveStats *st = (TPaveStats*)cur_hist->GetListOfFunctions()->FindObject("stats"); 
      //st->SetLabel(curstr); 
      st->SetY2NDC(0.40);     
      st->SetY1NDC(0.16);     
    }
    cur_hist->Draw((ref_hist) ? "samesp" : "p");
  }
  TLegend *legend=new TLegend(posLeg1X,0.75,posLeg2X,0.88);
  legend->SetTextFont(72);
  legend->SetTextSize(0.03);
  if (ref_hist) legend->AddEntry(ref_hist,refstr,"l");
  if (cur_hist) legend->AddEntry(cur_hist,curstr,"p");
  legend->Draw();

  c0->Modified();
  c0->Update(); 

  return 1;
}


// print out one line of RTT results table for histogram details page, in html format
int PrintHtmlTableEntry2(char *html, char *test, char*result, bool pass)
{
  FILE * f = fopen(html,"a");
  if (!f) return 0;

  // cell 1
  fputs("<tr>\n",f);
  fprintf(f,"<td align=\"center\">%s</td>\n",test);
  fprintf(f,"<td align=\"center\">%s</td>\n",result);
  fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s</span></td>\n",
          (pass ? "51, 204, 0":"255, 0, 0"),  (pass ? "PASS" : "FAIL"));
  fputs("</tr>\n",f);
  fclose(f);

  return 1;
}

// print out one line of RTT results table in html format
int PrintHtmlTableEntry(char *html, char *hist_name, bool pass_ref, bool pass_cur, bool pass_cmp)
{
  FILE * f = fopen(html,"a");
  if (!f) return 0;

  fputs("<tr>\n",f);

  // cell 1
  fprintf(f,"<td align=\"center\"><a href=\"%s_expl.txt\">%s</a></td>\n",hist_name,hist_name);

  // cell 2
  fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s <a target=\"_blank\" href=\"%s_ref.html\">(view)</a></span></td>\n",
          (pass_ref ? "51, 204, 0":"255, 0, 0"),  (pass_ref ? "PASS" : "FAIL"), hist_name);

  // cell 3
  fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s <a target=\"_blank\" href=\"%s_cur.html\">(view)</a></span></td>\n",
          (pass_cur ? "51, 204, 0":"255, 0, 0"),  (pass_cur ? "PASS" : "FAIL"), hist_name);

  // cell 4
  fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s <a target=\"_blank\" href=\"%s_cmp.html\">(view)</a></span></td>\n",
          (pass_cmp ? "51, 204, 0":"255, 0, 0"),  (pass_cmp ? "PASS" : "FAIL"), hist_name);

  fputs("</tr>\n",f);

  if (!f) return 0;
  fclose(f);
  return 1;
}


// used to build html pages that show histograms
int PrintHtmlHead2(char *html, char *hname)
{
  FILE * f = fopen(html,"w");
  if (!f) return 0;

  fputs("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n",f);
  fputs("<html>\n",f);
  fputs("<head>\n",f);
  fputs("<meta content=\"text/html; charset=ISO-8859-1\"\n",f);
  fputs("http-equiv=\"content-type\">\n",f);
  fputs("<title>ATLAS B-tagging RTT</title>\n",f);
  fputs("</head>\n",f);
  fputs("<body>\n",f);
  fputs("<span style=\"font-weight: bold;\">B-tagging Histogram\n",f);
  fprintf(f,"\"%s\"<br>\n",hname);
  fputs("<br>\n",f);
  fputs("</span>\n",f);
  fputs("<table style=\"text-align: left; width: 70%;\" border=\"1\" cellpadding=\"2\"\n",f);
  fputs("cellspacing=\"2\">\n",f);
  fputs("<tbody>\n",f);
  fputs("<tr>\n",f);
  fputs("<tr>\n",f);
  fputs("<td style=\"font-weight: bold;\" align=\"center\">Test performed</td>\n",f);
  fputs("<td style=\"font-weight: bold;\" align=\"center\">Result</td>\n",f);
  fputs("<td style=\"font-weight: bold;\" align=\"center\">Pass/Fail</td>\n",f);
  fputs("</tr>\n",f);
  fclose(f);
  return 1;
}

// used to build html pages that show histograms
int PrintHtmlFoot2(char *html, char *image_file)
{
  FILE * f = fopen(html,"a");
  if (!f) return 0;

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  fputs("</tr>\n",f);
  fputs("</tbody>\n",f);
  fputs("</table>\n",f);
  fputs("<br>\n",f);

  fputs("<img style=\"border: 1px solid;\" alt=\"IMAGE\"\n",f);
  fprintf(f,"src=\"%s\"><br>\n",image_file);
  fputs("<br>\n",f);
  fputs("<br>\n",f);
  fputs("<address>This page was automatically generated using a modified version of root_to_html (S.Vahsen/LBNL) on ",f);
  fputs(asctime(timeinfo),f);
  fputs(".</address>\n",f);
  fputs("</body>\n",f);
  fputs("</html>\n",f);
  fclose(f);
  return 1;
}


int PrintHtmlHead(char *html, char *ref_version, char *cur_version, char *particle_type, TFile &cfile, TFile &rfile)
{
  FILE * f = fopen(html,"w");
  if (!f) return 0;

  fputs("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n",f);
  fputs("<html>\n",f);
  fputs("<head>\n",f);
  fputs("<meta content=\"text/html; charset=ISO-8859-1\"\n",f);
  fputs("http-equiv=\"content-type\">\n",f);
  fputs("<title>ATLAS B-tagging RTT</title>\n",f);
  fputs("</head>\n",f);
  fputs("<body>\n",f);
  fputs("<h1>ATLAS B-tagging RTT</h1>\n",f);

  fputs("<table style=\"text-align: left;\" border=\"1\"\n",f);
  fputs("cellpadding=\"2\" cellspacing=\"2\">\n",f);
  fputs("<tbody>\n",f);
  fputs("<tr>\n",f);

  fputs("<td align=\"center\">Current Athena release:</td>\n",f);
  fprintf(f,"<td align=\"center\">%s</td>\n",cur_version);
  fputs("<td align=\"center\">Histogram file:</td>\n",f);  
          fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s%s</span></td>\n",
          (cfile.IsOpen() ? "51, 204, 0":"255, 0, 0"),  cfile.GetName(), (cfile.IsOpen() ? "" : ": UNABLE TO OPEN FILE!"));
  fputs("</tr>\n",f);

  fputs("<tr>\n",f);
  fputs("<td align=\"center\">Reference Athena release:</td>\n",f);
  fprintf(f,"<td align=\"center\">%s</td>\n",ref_version);
  fputs("<td align=\"center\">Histogram file:</td>\n",f);  
          fprintf(f,"<td align=\"center\"><span style=\"color: rgb(%s);\">%s%s</span></td>\n",
          (rfile.IsOpen() ? "51, 204, 0":"255, 0, 0"),  rfile.GetName(), (rfile.IsOpen() ? "" : ": UNABLE TO OPEN FILE!"));
  fputs("</tr>\n",f);

  //fputs("<tr>\n",f);
  //fputs("<td align=\"center\">Particle Species:</td>\n",f);
  //fprintf(f,"<td align=\"center\">%s</td>\n",particle_type);
  //fprintf(f,"<td align=\"center\" colspan=2>All tracks from iPatRec<br>via CBNT::TrackParticle (for now)</td>\n"); 
  //fputs("</tr>\n",f);

  fputs("</tbody>\n",f);
  fputs("</table>\n",f);
  fputs("<p>Click on the link in the histogram column to see the a brief explanation of the histogram.  <br>Click on \"view\" for each column to see what tests were run and a plot of the quantity.\n",f);
  fputs("&nbsp;<br>\n",f);
  fputs("<h3>Test Results: <br>\n",f);
  fputs("</h3>\n",f);
  //fputs("<h3>Click on histogram names for brief explanations. <br>\n",f);
  //fputs("</h3>\n",f);
  //fputs("<h3>Click on Pass/Fail to view actual histogram and tests performed.</h3>\n",f);
  fputs("<table style=\"text-align: left; width: 70%;\" border=\"1\" cellpadding=\"2\"\n",f);
  fputs("cellspacing=\"2\">\n",f);
  fputs("<tbody>\n",f);
  fputs("<tr>\n",f);
  fputs("<td align=\"center\"><span style=\"font-weight: bold;\">Histogram</span></td>\n",f);
  fputs("<td align=\"center\"><span style=\"font-weight: bold;\">Reference\n",f);
  fputs("Release<br>\n",f);
  fputs("</span></td>\n",f);
  fputs("<td align=\"center\"><span style=\"font-weight: bold;\">Current\n",f);
  fputs("Release</span></td>\n",f);
  fputs("<td align=\"center\"><span style=\"font-weight: bold;\">Comparison</span></td>\n",f);
  fputs("</tr>\n",f);
  fclose(f);
  return 1;
}

int PrintHtmlFoot(char *html)
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  FILE * f = fopen(html,"a");
  if (!f) return 0;
  fputs("</tr>\n",f);
  fputs("</tbody>\n",f);
  fputs("</table>\n",f);
  fputs("<br>\n",f);
  fputs("<address>This page was automatically generated using a modified version of root_to_html (S.Vahsen/LBNL) on ",f);
  fputs(asctime(timeinfo),f);
  fputs(".</address>\n",f);
  fputs("</body>\n",f);
  fputs("</html>\n",f);
  fclose(f);
  return 1;
}

int SaveHistograms(TCanvas * cx, char *hname, char *ext, char *dir)
{
  char epsname[512];
  char gifname[512];
  char comm1[512];
  char comm2[512];
  char comm3[512];

  sprintf(epsname,"%s%s_%s.eps",dir, hname,ext);
  sprintf(gifname,"%s%s_%s.gif",dir, hname,ext);
  sprintf(comm1,"%s %s","pstopnm -ppm -xborder 0 -yborder 0 -portrait",epsname);
  sprintf(comm2,"%s %s%s %s","ppmtogif",epsname,"001.ppm >",gifname);
  sprintf(comm3,"%s%s%s","rm -f ",epsname,"001.ppm");

  //  cx->SaveAs(gifname);

  cx->SaveAs(epsname);
  gSystem->Exec(comm1);
  gSystem->Exec(comm2);
  gSystem->Exec(comm3);

  return 1;
}

void JetTagAna_RTT_postProcessing() {
  cout << "Starting JetTagAna_RTT_postProcessing..." << endl;
  root_to_html("JetTagAna_RTT_refsHist.root","tagana.root","12.0.6.5","current","","JetTagAna_RTT_testList.txt","","all_tests.html");
}
