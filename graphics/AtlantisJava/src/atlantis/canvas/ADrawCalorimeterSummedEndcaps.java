package atlantis.canvas;

import java.awt.Color;

import atlantis.data.ACalorimeterData;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.utils.AMath;
import atlantis.utils.APolygon;

/**
 * Contains the function for drawing the summed LAr and HEC endcaps
 * 
 * @author Mark Stockton
 */
public class ADrawCalorimeterSummedEndcaps
{
    static int minEtaIndex,minPhiIndex,maxEtaIndex,maxPhiIndex,numCombinedCells,binvalue=1,SmallestEtaBinNumber,LargestEtaBinNumber;
    static double etaBin, phiBin, SmallestZMin,maxCellEnergy,minCellEnergy;
    static String name;
    
    protected static APar parameterStore = APar.instance();
    
    /**
     * Draws the summed endcaps. This method is called
     * from drawCalorimeters and should not be used directly.
     *
     * @param window AWindow window to use
     * @param ag AGraphics graphics object to draw onto
     * @param projection AProjection2D current projection
     * @param calorimeter ACalorimeterData current calorimeter
     * @param etaIndex short[] index of eta values
     * @param phiIndex short[] index of phi values
     * @param side byte[] side of detector 
     */
    public static void drawCalorimeterSummedEndcaps(AWindow window, AGraphics ag, AProjection2D projection, ACalorimeterData calorimeter, short[] etaIndex, short[] phiIndex, byte[] side)
    {
        resetGlobalValues(calorimeter.getName());
        ACoord data;
        double[][] EndcapBinnedEnergy = new double[(36/binvalue)][(64/binvalue)];
        calorimeter.makeDrawList();
        //loop over drawlist to find z of first plane and smallest and largest eta
        setSmallestAndLargest(calorimeter);
        //now loop over drawlist to draw
        for (int k = 0; k < calorimeter.getNumDraw(); k++)
        {
            int j = calorimeter.getDrawIndex(k);
            short detectorIndex = (short) ACalorimeterDetector.getDetectorIndex(calorimeter.getIdFromIndex(j));
            ACalorimeterDetector currentDetector=ACalorimeterDetector.get(detectorIndex);
            if (detectorIndex >= 0)
            {
                ACoord coord = currentDetector.getCell(projection, etaIndex[j], phiIndex[j], side[j]);
                if (coord.hv[0].length > 0)
                {
                    //has a cell so get its energy
                    double cellEnergy=calorimeter.getEnergy(j);
                    //find the eta min and max of each cell
                    double[] etaminmax=findEtaMinAndMax(currentDetector,etaIndex[j],side[j]);
                    //test to see if the cell overlaps the eta border
                    if(etaminmax[2]==1)    
                    {
                        //Only want cells upto the eta split
                        double middle=parameterStore.get("YX", "SplitEta").getD()-(0.5*etaBin);
                        //find the min and max of each cell
                        double[] minmax1=findMinAndMax(currentDetector,etaminmax[0],middle,phiIndex[j]);
                        //has to find out how many cells overlap new binning border
                        int totNumOfCells1 =findNumberOfCells(minmax1);
                        //Only want cells above the eta split
                        middle=parameterStore.get("YX", "SplitEta").getD()+(0.5*etaBin);
                        //find the min and max of each cell
                        double[] minmax2=findMinAndMax(currentDetector,middle,etaminmax[1],phiIndex[j]);
                        //has to find out how many cells overlap new binning border
                        int totNumOfCells2 =findNumberOfCells(minmax1);
                        //check found positive number of cells
                        int NumOfCellsTotal=0;
                        if(totNumOfCells1>0)
                            NumOfCellsTotal+=totNumOfCells1;
                        else
                            System.out.println("Found negative amount of cells!?!");
                        if(totNumOfCells2>0)
                            NumOfCellsTotal+=totNumOfCells2;
                        else
                            System.out.println("Found negative amount of cells!?!");
                        if(NumOfCellsTotal>0)
                        {
                            //split energy between the cells
                            cellEnergy/=NumOfCellsTotal;
                            //now add on energy
                            if(totNumOfCells1>0)
                            {
                                EndcapBinnedEnergy=AddOnToEnergy(EndcapBinnedEnergy, cellEnergy, minmax1);
                            }
                            if(totNumOfCells2>0)
                            {
                                EndcapBinnedEnergy=AddOnToEnergy(EndcapBinnedEnergy, cellEnergy, minmax2);
                            }
                        }
                    }
                    else
                    {
                        //find the min and max of each cell
                        double[] minmax=findMinAndMax(currentDetector,etaminmax[0],etaminmax[1],phiIndex[j]);
                        //has to find out how many cells overlap new binning border
                        int totNumOfCells =findNumberOfCells(minmax);
                        //if found cells
                        if(totNumOfCells>0)
                        {
                            //split energy between the cells
                            cellEnergy/=totNumOfCells;
                            //not overlapping eta split so just add on energy
                            EndcapBinnedEnergy=AddOnToEnergy(EndcapBinnedEnergy, cellEnergy, minmax);
                        }
                        else
                            System.out.println("Found negative amount of cells!?!");
                    }
                }
            }
        }
        //if no cells then don't need to draw 
        if(numCombinedCells==0)
            return;
        //now need to order energy into a list aswell as get the geometry of cells with energy
        double[] EndcapBinnedEnergyList;
        double[] YXBinnedEnergyList=new double[numCombinedCells];//needed as could be FR projection
        double[][][] YXhv=new double[2][numCombinedCells][0];
        int currentCount=0;
        for(int a=minEtaIndex; a<=maxEtaIndex;a++)
        {
            for(int b=minPhiIndex; b<=maxPhiIndex;b++)
            {
                if(EndcapBinnedEnergy[a][b]>0.0)
                {
                    //has an energy so store its cell coordinates
                    ACoord coord = calculateCellGeometry(a,b);
                    YXhv[0][currentCount]=coord.hv[0][0];
                    YXhv[1][currentCount]=coord.hv[1][0];
                    //sorts energy into a list less blank cells
                    YXBinnedEnergyList[currentCount]=EndcapBinnedEnergy[a][b];
                    currentCount++;
                    //check if is largest/smallest cell energy
                    if(EndcapBinnedEnergy[a][b]>maxCellEnergy)
                        maxCellEnergy=EndcapBinnedEnergy[a][b];
                    if(EndcapBinnedEnergy[a][b]<minCellEnergy)
                        minCellEnergy=EndcapBinnedEnergy[a][b];
                }
            }            
        }
        //store coords and scale for display
        ACoord YXData = new ACoord(YXhv);
        //now need to alter to FR projection if needed otherwise will continue with temp
        if(projection instanceof AProjectionFR)
        {
            //convert data to FR and add data to loop in phi
            data=convertToFR(YXData, window);
            //now need to loop the energies
            EndcapBinnedEnergyList=new double[3*numCombinedCells];
            for(int j=0; j<numCombinedCells;j++)
            {
                EndcapBinnedEnergyList[j]=YXBinnedEnergyList[j];
                EndcapBinnedEnergyList[j+numCombinedCells]=YXBinnedEnergyList[j];
                EndcapBinnedEnergyList[j+(2*numCombinedCells)]=YXBinnedEnergyList[j];
            }
            numCombinedCells*=3;//now have 3 lots of cells
        }
        else
        {
            //If not FR then the YX values are correct
            EndcapBinnedEnergyList=YXBinnedEnergyList;
            data=YXData;
        }
        data = window.calculateDisplay(projection.nonLinearTransform(data));
        // Draw the cell frames
        boolean drawCells = parameterStore.get( name, "Cells").getStatus();
        if (drawCells)
            drawCells(window, ag, data, EndcapBinnedEnergyList);
        // Draw the histograms.
        ACalorimeterData.drawEndcapHistogram(calorimeter, window, ag, projection);
    }

