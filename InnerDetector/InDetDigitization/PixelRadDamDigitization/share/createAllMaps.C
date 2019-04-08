#include "TH2F.h"
#include "TH3F.h"
#include "TRandom.h"
#include "TColor.h"
#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGLViewer.h"
#include "TLegend.h"

#include <string>
#include <sstream>

//-----------------------------------------------------------------------------------------------------------------------------
// Translate 3D-sensor TCAD data into root histograms for E-field, charge collection time, charge position and Ramo potential
//-----------------------------------------------------------------------------------------------------------------------------

void createAllMaps(TString EfieldPath = "EfieldMaps_Phi1.0e14_Temp283.root", TString RamoPath = "phi_1e14_20V.root", double fluence = 1e14, double vbias = 20, double temp = 283.0, bool visualizeMaps = true)    {

    std::cout << "Using efield from " << EfieldPath << std::endl; 
    std::cout << "Using Ramo map from " << RamoPath << std::endl;
    std::cout << "Using fluence = " << fluence << std::endl;
    std::cout << "Using	vbias	= " << vbias << std::endl;
    std::cout << "Using	temp	= " << temp << std::endl;

    //create efield map

    TH2F* efieldmapx = new TH2F("efieldx","Electric Field X",500,0,250,100,0,50);
    TH2F* efieldmapy = new TH2F("efieldy","Electric Field Y",500,0,250,100,0,50);
    TH2F* efieldmap = new TH2F("efield","Electric Field",500,0,250,100,0,50);

    TFile * mapsFile = new TFile(EfieldPath);

    TH3F* ex = (TH3F*)mapsFile->Get("ex");
    TH3F* ey = (TH3F*)mapsFile->Get("ey");
    TH3F* emod = (TH3F*)mapsFile->Get("emod");

    for (int i = 0; i < efieldmapx->GetNbinsX(); i++) {
        for (int j = 0; j < efieldmapx->GetNbinsY(); j++) {
            efieldmapx->SetBinContent(i, j, ex->GetBinContent(i, j, ex->GetZaxis()->FindBin(vbias)));
            efieldmapy->SetBinContent(i, j, ey->GetBinContent(i, j, ey->GetZaxis()->FindBin(vbias)));
            efieldmap->SetBinContent(i, j, emod->GetBinContent(i, j, emod->GetZaxis()->FindBin(vbias)));
      }
    }
    
    //create charge collection time map
    
    TH2F* timemap_e = new TH2F("etimes","Electron Collection Time",500,0,250,100,0,50);
    TH2F* timemap_h = new TH2F("htimes","Hole Collection Time",500,0,250,100,0,50);
          
    //electron data for mobility computations
    double vs_e = 1.53*pow(10,9)*pow(temp,-0.87);   //[cm/s]
    double Ec_e = 1.01*pow(temp,1.55);              //[V/cm]
    double beta_e = 2.57*pow(10,-2)*pow(temp,0.66);
     
    //hole data for mobility computations
    double vs_h = 1.62*pow(10,8)*pow(temp,-0.52);   //[cm/s]
    double Ec_h = 1.24*pow(temp,1.68);              //[V/cm]
    double beta_h = 0.46*pow(temp,0.17);
     
    double maxTimeLimit = 5; //in nanosec.
    double granularity = 100.; //precision, e.g. 100 -> 100th of a nanosecond timestep;
     
    //fill charge collection time map for electrons
    for (double y=0.25; y<=49.75; y += 0.5){
        for (double x=0.25; x<=249.75; x += 0.5){
     
            //define start position for hit
            double startx=x;
            double starty=y;
     
            for (int i=0; i<maxTimeLimit*granularity; i++){
     
                //get e-field
                double efieldx = efieldmapx->GetBinContent(efieldmapx->GetXaxis()->FindBin(startx),efieldmapx->GetYaxis()->FindBin(starty));
                double efieldy = efieldmapy->GetBinContent(efieldmapy->GetXaxis()->FindBin(startx),efieldmapy->GetYaxis()->FindBin(starty));

                //get mobility
                double mu_e = vs_e/(Ec_e*pow(1+pow((sqrt(efieldx*efieldx+efieldy*efieldy)/Ec_e),beta_e),(1/beta_e)));
     
                //drift velocity
                double vx = -mu_e*efieldx;   //[cm/s]
                double vy = -mu_e*efieldy;  //[cm/s]
         
                startx+=vx*pow(10.,-11)*pow(10,4); //nanosecond time step: 10^(-9)/100=10^(-11), and micron spacing: 10^4 (cm->um)
                starty+=vy*pow(10.,-11)*pow(10,4);
                
                double speed = sqrt(vx*vx+vy*vy)*pow(10,2); //efield in [V/cm] so convert to [V/m] to get speed in [m/s]
                if ( speed < 1e8 ){
                    timemap_e->SetBinContent(timemap_e->GetXaxis()->FindBin(x),timemap_e->GetYaxis()->FindBin(y),i/granularity);
                    break;
                }
     
            }
     
        }
    }
    
    //timemap_e->SetMaximum(0.8);
    
    //fill charge collection time map for holes
    for (double y=0.25; y<=49.75; y += 0.5){
        for (double x=0.25; x<=249.75; x += 0.5){
            
            //define start position for hit
            double startx=x;
            double starty=y;
            
            for (int i=0; i<maxTimeLimit*granularity; i++){
                
                //get e-field
                double efieldx = efieldmapx->GetBinContent(efieldmapx->GetXaxis()->FindBin(startx),efieldmapx->GetYaxis()->FindBin(starty));
                double efieldy = efieldmapy->GetBinContent(efieldmapy->GetXaxis()->FindBin(startx),efieldmapy->GetYaxis()->FindBin(starty));
                
                //get mobility
                double mu_h = vs_h/(Ec_h*pow(1+pow((sqrt(efieldx*efieldx+efieldy*efieldy)/Ec_h),beta_h),(1/beta_h)));
                
                //drift velocity
                double vx = mu_h*efieldx;   //[cm/s]
                double vy = mu_h*efieldy;  //[cm/s]
                
                startx+=vx*pow(10.,-9)/granularity*pow(10,4); //nanosecond time step: 10^(-9), and micron spacing: 10^4 (cm->um)
                starty+=vy*pow(10.,-9)/granularity*pow(10,4);
                
                double speed = sqrt(vx*vx+vy*vy)*pow(10,2); //efield in [V/cm] so convert to [V/m] to get speed in [m/s]
                
                if (speed < 4e7){
                    timemap_h->SetBinContent(timemap_h->GetXaxis()->FindBin(x),timemap_h->GetYaxis()->FindBin(y),i/granularity);
                    break;
                }
                
            }
            
        }
    }
    
    double maxTimeToElectrode_e = timemap_e->GetBinContent(timemap_e->GetMaximumBin()); //[ns]
    double maxTimeToElectrode_h = timemap_h->GetBinContent(timemap_h->GetMaximumBin()); //[ns]
    const int timesteps_e = granularity*maxTimeToElectrode_e; //number of bins
    const int timesteps_h = granularity*maxTimeToElectrode_h; //number of bins

    //create charge position map
    TH3F *voxelX_e = new TH3F("xPosition_e", "Electron X-Position Map", 500,0,250,100,0,50, timesteps_e, 0, maxTimeToElectrode_e);
    TH3F *voxelY_e = new TH3F("yPosition_e", "Electron Y-Position Map", 500,0,250,100,0,50, timesteps_e, 0, maxTimeToElectrode_e);
    TH3F *voxelX_h = new TH3F("xPosition_h", "Hole X-Position Map", 500,0,250,100,0,50, timesteps_h, 0, maxTimeToElectrode_h);
    TH3F *voxelY_h = new TH3F("yPosition_h", "Hole Y-Position Map", 500,0,250,100,0,50, timesteps_h, 0, maxTimeToElectrode_h);
    
    //fill distance maps for electrons and holes
    for (double x=0.25; x<=249.75; x += 0.5){
        for (double y=0.25; y<=49.75; y += 0.5){
            
            //define start position for hit
            double startx_e=x;
            double starty_e=y;
            double startx_h=x;
            double starty_h=y;
            
            //set initial conditions
            voxelX_e->SetBinContent(voxelX_e->GetXaxis()->FindBin(x),voxelX_e->GetYaxis()->FindBin(y),voxelX_e->GetZaxis()->FindBin(0.),startx_e);
            voxelY_e->SetBinContent(voxelY_e->GetXaxis()->FindBin(x),voxelY_e->GetYaxis()->FindBin(y),voxelY_e->GetZaxis()->FindBin(0.),starty_e);
            voxelX_h->SetBinContent(voxelX_h->GetXaxis()->FindBin(x),voxelX_h->GetYaxis()->FindBin(y),voxelX_h->GetZaxis()->FindBin(0.),startx_h);
            voxelY_h->SetBinContent(voxelY_h->GetXaxis()->FindBin(x),voxelY_h->GetYaxis()->FindBin(y),voxelY_h->GetZaxis()->FindBin(0.),starty_h);
            
            for (int i=1; i<timesteps_e; i++){
                
                //get e-field
                double efieldx_e = efieldmapx->GetBinContent(efieldmapx->GetXaxis()->FindBin(startx_e),efieldmapx->GetYaxis()->FindBin(starty_e));
                double efieldy_e = efieldmapy->GetBinContent(efieldmapy->GetXaxis()->FindBin(startx_e),efieldmapy->GetYaxis()->FindBin(starty_e));
                
                //get mobility
                double mu_e = vs_e/(Ec_e*pow(1+pow((sqrt(efieldx_e*efieldx_e+efieldy_e*efieldy_e)/Ec_e),beta_e),(1/beta_e)));
                
                //drift velocity
                double vx_e = -mu_e*efieldx_e;   //[cm/s]
                double vy_e = -mu_e*efieldy_e;  //[cm/s]
                
                startx_e+=vx_e*pow(10.,-9)/granularity*pow(10,4);
                starty_e+=vy_e*pow(10.,-9)/granularity*pow(10,4);
                
                voxelX_e->SetBinContent(voxelX_e->GetXaxis()->FindBin(x),voxelX_e->GetYaxis()->FindBin(y),voxelX_e->GetZaxis()->FindBin(i/granularity),startx_e);
                voxelY_e->SetBinContent(voxelY_e->GetXaxis()->FindBin(x),voxelY_e->GetYaxis()->FindBin(y),voxelY_e->GetZaxis()->FindBin(i/granularity),starty_e);
                
            }
            
            for (int i=1; i<timesteps_h; i++){
                
                //get e-field
                double efieldx_h = efieldmapx->GetBinContent(efieldmapx->GetXaxis()->FindBin(startx_h),efieldmapx->GetYaxis()->FindBin(starty_h));
                double efieldy_h = efieldmapy->GetBinContent(efieldmapy->GetXaxis()->FindBin(startx_h),efieldmapy->GetYaxis()->FindBin(starty_h));
                
                //get mobility
                double mu_h = vs_h/(Ec_h*pow(1+pow((sqrt(efieldx_h*efieldx_h+efieldy_h*efieldy_h)/Ec_h),beta_h),(1/beta_h)));
                
                //drift velocity
                double vx_h = mu_h*efieldx_h;   //[cm/s]
                double vy_h = mu_h*efieldy_h;  //[cm/s]

                startx_h+=vx_h*pow(10.,-9)/granularity*pow(10,4);
                starty_h+=vy_h*pow(10.,-9)/granularity*pow(10,4);
                
                voxelX_h->SetBinContent(voxelX_h->GetXaxis()->FindBin(x),voxelX_h->GetYaxis()->FindBin(y),voxelX_h->GetZaxis()->FindBin(i/granularity),startx_h);
                voxelY_h->SetBinContent(voxelY_h->GetXaxis()->FindBin(x),voxelY_h->GetYaxis()->FindBin(y),voxelY_h->GetZaxis()->FindBin(i/granularity),starty_h);
                
            }
            
        }
    }
    

//------------------------------------------------------------------------------------------
// Create Ramo Potential Map (from txt file)
//------------------------------------------------------------------------------------------

/*    
    TH2F* ramomap2D = new TH2F("ramo","Ramo Potential",1000,0.5,500.5,700,0.5,350.5);
    
    ifstream inRamo ("WP_pixel_total.txt");
    
    double xcoord, ycoord, ramo;
    while(!inRamo.eof()){
        inRamo >> xcoord;
        inRamo >> ycoord;
        inRamo >> ramo;
        
        ramomap2D->SetBinContent(ramomap2D->GetXaxis()->FindBin(xcoord),ramomap2D->GetYaxis()->FindBin(ycoord),ramo);
        
    }
*/    

//------------------------------------------------------------------------------------------
// Create Ramo Potential Map (from root file)
//------------------------------------------------------------------------------------------

    TFile * ramoMapsFile = new TFile(RamoPath);
    TH2F* ramomap2D = (TH2F*)ramoMapsFile->Get("ramo");

    
//------------------------------------------------------------------------------------------
// Create Map for "Unsmearing"
//------------------------------------------------------------------------------------------
    
    double trappingTimeElectrons = 1/(3e-16*fluence);
    int steps_e = 1000;
    int steps_h = 3000;
    
    TH2F* chargeChunkMap_e = new TH2F("avgCharge_e","Electron induced charge",500,0,250,100,0,50);
    TH2F* chargeChunkMap_h = new TH2F("avgCharge_h","Hole induced charge",500,0,250,100,0,50);
    
    for (double x = 0.25; x<=250.5; x+=0.5) {
        for (double y = 0.25; y<=50.5; y+=0.5) {
            double electricField = efieldmap->GetBinContent(efieldmap->GetXaxis()->FindBin(x),efieldmap->GetYaxis()->FindBin(y));
            double timeToElectrode_e = timemap_e->GetBinContent(timemap_e->GetXaxis()->FindBin(x),timemap_e->GetYaxis()->FindBin(y));
            double timeToElectrode_h = timemap_h->GetBinContent(timemap_h->GetXaxis()->FindBin(x),timemap_h->GetYaxis()->FindBin(y));
            if (electricField == 0 || timeToElectrode_e == 0 || timeToElectrode_h == 0) continue;
            double prob_e = 0;
            double prob_h = 0;
            double ramoInit = ramomap2D->GetBinContent(ramomap2D->GetXaxis()->FindBin(x+125),ramomap2D->GetYaxis()->FindBin(y+150));
            for (double t = 0; t<=timeToElectrode_e; t+=timeToElectrode_e/(steps_e)) {
                double xposTrapped_e = voxelX_e->GetBinContent(voxelX_e->GetXaxis()->FindBin(x),voxelX_e->GetYaxis()->FindBin(y),voxelX_e->GetZaxis()->FindBin(t));
                double yposTrapped_e = voxelY_e->GetBinContent(voxelY_e->GetXaxis()->FindBin(x),voxelY_e->GetYaxis()->FindBin(y),voxelY_e->GetZaxis()->FindBin(t));
                double ramoFinal_e = ramomap2D->GetBinContent(ramomap2D->GetXaxis()->FindBin(xposTrapped_e+125),ramomap2D->GetYaxis()->FindBin(yposTrapped_e+150));
                prob_e += (ramoFinal_e - ramoInit)*(timeToElectrode_e/steps_e)*exp(-t/trappingTimeElectrons)/trappingTimeElectrons;
            }
            for (double t = 0; t<=timeToElectrode_h; t+=timeToElectrode_h/(steps_h)) {
                double xposTrapped_h = voxelX_h->GetBinContent(voxelX_h->GetXaxis()->FindBin(x),voxelX_h->GetYaxis()->FindBin(y),voxelX_h->GetZaxis()->FindBin(t));
                double yposTrapped_h = voxelY_h->GetBinContent(voxelY_h->GetXaxis()->FindBin(x),voxelY_h->GetYaxis()->FindBin(y),voxelY_h->GetZaxis()->FindBin(t));
                double ramoFinal_h = ramomap2D->GetBinContent(ramomap2D->GetXaxis()->FindBin(xposTrapped_h+125),ramomap2D->GetYaxis()->FindBin(yposTrapped_h+150));
                prob_h += -(ramoFinal_h - ramoInit)*(timeToElectrode_h/steps_h)*exp(-t/trappingTimeElectrons)/trappingTimeElectrons;
            }
            chargeChunkMap_e->SetBinContent(chargeChunkMap_e->GetXaxis()->FindBin(x),chargeChunkMap_e->GetYaxis()->FindBin(y),prob_e);
            chargeChunkMap_h->SetBinContent(chargeChunkMap_h->GetXaxis()->FindBin(x),chargeChunkMap_h->GetYaxis()->FindBin(y),prob_h);
        }
    }
    
    
//------------------------------------------------------------------------------------------
// Store data in root histogram
//------------------------------------------------------------------------------------------
    
    TFile *outputfile = new TFile(Form("output/phi_%.0e_%.0fV_%.0fK.root", fluence, vbias, temp),"recreate");
    efieldmap->Write();
    timemap_e->Write();
    timemap_h->Write();
    voxelX_e->Write();
    voxelY_e->Write();
    voxelX_h->Write();
    voxelY_h->Write();
    ramomap2D->Write();
    chargeChunkMap_e->Write();
    chargeChunkMap_h->Write();
    outputfile->Close();
    
//------------------------------------------------------------------------------------------
// Visualize TCAD maps
//------------------------------------------------------------------------------------------
    
    
    if (visualizeMaps) {

        gStyle->SetOptStat(0);
        gStyle->SetPadLeftMargin(0.2);
        gStyle->SetPadRightMargin(0.25);
        gStyle->SetTitleFontSize(0.065);
        
        //Efield
        TCanvas *canvas_efield = new TCanvas("efield_canvas","efield_canvas",2000,400);
        efieldmap->GetXaxis()->SetNdivisions(505);
        efieldmap->GetYaxis()->SetNdivisions(505);
        efieldmap->GetXaxis()->SetTitle("x position [#mum]");
        efieldmap->GetXaxis()->SetLabelSize(0.06);
        efieldmap->GetXaxis()->SetTitleSize(0.06);
        efieldmap->GetYaxis()->SetTitle("y position [#mum]");
        efieldmap->GetYaxis()->SetLabelSize(0.06);
        efieldmap->GetYaxis()->SetTitleSize(0.06);
        efieldmap->GetZaxis()->SetTitle("E-field [V/cm]");
        efieldmap->GetZaxis()->SetLabelSize(0.06);
        efieldmap->GetZaxis()->SetTitleSize(0.06);
        efieldmap->GetXaxis()->SetTitleOffset(0.85);
        efieldmap->GetYaxis()->SetTitleOffset(0.4);
        efieldmap->GetZaxis()->SetTitleOffset(0.6);
        efieldmap->Draw("colz");
        canvas_efield->Print("efield.pdf");
        
        //Electron collection time
        //timemap_e->SetContour(100);
        TCanvas *canvas_e = new TCanvas("etimes_canvas","etimes_canvas",2000,400);
        timemap_e->GetXaxis()->SetNdivisions(505);
        timemap_e->GetYaxis()->SetNdivisions(505);
        timemap_e->GetXaxis()->SetTitle("x position [#mum]");
        timemap_e->GetXaxis()->SetLabelSize(0.052);
        timemap_e->GetXaxis()->SetTitleSize(0.052);
        timemap_e->GetYaxis()->SetTitle("y position [#mum]");
        timemap_e->GetYaxis()->SetLabelSize(0.052);
        timemap_e->GetYaxis()->SetTitleSize(0.052);
        timemap_e->GetZaxis()->SetTitle("Time [ns]");
        timemap_e->GetZaxis()->SetLabelSize(0.052);
        timemap_e->GetZaxis()->SetTitleSize(0.052);
        timemap_e->GetXaxis()->SetTitleOffset(0.85);
        timemap_e->GetYaxis()->SetTitleOffset(0.4);
        timemap_e->GetZaxis()->SetTitleOffset(0.5);
        gPad->SetFrameFillColor(kViolet+7);
        timemap_e->SetMaximum(1.5);
        timemap_e->Draw("colz");
        canvas_e->Print("time_canvas_e_1e16.pdf");
        
        //Hole collection time
        TCanvas *canvas_h = new TCanvas("htimes_canvas","htimes_canvas",2000,400);
        timemap_h->GetXaxis()->SetNdivisions(505);
        timemap_h->GetYaxis()->SetNdivisions(505);
        timemap_h->GetXaxis()->SetTitle("x position [#mum]");
        timemap_h->GetXaxis()->SetLabelSize(0.052);
        timemap_h->GetXaxis()->SetTitleSize(0.052);
        timemap_h->GetYaxis()->SetTitle("y position [#mum]");
        timemap_h->GetYaxis()->SetLabelSize(0.052);
        timemap_h->GetYaxis()->SetTitleSize(0.052);
        timemap_h->GetZaxis()->SetTitle("Time [ns]");
        timemap_h->GetZaxis()->SetLabelSize(0.052);
        timemap_h->GetZaxis()->SetTitleSize(0.052);
        timemap_h->GetXaxis()->SetTitleOffset(0.85);
        timemap_h->GetYaxis()->SetTitleOffset(0.4);
        timemap_h->GetZaxis()->SetTitleOffset(0.4);
        gPad->SetFrameFillColor(kViolet+7);
        timemap_h->Draw("colz");
        canvas_h->Print("time_canvas_h.png");
        
        //Charge position 4D plot
        /*gStyle->SetCanvasPreferGL(1);
        TCanvas *positioncanvasX_e = new TCanvas("positioncanvasX_e","positioncanvasX_e",750,500);
        voxelX_e->GetXaxis()->SetTitle("Initial x location [#mum]");
        voxelX_e->GetXaxis()->SetLabelSize(0.05);
        voxelX_e->GetXaxis()->SetTitleSize(0.05);
        voxelX_e->GetXaxis()->SetTitleOffset(2.0);
        voxelX_e->GetYaxis()->SetTitle("Initial y location [#mum]");
        voxelX_e->GetYaxis()->SetLabelSize(0.05);
        voxelX_e->GetYaxis()->SetTitleSize(0.05);
        voxelX_e->GetYaxis()->SetTitleOffset(2.2);
        voxelX_e->GetZaxis()->SetTitle("Drift time [ns]");
        voxelX_e->GetZaxis()->SetLabelSize(0.05);
        voxelX_e->GetZaxis()->SetTitleSize(0.05);
        voxelX_e->GetZaxis()->SetTitleOffset(1.6);
        voxelX_e->Draw("glcolz");
        //positioncanvasX_e->Print("positionmap_eX.png");
         
        TCanvas *positioncanvasY_e = new TCanvas("positioncanvasY_e","positioncanvasY_e",750,500);
        voxelY_e->GetXaxis()->SetTitle("Initial x location [#mum]");
        voxelY_e->GetXaxis()->SetLabelSize(0.05);
        voxelY_e->GetXaxis()->SetTitleSize(0.05);
        voxelY_e->GetXaxis()->SetTitleOffset(2.0);
        voxelY_e->GetYaxis()->SetTitle("Initial y location [#mum]");
        voxelY_e->GetYaxis()->SetLabelSize(0.05);
        voxelY_e->GetYaxis()->SetTitleSize(0.05);
        voxelY_e->GetYaxis()->SetTitleOffset(2.2);
        voxelY_e->GetZaxis()->SetTitle("Drift time [ns]");
        voxelY_e->GetZaxis()->SetLabelSize(0.05);
        voxelY_e->GetZaxis()->SetTitleSize(0.05);
        voxelY_e->GetZaxis()->SetTitleOffset(1.6);
        voxelY_e->Draw("glcolz");
        //positioncanvasY_e->Print("positionmap_eY.png");
         
        TCanvas *positioncanvasX_h = new TCanvas("positioncanvasX_h","positioncanvasX_h",750,500);
        voxelX_h->GetXaxis()->SetTitle("Initial x location [#mum]");
        voxelX_h->GetXaxis()->SetLabelSize(0.05);
        voxelX_h->GetXaxis()->SetTitleSize(0.05);
        voxelX_h->GetXaxis()->SetTitleOffset(2.0);
        voxelX_h->GetYaxis()->SetTitle("Initial y location [#mum]");
        voxelX_h->GetYaxis()->SetLabelSize(0.05);
        voxelX_h->GetYaxis()->SetTitleSize(0.05);
        voxelX_h->GetYaxis()->SetTitleOffset(2.2);
        voxelX_h->GetZaxis()->SetTitle("Drift time [ns]");
        voxelX_h->GetZaxis()->SetLabelSize(0.05);
        voxelX_h->GetZaxis()->SetTitleSize(0.05);
        voxelX_h->GetZaxis()->SetTitleOffset(1.6);
        voxelX_h->Draw("glcolz");
        //positioncanvasX_h->Print("positionmap_hX.png");
        
        TCanvas *positioncanvasY_h = new TCanvas("positioncanvasY_h","positioncanvasY_h",750,500);
        voxelY_h->GetXaxis()->SetTitle("Initial x location [#mum]");
        voxelY_h->GetXaxis()->SetLabelSize(0.05);
        voxelY_h->GetXaxis()->SetTitleSize(0.05);
        voxelY_h->GetXaxis()->SetTitleOffset(2.0);
        voxelY_h->GetYaxis()->SetTitle("Initial y location [#mum]");
        voxelY_h->GetYaxis()->SetLabelSize(0.05);
        voxelY_h->GetYaxis()->SetTitleSize(0.05);
        voxelY_h->GetYaxis()->SetTitleOffset(2.2);
        voxelY_h->GetZaxis()->SetTitle("Drift time [ns]");
        voxelY_h->GetZaxis()->SetLabelSize(0.05);
        voxelY_h->GetZaxis()->SetTitleSize(0.05);
        voxelY_h->GetZaxis()->SetTitleOffset(1.6);
        voxelY_h->Draw("glcolz");
        //positioncanvasY_h->Print("positionmap_hY.png");*/
        
        //Ramo potential
        TCanvas *canvas_ramo2D = new TCanvas("ramo2D_canvas","ramo2D_canvas",5000,3500);
        ramomap2D->GetXaxis()->SetNdivisions(505);
        ramomap2D->GetYaxis()->SetNdivisions(505);
        ramomap2D->GetXaxis()->SetTitle("x position [#mum]");
        ramomap2D->GetXaxis()->SetLabelSize(0.045);
        ramomap2D->GetXaxis()->SetTitleSize(0.045);
        ramomap2D->GetYaxis()->SetTitle("y position [#mum]");
        ramomap2D->GetYaxis()->SetLabelSize(0.045);
        ramomap2D->GetYaxis()->SetTitleSize(0.045);
        ramomap2D->GetZaxis()->SetTitle("Ramo potential");
        ramomap2D->GetZaxis()->SetLabelSize(0.045);
        ramomap2D->GetZaxis()->SetTitleSize(0.045);
        ramomap2D->GetZaxis()->SetTitleOffset(1.2);
        ramomap2D->SetContour(100);
        gStyle->SetPalette(58);
        gPad->SetFrameFillColor(1); //change color of zero-valued bins from white to black
        canvas_ramo2D->SetLogz(1); //plot in log scale to emphasize lower potentials
        ramomap2D->Draw("colz");
        canvas_ramo2D->Print("RamoPotential_log.pdf");

    }
    
}
