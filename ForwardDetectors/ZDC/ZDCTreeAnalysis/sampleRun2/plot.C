// 2015
zdcTree->Draw("zdc_ZdcEnergy[0]>>(100,0,10000)","L1_ZDC_A") // zoom in 0-100
zdcTree->Draw("zdc_ZdcEnergy[1]>>(100,0,10000)","L1_ZDC_C") // zoom in 0-100


// 2016 low
zdcTree->Draw("zdc_ZdcAmp[0]>>(100,0,1000)","L1_ZDC_A")     // zoom in 0-8000
zdcTree->Draw("zdc_ZdcAmp[1]>>(100,0,1000)","L1_ZDC_C")     // zoom in 0-500

// 2016 high
zdcTree->Draw("zdc_ZdcAmp[0]>>(100,0,1000)","L1_ZDC_A")     // log scale
zdcTree->Draw("zdc_ZdcAmp[1]>>(100,0,1000)","L1_ZDC_C")     // log scale
zdcTree->Draw("zdc_ZdcAmp[1]>>(100,0,1000)","L1_ZDC_C && (CountBits(zdc_ZdcModuleMask>>4)>1)")

// 2018
zdcTree->Draw("zdc_ZdcEnergy[0]>>(100,0,10000)","L1_ZDC_A") // log scale
zdcTree->Draw("zdc_ZdcEnergy[1]>>(100,0,10000)","L1_ZDC_C") // log scale