    /**
     * Resets gobal values
     * @param CalName name of calorimeter
     */
    protected static void resetGlobalValues(String CalName)
    {
        name=CalName;
        minEtaIndex=999;
        minPhiIndex=999;
        maxEtaIndex=0;
        maxPhiIndex=0;
        numCombinedCells=0;
        SmallestZMin = 99999.0;
        SmallestEtaBinNumber = 99999;
        LargestEtaBinNumber = 0;
        maxCellEnergy=0.0;
        minCellEnergy = 99999.0;
        //check if using 0.1 binning or 0.2 binning
        if(name.equals("HEC"))
            binvalue = parameterStore.get("YX", "HECBin").getI();
        else
            binvalue = parameterStore.get("YX", "LArBin").getI();
        if(binvalue==3)
        {
            etaBin=0.1;
            phiBin=(2*Math.PI/64);
            binvalue=1;
        }
        else
        {
            etaBin=0.1*binvalue;
            phiBin=(2*Math.PI/64)*binvalue;//(2*Math.PI/64)=0.098
        }
    }
    
    /**
     * Set the smallest Z, smallest and largest eta
     * 
     * @param calorimeter ACalorimeterData current calorimeter
     */
    protected static void setSmallestAndLargest(ACalorimeterData calorimeter)
    {
        double largestRho=0.0,smallestRho=99999.0;
        double largestEtaValue=0.0,smallestEtaValue=99999.0;
        //to shorten detectors to loop over
        short prevDetectorIndex=999;
        //loop over drawlist
        for (int k = 0; k < calorimeter.getNumDraw(); k++)
        {
            int j = calorimeter.getDrawIndex(k);
            short detectorIndex = (short) ACalorimeterDetector.getDetectorIndex(calorimeter.getIdFromIndex(j));
            ACalorimeterDetector currentDetector=ACalorimeterDetector.get(detectorIndex);
            String detectorName=currentDetector.getName();
            if (detectorIndex!=prevDetectorIndex && detectorIndex >= 0 
                    && (detectorName.indexOf("Endcap")>0 || detectorName.indexOf("EC")>=0 || detectorName.equals("HEC")))
            {
                //in a different detector
                prevDetectorIndex=detectorIndex;
                //find z of the first plane
                if(currentDetector.getZMin()<SmallestZMin)
                {
                    SmallestZMin=currentDetector.getZMin();
                }
                //find smallest rho
                if(currentDetector.getRMin()<smallestRho)
                {
                    smallestRho=currentDetector.getRMin();
                    largestEtaValue=AMath.etaAbs(currentDetector.getZMin(), smallestRho);
                }
                //find largest rho
                if(currentDetector.getRMax()>largestRho)
                {
                    largestRho=currentDetector.getRMax();
                    smallestEtaValue=AMath.etaAbs(currentDetector.getZMin(), largestRho);
                }
            }
        }
        //if drawing cell outlines then this will have already been done 
        if (splitBinning() && !parameterStore.getUnknown("Det", "CaloDetail").getStatus())
            ACalorimeterDetector.setEtaSplit();            
        //if using split binning and eta is in inner section
        if(splitBinning() && smallestEtaValue>=parameterStore.get("YX", "SplitEta").getD())
        {
            double belowSplit=parameterStore.get("YX", "SplitEta").getD()/0.1;
            double aboveSplit=(smallestEtaValue-parameterStore.get("YX", "SplitEta").getD())/0.2;
            SmallestEtaBinNumber=(int)Math.floor(belowSplit+aboveSplit);
        }
        else
        {
            SmallestEtaBinNumber=(int)Math.floor(smallestEtaValue/etaBin);
        }
        if(splitBinning() && largestEtaValue>=parameterStore.get("YX", "SplitEta").getD())
        {
            double belowSplit=parameterStore.get("YX", "SplitEta").getD()/0.1;
            double aboveSplit=(largestEtaValue-parameterStore.get("YX", "SplitEta").getD())/0.2;
            LargestEtaBinNumber=(int)Math.floor(belowSplit+aboveSplit);
        }
        else
        {
        LargestEtaBinNumber=(int)Math.floor(largestEtaValue/etaBin);
        }
    }
    
