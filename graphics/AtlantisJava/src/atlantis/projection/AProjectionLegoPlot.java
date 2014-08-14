package atlantis.projection;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import java.util.HashSet;
import java.util.Set;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.ACalorimeterData;
import atlantis.data.ADHelix;
import atlantis.event.AEvent;
import atlantis.data.ALegoData;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.graphics.ALegoDraw;
import atlantis.graphics.colormap.AColorMap;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.AMath;

public class AProjectionLegoPlot extends AProjection2D
{
    public static byte nLayers = 29;//also used in ALegoData
    //xz and yz are used to define the angle of the eta axis (altered in rotate interaction)
    private static double[] xz;
    private static double[] yz;
    //Variables needed by ALegoDraw and ALegoData that are updated here from changes to the gui value
    public static int defaultScale = parameterStore.get("LegoPlot", "Scale").getI();//set the axis scale (standard, log or sqrt)
    public static int nPhiCells = parameterStore.get("LegoPlot", "nphicells").getI();
    public static int nEtaCells = parameterStore.get("LegoPlot", "netacells").getI();
    public static Color[] defaultColorMap = AColorMap.getColors();
    public static Color[] caloColorMap = AColorMap.getColors();//to allow colouring by energy
    public static boolean reverse = false;//also used by ASynchroCursorsInteraction
    public static int mode = parameterStore.get("LegoPlot", "Mode").getI();
    public static int colorEM, colorHad;
    public static Set colorset = new HashSet();//not updated here
    public static final int minimumofLogScale=(int) Math.round(Math.log10(0.01));// we set 10MeV to lego height 0 on log scale
    protected final static String VIEW_DEFAULT = "View normal";
    protected final static String VIEW_FROM_ABOVE = "View from above";
    protected final static String VIEW_ET_ETA = "View Et v " + AMath.ETA;
    protected final static String VIEW_ET_PHI = "View Et v " + AMath.PHI;
    
    //to select th right drawing options for AOD object towers
    public final static int DRAW_MET = -21;
    public final static int DRAW_MUON = -2;
    public final static int DRAW_ELECTRON = -3;
    public final static int DRAW_PHOTON = -4;
    public final static int DRAW_CLUSTER = -5;
    public final static int DRAW_COMPOSITEPARTICLE = -6;
    public final static int DRAW_BJET = -7;
    public final static int DRAW_TAUJET = -8;
    public final static int DRAW_JET = -9;
    
    public AProjectionLegoPlot() {
        addPopupItem(VIEW_DEFAULT);
        addPopupItem(VIEW_FROM_ABOVE);
        addPopupItem(VIEW_ET_ETA);
        addPopupItem(VIEW_ET_PHI);
    }
    
    @Override
    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();
        int index=0;
        AWindow currentWindow = ACanvas.getCanvas().getCurrentWindow();
        index=currentWindow.getIndex();
        Point2D.Double[] corners = currentWindow.getUserCorners();
        double dPhi=0,dEta=0,currentxz=xz[index],currentyz=yz[index];