    /**
     * Test to see if using split binning
     * 
     * @return Status of if using split binning
     */
    protected static boolean splitBinning()
    {
        int test=0;
        if(name.equals("HEC"))
            test = parameterStore.get("YX", "HECBin").getI();
        else
            test = parameterStore.get("YX", "LArBin").getI();
        if(test==3)
            return true;
        else
            return false;
    }
    
    /**
     * Finds the max and min values of the hit cell
     * Also tests and sets the min and max indicies for eta and phi
     * 
     * @param currentDetector ACalorimeterDetector current detector 
     * @param etaIndex short index of eta values
     * @param side byte side of detector 
     * 
     * @return minAndMax[0] double minEta of cell
     * @return minAndMax[1] double maxEta of cell
     * @return minAndMax[2] double status of if is an overlapping cell
     */
    protected static double[] findEtaMinAndMax(ACalorimeterDetector currentDetector, short etaIndex, byte side)
    {
        double realEtaBin=etaBin;
        //if using split binning and eta is in outer section
        if(splitBinning() && Math.abs(currentDetector.getEtaMin(etaIndex, side))>=parameterStore.get("YX", "SplitEta").getD())
        {
            realEtaBin=0.2;
        }
        //used for x when taking arsinh(x)=ln(x+sqrt(x*x+1))
        double asinhOf;
        //have to alter eta so can be drawn at the front layer
        double zFactor = SmallestZMin/currentDetector.getZMin();
        //do for minimum of cell
        double calcEtaMin=Math.abs(currentDetector.getEtaMin(etaIndex, side));
        asinhOf = zFactor*Math.sinh(calcEtaMin);
        calcEtaMin = Math.log(asinhOf + Math.sqrt(asinhOf*asinhOf+1));
        //do for center of cell
        double calcEta=Math.abs(currentDetector.getEta(etaIndex, side));
        asinhOf = zFactor*Math.sinh(calcEta);
        calcEta= Math.log(asinhOf + Math.sqrt(asinhOf*asinhOf+1));
        //find cell size in eta and phi
        double deta=currentDetector.getDeltaEta();
        double minEta,maxEta;
        //check for overlaping cells in eta and phi
        if(deta>realEtaBin)
        {
            minEta=calcEtaMin;
            maxEta=minEta+deta;
        }
        else
        {
            minEta=calcEta;
            maxEta=minEta;//there is only 1 cell
        }    

        //make minEta in detector otherwise causes problems if SplitEta=SmallestEtaBinNumber*0.1
        if(minEta<SmallestEtaBinNumber*0.1)
        {
            minEta=SmallestEtaBinNumber*0.1;
        }
        
        //now store in variable to return
        double[] minAndMax=new double[3];
        minAndMax[0]= minEta;
        minAndMax[1]= maxEta;
        
        if(splitBinning())
        {
            if(minEta<parameterStore.get("YX", "SplitEta").getD() && 
                maxEta>parameterStore.get("YX", "SplitEta").getD())
            {
                minAndMax[2]=1;//overlapping cell
            }
            else
                minAndMax[2]=0;
        }
        else
            minAndMax[2]=0;
        return minAndMax;
    }
    
    /**
     * Finds the max and min values of the hit cell
     * Also tests and sets the min and max indicies for eta and phi
     * 
     * @param currentDetector ACalorimeterDetector current detector
     * @param minEta double min eta of cell to draw
     * @param maxEta double max eta of cell to draw 
     * @param phiIndex short index of phi values 
     * 
     * @return minAndMax[0] double minEta of cell
     * @return minAndMax[1] double maxEta of cell
     * @return minAndMax[2] double minPhi of cell
     * @return minAndMax[3] double maxPhi of cell
     */
    protected static double[] findMinAndMax(ACalorimeterDetector currentDetector, double minEta, double maxEta, short phiIndex)
    {
        double realEtaBin=etaBin, realPhiBin=phiBin;
        //if using split binning
        if(splitBinning())
        {
            //if eta is in inner section
            if(minEta>=parameterStore.get("YX", "SplitEta").getD())
            {
                realEtaBin=0.2;
                realPhiBin=(2*Math.PI/32);
            }
        }    
        //find cell size in eta and phi
        double deta=currentDetector.getDeltaEta();
        double dphi=currentDetector.getDeltaPhi();
        double minPhi,maxPhi;
        //check for overlaping cells in eta and phi
        if(deta>realEtaBin)
        {
            if(realEtaBin==etaBin)
            {
                minEta=Math.floor(minEta/realEtaBin);
                maxEta=Math.floor(maxEta/realEtaBin);
            }
            else
            {
                //inside split binning
                double belowSplit=parameterStore.get("YX", "SplitEta").getD()/0.1;
                double aboveSplit=(minEta-parameterStore.get("YX", "SplitEta").getD())/0.2;
                minEta=Math.floor(belowSplit+aboveSplit);
                aboveSplit=(maxEta-parameterStore.get("YX", "SplitEta").getD())/0.2;
                maxEta=Math.floor(belowSplit+aboveSplit);
            }
        }
        else
        {
            if(realEtaBin==etaBin)
            {
                minEta=Math.floor(minEta/realEtaBin);
            }
            else
            {
                //split binning
                double belowSplit=parameterStore.get("YX", "SplitEta").getD()/0.1;
                double aboveSplit=(minEta-parameterStore.get("YX", "SplitEta").getD())/0.2;
                minEta=Math.floor(belowSplit+aboveSplit);
            }
            maxEta=minEta;//there is only 1 cell
        }    
        if(dphi>realPhiBin)
        {
            minPhi=Math.abs(currentDetector.getPhiMin(phiIndex));
            minPhi=Math.floor(minPhi/realPhiBin);
            maxPhi=Math.floor(minPhi+(dphi/realPhiBin));
        }
        else
        {
            minPhi=Math.abs(currentDetector.getPhi(phiIndex));
            minPhi=Math.floor(minPhi/realPhiBin);
            maxPhi=minPhi;//there is only 1 cell
        }    
        //check for exceeding array limits
        if(maxPhi>=(64/binvalue))
        {
            maxPhi=(64/binvalue)-1;
            if(minPhi>=(64/binvalue)-1)
                minPhi=(64/binvalue)-1;
        }
        if(minPhi<0)
        {
            minPhi=0;
            if(maxPhi<0)
                maxPhi=0;
        }
        //check not out of eta array size range
        if(maxEta>=(36/binvalue))
        {
            maxEta=(36/binvalue)-1;
            if(minEta>=(36/binvalue))
                minEta=(36/binvalue)-1;
        }
        //check physical eta conditions 
        if(maxEta>LargestEtaBinNumber)
        {
            maxEta=LargestEtaBinNumber;
            if(minEta>LargestEtaBinNumber)
                minEta=LargestEtaBinNumber;
        }
        if(minEta<SmallestEtaBinNumber)
        {
            minEta=SmallestEtaBinNumber;
            if(maxEta<SmallestEtaBinNumber)
                maxEta=SmallestEtaBinNumber;
        }
        //find min and max eta and phi indices
        if(minEta<minEtaIndex)
            minEtaIndex=(int)minEta;
        if(maxEta>maxEtaIndex)
            maxEtaIndex=(int)maxEta;
        if(minPhi<minPhiIndex)
            minPhiIndex=(int)minPhi;
        if(maxPhi>maxPhiIndex)
            maxPhiIndex=(int)maxPhi;
        //now store in variable to return
        double[] minAndMax=new double[4];
        minAndMax[0]= minEta;
        minAndMax[1]= maxEta;
        minAndMax[2]= minPhi;
        minAndMax[3]= maxPhi;
        return minAndMax;
    }
       