        if (action.equals(VIEW_DEFAULT))
        {
            xz[index]=0.6;
            yz[index]=0.5;
        }
        else if (action.equals(VIEW_FROM_ABOVE))
        {
            xz[index]=0.8;
            yz[index]=0.9;
        }
        else if (action.equals(VIEW_ET_PHI))
        {
            xz[index]=0.0;
            yz[index]=-50;
        }
        else if (action.equals(VIEW_ET_ETA))
        {
            xz[index]=360.0;
            yz[index]=-75;
        }
        dPhi=360*(xz[index]-currentxz);
        dEta=50*(currentyz-yz[index]);
        corners[0].x -= (dPhi);
        corners[0].y -= (dEta);
        corners[1].y -= (dEta);
        currentWindow.setUserCorners(corners);
        //reset the center of detector dot on ZMR
        // TODO: Check if this really is superfluous. I can't find any situation where it
        //       sets the interaction group to anything other than its old value. - Ben Waugh 2011-11-09
//        currentWindow.getInteractionManager().setContext(currentWindow.getInteractionToolBar().getSelectedGroup());
    }

    @Override
    public void setAspectRatio1(AWindow window)
    {
        AOutput.append("\nNot implemented for this projection\n", ALogInterface.WARNING);
    }
    
    public String getName()
    {
        return "LegoPlot";
    }
    
    public String getXLabel()
    {
        return AMath.PHI;
    }
    
    public String getYLabel()
    {
        return AMath.ETA;
    }
    
    public String getXUnits()
    {
        return AMath.DEGREES;
    }
    
    public String getYUnits()
    {
        return "";
    }
    
    public Point2D.Double getCenter()
    {
        /*in the lego plot the center changes if there is rotation
        however the getCenter is also needed in initialization
        which is before windows are indexed*/
        int index=0;
        double temp=0;
        //check to see if the windows are indexed 
        try
        {
            index=ACanvas.getCanvas().getCurrentWindow().getIndex();
        }
        catch(Throwable t)
        {
            temp=0.6;//hasn't indexed windows yet
        }
        if(temp==0)
            temp=xz[index];
        return new Point2D.Double(-360.0*temp,-5.0);
    }
    
    public Point2D.Double[] calculateNoZoomCorners(Dimension wSize)
    {
        if(xz==null)
        {
            xz=new double[ACanvas.getCanvas().getWindowsCount()];
            for(int i=0; i<ACanvas.getCanvas().getWindowsCount(); i++)
            {
                xz[i]=0.6;
            }
        }
        if(yz==null)
        {
            yz=new double[ACanvas.getCanvas().getWindowsCount()];
            for(int i=0; i<ACanvas.getCanvas().getWindowsCount(); i++)
            {
                yz[i]=0.5;
            }
        }
        if(ACanvas.getCanvas().getCurrentWindow()!=null)
        {
            int currentIndex =ACanvas.getCanvas().getCurrentWindow().getIndex();
            xz[currentIndex]=0.6;
            yz[currentIndex]=0.5;
        }        
        //called before have windows so xz=0.6 and yz=0.5
        Point2D.Double[] corners = new Point2D.Double[3];
        corners[0]=new Point2D.Double(-360.0*0.6,-5-0.5*50);
        corners[1]=new Point2D.Double(360.0, -5-0.5*50);
        corners[2]=new Point2D.Double(360.0, 5);
        //add on an offset
        corners[0].x-=25.0;
        corners[1].x+=25.0;
        corners[2].x+=25.0;
        corners[2].y+=3.0;
        return corners;
    }
    
    public ACoord getUserPoint(ADHelix dH, double s)
    {
        return nonLinearTransform(dH.getLEGOPoint(s));
    }
    
    @Override
    public void paint(AWindow window, Graphics g) 
    {
        // update parameters
        update();
        // Load the event
        AEvent event = eventManager.getCurrentEvent();
        if (event == null)
            return;
        // Create and fill histograms
        double[][][] lego = new double[nPhiCells][nEtaCells][nLayers];
        ALegoData.fillHistograms(event, lego);
        // Find the maximum energy
        double maxEt = 0.0, AODmaxEt = 0.0, LEGOmaxEt = 0.0;
        // loop over histogram data to scale towers and find maxEt
        LEGOmaxEt = ALegoData.findMaxEt(lego);
        maxEt=LEGOmaxEt;
        // loop over AOD data
        AODmaxEt = ALegoData.findAODMaxEt(event);
        //check if user wants to scale to AOD objects rather than cells
        //but only if the AOD max is greater than 0
        if(AODmaxEt==0)
        {
            if(!parameterStore.get("LegoPlot", "ScaleToAOD").isInitialized())
                parameterStore.get("LegoPlot", "ScaleToAOD").initialize();
            parameterStore.get("LegoPlot", "ScaleToAOD").getNameComponent().setEnabled(false);
        }
        else
        {
            if(!parameterStore.get("LegoPlot", "ScaleToAOD").isInitialized())
                parameterStore.get("LegoPlot", "ScaleToAOD").initialize();
            parameterStore.get("LegoPlot", "ScaleToAOD").getNameComponent().setEnabled(true);
        }
        if(AODmaxEt>0 && parameterStore.get("LegoPlot", "ScaleToAOD").getStatus())
        {
            maxEt=AODmaxEt;
            //user can no longer set the scale manually
            if(!parameterStore.get("LegoPlot", "ETAxisHeight").isInitialized())
                parameterStore.get("LegoPlot", "ETAxisHeight").initialize();
            parameterStore.get("LegoPlot", "ETAxisHeight").getNameComponent().setEnabled(false);
            parameterStore.get("LegoPlot", "ETAxisHeight").getValueComponent().setEnabled(false);
        }
        else
        {
            //user can now set the scale maually
            if(!parameterStore.get("LegoPlot", "ETAxisHeight").isInitialized())
                parameterStore.get("LegoPlot", "ETAxisHeight").initialize();
            parameterStore.get("LegoPlot", "ETAxisHeight").getNameComponent().setEnabled(true);
            parameterStore.get("LegoPlot", "ETAxisHeight").getValueComponent().setEnabled(true);
            //check if user has defined a scale
            if (parameterStore.get("LegoPlot", "ETAxisHeight").getD() > 0.0) 
            {
                // use the user defined scale
                maxEt = parameterStore.get("LegoPlot", "ETAxisHeight").getD();
            }
        }
        // Find the missing ET
        double met = ALegoData.findMissingEt(window,lego,event);
        // Begin drawing
        AGraphics ag = AGraphics.makeAGraphics(g);
        // Fill the background
        ALegoDraw.fillBackground(window, ag);
        // Draw the legend
        boolean drawLegend=false;
        if(!drawLegend && parameterStore.get("LegoPlot", "MainLegend").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "L1EtLegend").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "L1Items").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "L2Items").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "EFItems").getStatus())drawLegend=true;
        if(drawLegend)ALegoDraw.drawLegend(window,ag,event,met,maxEt,AODmaxEt);

        // Scale the values for the axis type
        maxEt=scaleMaxET(maxEt,AODmaxEt);
        AODmaxEt=ALegoData.scaleValue(AODmaxEt);
        met=ALegoData.scaleValue(met);
        if(parameterStore.get("LegoPlot","DrawPlot").getStatus())
        {
            // Draw the grid
            //System.out.println("maxEt = "+maxEt+" and LEGOmaxEt = "+LEGOmaxEt);
            ALegoDraw.drawGrid(window, ag, maxEt);
            // Draw the histograms
            ALegoData.drawHistograms(window, ag, event, lego, maxEt, met, AODmaxEt);
        }
    } // paintLego()
    
    /**
     * MaxEt is rounded to get the axis height as well as being scaled
     */
    private static double scaleMaxET(double maxEt, double AODmaxEt)
    {
        //for drawing if maxEt is 0 use AODmaxEt for scaling
        if (maxEt == 0)
            maxEt = AODmaxEt;
        maxEt=ALegoData.scaleValue(maxEt);
        //now round to get axis height
        if (defaultScale ==1 )
        {
            maxEt = Math.ceil(maxEt);
        }
        else if (maxEt > 10.0)
        {
            //set maximum to nearest 5 above maxEt
            maxEt = 5 * (Math.ceil(maxEt / 5.0));
        }
        else if (maxEt < 10.0)
        {
            maxEt = Math.ceil(maxEt);
        }
        return maxEt;
    }
    
    /** Shifts the real value of phi to the screen coord to account for the slope of the eta axis.
     *  For reverse of operation use -adjustPhi(window,-phi,eta)
     */
    public static double adjustPhi(AWindow window, double phi, double eta)
    {
        int index=window.getIndex();
        eta+=5;//scale to between 0 and 10
        eta/=10.0;//scale to between 0 and 1
        eta-=1;//scale to between -1 and 0
        eta*=(xz[index]*360.0);//fraction of full offset to take off
        phi+=eta;//take off the fraction
        return phi;//now adjusted from slant of eta axis
    }
    
    public static double getxz(int index)
    {
        return xz[index];
    }

    public static double getyz(int index)
    {
        return yz[index];
    }

    public static void setxz(int index, double xznew)
    {
        xz[index]=xznew;
    }
    
    public static void setyz(int index, double yznew)
    {
        yz[index]=yznew;
    }
    
    /**check if colouring by EM/HAD
     * 
     * @return boolean drawEMHAD 
     */
    public static boolean getDrawEMHAD()
    {
        boolean drawEMHAD=false;
        if (mode == 0 && parameterStore.get("LAr", "ColorFunction").getI()==6)
        {
            //Calorimeter Lego
            drawEMHAD=true;
        }
        else if (mode == 1 && parameterStore.get("LVL1TriggerTower", "ColorFunction").getI()==1)
        {
            //Trigger Tower Lego
            drawEMHAD=true;
        }
        return drawEMHAD;
    }
    
    public static void update()
    {
        defaultScale = parameterStore.get("LegoPlot", "Scale").getI();
        nPhiCells = parameterStore.get("LegoPlot", "nphicells").getI();
        nEtaCells = parameterStore.get("LegoPlot", "netacells").getI();
        defaultColorMap = AColorMap.getColors();
        caloColorMap = AColorMap.getColors();
        if (parameterStore.get("LAr", "ColorFunction").getI() == ACalorimeterData.COLOR_FUNC_ENERGY)
        {
            switch (AColorMap.getColorMap())
            {
                case AColorMap.COLOR_MAP_DEFAULT1:
                case AColorMap.COLOR_MAP_DEFAULT2:
                case AColorMap.COLOR_MAP_M4M5:
                case AColorMap.COLOR_MAP_GRAYDET:
                case AColorMap.COLOR_MAP_ORIGINAL:
                    // Use colors.
                    caloColorMap = AColorMap.getColors(AColorMap.COLOR_MAP_HITCOL);
                    break;
                case AColorMap.COLOR_MAP_GRAY:
                case AColorMap.COLOR_MAP_BW:
                    // Use grayscale.
                    caloColorMap = AColorMap.getColors(AColorMap.COLOR_MAP_GRAY_HITCOL);
                    break;
            }
        }
        else if (parameterStore.get("LAr", "ColorFunction").getI() == ACalorimeterData.COLOR_FUNC_TIME)
        {
            caloColorMap = AColorMap.getShades(nLayers+1);
        }
        reverse = parameterStore.get("LegoPlot", "Reverse").getStatus();
        mode = parameterStore.get("LegoPlot", "Mode").getI();
        //Use detector color but add on 12 to get different shade of same color
        colorEM = parameterStore.get("Det", "ECAL" + "Fill").getI() + 12;
        colorHad = parameterStore.get("Det", "HCAL" + "Fill").getI() + 12;
    }
}//end of LegoProjection class