    /**
     * Finds the number of cells in the new binning
     * 
     * @param minmax array containing: minEta, maxEta, minPhi, maxPhi
     */
    protected static int findNumberOfCells(double[] minmax)
    {
        int numOfEtaCells=0;
        int numOfPhiCells=0;
        double minEta=minmax[0];
        double maxEta=minmax[1];
        double minPhi=minmax[2];
        double maxPhi=minmax[3];
        //calculate the number of cells
        //add one as difference in index is 1 less than number of cells
        numOfEtaCells+=maxEta-minEta+1;
        numOfPhiCells+=maxPhi-minPhi+1;
        //get total number of cells 
        int totNumOfCells = numOfEtaCells*numOfPhiCells;
        return totNumOfCells;
    }
    
    /**
     * Adds on the energy to EndcapBinnedEnergy and then returns it
     * 
     *  @param EndcapBinnedEnergy double[][] energy listing of cells [eta][phi] 
     *  @param cellEnergy double energy of each new cell
     *  @param minmax double[] min and max of cell in eta and phi
     *  
     *  @return EndcapBinnedEnergy double[][] altered energy listing of cells [eta][phi] 
     */
    protected static double[][] AddOnToEnergy(double[][] EndcapBinnedEnergy, double cellEnergy, double[] minmax)
    {
        //now loop over the min to max for eta and phi
        for(int a= (int) minmax[0]; a<=(int) minmax[1]; a++)
        {
            for(int b=(int) minmax[2]; b<=(int) minmax[3]; b++)
            {
                //add to cell count if no energy already stored in cell 
                if(EndcapBinnedEnergy[a][b]==0.0)
                    numCombinedCells++;
                //add energy to cell
                EndcapBinnedEnergy[a][b]+= cellEnergy;
            }
        }
        return EndcapBinnedEnergy;
    }

    /**
     * Calculates the cell geometry from its eta and phi bin numbers
     * 
     * @param a int eta bin number
     * @param b int phi bin number
     * 
     * @return coord Acoord cell coords
     */
    protected static ACoord calculateCellGeometry(int a, int b)
    {
        double realEtaBin=etaBin, realPhiBin=phiBin;
        double eta=a*etaBin;
        double phi=b*phiBin;
        //if using split binning and eta is in inner section
        if(splitBinning() && eta>=parameterStore.get("YX", "SplitEta").getD())
        {
            realEtaBin=0.2;
            realPhiBin=(2*Math.PI/32);
            double belowSplit=parameterStore.get("YX", "SplitEta").getD()/0.1;
            double aboveSplit=a-belowSplit;
            eta=aboveSplit*0.2+belowSplit*0.1;
            phi=b*realPhiBin;
        }
        double[][] temphv = new double[2][];

        //all drawn on the front plane of the detector
        double newrMax=SmallestZMin/Math.sinh(eta);
        double newrMin=SmallestZMin/Math.sinh(eta+realEtaBin);
        //store the cell coords
        temphv[0] = new double[] {newrMax*Math.cos(phi), newrMax*Math.cos(phi+realPhiBin),
                              newrMin*Math.cos(phi+realPhiBin), newrMin*Math.cos(phi)};
        temphv[1] = new double[] {newrMax*Math.sin(phi), newrMax*Math.sin(phi+realPhiBin),
                              newrMin*Math.sin(phi+realPhiBin), newrMin*Math.sin(phi)};
        return new ACoord(temphv);
    }
    
    /**
     * Converts cells to FR using normal convertYXtoFR
     * Then adds on data above and below so loops in phi
     * 
     * @param YXData ACoord data in YX projection
     * @param window AWindow current window
     * 
     * @return coord Acoord cell coords
     */
    protected static ACoord convertToFR(ACoord YXData, AWindow window)
    {
        //convert to FR then have to loop data
        YXData.convertYXToFR();
        //new hv to store the values at + and - 2pi
        double[][][] FRhv=new double[2][3*numCombinedCells][YXData.hv[0][0].length];
        //get corners and then shift data to be inside corners
        double phiTop = window.getUserCorners()[0].y;
        double phiBottom = window.getUserCorners()[2].y;
        int factor=0;
        if(phiTop>720.0)
            //-2 as already draws upto 720 so doensn't need this extra 2*360
            factor=(int) Math.ceil(phiTop/360.0)-2;
        if(phiBottom<-360.0)
            //-1 as already draws down to -360 so doensn't need this extra -1*360
            factor=(int) Math.floor(phiBottom/360.0)-1;
        for(int j=0; j<numCombinedCells;j++)
            for(int k=0; k<YXData.hv[0][j].length; k++)
            {
                FRhv[1][j][k]=YXData.hv[1][j][k]+360.0*(factor-1);
                FRhv[1][numCombinedCells+j][k]=YXData.hv[1][j][k]+360.0*factor;
                FRhv[1][(2*numCombinedCells)+j][k]=YXData.hv[1][j][k]+360.0*(factor+1);
                FRhv[0][j][k]=YXData.hv[0][j][k];
                FRhv[0][numCombinedCells+j][k]=YXData.hv[0][j][k];
                FRhv[0][(2*numCombinedCells)+j][k]=YXData.hv[0][j][k];
            }
        //now store into data to be used to draw
        return new ACoord(FRhv);
    }
    
    /**
     * Draws the cells 
     * 
     * @param ag AGraphics graphics to draw on
     * @param data ACoord cell geometries
     * @param EndcapBinnedEnergyList double[] energy of each cell
     */
    protected static void drawCells(AWindow window, AGraphics ag, ACoord data, double[] EndcapBinnedEnergyList)
    {
        //Storage of parameters to be used later
        Color[] colorMap = AColorMap.getColors();
        //AParameter frame = parameterStore.get(name, "Frame");
        boolean drawFrame = parameterStore.get(name, "Frame").getStatus();
        int frameColor = parameterStore.get(name, "Frame").getI();
        //only draw frames for Grey/BW color maps if is selected to draw frames
        if(drawFrame && AColorMap.drawFrames())
            drawFrame=true;
        else
            drawFrame=false;
        AParameter cellGeometry = parameterStore.get(name, "CellGeometry");
        // Draw frames for the filled cells.
        if (drawFrame && cellGeometry.getStatus())
        {
            for (int j = 0; j < numCombinedCells; j++)
            {
                ag.setColor(colorMap[frameColor]);
                ag.drawPolygon(data.hv[0][j], data.hv[1][j], data.hv[0][j].length);
            }
        }
        // Draw the cell geometry.
        AParameter cellOutline = parameterStore.get(name, "CellOutline");
        // Draw filled cells.
        if (cellGeometry.getStatus())
        {
            for (int j = 0; j < numCombinedCells; j++)
            {
                ag.setColor(colorMap[cellGeometry.getI()]);
                ag.fillPolygon(data.hv[0][j], data.hv[1][j], data.hv[0][j].length);
            }
        }
        // Draw cell outlines.
        if (cellOutline.getStatus())
        {
            ag.updateDrawParameters(new ADrawParameters(true, cellOutline.getI(), 0, 1, 0, 0, false, 1, 0));
            for (int j = 0; j < numCombinedCells; ++j)
            {
                ag.drawPolygon(data.hv[0][j], data.hv[1][j], data.hv[0][j].length);
            }
        }
        //Scale the cell polygons.
        data = scaleEndcapPolygons(EndcapBinnedEnergyList, data, minCellEnergy, maxCellEnergy);
        //Get the colours of the cells
        byte[] EndcapColor= internalEndcapColor(EndcapBinnedEnergyList, minCellEnergy, maxCellEnergy);
        // And draw them.
        AParameter colorFunction = parameterStore.get(name, "ColorFunction");
        if (colorFunction.getI() == ACalorimeterData.COLOR_FUNC_ENERGY)
        {
            switch (AColorMap.getColorMap())
            {
                case AColorMap.COLOR_MAP_DEFAULT1:
                case AColorMap.COLOR_MAP_DEFAULT2:
                case AColorMap.COLOR_MAP_M4M5:
                case AColorMap.COLOR_MAP_GRAYDET:
                case AColorMap.COLOR_MAP_ORIGINAL:
                    // Use colors.
                    colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_HITCOL);
                    break;
                case AColorMap.COLOR_MAP_GRAY:
                case AColorMap.COLOR_MAP_BW:
                    // Use grayscale.
                    colorMap = AColorMap.getColors(AColorMap.COLOR_MAP_GRAY_HITCOL);
                    break;
            }
            //draw the legend
            if(ALegendWindow.exists())
            {
                if (AColorMap.getColorMap() != AColorMap.COLOR_MAP_BW)
                {
                    double[] colorEnergy = energyOfEndcapColor(minCellEnergy,maxCellEnergy);
                    ALegendWindow.getInstance().addEnergyText(window, colorEnergy,maxCellEnergy, colorMap);
                }
                else
                    ALegendWindow.getInstance().addEnergyText(window, minCellEnergy,maxCellEnergy);
            }
        }
        else if(ALegendWindow.exists())
            ALegendWindow.getInstance().clearText(window);
        //draw filled cell
        for (int j = 0; j < numCombinedCells; j++)
        {
            ag.setColor(colorMap[EndcapColor[j]]);
            ag.fillPolygon(data.hv[0][j],data.hv[1][j], data.hv[0][j].length);
        }
        //draw frame around cell
        if (drawFrame && !cellGeometry.getStatus())
        {
            for (int j = 0; j < numCombinedCells; j++)
            {
                ag.setColor(colorMap[frameColor]);
                ag.drawPolygon(data.hv[0][j], data.hv[1][j], data.hv[0][j].length);
            }
        }
    }
    
    /**
     * Scales a series of polygons according to the user settings.
     * Used for summing over layer in the Endcaps in the YX projection
     *
     * @param cellEnergy double[] list of each cells energy
     * @param coord ACoord polygons to be scaled
     * @param minEnergy double min Energy in a cell
     * @param maxEnergy double max Energy in a cell
     * 
     * @return ACoord scaled polygons
     */
    protected static ACoord scaleEndcapPolygons(double[] cellEnergy, ACoord coord, double minEnergy, double maxEnergy)
    {
        int energyMode = parameterStore.get(name, "EnergyMode").getI();

        for (int i = 0; i < cellEnergy.length; i++)
        {
           if (cellEnergy[i] == 0)
                continue;

            double factor;
            if (parameterStore.get(name, "ColorFunction").getI() != ACalorimeterData.COLOR_FUNC_ENERGY)
            {
                // The user has (implicitly) selected energy scaling.
                
                //each cell is scaled to the new 0.1*0.1 binning so 
                //density for each just depends on energy
                double density = cellEnergy[i];
                double minDensity = minEnergy;
                double maxDensity = maxEnergy;

                switch (energyMode)
                {
                    case ACalorimeterData.ENERGY_MODE_MAX_LIN:
                    case ACalorimeterData.ENERGY_MODE_SUM_LIN:
                        factor = Math.sqrt(density / maxDensity);
                        break;

                    case ACalorimeterData.ENERGY_MODE_MAX_LOG:
                    case ACalorimeterData.ENERGY_MODE_SUM_LOG:
                        double magnitude = Math.floor(Math.log10(Math.sqrt(maxDensity / minDensity)) + 1.0);
                        factor = (Math.log10(Math.sqrt(density / maxDensity)) + magnitude) / magnitude;
                        break;

                    case ACalorimeterData.ENERGY_MODE_MAX_SQRT:
                    case ACalorimeterData.ENERGY_MODE_SUM_SQRT:
                        factor = Math.sqrt(Math.sqrt(density / maxDensity));
                        break;

                    default:
                        factor = ACalorimeterData.getNonScalingFactor();
                }
            }
            else
            {
                // No energy scaling, scale all cells by the same factor.
                factor = ACalorimeterData.getNonScalingFactor();
            }

            if (factor <= 1.0)
            {
                APolygon.scale(coord.hv[0][i], coord.hv[1][i], factor);
            }
        }
        return coord;
    }

    /**
     * Determines the color for each hit according to the color function set by
     * the user. Used for summing over layer in the Endcaps in the YX projection
     *
     * @param cellEnergy double[] list of each cells energy
     * @param minEnergy double min Energy in a cell
     * @param maxEnergy double max Energy in a cell
     * 
     * @return byte[] colors for each energy 
     */
    protected static byte[] internalEndcapColor(double[] cellEnergy, double minEnergy, double maxEnergy)
    {
        byte[] EndcapColor= new byte[cellEnergy.length];
        
        int colorFunction = parameterStore.get(name, "ColorFunction").getI();
        int constantColor = parameterStore.get(name, "Constant").getI();
        switch (colorFunction)
        {
            case ACalorimeterData.COLOR_FUNC_CONSTANT:
                //colorByConstant();
                for(int i = 0; i < cellEnergy.length; i++)
                    EndcapColor[i] = (byte) constantColor;
                break;
/*            case COLOR_FUNC_SUBDET:
                colorBy(sub);
                break;

            case COLOR_FUNC_CLUSTER:
                colorBy(getClusters());
                break;

            case COLOR_FUNC_SAMPLING:
                colorBy(sampling);
                break;
*/
            case ACalorimeterData.COLOR_FUNC_ENERGY:
                EndcapColor=colorEndcapByEnergy( cellEnergy, minEnergy, maxEnergy);
                break;
/*
            case COLOR_FUNC_JET:
                colorBy(getJets());
                break;
*/
            default:
                for(int i = 0; i < cellEnergy.length; i++)
                    EndcapColor[i] = (byte) constantColor;
        }

        return EndcapColor;
    }
    
    /**
     * Color cells by energy.
     * Used for summing over layer in the Endcaps in the YX projection
     *
     * @param cellEnergy double[] list of each cells energy
     * @param minEnergy double min Energy in a cell
     * @param maxEnergy double max Energy in a cell
     * 
     * @return byte[] colors for each energy 
     */
    protected static byte[] colorEndcapByEnergy(double[] cellEnergy, double minEnergy, double maxEnergy)
    {
        byte[] EndcapColor= new byte[cellEnergy.length];
        int numColors = 17;
        for (int i = 0; i < cellEnergy.length; i++)
        {
            if (AColorMap.getColorMap() == AColorMap.COLOR_MAP_BW)
            {
                // Black and white colormap.
                EndcapColor[i] = AColorMap.BK;
            }
            else
            {
                // We have numColors colors available.
                switch (parameterStore.get(name, "EnergyMode").getI())
                {
                    case ACalorimeterData.ENERGY_MODE_MAX_LIN:
                    case ACalorimeterData.ENERGY_MODE_SUM_LIN:
                        EndcapColor[i] = (byte) (numColors * (cellEnergy[i] - minEnergy) / (maxEnergy - minEnergy));
                        break;
                    case ACalorimeterData.ENERGY_MODE_MAX_LOG:
                    case ACalorimeterData.ENERGY_MODE_SUM_LOG:
                        EndcapColor[i] = (byte) (numColors * (Math.log(cellEnergy[i]) - Math.log(minEnergy)) / (Math.log(maxEnergy) - Math.log(minEnergy)));
                        break;
                    case ACalorimeterData.ENERGY_MODE_MAX_SQRT:
                    case ACalorimeterData.ENERGY_MODE_SUM_SQRT:
                        EndcapColor[i] = (byte) (numColors * Math.sqrt((cellEnergy[i] - minEnergy) / (maxEnergy - minEnergy)));
                        break;
                }
                if (EndcapColor[i] >= numColors)
                    EndcapColor[i] = (byte) (numColors - 1);
            }
        }
        return EndcapColor;
    }
    
    /**
     * Energy of each color.
     * Used for summing over layer in the Endcaps in the YX projection
     *
     * @param minEnergy double min Energy in a cell
     * @param maxEnergy double max Energy in a cell
     * 
     * @return byte[] colors for each energy 
     */
    protected static double[] energyOfEndcapColor(double minEnergy, double maxEnergy)
    {
        int numColors = 17;
        double[] colorEnergy= new double[numColors];
        for (int i = 0; i < numColors; i++)
        {
            // We have numColors colors available.
            switch (parameterStore.get(name, "EnergyMode").getI())
            {
                case ACalorimeterData.ENERGY_MODE_MAX_LIN:
                case ACalorimeterData.ENERGY_MODE_SUM_LIN:
                    colorEnergy[i] = ((i * (maxEnergy - minEnergy))/numColors)+minEnergy;
                    break;
                case ACalorimeterData.ENERGY_MODE_MAX_LOG:
                case ACalorimeterData.ENERGY_MODE_SUM_LOG:
                    colorEnergy[i] = Math.exp(((i * (Math.log(maxEnergy) - Math.log(minEnergy)))/numColors)+Math.log(minEnergy));
                    break;
                case ACalorimeterData.ENERGY_MODE_MAX_SQRT:
                case ACalorimeterData.ENERGY_MODE_SUM_SQRT:
                    colorEnergy[i] = (Math.pow(i/numColors,2) * (maxEnergy - minEnergy))+minEnergy;
                    break;
            }
            //may need check to see if lower or higher than min or max
            if (colorEnergy[i] >= numColors)
                colorEnergy[i] = (byte) (numColors -1);
        }
        return colorEnergy;
    }

}